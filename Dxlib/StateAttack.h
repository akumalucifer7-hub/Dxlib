#pragma once
#include "ICharacterState.h"
#include "Character.h"

class StateAttack : public ICharacterState
{
public:
    StateAttack(int attackIndex = 1, bool isParryAttempt = false);
    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;

private:
    int comboStep;              // 現在のコンボ段階 (1, 2, 3...)
    bool isNextComboRequested;  // 先行入力（コンボ予約）フラグ
    float stepForwardSpeed;     // 前進速度
    bool isParryAttempt;        // パリィ攻撃かどうか
};