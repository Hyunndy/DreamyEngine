////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

/*
	�츮�� ��� ����� �������� ���� ������ DirectX11���� �����ϴ� ī�޶� Ŭ����.
	ī�޶� Ŭ������ ī�޶��� ��ġ, ���� ȸ�� ���¸� ��� ������ �Ѵ�.
	�� ������ �̿��Ͽ� �������ÿ� HLSL���̴����� ����� �� ����� �����Ѵ�.

	18.02.09 �ݻ� ���� ���� ��
	��� �ݻ縦 �ٷ�� ���� �߰� ���� ����.

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

	//��� �ݻ縦 ���� �� �Լ�.
	void RenderReflection(float, float);
	D3DXMATRIX GetReflectionViewMatrix();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	D3DXVECTOR3 m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_reflectionViewMatrix;
};

#endif