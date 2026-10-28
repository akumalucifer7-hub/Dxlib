#pragma once
#include "GameObject.h"
#include "DxLib.h"
#include "Animator.h"
#include "Collision.h"
#include "IController.h"
#include "CharacterCollision.h" // コリジョンコンポーネント
#include <memory>

class ICharacterState;
struct GameContext;

class Character : public GameObject
{
public:
    Character();
    enum class CharacterStateID
    {
        Idle, Run, Jump, Fall, Attack1, Attack2, Attack3, Attack4, Evade, Damage1, Parry, MAX
    };

    virtual ~Character();

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
	// --- 攻撃関連の仮想関数 ---
    virtual int GetMaxComboSteps() const { return 2; } // デフォルトは2段
    virtual CharacterStateID GetAttackAnimID(int comboStep) const;
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
	void SetIsGround(bool ground) { isGround = ground; }
	float GetVelocityY() const { return velocityY; }
	void SetVelocityY(float newVelocityY) { velocityY = newVelocityY; }
    bool GetIsMeleeAttacking() const { return isMeleeAttacking; }
    void SetIsMeleeAttacking(bool attacking) { isMeleeAttacking = attacking; }
	int GetComboStep() const { return currentComboStep; }
	void SetComboStep(int step) { currentComboStep = step; }
	bool GetIsHit() const { return isHit; }
	void SetIsHit(bool hit) { isHit = hit; }
	int IncrementComboStep() { return ++currentComboStep; }
    int ResetComboStep() { currentComboStep = 0; return currentComboStep; }
    bool GetHasHitTarget() const { return hasHitTarget; }
    void SetHasHitTarget(bool hit) { hasHitTarget = hit; }
	bool GetIsInvincible() const { return isInvincible; }
	void SetIsInvincible(bool invincible) { isInvincible = invincible; }
    float GetAnimNormalizedTime() const { return animstate.GetNormalizedTime(); }
	float GetDeltaTime() const { return deltaTime; }
	float GetHP() const { return HP; }
	float GetMaxHP() const { return maxHP; }
	int GetLevel() const { return CharacterLevel; }
	int GetAttackPower() const { return AttackPower; }
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
	bool isHit = false;
	int currentComboStep = 0;
	int maxComboSteps = 2; // デフォルトの最大コンボ段数
    bool hasHitTarget = false; 
    float gravity = -0.8f;
    bool isMeleeAttacking = false;
    bool isInvincible = false;
    int currentAnimID = 0;
    bool isAnimLoop = true;
    int centerFrameIndex = -1;
	float HP = 100.0f; // キャラクターの体力
	float maxHP = 100.0f; // キャラクターの最大体力
	int CharacterLevel = 1; // キャラクターのレベル（デフォルトは1）
	int AttackPower = 5; // キャラクターの攻撃力（デフォルトは10）
};