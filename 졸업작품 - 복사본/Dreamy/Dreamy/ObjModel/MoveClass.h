#pragma once

#include <d3dx10math.h>

class MoveClass
{
public:
	MoveClass();
	MoveClass(const MoveClass&);
	~MoveClass();

	
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void GetPosition(D3DXVECTOR3&);
	void GetRotation(D3DXVECTOR3&);
	
	void SetFrameTime(float time);

	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);
	void GoForward(bool keydown);
	void GoBackward(bool keydown);
	void LookUpward(bool keydown);
	void LookDownward(bool keydown);


private:

	D3DXVECTOR3 m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	float m_frameTime;

	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_ForwardSpeed, m_backwardSpeed;
	float m_upwardSpeed, m_downwardSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

};

