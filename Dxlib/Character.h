#pragma once
#include "GameObject.h"
#include "DxLib.h"
#include "Animator.h"
#include "Collision.h"
#include "IController.h"
#include "CharacterCollision.h" // コリジョンコンポーネント
#include <memory>
#include"IController.h"

class ICharacterState;
struct GameContext;

class Character : public GameObject
{
public:
    enum class CharacterStateID
    {
        Idle, Run, Jump, Fall, Attack1, Attack2, Attack3, Attack4, Evade, Damage1, MAX
    };

    virtual ~Character() = default;

    virtual void Init(const GameContext& context) override = 0;
    virtual void Update(const GameContext& context) override;
    virtual void Draw() override;
	virtual void FrameRotate();
	virtual void AttatchItems();
    // --- 当たり判定用の仮想関数 ---
    virtual AABB GetHurtboxAABB() const;
    virtual bool GetCurrentAttackAABB(AABB& outAABB) const;
    virtual void DrawDebug();
    virtual void OnTakeDamage(const GameContext& context);

    // コリジョンコンポーネントへのアクセサ
    CharacterCollision& GetCollision() { return collision; }
    const CharacterCollision& GetCollision() const { return collision; }

    // ステート遷移
    void ChangeState(std::unique_ptr<ICharacterState> newState, const GameContext& context);

    // 共通アクセサ・操作
    CharacterStateID GetCurrentStateID() const { return currentStateID; }
    int GetModelHandle() const { return modelHandle; }
    Animstate& GetAnimState() { return animstate; }
    VECTOR GetPos() const { return pos; }
    void SetPos(const VECTOR& newPos) { pos = newPos; }
    float GetAngle() const { return angle; }
    void SetAngle(float newAngle) { angle = newAngle; }
	bool GetIsGround() const { return isGround; }
    bool GetIsMeleeAttacking() const { return isMeleeAttacking; }
    void SetIsMeleeAttacking(bool attacking) { isMeleeAttacking = attacking; }
	bool GetIsHit() const { return isInvincible; }
    float GetAnimNormalizedTime() const { return animstate.GetNormalizedTime(); }
    bool GetIsAnimationEnd() const { return animstate.isAnimationEnd; }
    void SetAnimation(int animID, bool isLoop) { currentAnimID = animID; isAnimLoop = isLoop; }
    int GetCurrentAnimID() const { return currentAnimID; }
	const IController* GetController() const { return controller.get(); }
protected:
    void UpdatePhysics(const GameContext& context);
    void UpdateTransform();

    CharacterStateID currentStateID = CharacterStateID::Idle;
    std::unique_ptr<ICharacterState> currentState;
    std::unique_ptr<IController> controller;

    CharacterCollision collision; // コンポーネントとして所有

    Animstate animstate;
    float deltaTime = 0.0f;
    float angle = DX_PI_F;
    float velocityY = 0.0f;
    bool isGround = true;
    float gravity = -0.8f;
    bool isMeleeAttacking = false;
    bool isInvincible = false;
    int currentAnimID = 0;
    bool isAnimLoop = true;
    int centerFrameIndex = -1;
};