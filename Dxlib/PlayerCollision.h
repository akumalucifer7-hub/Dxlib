#pragma once
#include "DxLib.h"
#include "Collision.h"
#include "AttackHitbox.h"
#include "AABBTransform.h"
#include <unordered_map>

class Player;
struct GameContext;

class PlayerCollision
{
public:
    PlayerCollision() = default;
    ~PlayerCollision() = default;

    void Init();
    void Update(Player& player, const GameContext& context, float deltaTime);
    void DrawDebug(const Player& player);

    // 被弾用AABB (Hurtbox) の取得
    AABB GetHurtboxAABB(const VECTOR& pos, float playerAngle) const;

    // 現在アクティブな攻撃AABB (Hitbox) の取得
    bool GetCurrentAttackAABB(const Player& player, AABB& outAABB) const;

    // フラグ・タイマーの取得
    bool GetIsHit() const { return isHit; }
    float GetParryTimer() const { return parryTimer; }

private:
    void InitHitboxTable();

    bool isHit = false;
    float parryTimer = 0.0f;

    // Player::State (intにキャストして格納) をキーとする攻撃Hitboxマップ
    std::unordered_map<int, AttackHitboxData> attackHitboxTable;
};