#include "PointLightClass.h"



PointLightClass::PointLightClass()
{
}

PointLightClass::PointLightClass(const PointLightClass& other)
{
}

PointLightClass::~PointLightClass()
{
}


void PointLightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR4(x, y, z, 1.0f);
	return;
}

D3DXVECTOR4 PointLightClass::GetPosition()
{
	return m_position;
}