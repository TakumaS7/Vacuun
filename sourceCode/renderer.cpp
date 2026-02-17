/*==============================================================================

   レンダリング管理 [renderer.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "renderer.h"



//*********************************************************
// 構造体
//*********************************************************


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*           g_D3DDevice = NULL;
ID3D11DeviceContext*    g_ImmediateContext = NULL;
IDXGISwapChain*         g_SwapChain = NULL;
ID3D11RenderTargetView* g_RenderTargetView = NULL;
ID3D11DepthStencilView* g_DepthStencilView = NULL;



ID3D11VertexShader*     g_VertexShader = NULL;
ID3D11PixelShader*      g_PixelShader = NULL;
ID3D11InputLayout*      g_VertexLayout = NULL;
ID3D11Buffer*			g_ConstantBuffer = NULL;
ID3D11Buffer*			g_MaterialBuffer = NULL;


XMMATRIX				g_WorldMatrix;
XMMATRIX				g_ViewMatrix;
XMMATRIX				g_ProjectionMatrix;

ID3D11DepthStencilState* g_DepthStateEnable;
ID3D11DepthStencilState* g_DepthStateDisable;

float bFactor[4] = { 0.0f,0.0f,0.0f,0.0f };
ID3D11BlendState* bState[BLENDSTATE_MAX];
static ID3D11VertexShader* g_pVideoVertexShader = nullptr;
static ID3D11PixelShader* g_pVideoPixelShader = nullptr;
static ID3D11PixelShader* g_pChromaKeyPixelShader = nullptr;
static D3D11_VIEWPORT g_OriginalViewport;

ID3D11Device* GetDevice( void )
{
	return g_D3DDevice;
}


ID3D11DeviceContext* GetDeviceContext( void )
{
	return g_ImmediateContext;
}


void SetDepthEnable( bool Enable )
{
	if( Enable )
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateEnable, NULL );
	else
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateDisable, NULL );

}

void SetWorldViewProjection2D( void )
{
	XMMATRIX worldViewProjection;

	worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	worldViewProjection = XMMatrixTranspose(worldViewProjection);

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, worldViewProjection);
	GetDeviceContext()->UpdateSubresource( g_ConstantBuffer, 0, NULL, &matrix, 0, 0 );
}


void SetWorldMatrix( XMMATRIX WorldMatrix )
{
	g_WorldMatrix = WorldMatrix;

	XMMATRIX worldViewProjection = g_WorldMatrix * g_ViewMatrix * g_ProjectionMatrix;
	worldViewProjection = XMMatrixTranspose(worldViewProjection);

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_ConstantBuffer, 0, NULL, &matrix, 0, 0);
}

void SetViewMatrix( XMMATRIX ViewMatrix )
{
	g_ViewMatrix = ViewMatrix;

	XMMATRIX worldViewProjection = g_WorldMatrix * g_ViewMatrix * g_ProjectionMatrix;
	worldViewProjection = XMMatrixTranspose(worldViewProjection);

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_ConstantBuffer, 0, NULL, &matrix, 0, 0);
}

void SetProjectionMatrix( XMMATRIX ProjectionMatrix )
{
	g_ProjectionMatrix = ProjectionMatrix;

	XMMATRIX worldViewProjection = g_WorldMatrix * g_ViewMatrix * g_ProjectionMatrix;
	worldViewProjection = XMMatrixTranspose(worldViewProjection);

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_ConstantBuffer, 0, NULL, &matrix, 0, 0);
}

// ワールド座標をスクリーン座標に変換
XMFLOAT2 WorldToScreen(XMFLOAT3 worldPos)
{
	XMFLOAT4 clipSpacePos;
	XMVECTOR pos = XMLoadFloat3(&worldPos);
	// ワールド座標をビュー空間へ変換
	pos = XMVector3TransformCoord(pos, g_ViewMatrix);
	// ビュー空間をスクリーン座標へ変換
	pos = XMVector3TransformCoord(pos, g_ProjectionMatrix);
	XMStoreFloat4(&clipSpacePos, pos);
	float screenX = ((clipSpacePos.x + 1.0f) * 0.5f) * SCREEN_WIDTH;
	float screenY = ((1.0f - clipSpacePos.y) * 0.5f) * SCREEN_HEIGHT;
	return { screenX, screenY };
}


void SetMaterial( MATERIAL Material )
{

	GetDeviceContext()->UpdateSubresource( g_MaterialBuffer, 0, NULL, &Material, 0, 0 );

}

void ZoomViewport(XMFLOAT3 pos, float zoomNum)
{
	
	// 現在のビューポートを保存
	UINT numViewports = 1;
	GetDeviceContext()->RSGetViewports(&numViewports, &g_OriginalViewport);
	// **ワールド座標をスクリーン座標に変換**
	XMFLOAT2 screenPos = WorldToScreen(pos);
	// 新しいビューポートを作成
	D3D11_VIEWPORT bossViewport = g_OriginalViewport;
	// **ビューポートを縮小（拡大）**
	bossViewport.Width = g_OriginalViewport.Width / zoomNum;
	bossViewport.Height = g_OriginalViewport.Height / zoomNum;
	// **ボスのスクリーン座標を中心にする**
	bossViewport.TopLeftX = -screenPos.x - 200;
	bossViewport.TopLeftY = -screenPos.y -25 ;
	
	// **ビューポートを適用**
	GetDeviceContext()->RSSetViewports(1, &bossViewport);


}

void ResetViewport()
{
	// 保存しておいた元のビューポートに戻す
	GetDeviceContext()->RSSetViewports(1, &g_OriginalViewport);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										0,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&g_SwapChain,
										&g_D3DDevice,
										&g_FeatureLevel,
										&g_ImmediateContext );
	if( FAILED( hr ) )
		return hr;


	// レンダーターゲットビュー生成、設定
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	g_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();



	//ステンシル用テクスチャー作成
	ID3D11Texture2D* depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags	= 0;
    td.MiscFlags		= 0;
	g_D3DDevice->CreateTexture2D( &td, NULL, &depthTexture );

	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	g_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &g_DepthStencilView );


	g_ImmediateContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );


	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports( 1, &vp );



	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof( rd ) );
	rd.FillMode = D3D11_FILL_SOLID; 
	rd.CullMode = D3D11_CULL_NONE; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 

	ID3D11RasterizerState *rs;
	g_D3DDevice->CreateRasterizerState( &rd, &rs );

	g_ImmediateContext->RSSetState( rs );




	// ブレンドステート設定
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//無効
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	g_D3DDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_NONE]);

	//αブレンド
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	g_D3DDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_ALPHA]);

	//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	g_D3DDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_ADD]);

	//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	g_D3DDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_SUB]);

	SetBlendState(BLENDSTATE_ALPHA);


	//float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	//ID3D11BlendState* blendState = NULL;
	//g_D3DDevice->CreateBlendState( &blendDesc, &blendState );
	//g_ImmediateContext->OMSetBlendState( blendState, blendFactor, 0xffffffff );


	// 深度ステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateEnable );//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateDisable );//深度無効ステート

	g_ImmediateContext->OMSetDepthStencilState( g_DepthStateEnable, NULL );




	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = NULL;
	g_D3DDevice->CreateSamplerState( &samplerDesc, &samplerState );

	g_ImmediateContext->PSSetSamplers( 0, 1, &samplerState );



	// 頂点シェーダコンパイル・生成
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	hr = D3DCompileFromFile( L"shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pErrorBlob);
	if( FAILED(hr) )
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShader );

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE( layout );

	g_D3DDevice->CreateInputLayout( layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayout );

	pVSBlob->Release();


	// ピクセルシェーダコンパイル・生成
	ID3DBlob* pPSBlob = NULL;
	hr = D3DCompileFromFile( L"shader.hlsl", NULL, NULL, "PixelShaderPolygon", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
	if( FAILED(hr) )
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}
	g_D3DDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_PixelShader );
	
	//動画用ピクセルシェーダコンパイル・生成
	hr = D3DCompileFromFile(L"video.hlsl", NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}
	hr = g_D3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pVideoPixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}

	hr = D3DCompileFromFile(L"chromaKey.hlsl", NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
	hr = g_D3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pChromaKeyPixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}
	pPSBlob->Release();


	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	g_D3DDevice->CreateBuffer( &hBufferDesc, NULL, &g_ConstantBuffer );
	g_ImmediateContext->VSSetConstantBuffers( 0, 1, &g_ConstantBuffer );


	hBufferDesc.ByteWidth = sizeof(MATERIAL);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	g_D3DDevice->CreateBuffer( &hBufferDesc, NULL, &g_MaterialBuffer );
	g_ImmediateContext->VSSetConstantBuffers( 1, 1, &g_MaterialBuffer );




	// 入力レイアウト設定
	g_ImmediateContext->IASetInputLayout( g_VertexLayout );

	// シェーダ設定
	g_ImmediateContext->VSSetShader( g_VertexShader, NULL, 0 );
	g_ImmediateContext->PSSetShader( g_PixelShader, NULL, 0 );
	

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitRenderer(void)
{
	// オブジェクト解放
	if( g_ConstantBuffer )		g_ConstantBuffer->Release();
	if( g_MaterialBuffer )		g_MaterialBuffer->Release();
	if( g_VertexLayout )		g_VertexLayout->Release();
	if( g_VertexShader )		g_VertexShader->Release();
	if( g_PixelShader )			g_PixelShader->Release();
	if (g_pVideoPixelShader)	g_pVideoPixelShader->Release();
	if( g_ImmediateContext )	g_ImmediateContext->ClearState();
	if( g_RenderTargetView )	g_RenderTargetView->Release();
	if( g_SwapChain )			g_SwapChain->Release();
	if( g_ImmediateContext )	g_ImmediateContext->Release();
	if( g_D3DDevice )			g_D3DDevice->Release();
}


//=============================================================================
// バックバッファクリア
//=============================================================================
void Clear(void)
{
	// バックバッファクリア
	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_ImmediateContext->ClearRenderTargetView( g_RenderTargetView, ClearColor );
	g_ImmediateContext->ClearDepthStencilView( g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


//=============================================================================
// プレゼント
//=============================================================================
void Present(void)
{
	g_SwapChain->Present( 0, 0 );
}

// テクスチャ描画前に呼び出すことでブレンド方式を変えるぞ
void SetBlendState(BLENDSTATE blend)
{
	g_ImmediateContext->OMSetBlendState(bState[blend], bFactor, 0xffffffff);
}

void SetShader(SHADER_TYPE type)
{
	GetDeviceContext()->VSSetShader(g_VertexShader, nullptr, 0);

	if (type == SHADER_VIDEO) {
		
		GetDeviceContext()->PSSetShader(g_pVideoPixelShader, nullptr, 0);
	}
	else if (type == SHADER_VIDEO_CHROMAKEY) {

		GetDeviceContext()->PSSetShader(g_pChromaKeyPixelShader, nullptr, 0);

	}
	else {
		
		GetDeviceContext()->PSSetShader(g_PixelShader, nullptr, 0);
	}
}

