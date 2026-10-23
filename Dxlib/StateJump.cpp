#include "StateJump.h"
#include "StateIdle.h"
#include "Character.h"
#include "StateFall.h"
#include"ICharacterState.h"

void StateJump::OnEnter(Character& character, const GameContext& context)
{
	// Jumpアニメーションの再生設定
	character.SetAnimation(static_cast<int>(Character::CharacterStateID::Jump), false);
	
	// ジャンプ開始時の初期速度を設定
	character.SetVelocityY(12.5f); // 例: 上方向に12.5fの速度を与える

	//  離陸直後の誤着地判定を防ぐため、接地フラグを解除し位置を少しだけ浮かせる
	character.SetIsGround(false);
	VECTOR currentPos = character.GetPos();
	currentPos.y += 3.5f;
	character.SetPos(currentPos);
}

void StateJump::Update(Character& character, const GameContext& context)
{
	// --- 空中攻撃の割り込み判定 ---
	
	// --- 状態遷移判定 ---
	// 頂点に達し、Y軸速度が下向きになったら落下状態へ遷移
	if (character.GetVelocityY() < 0.0f)
	{
		character.ChangeState(std::make_unique<StateFall>(), context);
		return;
	}

	// 天井にぶつかる等で即座に着地した場合
	if (character.GetVelocityY() <= 0.0f && character.GetIsGround())
	{
		character.ChangeState(std::make_unique<StateFall>(), context);
		return;
	}
}

void StateJump::OnExit(Character& character, const GameContext& context)
{
	// ジャンプ状態終了時の処理（特になければ空で問題ありません）
}