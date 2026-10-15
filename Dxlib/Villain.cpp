#include "Villain.h"
#include "Player.h"

void Villain::Init(const GameContext& context)
{
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
	modelHandle = MV1LoadModel("Assets/3Dmodel/koharu_rikka_natsufuku_mmd_vrm/KoharuRikka_Natsufuku.pmx");
	centerFrameIndex = MV1SearchFrame(modelHandle, "センター");
	context.animator->initAnimator(modelHandle, animstate, static_cast<int>(State::Idle));

	items.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
	items2.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");

	collision.Init();
}

void Villain::Update(const GameContext& context)
{
	float deltaTime = CalculateDeltaTime();

	// 被弾アニメーションの終了判定
	if (currentState == State::Damage1)
	{
		if (animstate.isAnimationEnd)
		{
			currentState = State::Idle;
			isMeleeAttacking = false;
		}
	}
	else
	{
		// 被弾中でなければAI/攻撃行動を実行
		AttackMovement(context);
	}

	MV1SetPosition(modelHandle, pos);
	IsLoop = (currentState == State::Idle || currentState == State::Run);

	context.animator->updateAnimator(modelHandle, animstate, static_cast<int>(currentState), IsLoop, deltaTime);

	items.AttachWeapon(modelHandle, "右中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);
	items2.AttachWeapon(modelHandle, "左中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);

	FrameLotate();

	// 当たり判定更新
	collision.Update(*this, context);
}

void Villain::OnTakeDamage()
{
	// 既に被弾ステートでなければ被弾モーションへ移行
	if (currentState != State::Damage1)
	{
		currentState = State::Damage1;
		isMeleeAttacking = false;
		animstate.PlayTime = 0.0f; // アニメーションを最初から再生
	}
}

void Villain::AttackMovement(const GameContext& context)
{
	if (!context.player) return;

	VECTOR playerPos = context.player->GetPos();
	VECTOR diff = VSub(playerPos, pos);

	float distance = sqrtf(diff.x * diff.x + diff.z * diff.z);
	const float attackRange = 30.0f;

	if (currentState != State::Attack1)
	{
		if (distance <= attackRange)
		{
			playerAngle = atan2f(diff.x, diff.z) + DX_PI_F;
			MV1SetRotationXYZ(modelHandle, VGet(0.0f, playerAngle, 0.0f));

			currentState = State::Attack1;
			isMeleeAttacking = true;
			animstate.PlayTime = 0.0f;
		}
		else
		{
			currentState = State::Idle;
			isMeleeAttacking = false;
		}
	}

	if (currentState == State::Attack1 && animstate.isAnimationEnd)
	{
		currentState = State::Idle;
		isMeleeAttacking = false;
	}
}

void Villain::FrameLotate()
{
	MV1ResetFrameUserLocalMatrix(modelHandle, centerFrameIndex);
	MATRIX rootMatrix = MV1GetFrameLocalMatrix(modelHandle, centerFrameIndex);
	rootMatrix.m[3][0] = 0.0f;
	rootMatrix.m[3][2] = 0.0f;
	MV1SetFrameUserLocalMatrix(modelHandle, centerFrameIndex, rootMatrix);
}

void Villain::Draw()
{
	if (modelHandle != -1)
	{
		MV1DrawModel(modelHandle);
	}
	else
	{
		DrawString(260, 300, "Model Load Failed", GetColor(255, 0, 0));
	}
	items.DrawWeapon();
	items2.DrawWeapon();
}

void Villain::DrawDebug()
{
	collision.DrawDebug(*this);
}