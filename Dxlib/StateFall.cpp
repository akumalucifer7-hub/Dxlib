#include "StateFall.h"
#include "StateIdle.h"
#include "Character.h"
#include "IController.h" // 追加
#include "Field.h"       // 追加

void StateFall::OnEnter(Character& character, const GameContext& context)
{
	character.SetAnimation(static_cast<int>(Character::CharacterStateID::Fall), true);
}

void StateFall::Update(Character& character, const GameContext& context)
{
	// --- 状態遷移判定 ---
	if (character.GetIsGround())
	{
		character.ChangeState(std::make_unique<StateIdle>(), context);
		return;
	}

	// --- 落下中の移動処理 ---
	if (const IController* controller = character.GetController())
	{
		CommandState cmd = controller->GetCommand(character, context);
		float squareLength = cmd.moveX * cmd.moveX + cmd.moveY * cmd.moveY;

		if (squareLength >= 0.001f)
		{
			VECTOR moveVec = VNorm(VGet(cmd.moveX, 0.0f, cmd.moveY));

			character.SetAngle(atan2f(moveVec.x, moveVec.z) + DX_PI_F);

			float moveSpeed = 1.0f;
			VECTOR moveOffset = VScale(moveVec, moveSpeed);

			VECTOR currentPos = character.GetPos();
			VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);
			currentPos.x = finalPos.x;
			currentPos.z = finalPos.z;

			character.SetPos(currentPos);
		}
	}
}

void StateFall::OnExit(Character& character, const GameContext& context)
{
}