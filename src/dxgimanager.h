#ifndef DXGIMANAGER_H
#define DXGIMANAGER_H

#include <QPixmap>

#ifdef Q_OS_WIN
# include <windows.h>
#  if defined (_WIN32_WINNT_WIN10)  //win10 dxgi1_6
#   include <dxgi1_6.h>
#   include <d3d11.h>
#  endif
#endif

class Texture
{
public:
    virtual ~Texture();
    virtual QPixmap copyToImage(IDXGIResource *res) = 0;

protected:
    ID3D11Texture2D *m_texture = nullptr;
};

class DxgiTextureStaging : public Texture
{
public:
    DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context);
    ~DxgiTextureStaging();

    QPixmap copyToImage(IDXGIResource *res);

private:
    ID3D11Device *m_device = nullptr;
    ID3D11DeviceContext * m_context = nullptr;
};

class DxgiManager
{
public:
    DxgiManager();
    ~DxgiManager();

    bool init();
    QString lastError() const;
    QPixmap grabScreen();

private:
    QString m_lastError = QString();
    Texture *m_texture = nullptr;
    IDXGIOutputDuplication *m_duplication = nullptr;
};

#endif // DXGIMANAGER_H
