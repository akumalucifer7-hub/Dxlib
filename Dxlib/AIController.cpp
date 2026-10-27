#include "AIController.h"
#include "Character.h"
#include"Player.h"
AIController::AIController(float attackRange, float attackInterval)
    : attackRange(attackRange)
    , attackInterval(attackInterval)
{
}

CommandState AIController::GetCommand(const Character& owner, const GameContext& context) const
{
    CommandState cmd;
	float deltatime;
	deltatime = owner.GetDeltaTime();
    // ターゲット（プレイヤー）の存在確認
    if (!context.player) return cmd;

    // 1. タイマー更新
    if (attackTimer > 0.0f)
    {
        attackTimer -= deltatime;
    }

    // 2. ターゲットとの位置関係を計算
    VECTOR playerPos = context.player->GetPos();
    VECTOR myPos = owner.GetPos();
    VECTOR diff = VSub(playerPos, myPos);

    float distance = sqrtf(diff.x * diff.x + diff.z * diff.z);

    // 3. AI思考ロジック
    if (distance <= attackRange)
    {
        // 攻撃範囲内の場合
        if (attackTimer <= 0.0f)
        {
            cmd.isAttack = true;
            attackTimer = attackInterval; // クールダウン開始
        }
    }
    else
    {
        // 攻撃範囲外ならプレイヤーへ向かって接近移動
        if (distance > 0.001f)
        {
            cmd.moveX = diff.x / distance; // 正規化された方向ベクトル
            cmd.moveY = diff.z / distance;
        }
    }

    return cmd;
}