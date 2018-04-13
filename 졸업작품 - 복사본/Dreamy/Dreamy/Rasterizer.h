#pragma once
#include "D3D.h"

/*
- 이제 OMSetRenderTarget함수를 호출할 수 있다.
- 이 함수는 렌더타겟뷰와 깊이-스텐실 뷰를 출력 렌더링 파이프라인에 바인딩시킨다.

- 이렇게 하여 파이프라인을 이용한 렌더링이 수행될 때 우리가 만들었던 백버퍼에 그려지게 된다.
- 그리고 백버퍼에 그려진것을 프론트 버퍼와 바꿔치기하여 유저의 모니터에 출력하게 된다.

- 렌더 타겟이 설정된 뒤 추가적인 기능을 설정할 수 있다.

1. 래스터화기상태(rasterizer state)
- 도형이 어떻게 그려지는지에 대한 제어를 가질 수 있게 한다.
- 이를 이용해 우리의 화면을 와이어프레임 모드로 그리거나 도형의 앞뒷면을 모두 그리도록 할 수도 있다.
- 정점 셰이더(혹은 기하 셰이더)가 공급한 정점 특성들을 삼각형의 픽셀들에 따라 보간한 결과를 픽셀셰이더에 도달하기까지의 경로를 말한다.
*/

class Rasterizer
{
public:
	static Rasterizer* Get();
	static void Delete();

	void SetSolid();
	void SetWireframe();

	void SetOnCullMode();
	void SetOffCullMode();

private:
	static Rasterizer* instance;

	Rasterizer();
	~Rasterizer();

	ID3D11RasterizerState* solidState;
	ID3D11RasterizerState* wireframeState;
	ID3D11RasterizerState* onCullState;
	ID3D11RasterizerState* offCullState;
};