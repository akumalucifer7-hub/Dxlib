#pragma once
#include "ICharacterState.h"

class StateDamage : public ICharacterState
{
public:
    StateDamage(float knockbackSpeed = 100.0f)
        : knockbackSpeed(knockbackSpeed) {
    }

    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;

private:
    float knockbackSpeed = 100.0f; // ノックバック速度
};