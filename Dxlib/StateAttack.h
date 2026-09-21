#pragma once
#include"IPlayerState.h"
#include"Player.h"

class StateAttack : public IPlayerState
{
public:

    StateAttack(int comboStep = 1, Player::State attackState = Player::State::Attack1);

	void OnEnter(Player& player, const GameContext& context) override;
	void Update(Player& player, const GameContext& context) override;
	void OnExit(Player& player, const GameContext& context) override;

private:
    int comboStep;                      // 現在のコンボ段階 (1, 2, 3...)
    Player::State attackAnimState;      // 再生するアニメーションState

    bool isNextComboRequested;          // 次の連撃入力が先行入力されたか
    bool canAcceptComboInput;           // コンボ先行入力を受け付けるタイミングか
    float attackTimer;                  // 攻撃経過時間
    float stepForwardSpeed;             // 攻撃開始時の踏み込み（前進）速度
};