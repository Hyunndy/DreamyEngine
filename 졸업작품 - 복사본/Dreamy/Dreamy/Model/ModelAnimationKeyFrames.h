#pragma once

class ModelAnimationQuaternionKeyFrame
{
public:
	ModelAnimationQuaternionKeyFrame(const D3DXMATRIX& transform)
	{
		D3DXQuaternionRotationMatrix(&q, &transform);
		translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
	}

	const D3DXQUATERNION& GetQuaternion() const { return q; }
	const D3DXVECTOR3& GetTranslation() const { return translation; }

private:
	D3DXQUATERNION q;
	D3DXVECTOR3 translation;
};

class ModelAnimationKeyFrames
{
public:
	ModelAnimationKeyFrames(wstring animationName);
	~ModelAnimationKeyFrames();

	void AddKeyFrame(const D3DXMATRIX& transform);

	wstring GetAnimationName();
	const D3DXMATRIX& GetKeyFrameTransform(int keyFrame) const;
	const ModelAnimationQuaternionKeyFrame& GetQuaternionKeyFrame(int keyFrame) const;

private:
	wstring animationName;

	typedef pair<D3DXMATRIX, ModelAnimationQuaternionKeyFrame> Pair;
	vector<Pair> keyFrames;
};