#pragma once
#include "FBXShader.h"

class FBXAnimationModel;
class ModelMaterial;
class ModelBoneWeights;
class ModelBuffer;
class ModelPart : public FBXShader
{
public:
	ModelPart(FBXAnimationModel* model, ModelMaterial* material, ModelBuffer* modelBuffer);
	~ModelPart();

	void Update(bool isAnimation);
	void Render();

	void AddVertex(D3DXVECTOR3& position, D3DXVECTOR3& normal, D3DXVECTOR2& uv, const ModelBoneWeights& boneWeights);
	void CreateBuffer();

	ModelMaterial* GetMaterial()
	{
		return material;
	}
	
	void SetWorldTransform(D3DXMATRIX& world);

private:
	FBXAnimationModel* model;

	vector<D3DXVECTOR3> positions;
	vector<D3DXVECTOR3> normals;
	vector<D3DXVECTOR2> uvs;
	vector<UINT> indices;
	vector<ModelBoneWeights> boneWeights;

	ID3D11Buffer* vertexBuffer;
	VertexTextureNormalBlend* vertex;
	UINT vertexCount;

	ID3D11Buffer* indexBuffer;
	UINT* index;
	UINT indexCount;

	ModelBuffer* modelBuffer;
	ModelMaterial* material;

	bool isSkinnedModel;

private:
	D3DXMATRIX world;
};