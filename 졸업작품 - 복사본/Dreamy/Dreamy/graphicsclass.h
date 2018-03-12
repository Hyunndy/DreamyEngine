////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

// graphicsclass�� �� ���ø�Ű������ ��� �׷��� ����� �� ��ü�ȿ� ĸ��ȭ�Ǿ� �ִ�.
// ���߿� ��� �׷��� ��ü���� �������̴�. -> ���⼭ �׷��� ��ü���� �� Ŭ������ ������..?

//////////////
// INCLUDES //
//////////////
#pragma once
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ShaderManagerClass.h"
#include "LightClass.h"
#include "ImageClass.h"
#include "TextClass.h"
#include "FrustumClass.h"
#include "ModelManagerClass.h"
#include "RTTClass.h"
#include "RTTTextureClass.h"
#include "TerrainClass.h"
#include "TerrainShaderClass.h"

#include "SkyClass.h"
#include "FBXModel.h"




/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
//���� ����ȭ(fps����)
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	bool Loading(int, int, HWND);

	void Shutdown();
	bool Frame(int, int, float, D3DXVECTOR3, D3DXVECTOR3);// + ī�޶� ȸ��
	bool Render(bool);


	bool RenderMainScene();
	bool RenderLoadingScene();

public:
	bool F1pressed;
	D3DXMATRIX baseViewMatrix; //�۲��� �׻� ���� ��ġ�� ��µǰ�
	D3DXMATRIX minimapMatrix; // �̴ϸ� 


private:

	//RTT����� ����� �� �ְ� �� ������ ������ �Լ��� �����Ѵ�.
	//�������� �� �� �Ͼ���� �и��ߴ�.(ù��°�� �ؽ�ó��, �� ���� �Ϲ� ����ۿ� �׸��°�)
	bool RenderToTexture();
	bool RenderRTTScene();
	bool RenderRunningScene(bool);

private:

	D3DClass* m_D3D; 
	
	CameraClass* m_Camera;
	CameraClass* m_minimapCamera;

	ShaderManagerClass* m_Shader;
	LightClass* m_Light;

	ModelClass* m_Model_Plane;
	ModelClass* m_Model_Plane2;

	ModelClass* m_Model_Cube;
	ModelClass* m_Model_Cube2;
	ModelClass* m_Model_Cube3;

	ModelClass* m_Model_Mirror;

	ModelClass* m_Model_Circle;
	ModelManagerClass* m_Model_CircleList;

	ImageClass* m_2D_Love;
	ImageClass* m_Loading;

	TextClass* m_Title;

	FrustumClass* m_Frustum;

	RTTClass* m_RTT;
	RTTTextureClass* m_RTTTexture;
	
	TerrainClass* m_Terrain;
	TerrainShaderClass* m_TerrainShader;


	SkyClass* m_Sky;
	SkyClass* m_Cloud;

	D3DXVECTOR3 CharacterPos;
	D3DXVECTOR3 CharacterRot;

	FBXModel* m_fbx;




};

