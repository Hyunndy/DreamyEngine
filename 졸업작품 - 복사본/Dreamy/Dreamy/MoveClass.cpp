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
�̸� : SetFrameTime()
�뵵 :
- ������ �ӵ��� �����ϱ� ���� ���ȴ�.
- �� ������ �ð��� ����Ͽ� ī�޶� �󸶳� ������ �����̰� ȸ���ϴ��� ����Ѵ�.
- ī�޶� �����̱� ���� �� �������� ���ۿ� �Ҹ����� �ؾ��Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
void MoveClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}



/*------------------------------------------------------------------------------------------------------------------
�̸� : Turn()
�뵵 :
- ������ ���� �Լ�, �� ������ ���� ȣ��ȴ�.
- Ű�� �����ٸ� �� ������ ���� �ְ� �ӵ����� �ش� �������� ���ӵȴ�.
- Ű�� ���� �� ������ ���� �ӵ��� ������ ������ �ε巴�� �پ��� 0�� �ȴ�.
- �����Ӱ� �ð��� ������ �ֱ� ������ fps���� ������� �����ϰ� �����δ�(cpu�� ������ �ƴϴ�)
--------------------------------------------------------------------------------------------------------------------*/
void MoveClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.005f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
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
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))

		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.005f;
		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
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
		m_lookUpSpeed += m_frameTime * 0.01f;

		if (m_lookUpSpeed > (m_frameTime * 0.15f))

		{
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_lookUpSpeed -= m_frameTime* 0.005f;
		if (m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
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
		m_lookDownSpeed += m_frameTime * 0.01f;

		if (m_lookDownSpeed > (m_frameTime * 0.15f))

		{
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_lookDownSpeed -= m_frameTime* 0.005f;
		if (m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
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
		m_ForwardSpeed += m_frameTime * 0.001f;

		if (m_ForwardSpeed > (m_frameTime * 0.03f))
		{
			m_ForwardSpeed = m_frameTime * 0.03f;
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

	// ���� �������� ��ȯ�Ѵ�.
	float radians = m_rotation.y * 0.0174532925f;

	//��ġ�� ������Ʈ �Ѵ�.
	m_position.x += sinf(radians) * m_ForwardSpeed;
	m_position.z += cosf(radians) * m_ForwardSpeed;


	return;
}

void MoveClass::GoBackward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_backwardSpeed += m_frameTime * 0.001f;

		if (m_backwardSpeed  > (m_frameTime * 0.03f))
		{
			m_backwardSpeed = m_frameTime * 0.03f;
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

	// ���� �������� ��ȯ�Ѵ�.
	float radians = m_rotation.y * 0.0174532925f;

	//��ġ�� ������Ʈ �Ѵ�.
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;

}

