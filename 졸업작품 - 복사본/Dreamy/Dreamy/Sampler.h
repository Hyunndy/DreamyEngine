#pragma once
#include "D3D.h"

class Sampler
{
public:
	static Sampler* Get();
	static void Delete();

	void SetDefault();

private:
	static Sampler* instance;

	Sampler();
	~Sampler();

	ID3D11SamplerState* defaultState;
};