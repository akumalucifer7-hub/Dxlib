#include "CharacterCollision.h"
#include "Character.h"
#include "GameContext.h"
#include "InputValidation.h"
#include "StateDamage.h"
#include "StateParry.h"
#include "DebugRenderer.h"
#include "DebugColor.h"
#include"IController.h"
#include"Player.h"
#include"EffectManager.h"
void CharacterCollision::ResolveCharacterCollisions(Character& player, Character& villain, const GameContext& context)
{
    player.SetIsHit(false);
    villain.SetIsHit(false);
    VECTOR hitPos;
    AABB villainAttackAABB;
    if (villain.GetCurrentAttackAABB(villainAttackAABB))
    {
        if (CheckAABBOverlap(villainAttackAABB, player.GetHurtboxAABB()))
        {
            player.SetIsHit(true);

            if (!villain.GetHasHitTarget())
            {
                villain.SetHasHitTarget(true);
                 hitPos = CalculateHitPosition(villainAttackAABB, player.GetHurtboxAABB());
                Player* pPlayer = dynamic_cast<Player*>(&player);
                if (pPlayer && pPlayer->GetParryTimer() > 0.0f)
                {
                    // 1. パリィ成功・タイマーリセット
                    pPlayer->ClearParryTimer();

                    // 2. プレイヤーを敵の方向へ向かせる
                    VECTOR diff = VSub(villain.GetPos(), player.GetPos());
                    diff.y = 0.0f;
                    if (VSize(diff) > 0.001f)
                    {
                        player.SetAngle(atan2f(diff.x, diff.z) + DX_PI_F);
                    }

                    // 3. プレイヤーを Parry ステートへ変更
                    player.ChangeState(std::make_unique<StateParry>(), context);

                    // 敵の攻撃判定を強制終了し、攻撃フラグを折る
                    villain.SetIsMeleeAttacking(false);
                    villain.SetHasHitTarget(true); // 残存Hitboxの無効化

                    // ★ 敵側を被弾/怯みステート（StateDamage等）へ強制遷移させて攻撃を完全に叩き落とす
                    villain.OnTakeDamage(context);
                }
                else if (!player.GetIsInvincible())
                {
                    EffectManager::Instance().Play("HitSlash", hitPos, villain.GetAngle(), 1.0f);
                    player.OnTakeDamage(context);
                }
            }
        }
    }

    // 2. プレイヤーの攻撃 vs 敵の被弾判定
    AABB playerAttackAABB;
    if (player.GetCurrentAttackAABB(playerAttackAABB))
    {
        if (CheckAABBOverlap(playerAttackAABB, villain.GetHurtboxAABB()))
        {
            if (!villain.GetIsInvincible() &&
                villain.GetCurrentStateID() != Character::CharacterStateID::Damage1)
            {
                hitPos = CalculateHitPosition(playerAttackAABB, villain.GetHurtboxAABB());
                EffectManager::Instance().Play("HitSlash", hitPos, player.GetAngle(), 1.0f);
                villain.OnTakeDamage(context);
                villain.SetIsHit(true);
            }
        }
    }

    // 3. キャラクター同士のめり込み押し出し
    VECTOR playerPos = player.GetPos();
    VECTOR villainPos = villain.GetPos();

    ResolveAABBPushoutXZ(
        player.GetHurtboxAABB(), playerPos,
        villain.GetHurtboxAABB(), villainPos
    );

    player.SetPos(playerPos);
    villain.SetPos(villainPos);
}

void CharacterCollision::RegisterAttackHitbox(int stateID, float startTime, float endTime, const AABB& localAABB)
{
    hitboxTable[stateID] = { startTime, endTime, localAABB };
}

void CharacterCollision::RegisterAttackHitbox(int stateID, const AttackHitboxData& data)
{
    hitboxTable[stateID] = data;
}

bool CharacterCollision::GetCurrentAttackAABB(const Character& owner, AABB& outAABB) const
{
    int currentAnimID = owner.GetCurrentAnimID();
    auto it = hitboxTable.find(currentAnimID);
    if (it == hitboxTable.end()) return false;

    float normTime = owner.GetAnimNormalizedTime();
    const auto& data = it->second;

    if (normTime >= data.startTime && normTime <= data.endTime)
    {
        outAABB = TransformAABB(data.localAABB, owner.GetPos(), owner.GetAngle());
        return true;
    }

    return false;
}

AABB CharacterCollision::GetHurtboxAABB(const Character& owner) const
{
    return TransformAABB(localHurtbox, owner.GetPos(), owner.GetAngle());
}

void CharacterCollision::DrawDebug(const Character& owner) const
{
    // 1. 被弾ボックス(Hurtbox)を緑色で描画
    AABB hurtbox = GetHurtboxAABB(owner);
    DebugRenderer::DrawAABB(hurtbox, DebugColor::Hurtbox);

    // 2. 攻撃ボックス(Hitbox)発生中は赤色/黄色で描画
    AABB attackAABB;
    if (GetCurrentAttackAABB(owner, attackAABB))
    {
        unsigned int color = owner.GetIsHit() ? DebugColor::ActiveHit : DebugColor::Hitbox;
        DebugRenderer::DrawAABB(attackAABB, color);
    }
}

 VECTOR CharacterCollision::CalculateHitPosition(const AABB& a, const AABB& b)
{
    // X, Y, Z 各軸で重なっている範囲（Overlap）の最小・最大を求める
    float minX = (a.min.x > b.min.x) ? a.min.x : b.min.x;
    float maxX = (a.max.x < b.max.x) ? a.max.x : b.max.x;

    float minY = (a.min.y > b.min.y) ? a.min.y : b.min.y;
    float maxY = (a.max.y < b.max.y) ? a.max.y : b.max.y;

    float minZ = (a.min.z > b.min.z) ? a.min.z : b.min.z;
    float maxZ = (a.max.z < b.max.z) ? a.max.z : b.max.z;

    // 重なり領域の中心点をヒット座標とする
    return VGet(
        minX + (maxX - minX) * 0.5f,
        minY + (maxY - minY) * 0.5f,
        minZ + (maxZ - minZ) * 0.5f
    );
}