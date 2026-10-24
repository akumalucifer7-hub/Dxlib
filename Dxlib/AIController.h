#pragma once
#include "IController.h"
#include"Character.h"

class AIController : public IController
{
public:
    AIController(float attackRange = 30.0f, float attackInterval = 1.5f);

    CommandState GetCommand(const Character& owner, const GameContext& context) const override;

private:
    float attackRange;      // 間合い（攻撃に移行する距離）
    float attackInterval;   // 攻撃のクールダウン時間
    float attackTimer = 0.0f;
};