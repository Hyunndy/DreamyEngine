////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Shader = 0; 

	m_Light = 0;

	m_Model_Circle = 0;
	m_Model_Plane = 0;
	m_Model_Cube = 0;
	m_Model_Cube2 = 0;
	m_Model_CircleList = 0;


	m_2D_Love = 0;

	m_Title = 0;

	m_Frustum = 0;

	F1pressed = false;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}
GraphicsClass::~GraphicsClass()
{
}


/*----------------------------------------------------------------------------------------
이름 : Initialize()
용도 : 객체들 초기화
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// D3D객체 생성
	//-------------------------------------------------------------------------------------
	m_D3D = new D3DClass;

	if (!m_D3D) { return false; }

	/* Direct3D 객체를 초기화합니다.
	 이 함수에서 화면의 너비, 높이, 윈도우 핸들, Graphicsclass.h에 정의된 네 개의 전역 변수들을 전달한다.
	 D3DCLASS에서 이 변수들을 사용하여 Direct3D 시스템을 설정한다. */
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// 카메라객체 생성
	//-------------------------------------------------------------------------------------
	m_Camera = new CameraClass;

	if (!m_Camera) { return false; }

	//글꼴의 위치
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	//-------------------------------------------------------------------------------------

	// Text객체 생성
	//-------------------------------------------------------------------------------------
	m_Title = new TextClass;

	if (!m_Title) { return false; }

	result = m_Title->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) { MessageBox(hwnd, L"Could not initialize TitleText", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// 3D모델 객체 생성
	//-------------------------------------------------------------------------------------
	m_Model_Circle = new ModelClass;

	result = m_Model_Circle->Initialize(m_D3D->GetDevice(), "../Dreamy/sphere.txt", L"../Dreamy/seafloor.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }

	m_Model_Plane = new ModelClass;

	result = m_Model_Plane->InitializeTriple(m_D3D->GetDevice(), "../Dreamy/plane01.txt", L"../Dreamy/dirt01.dds", L"../Dreamy/stone01.dds", L"../Dreamy/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize mo456456del object", L"Error", MB_OK); return false; }

	m_Model_Cube = new ModelClass;

	result = m_Model_Cube->InitializeBumpMap(m_D3D->GetDevice(), "../Dreamy/cube.txt", L"../Dreamy/stone01.dds", L"../Dreamy/bump01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initializddwqdkqwpdkp[l object", L"Error", MB_OK); return false; }

	m_Model_Cube2 = new ModelClass;

	result = m_Model_Cube2->InitializeSpecMap(m_D3D->GetDevice(), "../Dreamy/cube.txt", L"../Dreamy/stone02.dds", L"../Dreamy/bump02.dds", L"../Dreamy/spec02.dds");
	if (!result) { MessageBox(hwnd, L"Could not dqwd object", L"Error", MB_OK); return false; }
	
	//-------------------------------------------------------------------------------------

	// 2D이미지 객체 생성
	//-------------------------------------------------------------------------------------
	m_2D_Love = new ImageClass;

	result = m_2D_Love->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/love.jpg", 400, 300);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 셰이더 객체 생성
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }

	result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// 빛 객체 생성
	//-------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.30f, 0.30f, 0.30f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f); // 빛의 방향을 z축의 양의 값으로 한다.
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // 반사강도값이 낮을수록 반사광 효과가 커진다!
    //-------------------------------------------------------------------------------------

	// ModelList객체 생성
	//-------------------------------------------------------------------------------------
	m_Model_CircleList = new ModelManagerClass;
	if (!m_Model_CircleList) { return false; }

	// 구 4개를 만들자!
	m_Model_CircleList->Initialize(4);
	if (!result) { MessageBox(hwnd, L"리스트 만드는데 실패함", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------


	// Frustum 객체 생성
	//-------------------------------------------------------------------------------------
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }

	//-------------------------------------------------------------------------------------

	return true;

}

