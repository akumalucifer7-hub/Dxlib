#pragma once
#include "IController.h"

// 前方向宣言（循環参照を防止）
class Character;
struct GameContext;

class AIController : public IController
{
public:
    AIController(float attackRange = 30.0f, float attackInterval = 1.5f);

    CommandState GetCommand(const Character& owner, const GameContext& context) const override;

private:
    float attackRange;      // 間合い（攻撃に移行する距離）
    float attackInterval;   // 攻撃のクールダウン時間

    // const メンバ関数内からの更新を許可するために mutable を付与
    mutable float attackTimer = 0.0f;
};