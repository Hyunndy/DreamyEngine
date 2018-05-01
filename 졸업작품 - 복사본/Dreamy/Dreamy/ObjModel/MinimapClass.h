#pragma once

#include "ImageClass.h"
#include "TextureShaderClass.h"

class MinimapClass
{
public:
	MinimapClass();
	MinimapClass(const MinimapClass&);
	~MinimapClass();

	bool Initialize( int, int, float, float);
	void Shutdown();
	bool Render( TextureShaderClass* ShaderManager, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX orthoMatrix);

	void PositionUpdate(float, float);

public:
	ImageClass *m_MiniMapBitmap, *m_PointBitmap;

private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;



};

