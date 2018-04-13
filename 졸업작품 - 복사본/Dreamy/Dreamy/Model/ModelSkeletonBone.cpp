#include "../stdafx.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimationKeyFrames.h"

ModelSkeletonBone::ModelSkeletonBone(wstring name, int parentBoneIndex)
	: name(name), parentBoneIndex(parentBoneIndex)
{
	D3DXMatrixIdentity(&matBindPoseTransform);
	D3DXMatrixIdentity(&matInvBindPoseTransform);
	D3DXMatrixIdentity(&matBoneReferenceTransform);
	D3DXMatrixIdentity(&matInvBoneReferenceTransform);
}

ModelSkeletonBone::~ModelSkeletonBone()
{
	for each(Pair temp in animationKeyFrames)
		SAFE_DELETE(temp.second);
}

void ModelSkeletonBone::AddAnimationKeyFrames(ModelAnimationKeyFrames * keyFrames)
{
	animationKeyFrames.push_back(Pair(keyFrames->GetAnimationName(), keyFrames));
}

void ModelSkeletonBone::SetBindPoseTransform(const D3DXMATRIX & matBindPoseTransform)
{
	this->matBindPoseTransform = matBindPoseTransform;
	D3DXMatrixInverse(&this->matInvBindPoseTransform, NULL, &this->matBindPoseTransform);
}

void ModelSkeletonBone::SetBoneReferenceTransform(const D3DXMATRIX & matBoneReferenceTransform)
{
	this->matBoneReferenceTransform = matBoneReferenceTransform;
	D3DXMatrixInverse(&this->matInvBoneReferenceTransform, NULL, &this->matBoneReferenceTransform);
}

ModelAnimationKeyFrames * ModelSkeletonBone::GetAnimationKeyFrames(wstring animationName)
{
	for each(Pair temp in animationKeyFrames)
	{
		if (temp.first == animationName)
			return temp.second;
	}

	return NULL;
}
