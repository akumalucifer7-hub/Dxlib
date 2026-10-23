#pragma once
#include "ICharacterState.h"

class StateEvade : public ICharacterState
{
public:
    void OnEnter(Character& character, const GameContext& context) override;
    void Update(Character& character, const GameContext& context) override;
    void OnExit(Character& character, const GameContext& context) override;

private:
    float evadeSpeed;      // 回避の移動速度（初速）
    VECTOR moveDir;        // 回避方向の単位ベクトル
};
