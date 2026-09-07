#include"Villain.h"
#include"Player.h"

void Villain::Init(const GameContext& context)
{
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	modelHandle = MV1LoadModel("Assets/3Dmodel/koharu_rikka_natsufuku_mmd_vrm/KoharuRikka_Natsufuku.pmx");
	centerFrameIndex = MV1SearchFrame(modelHandle, "センター");
	context.animator->initAnimator(modelHandle, animstate, static_cast<int>(State::Idle));
	prevTime = GetNowHiPerformanceCount();
	items.InitItem("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
	items2.InitItem("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
	InitHitboxTable();
}

void Villain::InitHitboxTable()
{
	// Attack1時の攻撃AABB設定 (例: アニメーション時間の20%～60%の間、前方へ伸びる判定)
	attackHitboxTable[State::Attack1] = {
		0.2f, 0.6f,
		{ VGet(-5.0f, 0.0f, 0.0f), VGet(5.0f, 15.0f, 15.0f) }
	};
}
void Villain::Update(const GameContext& context)
{
	isHit = false;
	if (currentState == State::Damage1 && animstate.isAnimationEnd)
	{
		currentState = State::Idle;
		isMeleeAttacking = false;
	}
	LONGLONG currentTime = GetNowHiPerformanceCount();
	float deltaTime = CalculateDeltaTime();
	MV1SetPosition(modelHandle, pos);
	IsLoop = (currentState == State::Idle || currentState == State::Run);
	AttackMovement(context);
	context.animator->updateAnimator(modelHandle, animstate, static_cast<int>(currentState), IsLoop, deltaTime);
	items.AttachItem(modelHandle, "右中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);
	items2.AttachItem(modelHandle, "左中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);
	FrameLotate();
	prevTime = currentTime;
	CollisionUpdate(context);
}
void Villain::Draw()
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
	items.DrawItem();
	items2.DrawItem();

}

void Villain::AttackMovement(const GameContext& context)
{
	if (currentState == State::Damage1) return;
	if (context.player)
	{
		VECTOR playerPos = context.player->GetPos();
		VECTOR diff = VSub(playerPos, pos);

		// XZ平面上の距離を計算
		float distance = sqrtf(diff.x * diff.x + diff.z * diff.z);
		const float attackRange = 30.0f; // 至近距離とみなす閾値（必要に応じて調整）

		// 攻撃中でない場合のみ距離判定を行う
		if (currentState != State::Attack1)
		{
			if (distance <= attackRange)
			{
				// プレイヤーの方向を計算して向く
				playerAngle = atan2f(diff.x, diff.z)+ DX_PI_F;
				MV1SetRotationXYZ(modelHandle, VGet(0.0f, playerAngle, 0.0f));
				// 攻撃ステートへ遷移
				currentState = State::Attack1;
				isMeleeAttacking = true;

			}
			else
			{
				currentState = State::Idle;
				isMeleeAttacking = false;
			}
		}
	}

	// 攻撃アニメーション終了時に Idle ステートへ戻す
	if (currentState == State::Attack1 && animstate.isAnimationEnd)
	{
		currentState = State::Idle;
	}
}

// モデルのフレームを固定させる
void Villain::FrameLotate()
{

	MV1ResetFrameUserLocalMatrix(modelHandle, centerFrameIndex);
	MATRIX rootMatrix = MV1GetFrameLocalMatrix(modelHandle, centerFrameIndex);
	rootMatrix.m[3][0] = 0.0f;
	rootMatrix.m[3][2] = 0.0f;
	MV1SetFrameUserLocalMatrix(modelHandle, centerFrameIndex, rootMatrix);

}
bool Villain::GetCurrentAttackAABB(AABB& outAABB) const {
	if (!isMeleeAttacking) return false;

	auto it = attackHitboxTable.find(currentState);
	if (it == attackHitboxTable.end()) return false;

	float normTime = animstate.GetNormalizedTime();
	const auto& data = it->second;

	// 現在のアニメーション再生時間が有効範囲内かチェック
	if (normTime >= data.startTime && normTime <= data.endTime) {
		outAABB = TransformAABB(data.localAABB, pos, playerAngle + DX_PI_F);
		return true;
	}

	return false;
}


void Villain::DrawDebug() 
{
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


void Villain::CollisionUpdate(const GameContext& context)
{
	if (!context.player) return;
	AABB attackAABB;
	if (GetCurrentAttackAABB(attackAABB))
	{
		// 攻撃範囲と敵の被弾ボックスの衝突判定
		if (CheckAABBOverlap(attackAABB, context.player->GetHurtboxAABB()))
		{
			// 衝突した場合の処理（例: ダメージを与える）
			isHit = true;
		}
		else
		{
			isHit = false;
		}
	}
	//プレイヤーの攻撃判定と敵の被弾ボックスの衝突判定
	AABB playerAttackAABB;
	if (context.player->GetCurrentAttackAABB(playerAttackAABB))
	{
		if (CheckAABBOverlap(playerAttackAABB, GetHurtboxAABB()))
		{
			isHit = true;
			currentState = State::Damage1;
			animstate.PlayTime = 0.0f;
			//hp-= context.player->GetAttackPower(); // プレイヤーの攻撃力を減算
		}
	}
	ResolveAABBPushoutXZ(GetHurtboxAABB(), pos, context.player->GetHurtboxAABB(), context.player->GetPos());
}