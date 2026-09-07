//------------------------------------------------------------------------------------------------------
// アニメーションする電飾テクスチャ
//  ベース       : full.fx
//  full.fx製作者: 舞力介入氏
//  改変者       : Taro2
//                 テクスチャの動作方法を知りたい方は"はなから牛乳P氏","テンパカ氏"が制作されたfxをご参照ください
//  参考ソース   : はなから牛乳P氏制作のfx(テクスチャ操作部分)とテンパカ氏制作のfx(テクスチャ操作部分)
//                 はなから牛乳P氏制作物配布場所 : https://bowlroll.net/file/12407
//                 テンパカ氏制作物配布場所      : https://bowlroll.net/file/40599
//------------------------------------------------------------------------------------------------------

// パラメータ宣言 --------------------------------------------------------------------------------------
//テクスチャ物理ファイル名//
#define TEXFILENM         "tex/Illuminations.png"

//PMX素材番号//
#define SUBSETNO_GROUPA   "0-9"  //電飾 床           Type:3
#define SUBSETNO_GROUPB   "99" //電飾 電灯回転     Type:0
#define SUBSETNO_GROUPC   "88" //電飾 ライン       Type:0
#define SUBSETNO_GROUPD   "77" //電飾 板の横回転   Type:0
#define SUBSETNO_GROUPE   "66" //電飾 円矢印回転   Type:1

//動作機能//
#define SUBSET_TYPE0  0    //全機能使用可能(発光度加算,発光度減算,スクロール速度加算,動かし方切り替え,スクロール向き逆転)//
#define SUBSET_TYPE1  1    //動かし方切り替えのみ使用不可　他は使用可能//
#define SUBSET_TYPE2  2    //速度コントロールをOtherで制御する//
#define SUBSET_TYPE3  3    //動かし方切り替え不可，速度コントロールをOtherで制御する//

#define SUBSET_NO0 "0"
#define SUBSET_NO1 "1"
#define SUBSET_NO2 "2"
#define SUBSET_NO3 "3"
#define SUBSET_NO4 "4"
#define SUBSET_NO5 "5"
#define SUBSET_NO6 "6"
#define SUBSET_NO7 "7"
#define SUBSET_NO8 "8"
#define SUBSET_NO9 "9"

float Material_Luminous_UP   : CONTROLOBJECT < string name = "(self)"; string item = "Luminous+"; >;  //発光度加算//
float Material_Luminous_DW   : CONTROLOBJECT < string name = "(self)"; string item = "Luminous-"; >;  //発光度減算//
float Material_ScrollSpeedUP : CONTROLOBJECT < string name = "(self)"; string item = "ScrollSpd+"; >; //スクロール速度加算//
float Material_ScrollSpeedDW : CONTROLOBJECT < string name = "(self)"; string item = "ScrollSpd-"; >; //スクロール速度減算//
float Material_StopGoSpeedUP : CONTROLOBJECT < string name = "(self)"; string item = "StopGoSpd+"; >; //一時停止＆Goスクロール速度加算//
float Material_StopGoSpeedDW : CONTROLOBJECT < string name = "(self)"; string item = "StopGoSpd-"; >; //一時停止＆Goスクロール速度減算//
float Material_OtherSpeedUP  : CONTROLOBJECT < string name = "(self)"; string item = "OtherSpd+"; > ; //その他(Other)スクロール速度加算//
float Material_OtherSpeedDW  : CONTROLOBJECT < string name = "(self)"; string item = "OtherSpd-"; > ; //その他(Other)スクロール速度減算//
float Material_MovChange     : CONTROLOBJECT < string name = "(self)"; string item = "Switching"; >;  //動かし方切り替え//
float Material_MovRev        : CONTROLOBJECT < string name = "(self)"; string item = "ScrollRev"; >;  //スクロール向き逆転//

//以下の設定はテクスチャを動作させる為の動作開始設定値になります、変数になっていますが値を変更しないでください//
float U_Lower = 0.0f;  // U値の下限値    Default:0.0f//
float U_Upper = 1.0f;  // U値の上限値    Default:1.0f//
float U_Cycle = 10.0f; // U値の周期 [秒] Default:10.0f  0.0は停止  指定テクスチャのU(X軸)でスクロールさせたい場合0以上を設定//
float V_Lower = 0.0f;  // V値の下限値    Default:0.0f//
float V_Upper = 1.0f;  // V値の上限値    Default:1.0f//
float V_Cycle = 0.0f;  // V値の周期 [秒] Default:10.0f  0.0は停止//



