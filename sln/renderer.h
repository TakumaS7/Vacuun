/*==============================================================================

   レンダリング管理[renderer.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"

enum BLENDSTATE
{
	BLENDSTATE_NONE = 0,		//ブレンド無し
	BLENDSTATE_ALPHA,			//αブレンド
	BLENDSTATE_ADD,				//加算
	BLENDSTATE_SUB,				//減算

	BLENDSTATE_MAX,
};

enum SHADER_TYPE {
	SHADER_DEFAULT = 0,    // 通常シェーダー (shader.hlsl)
	SHADER_VIDEO,          // 動画用シェーダー (video.hlsl)
	SHADER_VIDEO_CHROMAKEY,
	SHADER_MAX,
};

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
};


// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	float		Dummy[3];//16byte境界用
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( bool Enable );

void SetWorldViewProjection2D( void );
void SetWorldMatrix(XMMATRIX WorldMatrix );
void SetViewMatrix(XMMATRIX ViewMatrix );
void SetProjectionMatrix(XMMATRIX ProjectionMatrix );


void SetMaterial( MATERIAL Material );

void SetBlendState(BLENDSTATE blend);

// シェーダーのセット
void SetShader(SHADER_TYPE type);

void ZoomViewport(XMFLOAT3 pos, float zoomNum);

void ResetViewport();

XMFLOAT2 WorldToScreen(XMFLOAT3 worldPos);