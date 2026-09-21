#pragma once
#include "IPlayerState.h"

class StateIdle : public IPlayerState
{
public:
    void OnEnter(Player& player, const GameContext& context) override;
    void Update(Player& player, const GameContext& context) override;
    void OnExit(Player& player, const GameContext& context) override;
};
