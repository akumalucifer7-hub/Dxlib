#include "EffectManager.h"


bool EffectManager::Init(int maxParticles)
{
	// Effekseerを初期化する（画面に表示する最大パーティクル数を設定）
	if (Effekseer_Init(maxParticles) == -1)
	{
		return false;
	}

	// フルスクリーン切り替えでリソースが消えるのを防ぐ
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// デバイスロスト時のコールバックを設定
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Zバッファを有効にする（Effekseer使用時は必須）
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	return true;
}

void EffectManager::Load(const std::string& key, const char* filePath, float defaultScale)
{
	if (effectHandles.find(key) == effectHandles.end())
	{
		int handle = LoadEffekseerEffect(filePath, defaultScale);
		if (handle != -1)
		{
			effectHandles[key] = handle;
		}
	}
}

int EffectManager::Play(const std::string& key, const VECTOR& pos, float rotY, float scale)
{
	auto it = effectHandles.find(key);
	if (it == effectHandles.end()) return -1;

	// EffekseerForDXLib 準拠の3Dエフェクト再生関数
	int playingHandle = PlayEffekseer3DEffect(it->second);
	if (playingHandle != -1)
	{
		SetPosPlayingEffekseer3DEffect(playingHandle, pos.x, pos.y, pos.z);
		SetRotationPlayingEffekseer3DEffect(playingHandle, 0.0f, rotY, 0.0f);
		SetScalePlayingEffekseer3DEffect(playingHandle, scale, scale, scale);
	}
	return playingHandle;
}

void EffectManager::SetPos(int playingHandle, const VECTOR& pos)
{
	if (playingHandle != -1)
	{
		SetPosPlayingEffekseer3DEffect(playingHandle, pos.x, pos.y, pos.z);
	}
}

void EffectManager::SetRotation(int playingHandle, float rotX, float rotY, float rotZ)
{
	if (playingHandle != -1)
	{
		SetRotationPlayingEffekseer3DEffect(playingHandle, rotX, rotY, rotZ);
	}
}

bool EffectManager::IsPlaying(int playingHandle) const
{
	if (playingHandle == -1) return false;
	// 0: 再生中, -1: 再生終了または無効
	return IsEffekseer3DEffectPlaying(playingHandle) == 0;
}

void EffectManager::Update()
{
	// 再生中の全エフェクトを更新
	UpdateEffekseer3D();
}

void EffectManager::Draw()
{
	// DXライブラリのカメラとEffekseerのカメラを同期
	Effekseer_Sync3DSetting();

	// エフェクトを描画（※事前に何かしらの描画が行われている必要がある）
	DrawEffekseer3D();
}

void EffectManager::Finalize()
{
	// 読み込んだ全エフェクトリソースを削除
	for (auto& pair : effectHandles)
	{
		DeleteEffekseerEffect(pair.second);
	}
	effectHandles.clear();

	// Effekseerを終了する
	Effkseer_End();
}