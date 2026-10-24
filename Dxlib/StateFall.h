#pragma once
#include "Character.h"
#include "ICharacterState.h"
class StateFall : public ICharacterState
{
public:
    void OnEnter(Character& character, const GameContext& context) override;
    void Update(Character& character, const GameContext& context) override;
    void OnExit(Character& character, const GameContext& context) override;
};
#pragma once
