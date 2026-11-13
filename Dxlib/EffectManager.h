#pragma once
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include <unordered_map>
#include <string>

class EffectManager
{
public:
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	// Effekseerの初期化（DxLib_Init() の直後に呼ぶ）
	bool Init(int maxParticles = 8000);

	// エフェクトリソースの読み込み
	void Load(const std::string& key, const char* filePath, float defaultScale = 1.0f);

	// 3Dエフェクトの再生（再生中ハンドルを返す）
	int Play(const std::string& key, const VECTOR& pos, float rotY = 0.0f, float scale = 1.0f);

	// 再生中エフェクトの座標・回転の更新（銃口への追従用）
	void SetPos(int playingHandle, const VECTOR& pos);
	void SetRotation(int playingHandle, float rotX, float rotY, float rotZ);
	bool IsPlaying(int playingHandle) const;

	// 毎フレームの更新
	void Update();

	// 毎フレームの描画（3Dモデルや背景の描画後に呼ぶ）
	void Draw();

	// リソース解放と終了処理（DxLib_End() の直前に呼ぶ）
	void Finalize();

private:
	EffectManager() = default;
	~EffectManager() = default;

	std::unordered_map<std::string, int> effectHandles;
};