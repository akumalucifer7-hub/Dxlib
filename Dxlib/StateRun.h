#pragma once
#include "ICharacterState.h"

class StateRun : public ICharacterState
{
public:
    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;
};