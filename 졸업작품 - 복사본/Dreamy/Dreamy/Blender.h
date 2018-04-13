#pragma once
#include "D3D.h"

class Blender
{
public:
	static Blender* Get();
	static void Delete();

	void SetLinear();
	void SetOff();
	void SetAdd();

private:
	static Blender* instance;

	Blender();
	~Blender();

	ID3D11BlendState* linearState;
	ID3D11BlendState* offState;
	ID3D11BlendState* AddState;
};