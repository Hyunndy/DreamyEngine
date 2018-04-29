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
#include "WaterClass.h"
#include "RTTTextureClass.h"




/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
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
	bool Frame(int, float, D3DXVECTOR3, D3DXVECTOR3, int, int, bool);// + 카메라 회전
	bool Render(bool);
	void CheckIntersection(int, int, int, int);
	void Shoot(float);

	bool RenderMainScene();
	bool RenderLoadingScene();
	bool RenderEndScene();

	bool End();

	//Picking 검사
	//void TestIntersection(int, int, int, int);
	//bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius);
public:
	bool F1pressed;
	D3DXMATRIX minimapMatrix; // 미니맵 
	D3DXMATRIX Cube3WorldMatrix;
	D3DXMATRIX ShootMatrix;

	D3DXVECTOR3 Gravity = { 0.0f, -9.8f, 0.0f };
	D3DXVECTOR3 Accell = { 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3 vPosition = { 0.0f, 0.0f, 0.0f };
	//D3DXVECTOR3 vVelocity = { 0.0f,0.97f,0.08f };
	D3DXVECTOR3 vVelocity = { 0.0f,1.5f,0.5f };
private:
	void PreShutdown();
	bool RenderRunningScene(bool);
	bool RenderRefractionToTexture(bool); // 호수 굴절 텍스처


	void SetEffectVariable(float frametime); //이펙트 관련 변수 세팅
	float CalculateBillboarding(D3DXVECTOR3,D3DXVECTOR3);

private:
	bool m_end = false; //게임 클리어인지 아닌지.
private:
	int width, height;
	bool sibal = false;
	bool shoot = false;
	float dy, dz;
	int dx=0;

private:
	FrustumClass* m_Frustum =  nullptr;
	ShaderManagerClass* m_Shader = nullptr;
	LightClass* m_Light = nullptr;


private:
	TerrainClass* m_Terrain= nullptr;
	TerrainShaderClass* m_TerrainShader= nullptr;

	
	WaterClass* m_Water;
	RTTTextureClass* m_RefractionTexture;
	D3DXMATRIX WaterWorldMatrix, WaterRotationMatrix, WaterreflectionViewMatrix;

	SkyClass* m_Sky= nullptr;

private:
	ModelClass* m_cube= nullptr;
	ModelClass* m_wcube = nullptr;
	ModelClass* m_circle = nullptr;
	ModelClass* m_Billboarding = nullptr;
	ModelClass* m_Mark = nullptr;
	ModelClass* m_Tree = nullptr;

	ModelClass* m_House = nullptr;
	bool OffFire = false;

private:
	float fire_frametime, distortionScale, distortionBias;
	D3DXVECTOR3 scrollSpeeds;
	D3DXVECTOR3 scales;
	D3DXVECTOR2 distortion1;
	D3DXVECTOR2 distortion2;
	D3DXVECTOR2 distortion3;

	ModelClass* m_Fire = nullptr;
	ModelClass* m_TFire = nullptr;
	ModelClass* m_TFire2 = nullptr;
	bool renderFire = false;
	bool renderTFire = false;
	bool renderTFire2 = false;


private:
	D3DXVECTOR3 horsePos= { 333.0f, 20.0f, 349.0f };
	D3DXVECTOR3 horse2Pos= { 235.0f, 10.0f, 500.0f };
	ModelScene* m_horse= nullptr;
	ModelScene* m_horse2 = nullptr;
	ModelScene* m_npc = nullptr;
	bool renderhorse = false;
	bool renderhorse2 = false;
	bool renderNpc = false;
	bool starthorse = false;
	bool starthorse2 = false;

private:
	ImageClass* m_Start= nullptr;
	ImageClass* m_Loading= nullptr;
	ImageClass* m_CrossHair= nullptr;

	float MousePosX, MousePosY;
	ImageClass* m_MouseCursor= nullptr;

private:
	ImageClass* m_Balloon = nullptr;
	bool Balloon1 = false, Balloon2 = false, Balloon3 = false;

	#define Shoot0 ( Balloon1==false && Balloon2 == false && Balloon3 == false)
	#define Shoot1 ( Balloon1==true && Balloon2==false && Balloon3 ==false)
	#define Shoot2 ( Balloon1==true && Balloon2==true && Balloon3 ==false)
	#define Shoot3 ( Balloon1==true && Balloon2==true && Balloon3 ==true)

private:
	ImageClass* m_UI =nullptr;

private:
	//집에 있는 나무
	InstancingClass* m_Instancing = nullptr;
	InstancingClass* m_Instancing2 = nullptr;
	bool OffTFire = false;

	InstancingClass* m_Instancing3 = nullptr;
	InstancingClass* m_Instancing4 = nullptr;
	bool OffTFire2 = false;


	InstancingClass* m_Instancing5 = nullptr;
	InstancingClass* m_Instancing6 = nullptr;

	//강가에 있는 나무
	InstancingClass* m_Instancing7 = nullptr;
	InstancingClass* m_Instancing8 = nullptr;
	InstancingClass* m_Instancing9 = nullptr;
	InstancingClass* m_Instancing10 = nullptr;


	MinimapClass* m_Minimap = nullptr;

private:
	TextClass* m_Text = nullptr;

private:
	ParticleSystem* m_Particle = nullptr;

private:


private:
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX ImageViewMatrix, ImageProjectionMatrix;
	D3DXMATRIX cupViewMatrix;

private:
	D3DXVECTOR3 CameraPos;



};

