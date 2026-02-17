#ifndef _VIDEOPLAYER_H_
#define _VIDEOPLAYER_H_

#include <d3d11.h>
#include <wrl/client.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <vector>

using Microsoft::WRL::ComPtr;

class VideoPlayer {
public:
  
    VideoPlayer();
    ~VideoPlayer();

    bool LoadVideo(const wchar_t* filename, UINT64 loopStartMS, UINT64 loopEndMS);
    
    void Update();
 
    ID3D11ShaderResourceView* GetTexture() const { return m_pTextureView.Get(); }

private:
    bool CreateTexture(int width, int height);
    bool CreateTextureView();
    

    ComPtr<ID3D11Texture2D> m_pVideoTexture = nullptr;
    ComPtr<ID3D11ShaderResourceView> m_pTextureView = nullptr;

    ComPtr<IMFSourceReader> m_pReader;
    UINT64 m_startTime, m_endTime;
    int m_Width, m_Height;
};

#endif
