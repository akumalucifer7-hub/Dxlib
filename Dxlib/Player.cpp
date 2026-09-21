#include"Player.h"
#include "StateIdle.h"

#define _HAS_STD_BYTE 0

void Player::Init(const GameContext& context)
{

	modelHandle = MV1LoadModel("Assets/3Dmodel/natsuki_karin_mmd_vrm/夏色花梨.pmx");
	centerFrameIndex = MV1SearchFrame(modelHandle, "センター");
	SwordItems.InitWeapon( "Assets/3Dmodel/weapons/匙式大直剣/匙式大直剣.pmx", "グリップ1");
	GunItemsRight.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
	GunItemsLeft.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
	pos = VGet(0.0f, 26.0f, -320.0f);
	GunItemsLeft.SetShapes("ステンレス", 1.0f);
	InitHitboxTable();
	// 初期状態をIdleに設定
	ChangeState(std::make_unique<StateIdle>(), context);
	context.animator->initAnimator(modelHandle, animstate, static_cast<int>(State::Idle));

	//HP=100.0f;
}

void Player::InitHitboxTable() {
	attackHitboxTable[State::Attack1] = {
		0.2f, 0.5f,
		{ VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
	};
	attackHitboxTable[State::Attack2] = {
	0.2f, 0.5f,
	{ VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
	};
	attackHitboxTable[State::Attack3] = {
		0.2f, 0.5f,
		{ VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) }
	};
	attackHitboxTable[State::Attack4] = {
		0.2f, 0.5f,
		{ VGet(-10.0f, 0.0f, 0.0f), VGet(10.0f, 20.0f, 40.0f) }
	};
	attackHitboxTable[State::Attackthrust] = {
		0.2f, 0.4f,
		{ VGet(-10.0f, 5.0f, 0.0f), VGet(10.0f, 25.0f, 70.0f) }
	};

	attackHitboxTable[State::AttackHelmbreak] = {
		0.3f, 0.7f,
		{ VGet(-30.0f, -10.0f, -30.0f), VGet(30.0f, 40.0f, 30.0f) }
	};
}

void Player::Update(const GameContext& context)
{
	isHit = false;
	 deltaTime = CalculateDeltaTime();
	if (parryTimer > 0.0f)
	{
		parryTimer -= deltaTime;
	}
	//移動と向きの計算
	updateMovement(context);
	// 物理計算の更新
	UpdatePhysics(context);
	// 現在の状態のUpdateを呼び出す
	if (currentState)
	{
		currentState->Update(*this, context);
	}
	FrameLotate();
	Animation(context, deltaTime);
	AttatchItems();
	CollisionUpdate(context);
}

void Player::ChangeState(std::unique_ptr<IPlayerState> newState, const GameContext& context)
{
	if (currentState)
	{
		// 古い状態の終了処理
		currentState->OnExit(*this, context);
	}

	currentState = std::move(newState);

	if (currentState)
	{
		// 新しい状態の開始処理
		currentState->OnEnter(*this, context);
	}
}

void Player::updateMovement(const GameContext& context)
{
	// 行列セット
	mat1 = MGetRotY(playerAngle);
	mat2 = MGetTranslate(pos);
	MV1SetMatrix(modelHandle, MMult(mat1, mat2));
}

// 物理計算の更新
void Player::UpdatePhysics(const GameContext& context)
{
	// ジャンプ・重力
	if (context.field->CheckGround(pos, groundY)) 
	{ isGround = true; }
	else 
	{
		groundY = -9999.0f;
		
		velocityY += Gravity;
		
		isGround = false;
	}

	pos.y += velocityY;
	float ceilingY = 0.0f;
	if (velocityY > 0.0f && context.field->CheckCeiling(pos, ceilingY))
	{
		pos.y = ceilingY - 18.0f; velocityY = 0.0f;
	}
	if (pos.y <= groundY || (isGround && pos.y <= groundY + 3.0f))
	{
		pos.y = groundY; velocityY = 0.0f; isGround = true;
	}
}

// アイテムのアタッチ処理
void Player::AttatchItems()
{
	if (isMeleeAttacking)
	{
		SwordItems.AttachWeapon(modelHandle, "右中指１", 0.2f, -0.4f, 0.0f, 0.0f, 0.0f, -90.0f);
	}
	else
	{
		SwordItems.AttachWeapon(modelHandle, "上半身", -4.0f, 6.5f, 1.5f, -135.0f, 90.0f, 0.0f);
		GunItemsRight.AttachWeapon(modelHandle, "右中指１", 0.3f, -0.3f, -0.15f, 0.0f, 90.0, -90.0f);
		GunItemsLeft.AttachWeapon(modelHandle, "左中指１", -0.3f, -0.3f, -0.15f, 0.0f, -90.0, 90.0f);
	}
}

void Player::Animation(const GameContext& context, float deltaTime)
{
	MV1PhysicsCalculation(modelHandle, deltaTime);

	// 現在の State から指定された currentAnimID と isAnimLoop を渡して更新
	// アニメーションIDが前回から変わっていれば、updateAnimator 内部で自動的にブレンド切り替えを発生させる
	context.animator->updateAnimator(
		modelHandle,
		animstate,
		static_cast<int>(currentAnimID),
		isAnimLoop,
		deltaTime
	);
}

void Player::FrameLotate()
{

	MV1ResetFrameUserLocalMatrix(modelHandle, centerFrameIndex);
	MATRIX rootMatrix = MV1GetFrameLocalMatrix(modelHandle, centerFrameIndex);
	rootMatrix.m[3][0] = 0.0f;
	rootMatrix.m[3][2] = 0.0f;
	MV1SetFrameUserLocalMatrix(modelHandle, centerFrameIndex, rootMatrix);

}

// 描画処理
void Player::Draw()
{

	//3Dの描画
	if (modelHandle != -1)
	{
		MV1DrawModel(modelHandle);
	}
	else
	{
		// 読み込みに失敗したらエラーログを出す
		DrawString(260, 300, "Model Load Failed", GetColor(255, 0, 0));
	}
	SwordItems.DrawWeapon();
	if (!isMeleeAttacking)
	{
		GunItemsRight.DrawWeapon();GunItemsLeft.DrawWeapon();
	}
} 


bool Player::GetCurrentAttackAABB(AABB& outAABB) const {
	if (!isMeleeAttacking) return false;

	auto it = attackHitboxTable.find(currentAnimID);
	if (it == attackHitboxTable.end()) return false;

	float normTime = animstate.GetNormalizedTime();
	const auto& data = it->second;

	// 現在のアニメーション再生時間が有効範囲内かチェック
	if (normTime >= data.startTime && normTime <= data.endTime) {
		outAABB = TransformAABB(data.localAABB, pos, playerAngle+ DX_PI_F);
		return true;
	}

	return false;
}


void Player::CollisionUpdate(const GameContext& context)
{
	if (!context.villain) return;

	// 1. プレイヤーの攻撃判定 vs 敵の被弾ボックス
	AABB attackAABB;
	if (GetCurrentAttackAABB(attackAABB))
	{
		if (CheckAABBOverlap(attackAABB, context.villain->GetHurtboxAABB()))
		{
			isHit = true;
		}
		else
		{
			isHit = false;
		}
	}

	// 2. 敵の攻撃判定 vs プレイヤーの被弾ボックス
	AABB villainAttackAABB;
	if (context.villain->GetCurrentAttackAABB(villainAttackAABB))
	{
		// まず重なっている場合のみ内部処理へ入る
		if (CheckAABBOverlap(villainAttackAABB, GetHurtboxAABB()))
		{
			// ヒットしている状態の中で「パリィ」か「被弾」かを分岐
			if (context.input->IsMouseTrigger(InputValidation::Mouse::Left))
			{
				//ChangeState(std::make_unique<StateSwordShinogi>(), context);
				isMeleeAttacking = true;
				//comboStep = 1;
				parryTimer = 0.6f;
				VECTOR enemyPos = context.villain->GetPos();
				VECTOR diff = VSub(enemyPos, pos);
				playerAngle = atan2f(diff.x, diff.z) + DX_PI_F;
			}
			else if (context.input->IsKeyTrigger(KEY_INPUT_LALT) || context.input->IsButtonTrigger(XINPUT_BUTTON_B))
			{
				//ChangeState(std::make_unique<StateEvade>(), context);
				isMeleeAttacking = false;
				IsInvincible = true;
				//comboStep = 0;
				//isNextAttackRequested = false;
			}
			else if (currentAnimID != State::SwordShinogi && currentAnimID != State::Damage1 && parryTimer <= 0.0f) 
			{
				//ChangeState(std::make_unique<StateDamage>(), context);
				//hp -= villain.GetDamageAmount(); // 敵の攻撃力を取得してHPを減少
			}
		}
	}
	ResolveAABBPushoutXZ(GetHurtboxAABB(), pos, context.villain->GetHurtboxAABB(), context.villain->GetPos());
}

void Player::DrawDebug() {
	// 1. プレイヤー自身の被弾ボックス (Hurtbox) を緑色で描画
	AABB hurtbox = GetHurtboxAABB();
	DebugRenderer::DrawAABB(hurtbox, DebugColor::Hurtbox);

	// 2. 攻撃判定発生中の場合、攻撃ボックス (Hitbox) を赤色で描画
	AABB attackAABB;
	if (GetCurrentAttackAABB(attackAABB)) {
		// ヒットが成立しているフレームなら黄色、それ以外は赤色
		unsigned int color = isHit ? DebugColor::ActiveHit : DebugColor::Hitbox;
		DebugRenderer::DrawAABB(attackAABB, color);
	}
	DrawFormatString(10, 500, GetColor(255, 255, 255), "AnimID: %d, AttachIndex: %d", static_cast<int>(currentAnimID), animstate.AttachIndex);
}