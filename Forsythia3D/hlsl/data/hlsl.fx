/*

[ Forsythia3D HLSL Sample ]    Copyright(c) 2003 MIA
￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
このファイルには、以下のテクニックが含まれています。

・SphereShader : スフィア環境マップ
	出力に環境マップの色が加算される
	ShaderTex が与えられていること。

・CellShader : セルシェーダ
	輪郭線の描画に加え、シェーディングテクスチャを使用したライティングが
	施される。シェーディングテクスチャは、(法線の Z 方向, 法線と光線の内積)
	として使用され、テクスチャを書き換えることで様々な表現が可能。
	ShaderTex にシェーディングテクスチャ
	ShaderArg に光源の方向が与えられていること。

・CellShaderNT : セルシェーダ
	CellShader のテクスチャ無し版

・EdgeShader : 輪郭シェーダ
	最外周の輪郭だけが黒で描画され、後ろが透ける

*/

//////////////////////////////////////////////////////////////////////////////
// 透視変換用パラメータ（名前変更不可）
float4x4 World;
// float4x4 View;
// float4x4 Proj;
// float4x4 WorldView;
// float4x4 ViewProj;
float4x4 WorldViewProj;


//////////////////////////////////////////////////////////////////////////////
// マテリアル情報（名前変更不可）
float4	Diffuse;
float4	Ambient;
float4	Emissive;
float4	Specular;
float	Power;
texture	Texture;


//////////////////////////////////////////////////////////////////////////////
// 追加情報
float4	ShaderArg0;	// パラメータベクトル
texture	ShaderTex0;	// シェーディングテクスチャ

// ShaderTex / ShaderArg ともに、末尾の数字を 0～7 まで指定できます


//////////////////////////////////////////////////////////////////////////////
// テクスチャ参照方法の設定
sampler TextureSampler = sampler_state {
	Texture		= <Texture>;
	MinFilter	= LINEAR;
	MagFilter	= LINEAR;
	AddressU	= Wrap;
	AddressV	= Wrap;
};

sampler ShaderSampler = sampler_state {
	Texture		= <ShaderTex0>;
	MinFilter	= LINEAR;
	MagFilter	= LINEAR;
	AddressU	= Clamp;
	AddressV	= Clamp;
};


//////////////////////////////////////////////////////////////////////////////
// 頂点シェーダからピクセルシェーダに渡すデータ
struct VS_OUTPUT_POS {
	float4 Pos	: POSITION;
};

struct VS_OUTPUT_TEX0 {
	float4 Pos	: POSITION;
	float4 Col	: COLOR0;
};

struct VS_OUTPUT_TEX1 {
	float4 Pos	: POSITION;
	float4 Col	: COLOR0;
	float2 Tex1	: TEXCOORD0;
};

struct VS_OUTPUT_TEX2 {
	float4 Pos	: POSITION;
	float4 Col	: COLOR0;
	float2 Tex1	: TEXCOORD0;
	float2 Tex2	: TEXCOORD1;
};


