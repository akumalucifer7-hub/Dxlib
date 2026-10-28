#include "StateAttack.h"
#include "StateIdle.h"
#include "StateRun.h"
#include "Player.h"
#include "Field.h"
#include "InputValidation.h"
#include "IController.h"
#include"Player.h"
StateAttack::StateAttack(int attackIndex, bool isParryAttempt)
{
    comboStep = attackIndex;
    isNextComboRequested = false;
    stepForwardSpeed = 0.0f;

}

void StateAttack::OnEnter(Character& character, const GameContext& context)
{
    character.SetIsMeleeAttacking(true);
    character.SetHasHitTarget(false);

    // キャラクター本体のコンボ段数も同期
    character.SetComboStep(comboStep);

    // 相手への向き直し処理
    if (context.player && &character != context.player)
    {
        VECTOR myPos = character.GetPos();
        VECTOR playerPos = context.player->GetPos();
        VECTOR diff = VSub(playerPos, myPos);
        diff.y = 0.0f;

        if (VSize(diff) > 0.001f)
        {
            float targetAngle = atan2f(diff.x, diff.z) + DX_PI_F;
            character.SetAngle(targetAngle);
        }
    }
	//攻撃時に前方に移動するための初期速度を設定
    if (isParryAttempt)
    {
        context.player->SetParryTimer(0.25f);
    }
    
        // comboStep に対応するアニメーションを取得して再生
        Character::CharacterStateID animID = character.GetAttackAnimID(comboStep);
        character.SetAnimation(static_cast<int>(animID), false);

    
    
    stepForwardSpeed = 2.0f;
}

void StateAttack::Update(Character& character, const GameContext& context)
{
    float timer = context.player->GetParryTimer();
    if (timer > 0.0f) 
    {
        // deltaTimeがミリ秒なら / 1000.0f、秒ならそのまま
        context.player->SetParryTimer(timer - context.deltaTime);
    }

    CommandState cmd = character.GetController()->GetCommand(character, context);

    // ★ 1. 攻撃動作中に一度でも入力があれば、先行入力フラグを true で保持（予約）する
    if (cmd.isAttack)
    {
        isNextComboRequested = true;
    }
   
    // ★ 2. アニメーション終了時にコンボ派生判定
    if (character.GetIsAnimationEnd())
    {
        // 予約があり、かつ最大コンボ段数未満なら次の段（comboStep + 1）へ派生！
        if (isNextComboRequested && comboStep < character.GetMaxComboSteps())
        {
            character.ChangeState(std::make_unique<StateAttack>(comboStep + 1), context);
        }
        else
        {
            // コンボ終了時は Idle へ戻り、コンボ段数をリセット
            character.ResetComboStep();
            character.ChangeState(std::make_unique<StateIdle>(), context);
        }
    }
}

void StateAttack::OnExit(Character& character, const GameContext& context)
{
    character.SetIsMeleeAttacking(false);
	context.player->ClearParryTimer();
}