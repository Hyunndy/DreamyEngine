#pragma once
#ifndef _FRUSTUMCLASS_H_ 
#define _FRUSTUMCLASS_H_

/*
이름: FrustumClass
용도: 
- 시야 절두체 생성.
- 시야에 보이는 부분만 렌더링 할 수 있게 도와줘서 렌더링 성능을 크게 향상 시킬 수 있다.

*/

#include <D3DX10math.h>
#include <DirectXMath.h>
using namespace DirectX;


class FrustumClass
{
public:
	FrustumClass();
	FrustumClass(const FrustumClass&);
	~FrustumClass();

	void ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);


	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);
	bool CheckRectangle2(float, float, float, float, float, float);

private: 
	D3DXPLANE m_planes[6];


};

#endif
