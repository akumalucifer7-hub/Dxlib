#pragma once
#include "ICharacterState.h"

class Character;
struct GameContext;

class StateIdle : public ICharacterState
{
public:
    StateIdle() = default;
    ~StateIdle() override = default;

    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;
};