// 모든 그래픽 객체의 종료가 여기에서 일어난다.
void GraphicsClass::Shutdown()
{
	if (m_Title)	{ m_Title->Shutdown();	delete m_Title;	m_Title = 0;}

	if (m_2D_Love)	{ m_2D_Love->Shutdown(); delete m_2D_Love; m_2D_Love = 0;}

	if (m_Frustum) { delete m_Frustum; m_Frustum = 0; }

	if (m_Model_CircleList) { m_Model_CircleList->Shutdown(); delete m_Model_CircleList; m_Model_CircleList = 0; }

	if (m_Model_Cube) { m_Model_Cube->Shutdown(); delete m_Model_Cube; m_Model_Cube = 0; }

	if (m_Model_Cube2) { m_Model_Cube2->Shutdown(); delete m_Model_Cube2; m_Model_Cube2 = 0; }
	if (m_Light)	{ delete m_Light; m_Light = 0; }

	if (m_Shader)	{ m_Shader->Shutdown(); delete m_Shader; m_Shader = 0;}

	if (m_Model_Circle)	{ m_Model_Circle->Shutdown(); delete m_Model_Circle;m_Model_Circle = 0;}

	if (m_Model_Plane)	{ m_Model_Plane->Shutdown(); delete m_Model_Plane;m_Model_Plane = 0;}

	if (m_Camera)	{ delete m_Camera; m_Camera = 0;}

	if (m_D3D){ m_D3D->Shutdown(); delete m_D3D; m_D3D = 0; }
	
	return;
}

/*----------------------------------------------------------------------------------------
이름 : Frame()
용도 : 
- SystemClass에서 프레임 마다 호출된다.
- 마우스포인터 움직이는것 할 수 있음
- fps,cpu,frametime재서 text로 출력.
- 물체 돌릴 수 있음
------------------------------------------------------------------------------------------*/
// 매 호출마다 Render함수를 부른다.
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, float rotationY)
{
	bool result;

	// 마우스 포인터위치 잡을 수 있게
	//result = m_Title->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());

	// fps세팅
	result = m_Title->SetFps(fps, m_D3D->GetDeviceContext());
	if (!result) { return false; }

	// cpu세팅
	result = m_Title->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result) { return false; }

    //카메라 위치 초기화
	m_Camera->SetPosition(0.0f, 6.0f, -15.0f);
	//카메라 회전
	m_Camera->SetRotation(15.0f,0.0f, 0.0f);


	//물체 일정 속도로 회전시키고 싶을 때.
	//static float rotation = 0.0f;
	//rotation += (float)D3DX_PI * 0.01f;
	//if (rotation > 360.0f)
	//	rotation -= 360.0f;

	return true;
}


