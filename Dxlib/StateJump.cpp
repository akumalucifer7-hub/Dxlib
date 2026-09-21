#include "StateJump.h"
#include "StateIdle.h"
#include "Player.h"
#include "StateFall.h"

void StateJump::OnEnter(Player& player, const GameContext& context)
{
	// Jumpアニメーションの再生設定
	player.SetAnimation(Player::State::Jump, false);
	
	// ジャンプ開始時の初期速度を設定
	player.SetVelocityY(12.5f); // 例: 上方向に12.5fの速度を与える

	//  離陸直後の誤着地判定を防ぐため、接地フラグを解除し位置を少しだけ浮かせる
	player.SetIsGround(false);
	VECTOR currentPos = player.GetPos();
	currentPos.y += 3.5f;
	player.SetPos(currentPos);
}

void StateJump::Update(Player& player, const GameContext& context)
{
	// --- 空中攻撃の割り込み判定 ---
	bool isAttackPressed = context.input->IsMouseTrigger(InputValidation::Mouse::Left) ||
		context.input->IsButtonTrigger(XINPUT_BUTTON_X);

	// --- 状態遷移判定 ---
	// 頂点に達し、Y軸速度が下向きになったら落下状態へ遷移
	if (player.GetVelocityY() < 0.0f)
	{
		player.ChangeState(std::make_unique<StateFall>(), context);
		return;
	}

	// 天井にぶつかる等で即座に着地した場合
	if (player.GetVelocityY() <= 0.0f && player.GetIsGround())
	{
		player.ChangeState(std::make_unique<StateIdle>(), context);
		return;
	}
}

void StateJump::OnExit(Player& player, const GameContext& context)
{
	// ジャンプ状態終了時の処理（特になければ空で問題ありません）
}