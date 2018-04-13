#include "../stdafx.h"
#include "Model.h"
#include "ModelPart.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"
#include "ModelBoneWeights.h"
#include "ModelAnimation.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelAnimationController.h"

Model::Model(wstring name, ModelBuffer* modelBuffer)
	: name(name), modelBuffer(modelBuffer)
{
	//D3DXMatrixIdentity(&absoluteTransform);
	D3DXMatrixIdentity(&matGeometricOffset);
	D3DXMatrixIdentity(&matAnimationTransform);

	modelBuffer = new ModelBuffer();
}

Model::~Model()
{
	for each(Pair temp in animationKeyFrames)
		SAFE_DELETE(temp.second);

	for (size_t i = 0; i < parts.size(); i++)
		SAFE_DELETE(parts[i]);
}

void Model::Update(bool isAnimation)
{
	for each(ModelPart* part in parts)
		part->Update(isAnimation);
}

void Model::Render()
{
	for each(ModelPart* part in parts)
		part->Render();
}

void Model::AddAnimationKeyFrames(ModelAnimationKeyFrames * animationKeyFrames)
{
	this->animationKeyFrames.push_back(Pair(animationKeyFrames->GetAnimationName(), animationKeyFrames));
}

ModelAnimationKeyFrames * Model::GetAnimationKeyFrames(wstring animationName)
{
	for each(Pair temp in animationKeyFrames)
	{
		if (temp.first == animationName)
			return temp.second;
	}

	return NULL;
}

void Model::AddVertex(ModelMaterial * material, D3DXVECTOR3 position, D3DXVECTOR3 normal, D3DXVECTOR2 uv, const ModelBoneWeights& boneWeights)
{
	bool isNew = true;

	ModelPart* part = NULL;
	for (size_t i = 0; i < parts.size(); i++)
	{
		part = parts[i];

		if (material == part->GetMaterial())
		{
			part->AddVertex(position, normal, uv, boneWeights);
			isNew = false;
		}
	}
	 
	if (isNew == true)
	{
		part = new ModelPart(this, material, modelBuffer);
		part->AddVertex(position, normal, uv, boneWeights);
		
		parts.push_back(part);
	}
}

void Model::CreateBuffer()
{
	for each(ModelPart* part in parts)
		part->CreateBuffer();
}

void Model::UpdateAnimation(ModelAnimationController * animationController)
{
	if (animationController != NULL)
	{
		ModelAnimation* animation = animationController->GetCurrentAnimation();

		if (animation != NULL)
		{
			ModelAnimationKeyFrames* keyFrames = NULL;
			for each(Pair temp in animationKeyFrames)
			{
				if (temp.first == animation->GetName())
				{
					keyFrames = temp.second;

					break;
				}
			}

			if (keyFrames != NULL)
			{
				int keyFrame = animationController->GetCurrentKeyFrame();				
				matAnimationTransform = keyFrames->GetKeyFrameTransform(keyFrame);
				
				return;
			}
		}
	}

	D3DXMatrixIdentity(&matAnimationTransform);
}
