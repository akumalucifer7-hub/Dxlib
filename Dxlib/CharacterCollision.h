#pragma once
#include "Collision.h"
#include "AABBTransform.h"
#include "DxLib.h"
#include <unordered_map>

class Character;
struct GameContext;

// 攻撃ヒットボックス定義データ
struct AttackHitboxData
{
    float startTime;   // 判定開始アニメーション正規化時間 (0.0f～1.0f)
    float endTime;     // 判定終了アニメーション正規化時間 (0.0f～1.0f)
    AABB localAABB;    // キャラクターローカル空間でのAABB
};

class CharacterCollision
{
public:
    CharacterCollision() = default;

    // 被弾ボックス(Hurtbox)のローカル範囲を設定
    void SetLocalHurtbox(const AABB& localAABB) { localHurtbox = localAABB; }

    // 攻撃判定登録（4引数版）
    void RegisterAttackHitbox(int stateID, float startTime, float endTime, const AABB& localAABB);

    // 攻撃判定登録（2引数版：{ startTime, endTime, localAABB } で呼び出せるようにする）
    void RegisterAttackHitbox(int stateID, const AttackHitboxData& data);

    // 現在の攻撃AABBの取得（発生時間外なら false）
    bool GetCurrentAttackAABB(const Character& owner, AABB& outAABB) const;

    // 被弾用 (Hurtbox) AABBの取得
    AABB GetHurtboxAABB(const Character& owner) const;

    // デバッグ描画（HurtboxとHitboxのAABBを描画）
    void DrawDebug(const Character& owner) const;

    // 2キャラ間の衝突・パリィ・ダメージ・押し出しの一括解決
    static void ResolveCharacterCollisions(Character& player, Character& villain, const GameContext& context);

private:
    AABB localHurtbox = { VGet(-3.0f, 0.0f, -2.5f), VGet(3.0f, 20.0f, 2.5f) }; // デフォルト値
    std::unordered_map<int, AttackHitboxData> hitboxTable;
};