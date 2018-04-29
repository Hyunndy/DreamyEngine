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
	bool Frame(int, float, D3DXVECTOR3, D3DXVECTOR3, int, int, bool);// + ī�޶� ȸ��
	bool Render(bool);
	void CheckIntersection(int, int, int, int);
	void Shoot(float);

	bool RenderMainScene();
	bool RenderLoadingScene();
	bool RenderEndScene();

	bool End();

	//Picking �˻�
	//void TestIntersection(int, int, int, int);
	//bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius);
public:
	bool F1pressed;
	D3DXMATRIX minimapMatrix; // �̴ϸ� 
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
	bool RenderRefractionToTexture(bool); // ȣ�� ���� �ؽ�ó


	void SetEffectVariable(float frametime); //����Ʈ ���� ���� ����
	float CalculateBillboarding(D3DXVECTOR3,D3DXVECTOR3);

private:
	bool m_end = false; //���� Ŭ�������� �ƴ���.
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
	//���� �ִ� ����
	InstancingClass* m_Instancing = nullptr;
	InstancingClass* m_Instancing2 = nullptr;
	bool OffTFire = false;

	InstancingClass* m_Instancing3 = nullptr;
	InstancingClass* m_Instancing4 = nullptr;
	bool OffTFire2 = false;


	InstancingClass* m_Instancing5 = nullptr;
	InstancingClass* m_Instancing6 = nullptr;

	//������ �ִ� ����
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

