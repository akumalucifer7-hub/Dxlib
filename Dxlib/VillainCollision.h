#pragma once
#include "DxLib.h"
#include "Collision.h"
#include "AttackHitbox.h"
#include "AABBTransform.h"
#include <unordered_map>

class Villain;
struct GameContext;

class VillainCollision
{
public:
    VillainCollision() = default;
    ~VillainCollision() = default;

    void Init();
    void Update(Villain& villain, const GameContext& context);
    void DrawDebug(const Villain& villain);

    // 敵の被弾ボックス (Hurtbox)
    AABB GetHurtboxAABB(const VECTOR& pos, float playerAngle) const;

    // 敵の攻撃ボックス (Hitbox)
    bool GetCurrentAttackAABB(const Villain& villain, AABB& outAABB) const;

    // フラグ取得
    bool GetIsAttackingHit() const { return isAttackingHit; } // 自分の攻撃がプレイヤーに当たったか
    bool GetIsDamaged() const { return isDamaged; }           // 自分がプレイヤーから攻撃を受けたか

private:
    void InitHitboxTable();

    bool isAttackingHit = false;
    bool isDamaged = false;

    // Villain::State (int) をキーとした Hitbox テーブル
    std::unordered_map<int, AttackHitboxData> attackHitboxTable;
};