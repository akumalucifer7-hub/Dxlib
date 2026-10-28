#include"Winmain.h"
#include "GameContext.h"
#include <algorithm>
#include <windows.h>
#pragma warning(disable: C4819)
using namespace DxLib;

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int SCREENWIDTH ;
	int SCREENHEIGHT;
	int msgboxID = MessageBoxW(
		NULL,
		L"高画質モード(影あり・アンチエイリアス有効)で起動しますか？\n[いいえ]を選択すると低負荷モード(影なし)で起動します。",
		L"グラフィックモードを選択してください",
		MB_YESNO | MB_ICONQUESTION
	);
	if (msgboxID == IDCANCEL)
	{
		// 「キャンセル」または「×」ボタンが押されたら、何もせずにプログラムを終了する
		return 0;
	}
	int selectMode = 0; // 0: 低負荷, 1: 高負荷
	if (msgboxID == IDYES)
	{
		selectMode = 1; // 「はい」が選ばれたら高負荷モード
		SCREENWIDTH = 1920;
		SCREENHEIGHT = 1080;
	}
	else
	{
		selectMode = 0; // 「いいえ」が選ばれたら低負荷モード
		SCREENWIDTH = 1280;
		SCREENHEIGHT = 720;
	}
	
	SetUseCharSet(DX_CHARSET_UTF8);
	SetGraphMode(SCREENWIDTH, SCREENHEIGHT, 32);
	SetUse3DFlag(TRUE);
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	SetDrawScreen(DX_SCREEN_BACK);

	//SetUseSetDrawScreenSettingReset(FALSE);
	// 起動時の負荷モード選択処理

	
	gameScene.InitGame(selectMode, SCREENWIDTH, SCREENHEIGHT);

	//ゲームループの実行
	GameContext context;
	LONGLONG prevTime = GetNowHiPerformanceCount();
	while (ProcessMessage() == 0 && ClearDrawScreen() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		//画面のクリア
		ClearDrawScreen();
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		//ゲームシーンの更新
		LONGLONG currentTime = GetNowHiPerformanceCount();
		float rawDeltaTime = static_cast<float>(currentTime - prevTime) / 1000000.0f;
		prevTime = currentTime;
		context.deltaTime = (std::min)(rawDeltaTime, 0.1f);
		inputValidation.Instance().ValidateInputUpdate();
		if (gameScene.GetIsGameOver())
		{
			modestate = ModeState::GameOverMode;
		}
		switch (modestate)
		{
		case ModeState::GameMode:
			gameScene.UpdateGame(inputValidation.Instance(), SCREENWIDTH, SCREENHEIGHT);
			gameScene.DrawGame();
			break;
		case ModeState::TitleMode:
			// タイトル画面の更新処理
			break;
		case ModeState::GameOverMode:
			// ゲームオーバー画面の更新処理
			DrawString(500, 500, "Game Over", GetColor(255, 0, 0));
			if (input.IsMousePressed(InputValidation::Mouse::Left))
			{
				// ゲームオーバー画面でEnterキーが押された場合、ゲームを再起動する
				gameScene.InitGame(selectMode, SCREENWIDTH, SCREENHEIGHT);
				modestate = ModeState::GameMode;
			}
			break;
		}
		//DrawCustomFPS(10, 10);
		DrawFormatString(10, 10, GetColor(255, 255, 255), "FPS: %.2f", GetFPS());

		//画面の更新
		ScreenFlip();

	}
	// ＤＸライブラリの後始末
	DxLib_End();
	//プログラミング終了
	return 0;				// ソフトの終了 
}
