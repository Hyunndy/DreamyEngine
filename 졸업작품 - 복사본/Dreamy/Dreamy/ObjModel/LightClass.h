////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "../stdafx.h"

/*
	빛의 방향과 색상을 관리하는 클래스

	18.01.18
	방향광만!
*/


////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	static LightClass* Get();
	static void Delete();

	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);


	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);

private:
	static LightClass* instance;

	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
};

#endif