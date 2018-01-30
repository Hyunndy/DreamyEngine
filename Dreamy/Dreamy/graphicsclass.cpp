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
�̸� : Initialize()
�뵵 : ��ü�� �ʱ�ȭ
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// D3D��ü ����
	//-------------------------------------------------------------------------------------
	m_D3D = new D3DClass;

	if (!m_D3D) { return false; }

	/* Direct3D ��ü�� �ʱ�ȭ�մϴ�.
	 �� �Լ����� ȭ���� �ʺ�, ����, ������ �ڵ�, Graphicsclass.h�� ���ǵ� �� ���� ���� �������� �����Ѵ�.
	 D3DCLASS���� �� �������� ����Ͽ� Direct3D �ý����� �����Ѵ�. */
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// ī�޶�ü ����
	//-------------------------------------------------------------------------------------
	m_Camera = new CameraClass;

	if (!m_Camera) { return false; }

	//�۲��� ��ġ
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	//-------------------------------------------------------------------------------------

	// Text��ü ����
	//-------------------------------------------------------------------------------------
	m_Title = new TextClass;

	if (!m_Title) { return false; }

	result = m_Title->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) { MessageBox(hwnd, L"Could not initialize TitleText", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// 3D�� ��ü ����
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

	// 2D�̹��� ��ü ����
	//-------------------------------------------------------------------------------------
	m_2D_Love = new ImageClass;

	result = m_2D_Love->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/love.jpg", 400, 300);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// ���̴� ��ü ����
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }

	result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// �� ��ü ����
	//-------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.30f, 0.30f, 0.30f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f); // ���� ������ z���� ���� ������ �Ѵ�.
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // �ݻ簭������ �������� �ݻ籤 ȿ���� Ŀ����!
    //-------------------------------------------------------------------------------------

	// ModelList��ü ����
	//-------------------------------------------------------------------------------------
	m_Model_CircleList = new ModelManagerClass;
	if (!m_Model_CircleList) { return false; }

	// �� 4���� ������!
	m_Model_CircleList->Initialize(4);
	if (!result) { MessageBox(hwnd, L"����Ʈ ����µ� ������", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------


	// Frustum ��ü ����
	//-------------------------------------------------------------------------------------
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }

	//-------------------------------------------------------------------------------------

	return true;

}

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
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
�̸� : Frame()
�뵵 : 
- SystemClass���� ������ ���� ȣ��ȴ�.
- ���콺������ �����̴°� �� �� ����
- fps,cpu,frametime�缭 text�� ���.
- ��ü ���� �� ����
------------------------------------------------------------------------------------------*/
// �� ȣ�⸶�� Render�Լ��� �θ���.
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, float rotationY)
{
	bool result;

	// ���콺 ��������ġ ���� �� �ְ�
	//result = m_Title->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());

	// fps����
	result = m_Title->SetFps(fps, m_D3D->GetDeviceContext());
	if (!result) { return false; }

	// cpu����
	result = m_Title->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result) { return false; }

    //ī�޶� ��ġ �ʱ�ȭ
	m_Camera->SetPosition(0.0f, 6.0f, -15.0f);
	//ī�޶� ȸ��
	m_Camera->SetRotation(15.0f,0.0f, 0.0f);


	//��ü ���� �ӵ��� ȸ����Ű�� ���� ��.
	//static float rotation = 0.0f;
	//rotation += (float)D3DX_PI * 0.01f;
	//if (rotation > 360.0f)
	//	rotation -= 360.0f;

	return true;
}


