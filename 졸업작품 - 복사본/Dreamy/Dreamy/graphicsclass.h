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


#include "../Dreamy/System/ShaderManagerClass.h"
#include "../Dreamy/ObjModel/ModelClass.h"
#include "../Dreamy/System/D3D.h"
#include "FBXShader.h"
#include "../Dreamy/FBXAnimation/ModelScene.h"
#include "../Dreamy/ObjModel/TerrainClass.h"
#include "TerrainShaderClass.h"
#include "../Dreamy/ObjModel/LightClass.h"
#include "../Dreamy/ObjModel/FrustumClass.h"
#include "SkyClass.h"
#include "../Dreamy/System/Rasterizer.h"
#include "../Dreamy/System/Blender.h"
#include "../Dreamy/ObjModel/ImageClass.h"
#include "../Dreamy/ObjModel/TextClass.h"
#include "../Dreamy/ObjModel/InstancingClass.h"
#include "MinimapClass.h"
#include "../Dreamy/ObjModel/ParticleSystem.h"
#include "../Dreamy/ObjModel/WaterClass.h"
#include "../Dreamy/ObjModel/RTTTextureClass.h"
#include "UIManagerClass.h"
//#include "TerrainShaderClass2.h"

#include "../Dreamy/System/FpsClass.h"




/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
//수직 동기화(fps고정)
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	FpsClass* m_Timer = 0;
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	bool Loading(int, int, HWND);

	void Shutdown();
	bool Frame(int, float, D3DXVECTOR3, D3DXVECTOR3, int, int, bool);// + 카메라 회전
	bool Render();
	void CheckIntersection(int, int, int, int);


	bool RenderMainScene(int, int);
	bool RenderLoadingScene();
	bool RenderEndScene();

	bool End();


public:
	bool click = false;
	int  n_click = 0;
	bool isEffectSound = false;

private:
	bool isEnd = false; //게임 클리어인지 아닌지.
	int  m_endingTime = 0; // 불끄는데 걸린 시간

private:
	bool FBXLoading();
	bool OBJLoading(HWND);
	bool LandLoading(HWND);
	bool InstancingLoading(HWND);
	bool ImageLoading(HWND);
	bool EffectLoading(HWND);
	bool OtherLoading(HWND);

	void PreShutdown();
	bool RenderRunningScene();
	bool RenderRefractionToTexture(); // 호수 굴절 텍스처
	bool RenderShadowToTexture(); // 그림자맵

	float CalculateBillboarding(D3DXVECTOR3,D3DXVECTOR3);

