#include "PlayerCollision.h"
#include "Player.h"
#include "GameContext.h"
#include "DebugRenderer.h"
#include "DebugColor.h"

void PlayerCollision::Init()
{
    isHit = false;
    parryTimer = 0.0f;
    InitHitboxTable();
}

void PlayerCollision::InitHitboxTable()
{
    attackHitboxTable[static_cast<int>(Player::State::Attack1)] = {
        0.2f, 0.5f,
        { VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
    };
    attackHitboxTable[static_cast<int>(Player::State::Attack2)] = {
        0.2f, 0.5f,
        { VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
    };
    attackHitboxTable[static_cast<int>(Player::State::Attack3)] = {
        0.2f, 0.5f,
        { VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
    };
    attackHitboxTable[static_cast<int>(Player::State::Attack4)] = {
        0.2f, 0.5f,
        { VGet(-10.0f, 0.0f, 0.0f), VGet(10.0f, 20.0f, 40.0f) }
    };
    attackHitboxTable[static_cast<int>(Player::State::Attackthrust)] = {
        0.2f, 0.4f,
        { VGet(-10.0f, 5.0f, 0.0f), VGet(10.0f, 25.0f, 70.0f) }
    };
    attackHitboxTable[static_cast<int>(Player::State::AttackHelmbreak)] = {
        0.3f, 0.7f,
        { VGet(-30.0f, -10.0f, -30.0f), VGet(30.0f, 40.0f, 30.0f) }
    };
}

AABB PlayerCollision::GetHurtboxAABB(const VECTOR& pos, float playerAngle) const
{
    AABB localHurtbox = {
        VGet(-3.0f, 0.0f, -2.5f),
        VGet(3.0f, 20.0f, 2.5f)
    };
    return TransformAABB(localHurtbox, pos, playerAngle + DX_PI_F);
}

bool PlayerCollision::GetCurrentAttackAABB(const Player& player, AABB& outAABB) const
{
    if (!player.GetIsMeleeAttacking()) return false;

    auto it = attackHitboxTable.find(static_cast<int>(player.GetCurrentAnimID()));
    if (it == attackHitboxTable.end()) return false;

    float normTime = player.GetAnimNormalizedTime();
    const auto& data = it->second;

    // 現在のアニメーション再生時間が有効範囲内かチェック
    if (normTime >= data.startTime && normTime <= data.endTime) {
        outAABB = TransformAABB(data.localAABB, player.GetPos(), player.GetPlayerAngle() + DX_PI_F);
        return true;
    }

    return false;
}

void PlayerCollision::Update(Player& player, const GameContext& context, float deltaTime)
{
    isHit = false;

    if (parryTimer > 0.0f)
    {
        parryTimer -= deltaTime;
    }

    if (!context.villain) return;

    // 1. プレイヤーの攻撃判定 vs 敵の被弾ボックス
    AABB attackAABB;
    if (GetCurrentAttackAABB(player, attackAABB))
    {
        if (CheckAABBOverlap(attackAABB, context.villain->GetHurtboxAABB()))
        {
            isHit = true;
        }
    }

    // 2. 敵の攻撃判定 vs プレイヤーの被弾ボックス
    AABB villainAttackAABB;
    AABB playerHurtbox = GetHurtboxAABB(player.GetPos(), player.GetPlayerAngle());

    if (context.villain->GetCurrentAttackAABB(villainAttackAABB))
    {
        if (CheckAABBOverlap(villainAttackAABB, playerHurtbox))
        {
            if (context.input->IsMouseTrigger(InputValidation::Mouse::Left))
            {
                player.SetIsMeleeAttacking(true);
                parryTimer = 0.6f;
                VECTOR enemyPos = context.villain->GetPos();
                VECTOR diff = VSub(enemyPos, player.GetPos());
                player.SetPlayerAngle(atan2f(diff.x, diff.z) + DX_PI_F);
            }
            else if (context.input->IsKeyTrigger(KEY_INPUT_LALT))
            {
                player.SetIsMeleeAttacking(false);
                player.SetIsInvincible(true);
            }
            else if (player.GetCurrentAnimID() != Player::State::SwordShinogi &&
                player.GetCurrentAnimID() != Player::State::Damage1 &&
                parryTimer <= 0.0f)
            {
                // 被弾処理など（必要に応じて追加）
            }
        }
    }

    // 3. XZ軸の押し出し計算と位置更新
    VECTOR currentPos = player.GetPos();
    ResolveAABBPushoutXZ(playerHurtbox, currentPos, context.villain->GetHurtboxAABB(), context.villain->GetPos());
    player.SetPos(currentPos);
}

void PlayerCollision::DrawDebug(const Player& player)
{
    // 1. 被弾ボックス (Hurtbox) 描画
    AABB hurtbox = GetHurtboxAABB(player.GetPos(), player.GetPlayerAngle());
    DebugRenderer::DrawAABB(hurtbox, DebugColor::Hurtbox);

    // 2. 攻撃ボックス (Hitbox) 描画
    AABB attackAABB;
    if (GetCurrentAttackAABB(player, attackAABB)) {
        unsigned int color = isHit ? DebugColor::ActiveHit : DebugColor::Hitbox;
        DebugRenderer::DrawAABB(attackAABB, color);
    }
}