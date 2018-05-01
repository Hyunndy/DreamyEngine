#include "UIManagerClass.h"



UIManagerClass::UIManagerClass()
{
	m_UI[0] = nullptr;
	m_UI[1] = nullptr;
	m_UI[2] = nullptr;
	m_UI[3] = nullptr;
	m_UI[4] = nullptr;

	render[0] =false;
	render[1] =false;
	render[2] =false;
	render[3] =false;
	render[4] =false;
}


UIManagerClass::~UIManagerClass()
{
}


void UIManagerClass::Initialize(int screenWidth, int screenHeight)
{

	m_UI[0] = new ImageClass;
	m_UI[0]->Initialize(screenWidth, screenHeight, L"../Data/UI1.png", screenWidth, screenHeight);

	m_UI[1] = new ImageClass;
	m_UI[1]->Initialize(screenWidth, screenHeight, L"../Data/UI2.png", screenWidth, screenHeight);

	m_UI[2] = new ImageClass;
	m_UI[2]->Initialize(screenWidth, screenHeight, L"../Data/UI3.png", screenWidth, screenHeight);

	m_UI[3] = new ImageClass;
	m_UI[3]->Initialize(screenWidth, screenHeight, L"../Data/UI4.png", screenWidth, screenHeight);

	m_UI[4] = new ImageClass;
	m_UI[4]->Initialize(screenWidth, screenHeight, L"../Data/UI5.png", screenWidth, screenHeight);

}
//집  49 나무1 49 나무2 49
void UIManagerClass::renderUI(TextureShaderClass* Shader, D3DXMATRIX worldMatrix, D3DXMATRIX ImageViewMatrix, D3DXMATRIX orthoMatrix)
{

	if (m_UI[0]->active == true)
	{
		m_UI[0]->Render(0, 0);
		Shader->Render(m_UI[0]->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI[0]->GetTexture());
		render[0] = true;	
	}

	if (m_UI[1]->active == true && render[0]==true)
	{
		m_UI[1]->Render(1, 1);
		Shader->Render(m_UI[1]->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI[1]->GetTexture());
		render[1] = true;

	}

	if (m_UI[2]->active == true && render[1] == true )
	{
		m_UI[2]->Render(2, 2);
		Shader->Render(m_UI[2]->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI[2]->GetTexture());
		render[2] = true;

	
		m_UI[3]->Render(3, 3);
		Shader->Render(m_UI[3]->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI[3]->GetTexture());
		render[3] = true;

	}

	if (m_UI[4]->active == true && render[3] == true)
	{
		m_UI[4]->Render(4, 4);
		Shader->Render(m_UI[4]->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI[4]->GetTexture());
		render[4] = true;

	}
}

void UIManagerClass::active()
{
	if (m_UI[0]->active == false)
		m_UI[0]->active = true;

	else if (m_UI[0]->active == true && m_UI[1]->active == false)
		m_UI[1]->active = true;

	else if (m_UI[1]->active == true && m_UI[2]->active == false)
		m_UI[2]->active = true;

	else if (m_UI[2]->active == true && m_UI[3]->active == false)
		m_UI[3]->active = true;

	else if (m_UI[3]->active == true && m_UI[4]->active == false)
		m_UI[4]->active = true;

}

void UIManagerClass::unactive()
{
	for (int i = 0; i < 5; i++)
	{
		m_UI[i]->active = false;
		render[i] = false;
	}
}
void UIManagerClass::Shutdown()
{
	SAFE_SHUTDOWN(m_UI[0]);
	SAFE_SHUTDOWN(m_UI[1]);
	SAFE_SHUTDOWN(m_UI[2]);
	SAFE_SHUTDOWN(m_UI[3]);
	SAFE_SHUTDOWN(m_UI[4]);

}