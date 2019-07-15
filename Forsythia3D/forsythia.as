#uselib "forsythia.hpi"

#func fr_init		Init		$2
#func fr_bye		Bye		$100

#func fr_begin		BeginScene	$0
#func fr_end		EndScene	$0

#func fr_load		LoadModel	$6
#func fr_draw		DrawModel	$0
#func fr_free		FreeModel	$0
#func fr_setrs		SetRenderState	$0
#func fr_settss		SetTexStageState $0
#func fr_setposture	SetPosture	$1
#func fr_setpostureL	SetPostureLocal $1
#func fr_setobjcolor	SetObjColor	$1

#func fr_bgcolor	SetBgColor	$0
#func fr_setfont	SetFont		$5
#func fr_setcolor	SetColor	$0
#func fr_mes		PrintText	$6

#func fr_intersect	IntersectWorld	$1
#func fr_intersectL	IntersectLocal	$1

#func fr_atan		Atan		$1
#func fr_len		Len		$1
#func fr_getsintbl	GetSinTable	$1
#func fr_getcostbl	GetCosTable	$1

#func fr_addlight	AddLight	$0
#func fr_removelight	RemoveLight	$0
#func fr_setambient	SetAmbient	$0
#func fr_setlightparam	SetLightParam	$1
#func fr_setlightcol	SetLightColor	$0
#func fr_setlightspq	SetLightSpecular $0
#func fr_setlightpos	SetLightPosition $0
#func fr_setlightdir	SetLightDirection $0
#func fr_setlightatt	SetLightAttenuation $0

#func fr_settrackenable	SetTrackEnable	$0
#func fr_settrackweight	SetTrackWeight	$0
#func fr_getanimnum	GetAnimNum	$0

#func fr_setcampos	SetCameraPos	$0
#func fr_setcamtarget	SetCameraTarget $0
#func fr_setcamup	SetCameraUp	$0
#func fr_setproj	SetProj		$1

#func fr_vecftow	GetWorldVec	$1
#func fr_vecltof	GetFrameVec	$1
#func fr_getframeptr	GetFramePtrByIndex $1
#func fr_getframename	GetFrameNameByIndex $10

#func fr_loadtex	LoadTexture	$6
#func fr_freetex	FreeTexture	$0
#func fr_settex		SetTexture	$0

#func frp_add		AddParticle	$1
#func frp_update	UpdateParticle	$0
#func frp_draw		DrawParticle	$0

#func fr_loadshader	LoadShader	$16
#func fr_selshader	SelectShader	$0
#func fr_seltech	SelectTechnique $4
#func fr_setshadertex	SetShaderTex	$0
#func fr_setshaderarg	SetShaderArg	$1

#func fr_getcapssize	GetCapsSize	$1
#func fr_getcaps	GetCaps		$1

#func fr_drawsprite	DrawSprite	$1


;------------------------------------------------------------
; 以下の定数の詳細は DirectX のマニュアルを参照してください。
;------------------------------------------------------------

#const	D3DLIGHT_POINT		1;
#const	D3DLIGHT_SPOT		2;
#const	D3DLIGHT_DIRECTIONAL	3;

#const	D3DSHADE_FLAT		1;
#const	D3DSHADE_GOURAUD	2;
#const	D3DSHADE_PHONG		3;

#const	D3DFILL_POINT		1;
#const	D3DFILL_WIREFRAME	2;
#const	D3DFILL_SOLID		3;

#const	D3DBLEND_ZERO		1;
#const	D3DBLEND_ONE		2;
#const	D3DBLEND_SRCCOLOR	3;
#const	D3DBLEND_INVSRCCOLOR	4;
#const	D3DBLEND_SRCALPHA	5;
#const	D3DBLEND_INVSRCALPHA	6;
#const	D3DBLEND_DESTALPHA	7;
#const	D3DBLEND_INVDESTALPHA	8;
#const	D3DBLEND_DESTCOLOR	9;
#const	D3DBLEND_INVDESTCOLOR	10;
#const	D3DBLEND_SRCALPHASAT	11;
#const	D3DBLEND_BOTHSRCALPHA	12;
#const	D3DBLEND_BOTHINVSRCALPHA 13;
#const	D3DBLEND_BLENDFACTOR	 14; /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */
#const	D3DBLEND_INVBLENDFACTOR  15; /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */

