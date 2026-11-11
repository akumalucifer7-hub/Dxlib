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
	bool isNextAttackRequested = false; // 次の攻撃への遷移要求フラグ
};