// グローバル変数 --------------------------------------------------------------------------------------
// 座標変換行列
float4x4 WorldViewProjMatrix      : WORLDVIEWPROJECTION;
float4x4 WorldMatrix              : WORLD;
float4x4 ViewMatrix               : VIEW;

float3   LightDirection    : DIRECTION < string Object = "Light"; >;
float3   CameraPosition    : POSITION  < string Object = "Camera"; >;

// マテリアル色
float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;
float3   MaterialAmbient   : AMBIENT  < string Object = "Geometry"; >;
float3   MaterialEmmisive  : EMISSIVE < string Object = "Geometry"; >;
float3   MaterialSpecular  : SPECULAR < string Object = "Geometry"; >;
float    SpecularPower     : SPECULARPOWER < string Object = "Geometry"; >;
// ライト色
float3   LightDiffuse      : DIFFUSE   < string Object = "Light"; >;
float3   LightAmbient      : AMBIENT   < string Object = "Light"; >;
float3   LightSpecular     : SPECULAR  < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);
static float3 AmbientColor  = MaterialAmbient  * LightAmbient + MaterialEmmisive;
static float3 SpecularColor = MaterialSpecular * LightSpecular;

//bool     use_subtexture;    // サブテクスチャフラグ
bool     spadd;             // スフィアマップ加算合成フラグ

//テクニック宣言
#define DEF_TECHNIQUE( TECHNAME, USETEX, USESPH, USETOON, SUBSETTYPE_VS, SUBSETTYPE_PS, SUBSET, VSNAME, PSNAME )  \
technique TECHNAME < string MMDPass = "object"; bool UseTexture = USETEX; bool UseSphereMap = USESPH; bool UseToon = USETOON; string Subset = SUBSET; > { \
    pass DrawObject { \
        VertexShader = compile vs_3_0 VSNAME( USETEX, USESPH, USETOON, SUBSET, SUBSETTYPE_VS ); \
        PixelShader  = compile ps_3_0 PSNAME( USETEX, USESPH, USETOON, SUBSET, SUBSETTYPE_PS ); \
    }\
};
#define DEF_TECHNIQUE_SS( TECHNAME, USETEX, USESPH, USETOON, SUBSETTYPE_VS, SUBSETTYPE_PS, SUBSET, VSNAME, PSNAME )  \
technique TECHNAME < string MMDPass = "object_ss"; bool UseTexture = USETEX; bool UseSphereMap = USESPH; bool UseToon = USETOON; string Subset = SUBSET; > { \
    pass DrawObject { \
        VertexShader = compile vs_3_0 VSNAME( USETEX, USESPH, USETOON, SUBSET, SUBSETTYPE_VS ); \
        PixelShader  = compile ps_3_0 PSNAME( USETEX, USESPH, USETOON, SUBSET, SUBSETTYPE_PS ); \
    }\
};