#const	D3DBLENDOP_ADD		1;
#const	D3DBLENDOP_SUBTRACT	2;
#const	D3DBLENDOP_REVSUBTRACT	3;
#const	D3DBLENDOP_MIN		4;
#const	D3DBLENDOP_MAX		5;

#const	D3DTADDRESS_WRAP	1;
#const	D3DTADDRESS_MIRROR	2;
#const	D3DTADDRESS_CLAMP	3;
#const	D3DTADDRESS_BORDER	4;
#const	D3DTADDRESS_MIRRORONCE	5;

#const	D3DCULL_NONE		1;
#const	D3DCULL_CW		2;
#const	D3DCULL_CCW		3;

#const	D3DCMP_NEVER		1;
#const	D3DCMP_LESS		2;
#const	D3DCMP_EQUAL		3;
#const	D3DCMP_LESSEQUAL	4;
#const	D3DCMP_GREATER		5;
#const	D3DCMP_NOTEQUAL 	6;
#const	D3DCMP_GREATEREQUAL	7;
#const	D3DCMP_ALWAYS		8;

#const	D3DSTENCILOP_KEEP	1;
#const	D3DSTENCILOP_ZERO	2;
#const	D3DSTENCILOP_REPLACE	3;
#const	D3DSTENCILOP_INCRSAT	4;
#const	D3DSTENCILOP_DECRSAT	5;
#const	D3DSTENCILOP_INVERT	6;
#const	D3DSTENCILOP_INCR	7;
#const	D3DSTENCILOP_DECR	8;

#const	D3DFOG_NONE		0;
#const	D3DFOG_EXP		1;
#const	D3DFOG_EXP2		2;
#const	D3DFOG_LINEAR		3;

#const	D3DZB_FALSE		0;
#const	D3DZB_TRUE		1; // Z buffering
#const	D3DZB_USEW		2; // W buffering

