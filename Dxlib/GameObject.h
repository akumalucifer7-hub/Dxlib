#pragma once
// GameObject.h
#pragma once
#include "DxLib.h"
#include "GameContext.h"

struct Capsule
{
    VECTOR bottom;
    VECTOR top;
    float radius;
};

class GameObject
{
public:
    GameObject() : pos(VGet(0.0f, 0.0f, 0.0f)), modelHandle(-1), prevTime(0) {}
    virtual ~GameObject() {}

    // 純粋仮想関数
    virtual void Init(const GameContext& context) = 0;
    virtual void Update(const GameContext& context) = 0;
    virtual void Draw() = 0;
    virtual Capsule GetHurtCapsule() const
    {
        Capsule cap;
        cap.bottom = pos;
        cap.top = VGet(pos.x, pos.y + 16.0f, pos.z);
        cap.radius = 5.0f;
        return cap;
    }
    // 共通アクセサ関数
    virtual VECTOR GetPos() const { return pos; }
    virtual void SetPos(const VECTOR& newPos) { pos = newPos; }

protected:
    // デルタタイム計算の共通化
    float CalculateDeltaTime()
    {
        LONGLONG currentTime = GetNowHiPerformanceCount();
        float deltaTime = static_cast<float>(currentTime - prevTime) / 1000000.0f;
        prevTime = currentTime;
        return deltaTime;
    }

    VECTOR pos;
    int modelHandle = -1;
    int centerFrameIndex;
    LONGLONG prevTime = 0;
    float HP ;
    
};