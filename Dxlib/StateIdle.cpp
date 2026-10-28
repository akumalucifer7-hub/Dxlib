#include "StateIdle.h"
#include "StateRun.h"
#include "StateJump.h"
#include "StateAttack.h"
#include "StateDamage.h"
#include"StateFall.h"
#include "Character.h"
#include "IController.h"

void StateIdle::OnEnter(Character& owner, const GameContext& context)
{
    // 1. フラグの初期化
    owner.SetIsMeleeAttacking(false);

    // 2. Idleアニメーションの再生
    // （CharacterStateID::Idle に対応するアニメーションIDを指定）
    owner.SetAnimation(static_cast<int>(Character::CharacterStateID::Idle), true);
}

void StateIdle::Update(Character& owner, const GameContext& context)
{
    // コントローラー（PlayerController または AIController）からコマンドを取得
    const IController* controller = owner.GetController();
    if (!controller) return;

    CommandState cmd = controller->GetCommand(owner, context);

    // ステート遷移判定（優先度順）

    // 1. 被弾判定 (最優先)
    if (cmd.isDamaged)
    {
        owner.ChangeState(std::make_unique<StateDamage>(), context);
        return;
    }

    // 2. 空中落下判定 (空中からの着地漏れ・落下開始の検知)
    if (!owner.GetIsGround())
    {
        // 落下ステートがある場合は StateFall へ遷移
        // owner.ChangeState(std::make_unique<StateFall>(), context);
        return;
    }

    // 3. ジャンプ判定
    if (cmd.isJump && owner.GetIsGround())
    {
        owner.ChangeState(std::make_unique<StateJump>(), context);
        return;
    }

    // 4. 攻撃判定
    if (cmd.isAttack)
    {
        bool hasDirectionInput = (cmd.moveX != 0.0f || cmd.moveY != 0.0f);

        owner.ChangeState(std::make_unique<StateAttack>(1, hasDirectionInput), context);
        return;
    }

    // 5. 移動判定 (スティック入力または移動キーの押下)
    if (cmd.moveX != 0.0f || cmd.moveY != 0.0f)
    {
        owner.ChangeState(std::make_unique<StateRun>(), context);
        return;
    }
    //6. 落下判定
    if (!owner.GetIsGround())
    {
        owner.ChangeState(std::make_unique<StateFall>(), context);
    }
}

void StateIdle::OnExit(Character& owner, const GameContext& context)
{
    // 待機状態を抜ける際の共通処理（必要に応じて記述）
}