//////////////////////////////////////////////////////////////////////////////
// スフィア環境マッピング 頂点シェーダ
VS_OUTPUT_TEX2 VS_SPHERE(
	float4 Pos	  : POSITION,
	float3 Normal : NORMAL,
	float2 TexUV  : TEXCOORD0
){
	VS_OUTPUT_TEX2 Out = (VS_OUTPUT_TEX2)0; 	// 出力データ
	float3 WorldNorm = mul(Normal, World);		// 法線をワールド座標系に変換

	// 頂点情報の生成
	Out.Pos = mul(Pos, WorldViewProj);
	Out.Tex1= TexUV;
	Out.Col = Diffuse;

	// テクスチャ座標の生成
	WorldNorm[1] = -WorldNorm[1];
	Out.Tex2.xy = 0.5 + WorldNorm.xy * 0.5;

	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// スフィア環境マッピング ピクセルシェーダ
float4 PS_SPHERE(
		float4 Col : COLOR0,
		float2 Tex1: TEXCOORD0,
		float2 Tex2: TEXCOORD1) : COLOR {
	// ベーステクスチャに環境マップの色を加算
	return
		(tex2D( TextureSampler, Tex1 ) +
		tex2D( ShaderSampler , Tex2 )) * Col;
}


//////////////////////////////////////////////////////////////////////////////
// 法線方向に押し出す頂点シェーダ
VS_OUTPUT_POS VS_CELL_EDGE(
	float3 Normal	: NORMAL,			// モデルの法線
	float4 Pos		: POSITION		// モデルの頂点
){
	VS_OUTPUT_POS Out = (VS_OUTPUT_POS)0;		 // 出力データ
	Pos.xyz += Normal * ShaderArg0[3];
	Out.Pos = mul(Pos, WorldViewProj);
	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// セルシェーダ用 頂点シェーダ
VS_OUTPUT_TEX2 VS_CELL(
	float4 Pos		: POSITION,		// モデルの頂点
	float3 Normal	: NORMAL,			// モデルの法線
	float2 TexUV	: TEXCOORD0
){
	VS_OUTPUT_TEX2 Out = (VS_OUTPUT_TEX2)0; 	   // 出力データ

	// 法線をワールド座標系に変換
	float3 WorldNorm = mul(Normal, World);

	// ShaderArg と法線の内積を取得
	float l = dot( ShaderArg0, WorldNorm );

	// 頂点情報の生成
	Out.Pos = mul( Pos, WorldViewProj);
	Out.Col = Diffuse + Emissive;
	Out.Tex1= TexUV;

	// テクスチャ座標の生成
	Out.Tex2[0] = WorldNorm[2] * 0.5 + 0.5;
	Out.Tex2[1] = l * 0.5 + 0.5;

	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// セルシェーダ用 ピクセルシェーダ
float4 PS_CELL(
		float4 Color: COLOR0,
		float2 Tex1	: TEXCOORD0,
		float2 Tex2	: TEXCOORD1) : COLOR {
	// テクスチャを乗算し、色を掛ける
	return (
		tex2D( TextureSampler, Tex1 ) *
		tex2D( ShaderSampler , Tex2 ) *
		Color
	);
}


//////////////////////////////////////////////////////////////////////////////
// セルシェーダ用 ピクセルシェーダ
float4 PS_CELL_NT(
		float4 Color: COLOR0,
		float2 Tex2	: TEXCOORD1) : COLOR {
	// テクスチャを乗算し、色を掛ける
	return tex2D( ShaderSampler , Tex2 ) * Color;
}


//////////////////////////////////////////////////////////////////////////////
// テクニック一覧

technique SphereShader {
	pass P0 {
		VertexShader= compile vs_1_1 VS_SPHERE();
		PixelShader	= compile ps_1_1 PS_SPHERE();
	}
}

technique CellShader {
	pass P0 {
		VertexShader= compile vs_1_1 VS_CELL_EDGE();
		PixelShader	= NULL;
		SrcBlend	= ZERO;
		DestBlend	= ZERO;
		CullMode	= CW;
		AlphaBlendEnable = true;
	}

	pass P1 {
		VertexShader= compile vs_1_1 VS_CELL();
		PixelShader	= compile ps_1_1 PS_CELL();
		SrcBlend	= ONE;
		DestBlend	= ZERO;
		CullMode	= CCW;
	}
}

technique CellShaderNT {
	pass P0 {
		VertexShader= compile vs_1_1 VS_CELL_EDGE();
		PixelShader	= NULL;
		SrcBlend	= ZERO;
		DestBlend	= ZERO;
		CullMode	= CW;
		AlphaBlendEnable = true;
	}

	pass P1 {
		VertexShader= compile vs_1_1 VS_CELL();
		PixelShader	= compile ps_1_1 PS_CELL_NT();
		SrcBlend	= ONE;
		DestBlend	= ZERO;
		CullMode	= CCW;
	}
}

technique EdgeShader {
	pass P0 {
		VertexShader= NULL;
		PixelShader	= NULL;
		SrcBlend	= ZERO;
		DestBlend	= ONE;
		CullMode	= NONE;
		AlphaBlendEnable = true;
	}

	pass P1 {
		VertexShader= compile vs_1_1 VS_CELL_EDGE();
		PixelShader	= NULL;
		SrcBlend	= ZERO;
		DestBlend	= ZERO;
		CullMode	= CW;
	}

}
