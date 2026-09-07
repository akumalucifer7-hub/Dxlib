#pragma once
#include"Collision.h"
#include "DxLib.h"

struct AttackHitboxData {
    float startTime;   // 判定が発生する正規化時間 (0.0f ～ 1.0f)
    float endTime;     // 判定が終了する正規化時間 (0.0f ～ 1.0f)
    AABB localAABB;    // プレイヤー中心（ローカル座標系）の判定範囲
};