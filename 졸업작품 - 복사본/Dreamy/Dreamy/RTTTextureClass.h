#pragma once
#ifndef _RTTTEXTURECLASS_H_
#define _RTTTEXTURECLASS_H_

#include "stdafx.h"

/*
	RTT�ؽ�ó�� �׸���.

*/

class RTTTextureClass
{
private:

	// 3D��ü�� Z���۸� ���� �׷����°��̱� ������ UV��ǥ�� �� ��ǥ�� �׷���� �Ѵ�.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	RTTTextureClass();
	RTTTextureClass(const RTTTextureClass&);
	~RTTTextureClass();

	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);
	int GetIndexCount();

	//**�ؽ��Ĵ� ���� �ؽ�����ü�� ���������ο� ������ �ʴ´�.
	//**���̴°� ResourceView���·� ���δ�!
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);


private:

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;


	/*
	3D�𵨰��� �޸� 2D�̹����� ȭ�� ũ��, �̹��� ũ��, ������ �׷����� ��ġ�� ����ؾ� �Ѵ�.

	������ �׷����� ��ġ�� �˾ƾ� �ϴ� ������
	���� ���� �����Ӱ� ���ؼ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʾƵ� �Ǳ� ������
	������ ����� ���� �� �ֱ� �����̴�!
	*/
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;



};
#endif