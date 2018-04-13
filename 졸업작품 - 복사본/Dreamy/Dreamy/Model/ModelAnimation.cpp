#include "../stdafx.h"
#include "ModelAnimation.h"

ModelAnimation::ModelAnimation(wstring name, int keyFrame, float defaultFrameRate)
	: name(name), keyFrame(keyFrame), frameRate(keyFrame), defaultFrameRate(defaultFrameRate)
{
	
}

ModelAnimation::~ModelAnimation()
{
}
