////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


/*
�̸�: GraphicsClass
�뵵: 
- �� App�� ��� �׷��� ��ҵ��� ĸ��ȭ �Ǿ� �ִ�.
- ������(Billboarding)
 *�� �Ÿ��� �ִ� 3D������ ǥ���ϱ� ���� �ؽ�ó�� ���� �簢���� ����ϴ°�. 
 *�ǽð� �������� �ϱ⿣ �׷��� ī���� ���뼺�� ������ ������, ���� ����� �ֵ��� �������� ������-�ָ� �ִ� �ֵ��� ���� �������� ����ϴ� �����尡 �� �� �ִ�.
 *�����带 �����ϴ� ����鰣�� �������� �簢���� ��� ȸ���ϴ����� ���� �޷��ִ�.
 *�� App���� ������ ��ġ�� ���� ������ �׻� ���ֺ��� �ϴ� ����� ����Ѵ�.


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
//���� ����ȭ(fps����)
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
	bool Frame(int, float, D3DXVECTOR3, D3DXVECTOR3, int, int, bool);// + ī�޶� ȸ��
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
	bool isEnd = false; //���� Ŭ�������� �ƴ���.
	int  m_endingTime = 0; // �Ҳ��µ� �ɸ� �ð�

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
	bool RenderRefractionToTexture(); // ȣ�� ���� �ؽ�ó
	bool RenderShadowToTexture(); // �׸��ڸ�

	float CalculateBillboarding(D3DXVECTOR3,D3DXVECTOR3);

private:
	//��ǳ�� ����.
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

	//��ǳ�� UI����
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
	//OBJ��
	ModelClass* m_NpcBox= nullptr; //npc�浹�ڽ�
	ModelClass* m_WaterBox = nullptr; //ȣ���浹�ڽ�
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
	//��
	void SetEffectVariable(float frametime); //����Ʈ ���� ���� ����
	float fire_frametime, distortionScale, distortionBias;
	D3DXVECTOR3 scrollSpeeds;
	D3DXVECTOR3 scales;
	D3DXVECTOR2 distortion1;
	D3DXVECTOR2 distortion2;
	D3DXVECTOR2 distortion3;

	ModelClass* m_Fire = nullptr; //����
	ModelClass* m_TFire = nullptr;  //�ν��Ͻ� 1,2��
	ModelClass* m_TFire2 = nullptr; //�ν��Ͻ� 3,4��
	ModelClass* m_TFire3 = nullptr; //�ν��Ͻ� 5,6��
	ModelClass* m_TFire4 = nullptr; //�ν��Ͻ� 9,10��
	ModelClass* m_BFire = nullptr; // Barrel�� ���� ��

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
	
	//�������� �ø��� ���� bool������
	bool renderhorse = false;
	bool renderhorse2 = false;
	bool renderhorse3 = false;
	bool renderNpc = false;

	//�÷��̾��� �ü��ȿ� �־�� �ִϸ��̼��� ���۵ǰ� �ϱ� ���� bool ������.
	bool starthorse = false;
	bool starthorse2 = false;
	bool starthorse3 = false;

	//������ ���� �Լ�
	void HorseFrame(float);

private:
	//2Dȭ���
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
	//���� �ִ� ����
	InstancingClass* m_Instancing = nullptr;
	InstancingClass* m_Instancing2 = nullptr;
	bool OffTFire = false;

	//���� ���ʿ� �ִ� ����
	InstancingClass* m_Instancing3 = nullptr;
	InstancingClass* m_Instancing4 = nullptr;
	bool OffTFire2 = false;

	//���� �����ʿ� �ִ� ����
	InstancingClass* m_Instancing5 = nullptr;
	InstancingClass* m_Instancing6 = nullptr;
	bool OffTFire3 = false;

	//Barrel�ʳ���
	InstancingClass* m_Instancing9 = nullptr;
	InstancingClass* m_Instancing10 = nullptr;
	bool OffTFire4 = false;

	//������ �ִ� ����
	InstancingClass* m_Instancing7 = nullptr;
	InstancingClass* m_Instancing8 = nullptr;

	InstancingClass* m_Instancing11 = nullptr;
	InstancingClass* m_Instancing12 = nullptr;

	InstancingClass* m_Instancing13 = nullptr;
	InstancingClass* m_Instancing14 = nullptr;

private:
	ParticleSystem* m_Particle = nullptr;
	//��ƼŬ�ý��� 1���� �����ϰ� ��� �ҿ� ��Ա� ���� bool������ ��ġ�� �����Ѵ�.
	bool Fparticle = false;
	bool TFparticle = false;
	bool TF2particle = false;
	bool TF3particle = false;
	bool TF4particle = false;
	bool BFparticle = false;

private:
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX ImageViewMatrix, ImageProjectionMatrix;
	D3DXMATRIX minimapMatrix; // �̴ϸ� 
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;

private:
	D3DXVECTOR3 CameraPos;



};

