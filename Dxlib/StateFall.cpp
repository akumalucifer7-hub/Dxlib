#include "StateFall.h"
#include"StateIdle.h"
#include"Player.h"
void StateFall::OnEnter(Player& player, const GameContext& context)
{
	// Fallアニメーションの再生設定など
	player.SetAnimation(Player::State::Fall, true);
	// StateFall自身が、Fallアニメーション(Player::State::Fall)を再生するようAnimatorに指示する
	
}

void StateFall::Update(Player& player, const GameContext& context)
{
	// ---  落下中の処理 ---
	// ここで落下中の移動や入力判定などを行うことができます。

	// --- 状態遷移判定 ---
	// 接地したらIdle状態へ遷移
	if (player.GetIsGround())
	{
		player.ChangeState(std::make_unique<StateIdle>(), context);
		return;
	}
}
void StateFall::OnExit(Player& player, const GameContext& context)
{
	// Fall状態終了時の処理（もしあれば）
}