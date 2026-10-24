#include "StateDamage.h"
#include "StateIdle.h"
#include "Character.h"

void StateDamage::OnEnter(Character& owner, const GameContext& context)
{
    // 1. 攻撃動作などを中断し、無敵状態を設定
    owner.SetIsMeleeAttacking(false);
    owner.SetIsInvincible(true);

    // 2. 被弾アニメーションを単発再生
    owner.SetAnimation(static_cast<int>(Character::CharacterStateID::Damage1), false);
}

void StateDamage::Update(Character& owner, const GameContext& context)
{
	float deltatime = owner.calculateDeltaTime();
    // 1. ノックバック移動（後ろ方向に減速しながら移動）
    if (knockbackSpeed > 0.0f)
    {
        MATRIX rotMat = MGetRotY(owner.GetAngle());
        VECTOR forwardVec = VTransform(VGet(0.0f, 0.0f, -1.0f), rotMat); // 正面ベクトル
        VECTOR backwardVec = VScale(forwardVec, -1.0f);                  // 後方ベクトル

        VECTOR moveOffset = VScale(backwardVec, knockbackSpeed * deltatime);
        owner.SetPos(VAdd(owner.GetPos(), moveOffset));

        // 徐々に減速
        knockbackSpeed -= 500.0f * deltatime;
        if (knockbackSpeed < 0.0f) knockbackSpeed = 0.0f;
    }

    // 2. 被弾アニメーションが終了したら Idle へ戻す
    if (owner.GetIsAnimationEnd())
    {
        owner.ChangeState(std::make_unique<StateIdle>(), context);
    }
}

void StateDamage::OnExit(Character& owner, const GameContext& context)
{
    // 被弾ステートを抜けたタイミングで無敵を解除
    // （※起き上がり時の無敵時間を残したい場合はここでタイマーをかける等も可能）
    owner.SetIsInvincible(false);
}