//////////////////////////////////////////////////////////////////////////////////
//// Filename: d3dclass.cpp
//////////////////////////////////////////////////////////////////////////////////
//#include "d3dclass.h"
//
//
//D3DClass::D3DClass()
//{
//	m_swapChain = 0;
//	m_device = 0;
//	m_deviceContext = 0;
//	m_renderTargetView = 0;
//	m_depthStencilBuffer = 0;
//	m_depthStencilState = 0;
//	m_depthDisabledStencilState = 0;
//	m_depthStencilView = 0;
//	m_rasterState = 0;
//	m_rasterStateNoCulling = 0;
//	m_alphaEnableBlendingState = 0;
//	m_alphaDisableBlendingState = 0;
//	m_alphaBlendState2 = 0;
//
//}
//
//
//D3DClass::D3DClass(const D3DClass& other)
//{
//}
//
//
//D3DClass::~D3DClass()
//{
//}  
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : Initialize()
//용도 :
// - DirectX 11의 전체 Direct3D 설정이 일어나는 곳.
// 
// - screenwidth, screenheight는 systemclass에서 만든 윈도우의 너비와 높이이다.
// - Direct3D는 이를 이용하여 동일한 크기의 영역을 초기화하고 이용한다.
// - HWND변수는 만들어진 윈도우에 대한 핸들이다. Direct3D에선 이 변수를 이용하여 만들어진 윈도우에 접근한다.
// - ScreenDepth변수와 screenNear변수는 윈도우에 그려질 3D환경에서의 깊이(depth)값 이다.
// - vsync변수는 Direct3D 렌더링을 모니터의 새로고침 비율(fps)에 맞출 것인지, 아니면 가능한 한 빠르게 다시 그릴것인지 지정한다.
//
// - D3D11_DEPTH_STENCIL_DESC 깊이 스텐실 상태 변수를 통해 Z버퍼를 끄고 켤 수 있게 한다.
// - D3D11_BLEND_DESC 두 블렌딩 상태를 생성하기 위한 description
// -----------------------------------------------------------------------------------------------------------------------------------------------*/
//bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
//	float screenDepth, float screenNear)
//{
//	HRESULT result;
//	IDXGIFactory* factory;
//	IDXGIAdapter* adapter;
//	IDXGIOutput* adapterOutput;
//	unsigned int numModes, i, numerator, denominator, stringLength;
//	DXGI_MODE_DESC* displayModeList;
//	DXGI_ADAPTER_DESC adapterDesc;
//	int error;
//	DXGI_SWAP_CHAIN_DESC swapChainDesc;
//	D3D_FEATURE_LEVEL featureLevel;
//	ID3D11Texture2D* backBufferPtr;
//	D3D11_TEXTURE2D_DESC depthBufferDesc;
//
//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
//	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
//	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
//	D3D11_RASTERIZER_DESC rasterDesc;
//
//	//D3D11_VIEWPORT viewport;
//
//	D3D11_BLEND_DESC blendStateDescription;
//
//	float fieldOfView, screenAspect;
//
//
//	// vsync(수직동기화) 설정을 저장한다.
//	m_vsync_enabled = vsync;
//
//	/*
//	Direct3D를 초기화하기 전에 그래픽카드/모니터의 FPS(주파수)를 알아야 한다.
//	컴퓨터마다 그 값이 조금씩 다르기 때문에 컴퓨터에 해당 정보를 물어보는것이 필요하다.
//	이 비율의 분자/분모값을 알아낸 뒤 DirectX설정 중에 그 값을 알려주어 적절한 fps비율을 계산하도록 한다.
//	
//	만일 이 작업을 하지않고 대충 지원되지 않는 기본값으로 맞춘다면 DirectX는 buffer flip 대신 blit을 사용하여
//	화면에 표시하게 되며, 이것은 어플리케이션의 성능을 떨어뜨린다. 또한 디버그 출력에 거슬리는 에러 메세지를 남긴다.
//	*/
//	
//	// DirectX 그래픽 인터페이스 팩토리를 만든다.
//	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 어답터를 만든다.
//	result = factory->EnumAdapters(0, &adapter);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 출력(모니터)에 대한 첫번째 어답터를 나열한다.
//	result = adapter->EnumOutputs(0, &adapterOutput);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드의 갯수를 구한다.
//	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// C가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성한다.
//	displayModeList = new DXGI_MODE_DESC[numModes];
//	if (!displayModeList)
//	{
//		return false;
//	}
//
//	// 디스플레이 모드에 대한 리스트 구조를 채워넣는다.
//	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 이제 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드를 찾는다.
//	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장한다.
//	for (i = 0; i<numModes; i++)
//	{
//		if (displayModeList[i].Width == (unsigned int)screenWidth)
//		{
//			if (displayModeList[i].Height == (unsigned int)screenHeight)
//			{
//				numerator = displayModeList[i].RefreshRate.Numerator;
//				denominator = displayModeList[i].RefreshRate.Denominator;
//			}
//		}
//	}
//
//	// 이제 우리는 FPS의 분자와 분모를 갖게 되었다.
//	// 어댑터를 이용하여 받을것은 그래픽 카드의 이름과 사용가능한 비디오 카드 메로리의 크기이다.
//	result = adapter->GetDesc(&adapterDesc);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장한다.
//	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
//
//	// 그래픽카드의 이름을 CHAR형 문자열 배열로 바꾼 뒤 저장한다.
//	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
//	if (error != 0)
//	{
//		return false;
//	}
//
//	// 이 시점에서 우린 저장된 FPS의 분자/분모값과 그래픽카드의 정보를 갖고 있기 때문에 정보를 얻기 위해 사용한 구조체와
//	// 인터페이스들을 반납해야한다.
//
//	// 디스플레이 모드 리스트의 할당을 해제한다.
//	delete[] displayModeList;
//	displayModeList = 0;
//
//	// 출력 어답터를 할당 해제한다.
//	adapterOutput->Release();
//	adapterOutput = 0;
//
//	// 어답터를 할당 해제한다.
//	adapter->Release();
//	adapter = 0;
//
//	// 팩토리 객체를 할당 해제합니다.
//	factory->Release();
//	factory = 0;
//
//	/*
//
//	여기서 우린 시스템에서 얻어온 FPS를 갖고있으므로 DirectX의 초기화를 시작할 수 있다.
//	
//	1. 스왑 체인의 description 구조체를 채워넣는 일
//	- 스왑 체인은 프론트 버퍼와 백버퍼로 이루어져있는 우리가 실제로 렌더링을 하면 쓰여지는 곳이다.
//	- 보통 사람들은 하나의 백버퍼만을 사용하여 그 위에 그린 뒤 프론트 버퍼와 바꿔치기 한다.(swap)
//	- 이것이 스왑 체인이라고 불리는 이유다.
//
//	2. FPS
//	- FPS는 초당 몇장의 백버퍼를 그려 프론트 버퍼와 바꿔치기하는지를 나타내는 숫자.
//	- 만약 Graphicsclass.h의 vsync변수가 true로 설정되어 있다면 시스템에서 정한 fps로 고정된다. (60헤르츠)
//	- 반면에 vsync 변수가 false라면 프로그램은 최대한 많이 화면을 그려내려고 할것이다. (하지만 이것은 잔상을 남긴다.)
//
//	*/
//
//	// 스왑체인의 description을 초기화한다.
//	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
//
//	// 하나의 백버퍼만을 사용하도록 한다.
//	swapChainDesc.BufferCount = 1;
//
//	// 백버퍼의 너비와 높이를 설정한다.
//	swapChainDesc.BufferDesc.Width = screenWidth;
//	swapChainDesc.BufferDesc.Height = screenHeight;
//
//	// 백버퍼로 일반적인 32bit의 서페이스를 지정한다.
//	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//
//	// 백버퍼의 fps비율을 설정한다.
//	if (m_vsync_enabled)
//	{
//		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
//		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
//	}
//	else
//	{
//		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
//		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
//	}
//
//	// 백버퍼의 용도를 설정한다.
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//
//	// 렌더링이 이루어질 윈도우의 핸들을 설정한다.
//	swapChainDesc.OutputWindow = hwnd;
//
//	// 멀티샘플링을 끈다.
//	// *멀티샘플링: 어플리케이션의 속도는 매우 느리게하지만, 주로 안티앨리어싱을 잡기 위해 사용되는 기술.
//	swapChainDesc.SampleDesc.Count = 1;
//	swapChainDesc.SampleDesc.Quality = 0;
//
//	// 윈도우/풀스크린 모드를 설정
//	if (fullscreen)
//	{
//		swapChainDesc.Windowed = false;
//	}
//	else
//	{
//		swapChainDesc.Windowed = true;
//	}
//
//	// 스왑체인의 스캔라인의 정렬과 스캔라이닝을 지정되지 않음으로(unspecified) 설정한다.
//	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//	// 출력된 이후의 백버퍼의 내용을 버린다.
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//
//	// 추가 옵션 플래그를 사용하지 않는다.
//	swapChainDesc.Flags = 0;
//
//	
//	
//	//스왑 체인 description 구조체를 채움과 더불어 피쳐 레벨이라는 변수도 설정해야 한다.
//	// DirectX 11로 설정한다.
//	featureLevel = D3D_FEATURE_LEVEL_11_0;
//
//	/*
//	
//	스왑체인 description과 피쳐 레벨이 설정되어 이제 스왑 체인, D3D 디바이스, 그리고 D3D 디바이스 컨텍스트를 만들 수 있다.
//	Direct3D 디바이스와 Direct3D 디바이스 컨텍스트는 모든 Direct3D 기능의 인터페이스가 되기 때문에 매우 중요하다.
//
//	이전 버전에서는 Direct3D 디바이스만 사용했지만, 디바이스 컨텍스트는 DirectX11에서 사용한다.
//	기본적으로 이 둘은 기존 Direct3D 디바이스의 기능을 두 개로 쪼갰다고 생각하면 된다. 따라서 두 객체가 모두 필요하며 사용된다.
//
//	만약 유저가 DirectX 11을 지원하는 그래픽카드를 갖고있지않다면 디바이스와 디바이스 컨텍스트를 생성하는 함수는 실패할 것이다.
//	따라서 테스트는 하고싶지만 그래픽카드가 지원하지 않는 경우 이 부분에서 D3D_DRIVER_TYPE_HARDWARE을 D3D_DRIVER_TYPE_REFERENCE로 바꾸어서 렌더링을 CPU에서 처리할 수 있게 할 수 있다.
//	속도는 1/1000정도 느리다.
//
//	가끔씩 첫번째 그래픽카드가 DirectX11과 호환되지 않는 경우 실패하는 경우가 있다.
//	첫번째 그래픽카드가 DirectX10 까지만 지원하고 두번째 그래픽카드에서 DirectX11을 지원할 수 있습니다.
//	또한 일부 하이브리드 그래픽카드일 경우 첫번째는 저전력 인텔 카드고, 두번째는 엔비디아카드로 동작할 수 있다.
//	
//	이런 경우에 모두 대응하기 위해서 절대로 기본 그래픽카드로 생성하지 않고 모든 비디오 카드들을 대조하여 가장 잘맞는 그래픽 카드로 생성할 수 있도록 해야한다.
//	*/
//
//	// 스왑 체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트를 생성한다.
//	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
//		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 이제 스왑 체인이 생겼으므로 우리는 백버퍼를 만들고 그 포인터를 스왑 체인에 연결시켜 주어야 한다.
//	// 이를 위해 CreateRenderTargetView 함수를 사용하여 백버퍼를 스왑 체인에 연결한다.
//	// 백버퍼의 포인터를 가져온다.
//	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 백버퍼의 포인터로 렌더타겟 뷰를 생성한다.
//	// 백버퍼-스왑체인 연결
//	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 백버퍼 포인터를 더이상 사용하지 않으므로 할당 해제한다.
//	backBufferPtr->Release();
//	backBufferPtr = 0;
//
//	/*
//	- 깊이 버퍼의 description 구조체도 작성해야 한다.
//	- 이를 이용해 깊이 버퍼를 만들어야 3D공간에서 우리의 폴리곤들이 올바르게 그려진다.
//	
//	- 또한 동시에 스텐실 버퍼도 이 깊이 버퍼에 연결할 것이다.
//	- 스텐실 버퍼는 모션 블러, 볼륨있는 그림자 등의 효과를 나타낼 때 사용된다.
//
//	- 깊이/스텐실 버퍼는 CreateTexture2D로 만든다.
//	- 버퍼는 단지 2D텍스쳐에 지나지 않는다.
//	- 그 이유는 폴리곤들이 정렬되고 래스터화된 이후에는 어쨋든 2D좌표의 픽셀들이 되기 때문이다.
//	- 그리고 이 2D버퍼는 화면에 그려진다.
//	*/
//
//	// 깊이 버퍼의 description을 초기화한다.
//	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
//
//	// 깊이 버퍼의 description을 작성한다.
//	depthBufferDesc.Width = screenWidth;
//	depthBufferDesc.Height = screenHeight;
//	depthBufferDesc.MipLevels = 1;
//	depthBufferDesc.ArraySize = 1;
//	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthBufferDesc.SampleDesc.Count = 1;
//	depthBufferDesc.SampleDesc.Quality = 0;
//	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	depthBufferDesc.CPUAccessFlags = 0;
//	depthBufferDesc.MiscFlags = 0;
//
//	// description을 사용하여 깊이 버퍼의 텍스쳐를 생성한다.
//	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	/* 
//	- 이제 깊이/스텐실 버퍼의 description을 작성하는것이다.
//	- 이 과정은 우리가 Direct3D에서 각 픽셀에 어떤 깊이 테스트를 할것인지 정할 수 있게 해준다.
//	- 깊이 테스트: 특정 픽셀의 위치에서 경쟁하는 픽셀의 깊이를 비교하는 과정.
//	*/
//
//	// 스텐실 상태의 description을 초기화 한다.
//	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
//
//	// 스텐실 상태의 description을 작성한다.
//	depthStencilDesc.DepthEnable = true;
//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//
//	depthStencilDesc.StencilEnable = true;
//	depthStencilDesc.StencilReadMask = 0xFF;
//	depthStencilDesc.StencilWriteMask = 0xFF;
//
//	// Stencil operations if pixel is front-facing.
//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Stencil operations if pixel is back-facing.
//	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// 위에 작성한 description을 토대로 깊이-스텐실 상태 변수를 만든다.
//	// 깊이-스텐실 상태를 생성한다.
//	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 디바이스 컨텍스트를 사용해 이것을 적용한다.
//	// 깊이-스텐실 상태를 설정한다.
//	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
//
//	/*
//	- 다음으로는 깊이-스텐실 버퍼의 뷰에 대한 description을 만드는 것이다.
//	- 이 작업을 해야 Direct3D에서 깊이 버퍼를 깊이-스텐실 텍스쳐로 인식한다.
//	- 이 구조체를 채우고 난 후에 CreateDepthStencilView 함수를 호출하여 생성한다.
//	*/
//
//	// 깊이-스텐실 뷰의 description을 초기화한다.
//	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
//
//	// 깊이-스텐실 뷰의 description을 작성한다.
//	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	depthStencilViewDesc.Texture2D.MipSlice = 0;
//
//	// 깊이-스텐실 뷰를 생성한다.
//	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	/*
//	- 이제 OMSetRenderTarget함수를 호출할 수 있다.
//	- 이 함수는 렌더타겟뷰와 깊이-스텐실 뷰를 출력 렌더링 파이프라인에 바인딩시킨다.
//
//	- 이렇게 하여 파이프라인을 이용한 렌더링이 수행될 때 우리가 만들었던 백버퍼에 그려지게 된다.
//	- 그리고 백버퍼에 그려진것을 프론트 버퍼와 바꿔치기하여 유저의 모니터에 출력하게 된다.
//
//	- 렌더 타겟이 설정된 뒤 추가적인 기능을 설정할 수 있다.
//
//	1. 래스터화기상태(rasterizer state)
//	- 도형이 어떻게 그려지는지에 대한 제어를 가질 수 있게 한다.
//	- 이를 이용해 우리의 화면을 와이어프레임 모드로 그리거나 도형의 앞뒷면을 모두 그리도록 할 수도 있다.
//	- 정점 셰이더(혹은 기하 셰이더)가 공급한 정점 특성들을 삼각형의 픽셀들에 따라 보간한 결과를 픽셀셰이더에 도달하기까지의 경로를 말한다.
//	*/
//
//	// 렌더타겟 뷰와 깊이-스텐실 버퍼를 각각 출력 파이프라인에 바인딩한다.
//	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
//
//	// 어떤 도형을 어떻게 그릴 것인지 결정하는 래스터화기 description을 작성한다.
//	rasterDesc.AntialiasedLineEnable = false;
//	rasterDesc.CullMode = D3D11_CULL_BACK;
//	rasterDesc.DepthBias = 0;
//	rasterDesc.DepthBiasClamp = 0.0f;
//	rasterDesc.DepthClipEnable = true;
//	rasterDesc.FillMode = D3D11_FILL_SOLID;
//	rasterDesc.FrontCounterClockwise = false;
//	rasterDesc.MultisampleEnable = false;
//	rasterDesc.ScissorEnable = false;
//	rasterDesc.SlopeScaledDepthBias = 0.0f;
//
//	// 작성한 description으로부터 래스터화기 상태를 생성한다.
//	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 래스터화기 상태를 설정한다.
//	m_deviceContext->RSSetState(m_rasterState);
//
//	// 후면 버퍼 끄는 래스터화기 상태.
//	rasterDesc.AntialiasedLineEnable = false;
//	rasterDesc.CullMode = D3D11_CULL_NONE;
//	rasterDesc.DepthBias = 0;
//	rasterDesc.DepthBiasClamp = 0.0f;
//	rasterDesc.DepthClipEnable = true;
//	rasterDesc.FillMode = D3D11_FILL_SOLID;
//	rasterDesc.FrontCounterClockwise = false;
//	rasterDesc.MultisampleEnable = false;
//	rasterDesc.ScissorEnable = false;
//	rasterDesc.SlopeScaledDepthBias = 0.0f;
//
//	// Create the no culling rasterizer state.
//	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	/* 렌더타겟 공간에서 클리핑을 수행할 수 있게 뷰포트가 있어야 한다.
//
//	 - 클리핑 : 화면에 무언가를 그리기 위해 사용될 좌표를 DirectX에 알려주는 과정
//	 - 뷰포트 : 창의 클라이언트 영역(윈도우) 내에 클리핑 영역을 그리는데 사용되는 영역
//
//	 뷰포트 영역은 윈도우 영역의 일부 영역만을 지정할 수도 있다.
//	 물체는 클리핑 공간에서와 같은 크기로 보이게 되서 영역이 작으면 확대, 크면 잘려서 보이게 된다.
//	*/
//	m_viewport.Width = (float)screenWidth;
//	m_viewport.Height = (float)screenHeight;
//	m_viewport.MinDepth = 0.0f;
//	m_viewport.MaxDepth = 1.0f;
//	m_viewport.TopLeftX = 0.0f;
//	m_viewport.TopLeftY = 0.0f;
//
//	// 뷰포트를 생성한다.
//	m_deviceContext->RSSetViewports(1, &m_viewport);
//
//	/*
//	1. 투영행렬
//	- 투영행렬은 3D의 화면을 앞서 만들었던 2D 뷰포트 공간으로 변환하도록 해주는 것이다.
//	- 이 행렬의 복사본을 만들어서 셰이더에서 사용할 수 있도록 해야 한다.
//
//	2. 월드 행렬
//	- 이 행렬은 우리의 오브젝트들을 3D세계의 좌표로 변환하는 데 사용된다.
//	- 또한 3차원상에서의 회전/이동/크기 변환에도 사용된다.
//	- 이 행렬의 복사본을 만들어서 셰이더에서 사용할 수 있도록 해야 한다.
//
//	3. 뷰행렬
//	- 뷰 행렬은 우리가 어디를 보고있는가를 계산하는 데 이용된다.
//	- 주로 카메라에 많이 쓰인다.
//
//	*/
//
//	// 투영행렬을 설정한다.
//	fieldOfView = (float)D3DX_PI / 4.0f;
//	screenAspect = (float)screenWidth / (float)screenHeight;
//
//	// 3D렌더링을 위한 투영 행렬을 생성한다.
//	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
//
//	// 월드 행렬을 단위 행렬로 초기화한다.
//	D3DXMatrixIdentity(&m_worldMatrix);
//
//	// 여기에 나중에 뷰 행렬을 만들어야 한다.
//
//	// 정사영 행렬을 만든다.
//	// 이 행렬은 3D객체가 아니라 UI와 같은 2D의 요소들을 그리기 위해 사용된다.
//	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
//
//	/*
//	- 2D 이미지 렌더링을 위해 2번째 깊이-스텐실 상태 변수의 description을 작성한다.
//	- 두번째 깊이-스텐실 상태 변수는 DepthEnable이 false로 세팅된다.(유일한 차이점)
//	*/
//
//	// Clear the second depth stencil state before setting the parameters.
//	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
//
//	depthDisabledStencilDesc.DepthEnable = false;
//	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//	depthDisabledStencilDesc.StencilEnable = true;
//	depthDisabledStencilDesc.StencilReadMask = 0xFF;
//	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
//	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	//2번째 깊이-스텐실 상태 변수 생성
//	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
//	if (FAILED(result)) { return false; }
//
//	/*
//	- Text와 3D모델들이 잘 어우러지게 하기위해 알파블렌딩 상태를 만든다.
//	*/
//	
//	// Clear the blend state description
//	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
//
//	/*
//	알파 블렌딩 기능을 사용할 알파블렌딩상태의 DESC를 작성한다.
//	여기선 어떻게 색상을 혼합할 지 결정할 블렌딩 공식을 설정한다.
//
//	DestBlend: 이미 그 자리에 그려져 있던 목표 픽셀의 색상
//	여기에 사용할 블렌딩 함수 : D3D11_BLEND_INV_SRC_ALPHA는 소스 텍스처의 알파를 역전시키는것이다.
//	1에서 소스 텍스처의 알파만큼 뺀다는 뜻. 만약 소스의 알파값이 0.3이면 목표의 알파는 0.7로 보고 목표 픽셀의 70%만 사용하게한다.
//
//	srcBlend: 소스 텍스처의 색상을 계산하는데 사용된다.
//	src_alpha 공식: 텍스처가 가지고 있는 알파 값을 그대로 사용한다.
//
//	소스와 목표값을 더해 최종 픽셀의 색상값을 구하게 된다.
//	
//	*/
//	blendStateDescription.RenderTarget[0].BlendEnable = TRUE; // 블렌딩을 true로
//	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
//	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; 
//	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
//
//	// 이 Discription을 통해 알파값이 가능한 블렌딩 상태를 만든다.
//	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
//	if(FAILED(result)) { return false; }
//
//	
//	// 알파값이 적용되지 않은 상태를 만들기 위해 BlendEnable을 FALSE로 설정한다.
//	blendStateDescription.RenderTarget[0].BlendEnable = FALSE; 
//
//	// 바뀐 Discription을 통해 알파값이 꺼진 블렌딩 상태를 생성한다.
//	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
//	if (FAILED(result)) { return false; }
//
//
//	// 2 차 알파 혼합 상태 설명을 만듭니다. 
//	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
//	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
//	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
//	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
//
//	// 설명을 사용하여 혼합 상태를 만듭니다.
//	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaBlendState2);
//	if (FAILED(result))
//	{
//		return false;
//	}
//	return true;
//}
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : Shutdown()
//용도 :
// - Initialize함수에서 사용했던 포인터들을 정리하고 반납하는 일을 한다.
//
// - 포인터 정리&반납 전에 스왑체인을 윈도우 모드로 바꾸는 함수를 호출한다.
// - 이 작업을 하지 않는다면 풀스크린 상태에서 스왑 체인의 해제가 일어날 때 몇몇 예외가 발생하게 된다.
// - 그래서 Directr3D를 종료하기 전에는 언제나 스왑 체인을 윈도우 모드로 바꿔주어야 한다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::Shutdown()
//{
//	// 종료하기전에 이렇게 윈도우 모드로 바꾸지 않으면 스왑체인을 할당 해제할 때 예외가 발생한다.
//	if (m_swapChain)
//	{
//		m_swapChain->SetFullscreenState(false, NULL);
//	}
//
//
//	if (m_alphaDisableBlendingState)
//	{
//		m_alphaDisableBlendingState->Release();
//		m_alphaDisableBlendingState = 0;
//	}
//
//	if (m_alphaEnableBlendingState)
//	{
//		m_alphaEnableBlendingState->Release();
//		m_alphaEnableBlendingState = 0;
//	}
//	if (m_alphaBlendState2)
//	{
//		m_alphaBlendState2->Release();
//		m_alphaBlendState2 = 0;
//	}
//
//	if (m_rasterStateNoCulling)
//	{
//		m_rasterStateNoCulling->Release();
//		m_rasterStateNoCulling = 0;
//	}
//
//	if (m_rasterState)
//	{
//		m_rasterState->Release();
//		m_rasterState = 0;
//	}
//	if (m_depthStencilView)
//	{
//		m_depthStencilView->Release();
//		m_depthStencilView = 0;
//	}
//	if (m_depthDisabledStencilState)
//	{
//		m_depthDisabledStencilState->Release();
//		m_depthDisabledStencilState = 0;
//	}
//	if (m_depthStencilState)
//	{
//		m_depthStencilState->Release();
//		m_depthStencilState = 0;
//	}
//
//	if (m_depthStencilBuffer)
//	{
//		m_depthStencilBuffer->Release();
//		m_depthStencilBuffer = 0;
//	}
//
//	if (m_renderTargetView)
//	{
//		m_renderTargetView->Release();
//		m_renderTargetView = 0;
//	}
//
//	if (m_deviceContext)
//	{
//		m_deviceContext->Release();
//		m_deviceContext = 0;
//	}
//
//	if (m_device)
//	{
//		m_device->Release();
//		m_device = 0;
//	}
//
//	if (m_swapChain)
//	{
//		m_swapChain->Release();
//		m_swapChain = 0;
//	}
//
//	return;
//}
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : BeginScene()
//용도 :
// - 새로운 매 프레임 마다 3D화면을 그리기 시작할 때 호출된다.
// - 버퍼를 빈 값으로 초기화 하고 렌더링이 이루어지도록 준비한다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::BeginScene(float red, float green, float blue, float alpha)
//{
//	float color[4];
//
//
//	// Setup the color to clear the buffer to.
//	color[0] = red;
//	color[1] = green;
//	color[2] = blue;
//	color[3] = alpha;
//
//	// 백버퍼의 내용을 지웁니다.
//	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
//
//	// 깊이 버퍼를 지웁니다.
//	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//
//	return;
//}
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : EndScene()
//용도 :
// - 백버퍼의 내용(렌더링)이 다 채워졌으면 화면에 표시한다.
// - fps를 고정시킬건지 아닌지 본 후 백버퍼-프론트 버퍼를 swap시킨다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::EndScene()
//{
//
//	if (m_vsync_enabled)
//	{
//		// FPS 비율을 고정한다.
//		m_swapChain->Present(1, 0);
//	}
//	else
//	{
//		// 가능한 한 빠르게 표시한다.
//		m_swapChain->Present(0, 0);
//	}
//
//	return;
//}
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : Get()
//용도 :
//- GraphicsClass에서 사용할 함수들. 
//- Direct3D디바이스와 디바이스 컨텍스트의 포인터를 반환한다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//
//ID3D11Device* D3DClass::GetDevice()
//{
//	return m_device;
//}
//ID3D11DeviceContext* D3DClass::GetDeviceContext()
//{
//	return m_deviceContext;
//}
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : Get() Matrix(행렬들)
//용도 :
//- GraphicsClass에서 사용할 함수들.
//- 투영, 월드, 정사영 행렬을 반환한다.
//- ShaderClass의 ShaderParameter에서 이 행렬들이 필요하기 때문에 이를 가져올 손쉬울 방법이 필요해서 선언.
//- 왜 뷰행렬은 없지? : 뷰 행렬은 CameraClass에서 얻을 수 있다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//
//void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
//{
//	projectionMatrix = m_projectionMatrix;
//	return;
//}
//
//void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
//{
//	worldMatrix = m_worldMatrix;
//	return;
//}
//
//void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
//{
//	orthoMatrix = m_orthoMatrix;
//	return;
//}
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : GetVideoCardInfo()
//용도 :
//- 그래픽카드의 이름과 사용가능한 메모리의 양을 반환한다.
//- 그래픽 카드의 이름과 메모리의 크기를 아는것은 서로 다른 설정에서 디버깅 하는데 도움이 될 수 있다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
//{
//	strcpy_s(cardName, 128, m_videoCardDescription);
//	memory = m_videoCardMemory;
//	return;
//}
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : TurnZbufferOn/Off()
//용도 :
//- 2D렌더링을 위해 Z버퍼를 켜고 끄는일을 한다.
//- Z버퍼를 켜는 것 : 원래 있던 깊이-스텐실 상태를 사용한다.
//- Z버퍼를 끄는 것 : depthEnable을 false로 지정했던 두번째 깊이-스텐실 상태를 사용한다.
//
//- 일반적으로 3D렌더링 - Z버퍼 OFF - 2D렌더링 - Z버퍼 ON의 단계를 거친다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::TurnZBufferOn()
//{
//	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
//	return;
//}
//
//void D3DClass::TurnZBufferOff()
//{
//	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
//	return;
//}
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : TurnAlphaBlendingOn/Off
//용도 :
//- Text렌더링을 위해 알파블렌딩 상태를 켜고 끄는일을 한다.
//
//- OMsetBlendState함수와 BlendingState변수를 사용해 알파블렌딩을 켜고 끄는일을 한다.
//
//- additive blend for cloud
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::TurnOnAlphaBlending()
//{
//	float blendFactor[4];
//
//	// Setup the blend factor.
//	blendFactor[0] = 0.0f; 
//	blendFactor[1] = 0.0f;
//	blendFactor[2] = 0.0f;
//	blendFactor[3] = 0.0f;
//	
//	// Turn on the alpha blending.
//	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
//	
//	return;
//}
//
//void D3DClass::TurnOffAlphaBlending()
//{
//	float blendFactor[4];
//
//	// Setup the blend factor.
//	blendFactor[0] = 0.0f;
//	blendFactor[1] = 0.0f;
//	blendFactor[2] = 0.0f;
//	blendFactor[3] = 0.0f;
//
//	// Turn on the alpha blending.
//	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
//
//	return;
//}
//
////파티클 섞는데에 쓰인다.
//void D3DClass::EnableSecondBlendState()
//{
//	float blendFactor[4];
//
//
//	// 블렌드 인수를 설정합니다. 
//	blendFactor[0] = 0.0f;
//	blendFactor[1] = 0.0f;
//	blendFactor[2] = 0.0f;
//	blendFactor[3] = 0.0f;
//
//	// 알파 블렌딩을 켭니다. 
//	m_deviceContext->OMSetBlendState(m_alphaBlendState2, blendFactor, 0xffffffff);
//
//	return;
//}
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : TurnCullingOn/off
//용도 : 후면 컬링 : 파이프라인에서 사용자가 보지 않는 뒷쪽면, 즉 후면 삼각형을 골라서 폐기하는 공정이다. 
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::TurnOnCulling()
//{
//	// Set the culling rasterizer state.
//	m_deviceContext->RSSetState(m_rasterState);
//
//	return;
//}
//
//void D3DClass::TurnOffCulling()
//{
//	// Set the no back face culling rasterizer state.
//	m_deviceContext->RSSetState(m_rasterStateNoCulling);
//
//	return;
//}
//
//
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : GetDepthStencilView()
//용도 :
//- 깊이 스텐실 뷰에 대한 접근을 제공한다.
//- 왜 필요 하냐면 RTTClass에서 RTT텍스처를 렌더타겟으로 정할 때 깊이-스텐실 뷰가 필요하기 때문에 GRAPHICS.에서 합치려고!
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
//{
//	return m_depthStencilView;
//}
///*---------------------------------------------------------------------------------------------------------------------------------------------
//이름 : SetBackBufferRenderTarget()
//용도 :
//- 클래스의 백 버퍼를 현재 렌더 타겟으로 설정한다.
//- RTT가 끝나고 렌더 타겟을 다시 백버퍼로 돌려야 할 때 호출된다.
//-----------------------------------------------------------------------------------------------------------------------------------------------*/
//void D3DClass::SetBackBufferRenderTarget()
//{
//	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
//
//	return;
//}
//
//void D3DClass::ResetViewport()
//{
//	// Set the viewport.
//	m_deviceContext->RSSetViewports(1, &m_viewport);
//
//	return;
//}