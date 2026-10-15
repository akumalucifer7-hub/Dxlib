#include "StateIdle.h"
#include "Character.h"
#include "StateRun.h"
#include "StateAttack.h"

void StateIdle::OnEnter(Character& owner, const GameContext& context)
{
    // アニメーションID指定（0 = Idle と定義）
    owner.SetAnimation(0, true);
}

void StateIdle::Update(Character& owner, const GameContext& context)
{
    // コントローラー/AIから現在のコマンドを取得
    // ※ Character 内に IController へのスマートポインタを持たせる構成にすると非常にスマートです
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