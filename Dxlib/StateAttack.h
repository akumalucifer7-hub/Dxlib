#pragma once
#include "ICharacterState.h"

class StateAttack : public ICharacterState
{
public:
    void OnEnter(Character& owner, const GameContext& context) override;
    void Update(Character& owner, const GameContext& context) override;
    void OnExit(Character& owner, const GameContext& context) override;


private:
    int comboStep;                      // 現在のコンボ段階 (1, 2, 3...)
    Player::StateID attackAnimState;      // 再生するアニメーションState

    bool isNextComboRequested;          // 次の連撃入力が先行入力されたか
    bool canAcceptComboInput;           // コンボ先行入力を受け付けるタイミングか
    float attackTimer;                  // 攻撃経過時間
    float stepForwardSpeed;             // 攻撃開始時の踏み込み（前進）速度
};