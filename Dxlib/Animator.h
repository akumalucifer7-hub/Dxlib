#pragma once
#include "DxLib.h"
#include <cmath>
#include <algorithm>

struct Animstate
{
	float TotalTime = 0.0f;
	float PlayTime = 0.0f;
	float BlendRate = 1.0f;
	int CurrentAttach = -1;
	int oldIndex = -1;
	int AttachIndex = -1;
	bool isAnimationEnd = false;

	int CurrentAnimation = -1;
	int NextAnimation = -1;
	bool IsLoop = false;
	float GetNormalizedTime() const { return PlayTime / TotalTime; }
	bool GetIsAnimationEnd() const { return isAnimationEnd; }
};
class Animator
{
public:
	// 初期アニメーションIndexを受け取れるように変更
	void initAnimator(int modele1, Animstate& state, int defaultAnimIndex = 0);

	// アニメーションIndex(int)とループ判定(bool)を受け取るように変更
	void updateAnimator(int modele1, Animstate& state, int nextAnimIndex, bool isLoop, float deltaTime);

	
	
};