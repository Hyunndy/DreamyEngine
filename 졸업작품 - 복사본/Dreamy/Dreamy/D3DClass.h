////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


/////////////
// LINKING //
/////////////
/*
�� ���̺귯������ 
DirectX�� ����, 3D�׷����� ������, ���ΰ�ħ ����(FPS) ���, ����ϴ� �׷���ī���� ���� ��� ���� �ϵ������� ����� ���� �ʿ��� ��� Direct3D�� ��ɵ��� ��� �ִ�.
DX10�� ���̺귯���� �״�� ���̴°� �ش� ���̺귯���� �ٲ� �ʿ䰡 ���� �����̴�.
*/
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")


//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include "stdafx.h"


/*--------------------------------------------------------------------------------------------------------------------------------------
��������
- 18.01.23 2D�̹��� �������� ���� Z���۸� �Ѱ� �� �� �ֵ��� ������.
- 18.01.23 Text�������� ���� ���� ���¸� �����ϴ� �Լ��� �߰���
		   ����(blending) : ���ڰ� 3D��ü���� ������� ��ȭ�Ӱ� �׷����� �Ѵ�.
- 18.01.30 RTT����� ����� �� �ִ� �Լ����� �߰���.
- �߰����� ������ �� �ְ� ��
--------------------------------------------------------------------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	// Z���۸� �Ѱ� ���� �Լ�
	void TurnZBufferOn();
	void TurnZBufferOff();

	// ���ĺ����� �Ѱ� ���� �Լ�
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	// �ĸ� �ø��� �Ѱ� ���� �Լ�
	void TurnOnCulling();
	void TurnOffCulling();

	//additive blending
	void EnableSecondBlendState();

	// RTT����� �� �� �ְ� ���ִ� �Լ�.
	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	
private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;

	//z���۸� �Ѱ� ���� ���� ����-���ٽ�
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;

	ID3D11DepthStencilView* m_depthStencilView;


	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;

	//���� ���¸� ǥ���ϴ� ����
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	//additive blending for cloud
	ID3D11BlendState * m_alphaBlendState2;

	//�����Ͷ����� ���� �ĸ� �ø� �Ѱ� ���°�.
	ID3D11RasterizerState* m_rasterState;	
	ID3D11RasterizerState* m_rasterStateNoCulling;
};

#endif