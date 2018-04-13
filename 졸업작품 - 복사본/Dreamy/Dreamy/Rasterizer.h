#pragma once
#include "D3D.h"

/*
- ���� OMSetRenderTarget�Լ��� ȣ���� �� �ִ�.
- �� �Լ��� ����Ÿ�ٺ�� ����-���ٽ� �並 ��� ������ ���������ο� ���ε���Ų��.

- �̷��� �Ͽ� ������������ �̿��� �������� ����� �� �츮�� ������� ����ۿ� �׷����� �ȴ�.
- �׸��� ����ۿ� �׷������� ����Ʈ ���ۿ� �ٲ�ġ���Ͽ� ������ ����Ϳ� ����ϰ� �ȴ�.

- ���� Ÿ���� ������ �� �߰����� ����� ������ �� �ִ�.

1. ������ȭ�����(rasterizer state)
- ������ ��� �׷��������� ���� ��� ���� �� �ְ� �Ѵ�.
- �̸� �̿��� �츮�� ȭ���� ���̾������� ���� �׸��ų� ������ �յ޸��� ��� �׸����� �� ���� �ִ�.
- ���� ���̴�(Ȥ�� ���� ���̴�)�� ������ ���� Ư������ �ﰢ���� �ȼ��鿡 ���� ������ ����� �ȼ����̴��� �����ϱ������ ��θ� ���Ѵ�.
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