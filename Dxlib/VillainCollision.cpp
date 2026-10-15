#include "VillainCollision.h"
#include "Villain.h"
#include "Player.h"
#include "GameContext.h"
#include "DebugRenderer.h"
#include "DebugColor.h"

void VillainCollision::Init()
{
    isAttackingHit = false;
    isDamaged = false;
    InitHitboxTable();
}

void VillainCollision::InitHitboxTable()
{
    // State::Attack1 の攻撃判定領域設定
    attackHitboxTable[static_cast<int>(Villain::State::Attack1)] = {
        0.2f, 0.6f,
        { VGet(-10.0f, 0.0f, 0.0f), VGet(10.0f, 20.0f, 30.0f) }
    };
}

AABB VillainCollision::GetHurtboxAABB(const VECTOR& pos, float playerAngle) const
{
    AABB localBox = {
        VGet(-3.0f, 0.0f, -2.5f),
        VGet(3.0f, 20.0f, 2.5f)
    };
    return TransformAABB(localBox, pos, playerAngle + DX_PI_F);
}

bool VillainCollision::GetCurrentAttackAABB(const Villain& villain, AABB& outAABB) const
{
    if (!villain.GetIsMeleeAttacking()) return false;

    auto it = attackHitboxTable.find(static_cast<int>(villain.GetCurrentState()));
    if (it == attackHitboxTable.end()) return false;

    float normTime = villain.GetAnimNormalizedTime();
    const auto& data = it->second;

    if (normTime >= data.startTime && normTime <= data.endTime)
    {
        outAABB = TransformAABB(data.localAABB, villain.GetPos(), villain.GetPlayerAngle() + DX_PI_F);
        return true;
    }

    return false;
}

void VillainCollision::Update(Villain& villain, const GameContext& context)
{
    isAttackingHit = false;
    isDamaged = false;

    if (!context.player) return;

    // 1. 敵の攻撃判定 vs プレイヤーの被弾ボックス
    AABB attackAABB;
    if (GetCurrentAttackAABB(villain, attackAABB))
    {
        if (CheckAABBOverlap(attackAABB, context.player->GetHurtboxAABB()))
        {
            isAttackingHit = true;
        }
    }

    // 2. プレイヤーの攻撃判定 vs 敵の被弾ボックス（被弾処理）
    AABB playerAttackAABB;
    if (context.player->GetCurrentAttackAABB(playerAttackAABB))
    {
        if (CheckAABBOverlap(playerAttackAABB, GetHurtboxAABB(villain.GetPos(), villain.GetPlayerAngle())))
        {
            isDamaged = true;
            // 被弾状態へ遷移させる
            villain.OnTakeDamage();
        }
    }

    // ※押し出し処理（ResolveAABBPushoutXZ）は PlayerCollision 側で一括して実行するため、こちらからは削除して二重計算を防ぎます。
}

void VillainCollision::DrawDebug(const Villain& villain)
{
    // 1. 被弾ボックス (Hurtbox) の描画（緑）
    AABB hurtbox = GetHurtboxAABB(villain.GetPos(), villain.GetPlayerAngle());
    DebugRenderer::DrawAABB(hurtbox, DebugColor::Hurtbox);

    // 2. 攻撃ボックス (Hitbox) の描画（赤/黄）
    AABB attackAABB;
    if (GetCurrentAttackAABB(villain, attackAABB))
    {
        unsigned int color = isAttackingHit ? DebugColor::ActiveHit : DebugColor::Hitbox;
        DebugRenderer::DrawAABB(attackAABB, color);
    }
}