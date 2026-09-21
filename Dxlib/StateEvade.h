#pragma once
#include "IPlayerState.h"

class StateEvade : public IPlayerState
{
public:
    void OnEnter(Player& player, const GameContext& context) override;
    void Update(Player& player, const GameContext& context) override;
    void OnExit(Player& player, const GameContext& context) override;

private:
    float evadeSpeed;      // 回避の移動速度（初速）
    VECTOR moveDir;        // 回避方向の単位ベクトル
};
