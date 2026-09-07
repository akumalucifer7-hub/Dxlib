#pragma once
#pragma once
#include "DxLib.h"
#include <algorithm>

// AABB構造体（最小点と最大点）
struct AABB 
{
    VECTOR min;
    VECTOR max;
};

// AABB vs AABB の衝突判定
inline bool CheckAABBOverlap(const AABB& a, const AABB& b) {
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
    return true;
}

inline void ResolveAABBPushoutXZ(const AABB& boxA, VECTOR& posA, const AABB& boxB, const VECTOR& posB)
{
    if (!CheckAABBOverlap(boxA, boxB)) return;
    // 各軸の重なり量を計算
    float overlapX = (std::min)(boxA.max.x, boxB.max.x) - (std::max)(boxA.min.x, boxB.min.x);
    float overlapZ = (std::min)(boxA.max.z, boxB.max.z) - (std::max)(boxA.min.z, boxB.min.z);

    if (overlapX < overlapZ)
    {
        float dirX = (posA.x >= posB.x) ? 1.0f : -1.0f;
        posA.x += dirX * overlapX * 0.5f; 
    }
    else
    {
        float dirZ = (posA.z >= posB.z) ? 1.0f : -1.0f;
        posA.z += dirZ * overlapZ * 0.5f; 
    }
}