private:
	//물풍선 관련.
	D3DXMATRIX BalloonWorldMatrix, BalloonViewMatrix, ShootMatrix;

	D3DXVECTOR3 vGravity = { 0.0f, -11.8f, 0.0f };
	D3DXVECTOR3 vAccell = { 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3 vPosition = { 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3 vVelocity = { 0.0f,3.0f,1.5f };
	int width=0, height=0;
	bool shoot = false;
	float dy =0.0f, dz=0.0f;
	int dx=0;

	void Shoot(float);
	void BalloonFrame();

	//물풍선 UI관련
	ImageClass* m_Balloon = nullptr;
	bool Balloon1 = false, Balloon2 = false, Balloon3 = false;

	#define Shoot0 ( Balloon1==false && Balloon2 == false && Balloon3 == false)
	#define Shoot1 ( Balloon1==true && Balloon2==false && Balloon3 ==false)
	#define Shoot2 ( Balloon1==true && Balloon2==true && Balloon3 ==false)
	#define Shoot3 ( Balloon1==true && Balloon2==true && Balloon3 ==true)

private:
	FrustumClass* m_Frustum =  nullptr;
	ShaderManagerClass* m_Shader = nullptr;
	LightClass* m_Light = nullptr;

private:
	TerrainClass* m_Terrain= nullptr;
	TerrainShaderClass* m_TerrainShader= nullptr;
	//TerrainShaderClass2* m_TerrainShader2 = nullptr;

	WaterClass* m_Water = nullptr;
	D3DXMATRIX WaterWorldMatrix, WaterRotationMatrix, WaterreflectionViewMatrix;

	SkyClass* m_Sky= nullptr;
private:
	RTTTextureClass* m_RefractionTexture =nullptr;
	RTTTextureClass* m_ShadowMap = nullptr;

private:
	//OBJ모델
	ModelClass* m_NpcBox= nullptr; //npc충돌박스
	ModelClass* m_WaterBox = nullptr; //호수충돌박스
	ModelClass* m_circle = nullptr;
	ModelClass* m_Billboarding = nullptr;
	ModelClass* m_Mark = nullptr;
	ModelClass* m_Tree = nullptr;
	ModelClass* m_Crate = nullptr;
	ModelClass* m_Crate2 = nullptr;

	ModelClass* m_Barrel = nullptr;
	ModelClass* m_Barrel2 = nullptr;
	ModelClass* m_Barrel3 = nullptr;
	bool OffBFire = false;

	ModelClass* m_House = nullptr;
	bool OffFire = false;

private:
	//불
	void SetEffectVariable(float frametime); //이펙트 관련 변수 세팅
	float fire_frametime, distortionScale, distortionBias;
	D3DXVECTOR3 scrollSpeeds;
	D3DXVECTOR3 scales;
	D3DXVECTOR2 distortion1;
	D3DXVECTOR2 distortion2;
	D3DXVECTOR2 distortion3;

	ModelClass* m_Fire = nullptr; //집불
	ModelClass* m_TFire = nullptr;  //인스턴싱 1,2불
	ModelClass* m_TFire2 = nullptr; //인스턴싱 3,4불
	ModelClass* m_TFire3 = nullptr; //인스턴싱 5,6불
	ModelClass* m_TFire4 = nullptr; //인스턴싱 9,10불
	ModelClass* m_BFire = nullptr; // Barrel에 붙은 불

	bool renderFire = false;
	bool renderTFire = false;
	bool renderTFire2 = false;
	bool renderTFire3 = false;
	bool renderTFire4 = false;
	bool renderBFire = false;

private:
	D3DXVECTOR3 horsePos= { 333.0f, 20.0f, 349.0f };
	D3DXVECTOR3 horse2Pos= { 235.0f, 10.0f, 500.0f };
	D3DXVECTOR3 horse3Pos = { 670.0f, 51.0f, 464.0f };

	ModelScene* m_horse= nullptr;
	ModelScene* m_horse2 = nullptr;
	ModelScene* m_horse3 = nullptr;
	ModelScene* m_npc = nullptr;
	
	//프러스텀 컬링을 위한 bool변수들
	bool renderhorse = false;
	bool renderhorse2 = false;
	bool renderhorse3 = false;
	bool renderNpc = false;

	//플레이어의 시선안에 있어야 애니메이션이 시작되게 하기 위한 bool 변수들.
	bool starthorse = false;
	bool starthorse2 = false;
	bool starthorse3 = false;

	//움직임 제어 함수
	void HorseFrame(float);

private:
	//2D화면들
	ImageClass* m_Start= nullptr;
	ImageClass* m_Loading= nullptr;
	ImageClass* m_Ending = nullptr;
	ImageClass* m_CrossHair= nullptr;
	UIManagerClass* m_UIManager = nullptr;
	
	float MousePosX, MousePosY;
	ImageClass* m_MouseCursor= nullptr;
	MinimapClass* m_Minimap = nullptr;
	TextClass* m_Text = nullptr;

private:
	//집에 있는 나무
	InstancingClass* m_Instancing = nullptr;
	InstancingClass* m_Instancing2 = nullptr;
	bool OffTFire = false;

	//집의 왼쪽에 있는 나무
	InstancingClass* m_Instancing3 = nullptr;
	InstancingClass* m_Instancing4 = nullptr;
	bool OffTFire2 = false;

	//집의 오른쪽에 있는 나무
	InstancingClass* m_Instancing5 = nullptr;
	InstancingClass* m_Instancing6 = nullptr;
	bool OffTFire3 = false;

	//Barrel쪽나무
	InstancingClass* m_Instancing9 = nullptr;
	InstancingClass* m_Instancing10 = nullptr;
	bool OffTFire4 = false;

	//강가에 있는 나무
	InstancingClass* m_Instancing7 = nullptr;
	InstancingClass* m_Instancing8 = nullptr;

	InstancingClass* m_Instancing11 = nullptr;
	InstancingClass* m_Instancing12 = nullptr;

	InstancingClass* m_Instancing13 = nullptr;
	InstancingClass* m_Instancing14 = nullptr;

private:
	ParticleSystem* m_Particle = nullptr;
	//파티클시스템 1개만 생성하고 모든 불에 써먹기 위해 bool변수로 위치를 조정한다.
	bool Fparticle = false;
	bool TFparticle = false;
	bool TF2particle = false;
	bool TF3particle = false;
	bool TF4particle = false;
	bool BFparticle = false;

private:
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX ImageViewMatrix, ImageProjectionMatrix;
	D3DXMATRIX minimapMatrix; // 미니맵 
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;

private:
	D3DXVECTOR3 CameraPos;



};

