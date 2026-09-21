#pragma once
#include "DxLib.h"
#include <cmath>
#include <algorithm>
#include"Weapon.h"
#include"GameObject.h"
#include "Field.h"
#include "Animator.h"
#include "Collision.h"
#include "AttackHitbox.h"
#include"AABBTransform.h"
#include <unordered_map>

class Villain : public GameObject
{
public:
	void Init(const GameContext& context) override;
	void Update(const GameContext& context) override;
	void Draw() override;

	~Villain() {};
	enum class State
	{
		Idle, Run, Jump, Fall, Attack1, Damage1, MAX,
	};
	State GetCurrentState(){ return static_cast<State>(currentState); }
	bool GetCurrentAttackAABB(AABB& outAABB) const;
	AABB GetHurtboxAABB() const
	{
		// 敵キャラ中心のローカル被弾ボックス
		AABB localBox = 
		{
		VGet(-3.0f, 0.0f, -2.5f),
		VGet(3.0f, 20.0f, 2.5f)
		};
		return TransformAABB(localBox, pos, playerAngle+ DX_PI_F);
	};
	void DrawDebug();
	bool GetIsHit() const { return isHit; }
private:
	void InitHitboxTable();
	void AttackMovement(const GameContext& context);
	void FrameLotate();
	void CollisionUpdate(const GameContext& context);
	Animstate animstate;

	Weapon items;
	Weapon items2;
	State currentState = State::Idle;
	float velocityY = 0.0f;
	bool isGround = true;
	float speed = 3.0;
	float Gravity = -0.8f;
	bool isMeleeAttacking = false;
	bool isHit = false;
	//マトリックス
	MATRIX mat1, mat2;
	//モデルのポジション
	float playerAngle = DX_PI_F;
	int FrameIndex;
	bool IsLoop = false;
	std::unordered_map<State, AttackHitboxData> attackHitboxTable;

};