#include "StateRun.h"
#include "StateIdle.h"
#include "StateJump.h"
#include "StateFall.h"
#include "StateAttack.h"
#include "StateDamage.h"
#include "Character.h"
#include "IController.h"
#include "Field.h"


void StateRun::OnEnter(Character& owner, const GameContext& context)
{
    // Runアニメーション再生
    owner.SetAnimation(static_cast<int>(Character::CharacterStateID::Run), true);
}

void StateRun::Update(Character& owner, const GameContext& context)
{
    const IController* controller = owner.GetController();
    if (!controller) return;

    CommandState cmd = controller->GetCommand(owner, context);

    // 1. ステート遷移判定（優先度順）

    // 被弾判定
    if (cmd.isDamaged)
    {
        owner.ChangeState(std::make_unique<StateDamage>(), context);
        return;
    }

    // 落下判定
    if (!owner.GetIsGround() && owner.GetVelocityY() < 0.0f)
    {
        owner.ChangeState(std::make_unique<StateFall>(), context);
        return;
    }

    // ジャンプ判定
    if (cmd.isJump && owner.GetIsGround())
    {
        owner.ChangeState(std::make_unique<StateJump>(), context);
        return;
    }

    // 攻撃判定
    if (cmd.isAttack)
    {
        owner.ChangeState(std::make_unique<StateAttack>(1), context);
        return;
    }

    // 移動入力がなくなったら Idle へ遷移
    float squareLength = cmd.moveX * cmd.moveX + cmd.moveY * cmd.moveY;
    if (squareLength < 0.001f)
    {
        owner.ChangeState(std::make_unique<StateIdle>(), context);
        return;
    }

    // 2. 移動処理
    VECTOR moveVec = VGet(cmd.moveX, 0.0f, cmd.moveY);
    moveVec = VNorm(moveVec); // 移動ベクトルの正規化

    // 向きの更新
    float newAngle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
    owner.SetAngle(newAngle);

    // キャラクターの移動速度を取得して移動量を計算
    // （※Character側に GetMoveSpeed() が未実装の場合は固定値やプロパティを参照）
    float moveSpeed = 1.75f;
    VECTOR moveOffset = VScale(moveVec, moveSpeed);

    // フィールドとの壁衝突判定を行って座標を更新
    VECTOR currentPos = owner.GetPos();
    VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);
    currentPos.x = finalPos.x;
    currentPos.z = finalPos.z;

    owner.SetPos(currentPos);
}

void StateRun::OnExit(Character& owner, const GameContext& context)
{
}