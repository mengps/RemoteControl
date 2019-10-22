#include "dxgimanager.h"
#include <QDebug>

Texture::~Texture()
{
    m_texture->Release();
}

DxgiTextureStaging::DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context)
    : m_device(device), m_context(context)
{

}

DxgiTextureStaging::~DxgiTextureStaging()
{
    m_device->Release();
    m_context->Release();
}

QPixmap DxgiTextureStaging::copyToImage(IDXGIResource *res)
{
    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D *textrueRes = nullptr;
    HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&textrueRes));
    if (FAILED(hr)) {
       qDebug() << "Failed to ID3D11Texture2D result =" << hex << uint(hr);
       return QPixmap();
    }
    textrueRes->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = desc.Width;
    texDesc.Height = desc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.Format = desc.Format;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;
    m_device->CreateTexture2D(&texDesc, nullptr, &m_texture);
    m_context->CopyResource(m_texture, textrueRes);

    IDXGISurface1 *surface = nullptr;
    hr = m_texture->QueryInterface(__uuidof(IDXGISurface1), reinterpret_cast<void **>(&surface));
    if (FAILED(hr)) {
       qDebug() << "Failed to QueryInterface IDXGISurface1 ErrorCode =" << hex << uint(hr);
       return QPixmap();
    }

    DXGI_MAPPED_RECT map;
    surface->Map(&map, DXGI_MAP_READ);
    QPixmap pixmap = QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),
                                       int(desc.Width), int(desc.Height), QImage::Format_ARGB32));
    surface->Unmap();
    surface->Release();
    m_texture->Release();

    return pixmap;
}

DxgiManager::DxgiManager()
{

}

DxgiManager::~DxgiManager()
{
    m_duplication->Release();
}

bool DxgiManager::init()
{
    ID3D11Device *d3dDevice = nullptr;
    ID3D11DeviceContext *d3dContext = nullptr;
    D3D_FEATURE_LEVEL feat = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, &feat, &d3dContext);
    if (FAILED(hr)) {
        m_lastError = "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIDevice *dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIAdapter *dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to Get IDXGIAdapter ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIOutput *dxgiOutput = nullptr;
    QVector<IDXGIOutput *> outputs;
    for(uint i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
        outputs.push_back(dxgiOutput);
    }
    dxgiAdapter->Release();
    if (outputs.size() > 0) dxgiOutput = outputs.at(0);
    else {
        m_lastError = "Failed to IDXGIOutput is Empty!";
        return false;
    }

    IDXGIOutput6 *dxgiOutput6 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void**>(&dxgiOutput6));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    hr = dxgiOutput6->DuplicateOutput(d3dDevice, &m_duplication);
    dxgiOutput6->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    DXGI_OUTDUPL_DESC desc;
    m_duplication->GetDesc(&desc);
    m_texture = new DxgiTextureStaging(d3dDevice, d3dContext);
    if (desc.DesktopImageInSystemMemory) {
        qDebug() << "Dsc: CPU shared with GPU";
    } else {
        qDebug() << "Desc: CPU not shared with GPU";
    }

    return true;
}

QString DxgiManager::lastError() const
{
    return m_lastError;
}

QPixmap DxgiManager::grabScreen()
{
    IDXGIResource *desktopRes;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    m_duplication->ReleaseFrame();
    HRESULT hr = m_duplication->AcquireNextFrame(100, &frameInfo, &desktopRes);
    if (FAILED(hr)) {
        m_lastError = "Failed to AcquireNextFrame ErrorCode = " + QString::number(uint(hr), 16);
        return QPixmap();
    }

    return m_texture->copyToImage(desktopRes);
}
