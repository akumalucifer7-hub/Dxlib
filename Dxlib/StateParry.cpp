#include "StateParry.h"
#include "StateIdle.h"
#include "Character.h"

void StateParry::OnEnter(Character& character, const GameContext& context)
{
    // パリィ成功モーション再生 & 無敵状態付与
    character.SetIsInvincible(true);
    character.SetAnimation(static_cast<int>(Character::CharacterStateID::Parry), false);
}

void StateParry::Update(Character& character, const GameContext& context)
{
    // モーション終了で待機状態に戻る
    if (character.GetIsAnimationEnd())
    {
        character.ChangeState(std::make_unique<StateIdle>(), context);
    }
}

void StateParry::OnExit(Character& character, const GameContext& context)
{
    character.SetIsInvincible(false);
}