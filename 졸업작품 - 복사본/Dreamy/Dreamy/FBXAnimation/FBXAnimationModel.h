#pragma once
#include "stdafx_fbx.h"
#include "ModelPart.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"
#include "ModelBoneWeights.h"
#include "ModelAnimation.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelAnimationController.h"

class ModelPart;
class ModelMaterial;
class ModelBuffer;
class ModelBoneWeights;
class ModelAnimationKeyFrames;
class ModelAnimationController;

class FBXAnimationModel
{
public:
	FBXAnimationModel(wstring name, ModelBuffer* modelBuffer);
	~FBXAnimationModel();

	void Update(bool isAnimation);
	void UpdateAnimation(ModelAnimationController* animationController);
	void Render();

	wstring GetName() { return name; }

	void AddAnimationKeyFrames(ModelAnimationKeyFrames* animationKeyFrames);
	ModelAnimationKeyFrames* GetAnimationKeyFrames(wstring animationName);

	void SetGeometricOffset(const D3DXMATRIX& matGeometricOffset)
	{
		this->matGeometricOffset = matGeometricOffset;
	}

	D3DXMATRIX GetGeometricOffset()
	{
		return matGeometricOffset;
	}

	D3DXMATRIX GetAnimationTransform()
	{
		return matAnimationTransform;
	}

	void SetAbsoluteTransform(const D3DXMATRIX& matAbsoluteTransform)
	{
		this->matAnimationTransform = matAbsoluteTransform;
	}

	void AddVertex(ModelMaterial * material, D3DXVECTOR3 position, D3DXVECTOR3 normal, D3DXVECTOR2 uv, const ModelBoneWeights& boneWeights);
	void CreateBuffer();

	void SetWorldTransform(D3DXMATRIX& world);

private:
	wstring name;

	//D3DXMATRIX absoluteTransform;
	D3DXMATRIX matGeometricOffset;
	D3DXMATRIX matAnimationTransform;

	vector<ModelPart *> parts;

	ModelBuffer* modelBuffer;

	typedef pair<wstring, ModelAnimationKeyFrames *> Pair;
	vector<Pair> animationKeyFrames;
};