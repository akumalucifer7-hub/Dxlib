#pragma once
#include "ICharacterState.h"

class GunAttackState : public ICharacterState
{
public:
    GunAttackState() = default;
    ~GunAttackState() override = default;

    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;

private:
    bool hasFired = false; // 二重発射防止フラグ
};