/*----------------------------------------------------------------------------------------
이름 : Render()
용도 :
- 1. 카메라의 위치를 토대로 뷰 행렬을 만들기 위해 카메라의 render함수를 호출한다.
- 2. 뷰 행렬을 만들고 복사본을 가져온다.
- 3. d3dclass객체로 부터 월드 행렬과 투영 행렬을 복사해온다.
- 4. modelclass의 render()함수를 호출하여 그래픽 파이프라인에 삼각형 모델을 그리도록 한다.
- 5. 여기서 준비한 정점들로 셰이더를 호출하여 셰이더는 모델 정보와 정점을 배치시키기 위한 세 행렬을 사용하여 정점들을 그려낸다.
- 6. 삼각형이 백버퍼에 그려진다.

- ModelClass, ImageClass, TextClass의 Render()은 정점/인덱스 버퍼를 그래픽스 파이프라인에 묶는 I/A단계 까지만 한다.
- 이 기하구조를 그리는(출력하는)건 ShaderClass의 Render()이다!
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Render(bool Pressed, float RotationY, float Speed)
{
	D3DXMATRIX worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4;
	D3DXMATRIX JMatrix, Cube2Matrix;
	D3DXMATRIX textworldMatrix3, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color; // DiffuseColor
	bool renderModel, result;

	//시작
	//-------------------------------------------------------------------------------------
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라 위치
	//-------------------------------------------------------------------------------------
	m_Camera->Render();

	//기본 월드,뷰,투영,정사영 행렬 획득
	//-------------------------------------------------------------------------------------
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetWorldMatrix(worldMatrix2);
	m_D3D->GetWorldMatrix(worldMatrix3);
	m_D3D->GetWorldMatrix(worldMatrix4);


	m_D3D->GetWorldMatrix(JMatrix);
	m_D3D->GetWorldMatrix(Cube2Matrix);
	m_D3D->GetWorldMatrix(textworldMatrix3);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix); 
	//-------------------------------------------------------------------------------------

	//시야 절두체 생성
	//-------------------------------------------------------------------------------------
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	//-------------------------------------------------------------------------------------

	//기본 행렬 변환
	//-------------------------------------------------------------------------------------
	// 스케일링->회전->이동 순으로 합쳐야 한다.
	D3DXMatrixRotationY(&worldMatrix3,0.6f);
	D3DXMatrixTranslation(&JMatrix, 0, 5, -6);
	D3DXMatrixMultiply(&worldMatrix3, &worldMatrix3, &JMatrix);

	D3DXMatrixRotationY(&worldMatrix4, 0.6f);
	D3DXMatrixTranslation(&Cube2Matrix, RotationY*0.01f, 5, Speed*0.01f-6 );
	D3DXMatrixMultiply(&worldMatrix4, &worldMatrix4, &Cube2Matrix);

	//-------------------------------------------------------------------------------------

	//3D모델 리스트 변수들 초기화
	//-------------------------------------------------------------------------------------
	modelCount = m_Model_CircleList->GetModelCount();
	//-------------------------------------------------------------------------------------

	//기본 행렬 변환, 3D모델, 셰이더 렌더링, 프러스텀 컬링
	//-------------------------------------------------------------------------------------
	
	for (index = 0; index < modelCount; index++)
	{
	//리스트에 있는것들 차례대로 데이터를 읽어온다.
	m_Model_CircleList->GetData(index, positionX, positionY, positionZ, color);
	
	radius = 1.0f;
	
	//Frustum Culling(프러스텀 컬링)
	//3D모델에 경계볼륨을 씌워서 절두체에 보이는지 체크한다.
	renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);
	
		if (renderModel)
		{
			//리스트에서 정의된대로 위치 변환
			D3DXMatrixTranslation(&worldMatrix2, positionX, positionY, positionZ);
	
			//3D모델(구체) 렌더링
			m_Model_Circle->Render(m_D3D->GetDeviceContext());
	
			//셰이더 렌더링
			result = m_Shader->RenderLightShader(m_D3D->GetDeviceContext(), m_Model_Circle->GetIndexCount(), worldMatrix2, viewMatrix, projectionMatrix
				, m_Model_Circle->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), color
				, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result) { return false; }
	
			//구가 각자 다른 위치를 가져야 하니까 reset시킨다!
			m_D3D->GetWorldMatrix(worldMatrix2);
		}
		
	}

	m_Model_Plane->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderAlphaMapShader(m_D3D->GetDeviceContext(), m_Model_Plane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix
		, m_Model_Plane->GetTripleTextureArray());
	if (!result) { return false; }


	m_Model_Cube->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderBumpMapShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), worldMatrix3, viewMatrix, projectionMatrix
		, m_Model_Cube->GetMultiTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result) { return false; }

 

	m_Model_Cube2->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderSpecMapShader(m_D3D->GetDeviceContext(), m_Model_Cube2->GetIndexCount(), worldMatrix4, viewMatrix, projectionMatrix
		, m_Model_Cube2->GetTripleTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result) { return false; }


	//-------------------------------------------------------------------------------------


	//2D이미지, 2d셰이더 렌더링, Text, Z버퍼 ON/OFF, 알파블렌딩
	//-------------------------------------------------------------------------------------
	m_D3D->TurnZBufferOff();

	if (Pressed == true)
	{
		result = m_2D_Love->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) { return false; }

		/* 정점/인덱스 버퍼가 준비됬다면 텍스처 셰이더를 통해 그리게 된다.
		 2D렌더링을 위해 투영행렬 대신 정사영 행렬을 사용한다.
		 뷰 행렬의 내용이 계속 바뀌는 것이라면 2D렌더링만을 위한 기본 뷰 행렬을 따로 만들어 사용해야 한다.
		 (하지만 현재 카메라의 위치가 변하지 않으므로 일반 뷰 행렬을 사용한다) */
		result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_2D_Love->GetIndexCount(), textworldMatrix3, baseViewMatrix, orthoMatrix, m_2D_Love->GetTexture());
		if (!result) { return false; }
	}
	m_D3D->TurnOnAlphaBlending();

	result = m_Title->Render(m_D3D->GetDeviceContext(), textworldMatrix3, orthoMatrix);
	if (!result) { return false; }

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	//버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();

	return true;
}