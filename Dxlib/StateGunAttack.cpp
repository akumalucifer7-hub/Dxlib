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
    Player* pPlayer = dynamic_cast<Player*>(&owner);
    float normTime = owner.GetAnimNormalizedTime();

    // ★ 1. アニメーションが35%に達した瞬間に発砲
    if (!hasFired && normTime >= 0.35f)
    {
        if (pPlayer)
        {
            Weapon& gun = pPlayer->GetRightGun();
            VECTOR muzzlePos = gun.GetMuzzlePosition();
            VECTOR muzzleDir = gun.GetMuzzleForward();

            // 弾丸の生成
            BulletManager::Instance().SpawnBullet(muzzlePos, muzzleDir, 150.0f);

            // SE再生
            AudioManager::Instance().Play("GunShot", DX_PLAYTYPE_BACK);

            hasFired = true;
        }
    }

    // モーション終了で Idle へ戻る
    if (owner.GetIsAnimationEnd())
    {
        owner.ChangeState(std::make_unique<StateIdle>(), context);
    }
}

void GunAttackState::OnExit(Character& owner, const GameContext& context)
{
}