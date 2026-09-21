#include"Player.h"

#define _HAS_STD_BYTE 0

void Player::Init(const GameContext& context)
{

	modelHandle = MV1LoadModel("Assets/3Dmodel/natsuki_karin_mmd_vrm/夏色花梨.pmx");
	// 初期ステートをintにキャストして渡す
	context.animator->initAnimator(modelHandle, animstate, static_cast<int>(State::Idle));
	centerFrameIndex = MV1SearchFrame(modelHandle, "センター");
	SwordItems.InitWeapon( "Assets/3Dmodel/weapons/匙式大直剣/匙式大直剣.pmx", "グリップ1");
	GunItemsRight.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
	GunItemsLeft.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
	pos = VGet(0.0f, 26.0f, -320.0f);
	GunItemsLeft.SetShapes("ステンレス", 1.0f);
	InitHitboxTable();
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
	float deltaTime = CalculateDeltaTime();
	if (parryTimer > 0.0f)
	{
		parryTimer -= deltaTime;
	}
	AttackState(context);
	EvadeMovement(context);
	//移動と向きの計算
	updateMovement(context);
	JumpAction(context);
	// 物理計算の更新
	UpdatePhysics(context);
	UpdatePlayerState();
	FrameLotate();
	Animation(context, deltaTime);
	AttatchItems();
	CollisionUpdate(context);
}

