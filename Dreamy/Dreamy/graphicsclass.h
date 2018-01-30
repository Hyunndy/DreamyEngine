////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

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




/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
//���� ����ȭ(fps����)
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
	//bool Frame(int, int); ���콺 ������ �̵� ���
	//bool Frame(int, int, float); // fps, cpu, frametime�� ���
	bool Frame(int, int, float, float);// + ī�޶� ȸ��
	bool Render(bool, float, float);
	//bool Render(bool);

public:
	bool F1pressed;
	D3DXMATRIX baseViewMatrix; //�۲��� �׻� ���� ��ġ�� ��µǰ� 
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