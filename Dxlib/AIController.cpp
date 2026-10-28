
#include "AIController.h"
#include "Character.h"
#include"Player.h"

#include "GameContext.h"
#include <cmath>
#include <cstdlib>
AIController::AIController()
{
    currentMode = AIMode::Idle;
    stateTimer = 0.0f;
    attackCooldown = 0.0f;
    keepTypeTimer = 0.0f;
    currentKeepType = KeepDistType::Back;
}

CommandState AIController::GetCommand(const Character& owner, const GameContext& context) const
{
    CommandState cmd;
    if (!context.player) return cmd;

    float deltaTime = context.deltaTime;

    if (stateTimer > 0.0f) stateTimer -= deltaTime;
    if (attackCooldown > 0.0f) attackCooldown -= deltaTime;

    // プレイヤーとの相対位置計算
    VECTOR myPos = owner.GetPos();
    VECTOR playerPos = context.player->GetPos();
    VECTOR toPlayer = VSub(playerPos, myPos);
    toPlayer.y = 0.0f;
    float distToPlayer = VSize(toPlayer);

    // ★ 移動中・移動先でも常にプレイヤーの方向に向き直す
    if (distToPlayer > 0.001f)
    {
        float targetAngle = atan2f(toPlayer.x, toPlayer.z) + DX_PI_F;
        const_cast<Character&>(owner).SetAngle(targetAngle);
    }

    UpdateAIMode(owner, context, distToPlayer);

    // -------------------------------------------------------------
    // モードごとのコマンド生成
    // -------------------------------------------------------------
    switch (currentMode)
    {
    case AIMode::Approach:
    {
        if (distToPlayer > 0.001f)
        {
            VECTOR dir = VNorm(toPlayer);
            cmd.moveX = dir.x;
            cmd.moveY = dir.z;
        }
        break;
    }

    case AIMode::Attack:
    {
        cmd.isAttack = true;
        break;
    }

    case AIMode::KeepDistance:
    {
        // 牽制パターンの更新（0.4〜0.8秒ごとにパターン変更）
        UpdateKeepDistanceBehavior(deltaTime);

        if (distToPlayer > 0.001f)
        {
            VECTOR forward = VNorm(toPlayer);                     // プレイヤーへ向かうベクトル
            VECTOR left = VGet(-forward.z, 0.0f, forward.x);      // 左方向のベクトル

            switch (currentKeepType)
            {
            case KeepDistType::Back:
                // 後退（プレイヤーと反対方向）
                cmd.moveX = -forward.x * 0.7f;
                cmd.moveY = -forward.z * 0.7f;
                break;

            case KeepDistType::StrafeLeft:
                // 左へステップ・横移動
                cmd.moveX = left.x * 0.5f;
                cmd.moveY = left.z * 0.5f;
                break;

            case KeepDistType::StrafeRight:
                // 右へステップ・横移動
                cmd.moveX = -left.x * 0.5f;
                cmd.moveY = -left.z * 0.5f;
                break;

            case KeepDistType::Watch:
            default:
                // 立ち止まって睨み合い（移動入力なし）
                cmd.moveX = 0.0f;
                cmd.moveY = 0.0f;
                break;
            }
        }
        break;
    }

    case AIMode::CoolingDown:
    case AIMode::Idle:
    default:
        break;
    }

    return cmd;
}

void AIController::UpdateKeepDistanceBehavior(float deltaTime) const
{
    keepTypeTimer -= deltaTime;
    if (keepTypeTimer <= 0.0f)
    {
        // 0.4秒〜0.8秒間隔で行動パターンをランダムに選ぶ
        keepTypeTimer = 0.4f + (rand() % 40 / 100.0f);

        int roll = rand() % 100;
        if (roll < 40)
        {
            currentKeepType = KeepDistType::Back;        // 40% で後退
        }
        else if (roll < 65)
        {
            currentKeepType = KeepDistType::StrafeLeft;  // 25% で左移動
        }
        else if (roll < 90)
        {
            currentKeepType = KeepDistType::StrafeRight; // 25% で右移動
        }
        else
        {
            currentKeepType = KeepDistType::Watch;       // 10% で立ち止まり
        }
    }
}

void AIController::UpdateAIMode(const Character& owner, const GameContext& context, float distToPlayer) const
{
    Character::CharacterStateID currentState = owner.GetCurrentStateID();

    // 攻撃動作中（Attack1 〜 Attack4）のコンボ判断
    if (currentState == Character::CharacterStateID::Attack1 ||
        currentState == Character::CharacterStateID::Attack2 ||
        currentState == Character::CharacterStateID::Attack3 ||
        currentState == Character::CharacterStateID::Attack4)
    {
        // ★ 1. キャラクター固有の最大コンボ数（Villainなら2段など）に達している場合はコンボを終了
        if (owner.GetComboStep() >= owner.GetMaxComboSteps())
        {
            currentMode = AIMode::CoolingDown;
            return;
        }

        float normTime = owner.GetAnimNormalizedTime();

        // ★ 2. アニメーション後半（0.2f 〜 0.9f）まで攻撃入力を維持し、確実に次段を予約させる
        if (normTime >= 0.2f && normTime <= 0.9f)
        {
            if (distToPlayer <= attackRange + 8.0f)
            {
                currentMode = AIMode::Attack; // cmd.isAttack = true を維持
                return;
            }
        }

        currentMode = AIMode::CoolingDown;
        return;
    }

    // 被弾・落下中
    if (currentState == Character::CharacterStateID::Damage1 ||
        currentState == Character::CharacterStateID::Fall)
    {
        currentMode = AIMode::Idle;
        return;
    }

    // クールダウン中（攻撃と攻撃の間の間合い調整）
    if (attackCooldown > 0.0f)
    {
        if (distToPlayer < keepDistMin + 4.0f)
        {
            currentMode = AIMode::KeepDistance;
        }
        else
        {
            currentMode = AIMode::CoolingDown;
        }
        return;
    }

    // 通常時の攻撃始動判定
    if (distToPlayer <= attackRange)
    {
        currentMode = AIMode::Attack;
        // 1コンボ出し切った後のクールダウン時間（1.0〜1.8秒）
        attackCooldown = 1.0f + (rand() % 80 / 100.0f);
    }
    else if (distToPlayer <= detectionRange)
    {
        currentMode = AIMode::Approach;
    }
    else
    {
        currentMode = AIMode::Idle;
    }
}