void Player::AttackState(const GameContext& context)
{
	if (currentState == State::Damage1) return;
	// 今フレームで左クリックされていて、かつ前フレームではクリックされていなければ「押した瞬間」
	bool isAttackPressed = (context.input->IsMouseTrigger(InputValidation::Mouse::Left)|| context.input->IsButtonTrigger(XINPUT_BUTTON_X));
	bool isGunAttackPressed = context.input->IsMousePressed(InputValidation::Mouse::Right) || context.input->IsButtonTrigger(XINPUT_BUTTON_Y);
	// 来フレームのために状態を保存

	//攻撃の開始・コンボ判定
	if (!isMeleeAttacking)
	{
		// 先に特殊攻撃（シフトキー必須）の判定を行う
		if (isAttackPressed && isGround && context.input->IsKeyPressed(KEY_INPUT_LSHIFT) &&
			(currentState == State::Idle || currentState == State::Run || currentState == State::Walk))
		{
			isMeleeAttacking = true;
			comboStep = 1;
			isNextAttackRequested = false;
			currentState = State::Attackthrust;
		}
		// 空中の攻撃（ヘルムブレイカー）：JumpかFallのとき
		else if (isAttackPressed && !isGround && context.input->IsKeyPressed(KEY_INPUT_LSHIFT) &&
			(currentState == State::Jump || currentState == State::Fall))
		{
			isMeleeAttacking = true;
			comboStep = 1;
			isNextAttackRequested = false;
			currentState = State::AttackHelmbreak;
		}
		//  シフトキーを押していない場合の通常攻撃（1段目）を最後に判定
		else if (isAttackPressed && (currentState == State::Idle || currentState == State::Run || currentState == State::Walk))
		{
			isMeleeAttacking = true;
			comboStep = 1;
			isNextAttackRequested = false;
			currentState = State::Attack1;
		}
		else if (isAttackPressed && !isGround &&
			(currentState == State::Jump || currentState == State::Fall))
		{
			isMeleeAttacking = true;
			comboStep = 1;
			isNextAttackRequested = false;
			currentState = State::AerialAttack;
			velocityY = 0.0f;
		}
	}
	else
	{
		// 攻撃中かつ、まだ最大コンボ数（5段）に達していない場合、クリックで次を予約
		if (isAttackPressed && comboStep < 4)
		{
			isNextAttackRequested = true;
		}

		// アニメーションが終了したタイミングでの処理
		if (animstate.isAnimationEnd)
		{
			if (currentState == State::AttackHelmbreak)
			{
				currentState = State::AttackHelmbreak;
				Gravity = -1.4f;
			}
			else if (isNextAttackRequested)
			{
				// 予約があれば次のコンボへ移行
				comboStep++;
				isNextAttackRequested = false;
				if (currentState == State::AerialAttack)
				{
					// 空中コンボ2段目へ
					currentState = State::AerialAttack2;
					velocityY = 0.0f; // 2段目発動時にも落下をリセット
				}
				else
				{
					switch (comboStep)
					{
					case 2:
						currentState = State::Attack2;
						break;
					case 3:
						currentState = State::Attack3;
						break;
					case 4:
						currentState = State::Attack4;
						break;
					}
				}
			}
			else
			{
				// 予約がなければ攻撃シーケンス終了
				isMeleeAttacking = false;
				comboStep = 0;
			}
		}

	}
	if (!isGunAttacking)
	{
		if (isGunAttackPressed && (currentState == State::Idle || currentState == State::Run))
		{
			isGunAttacking = true;
			currentState = State::GunAttack;
		}

	}
	else
	{
		if (!isGunAttackPressed)
		{
			isGunAttacking = false;
		}
	}
}
void Player::EvadeMovement(const GameContext& context)
{
	// 地上にいて、かつ攻撃中でない場合のみ回避可能にする（論理エラー修正）
	if (isGround && !isMeleeAttacking && !isGunAttacking && currentState != State::Damage1 && currentState != State::Evade)
	{
		bool isEvadePressed = context.input->IsKeyTrigger(KEY_INPUT_LALT) || context.input->IsButtonTrigger(XINPUT_BUTTON_B);
		if (isEvadePressed)
		{
			// 攻撃フラグ等をリセットして回避に遷移
			isMeleeAttacking = false;
			isGunAttacking = false;
			comboStep = 0;
			isNextAttackRequested = false;
			currentState = State::Evade;

			// ※後方回避にするため、入力方向にキャラクターを向かせる処理は削除しました
		}
	}
}
void Player::updateMovement(const GameContext& context)
{
	if (currentState == State::Evade)
	{
		EvadeAction(context); // 回避中の移動処理
	}
	else if (isMeleeAttacking)
	{
		MeleeForwardMovement(context);
	}
	else
	{
		ApplyNormalMovement(context);
	}

	// 行列セット
	mat1 = MGetRotY(playerAngle);
	mat2 = MGetTranslate(pos);
	MV1SetMatrix(modelHandle, MMult(mat1, mat2));
}
//攻撃時の移動処理
void Player::MeleeForwardMovement(const GameContext& context)
{
	if (isGround)
	{
		float attackSpeed = 0.0f;
		switch (comboStep)
		{
		case 1: attackSpeed = 1.0f; break;
		case 2: attackSpeed = 1.2f; break;
		case 3: attackSpeed = 1.4f; break;
		case 4: attackSpeed = 2.0f; break;
		}
		if (animstate.GetNormalizedTime() > 0.2f && animstate.GetNormalizedTime() < 0.5f)
		{
			//攻撃方向へ前進させる処理
			VECTOR forward = VGet(sinf(playerAngle + DX_PI_F), 0.0f, cosf(playerAngle + DX_PI_F));
			VECTOR moveOffset = VScale(forward, attackSpeed);

			VECTOR finalPos = context.field->WallCollision(pos, moveOffset);
			pos.x = finalPos.x;
			pos.z = finalPos.z;
		}
		if (currentState == State::Attackthrust)
		{
			if (animstate.GetNormalizedTime() > 0.2f && animstate.GetNormalizedTime() < 0.5f)
			{
				//攻撃方向へ前進させる処理
				VECTOR forward = VGet(sinf(playerAngle + DX_PI_F), 0.0f, cosf(playerAngle + DX_PI_F));
				VECTOR moveOffset = VScale(forward, attackSpeed * 2.0f);
				VECTOR finalPos = context.field->WallCollision(pos, moveOffset);
				pos.x = finalPos.x;
				pos.z = finalPos.z;
			}
		}

	}
}
// 通常移動処理
void Player::ApplyNormalMovement(const GameContext& context)
{
	MATRIX cameraRotMat = MGetRotY(context.camera->GetCameraYaw());

	playerspeed = 3.0f;
	context.input->GetLeftStick(LX, LY, 0.2f);

	if (context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP)) { LY = 1.0f; }
	if (context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN)) { LY = -1.0f; }
	if (context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT)) { LX = -1.0f; }
	if (context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT)) { LX = 1.0f; }
	if (isGunAttacking|| currentState == State::Damage1)
	{
		LX = 0.0f;	
		LY = 0.0f;
	}
	VECTOR localMoveVec = VGet(LX, 0.0f, LY);
	moveVec = VTransform(localMoveVec, cameraRotMat);
	float length = sqrtf(moveVec.x * moveVec.x + moveVec.z * moveVec.z);

	if (length > 0.0f)
	{
		moveVec = VNorm(moveVec);
		playerAngle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
		if (currentState == State::Run)
		{
			playerspeed = 3.0f;
		}
		else
		{
			playerspeed = 1.0f;
		}
		VECTOR moveOffset = VScale(moveVec, playerspeed);
		VECTOR finalPos = context.field->WallCollision(pos, moveOffset);
		pos.x = finalPos.x;
		pos.z = finalPos.z;
	}
}
void Player::EvadeAction(const GameContext& context)
{
	float evadeSpeed = 7.0f;

	// ★前方のベクトルを反転させて、後方（バックステップ）のベクトルを作る
	VECTOR backward = VGet(-sinf(playerAngle + DX_PI_F), 0.0f, -cosf(playerAngle + DX_PI_F));
	VECTOR moveOffset = VScale(backward, evadeSpeed);

	if (animstate.GetNormalizedTime() > 0.2f && animstate.GetNormalizedTime() < 0.5f)
	{
		VECTOR finalPos = context.field->WallCollision(pos, moveOffset);
		pos.x = finalPos.x;
		pos.z = finalPos.z;
	}
}
void Player::JumpAction(const GameContext& context)
{
	if ((context.input->IsKeyTrigger(KEY_INPUT_SPACE) && isGround &&( !isMeleeAttacking|| currentState != State::Damage1) && currentState != State::Fall)
		|| context.input->IsButtonTrigger(XINPUT_BUTTON_A) && isGround && (!isMeleeAttacking || currentState != State::Damage1) && currentState != State::Fall)
	{
		if (!context.field->IsInsideWall(pos))
		{
			velocityY += 12.5f; isGround = false;
		}
	}
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
		if (currentState == State::AerialAttack)
		{
			velocityY = 0.0f;
		}
		else
		{
			velocityY += Gravity;
		}
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
		if (currentState == State::AttackHelmbreak)
		{
			isMeleeAttacking = false;
			comboStep = 0;
			Gravity = -0.8f;
		}
		if (currentState == State::AerialAttack)
		{
			pos.y = pos.y;
			comboStep = 0;
		}
	}
}

