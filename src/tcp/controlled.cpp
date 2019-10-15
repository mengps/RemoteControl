#include "controlled.h"
#include "remoteevent.h"
#include "socket.h"

#include <QBuffer>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QThread>
#include <QTime>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_WIN
  #if defined (USE_GDI)
    #include <QtWin>
  #elif defined (USE_D3D)
  #include <QtWin>
    #include <d3d9.h>
    #include <d3dx9tex.h>
  #endif
#endif

Controlled::Controlled(QObject *parent)
    : QTcpServer (parent)
{
    listen(QHostAddress::Any, 43800);
}

Controlled::~Controlled()
{

}

void Controlled::finish()
{
    if (m_controlled)
        QMetaObject::invokeMethod(m_controlled, "abort");
}

void Controlled::processEvent(const RemoteEvent &ev)
{
    QRectF screenRect = qApp->primaryScreen()->geometry();
    QPointF localPos(ev.position().x() * screenRect.width(),
                     ev.position().y() * screenRect.height());

    auto clickFunc = [localPos](int clickCount) {
#ifdef Q_OS_WIN
        POINT point;
        point.x = int(localPos.x());
        point.y = int(localPos.y());

        //获取点击处的窗口句柄
        HWND pointWindow = WindowFromPoint(point);
        WINDOWINFO info;
        GetWindowInfo(pointWindow, &info);
        //uint border = info.cyWindowBorders;
        //uint captionY = (info.rcWindow.bottom - info.rcWindow.top) - (info.rcClient.bottom - info.rcClient.top) - border * 2;

        //设置到前台并激活
        SetForegroundWindow(pointWindow);
        SetActiveWindow(pointWindow);
        SetCursorPos(point.x, point.y);

        //发送点击事件，按下+弹起为点击一次
        for (int i = 0; i < clickCount; i++) {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
#endif
    };

    switch (ev.type())
    {
    case RemoteEvent::Click:
        clickFunc(1);
        break;

    case RemoteEvent::DoubleClick:
        clickFunc(2);
        break;

    case RemoteEvent::KeyInput:
        break;
    }
}

void Controlled::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (m_controlled) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
#if defined (USE_GDI) || defined (USE_D3D)
        grabScreen().save(&buffer, "jpg", -1);
#else
        QTime time = QTime::currentTime();
        QPixmap pixmap = QGuiApplication::primaryScreen()->grabWindow(0);
        pixmap.save(&buffer, "jpg", -1);
        qDebug() << QTime::currentTime().msecsTo(time);
#endif
        BlockHeader header = { SCREEN_TYPE, qint32(buffer.size()) };
        DataBlock data = { header, buffer.data() };
        QMetaObject::invokeMethod(m_controlled, "writeToSocket", Q_ARG(DataBlock, data));
    }
}

void Controlled::incomingConnection(qintptr socketDescriptor)
{
    if (!m_controlled)
    {
        QThread *thread = new QThread;
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        m_controlled = new Socket;
        connect(m_controlled, &Socket::stateChanged, this, [this](QAbstractSocket::SocketState socketState) {
            switch (socketState)
            {
            case QAbstractSocket::ConnectedState:
                emit connected();
                break;
            default:
                break;
            }
        });
        connect(m_controlled, &Socket::disconnected, this, [this]() {
            Socket *socket = m_controlled;
            m_controlled = nullptr;
            socket->deleteLater();
            killTimer(m_timerId);
            m_timerId = 0;
            emit disconnected();
        });
        connect(m_controlled, &Socket::hasEventData, this, [this](const RemoteEvent &event) {
            processEvent(event);
        });
        m_controlled->setSocketDescriptor(socketDescriptor);
        m_controlled->moveToThread(thread);
        thread->start();

        if (!m_timerId)
            m_timerId = startTimer(std::chrono::milliseconds(40));
    }
}

QPixmap Controlled::grabScreen()
{
    QPixmap screen;
#if defined (USE_D3D) && defined(Q_OS_WIN)
    static bool d3dCreated = false;
    static LPDIRECT3D9 lpD3D = nullptr;
    static LPDIRECT3DDEVICE9 lpDevice = nullptr;
    static D3DDISPLAYMODE ddm;

    if (!d3dCreated) {
        lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
        lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm);
        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        d3dpp.Windowed = true;
        d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        d3dpp.BackBufferFormat = ddm.Format;
        d3dpp.BackBufferHeight = ddm.Height;
        d3dpp.BackBufferWidth = ddm.Width;
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = GetDesktopWindow();
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

        lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(),
                            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpDevice);
        d3dCreated = true;
        qDebug() << ddm.Width << ddm.Height << lpDevice;
    }

    if (lpDevice) {
        LPDIRECT3DSURFACE9 surface;
        lpDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, nullptr);
        lpDevice->GetFrontBufferData(0, surface);
        LPD3DXBUFFER bufferedImage = nullptr;
        D3DXSaveSurfaceToFileInMemory(&bufferedImage, D3DXIFF_JPG, surface, nullptr, nullptr);
        screen.loadFromData(static_cast<uchar *>(bufferedImage->GetBufferPointer()), bufferedImage->GetBufferSize(), "JPG");
        bufferedImage->Release();
        surface->Release();
    }
#endif

#if defined (USE_GDI) && defined(Q_OS_WIN)
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = GetDesktopWindow();
    HDC display_dc = GetDC(nullptr);
    HDC bitmap_dc = CreateCompatibleDC(display_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(display_dc, width, height);
    HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

    // copy data
    HDC window_dc = GetDC(hwnd);
    BitBlt(bitmap_dc, 0, 0, width, height, window_dc, 0, 0, SRCCOPY | CAPTUREBLT);

    // clean up all but bitmap
    ReleaseDC(hwnd, window_dc);
    SelectObject(bitmap_dc, null_bitmap);
    DeleteDC(bitmap_dc);

    screen = QtWin::fromHBITMAP(bitmap);
    DeleteObject(bitmap);
    ReleaseDC(nullptr, display_dc);
#endif
    return screen;
}