#const	D3DRS_ZENABLE		7;    /* D3DZBUFFERTYPE (or TRUE/FALSE for legacy) */
#const	D3DRS_FILLMODE		8;    /* D3DFILLMODE */
#const	D3DRS_SHADEMODE 	9;    /* D3DSHADEMODE */
#const	D3DRS_ZWRITEENABLE	14;   /* TRUE to enable z writes */
#const	D3DRS_ALPHATESTENABLE	15;   /* TRUE to enable alpha tests */
#const	D3DRS_LASTPIXEL 	16;   /* TRUE for last-pixel on lines */
#const	D3DRS_SRCBLEND		19;   /* D3DBLEND */
#const	D3DRS_DESTBLEND 	20;   /* D3DBLEND */
#const	D3DRS_CULLMODE		22;   /* D3DCULL */
#const	D3DRS_ZFUNC		23;   /* D3DCMPFUNC */
#const	D3DRS_ALPHAREF		24;   /* D3DFIXED */
#const	D3DRS_ALPHAFUNC 	25;   /* D3DCMPFUNC */
#const	D3DRS_DITHERENABLE	26;   /* TRUE to enable dithering */
#const	D3DRS_ALPHABLENDENABLE	27;   /* TRUE to enable alpha blending */
#const	D3DRS_FOGENABLE 	28;   /* TRUE to enable fog blending */
#const	D3DRS_SPECULARENABLE	29;   /* TRUE to enable specular */
#const	D3DRS_FOGCOLOR		34;   /* D3DCOLOR */
#const	D3DRS_FOGTABLEMODE	35;   /* D3DFOGMODE */
#const	D3DRS_FOGSTART		36;   /* Fog start (for both vertex and pixel fog) */
#const	D3DRS_FOGEND		37;   /* Fog end      */
#const	D3DRS_FOGDENSITY	38;   /* Fog density  */
#const	D3DRS_RANGEFOGENABLE	48;   /* Enables range-based fog */
#const	D3DRS_STENCILENABLE	52;   /* BOOL enable/disable stenciling */
#const	D3DRS_STENCILFAIL	53;   /* D3DSTENCILOP to do if stencil test fails */
#const	D3DRS_STENCILZFAIL	54;   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
#const	D3DRS_STENCILPASS	55;   /* D3DSTENCILOP to do if both stencil and Z tests pass */
#const	D3DRS_STENCILFUNC	56;   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
#const	D3DRS_STENCILREF	57;   /* Reference value used in stencil test */
#const	D3DRS_STENCILMASK	58;   /* Mask value used in stencil test */
#const	D3DRS_STENCILWRITEMASK	59;   /* Write mask applied to values written to stencil buffer */
#const	D3DRS_TEXTUREFACTOR	60;   /* D3DCOLOR used for multi-texture blend */
#const	D3DRS_WRAP0		128;  /* wrap for 1st texture coord. set */
#const	D3DRS_WRAP1		129;  /* wrap for 2nd texture coord. set */
#const	D3DRS_WRAP2		130;  /* wrap for 3rd texture coord. set */
#const	D3DRS_WRAP3		131;  /* wrap for 4th texture coord. set */
#const	D3DRS_WRAP4		132;  /* wrap for 5th texture coord. set */
#const	D3DRS_WRAP5		133;  /* wrap for 6th texture coord. set */
#const	D3DRS_WRAP6		134;  /* wrap for 7th texture coord. set */
#const	D3DRS_WRAP7		135;  /* wrap for 8th texture coord. set */
#const	D3DRS_CLIPPING		136;
#const	D3DRS_LIGHTING		137;
#const	D3DRS_AMBIENT		139;
#const	D3DRS_FOGVERTEXMODE	140;
#const	D3DRS_COLORVERTEX	141;
#const	D3DRS_LOCALVIEWER	142;
#const	D3DRS_NORMALIZENORMALS	143;
#const	D3DRS_DIFFUSEMATERIALSOURCE	145;
#const	D3DRS_SPECULARMATERIALSOURCE	146;
#const	D3DRS_AMBIENTMATERIALSOURCE	147;
#const	D3DRS_EMISSIVEMATERIALSOURCE	148;
#const	D3DRS_VERTEXBLEND	151;
#const	D3DRS_CLIPPLANEENABLE	152;
#const	D3DRS_POINTSIZE 	154;   /* float point size */
#const	D3DRS_POINTSIZE_MIN	155;   /* float point size min threshold */
#const	D3DRS_POINTSPRITEENABLE 156;   /* BOOL point texture coord control */
#const	D3DRS_POINTSCALEENABLE	157;   /* BOOL point size scale enable */
#const	D3DRS_POINTSCALE_A	158;   /* float point attenuation A value */
#const	D3DRS_POINTSCALE_B	159;   /* float point attenuation B value */
#const	D3DRS_POINTSCALE_C	160;   /* float point attenuation C value */
#const	D3DRS_MULTISAMPLEANTIALIAS	161;  // BOOL - set to do FSAA with multisample buffer
#const	D3DRS_MULTISAMPLEMASK	162;  // DWORD - per-sample enable/disable
#const	D3DRS_PATCHEDGESTYLE	163;  // Sets whether patch edges will use float style tessellation
#const	D3DRS_DEBUGMONITORTOKEN 165;  // DEBUG ONLY - token to debug monitor
#const	D3DRS_POINTSIZE_MAX	166;   /* float point size max threshold */
#const	D3DRS_INDEXEDVERTEXBLENDENABLE	167;
#const	D3DRS_COLORWRITEENABLE	168;  // per-channel write enable
#const	D3DRS_TWEENFACTOR	170;   // float tween factor
#const	D3DRS_BLENDOP		171;   // D3DBLENDOP setting
#const	D3DRS_POSITIONDEGREE	172;   // NPatch position interpolation degree. D3DDEGREE_LINEAR or D3DDEGREE_CUBIC (default)
#const	D3DRS_NORMALDEGREE	173;   // NPatch normal interpolation degree. D3DDEGREE_LINEAR (default) or D3DDEGREE_QUADRATIC
#const	D3DRS_SCISSORTESTENABLE 174;
#const	D3DRS_SLOPESCALEDEPTHBIAS	175;
#const	D3DRS_ANTIALIASEDLINEENABLE	176;
#const	D3DRS_MINTESSELLATIONLEVEL	178;
#const	D3DRS_MAXTESSELLATIONLEVEL	179;
#const	D3DRS_ADAPTIVETESS_X	180;
#const	D3DRS_ADAPTIVETESS_Y	181;
#const	D3DRS_ADAPTIVETESS_Z	182;
#const	D3DRS_ADAPTIVETESS_W	183;
#const	D3DRS_ENABLEADAPTIVETESSELLATION 184;
#const	D3DRS_TWOSIDEDSTENCILMODE	185;   /* BOOL enable/disable 2 sided stenciling */
#const	D3DRS_CCW_STENCILFAIL	186;   /* D3DSTENCILOP to do if ccw stencil test fails */
#const	D3DRS_CCW_STENCILZFAIL	187;   /* D3DSTENCILOP to do if ccw stencil test passes and Z test fails */
#const	D3DRS_CCW_STENCILPASS	188;   /* D3DSTENCILOP to do if both ccw stencil and Z tests pass */
#const	D3DRS_CCW_STENCILFUNC	189;   /* D3DCMPFUNC fn.  ccw Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
#const	D3DRS_COLORWRITEENABLE1 190;   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
#const	D3DRS_COLORWRITEENABLE2 191;   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
#const	D3DRS_COLORWRITEENABLE3 192;   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
#const	D3DRS_BLENDFACTOR	193;   /* D3DCOLOR used for a constant blend factor during alpha blending for devices that support D3DPBLENDCAPS_BLENDFACTOR */
#const	D3DRS_SRGBWRITEENABLE	194;   /* Enable rendertarget writes to be DE-linearized to SRGB (for formats that expose D3DUSAGE_QUERY_SRGBWRITE) */
#const	D3DRS_DEPTHBIAS 	195;
#const	D3DRS_WRAP8		198;   /* Additional wrap states for vs_3_0+ attributes with D3DDECLUSAGE_TEXCOORD */
#const	D3DRS_WRAP9		199;
#const	D3DRS_WRAP10		200;
#const	D3DRS_WRAP11		201;
#const	D3DRS_WRAP12		202;
#const	D3DRS_WRAP13		203;
#const	D3DRS_WRAP14		204;
#const	D3DRS_WRAP15		205;
#const	D3DRS_SEPARATEALPHABLENDENABLE	206;  /* TRUE to enable a separate blending function for the alpha channel */
#const	D3DRS_SRCBLENDALPHA	207;  /* SRC blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */
#const	D3DRS_DESTBLENDALPHA	208;  /* DST blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */
#const	D3DRS_BLENDOPALPHA	209;  /* Blending operation for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */

