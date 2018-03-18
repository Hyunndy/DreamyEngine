#include "MoveClass.h"



MoveClass::MoveClass()
{
	m_frameTime=0;

	m_leftTurnSpeed=0;
	m_rightTurnSpeed=0;

	m_ForwardSpeed=0;
	m_backwardSpeed=0;
	m_upwardSpeed=0;
	m_downwardSpeed=0;
	m_lookUpSpeed=0;
	m_lookDownSpeed=0;
}

MoveClass::MoveClass(const MoveClass& other)
{
}


MoveClass::~MoveClass()
{
}

void MoveClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

}

void MoveClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void MoveClass::GetPosition(D3DXVECTOR3& pos)
{
	pos = m_position;
}

void MoveClass::GetRotation(D3DXVECTOR3& rot)
{
	rot = m_rotation;
}

/*------------------------------------------------------------------------------------------------------------------
이름 : SetFrameTime()
용도 :
- 프레임 속도를 설정하기 위해 사용된다.
- 이 프레임 시간을 사용하여 카메라가 얼마나 빠르게 움직이고 회전하는지 계산한다.
- 카메라가 움직이기 전에 매 프레임의 시작에 불리도록 해야한다.
--------------------------------------------------------------------------------------------------------------------*/
void MoveClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}



/*------------------------------------------------------------------------------------------------------------------
이름 : Turn()
용도 :
- 움직임 관련 함수, 매 프레임 마다 호출된다.
- 키가 눌린다면 매 프레임 마다 최고 속도까지 해당 방향으로 가속된다.
- 키를 떼면 매 프레임 마다 속도가 일정한 비율로 부드럽게 줄어들어 0이 된다.
- 프레임간 시간에 기초해 있기 때문에 fps값과 상관없이 동일하게 움직인다(cpu에 종속이 아니다)
--------------------------------------------------------------------------------------------------------------------*/
void MoveClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		//m_leftTurnSpeed += m_frameTime * 0.01f;

		//if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		//{
			m_leftTurnSpeed = m_frameTime * 0.13f;
		//}
	}

	else
	{
		//m_leftTurnSpeed -= m_frameTime* 0.005f;

		//if (m_leftTurnSpeed < 0.0f)
		//{
			m_leftTurnSpeed = 0.0f;
		//}
	}

	m_rotation.y -= m_leftTurnSpeed;

	//if (m_rotation.y < 0.0f)
	//{
	//	m_rotation.y += 360.0f;
	//}

}

void MoveClass::TurnRight(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right. If not slow down the turn speed.
	if (keydown)
	{
		//m_rightTurnSpeed += m_frameTime * 0.01f;

		//if (m_rightTurnSpeed > (m_frameTime * 0.15f))

		//{
			m_rightTurnSpeed = m_frameTime * 0.13f;
		//}
	}

	else
	{
		//m_rightTurnSpeed -= m_frameTime* 0.005f;
		//if (m_rightTurnSpeed < 0.0f)
		//{
			m_rightTurnSpeed = 0.0f;
		//}
	}

	m_rotation.y += m_rightTurnSpeed;

	//if (m_rotation.y > 360.0f)
	//{
	//	m_rotation.y -= 360.0f;
	//}
}

void MoveClass::LookUpward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right. If not slow down the turn speed.
	if (keydown)
	{
		//m_lookUpSpeed += m_frameTime * 0.01f;

		//if (m_lookUpSpeed > (m_frameTime * 0.15f))

		//{
			m_lookUpSpeed = m_frameTime * 0.15f;
		//}
	}

	else
	{
		//m_lookUpSpeed -= m_frameTime* 0.005f;
		//if (m_lookUpSpeed < 0.0f)
		//{
			m_lookUpSpeed = 0.0f;
		//}
	}

	m_rotation.x -= m_lookUpSpeed;

	if (m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}
}

void MoveClass::LookDownward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right. If not slow down the turn speed.
	if (keydown)
	{
		//m_lookDownSpeed += m_frameTime * 0.01f;

		//if (m_lookDownSpeed > (m_frameTime * 0.15f))

		//{
			m_lookDownSpeed = m_frameTime * 0.15f;
		//}
	}

	else
	{
		//m_lookDownSpeed -= m_frameTime* 0.005f;
		//if (m_lookDownSpeed < 0.0f)
		//{
			m_lookDownSpeed = 0.0f;
		//}
	}

	m_rotation.x += m_lookDownSpeed;

	if (m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}
}

void MoveClass::GoForward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_ForwardSpeed += m_frameTime * 0.01f;

		if (m_ForwardSpeed > (m_frameTime * 0.05f))
		{
			m_ForwardSpeed = m_frameTime * 0.05f;
		}
	}

	else
	{
		m_ForwardSpeed -= m_frameTime* 0.0007f;

		if (m_ForwardSpeed < 0.0f)
		{
			m_ForwardSpeed = 0.0f;
		}
	}

	// 값을 라디안으로 변환한다.
	float radians = m_rotation.y * 0.0174532925f;

	//위치를 업데이트 한다.
	m_position.x += sinf(radians) * m_ForwardSpeed;
	m_position.z += cosf(radians) * m_ForwardSpeed;


	return;
}

void MoveClass::GoBackward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_backwardSpeed += m_frameTime * 0.01f;

		if (m_backwardSpeed  > (m_frameTime * 0.05f))
		{
			m_backwardSpeed = m_frameTime * 0.05f;
		}
	}

	else
	{
		m_backwardSpeed -= m_frameTime* 0.007f;

		if (m_backwardSpeed  < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// 값을 라디안으로 변환한다.
	float radians = m_rotation.y * 0.0174532925f;

	//위치를 업데이트 한다.
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;

}

