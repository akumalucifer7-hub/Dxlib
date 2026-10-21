#pragma once
#include "GameObject.h"
#include "DxLib.h"
#include "Animator.h"
#include "Collision.h"
#include "IController.h"
#include <memory>

class ICharacterState;
struct GameContext;

class Character : public GameObject
{
public:
    virtual ~Character() = default;

    virtual void Init(const GameContext& context) override = 0;
    virtual void Update(const GameContext& context) override;
    virtual void Draw() override;

    // ステート遷移
    void ChangeState(std::unique_ptr<ICharacterState> newState, const GameContext& context);

	// --- キャラクター固有の操作 ---
     void AttatchItems();
     void FrameLotate();

    // --- アクセサ・共通操作 ---
    int GetModelHandle() const { return modelHandle; }
    Animstate& GetAnimState() { return animstate; }

    VECTOR GetPos() const { return pos; }
    void SetPos(const VECTOR& newPos) { pos = newPos; }

    float GetAngle() const { return angle; }
    void SetAngle(float newAngle) { angle = newAngle; }

    float GetVelocityY() const { return velocityY; }
    void SetVelocityY(float v) { velocityY = v; }

    bool GetIsGround() const { return isGround; }
    void SetIsGround(bool ground) { isGround = ground; }

    bool GetIsMeleeAttacking() const { return isMeleeAttacking; }
    void SetIsMeleeAttacking(bool attacking) { isMeleeAttacking = attacking; }

    float GetAnimNormalizedTime() const { return animstate.GetNormalizedTime(); }
    bool GetIsAnimationEnd() const { return animstate.isAnimationEnd; }
    // 
    // アニメーション設定
    void SetAnimation(int animID, bool isLoop)
    {
        currentAnimID = animID;
        isAnimLoop = isLoop;
    }
    int GetCurrentAnimID() const { return currentAnimID; }

    // 抽象判定（各キャラごとに設定または共通コリジョンクラス等へ委譲）
    virtual AABB GetHurtboxAABB() const = 0;
    virtual bool GetCurrentAttackAABB(AABB& outAABB) const = 0;
    virtual void OnTakeDamage() = 0;

protected:
    void UpdatePhysics(const GameContext& context);
    void UpdateTransform();

    std::unique_ptr<ICharacterState> currentState;
    Animstate animstate;

	float deltaTime = 0.0f;
    float angle = DX_PI_F;
    float velocityY = 0.0f;
    bool isGround = true;
    float gravity = -0.8f;

    bool isMeleeAttacking = false;
	bool IsInvincible = false;

    int currentAnimID = 0;
    bool isAnimLoop = true;
};