/*----------------------------------------------------------------------------------------
�̸� : Render()
�뵵 :
- 1. ī�޶��� ��ġ�� ���� �� ����� ����� ���� ī�޶��� render�Լ��� ȣ���Ѵ�.
- 2. �� ����� ����� ���纻�� �����´�.
- 3. d3dclass��ü�� ���� ���� ��İ� ���� ����� �����ؿ´�.
- 4. modelclass�� render()�Լ��� ȣ���Ͽ� �׷��� ���������ο� �ﰢ�� ���� �׸����� �Ѵ�.
- 5. ���⼭ �غ��� ������� ���̴��� ȣ���Ͽ� ���̴��� �� ������ ������ ��ġ��Ű�� ���� �� ����� ����Ͽ� �������� �׷�����.
- 6. �ﰢ���� ����ۿ� �׷�����.

- ModelClass, ImageClass, TextClass�� Render()�� ����/�ε��� ���۸� �׷��Ƚ� ���������ο� ���� I/A�ܰ� ������ �Ѵ�.
- �� ���ϱ����� �׸���(����ϴ�)�� ShaderClass�� Render()�̴�!
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

	//����
	//-------------------------------------------------------------------------------------
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//ī�޶� ��ġ
	//-------------------------------------------------------------------------------------
	m_Camera->Render();

	//�⺻ ����,��,����,���翵 ��� ȹ��
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

	//�þ� ����ü ����
	//-------------------------------------------------------------------------------------
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	//-------------------------------------------------------------------------------------

	//�⺻ ��� ��ȯ
	//-------------------------------------------------------------------------------------
	// �����ϸ�->ȸ��->�̵� ������ ���ľ� �Ѵ�.
	D3DXMatrixRotationY(&worldMatrix3,0.6f);
	D3DXMatrixTranslation(&JMatrix, 0, 5, -6);
	D3DXMatrixMultiply(&worldMatrix3, &worldMatrix3, &JMatrix);

	D3DXMatrixRotationY(&worldMatrix4, 0.6f);
	D3DXMatrixTranslation(&Cube2Matrix, RotationY*0.01f, 5, Speed*0.01f-6 );
	D3DXMatrixMultiply(&worldMatrix4, &worldMatrix4, &Cube2Matrix);

	//-------------------------------------------------------------------------------------

	//3D�� ����Ʈ ������ �ʱ�ȭ
	//-------------------------------------------------------------------------------------
	modelCount = m_Model_CircleList->GetModelCount();
	//-------------------------------------------------------------------------------------

	//�⺻ ��� ��ȯ, 3D��, ���̴� ������, �������� �ø�
	//-------------------------------------------------------------------------------------
	
	for (index = 0; index < modelCount; index++)
	{
	//����Ʈ�� �ִ°͵� ���ʴ�� �����͸� �о�´�.
	m_Model_CircleList->GetData(index, positionX, positionY, positionZ, color);
	
	radius = 1.0f;
	
	//Frustum Culling(�������� �ø�)
	//3D�𵨿� ��躼���� ������ ����ü�� ���̴��� üũ�Ѵ�.
	renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);
	
		if (renderModel)
		{
			//����Ʈ���� ���ǵȴ�� ��ġ ��ȯ
			D3DXMatrixTranslation(&worldMatrix2, positionX, positionY, positionZ);
	
			//3D��(��ü) ������
			m_Model_Circle->Render(m_D3D->GetDeviceContext());
	
			//���̴� ������
			result = m_Shader->RenderLightShader(m_D3D->GetDeviceContext(), m_Model_Circle->GetIndexCount(), worldMatrix2, viewMatrix, projectionMatrix
				, m_Model_Circle->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), color
				, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result) { return false; }
	
			//���� ���� �ٸ� ��ġ�� ������ �ϴϱ� reset��Ų��!
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


	//2D�̹���, 2d���̴� ������, Text, Z���� ON/OFF, ���ĺ���
	//-------------------------------------------------------------------------------------
	m_D3D->TurnZBufferOff();

	if (Pressed == true)
	{
		result = m_2D_Love->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) { return false; }

		/* ����/�ε��� ���۰� �غ��ٸ� �ؽ�ó ���̴��� ���� �׸��� �ȴ�.
		 2D�������� ���� ������� ��� ���翵 ����� ����Ѵ�.
		 �� ����� ������ ��� �ٲ�� ���̶�� 2D���������� ���� �⺻ �� ����� ���� ����� ����ؾ� �Ѵ�.
		 (������ ���� ī�޶��� ��ġ�� ������ �����Ƿ� �Ϲ� �� ����� ����Ѵ�) */
		result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_2D_Love->GetIndexCount(), textworldMatrix3, baseViewMatrix, orthoMatrix, m_2D_Love->GetTexture());
		if (!result) { return false; }
	}
	m_D3D->TurnOnAlphaBlending();

	result = m_Title->Render(m_D3D->GetDeviceContext(), textworldMatrix3, orthoMatrix);
	if (!result) { return false; }

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();

	return true;
}