// オブジェクトのテクスチャ
//texture ObjectTexture: MATERIALTEXTURE;
texture ObjectTexture <string ResourceName = TEXFILENM;>;
sampler ObjTexSampler = sampler_state {
    texture = <ObjectTexture>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

//////////////////////////////////////////////////////
texture ObjectTexture_A < string ResourceName = "tex/A.png"; > ;
sampler ObjTexSampler_A = sampler_state {
	texture = <ObjectTexture_A>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
//////////////////////////////////////////////////////

// スフィアマップのテクスチャ
texture ObjectSphereMap: MATERIALSPHEREMAP;
sampler ObjSphareSampler = sampler_state {
    texture = <ObjectSphereMap>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

// トゥーンマップのテクスチャ
texture ObjectToonTexture: MATERIALTOONTEXTURE;
sampler ObjToonSampler = sampler_state {
    texture = <ObjectToonTexture>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = NONE;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

// MMD本来のsamplerを上書きしないための記述です。削除不可。
sampler MMDSamp0 : register(s0);
sampler MMDSamp1 : register(s1);
sampler MMDSamp2 : register(s2);


float time : TIME; // 0フレーム目からの再生時間 [秒]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクト描画
// シェーダ入出力用構造体 ---------------------
struct VS_OUTPUT {
    float4 Pos        : POSITION;    // 射影変換座標
    float2 Tex        : TEXCOORD1;   // テクスチャ
    float3 Normal     : TEXCOORD2;   // 法線
    float3 Eye        : TEXCOORD3;   // カメラとの相対位置
    float2 SpTex      : TEXCOORD4;	 // スフィアマップテクスチャ座標
    float4 Color      : COLOR0;      // ディフューズ色
};

// シェーダ -----------------------------------
// 頂点シェーダ
VS_OUTPUT Basic_VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Tex : TEXCOORD0, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon, uniform string  SubsetNo, uniform int SubsetType)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    // カメラ視点のワールドビュー射影変換
    Out.Pos = mul( Pos, WorldViewProjMatrix );
    
    // カメラとの相対位置
    Out.Eye = CameraPosition - mul( Pos, WorldMatrix );

    // 頂点法線
    Out.Normal = normalize( mul( Normal, (float3x3)WorldMatrix ) );
    
    // ディフューズ色＋アンビエント色 計算
    Out.Color.rgb = AmbientColor;
    if ( !useToon ) {
        Out.Color.rgb += max(0,dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb;
    }
    Out.Color.a = DiffuseColor.a;
    Out.Color = saturate( Out.Color );
    
    // テクスチャ座標
    // 1. テクスチャ座標を周期範囲座標(0=下限位置,+1=上限位置)に変換
    float U_CyclPos = (Tex.x- U_Lower) / (U_Upper - U_Lower);
    float V_CyclPos = (Tex.y- V_Lower) / (V_Upper - V_Lower);
    
    // 2. 位相(0～1)の分 周期範囲座標をずらす
    //テクスチャを動かす
    //--------------------------------------------------
    //True:切り替え可能 False:切り替え不可
    bool WK_Switch_SW = true;
    if ((SubsetType == SUBSET_TYPE1) || (SubsetType == SUBSET_TYPE3)) {
        WK_Switch_SW = false;
    }

    //速度調整値をWorkへセット
    float WK_ScrollSpeedUP = Material_ScrollSpeedUP;
    float WK_ScrollSpeedDW = Material_ScrollSpeedDW;
    float WK_StopGoSpeedUP = Material_StopGoSpeedUP;
    float WK_StopGoSpeedDW = Material_StopGoSpeedDW;
    //その他(Other)の速度操作に置き換える
    if ((SubsetType == SUBSET_TYPE2) || (SubsetType == SUBSET_TYPE3)) {
        WK_ScrollSpeedUP = Material_OtherSpeedUP;
        WK_ScrollSpeedDW = Material_OtherSpeedDW;
        WK_StopGoSpeedUP = Material_OtherSpeedUP;
        WK_StopGoSpeedDW = Material_OtherSpeedDW;
    }

    if ((WK_Switch_SW) && (Material_MovChange > 0)) {
        //U 一時停止＆動く
        if (U_Cycle > 0.0f || U_Cycle < 0.0f) {
            float U_fac2 = (WK_StopGoSpeedUP * 3.0f + 1.0f) - (WK_StopGoSpeedDW * WK_StopGoSpeedDW);
            U_CyclPos += sign(0.5f - Material_MovRev) * floor(time * 1.5f * U_fac2) / 24.0f;
        }
        //V 一時停止＆動く
        if (V_Cycle > 0.0f || V_Cycle < 0.0f) {
            float V_fac2 = (WK_StopGoSpeedUP * 3.0f + 1.0f) - (WK_StopGoSpeedDW * WK_StopGoSpeedDW);
            V_CyclPos += sign(0.5f - Material_MovRev) * floor(time * 1.5f * V_fac2) / 7.0f;
        }
    }
    else {
        //U 流れる動き
        if (U_Cycle > 0.0f || U_Cycle < 0.0f) {
            float U_fac1 = (((WK_ScrollSpeedUP * 10.0f) - 0.5f) * -1.0f) + ((WK_ScrollSpeedDW * 100.0f));
            U_CyclPos += sign(0.5 - Material_MovRev) * ((time - ((U_Cycle + U_fac1) * (int)(time / (U_Cycle + U_fac1)))) / (U_Cycle + U_fac1));
        }
        //V 流れる動き
        if (V_Cycle > 0.0f || V_Cycle < 0.0f) {
            float V_fac1 = (((WK_ScrollSpeedUP * 10.0f) - 0.5f) * -1.0f) + ((WK_ScrollSpeedDW * 100.0f));
            U_CyclPos += sign(0.5 - Material_MovRev) * ((time - ((U_Cycle + V_fac1) * (int)(time / (U_Cycle + V_fac1)))) / (U_Cycle + V_fac1));
        }
    }
    //--------------------------------------------------
    
    // 3. 周期範囲座標mod(周期範囲)をテクスチャ座標に戻す
    Out.Tex.x = (U_CyclPos * (U_Upper - U_Lower) + U_Lower);
    Out.Tex.y = (V_CyclPos * (V_Upper - V_Lower) + V_Lower);
    
    if ( useSphereMap ) {
        //if ( use_subtexture ) {
            // PMXサブテクスチャ座標
        //    Out.SpTex = Tex2;
        //} else {
            // スフィアマップテクスチャ座標
            float2 NormalWV = mul( Out.Normal, (float3x3)ViewMatrix );
            Out.SpTex.x = NormalWV.x * 0.5f + 0.5f;
            Out.SpTex.y = NormalWV.y * -0.5f + 0.5f;
        //}
    }

    // スペキュラ色計算
    //float3 HalfVector = normalize( normalize(Out.Eye) + -LightDirection );
    //Out.Specular = pow( max(0,dot( HalfVector, Out.Normal )), SpecularPower ) * SpecularColor;

    //発光度合加味
    Out.Color.rgb += (Material_Luminous_UP * 3.0f) - (Material_Luminous_DW * 3.0f);

    return Out;
}

// ピクセルシェーダ
float4 Basic_PS(VS_OUTPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon, uniform string  SubsetNo, uniform int SubsetType) : COLOR0
{
    float3 HalfVector = normalize(normalize(IN.Eye) + -LightDirection);
    float3 Specular = pow(max(0,dot(HalfVector, normalize(IN.Normal))), SpecularPower) * SpecularColor;

    float4 Color = IN.Color;
    if ( useTexture ) {
        // テクスチャ適用
        //Color *= tex2D( ObjTexSampler, IN.Tex );
		if (SubsetType == SUBSET_TYPE0) {
			Color *= tex2D(ObjTexSampler_A, IN.Tex);
		}
		else {
			Color *= tex2D(ObjTexSampler, IN.Tex);
		}
    }

    if ( useSphereMap ) {
        // スフィアマップ適用
        float4 TexColor = tex2D(ObjSphareSampler,IN.SpTex);
        if(spadd) Color.rgb += TexColor.rgb;
        else      Color.rgb *= TexColor.rgb;
        Color.a *= TexColor.a;
    }
    
    if ( useToon ) {
        // トゥーン適用
        float LightNormal = dot( IN.Normal, -LightDirection );
        Color *= tex2D(ObjToonSampler, float2(0, 0.5 - LightNormal * 0.5) );
    }
    
    // スペキュラ適用
    //Color.rgb += IN.Specular;
    Color.rgb += Specular;
    
    return Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// テクニック ------------------------------
DEF_TECHNIQUE(MainTec01, true, false, true, SUBSET_TYPE3, SUBSET_TYPE3, SUBSETNO_GROUPA, Basic_VS, Basic_PS)
DEF_TECHNIQUE(MainTec02, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPB, Basic_VS, Basic_PS)
DEF_TECHNIQUE(MainTec03, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPC, Basic_VS, Basic_PS)
DEF_TECHNIQUE(MainTec04, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPD, Basic_VS, Basic_PS)
DEF_TECHNIQUE(MainTec05, true, false, true, SUBSET_TYPE1, SUBSET_TYPE1, SUBSETNO_GROUPE, Basic_VS, Basic_PS)

DEF_TECHNIQUE_SS(MainTecSS01, true, false, true, SUBSET_TYPE3, SUBSET_TYPE3, SUBSETNO_GROUPA, Basic_VS, Basic_PS)
DEF_TECHNIQUE_SS(MainTecSS02, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPB, Basic_VS, Basic_PS)
DEF_TECHNIQUE_SS(MainTecSS03, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPC, Basic_VS, Basic_PS)
DEF_TECHNIQUE_SS(MainTecSS04, true, false, true, SUBSET_TYPE0, SUBSET_TYPE0, SUBSETNO_GROUPD, Basic_VS, Basic_PS)
DEF_TECHNIQUE_SS(MainTecSS05, true, false, true, SUBSET_TYPE1, SUBSET_TYPE1, SUBSETNO_GROUPE, Basic_VS, Basic_PS)


