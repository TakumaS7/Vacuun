#define NOMINMAX
#include "videoPlayer.h"
#include <iostream>
#include "renderer.h"
#include <algorithm>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

VideoPlayer::VideoPlayer()
    :m_Width(SCREEN_WIDTH), m_Height(SCREEN_HEIGHT),m_startTime(0),m_endTime(0) {
 
    MFStartup(MF_VERSION);
}

VideoPlayer::~VideoPlayer() {
    MFShutdown();
}

bool VideoPlayer::LoadVideo(const wchar_t* filename, UINT64 loopStartMS, UINT64 loopEndMS)
{
    HRESULT hr;

    //Media Foundation の初期化
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        std::cerr << "MFStartup failed: " << std::hex << hr << std::endl;
        return false;
    }

    IMFAttributes* pAttributes = nullptr;
    hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr)) {
        std::cerr << "MFCreateAttributes failed: " << std::hex << hr << std::endl;
        return false;
    }

    //RGBフォーマットに変換
    hr = pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    if (FAILED(hr)) {
        std::cerr << "SetUINT32 failed: " << std::hex << hr << std::endl;
        return false;
    }

    //MP4ファイルを開く
    hr = MFCreateSourceReaderFromURL(filename, pAttributes, &m_pReader);
    if (FAILED(hr)) {
        std::cerr << "MFCreateSourceReaderFromURL failed: " << std::hex << hr << std::endl;
        return false;
    }

    pAttributes->Release();

    // **ここで `RGB32` に変換**
    IMFMediaType* pNewMediaType = nullptr;
    hr = MFCreateMediaType(&pNewMediaType);
    if (SUCCEEDED(hr)) {
        pNewMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        pNewMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
        hr = m_pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pNewMediaType);
        if (SUCCEEDED(hr)) {
            std::cerr << "Media type successfully set to RGB32" << std::endl;
        }
        else {
            std::cerr << "Failed to set media type to RGB32" << std::endl;
        }

        // **フォーマット確認**
        IMFMediaType* pMediaType = nullptr;
        hr = m_pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
        if (SUCCEEDED(hr)) {
            GUID subtype;
            pMediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
            if (subtype == MFVideoFormat_NV12) {
                std::cerr << "Video format is NV12 (YUV) → `RGB` に変換されていない！" << std::endl;
            }
            else if (subtype == MFVideoFormat_RGB32) {
                std::cerr << "Video format is RGB32 → 正常" << std::endl;
            }
            else {
                std::cerr << "Unknown video format" << std::endl;
            }
            pMediaType->Release();
        }

        hr = m_pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pNewMediaType);
        if (SUCCEEDED(hr)) {
            std::cerr << "Media type successfully set to RGB32" << std::endl;
        }
        else {
            std::cerr << "Failed to set media type to RGB32" << std::endl;
        }
        pNewMediaType->Release();
    }

    CreateTexture(m_Width, m_Height);
    CreateTextureView();
    return true;
}



bool VideoPlayer::CreateTexture(int width, int height)
{
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid texture size: width=" << width << ", height=" << height << std::endl;
        return false;
    }

    m_Width = width;
    m_Height = height;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit RGBA
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC; // CPUから更新可能
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // シェーダー用
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUで更新可能
    desc.MiscFlags = 0;

    HRESULT hr = GetDevice()->CreateTexture2D(&desc, nullptr, &m_pVideoTexture);
    if (FAILED(hr)) {
        std::cerr << "CreateTexture2D failed: " << std::hex << hr << std::endl;
        return false;
    }

    return true;
}



bool VideoPlayer::CreateTextureView()
{
    if (!m_pVideoTexture) {
        std::cerr << "Texture not created!" << std::endl;
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    HRESULT hr = GetDevice()->CreateShaderResourceView(m_pVideoTexture.Get(), &srvDesc, &m_pTextureView);
    if (FAILED(hr)) {
        std::cerr << "CreateShaderResourceView failed: " << std::hex << hr << std::endl;
        return false;
    }
    // **RGB32に強制設定**
    IMFMediaType* pNewMediaType = nullptr;
    hr = MFCreateMediaType(&pNewMediaType);
    if (SUCCEEDED(hr)) {
        pNewMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        pNewMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
        hr = m_pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pNewMediaType);
        if (SUCCEEDED(hr)) {
            std::cerr << "Media type successfully set to RGB32" << std::endl;
        }
        else {
            std::cerr << "Failed to set media type to RGB32" << std::endl;
        }
        pNewMediaType->Release();
    }
    return true;
}


void VideoPlayer::Update()
{
    DWORD streamIndex = 0;
    DWORD flags = 0;
    LONGLONG timestamp = 0;
    IMFSample* pSample = nullptr;

    HRESULT hr = m_pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &flags, &timestamp, &pSample);
    if (FAILED(hr)) {
        std::cerr << "ReadSample failed: " << std::hex << hr << std::endl;
        return;
    }

    if (pSample) {
        IMFMediaBuffer* pBuffer = nullptr;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        if (FAILED(hr)) {
            pSample->Release();
            return;
        }

        BYTE* pData = nullptr;
        DWORD maxLength = 0, currentLength = 0;
        hr = pBuffer->Lock(&pData, &maxLength, &currentLength);
        if (FAILED(hr)) {
            pBuffer->Release();
            pSample->Release();
            return;
        }

        int rowSize = m_Width * 4;
        int computedHeight = std::max(1, std::min(m_Height, static_cast<int>(currentLength / rowSize)));

        if (m_pVideoTexture) {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hrMap = GetDeviceContext()->Map(m_pVideoTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            if (SUCCEEDED(hrMap)) {
                BYTE* mappedData = static_cast<BYTE*>(mappedResource.pData);

                for (int y = 0; y < computedHeight; ++y) {
                    BYTE* srcRow = pData + y * rowSize;
                    BYTE* dstRow = mappedData + y * mappedResource.RowPitch;

                    for (int x = 0; x < m_Width; ++x) {
                        dstRow[x * 4 + 0] = srcRow[x * 4 + 2]; // R ← B
                        dstRow[x * 4 + 1] = srcRow[x * 4 + 1]; // G ← G
                        dstRow[x * 4 + 2] = srcRow[x * 4 + 0]; // B ← R
                        dstRow[x * 4 + 3] = srcRow[x * 4 + 3]; // A ← A
                    }
                }

                GetDeviceContext()->Unmap(m_pVideoTexture.Get(), 0);
            }
        }

        pBuffer->Unlock();
        pBuffer->Release();
        pSample->Release();
    }
}