#pragma once
#include "IPlayerState.h"
#include "IController.h"
class StateIdle : public IPlayerState
{
public:
    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;
};
