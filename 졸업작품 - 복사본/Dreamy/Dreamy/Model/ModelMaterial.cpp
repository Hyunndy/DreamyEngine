#include "../stdafx.h"
#include "ModelMaterial.h"
#include "ModelScene.h"

ModelMaterial::ModelMaterial(int number, FbxSurfaceMaterial * material)
	: number(number)
	, ambient(0, 0, 0, 1), emissive(0, 0, 0, 1), diffuse(0, 0, 0, 1), specular(0, 0, 0, 1)
	, shininess(0.0f)
	, ambientFile(L""), emissiveFile(L""), diffuseFile(L""), specularFile(L"")
	, ambientView(NULL), emissiveView(NULL), diffuseView(NULL), specularView(NULL)
{
	ambient = GetProperty(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &ambientFile);
	emissive = GetProperty(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &emissiveFile);
	diffuse = GetProperty(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &diffuseFile);
	specular = GetProperty(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &specularFile);

	FbxProperty shininessProp = material->FindProperty(FbxSurfaceMaterial::sShininess);
	if (shininessProp.IsValid())
		shininess = (float)shininessProp.Get<FbxDouble>();


	CreateView(ambientFile, &ambientView);
	CreateView(emissiveFile, &emissiveView);
	CreateView(diffuseFile, &diffuseView);
	CreateView(specularFile, &specularView);
}

ModelMaterial::~ModelMaterial()
{
	SAFE_RELEASE(ambientView);
	SAFE_RELEASE(emissiveView);
	SAFE_RELEASE(diffuseView);
	SAFE_RELEASE(specularView);
}

D3DXCOLOR ModelMaterial::GetProperty(FbxSurfaceMaterial * material, const char * name, const char * factorName, wstring* textureName)
{
	D3DXCOLOR color(0, 0, 0, 1);
	
	FbxProperty prop = material->FindProperty(name);
	FbxProperty factorProp = material->FindProperty(factorName);
	if (prop.IsValid() == true && factorProp.IsValid() == true)
	{
		FbxDouble3 temp = prop.Get<FbxDouble3>();
		double factor = factorProp.Get<FbxDouble>();
		if (factor != 1)
		{
			temp[0] *= factor;
			temp[1] *= factor;
			temp[2] *= factor;
		}

		color.r = (float)temp[0];
		color.g = (float)temp[1];
		color.b = (float)temp[2];
	}

	if (prop.IsValid())
	{
		UINT count = prop.GetSrcObjectCount<FbxFileTexture>();
		if (count > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != NULL)
			{
				//TODO: 상대경로 변경
				string temp = (const char *)texture->GetFileName();
				*textureName = String::StringToWString(temp);
			}
			else
				*textureName = L"";
		}//if(count)
	}//if(prop)

	return color;
}

void ModelMaterial::CreateView(wstring path, ID3D11ShaderResourceView ** view)
{
	if (path.length() < 1) return;

	if (String::Contain(path, L"\\") == true)
		String::Replace(&path, L"\\", L"/");

	wstring filePath = L"";
	String::SplitFilePath(path, NULL, &filePath);
	filePath = ModelScene::TexturePath + filePath;

	DWORD fileValue = GetFileAttributes(filePath.c_str());
	if (fileValue < 0xFFFFFFFF)
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile
		(
			D3D::GetDevice()
			, filePath.c_str()
			, NULL
			, NULL
			, view
			, NULL
		);
		//assert(SUCCEEDED(hr));
	}
}
