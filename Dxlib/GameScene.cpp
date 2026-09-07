#include "GameScene.h"

void GameScene::InitGame(int selectMode, int SCREENWIDTH, int SCREENHEIGHT)
{

	switch (selectMode)
	{
	case 0:
		SetHighMode = false;
		break;
	case 1:
		SetHighMode = true;
		break;
	}

	// 選択されたモードに応じて初期化を切り替える
	if (SetHighMode)
	{
		SetFullSceneAntiAliasingMode(8, 8);
		// シャドウマップの作成
		ShadowMapHandle = MakeShadowMap(2048, 2048);
		SetShadowMapDrawArea(ShadowMapHandle, VGet(-200.0f, -200.0f, -200.0f), VGet(200.0f, 200.0f, 200.0f));
	}
	else
	{
		// 低負荷時はアンチエイリアスの数値を下げる
		SetFullSceneAntiAliasingMode(4, 4);
	}
	 context = CreateContext();
	gameObjects.clear();
	gameObjects.push_back(&player);
	gameObjects.push_back(&villain);
	gameObjects.push_back(&field);

	camera.initCamera(SCREENWIDTH, SCREENHEIGHT);
	for (GameObject* obj : gameObjects)
	{
		obj->Init(context);
	}
	SetGlobalAmbientLight(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));

	// ライトの向きを設定
	VECTOR lightDir = VGet(0.5f, -0.5f, 0.5f);
	SetLightDirection(lightDir);
	if (SetHighMode)
	{
		SetShadowMapLightDirection(ShadowMapHandle, lightDir);
	}
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
}

void GameScene::UpdateGame(InputValidation& input, int SCREENWIDTH, int SCREENHEIGHT)
{
	 isDebugDrawMode = false; // デバッグ描画モードのフラグ
	 context = CreateContext();
	//ループの先頭で現在の時間を記録
	context.input = &input;
	context.animator = &animator;
	StartTime = GetNowCount();
	camera.updateCamera(SCREENWIDTH, SCREENHEIGHT, player.GetPos(), field, input);

	for (GameObject* obj : gameObjects)
	{
		obj->Update(context);
	}

	// GetNowCount() はミリ秒単位で時間を取得する関数です
	while (GetNowCount() - StartTime < 16)
	{
		Sleep(1);
	}
	//次のフレームのために、現在の時間を記録（16ミリ秒足していく形が理想）
	StartTime = GetNowCount();
	if (input.IsKeyPressed(KEY_INPUT_F1))
	{
		isDebugDrawMode = true;
	}
}

void GameScene::DrawGame() 
{
	// 3. 影を描画する範囲の設定
	
	// シャドウマップ（影）の生成
	if (SetHighMode)
	{
		ShadowMap_DrawSetup(ShadowMapHandle);
		for (GameObject* obj : gameObjects)
		{
			obj->Draw();
		}
		ShadowMap_DrawEnd();
		SetUseShadowMap(0, ShadowMapHandle);

	}



// 実際に画面に表示するオブジェクトを描画
	for (GameObject* obj : gameObjects)
	{
		obj->Draw();
	}
	if (SetHighMode)
	{
		SetUseShadowMap(0, -1);
	}
	char posText[128];
	sprintf_s(posText, "現在の位置 [ X:%.2f  Y:%.2f  Z:%.2f ]", player.GetPos().x, player.GetPos().y, player.GetPos().z);

	// 文字列の描画幅を計算して、完全に中央に配置する
	int textWidth = GetDrawStringWidth(posText, (int)strlen(posText));
	int drawX = (1980 - textWidth) / 2;
	int drawY = 20;

	DrawString(drawX, drawY, posText, GetColor(255, 0, 0));
	if (player.GetIsHit())
	{
		DrawString(260, 500, "Hit!", GetColor(255, 0, 0));
	}
	else
	{
		DrawString(260, 500, "Not Hit!", GetColor(0, 255, 0));
	}
	if(villain.GetIsHit())
	{
		DrawString(260, 550, "Villain Hit!", GetColor(255, 0, 0));
	}
	if (isDebugDrawMode) {
		SetUseZBuffer3D(FALSE);

		player.DrawDebug();
		villain.DrawDebug();

		SetUseZBuffer3D(TRUE); 

		// 画面上にデバッグテキストを表示
		DrawString(10, 10, "[Debug Mode] Hitbox Visualizer Active (F1 to Toggle)", GetColor(255, 255, 255));
	}
}

GameContext GameScene::CreateContext()
{
	GameContext context;
	context.field = &field;
	context.input = &inputValidation;
	context.animator = &animator;
	context.camera = &camera;
	context.player = &player;	
	context.villain = &villain;	
	return context;
}
