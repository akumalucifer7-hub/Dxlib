// AudioManager.h
#pragma once
#include "DxLib.h"
#include <unordered_map>
#include <string>

class AudioManager
{
public:
    static AudioManager& Instance()
    {
        static AudioManager instance;
        return instance;
    }

    // 音声のロード（起動時に呼ぶ）
    void Load(const std::string& key, const char* filePath)
    {
        if (sounds.find(key) == sounds.end())
        {
            sounds[key] = LoadSoundMem(filePath);
        }
    }

    // 音声の再生（デフォルトは1回のみ再生する DX_PLAYTYPE_BACK）
    void Play(const std::string& key, int playType = DX_PLAYTYPE_BACK)
    {
        auto it = sounds.find(key);
        if (it != sounds.end())
        {
            // ループ再生の場合は、既に再生中かチェックして多重再生を防ぐ
            if (playType == DX_PLAYTYPE_LOOP && CheckSoundMem(it->second) == 1) return;

            PlaySoundMem(it->second, playType);
        }
    }

    // 音声の停止（ループ再生を止める時に呼ぶ）
    void Stop(const std::string& key)
    {
        auto it = sounds.find(key);
        if (it != sounds.end())
        {
            StopSoundMem(it->second);
        }
    }

private:
    std::unordered_map<std::string, int> sounds;
};