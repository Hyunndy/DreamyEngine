////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "D3D.h"

/*
	�츮�� ��� ����� �������� ���� ������ DirectX11���� �����ϴ� ī�޶� Ŭ����.
	ī�޶� Ŭ������ ī�޶��� ��ġ, ���� ȸ�� ���¸� ��� ������ �Ѵ�.
	�� ������ �̿��Ͽ� �������ÿ� HLSL���̴����� ����� �� ����� �����Ѵ�.

	18.02.09 �ݻ� ���� ���� ��
	��� �ݻ縦 �ٷ�� ���� �߰� ���� ����.

	18.03.14 Water ���� ���� ��
	reflection view matrix�� �����Ѵ�.

	������� ����� ���� �ʿ��� �� = ī�޶��� ��ġ+ī�޶� ���� ����+ī�޶��� up����

*/

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(D3DXVECTOR3);
	void SetRotation(D3DXVECTOR3);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	//ī�޶��� ��ġ�� ȸ�� ���¿� ����� �� ����� �����Ѵ�.
	void Render();

	//���̴����� �������� ����� �� �ֵ��� ī�޶� ��ü�� �� ����� �޾ƿ´�.
	void GetViewMatrix(D3DXMATRIX&);

	//�ſ� ��� �ݻ縦 ���� �� �Լ�.
	//void RenderReflection(float, float);
	void RenderReflection(float);

	//�� ��� �ݻ縦 ���� �Լ�
	void RenderWaterReflection(float);

	void GenerateBaseViewMatrix();
	void GetBaseViewMatrix(D3DXMATRIX&);

	void GetWaterReflectionViewMatrix(D3DXMATRIX&);
	D3DXMATRIX GetReflectionViewMatrix();

	void UpdateProjectionMatrix();
	D3DXMATRIX GetProjectionMatrix();

private:
	const static float screenNear; //�Ÿ�
	const static float screenDepth; //����
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	D3DXVECTOR3 m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXMATRIX m_viewMatrix, m_baseViewMatrix, m_reflectionViewMatrix;
	D3DXMATRIX m_WaterReflectionViewMatrix;

	D3D11_VIEWPORT viewport;
	float fieldOfView; //�þ߰�
	float screenAspect; //ȭ�����

	D3DXMATRIX projection;
};

#endif