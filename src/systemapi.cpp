#include "systemapi.h"

#include <QCursor>
#include <QGuiApplication>
#include <QPainter>
#include <QScreen>

#ifdef Q_OS_WIN
# include <windows.h>
# include <QtWin>
#  if defined (USE_D3D)
#   include <QtWin>
#   include <d3d9.h>
#   include <d3dx9tex.h>
#  endif
#endif

void SystemApi::mousePress(const QPointF &pos)
{
#ifdef Q_OS_WIN
    SetCursorPos(int(pos.x()), int(pos.y()));
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
#endif
}

void SystemApi::mouseRelease(const QPointF &pos)
{
#ifdef Q_OS_WIN
    SetCursorPos(int(pos.x()), int(pos.y()));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
#endif
}

void SystemApi::mouseMove(const QPointF &pos)
{
#ifdef Q_OS_WIN
    SetCursorPos(int(pos.x()), int(pos.y()));
    mouse_event(MOUSEEVENTF_MOVE, 0, 0, 0, 0);
#endif
}

QPixmap SystemApi::grabScreen()
{
    QPixmap screen;
#if defined (USE_D3D) && defined (Q_OS_WIN)
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
#elif defined (USE_GDI) && defined (Q_OS_WIN)
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
#elif defined (USE_DXGI) && defined (Q_OS_WIN) && defined (_WIN32_WINNT_WIN10)


#else
    screen = QGuiApplication::primaryScreen()->grabWindow(0);
#endif
    QPainter painter(&screen);
    painter.drawPixmap(QCursor::pos(), grabCursor());
    return screen;
}

QPixmap SystemApi::grabCursor()
{
    QPixmap cursorPixmap;
#ifdef Q_OS_WIN
    // Get Cursor Size
    int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
    int cursorHeight = GetSystemMetrics(SM_CYCURSOR);

    // Get your device contexts.
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Create the bitmap to use as a canvas.
    HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, cursorWidth, cursorHeight);

    // Select the bitmap into the device context.
    HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

    // Get information about the global cursor.
    CURSORINFO ci;
    ci.cbSize = sizeof(ci);
    GetCursorInfo(&ci);

    // Draw the cursor into the canvas.
    DrawIcon(hdcMem, 0, 0, ci.hCursor);

    // Convert to QPixmap
    cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapAlpha);

    // Clean up after yourself.
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmCanvas);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
#endif
    return cursorPixmap;
}
