#pragma once
#include "LightClass.h"


class PointLightClass :
	public LightClass
{
public:
	PointLightClass();
	PointLightClass(const PointLightClass&);
	~PointLightClass();

	void SetPosition(float, float, float);
	D3DXVECTOR4 GetPosition();

private:
	D3DXVECTOR4 m_position;
};

