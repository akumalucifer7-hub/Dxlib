#include "CharacterCollision.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "DebugColor.h"

void CharacterCollision::RegisterAttackHitbox(int stateID, const AttackHitboxData& data)
{
    attackHitboxTable[stateID] = data;
}

AABB CharacterCollision::GetHurtboxAABB(const Character& owner) const
{
    // 所有者の位置と向き（Angle + PI）で変換
    return TransformAABB(localHurtbox, owner.GetPos(), owner.GetAngle() + DX_PI_F);
}

bool CharacterCollision::GetCurrentAttackAABB(const Character& owner, AABB& outAABB) const
{
    if (!owner.GetIsMeleeAttacking()) return false;

    auto it = attackHitboxTable.find(owner.GetCurrentAnimID());
    if (it == attackHitboxTable.end()) return false;

    float normTime = owner.GetAnimNormalizedTime();
    const auto& data = it->second;

    if (normTime >= data.startTime && normTime <= data.endTime)
    {
        outAABB = TransformAABB(data.localAABB, owner.GetPos(), owner.GetAngle() + DX_PI_F);
        return true;
    }

    return false;
}

void CharacterCollision::DrawDebug(const Character& owner, bool isHit) const
{
    // 被弾枠の描画
    AABB hurtbox = GetHurtboxAABB(owner);
    DebugRenderer::DrawAABB(hurtbox, DebugColor::Hurtbox);

    // 攻撃枠の描画
    AABB attackAABB;
    if (GetCurrentAttackAABB(owner, attackAABB))
    {
        unsigned int color = isHit ? DebugColor::ActiveHit : DebugColor::Hitbox;
        DebugRenderer::DrawAABB(attackAABB, color);
    }
}