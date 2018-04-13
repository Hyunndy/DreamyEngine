#pragma once

class ModelAnimationKeyFrames;
class ModelSkeletonBone
{
public:
	ModelSkeletonBone(wstring name, int parentBoneIndex);
	~ModelSkeletonBone();

	void AddAnimationKeyFrames(ModelAnimationKeyFrames* keyFrames);

	void SetBindPoseTransform(const D3DXMATRIX& matBindPoseTransform);
	void SetBoneReferenceTransform(const D3DXMATRIX& matBoneReferenceTransform);

	ModelAnimationKeyFrames* GetAnimationKeyFrames(wstring animationName);

	const D3DXMATRIX& GetBindPoseTransform() 
	{ 
		return matBindPoseTransform; 
	}

	const D3DXMATRIX& GetInvBindPoseTransform() 
	{ 
		return matInvBindPoseTransform; 
	}

	const D3DXMATRIX& GetBoneReferenceTransform() 
	{ 
		return matBoneReferenceTransform; 
	}

	const D3DXMATRIX& GetInvBoneReferenceTransform() 
	{ 
		return matInvBoneReferenceTransform; 
	}

	wstring GetName() { return name; }
	int GetParentBoneIndex() { return parentBoneIndex; }

private:
	typedef pair<wstring, ModelAnimationKeyFrames *> Pair;

	wstring name;

	D3DXMATRIX matBindPoseTransform;
	D3DXMATRIX matInvBindPoseTransform;
	D3DXMATRIX matBoneReferenceTransform;
	D3DXMATRIX matInvBoneReferenceTransform;

	int parentBoneIndex;

	vector<pair<wstring, ModelAnimationKeyFrames *>> animationKeyFrames;
};