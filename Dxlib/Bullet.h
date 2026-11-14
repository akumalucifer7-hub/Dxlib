#pragma once
#include "DxLib.h"
#include "Collision.h"
#include"GameContext.h"
#include"Villain.h"
#include<iostream>
#include<vector>
struct Bullet
{
    VECTOR pos;         // 現在座標
    VECTOR velocity;    // 移動速度ベクトル
    float lifeTime;     // 生存時間（秒）
    float radius;       // 当たり判定半径
    bool isActive;      // 有効フラグ
};

class Villain; // 前方宣言
struct GameContext;

class BulletManager
{
public:
    static BulletManager& Instance()
    {
        static BulletManager instance;
        return instance;
    }

    // 弾丸の生成
    void SpawnBullet(const VECTOR& startPos, const VECTOR& direction, float speed = 120.0f)
    {
        Bullet b;
        b.pos = startPos;
        b.velocity = VScale(VNorm(direction), speed);
        b.lifeTime = 2.0f; // 2秒間生存
        b.radius = 0.01f;
        b.isActive = true;
        bullets.push_back(b);
    }

    // 毎フレームの更新 & 敵へのヒット判定
    void Update(const GameContext& context)
    {
        for (auto& b : bullets)
        {
            if (!b.isActive) continue;

            // 位置更新
            b.pos = VAdd(b.pos, VScale(b.velocity, context.deltaTime));
            b.lifeTime -= context.deltaTime;

            if (b.lifeTime <= 0.0f)
            {
                b.isActive = false;
                continue;
            }

            // 敵との判定（context.villain の Hurtbox と判定）
            if (context.villain)
            {
                AABB villainHurtbox = context.villain->GetHurtboxAABB();

                // 弾丸の点/球体 と AABB の衝突チェック
                if (b.pos.x >= villainHurtbox.min.x && b.pos.x <= villainHurtbox.max.x &&
                    b.pos.y >= villainHurtbox.min.y && b.pos.y <= villainHurtbox.max.y &&
                    b.pos.z >= villainHurtbox.min.z && b.pos.z <= villainHurtbox.max.z)
                {
                    // ヒット処理
                    context.villain->OnTakeDamage(context);
                    b.isActive = false; // 弾丸消滅
                }
            }
        }

        // 無効化された弾丸の削除
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.isActive; }),
            bullets.end()
        );
    }

    // 簡易描画（オレンジ色の直線 / 球体）
    void Draw() const
    {
        for (const auto& b : bullets)
        {
            if (!b.isActive) continue;
            VECTOR tailPos = VSub(b.pos, VScale(VNorm(b.velocity), 3.0f)); // 軌跡
            DrawLine3D(b.pos, tailPos, GetColor(255, 220, 100));
            DrawSphere3D(b.pos, b.radius, 8, GetColor(255, 255, 200), GetColor(255, 100, 0), TRUE);
        }
    }

private:
    std::vector<Bullet> bullets;
};