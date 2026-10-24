#pragma once
#include "DxLib.h"
#include "Collision.h"
#include "AttackHitbox.h"
#include "AABBTransform.h"
#include <unordered_map>

class Character;

class CharacterCollision
{
public:
    CharacterCollision() = default;

    // Hurtbox（被弾枠）のローカル設定
    void SetLocalHurtbox(const AABB& localBox) { localHurtbox = localBox; }

    // Hitbox（攻撃枠）の登録
    void RegisterAttackHitbox(int stateID, const AttackHitboxData& data);

    // ワールド座標系での被弾ボックス取得
    AABB GetHurtboxAABB(const Character& owner) const;

    // 現在のアニメーション・時間に応じたアクティブな攻撃ボックスを取得
    bool GetCurrentAttackAABB(const Character& owner, AABB& outAABB) const;

    // デバッグ描画
    void DrawDebug(const Character& owner, bool isHit = false) const;

private:
    AABB localHurtbox{ VGet(-3.0f, 0.0f, -2.5f), VGet(3.0f, 20.0f, 2.5f) };

    // StateID (int) -> 攻撃データ
    std::unordered_map<int, AttackHitboxData> attackHitboxTable;
};