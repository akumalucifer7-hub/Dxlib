#include "StateIdle.h"
#include "StateRun.h"
#include "StateAttack.h"
#include "Character.h"
#include "IController.h"

void StateIdle::OnEnter(Character& owner, const GameContext& context)
{
    // アニメーションID指定（例: 0 = Idle）
    owner.SetAnimation(static_cast<int>(Character::CharacterStateID::Idle), true);
}

void StateIdle::Update(Character& owner, const GameContext& context)
{
    // コントローラー/AIからコマンドを取得
    if (!owner.GetController()) return;
    CommandState cmd = owner.GetController()->GetCommand(owner, context);

    // 1. 攻撃判定
    if (cmd.isAttack)
    {
        owner.ChangeState(std::make_unique<StateAttack>(), context);
        return;
    }

    // 2. 移動判定
    if (cmd.moveX != 0.0f || cmd.moveY != 0.0f)
    {
        owner.ChangeState(std::make_unique<StateRun>(), context);
        return;
    }
}

void StateIdle::OnExit(Character& owner, const GameContext& context)
{
}