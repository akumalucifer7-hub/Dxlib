#include "StateGunAttack.h"
#include "StateIdle.h"
#include "Player.h"
#include "AudioManager.h"
#include "Bullet.h"

void GunAttackState::OnEnter(Character& owner, const GameContext& context)
{
    owner.SetAnimation(static_cast<int>(Character::CharacterStateID::GunAttack), false);
    hasFired = false;
}

void GunAttackState::Update(Character& owner, const GameContext& context)
{
    if (owner.GetController())
    {
        CommandState cmd = owner.GetController()->GetCommand(owner, context);
        if (cmd.isAttack) // 銃攻撃の入力フラグ（cmd.isGunAttack 等がある場合は適宜差し替え）
        {
            isNextAttackRequested = true;
        }
    }

    Player* pPlayer = dynamic_cast<Player*>(&owner);
    float normTime = owner.GetAnimNormalizedTime();

    if (!hasFired && normTime >= 0.35f)
    {
        if (pPlayer)
        {
            Weapon& rightGun = pPlayer->GetRightGun();
			Weapon& leftGun = pPlayer->GetLeftGun();
            // 1. 発射位置：銃口ボーンのリアルタイム・ワールド座標
            VECTOR muzzlePos1 = rightGun.GetMuzzlePosition();
            VECTOR muzzzlePos2= leftGun.GetMuzzlePosition();
            // 2. プレイヤーの真の正面向き（DX_PI_F の180度反転を考慮してマイナスを付与）
            float angle = pPlayer->GetAngle();
            VECTOR forwardDir = VGet(-sinf(angle), 0.0f, -cosf(angle));
            forwardDir = VNorm(forwardDir); // 正規化（長さ1のベクトルにする）

            // 3. 弾丸の生成（銃口の位置から、プレイヤーの正面へ水平に直線飛翔）
            BulletManager::Instance().SpawnBullet(muzzlePos1, forwardDir, 180.0f);
            BulletManager::Instance().SpawnBullet(muzzzlePos2, forwardDir, 180.0f);

            AudioManager::Instance().Play("GunShot", DX_PLAYTYPE_BACK);
            hasFired = true;
        }
    }

    if (owner.GetIsAnimationEnd())
    {
		if (isNextAttackRequested)
		{
			// 次の攻撃への遷移要求がある場合、再度 GunAttackState に遷移
			owner.ChangeState(std::make_unique<GunAttackState>(), context);
		}
		else
		{
			// アニメーション終了後は Idle 状態に戻す
			owner.ChangeState(std::make_unique<StateIdle>(), context);
		}
    }
}

void GunAttackState::OnExit(Character& owner, const GameContext& context)
{
}