#include "StateJump.h"
#include "StateIdle.h"
#include "Character.h"
#include "StateFall.h"
#include "ICharacterState.h"
#include "IController.h" // 追加
#include "Field.h"       // 追加

void StateJump::OnEnter(Character& character, const GameContext& context)
{
	character.SetAnimation(static_cast<int>(Character::CharacterStateID::Jump), false);
	character.SetVelocityY(9.0f);
	character.SetIsGround(false);
	VECTOR currentPos = character.GetPos();
	currentPos.y += 1.5f;
	character.SetPos(currentPos);
}

void StateJump::Update(Character& character, const GameContext& context)
{
	// --- 状態遷移判定 ---
	if (character.GetVelocityY() < 0.0f)
	{
		character.ChangeState(std::make_unique<StateFall>(), context);
		return;
	}

	if (character.GetVelocityY() <= 0.0f && character.GetIsGround())
	{
		character.ChangeState(std::make_unique<StateFall>(), context);
		return;
	}

	// --- 空中移動処理 ---
	if (const IController* controller = character.GetController())
	{
		CommandState cmd = controller->GetCommand(character, context);
		float squareLength = cmd.moveX * cmd.moveX + cmd.moveY * cmd.moveY;

		// 入力がある場合のみ移動
		if (squareLength >= 0.001f)
		{
			VECTOR moveVec = VNorm(VGet(cmd.moveX, 0.0f, cmd.moveY));

			// 向きの更新
			character.SetAngle(atan2f(moveVec.x, moveVec.z) + DX_PI_F);

			// 移動処理（空中での移動速度は Run と同じ 1.5f に設定。必要に応じて調整してください）
			float moveSpeed = 1.5f;
			VECTOR moveOffset = VScale(moveVec, moveSpeed);

			VECTOR currentPos = character.GetPos();
			VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);
			currentPos.x = finalPos.x;
			currentPos.z = finalPos.z;

			character.SetPos(currentPos);
		}
	}
}

void StateJump::OnExit(Character& character, const GameContext& context)
{
}