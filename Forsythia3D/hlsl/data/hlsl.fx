/*

[ Forsythia3D HLSL Sample ]    Copyright(c) 2003 MIA
�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
���̃t�@�C���ɂ́A�ȉ��̃e�N�j�b�N���܂܂�Ă��܂��B

�ESphereShader : �X�t�B�A���}�b�v
	�o�͂Ɋ��}�b�v�̐F�����Z�����
	ShaderTex ���^�����Ă��邱�ƁB

�ECellShader : �Z���V�F�[�_
	�֊s���̕`��ɉ����A�V�F�[�f�B���O�e�N�X�`�����g�p�������C�e�B���O��
	�{�����B�V�F�[�f�B���O�e�N�X�`���́A(�@���� Z ����, �@���ƌ����̓���)
	�Ƃ��Ďg�p����A�e�N�X�`�������������邱�Ƃŗl�X�ȕ\�����\�B
	ShaderTex �ɃV�F�[�f�B���O�e�N�X�`��
	ShaderArg �Ɍ����̕������^�����Ă��邱�ƁB

�ECellShaderNT : �Z���V�F�[�_
	CellShader �̃e�N�X�`��������

�EEdgeShader : �֊s�V�F�[�_
	�ŊO���̗֊s���������ŕ`�悳��A��낪������

*/

//////////////////////////////////////////////////////////////////////////////
// �����ϊ��p�p�����[�^�i���O�ύX�s�j
float4x4 World;
// float4x4 View;
// float4x4 Proj;
// float4x4 WorldView;
// float4x4 ViewProj;
float4x4 WorldViewProj;


//////////////////////////////////////////////////////////////////////////////
// �}�e���A�����i���O�ύX�s�j
float4	Diffuse;
float4	Ambient;
float4	Emissive;
float4	Specular;
float	Power;
texture	Texture;


//////////////////////////////////////////////////////////////////////////////
// �ǉ����
float4	ShaderArg0;	// �p�����[�^�x�N�g��
texture	ShaderTex0;	// �V�F�[�f�B���O�e�N�X�`��

// ShaderTex / ShaderArg �Ƃ��ɁA�����̐����� 0�`7 �܂Ŏw��ł��܂�


//////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���Q�ƕ��@�̐ݒ�
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
// ���_�V�F�[�_����s�N�Z���V�F�[�_�ɓn���f�[�^
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
// �X�t�B�A���}�b�s���O ���_�V�F�[�_
VS_OUTPUT_TEX2 VS_SPHERE(
	float4 Pos	  : POSITION,
	float3 Normal : NORMAL,
	float2 TexUV  : TEXCOORD0
){
	VS_OUTPUT_TEX2 Out = (VS_OUTPUT_TEX2)0; 	// �o�̓f�[�^
	float3 WorldNorm = mul(Normal, World);		// �@�������[���h���W�n�ɕϊ�

	// ���_���̐���
	Out.Pos = mul(Pos, WorldViewProj);
	Out.Tex1= TexUV;
	Out.Col = Diffuse;

	// �e�N�X�`�����W�̐���
	WorldNorm[1] = -WorldNorm[1];
	Out.Tex2.xy = 0.5 + WorldNorm.xy * 0.5;

	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// �X�t�B�A���}�b�s���O �s�N�Z���V�F�[�_
float4 PS_SPHERE(
		float4 Col : COLOR0,
		float2 Tex1: TEXCOORD0,
		float2 Tex2: TEXCOORD1) : COLOR {
	// �x�[�X�e�N�X�`���Ɋ��}�b�v�̐F�����Z
	return
		(tex2D( TextureSampler, Tex1 ) +
		tex2D( ShaderSampler , Tex2 )) * Col;
}


//////////////////////////////////////////////////////////////////////////////
// �@�������ɉ����o�����_�V�F�[�_
VS_OUTPUT_POS VS_CELL_EDGE(
	float3 Normal	: NORMAL,			// ���f���̖@��
	float4 Pos		: POSITION		// ���f���̒��_
){
	VS_OUTPUT_POS Out = (VS_OUTPUT_POS)0;		 // �o�̓f�[�^
	Pos.xyz += Normal * ShaderArg0[3];
	Out.Pos = mul(Pos, WorldViewProj);
	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// �Z���V�F�[�_�p ���_�V�F�[�_
VS_OUTPUT_TEX2 VS_CELL(
	float4 Pos		: POSITION,		// ���f���̒��_
	float3 Normal	: NORMAL,			// ���f���̖@��
	float2 TexUV	: TEXCOORD0
){
	VS_OUTPUT_TEX2 Out = (VS_OUTPUT_TEX2)0; 	   // �o�̓f�[�^

	// �@�������[���h���W�n�ɕϊ�
	float3 WorldNorm = mul(Normal, World);

	// ShaderArg �Ɩ@���̓��ς��擾
	float l = dot( ShaderArg0, WorldNorm );

	// ���_���̐���
	Out.Pos = mul( Pos, WorldViewProj);
	Out.Col = Diffuse + Emissive;
	Out.Tex1= TexUV;

	// �e�N�X�`�����W�̐���
	Out.Tex2[0] = WorldNorm[2] * 0.5 + 0.5;
	Out.Tex2[1] = l * 0.5 + 0.5;

	return Out;
}


//////////////////////////////////////////////////////////////////////////////
// �Z���V�F�[�_�p �s�N�Z���V�F�[�_
float4 PS_CELL(
		float4 Color: COLOR0,
		float2 Tex1	: TEXCOORD0,
		float2 Tex2	: TEXCOORD1) : COLOR {
	// �e�N�X�`������Z���A�F���|����
	return (
		tex2D( TextureSampler, Tex1 ) *
		tex2D( ShaderSampler , Tex2 ) *
		Color
	);
}


//////////////////////////////////////////////////////////////////////////////
// �Z���V�F�[�_�p �s�N�Z���V�F�[�_
float4 PS_CELL_NT(
		float4 Color: COLOR0,
		float2 Tex2	: TEXCOORD1) : COLOR {
	// �e�N�X�`������Z���A�F���|����
	return tex2D( ShaderSampler , Tex2 ) * Color;
}


//////////////////////////////////////////////////////////////////////////////
// �e�N�j�b�N�ꗗ

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
