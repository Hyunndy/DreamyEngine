#pragma once

class ModelMaterial
{
public:
	ModelMaterial(int number, FbxSurfaceMaterial* material);
	~ModelMaterial();

	ID3D11ShaderResourceView* GetDiffuseView()
	{
		return diffuseView;
	}

private:
	D3DXCOLOR GetProperty
	(
		FbxSurfaceMaterial* material
		, const char* name, const char* factorName
		, wstring* textureName
	);

	void CreateView(wstring path, ID3D11ShaderResourceView** view);

	UINT number;
	D3DXCOLOR ambient;
	D3DXCOLOR emissive;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float shininess;

	wstring ambientFile;
	wstring emissiveFile;
	wstring diffuseFile;
	wstring specularFile;

	ID3D11ShaderResourceView* ambientView;
	ID3D11ShaderResourceView* emissiveView;
	ID3D11ShaderResourceView* diffuseView;
	ID3D11ShaderResourceView* specularView;
};