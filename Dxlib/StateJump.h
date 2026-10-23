#pragma once
#include "ICharacterState.h"

class StateJump : public ICharacterState
{
public:
    void OnEnter(Character& character, const GameContext& context) override;
    void Update(Character& character, const GameContext& context) override;
    void OnExit(Character& character, const GameContext& context) override;
};