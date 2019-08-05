#include "controlled.h"
#include "connection.h"
#include "remoteevent.h"
#include "socket.h"

#include <QBuffer>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#if defined (USE_GDI)
#include <QtWin>
#elif defined (USE_D3D)
#include <QtWin>
#include <d3d9.h>
#include <d3dx9tex.h>
#endif

Controlled::Controlled(QObject *parent)
    : QTcpServer (parent)
{
    listen(QHostAddress::Any, 43801);
    m_screenSocket = new Socket;
    QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    m_screenSocket->moveToThread(thread);
    thread->start();
}

Controlled::~Controlled()
{

}

void Controlled::processEvent(const RemoteEvent &ev)
{
    QRectF screenRect = qApp->primaryScreen()->geometry();
    QPointF localPos(ev.position().x() * screenRect.width(),
                     ev.position().y() * screenRect.height());

    auto clickFunc = [localPos](int clickCount)
    {
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
        for (int i = 0; i < clickCount; i++)
        {
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
    if (m_screenSocket)
    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
#if defined (USE_GDI) || defined (USE_D3D)
        grabScreen().save(&buffer, "jpg", -1);       
#else
        QPixmap pixmap = QGuiApplication::primaryScreen()->grabWindow(0);
        pixmap.save(&buffer, "jpg", -1);
#endif
        QMetaObject::invokeMethod(m_screenSocket, "writeToSocket", Q_ARG(QByteArray, buffer.data()));
    }
}

void Controlled::incomingConnection(qintptr socketDescriptor)
{
    if (!m_connection)
    {
        m_connection = new Connection(this);
        m_connection->setSocketDescriptor(socketDescriptor);
        connect(m_connection, &Connection::connected, this, &Controlled::connected);
        connect(m_connection, &Connection::disconnected, this, [this]()
        {
            m_connection->deleteLater();
            m_connection = nullptr;
            QMetaObject::invokeMethod(m_screenSocket, "finish");
            killTimer(m_timerId);
            m_timerId = 0;
            emit disconnected();
        });
        connect(m_connection, &Connection::hasEventData, this, [this](const RemoteEvent &event)
        {
            processEvent(event);
        });
        QMetaObject::invokeMethod(m_screenSocket, "setDestAddr",
                                  Q_ARG(QHostAddress, QHostAddress(m_connection->peerAddress())));

        if (!m_timerId)
            m_timerId = startTimer(std::chrono::milliseconds(40));
    }
}

QPixmap Controlled::grabScreen()
{
    QPixmap screen;
#ifdef USE_D3D
    static bool d3dCreated = false;
    static LPDIRECT3D9 lpD3D = nullptr;
    static LPDIRECT3DDEVICE9 lpDevice = nullptr;
    static D3DDISPLAYMODE ddm;

    if (!d3dCreated)
    {
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

    if (lpDevice)
    {
        LPDIRECT3DSURFACE9 surface;
        lpDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, nullptr);
        lpDevice->GetFrontBufferData(0, surface);
        LPD3DXBUFFER bufferedImage = nullptr;
        D3DXSaveSurfaceToFileInMemory(&bufferedImage, D3DXIFF_JPG, surface, nullptr, nullptr);
        screen.loadFromData((uchar *)bufferedImage->GetBufferPointer(), bufferedImage->GetBufferSize(), "JPG");
        surface->Release();
    }
#endif

#ifdef USE_GDI
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
