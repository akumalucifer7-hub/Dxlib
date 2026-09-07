#include "Animator.h"

void Animator::initAnimator(int modele1, Animstate& state,int defaultAnimIndex)
{
	state.AttachIndex = MV1AttachAnim(modele1, defaultAnimIndex, -1, FALSE);
	state.CurrentAnimation = defaultAnimIndex;
}

void Animator::updateAnimator(int modele1, Animstate& state, int nextAnimIndex, bool isLoop, float deltaTime)
{
	state.NextAnimation = nextAnimIndex;
	state.IsLoop = isLoop;

	// アニメーションの切り替えとブレンド
	if (state.NextAnimation != state.CurrentAnimation)
	{
		if (state.oldIndex != -1)
		{
			MV1DetachAnim(modele1, state.oldIndex);
			state.oldIndex = -1;
		}
		state.oldIndex = state.AttachIndex;
		state.AttachIndex = MV1AttachAnim(modele1, state.NextAnimation);
		state.CurrentAnimation = state.NextAnimation;
		// 時間をリセット 
		state.PlayTime = 0.0f;
		state.BlendRate = 0.0f;
		MV1PhysicsResetState(modele1);
	}

	if (state.oldIndex != -1)
	{
		state.BlendRate += 10.0f * deltaTime;
		if (state.BlendRate > 1.0f)
		{
			state.BlendRate = 1.0f;
		}
		// 新旧のブレンド率を設定
		MV1SetAttachAnimBlendRate(modele1, state.oldIndex, 1.0f - state.BlendRate);
		MV1SetAttachAnimBlendRate(modele1, state.AttachIndex, state.BlendRate);
		if (state.BlendRate >= 1.0f && state.oldIndex != -1)
		{
			MV1DetachAnim(modele1, state.oldIndex);
			state.oldIndex = -1;
		}
	}

	// 再生時間を進める
	state.PlayTime += 45.0f * deltaTime;

	// 再生時間がアニメーションの総再生時間に達したらループ判定に応じて処理
	if (state.PlayTime >= state.TotalTime && state.TotalTime > 0.0f)
	{
		if (state.IsLoop)
		{
			state.PlayTime = 0.0f;
		}
		else
		{
			state.PlayTime = state.TotalTime;
		}
		state.isAnimationEnd = true;
	}
	else
	{
		state.isAnimationEnd = false; // 再生中はfalseにしておく
	}

	state.TotalTime = MV1GetAttachAnimTotalTime(modele1, state.AttachIndex);
	// 再生時間をセットする
	MV1SetAttachAnimTime(modele1, state.AttachIndex, state.PlayTime);
}