// プレイヤーのステートを更新する関数
void Player::UpdatePlayerState()
{
	// currentState != State::Evade を追加して上書きを防ぐ
	if (!isMeleeAttacking && currentState != State::Damage1 && currentState != State::SwordShinogi && currentState != State::Evade)
	{
		if (isGunAttacking)
		{
			currentState = State::GunAttack;
		}
		else
		{
			currentState = State::Idle;
			if (LX != 0.0f || LY != 0.0f) {
				if (CheckHitKey(KEY_INPUT_LSHIFT)) { currentState = State::Walk; }
				else { currentState = State::Run; }
			}
		}

		if (velocityY > 0.0f && currentState != State::Jump) {
			currentState = State::Jump;
			MV1PhysicsResetState(modelHandle);
		}
		if (velocityY < 0.0f) { currentState = State::Fall; }
	}

	//  回避（Evade）アニメーション終了時にも Idle に戻るように追加
	if ((currentState == State::SwordShinogi || currentState == State::Damage1 || currentState == State::Evade) && animstate.isAnimationEnd)
	{
		currentState = State::Idle;
		isMeleeAttacking = false;
		comboStep = 0;
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
// アニメーションの更新
void Player::Animation(const GameContext& context, float deltaTime)
{
	MV1PhysicsCalculation(modelHandle, deltaTime);
	IsLoop = (currentState == State::Idle || currentState == State::Run || currentState == State::Walk || currentState == State::GunAttack);
	context.animator->updateAnimator(modelHandle, animstate, static_cast<int>(currentState), IsLoop, deltaTime);

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

	auto it = attackHitboxTable.find(currentState);
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
				currentState = State::SwordShinogi;
				isMeleeAttacking = true;
				comboStep = 1;
				parryTimer = 0.6f;
				VECTOR enemyPos = context.villain->GetPos();
				VECTOR diff = VSub(enemyPos, pos);
				playerAngle = atan2f(diff.x, diff.z) + DX_PI_F;
			}
			else if (context.input->IsKeyTrigger(KEY_INPUT_LALT) || context.input->IsButtonTrigger(XINPUT_BUTTON_B))
			{
				currentState = State::Evade;
				isMeleeAttacking = false;
				comboStep = 0;
				isNextAttackRequested = false;
			}
			else if (currentState != State::SwordShinogi && currentState != State::Damage1 && parryTimer <= 0.0f)
			{
				currentState = State::Damage1;
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
}