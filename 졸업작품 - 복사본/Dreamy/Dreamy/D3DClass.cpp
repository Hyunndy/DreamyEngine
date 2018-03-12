////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_rasterStateNoCulling = 0;
	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
	m_alphaBlendState2 = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}  


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 :
 - DirectX 11�� ��ü Direct3D ������ �Ͼ�� ��.
 
 - screenwidth, screenheight�� systemclass���� ���� �������� �ʺ�� �����̴�.
 - Direct3D�� �̸� �̿��Ͽ� ������ ũ���� ������ �ʱ�ȭ�ϰ� �̿��Ѵ�.
 - HWND������ ������� �����쿡 ���� �ڵ��̴�. Direct3D���� �� ������ �̿��Ͽ� ������� �����쿡 �����Ѵ�.
 - ScreenDepth������ screenNear������ �����쿡 �׷��� 3Dȯ�濡���� ����(depth)�� �̴�.
 - vsync������ Direct3D �������� ������� ���ΰ�ħ ����(fps)�� ���� ������, �ƴϸ� ������ �� ������ �ٽ� �׸������� �����Ѵ�.

 - D3D11_DEPTH_STENCIL_DESC ���� ���ٽ� ���� ������ ���� Z���۸� ���� �� �� �ְ� �Ѵ�.
 - D3D11_BLEND_DESC �� ���� ���¸� �����ϱ� ���� description
 -----------------------------------------------------------------------------------------------------------------------------------------------*/
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	D3D11_VIEWPORT viewport;

	D3D11_BLEND_DESC blendStateDescription;

	float fieldOfView, screenAspect;


	// vsync(��������ȭ) ������ �����Ѵ�.
	m_vsync_enabled = vsync;

	/*
	Direct3D�� �ʱ�ȭ�ϱ� ���� �׷���ī��/������� FPS(���ļ�)�� �˾ƾ� �Ѵ�.
	��ǻ�͸��� �� ���� ���ݾ� �ٸ��� ������ ��ǻ�Ϳ� �ش� ������ ����°��� �ʿ��ϴ�.
	�� ������ ����/�и��� �˾Ƴ� �� DirectX���� �߿� �� ���� �˷��־� ������ fps������ ����ϵ��� �Ѵ�.
	
	���� �� �۾��� �����ʰ� ���� �������� �ʴ� �⺻������ ����ٸ� DirectX�� buffer flip ��� blit�� ����Ͽ�
	ȭ�鿡 ǥ���ϰ� �Ǹ�, �̰��� ���ø����̼��� ������ ����߸���. ���� ����� ��¿� �Ž����� ���� �޼����� �����.
	*/
	
	// DirectX �׷��� �������̽� ���丮�� �����.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� ����͸� �����.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// ���(�����)�� ���� ù��° ����͸� �����Ѵ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ������ ���Ѵ�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// C������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����Ѵ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ���÷��� ��忡 ���� ����Ʈ ������ ä���ִ´�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã�´�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����Ѵ�.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// ���� �츮�� FPS�� ���ڿ� �и� ���� �Ǿ���.
	// ����͸� �̿��Ͽ� �������� �׷��� ī���� �̸��� ��밡���� ���� ī�� �޷θ��� ũ���̴�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ �����Ѵ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷���ī���� �̸��� CHAR�� ���ڿ� �迭�� �ٲ� �� �����Ѵ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// �� �������� �츰 ����� FPS�� ����/�и𰪰� �׷���ī���� ������ ���� �ֱ� ������ ������ ��� ���� ����� ����ü��
	// �������̽����� �ݳ��ؾ��Ѵ�.

	// ���÷��� ��� ����Ʈ�� �Ҵ��� �����Ѵ�.
	delete[] displayModeList;
	displayModeList = 0;

	// ��� ����͸� �Ҵ� �����Ѵ�.
	adapterOutput->Release();
	adapterOutput = 0;

	// ����͸� �Ҵ� �����Ѵ�.
	adapter->Release();
	adapter = 0;

	// ���丮 ��ü�� �Ҵ� �����մϴ�.
	factory->Release();
	factory = 0;

	/*

	���⼭ �츰 �ý��ۿ��� ���� FPS�� ���������Ƿ� DirectX�� �ʱ�ȭ�� ������ �� �ִ�.
	
	1. ���� ü���� description ����ü�� ä���ִ� ��
	- ���� ü���� ����Ʈ ���ۿ� ����۷� �̷�����ִ� �츮�� ������ �������� �ϸ� �������� ���̴�.
	- ���� ������� �ϳ��� ����۸��� ����Ͽ� �� ���� �׸� �� ����Ʈ ���ۿ� �ٲ�ġ�� �Ѵ�.(swap)
	- �̰��� ���� ü���̶�� �Ҹ��� ������.

	2. FPS
	- FPS�� �ʴ� ������ ����۸� �׷� ����Ʈ ���ۿ� �ٲ�ġ���ϴ����� ��Ÿ���� ����.
	- ���� Graphicsclass.h�� vsync������ true�� �����Ǿ� �ִٸ� �ý��ۿ��� ���� fps�� �����ȴ�. (60�츣��)
	- �ݸ鿡 vsync ������ false��� ���α׷��� �ִ��� ���� ȭ���� �׷������� �Ұ��̴�. (������ �̰��� �ܻ��� �����.)

	*/

	// ����ü���� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// �ϳ��� ����۸��� ����ϵ��� �Ѵ�.
	swapChainDesc.BufferCount = 1;

	// ������� �ʺ�� ���̸� �����Ѵ�.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// ����۷� �Ϲ����� 32bit�� �����̽��� �����Ѵ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� fps������ �����Ѵ�.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������� �뵵�� �����Ѵ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �̷���� �������� �ڵ��� �����Ѵ�.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø��� ����.
	// *��Ƽ���ø�: ���ø����̼��� �ӵ��� �ſ� ������������, �ַ� ��Ƽ�ٸ������ ��� ���� ���Ǵ� ���.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ������/Ǯ��ũ�� ��带 ����
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ����ü���� ��ĵ������ ���İ� ��ĵ���̴��� �������� ��������(unspecified) �����Ѵ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ������ ������� ������ ������.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʴ´�.
	swapChainDesc.Flags = 0;

	
	
	//���� ü�� description ����ü�� ä��� ���Ҿ� ���� �����̶�� ������ �����ؾ� �Ѵ�.
	// DirectX 11�� �����Ѵ�.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/*
	
	����ü�� description�� ���� ������ �����Ǿ� ���� ���� ü��, D3D ����̽�, �׸��� D3D ����̽� ���ؽ�Ʈ�� ���� �� �ִ�.
	Direct3D ����̽��� Direct3D ����̽� ���ؽ�Ʈ�� ��� Direct3D ����� �������̽��� �Ǳ� ������ �ſ� �߿��ϴ�.

	���� ���������� Direct3D ����̽��� ���������, ����̽� ���ؽ�Ʈ�� DirectX11���� ����Ѵ�.
	�⺻������ �� ���� ���� Direct3D ����̽��� ����� �� ���� �ɰ��ٰ� �����ϸ� �ȴ�. ���� �� ��ü�� ��� �ʿ��ϸ� ���ȴ�.

	���� ������ DirectX 11�� �����ϴ� �׷���ī�带 ���������ʴٸ� ����̽��� ����̽� ���ؽ�Ʈ�� �����ϴ� �Լ��� ������ ���̴�.
	���� �׽�Ʈ�� �ϰ������ �׷���ī�尡 �������� �ʴ� ��� �� �κп��� D3D_DRIVER_TYPE_HARDWARE�� D3D_DRIVER_TYPE_REFERENCE�� �ٲپ �������� CPU���� ó���� �� �ְ� �� �� �ִ�.
	�ӵ��� 1/1000���� ������.

	������ ù��° �׷���ī�尡 DirectX11�� ȣȯ���� �ʴ� ��� �����ϴ� ��찡 �ִ�.
	ù��° �׷���ī�尡 DirectX10 ������ �����ϰ� �ι�° �׷���ī�忡�� DirectX11�� ������ �� �ֽ��ϴ�.
	���� �Ϻ� ���̺긮�� �׷���ī���� ��� ù��°�� ������ ���� ī���, �ι�°�� ������ī��� ������ �� �ִ�.
	
	�̷� ��쿡 ��� �����ϱ� ���ؼ� ����� �⺻ �׷���ī��� �������� �ʰ� ��� ���� ī����� �����Ͽ� ���� �߸´� �׷��� ī��� ������ �� �ֵ��� �ؾ��Ѵ�.
	*/

	// ���� ü��, Direct3D ����̽�, Direct3D ����̽� ���ؽ�Ʈ�� �����Ѵ�.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ���� ü���� �������Ƿ� �츮�� ����۸� ����� �� �����͸� ���� ü�ο� ������� �־�� �Ѵ�.
	// �̸� ���� CreateRenderTargetView �Լ��� ����Ͽ� ����۸� ���� ü�ο� �����Ѵ�.
	// ������� �����͸� �����´�.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// ������� �����ͷ� ����Ÿ�� �並 �����Ѵ�.
	// �����-����ü�� ����
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// ����� �����͸� ���̻� ������� �����Ƿ� �Ҵ� �����Ѵ�.
	backBufferPtr->Release();
	backBufferPtr = 0;

	/*
	- ���� ������ description ����ü�� �ۼ��ؾ� �Ѵ�.
	- �̸� �̿��� ���� ���۸� ������ 3D�������� �츮�� ��������� �ùٸ��� �׷�����.
	
	- ���� ���ÿ� ���ٽ� ���۵� �� ���� ���ۿ� ������ ���̴�.
	- ���ٽ� ���۴� ��� ��, �����ִ� �׸��� ���� ȿ���� ��Ÿ�� �� ���ȴ�.

	- ����/���ٽ� ���۴� CreateTexture2D�� �����.
	- ���۴� ���� 2D�ؽ��Ŀ� ������ �ʴ´�.
	- �� ������ ��������� ���ĵǰ� ������ȭ�� ���Ŀ��� ��¶�� 2D��ǥ�� �ȼ����� �Ǳ� �����̴�.
	- �׸��� �� 2D���۴� ȭ�鿡 �׷�����.
	*/

	// ���� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ������ description�� �ۼ��Ѵ�.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// description�� ����Ͽ� ���� ������ �ؽ��ĸ� �����Ѵ�.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/* 
	- ���� ����/���ٽ� ������ description�� �ۼ��ϴ°��̴�.
	- �� ������ �츮�� Direct3D���� �� �ȼ��� � ���� �׽�Ʈ�� �Ұ����� ���� �� �ְ� ���ش�.
	- ���� �׽�Ʈ: Ư�� �ȼ��� ��ġ���� �����ϴ� �ȼ��� ���̸� ���ϴ� ����.
	*/

	// ���ٽ� ������ description�� �ʱ�ȭ �Ѵ�.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ������ description�� �ۼ��Ѵ�.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� �ۼ��� description�� ���� ����-���ٽ� ���� ������ �����.
	// ����-���ٽ� ���¸� �����Ѵ�.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// ����̽� ���ؽ�Ʈ�� ����� �̰��� �����Ѵ�.
	// ����-���ٽ� ���¸� �����Ѵ�.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	/*
	- �������δ� ����-���ٽ� ������ �信 ���� description�� ����� ���̴�.
	- �� �۾��� �ؾ� Direct3D���� ���� ���۸� ����-���ٽ� �ؽ��ķ� �ν��Ѵ�.
	- �� ����ü�� ä��� �� �Ŀ� CreateDepthStencilView �Լ��� ȣ���Ͽ� �����Ѵ�.
	*/

	// ����-���ٽ� ���� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ����-���ٽ� ���� description�� �ۼ��Ѵ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ����-���ٽ� �並 �����Ѵ�.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	/*
	- ���� OMSetRenderTarget�Լ��� ȣ���� �� �ִ�.
	- �� �Լ��� ����Ÿ�ٺ�� ����-���ٽ� �並 ��� ������ ���������ο� ���ε���Ų��.

	- �̷��� �Ͽ� ������������ �̿��� �������� ����� �� �츮�� ������� ����ۿ� �׷����� �ȴ�.
	- �׸��� ����ۿ� �׷������� ����Ʈ ���ۿ� �ٲ�ġ���Ͽ� ������ ����Ϳ� ����ϰ� �ȴ�.

	- ���� Ÿ���� ������ �� �߰����� ����� ������ �� �ִ�.

	1. ������ȭ�����(rasterizer state)
	- ������ ��� �׷��������� ���� ��� ���� �� �ְ� �Ѵ�.
	- �̸� �̿��� �츮�� ȭ���� ���̾������� ���� �׸��ų� ������ �յ޸��� ��� �׸����� �� ���� �ִ�.
	- ���� ���̴�(Ȥ�� ���� ���̴�)�� ������ ���� Ư������ �ﰢ���� �ȼ��鿡 ���� ������ ����� �ȼ����̴��� �����ϱ������ ��θ� ���Ѵ�.
	*/

	// ����Ÿ�� ��� ����-���ٽ� ���۸� ���� ��� ���������ο� ���ε��Ѵ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// � ������ ��� �׸� ������ �����ϴ� ������ȭ�� description�� �ۼ��Ѵ�.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// �ۼ��� description���κ��� ������ȭ�� ���¸� �����Ѵ�.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// ������ȭ�� ���¸� �����Ѵ�.
	m_deviceContext->RSSetState(m_rasterState);

	// �ĸ� ���� ���� ������ȭ�� ����.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
	if (FAILED(result))
	{
		return false;
	}

	/* ����Ÿ�� �������� Ŭ������ ������ �� �ְ� ����Ʈ�� �־�� �Ѵ�.

	 - Ŭ���� : ȭ�鿡 ���𰡸� �׸��� ���� ���� ��ǥ�� DirectX�� �˷��ִ� ����
	 - ����Ʈ : â�� Ŭ���̾�Ʈ ����(������) ���� Ŭ���� ������ �׸��µ� ���Ǵ� ����

	 ����Ʈ ������ ������ ������ �Ϻ� �������� ������ ���� �ִ�.
	 ��ü�� Ŭ���� ���������� ���� ũ��� ���̰� �Ǽ� ������ ������ Ȯ��, ũ�� �߷��� ���̰� �ȴ�.
	*/
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����Ѵ�.
	m_deviceContext->RSSetViewports(1, &viewport);

	/*
	1. �������
	- ��������� 3D�� ȭ���� �ռ� ������� 2D ����Ʈ �������� ��ȯ�ϵ��� ���ִ� ���̴�.
	- �� ����� ���纻�� ���� ���̴����� ����� �� �ֵ��� �ؾ� �Ѵ�.

	2. ���� ���
	- �� ����� �츮�� ������Ʈ���� 3D������ ��ǥ�� ��ȯ�ϴ� �� ���ȴ�.
	- ���� 3�����󿡼��� ȸ��/�̵�/ũ�� ��ȯ���� ���ȴ�.
	- �� ����� ���纻�� ���� ���̴����� ����� �� �ֵ��� �ؾ� �Ѵ�.

	3. �����
	- �� ����� �츮�� ��� �����ִ°��� ����ϴ� �� �̿�ȴ�.
	- �ַ� ī�޶� ���� ���δ�.

	*/

	// ��������� �����Ѵ�.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D�������� ���� ���� ����� �����Ѵ�.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// ���� ����� ���� ��ķ� �ʱ�ȭ�Ѵ�.
	D3DXMatrixIdentity(&m_worldMatrix);

	// ���⿡ ���߿� �� ����� ������ �Ѵ�.

	// ���翵 ����� �����.
	// �� ����� 3D��ü�� �ƴ϶� UI�� ���� 2D�� ��ҵ��� �׸��� ���� ���ȴ�.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	/*
	- 2D �̹��� �������� ���� 2��° ����-���ٽ� ���� ������ description�� �ۼ��Ѵ�.
	- �ι�° ����-���ٽ� ���� ������ DepthEnable�� false�� ���õȴ�.(������ ������)
	*/

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//2��° ����-���ٽ� ���� ���� ����
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result)) { return false; }

	/*
	- Text�� 3D�𵨵��� �� ��췯���� �ϱ����� ���ĺ��� ���¸� �����.
	*/
	
	// Clear the blend state description
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	/*
	���� ���� ����� ����� ���ĺ��������� DESC�� �ۼ��Ѵ�.
	���⼱ ��� ������ ȥ���� �� ������ ���� ������ �����Ѵ�.

	DestBlend: �̹� �� �ڸ��� �׷��� �ִ� ��ǥ �ȼ��� ����
	���⿡ ����� ���� �Լ� : D3D11_BLEND_INV_SRC_ALPHA�� �ҽ� �ؽ�ó�� ���ĸ� ������Ű�°��̴�.
	1���� �ҽ� �ؽ�ó�� ���ĸ�ŭ ���ٴ� ��. ���� �ҽ��� ���İ��� 0.3�̸� ��ǥ�� ���Ĵ� 0.7�� ���� ��ǥ �ȼ��� 70%�� ����ϰ��Ѵ�.

	srcBlend: �ҽ� �ؽ�ó�� ������ ����ϴµ� ���ȴ�.
	src_alpha ����: �ؽ�ó�� ������ �ִ� ���� ���� �״�� ����Ѵ�.

	�ҽ��� ��ǥ���� ���� ���� �ȼ��� ������ ���ϰ� �ȴ�.
	
	*/
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE; // ������ true��
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; 
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// �� Discription�� ���� ���İ��� ������ ���� ���¸� �����.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(result)) { return false; }

	
	// ���İ��� ������� ���� ���¸� ����� ���� BlendEnable�� FALSE�� �����Ѵ�.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE; 

	// �ٲ� Discription�� ���� ���İ��� ���� ���� ���¸� �����Ѵ�.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result)) { return false; }


	// 2 �� ���� ȥ�� ���� ������ ����ϴ�. 
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// ������ ����Ͽ� ȥ�� ���¸� ����ϴ�.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaBlendState2);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Shutdown()
�뵵 :
 - Initialize�Լ����� ����ߴ� �����͵��� �����ϰ� �ݳ��ϴ� ���� �Ѵ�.

 - ������ ����&�ݳ� ���� ����ü���� ������ ���� �ٲٴ� �Լ��� ȣ���Ѵ�.
 - �� �۾��� ���� �ʴ´ٸ� Ǯ��ũ�� ���¿��� ���� ü���� ������ �Ͼ �� ��� ���ܰ� �߻��ϰ� �ȴ�.
 - �׷��� Directr3D�� �����ϱ� ������ ������ ���� ü���� ������ ���� �ٲ��־�� �Ѵ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::Shutdown()
{
	// �����ϱ����� �̷��� ������ ���� �ٲ��� ������ ����ü���� �Ҵ� ������ �� ���ܰ� �߻��Ѵ�.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}


	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}
	if (m_alphaBlendState2)
	{
		m_alphaBlendState2->Release();
		m_alphaBlendState2 = 0;
	}

	if (m_rasterStateNoCulling)
	{
		m_rasterStateNoCulling->Release();
		m_rasterStateNoCulling = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : BeginScene()
�뵵 :
 - ���ο� �� ������ ���� 3Dȭ���� �׸��� ������ �� ȣ��ȴ�.
 - ���۸� �� ������ �ʱ�ȭ �ϰ� �������� �̷�������� �غ��Ѵ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// ������� ������ ����ϴ�.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���۸� ����ϴ�.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : EndScene()
�뵵 :
 - ������� ����(������)�� �� ä�������� ȭ�鿡 ǥ���Ѵ�.
 - fps�� ������ų���� �ƴ��� �� �� �����-����Ʈ ���۸� swap��Ų��.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::EndScene()
{

	if (m_vsync_enabled)
	{
		// FPS ������ �����Ѵ�.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// ������ �� ������ ǥ���Ѵ�.
		m_swapChain->Present(0, 0);
	}

	return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Get()
�뵵 :
- GraphicsClass���� ����� �Լ���. 
- Direct3D����̽��� ����̽� ���ؽ�Ʈ�� �����͸� ��ȯ�Ѵ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}
ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Get() Matrix(��ĵ�)
�뵵 :
- GraphicsClass���� ����� �Լ���.
- ����, ����, ���翵 ����� ��ȯ�Ѵ�.
- ShaderClass�� ShaderParameter���� �� ��ĵ��� �ʿ��ϱ� ������ �̸� ������ �ս��� ����� �ʿ��ؼ� ����.
- �� ������� ����? : �� ����� CameraClass���� ���� �� �ִ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : GetVideoCardInfo()
�뵵 :
- �׷���ī���� �̸��� ��밡���� �޸��� ���� ��ȯ�Ѵ�.
- �׷��� ī���� �̸��� �޸��� ũ�⸦ �ƴ°��� ���� �ٸ� �������� ����� �ϴµ� ������ �� �� �ִ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : TurnZbufferOn/Off()
�뵵 :
- 2D�������� ���� Z���۸� �Ѱ� �������� �Ѵ�.
- Z���۸� �Ѵ� �� : ���� �ִ� ����-���ٽ� ���¸� ����Ѵ�.
- Z���۸� ���� �� : depthEnable�� false�� �����ߴ� �ι�° ����-���ٽ� ���¸� ����Ѵ�.

- �Ϲ������� 3D������ - Z���� OFF - 2D������ - Z���� ON�� �ܰ踦 ��ģ��.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : TurnAlphaBlendingOn/Off
�뵵 :
- Text�������� ���� ���ĺ��� ���¸� �Ѱ� �������� �Ѵ�.

- OMsetBlendState�Լ��� BlendingState������ ����� ���ĺ����� �Ѱ� �������� �Ѵ�.

- additive blend for cloud
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f; 
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
	
	return;
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::EnableSecondBlendState()
{
	float blendFactor[4];


	// ���� �μ��� �����մϴ�. 
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// ���� ������ �մϴ�. 
	m_deviceContext->OMSetBlendState(m_alphaBlendState2, blendFactor, 0xffffffff);

	return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : TurnCullingOn/off
�뵵 : �ĸ� �ø� : ���������ο��� ����ڰ� ���� �ʴ� ���ʸ�, �� �ĸ� �ﰢ���� ��� ����ϴ� �����̴�. 
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::TurnOnCulling()
{
	// Set the culling rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	return;
}

void D3DClass::TurnOffCulling()
{
	// Set the no back face culling rasterizer state.
	m_deviceContext->RSSetState(m_rasterStateNoCulling);

	return;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : GetDepthStencilView()
�뵵 :
- ���� ���ٽ� �信 ���� ������ �����Ѵ�.
- �� �ʿ� �ϳĸ� RTTClass���� RTT�ؽ�ó�� ����Ÿ������ ���� �� ����-���ٽ� �䰡 �ʿ��ϱ� ������ GRAPHICS.���� ��ġ����!
-----------------------------------------------------------------------------------------------------------------------------------------------*/
ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	return m_depthStencilView;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------
�̸� : SetBackBufferRenderTarget()
�뵵 :
- Ŭ������ �� ���۸� ���� ���� Ÿ������ �����Ѵ�.
- RTT�� ������ ���� Ÿ���� �ٽ� ����۷� ������ �� �� ȣ��ȴ�.
-----------------------------------------------------------------------------------------------------------------------------------------------*/
void D3DClass::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}
