#include "StateAttack.h"
#include "StateIdle.h"
#include "StateRun.h"
#include "Player.h"
#include "Field.h"
#include "InputValidation.h"
#include "IController.h"
#include "AudioManager.h"

StateAttack::StateAttack(int attackIndex, bool isParryAttempt)
{
    comboStep = attackIndex;
    isNextComboRequested = false;
    stepForwardSpeed = 0.0f;
    AudioManager::Instance().Play("Attack", DX_PLAYTYPE_BACK);
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
        context.player->SetParryTimer(timer - context.deltaTime);
    }

    if (stepForwardSpeed > 0.0f)
    {
        VECTOR pos = character.GetPos();
        float angle = character.GetAngle();

        // キャラクターの向いている方向へ移動
        pos.x += -sinf(angle) * stepForwardSpeed;
        pos.z += -cosf(angle) * stepForwardSpeed;

        // 速度を徐々に落とす（摩擦）
        stepForwardSpeed *= 0.85f; // 
        if (stepForwardSpeed < 0.01f)
        {
            stepForwardSpeed = 0.0f;
        }

        // 移動後の座標をセット
        character.SetPos(pos);
    }
  

    CommandState cmd = character.GetController()->GetCommand(character, context);

    // 1. 攻撃動作中に一度でも入力があれば、先行入力フラグを true で保持（予約）する
    if (cmd.isAttack)
    {
        isNextComboRequested = true;
    }

    // 2. アニメーション終了時にコンボ派生判定
    if (character.GetIsAnimationEnd())
    {
        if (isNextComboRequested && comboStep < character.GetMaxComboSteps())
        {
            character.ChangeState(std::make_unique<StateAttack>(comboStep + 1), context);
        }
        else
        {
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