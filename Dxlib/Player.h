#pragma once
#include "DxLib.h"
#include <cmath>
#include <algorithm>
#include "Weapon.h"
#include"GameObject.h"
#include "Field.h"
#include "InputValidation.h"
#include "Animator.h"
#include"Camera.h"
#include"Villain.h"
#include "Collision.h"
#include "AttackHitbox.h"
#include"AABBTransform.h"
#include"DebugRenderer.h"
#include"DebugColor.h"
#include <unordered_map>

class Player : public GameObject
{
public:

	void Init(const GameContext& context)override;
	void Update(const GameContext& context)override;
	void Draw()override;
	void DrawDebug();
	~Player() {};
	// 現在アクティブな攻撃AABBを取得（判定がない場合は false）
	bool GetCurrentAttackAABB(AABB& outAABB) const;
	// プレイヤー自身の被弾用（Hurtbox）AABBを取得
// 被弾用の直方体（例: キャラクターを囲むバウンディングボックス）
	AABB GetHurtboxAABB() const {
		AABB localHurtbox = {
			VGet(-3.0f, 0.0f, -2.5f),
			VGet(3.0f, 20.0f, 2.5f)
		};
		return TransformAABB(localHurtbox, pos, playerAngle + DX_PI_F);
	}
	bool GetIsHit() const { return isHit; }
	//int GetHP() const { return HP; }
	//int GetAttackPower() const { return attackPower; }
private:
	Animstate animstate;
	void AttackState(const GameContext& context);
	void EvadeMovement(const GameContext& context);    
	void updateMovement(const GameContext& context);
	void MeleeForwardMovement(const GameContext& context);
	void ApplyNormalMovement(const GameContext& context);
	void UpdatePhysics(const GameContext& context);
	void JumpAction(const GameContext& context);
	void EvadeAction(const GameContext& context);
	void UpdatePlayerState();
	void FrameLotate();
	void Animation(const GameContext& context, float deltaTime);
	void AttatchItems();
	void CollisionUpdate(const GameContext& context);
	// Playerの管理下にステートを定義
	enum class State
	{
		Idle,
		Run, 
		Jump,
		Fall,
		Attack1, 
		Attack2, 
		Attack3, 
		Attack4, 
		AttackHelmbreak, 
		Walk, 
		GunAttack, 
		Dodge, 
		Attackthrust, 
		Attack5, 
		AerialAttack,
		AerialAttack2,
		SwordShinogi,
		Damage1,
		Evade,
		MAX
	};
	State currentState = State::Idle;
	float parryTimer = 0.0f;
	//--- アイテム管理 ---
	Weapon SwordItems;
	Weapon GunItemsRight;
	Weapon GunItemsLeft;
	//--- 物理計算関連 ---
	bool isGround = true;
	float groundY = 0.0f;
	float Gravity = -0.8f;

	//--- 移動関連 ---
	float velocityY = 0.0f;
	float playerspeed = 3.0;
	MATRIX mat1, mat2;
	float LX;
	float LY;
	VECTOR moveVec;
	float playerAngle = DX_PI_F;
	float modelAngle = 0.0f;
	// --- 入力関連 ---
	int mouseX, mouseY;
	bool prevButtonX = false;
	int prevMouseInputLeft = 0;
	int prevMouseInputRight = 0;
	bool isMeleeAttacking = false;
	bool isGunAttacking = false;
	bool IsLoop = false;

	//コンボ管理用
	int comboStep = 0;
	bool isNextAttackRequested = false;
	bool isHit = false;
	// 攻撃パターンごとの判定設定マップ
	std::unordered_map<State, AttackHitboxData> attackHitboxTable;

	void InitHitboxTable();
};