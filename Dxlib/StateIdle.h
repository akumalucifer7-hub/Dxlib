#pragma once
#include "ICharacterState.h"
#include "IController.h"

class StateIdle : public ICharacterState
{
public:
    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;
};