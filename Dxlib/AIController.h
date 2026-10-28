// AIController.h

#pragma once
#include "IController.h"
#include "DxLib.h"

class AIController : public IController
{
public:
    enum class AIMode
    {
        Idle,
        Approach,
        Attack,
        KeepDistance,
        CoolingDown
    };

    enum class KeepDistType
    {
        Back,       // 後退
        StrafeLeft, // 左移動
        StrafeRight,// 右移動
        Watch       // 敵対対象の観察
    };

    AIController();
    ~AIController() override = default;

    CommandState GetCommand(const Character& owner, const GameContext& context) const override;

private:
    mutable AIMode currentMode = AIMode::Idle;
    mutable float stateTimer = 0.0f;
    mutable float attackCooldown = 0.0f;

    // ★ 牽制行動のバリエーション管理用変数
    mutable KeepDistType currentKeepType = KeepDistType::Back;
    mutable float keepTypeTimer = 0.0f; // 牽制パターンの切替タイマー

    const float detectionRange = 150.0f;
    const float attackRange = 10.0f;
    const float keepDistMin = 15.0f;

    void UpdateAIMode(const Character& owner, const GameContext& context, float distToPlayer) const;
    void UpdateKeepDistanceBehavior(float deltaTime) const;
};