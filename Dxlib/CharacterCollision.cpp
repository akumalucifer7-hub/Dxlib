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

void CharacterCollision::ResolveCharacterCollisions(Character& player, Character& villain, const GameContext& context)
{
    player.SetIsHit(false);
    villain.SetIsHit(false);

    // 1. 敵の攻撃 vs プレイヤーの被弾判定
    AABB villainAttackAABB;
    // ★ 敵が現在攻撃判定を出しているかのチェック（GetCurrentAttackAABB）を追加
    if (villain.GetCurrentAttackAABB(villainAttackAABB))
    {
        if (CheckAABBOverlap(villainAttackAABB, player.GetHurtboxAABB()))
        {
            player.SetIsHit(true);

            if (!villain.GetHasHitTarget())
            {
                villain.SetHasHitTarget(true);

                Player* pPlayer = dynamic_cast<Player*>(&player);
                if (pPlayer && pPlayer->GetParryTimer() > 0.0f)
                {
                    // パリィ成功処理
                    pPlayer->ClearParryTimer();

                    VECTOR diff = VSub(villain.GetPos(), player.GetPos());
                    diff.y = 0.0f;
                    if (VSize(diff) > 0.001f)
                    {
                        player.SetAngle(atan2f(diff.x, diff.z) + DX_PI_F);
                    }

                    player.ChangeState(std::make_unique<StateParry>(), context);
                }
                else if (!player.GetIsInvincible())
                {
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