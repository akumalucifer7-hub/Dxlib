#include "StateParry.h"
#include "StateIdle.h"
#include "Character.h"
#include"AudioManager.h"
void StateParry::OnEnter(Character& character, const GameContext& context)
{
    // パリィ成功モーション再生 & 無敵状態付与
    character.SetIsInvincible(true);
    character.SetAnimation(static_cast<int>(Character::CharacterStateID::Attack1), false);
    frameCounter = 0;
    character.SetIsInvincible(true);
	AudioManager::Instance().Play("Parry", DX_PLAYTYPE_BACK);
}

void StateParry::Update(Character& character, const GameContext& context)
{
    frameCounter++;
    // モーション終了で待機状態に戻る
    if (frameCounter >= 30)
    {
        character.ChangeState(std::make_unique<StateIdle>(), context);
    }
}

void StateParry::OnExit(Character& character, const GameContext& context)
{
    character.SetIsInvincible(false);
}