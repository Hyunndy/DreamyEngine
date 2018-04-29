#pragma once
#include "ImageClass.h"
#include "TextureShaderClass.h"

class UIManagerClass

{
public:
	UIManagerClass();
	~UIManagerClass();

	void Initialize(int, int);
	void Shutdown();
	void renderUI(TextureShaderClass* Shader, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void active();
	void unactive();

	ImageClass* m_UI[5];
private:
	bool render[5];


};