#const	D3DTSS_COLOROP		1;
#const	D3DTSS_COLORARG1	2;
#const	D3DTSS_COLORARG2	3;
#const	D3DTSS_ALPHAOP		4;
#const	D3DTSS_ALPHAARG1	5;
#const	D3DTSS_ALPHAARG2	6;
#const	D3DTSS_BUMPENVMAT00	7;
#const	D3DTSS_BUMPENVMAT01	8;
#const	D3DTSS_BUMPENVMAT10	9;
#const	D3DTSS_BUMPENVMAT11	10;
#const	D3DTSS_TEXCOORDINDEX	11;
#const	D3DTSS_BUMPENVLSCALE	22;
#const	D3DTSS_BUMPENVLOFFSET	23;
#const	D3DTSS_TEXTURETRANSFORMFLAGS	24;
#const	D3DTSS_COLORARG0	26;
#const	D3DTSS_ALPHAARG0	27;
#const	D3DTSS_RESULTARG	28;
#const	D3DTSS_CONSTANT		32;

#const	D3DTSS_TCI_PASSTHRU			0x00000000
#const	D3DTSS_TCI_CAMERASPACENORMAL		0x00010000
#const	D3DTSS_TCI_CAMERASPACEPOSITION		0x00020000
#const	D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR	0x00030000
#const	D3DTSS_TCI_SPHEREMAP			0x00040000

