////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


/*
이름: GraphicsClass
용도: 
- 이 App의 모든 그래픽 요소들이 캡슐화 되어 있다.
- 빌보드(Billboarding)
 *먼 거리에 있는 3D지형을 표시하기 위해 텍스처를 입힌 사각형을 사용하는것. 
 *실시간 렌더링을 하기엔 그래픽 카드의 가용성이 딸리기 때문에, 가장 가까운 애들은 디테일한 폴리곤-멀리 있는 애들은 적은 폴리곤을 사용하는 빌보드가 될 수 있다.
 *빌보드를 구현하는 방법들간의 차이점은 사각형이 어떻게 회전하는지에 따라 달려있다.
 *이 App에선 유저의 위치에 따라 유저를 항상 마주보게 하는 방법을 사용한다.


*/


//////////////
// INCLUDES //
//////////////
#pragma once


#include "ShaderManagerClass.h"
#include "ModelClass.h"
#include "D3D.h"
#include "FBXShader.h"
#include "ModelScene.h"
#include "TerrainClass.h"
#include "TerrainShaderClass.h"
#include "LightClass.h"
#include "FrustumClass.h"
#include "SkyClass.h"
#include "Rasterizer.h"
#include "Blender.h"
#include "ImageClass.h"
#include "TextClass.h"
#include "InstancingClass.h"
#include "MinimapClass.h"
#include "ParticleSystem.h"



/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
//수직 동기화(fps고정)
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
	bool Frame(int, int, float, D3DXVECTOR3, D3DXVECTOR3, int, int);// + 카메라 회전
	bool Render(bool, int);
	void CheckIntersection(int, int, int, int);

	bool RenderMainScene();
	bool RenderLoadingScene();

	//Picking 검사
	//void TestIntersection(int, int, int, int);
	//bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius);
public:
	bool F1pressed;
	D3DXMATRIX minimapMatrix; // 미니맵 


private:

	//RTT기능을 사용할 수 있게 두 종류의 렌더링 함수를 생성한다.
	//렌더링이 두 번 일어나도록 분리했다.(첫번째는 텍스처에, 그 다음 일반 백버퍼에 그리는것)
	bool RenderToTexture();
	bool RenderRTTScene();
	bool RenderRunningScene(bool);
	bool RenderRefractionToTexture(bool); // 호수 굴절 텍스처
	bool RenderReflectionToTexture(); // 호수 반사 텍스처
	
	void SetEffectVariable(); //이펙트 관련 변수 세팅
	float CalculateBillboarding(D3DXVECTOR3,D3DXVECTOR3);

private:
	bool sibal = true;



private:
	FrustumClass* m_Frustum =  nullptr;
	ShaderManagerClass* m_Shader = nullptr;
	LightClass* m_Light = nullptr;

private:
	TerrainClass* m_Terrain= nullptr;
	TerrainShaderClass* m_TerrainShader= nullptr;
	SkyClass* m_Sky= nullptr;

private:
	ModelClass* m_cube= nullptr;
	ModelClass* m_Tree = nullptr;

private:
	ModelScene* m_horse= nullptr;
	ModelScene* m_npc = nullptr;

private:
	ImageClass* m_Start= nullptr;
	ImageClass* m_Loading= nullptr;
	ImageClass* m_CrossHair= nullptr;

	float MousePosX, MousePosY;
	ImageClass* m_MouseCursor= nullptr;

private:
	ImageClass* m_UI =nullptr;

private:
	InstancingClass* m_Instancing = nullptr;
	InstancingClass* m_Instancing2 = nullptr;
	MinimapClass* m_Minimap = nullptr;

private:
	TextClass* m_Text = nullptr;

private:
	ParticleSystem* m_Particle = nullptr;
private:
	wstring tPosePath;
	wstring idlePath;
	wstring runPath;
	wstring walkPath;

private:
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX ImageViewMatrix;

private:
	D3DXVECTOR3 CameraPos;



};

