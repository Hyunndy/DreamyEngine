#pragma once

enum class AnimationMode
{
	Play = 0, Pause, Stop,
};

class ModelAnimation;
class ModelAnimationController
{
public:
	ModelAnimationController();
	~ModelAnimationController();

	void AddAnimation(ModelAnimation* animation);

	ModelAnimation* GetCurrentAnimation();
	void SetCurrentAnimation(wstring animationName);
	void SetCurrentAnimation(int index);

	UINT GetAnimationCount();
	ModelAnimation* GetAnimation(int index);

	void Play();
	void Pause();
	void Stop();

	int GetCurrentKeyFrame() { return currentKeyFrame; }
	void SetCurrentKeyFrame(int keyFrame);
	
	int GetNextKeyFrame() { return nextKeyFrame; }
	float GetKeyFrameFactor() { return keyFrameFactor; }

	AnimationMode GetAnimationMode() { return animationMode; }

	void UseQuaternionKeyFrames(bool use) { useQuaternionKeyFrames = use; }
	bool UseQuaternionKeyFrames() { return useQuaternionKeyFrames; }

	void Update();
private:
	AnimationMode animationMode;
	ModelAnimation* currentAnimation;

	typedef pair<wstring, ModelAnimation *> Pair;
	vector<Pair> animations;
	
	float frameTimer;

	int currentKeyFrame;
	int nextKeyFrame;
	float keyFrameFactor;
	bool useQuaternionKeyFrames;
};