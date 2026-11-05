#include "StateParry.h"
#include "StateIdle.h"
#include "Character.h"
#include "AudioManager.h"

void StateParry::OnEnter(Character& character, const GameContext& context)
{
    // 無敵状態の付与
    character.SetIsInvincible(true);
    character.SetIsMeleeAttacking(true); // 攻撃中フラグを折る

    // コンボ段数とフラグの完全リセット（StateAttackへの引き戻しを防止）
    character.ResetComboStep();

    // モーション再生
    character.SetAnimation(static_cast<int>(Character::CharacterStateID::Parry), false);
    character.GetAnimState().isAnimationEnd = false;
    frameCounter = 0;

    AudioManager::Instance().Play("Parry", DX_PLAYTYPE_BACK);
}

void StateParry::Update(Character& character, const GameContext& context)
{
    if (character.GetIsAnimationEnd())
    {
        

            character.ChangeState(std::make_unique<StateIdle>(), context);
        
    }
}
void StateParry::OnExit(Character& character, const GameContext& context)
{
    character.SetIsInvincible(false);
    character.SetIsMeleeAttacking(false);
}
