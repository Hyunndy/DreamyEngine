//#include "RTTClass.h"
//
//
//
//RTTClass::RTTClass()
//{
//	m_renderTargetTexture = 0;
//	m_renderTargetView = 0;
//	m_shaderResourceView = 0;
//}
//
//RTTClass::RTTClass(const RTTClass& other)
//{
//}
//
//RTTClass::~RTTClass()
//{
//}
//
//
///*----------------------------------------------------------------------------------------
//이름: Initialize()
//용도:
//- RTT할 영역의 너비와 높이를 인자로 받는다
//*********만약 화면의 내용을 텍스처에 그린다면 찌그러짐을 방지하기 위해 반드시 RTT의 가로세로 비율을 화면 비율과 같게 한다.
//
//- 1. 텍스처의 DESC를 작성하고 생성하는 방법으로 타겟이 되는 텍스처를 만든다.
//- 2. 만들어진 텍스처를 렌더 타겟 뷰로 설정하여 렌더링이 텍스처에 일어나도록 한다.
//  여기서 렌더 타겟 뷰를 설정하는 CreateRenderTargetView()함수는 D3D클래스에서 첫번째인자가 백버퍼포인터였다.
//  하지만 여긴 만들어진 텍스처가 들어간다.
//
//- 3. 이 텍스처에 대한 ID3D11ShaderResourceView를 만들어 렌더링된 데이터에 접근한다.
//  셰이더 리소스 뷰는 처음 들어보는걸????
//----------------------------------------------------------------------------------------*/
//bool RTTClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
//{
//	D3D11_TEXTURE2D_DESC textureDesc;
//	HRESULT result;
//
//
//	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
//
//
//	//RTT DESC를 초기화한다.
//	ZeroMemory(&textureDesc, sizeof(textureDesc));
//
//	// RTT DESC를 세팅한다.
//	// BindFlgs가 어떤건 vertexbuffer, indexbuffer, constantbuffer 등 많아.
//	textureDesc.Width = textureWidth;
//	textureDesc.Height = textureHeight;
//	textureDesc.MipLevels = 1;
//	textureDesc.ArraySize = 1;
//	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	textureDesc.SampleDesc.Count = 1;
//	textureDesc.Usage = D3D11_USAGE_DEFAULT;
//	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//	textureDesc.CPUAccessFlags = 0;
//	textureDesc.MiscFlags = 0;
//
//	// RTT를 생성한다. (텍스처 DESC를 작성한 후 타겟이 되는 텍스처를 생성한다.)
//	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 렌더 타겟 뷰 DESC를 생성한다.
//	renderTargetViewDesc.Format = textureDesc.Format;
//	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//	renderTargetViewDesc.Texture2D.MipSlice = 0;
//
//	// 렌더 타겟 뷰를 생성한다. 
//	//여기서 렌더 타겟 뷰를 설정하는 CreateRenderTargetView()함수는 D3D클래스에서 첫번째인자가 백버퍼포인터였다.
//	//하지만 여긴 만들어진 텍스처가 들어간다.
//	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// 셰이더 리소스 뷰에 대한 DESC를 설정한다.
//	shaderResourceViewDesc.Format = textureDesc.Format;
//	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
//	shaderResourceViewDesc.Texture2D.MipLevels = 1;
//
//	// 셰이더 리소스 뷰를 생성한다.
//	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	return true;
//}
//
//
//void RTTClass::Shutdown()
//{
//	if (m_shaderResourceView)
//	{
//		m_shaderResourceView->Release();
//		m_shaderResourceView = 0;
//	}
//
//	if (m_renderTargetView)
//	{
//		m_renderTargetView->Release();
//		m_renderTargetView = 0;
//	}
//
//	if (m_renderTargetTexture)
//	{
//		m_renderTargetTexture->Release();
//		m_renderTargetTexture = 0;
//	}
//
//	return;
//}
//
///*----------------------------------------------------------------------------------------
//이름: SetRenderTarget()
//용도: m_renderTargetView를 렌더 타겟으로 설정해서 모든 렌더링이 이 텍스처에 적용되게 한다.
//	  D3D클래스에도 있다.
//----------------------------------------------------------------------------------------*/
//void RTTClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
//{
//	// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 파이프라인에 바인딩한다.
//	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
//
//	return;
//}
//
///*----------------------------------------------------------------------------------------
//이름: ClearRenderTarget()
//용도: 
//- D3DClass::BeginScene()와 같다.
//- 차이점은 백버퍼가 아닌 m_renderTargetView에 클리어가 이루어진다.
//- 매 프레임 RTT가 일어나기전에 호출해야 한다.
//----------------------------------------------------------------------------------------*/
//void RTTClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
//	float red, float green, float blue, float alpha)
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
//	// Clear the back buffer.
//	deviceContext->ClearRenderTargetView(m_renderTargetView, color);
//
//	// Clear the depth buffer.
//	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//
//	return;
//}
//
///*----------------------------------------------------------------------------------------
//이름: GetShaderResourceView()
//용도:
//- 텍스처 데이터를 셰이더 리소스 뷰로 전달한다.
//- RTT의 결과물 텍스처를 이용하는 서로 다른 셰이더들에게 텍스처 데이터에 접근할 수 있게 한다.
//- RTT 텍스처를 쓰려면 일반적으로 셰이더에 텍스처를 직접 전달 해주던 부분에 이 함수를 호출하는 부분을 넣는다.
//  ( graphics클래스에서 renderShader()에서 GetTextureArray() 하던거 말인듯..?)
//----------------------------------------------------------------------------------------*/
//ID3D11ShaderResourceView* RTTClass::GetShaderResourceView()
//{
//	return m_shaderResourceView;
//}