#const	D3DTOP_DISABLE			1;   disables stage
#const	D3DTOP_SELECTARG1		2;   the default
#const	D3DTOP_SELECTARG2		3;
#const	D3DTOP_MODULATE 		4;   multiply args together
#const	D3DTOP_MODULATE2X		5;   multiply and  1 bit
#const	D3DTOP_MODULATE4X		6;   multiply and  2 bits
#const	D3DTOP_ADD			7;   add arguments together
#const	D3DTOP_ADDSIGNED		8;   add with -0.5 bias
#const	D3DTOP_ADDSIGNED2X		9;   as above but left	1 bit
#const	D3DTOP_SUBTRACT 		10;  Arg1 - Arg2, with no saturation
#const	D3DTOP_ADDSMOOTH		11;  Arg1 + (1-Arg1)*Arg2
#const	D3DTOP_BLENDDIFFUSEALPHA	12;  iterated alpha
#const	D3DTOP_BLENDTEXTUREALPHA	13;  texture alpha
#const	D3DTOP_BLENDFACTORALPHA 	14;  alpha from D3DRS_TEXTUREFACTOR
#const	D3DTOP_BLENDTEXTUREALPHAPM	15;  texture alpha
#const	D3DTOP_BLENDCURRENTALPHA	16;  by alpha of current color
#const	D3DTOP_PREMODULATE		17;  modulate with next texture before use
#const	D3DTOP_MODULATEALPHA_ADDCOLOR	18;  Arg1.RGB + Arg1.A*Arg2.RGB
#const	D3DTOP_MODULATECOLOR_ADDALPHA	19;  Arg1.RGB*Arg2.RGB + Arg1.A
#const	D3DTOP_MODULATEINVALPHA_ADDCOLOR 20; (1-Arg1.A)*Arg2.RGB + Arg1.RGB
#const	D3DTOP_MODULATEINVCOLOR_ADDALPHA 21; (1-Arg1.RGB)*Arg2.RGB + Arg1.A
#const	D3DTOP_BUMPENVMAP		22;  per pixel env map perturbation
#const	D3DTOP_BUMPENVMAPLUMINANCE	23;  with luminance channel
#const	D3DTOP_DOTPRODUCT3		24;  dot(Arg1, Arg2)
#const	D3DTOP_MULTIPLYADD		25;  Arg0 + Arg1*Arg2
#const	D3DTOP_LERP			26;  (Arg0)*Arg1 + (1-Arg0)*Arg2

#const	D3DTA_SELECTMASK	0x0000000f;  mask for arg selector
#const	D3DTA_DIFFUSE		0x00000000;  select diffuse color (read only)
#const	D3DTA_CURRENT		0x00000001;  select stage destination register (read/write)
#const	D3DTA_TEXTURE		0x00000002;  select texture color (read only)
#const	D3DTA_TFACTOR		0x00000003;  select D3DRS_TEXTUREFACTOR (read only)
#const	D3DTA_SPECULAR		0x00000004;  select specular color (read only)
#const	D3DTA_TEMP		0x00000005;  select temporary register color (read/write)
#const	D3DTA_CONSTANT		0x00000006;  select texture stage constant
#const	D3DTA_COMPLEMENT	0x00000010;  take 1.0 - x (read modifier)
#const	D3DTA_ALPHAREPLICATE	0x00000020;  replicate alpha to color components (read modifier)

#const	D3DTEXF_NONE            0; filtering disabled (valid for mip filter only)
#const	D3DTEXF_POINT           1; nearest
#const	D3DTEXF_LINEAR          2; linear interpolation
#const	D3DTEXF_ANISOTROPIC     3; anisotropic
#const	D3DTEXF_PYRAMIDALQUAD   6; 4-sample tent
#const	D3DTEXF_GAUSSIANQUAD    7; 4-sample gaussian

