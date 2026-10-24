#include "StateFall.h"
#include"StateIdle.h"
#include"Character.h"
void StateFall::OnEnter(Character& character, const GameContext& context)
{
	// Fallアニメーションの再生設定など
	character.SetAnimation(static_cast<int>(Character::CharacterStateID::Fall), true);
	// StateFall自身が、Fallアニメーション(Character::State::Fall)を再生するようAnimatorに指示する
	
}

void StateFall::Update(Character& character, const GameContext& context)
{
	// ---  落下中の処理 ---
	// ここで落下中の移動や入力判定などを行うことができます。

	// --- 状態遷移判定 ---
	// 接地したらIdle状態へ遷移
	if (character.GetIsGround())
	{
		character.ChangeState(std::make_unique<StateIdle>(), context);
		return;
	}
}
void StateFall::OnExit(Character& character, const GameContext& context)
{

	// Fall状態終了時の処理（もしあれば）
}