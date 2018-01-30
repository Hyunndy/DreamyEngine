////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// graphicsclass는 이 어플리키에션의 모든 그래픽 기능이 이 객체안에 캡슐화되어 있다.
// 나중에 모든 그래픽 객체들을 담을것이다. -> 여기서 그래픽 객체들을 또 클래스로 묶으면..?

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




/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
//수직 동기화(fps고정)
const bool VSYNC_ENABLED = false;
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
	void Shutdown();
	//bool Frame(int, int); 마우스 포인터 이동 출력
	//bool Frame(int, int, float); // fps, cpu, frametime만 출력
	bool Frame(int, int, float, float);// + 카메라 회전
	bool Render(bool, float, float);
	//bool Render(bool);

public:
	bool F1pressed;
	D3DXMATRIX baseViewMatrix; //글꼴이 항상 같은 위치에 출력되게 
private:



private:

	D3DClass* m_D3D; 
	CameraClass* m_Camera;
	ShaderManagerClass* m_Shader;
	LightClass* m_Light;

	ModelClass* m_Model_Circle;

	ModelClass* m_Model_Plane;
	ModelClass* m_Model_Cube;
	ModelClass* m_Model_Cube2;

	ModelManagerClass* m_Model_CircleList;

	ImageClass* m_2D_Love;

	TextClass* m_Title;

	FrustumClass* m_Frustum;
};

#endif