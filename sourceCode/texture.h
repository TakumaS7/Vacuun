#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "main.h"
#include "renderer.h"

struct TextureForVideo {
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* shader_resource_view = nullptr;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    uint32_t xres = 0;
    uint32_t yres = 0;
    bool create(uint32_t xres, uint32_t yres, DXGI_FORMAT new_format, bool is_dynamic);
    void activate(int slot) const;
    void destroy();
    bool updateFromIYUV(const uint8_t* new_data, size_t data_size);
};

ID3D11ShaderResourceView* InitTexture(const wchar_t* textureName);
void UnInitTexture(ID3D11ShaderResourceView*);
int GetWidth(ID3D11ShaderResourceView* texture);
int GetHeight(ID3D11ShaderResourceView* texture);
#endif//_TEXTURE_H
