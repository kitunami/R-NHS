
//
//		HSP3.0 plugin sample
//		onion software/onitama 2004/9
//

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include "hsp3plugin.h"


#define _CRTDBG_MAP_ALLOC

#include <windows.h>

#define COEFHSTRING
#include <coef.h>
#include <basedat.h>
#include <process.h>
#include <advwin32.h>


//#include <windowsx.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <io.h>

#include <crtdbg.h>


#define DBGH
#include <dbg.h>

#include "d3dapp.h"
#include "d3dfont.h"

#include <shdhandler.h>
#include <lightdata.h>
#include <quaternion.h>
#include <quaternion2.h>

#include <groundbmp.h>

#include <bgdisp2.h>
#include "mysprite.h"

#include <commctrl.h> //for progress bar


#include <naviline.h>
#include <navipoint.h>

#include <qhandler.h>

#include "npcleardata.h"

#include <dibfile.h>
#include <avidata.h>
#include <xfont.h>

#include "bsphere.h"
#include <texbank.h>

#include "HuSound.h"
#include "handlerset.h"


#include <BoneProp.h>
#include <LiSPSM.h>
#include <Vec3List.h>

#include <cpuid.h>
#include <rtsc.h>
#include <asmfunc.h>

#include "resource.h"

#include "IncludeManager.h"

#include <CameraAnimHandler.h>
#include <CameraAnim.h>
#include <CameraFile.h>


#include "hsp3plugin.h"
#include "hsp3struct.h"
#include "hspvar_core.h"
#include "hspwnd.h"
#include "hsp3debug.h"
#include "hsp3struct.h"//HSPCTX


#define E3DHSPVERSION	5305

#define POSITIONMIN	-5000.0f
#define POSITIONMAX 5000.0f

//////////

/***	
static HANDLE s_hThread1 = NULL;
static DWORD s_dwMainId1;
static HANDLE s_hThread2 = NULL;
static DWORD s_dwMainId2;
static HANDLE s_hThread3 = NULL;
static DWORD s_dwMainId3;

DWORD WINAPI ThreadFunc( LPVOID	lpThreadParam );
***/


static CCameraAnimHandler* m_cah = 0;

//static float s_shadowbias = 0.005f;
static int s_shadowmode = 0;
static float s_shadowcoef[2] = { 0.005f, 1.0f };// [0]: bias, [1]: darkness
static int s_keyboardcnt[256];

typedef struct tag_tonemapelem
{
	int tonemapid;
	int tonenum;
	int* pscid;
	int* ptexid;
} TONEMAPELEM;
#define TONEMAPMAX	10
static TONEMAPELEM s_tonemapelem[ TONEMAPMAX ];
static int s_usertonemapcnt = 0;


typedef struct tag_hsidelem
{
	int hsid;
	int validflag;
	CHandlerSet* hsptr;
} HSIDELEM;
#define HSIDMAX	1001
static HSIDELEM s_hsidelem[ HSIDMAX ];


static int s_swapchaincnt = 0;
typedef struct tag_swapchainelem
{
	int scid;
	int wid;
	int objid;
	HWND hwnd;
	IDirect3DSwapChain9* pSwapChain;
	IDirect3DSurface9* pZSurf;
	IDirect3DSurface9* pBackBuffer;
	int texid;
	POINT texsize;
	D3DCOLOR clearcol;
} SWAPCHAINELEM;

#define SCMAX	256
static SWAPCHAINELEM s_sctable[ SCMAX ];


typedef struct tag_bgdispelem
{
	int scid;
	CBgDisp2* bgdisp;
} BGDISPELEM;
static BGDISPELEM s_bgdisp[ SCMAX ];



typedef struct tag_threadtable
{
	int threadid;
	HANDLE hThread;
	DWORD dwId;

	int returnval;
	int returnval2;

	char* filename;
	char* filename2;
	char* filename3;
	char* filename4;
	//int mult100;
	float mult;
	int adjustuvflag;

	char* bufptr;
	int bufsize;

	float maxx;
	float maxz;
	int divx;
	int divz;
	float maxheight;

	int hsid;
	int useflag;
}THREADTABLE;


#define THMAX	4098
static THREADTABLE s_thtable[THMAX];
//static int s_threadnum = 0;

static int s_callinitflag = 0;
static HMODULE s_hMod = 0;


/////////////////


////////////////
// global

//float g_specular = 1.0f;
//float g_ambient = 1.0f;

int g_index32bit = 0;

SLispStatus	g_sStatus;//!< LiSPSM�X�e�[�^�X

int g_activateapp = 0;

ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
ID3DXEffect*			g_pPostEffect = NULL;

CRITICAL_SECTION g_crit_hshead;
//CRITICAL_SECTION g_crit_tempmp; 
CRITICAL_SECTION g_crit_restore;

float g_proj_near;// = 100.0f;
float g_proj_far;// = 10000.0f;
float g_proj_fov;// = 45.0f;
float g_fovindex[ FOVINDEXSIZE + 1 ];
int	g_ortho;
float g_orthosize;

int g_fogenable;// = 0;
D3DCOLOR g_fogcolor;// = 0x00FFFFFF;
float g_fogstart;// = 0.0f;
float g_fogend;// = 10000.0f;	
float g_fogtype = 0.0f;
static int s_savefogenable;// = g_fogenable;

int g_rendercnt = 0;
int g_presentinterval = D3DPRESENT_INTERVAL_IMMEDIATE;

D3DXMATRIX g_matProj;
POINT		g_scsize;

DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];
int g_rsindex[80] = {
    D3DRS_ZENABLE, D3DRS_FILLMODE, D3DRS_SHADEMODE, -1, D3DRS_ZWRITEENABLE, 
	D3DRS_ALPHATESTENABLE, D3DRS_LASTPIXEL, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_CULLMODE,

    D3DRS_ZFUNC, D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_DITHERENABLE, D3DRS_ALPHABLENDENABLE,
	D3DRS_FOGENABLE, D3DRS_SPECULARENABLE, -1, D3DRS_FOGCOLOR, D3DRS_FOGTABLEMODE,
    
	D3DRS_FOGSTART, D3DRS_FOGEND, D3DRS_FOGDENSITY, -1, -1,
	D3DRS_RANGEFOGENABLE, D3DRS_STENCILENABLE, D3DRS_STENCILFAIL, D3DRS_STENCILZFAIL, D3DRS_STENCILPASS,

    D3DRS_STENCILFUNC, D3DRS_STENCILREF, D3DRS_STENCILMASK, D3DRS_STENCILWRITEMASK, D3DRS_TEXTUREFACTOR,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_WRAP2, D3DRS_WRAP3, D3DRS_WRAP4,
    
	D3DRS_WRAP5, D3DRS_WRAP6, D3DRS_WRAP7, D3DRS_CLIPPING, D3DRS_LIGHTING,
	D3DRS_AMBIENT, D3DRS_FOGVERTEXMODE, D3DRS_COLORVERTEX, D3DRS_LOCALVIEWER, D3DRS_NORMALIZENORMALS,

    D3DRS_DIFFUSEMATERIALSOURCE, D3DRS_SPECULARMATERIALSOURCE, D3DRS_AMBIENTMATERIALSOURCE, D3DRS_EMISSIVEMATERIALSOURCE, D3DRS_VERTEXBLEND,
	D3DRS_CLIPPLANEENABLE, -1, D3DRS_POINTSIZE, D3DRS_POINTSIZE_MIN, D3DRS_POINTSPRITEENABLE,

    D3DRS_POINTSCALEENABLE, D3DRS_POINTSCALE_A, D3DRS_POINTSCALE_B, D3DRS_POINTSCALE_C, D3DRS_MULTISAMPLEANTIALIAS,
	D3DRS_MULTISAMPLEMASK, D3DRS_PATCHEDGESTYLE, -1, D3DRS_DEBUGMONITORTOKEN, D3DRS_POINTSIZE_MAX,

    D3DRS_INDEXEDVERTEXBLENDENABLE, D3DRS_COLORWRITEENABLE, D3DRS_TWEENFACTOR, D3DRS_BLENDOP, -1,
	-1,-1,-1,-1,-1
};

static int s_partialrs[ 28 ] = {
	D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_LIGHTING, D3DRS_CULLMODE, D3DRS_SHADEMODE,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_DITHERENABLE, D3DRS_SPECULARENABLE, D3DRS_ZENABLE, 
	D3DRS_AMBIENT, D3DRS_NORMALIZENORMALS, D3DRS_VERTEXBLEND, D3DRS_ZWRITEENABLE, D3DRS_ZFUNC, 
	D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_ALPHABLENDENABLE, D3DRS_MULTISAMPLEANTIALIAS, D3DRS_MULTISAMPLEMASK,
	D3DRS_BLENDOP, D3DRS_TEXTUREFACTOR, D3DRS_FOGENABLE, D3DRS_FOGCOLOR, D3DRS_FOGSTART,
	D3DRS_FOGEND, D3DRS_FOGDENSITY, D3DRS_FOGVERTEXMODE
	
};

DWORD g_minfilter = D3DTEXF_LINEAR;
DWORD g_magfilter = D3DTEXF_LINEAR;

CNaviLine* g_nlhead = 0;


UINT g_miplevels = 1;//!!!!!!!!!!!!!!!!
DWORD g_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;

CCpuid g_cpuinfo;

RTSC g_rtsc;

int g_LightOfBump[3] = { 0, -1, -1 };
int g_lightnum = 0;
int g_bumplightnum = 0;

int g_usePostEffect = 1;

//------------------------
//shader
//------------------------
D3DXHANDLE g_hRenderSceneBone4 = 0;
D3DXHANDLE g_hRenderSceneBone1 = 0;
D3DXHANDLE g_hRenderSceneBone0 = 0;
D3DXHANDLE g_hRenderSceneBone4Toon = 0;
D3DXHANDLE g_hRenderSceneBone1Toon = 0;
D3DXHANDLE g_hRenderSceneBone0Toon = 0;
D3DXHANDLE g_hRenderSceneBone4ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone1ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone0ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone4Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone1Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone0Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone4Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone1Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone0Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone4Edge0 = 0;
D3DXHANDLE g_hRenderSceneBone1Edge0 = 0;
D3DXHANDLE g_hRenderSceneBone0Edge0 = 0;
D3DXHANDLE g_hRenderSceneNormalShadow = 0;//vs2
D3DXHANDLE g_hRenderSceneBumpShadow = 0;//vs2
D3DXHANDLE g_hRenderSceneToon1Shadow = 0;//vs2
D3DXHANDLE g_hRenderSceneToon0Shadow = 0;//vs2
D3DXHANDLE g_hRenderSceneBone4PP = 0;
D3DXHANDLE g_hRenderSceneBone0PP = 0;
D3DXHANDLE g_hRenderScenePPShadow = 0;


D3DXHANDLE g_hmWorldMatrixArray = 0;
D3DXHANDLE g_hmView = 0;
D3DXHANDLE g_hmoldView = 0;
D3DXHANDLE g_hmProj = 0;
D3DXHANDLE g_hmEyePos = 0;
D3DXHANDLE g_hmLightParams = 0;
D3DXHANDLE g_hmBumpLight = 0;//vs2
D3DXHANDLE g_hmFogParams = 0;
D3DXHANDLE g_hmToonParams = 0;
D3DXHANDLE g_hmLightNum = 0;
D3DXHANDLE g_hmEdgeCol0 = 0;
D3DXHANDLE g_hmAlpha = 0;
D3DXHANDLE g_hNormalMap = 0;//vs2
D3DXHANDLE g_hDecaleTex = 0;//vs2
D3DXHANDLE g_hDecaleTex1 = 0;//vs2
D3DXHANDLE g_hmLP = 0;//vs2
D3DXHANDLE g_hmLPB = 0;//vs2
D3DXHANDLE g_hShadowMap = 0;//vs2
D3DXHANDLE g_hmShadowCoef = 0;//vs2
D3DXHANDLE g_hmMinAlpha = 0;
D3DXHANDLE g_hmGlowMult = 0;
D3DXHANDLE g_hmToonDiffuse = 0;
D3DXHANDLE g_hmToonAmbient = 0;
D3DXHANDLE g_hmToonSpecular = 0;

D3DXHANDLE g_hPostEffectBlur0 = 0;
D3DXHANDLE g_hPostEffectGauss = 0;
D3DXHANDLE g_hPostEffectTLV = 0;
D3DXHANDLE g_hPostEffectDownScale = 0;
D3DXHANDLE g_hPostEffectToneMap = 0;
D3DXHANDLE g_hPostEffectBrightPass = 0;
D3DXHANDLE g_hPostEffectHDRFinal = 0;
D3DXHANDLE g_hPostEffectMono = 0;
D3DXHANDLE g_hPostEffectCbCr = 0;
D3DXHANDLE g_hPostEffectUser[5] = {0, 0, 0, 0, 0};


D3DXHANDLE g_hpeWidth0 = 0;
D3DXHANDLE g_hpeHeight0 = 0;
D3DXHANDLE g_hpeWidth1 = 0;
D3DXHANDLE g_hpeHeight1 = 0;
D3DXHANDLE g_hpeTexture0 = 0;
D3DXHANDLE g_hpeTexture1 = 0;
D3DXHANDLE g_hpeTexture2 = 0;

D3DXHANDLE g_hpeWeight = 0;
D3DXHANDLE g_hpeOffsetX = 0;
D3DXHANDLE g_hpeOffsetY = 0;
D3DXHANDLE g_hpeSampleWeight = 0;
D3DXHANDLE g_hpeSampleOffset = 0;
D3DXHANDLE g_hpeMiddleGray = 0;
D3DXHANDLE g_hpeBrightPassThreshold = 0;
D3DXHANDLE g_hpeBrightPassOffset = 0;
D3DXHANDLE g_hpeColMult = 0;
D3DXHANDLE g_hpeCbCr = 0;
D3DXHANDLE g_hpeUserFl4[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
D3DXHANDLE g_hpeUserTex[5] = {0, 0, 0, 0, 0};


////// global func

int e3dRestoreDeviceObject();
int e3dInvalidateDeviceObject( int resetflag );
CHandlerSet* GetHandlerSet( int hsid );


///////////////////
// extern

extern int g_useGPU;
extern int g_chkVS;
extern CD3DApplication* g_pD3DApp = 0;
extern CLightData* g_lighthead;// d3ddisp.cpp�Œ�`�A�g�p�B

extern CHandlerSet* g_bbhs;

extern CTexBank* g_texbnk;// shdhandler.cpp


extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;

extern int g_bumpEnable;
extern int g_bumpUse;
extern int g_shadowEnable;
extern int g_shadowUse;


//////////////////
// static
//static CHandlerSet* hshead = 0;
static CAviData* avihead = 0;
static CXFont* xfonthead = 0;

static HWND s_hwnd = 0;
static HWND s_PBwnd = 0;
static HINSTANCE s_inst = 0;


static DWORD s_clientWidth = 0;
static DWORD s_clientHeight = 0;

static D3DXMATRIX s_matView;
static D3DXMATRIX s_matViewShadow;
static D3DXMATRIX s_matProjShadow;
static D3DXMATRIX s_matBiasShadow;
static LPDIRECT3DTEXTURE9 s_ShadowMapTex = NULL;


static LPD3DXSPRITE	s_d3dxsprite = 0;
static CMySprite* s_spritehead = 0;

static CQHandler* s_qh = 0;
static CQuaternion2 s_cameraq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

static CHuSound* s_HS = 0;

static int s_multisample = 0;


static int CameraElemToParams( CAMERAELEM ce );

static CAviData* GetAviData( int aviid );
static CXFont* GetXFont( int fontid );
static CLightData* GetLightData( int lid );
static CMySprite* GetSprite( int spriteid );

static CNaviLine* GetNaviLine( int nlid );

static int ChainNewHandlerSet( CHandlerSet* newhs );

static int Restore( CHandlerSet* hsptr, int billboardid );
static int FreeHandlerSet();
static int DirToTheOtherXZ( CGPData* e3d1, CGPData e3d2 );
static int DirToTheOther( CGPData* e3d1, CGPData e3d2 );
static int SeparateFrom( int hsid1, int hsid2, float dist );
static int CheckOutOfRange( CGPData* charaptr );
static int CreateAfterImage( int hsid, LPDIRECT3DDEVICE9 pdev, int imagenum );
static int MoveGirlXZ( int hsid, int keyid );

static int SetModuleDir();
static int SetResDir0();
static int SetResDir( char* filename, CHandlerSet* hsptr );

static int DestroySoundObject();

static int SetFovIndex();

static int SetDefaultRenderState();
static int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev );

static int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );
static int DestroyHandlerSet( int hsid );
static int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag );


static int CatmullRomPoint( D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* p3, D3DXVECTOR3* p4, float t, D3DXVECTOR3* resp );
static int GetScreenPos( D3DXVECTOR3 vec3d, int* x2dptr, int* y2dptr, int* validptr );

static int CalcMatView();
static int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos );
static int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir );

static int DestroyTexture( int hsid, int bbid, int spriteid, int dirtyreset, int bgno );


static int s_cameratype = 0;
static D3DXVECTOR3 s_camerapos( 0.0f, 0.0f, 4200.0f );
static float s_camera_degxz = 0.0f;
static float s_camera_degy = 0.0f;
static D3DXVECTOR3 s_cameratarget( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_cameraupvec( 0.0f, 1.0f, 0.0f );
static float s_camera_twist = 0.0f;

static D3DXVECTOR3 s_camposshadow( 0.0f, 10000.0f, 10000.0f );
static D3DXVECTOR3 s_camtarshadow( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_camupshadow( 0.0f, 1.0f, 0.0f );

static D3DXVECTOR3 s_lightdirshadow( 0.0f, -1.0f, 1.0f );

static int byeflag = 0;


static int SetMovableArea_F( char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr );
static DWORD WINAPI ThreadSetMovableArea( LPVOID lpThreadParam );

static int LoadMQOFileAsGround_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype );
static DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID lpThreadParam );

static int SigLoad_F( char* tempname, float mult, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadSigLoad( LPVOID lpThreadParam );

static int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadSigLoadFromBuf( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableArea_F( char* tempname, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID lpThreadParam );

static int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, float maxx, float maxz, int divx, int divz, float maxheight, int TLmode, int* hsidptr );
static DWORD WINAPI ThreadLoadGroundBMP( LPVOID lpThreadParam );

static int LoadGNDFile_F( char* tempname, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFile( LPVOID lpThreadParam );

static int LoadGNDFileFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFileFromBuf( LPVOID lpThreadParam );

static int LoadSigFileAsGround_F( char* tempname, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadSigFileAsGround( LPVOID lpThreadParam );

static int LoadSigFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadSigFileAsGroundFromBuf( LPVOID lpThreadParam );

static int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr, float mvmult );
static DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam );

static int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr, float mvmult );
static DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam );

static SWAPCHAINELEM* GetFreeSwapChainElem();
static SWAPCHAINELEM* GetSwapChainElem( int scid );
static int DestroySwapChainElem( SWAPCHAINELEM* cursc, int initflag );
static int CreateSwapChainElem( int scid, SWAPCHAINELEM* cursc, int wid, int objid, int texid, D3DCOLOR* clearcol );

static BGDISPELEM* GetFreeBGDispElem();
static BGDISPELEM* GetBGDispElem( int scid );
int GetName( char* dstname, int srcleng );

static int DisableFogState();
static int RollbackFogState();

//static D3DXVECTOR3 s_cameradir( 0.0f, 0.0f, -1.0f );
static int s_afterimagenum = 5;

static float s_movestep = 100.0f;

static LONGLONG s_timerfreq = 0;

static int s_usertexcnt = 0;

static int XORFuncBeta( unsigned char* newbuf, int bufleng, int* xorptr );

static int InitThTableElem( int thno );
static int GetFreeThTable();
static int FreeThTableElem( int tableno );

static int SetShaderConst( int shadowflag );
static int GetShaderHandle();

static int CreateRenderTargetTexture( int sizex, int sizey, int srcfmt, int* scidptr, int* texidptr );
static TONEMAPELEM* GetFreeToneMapElem();
static TONEMAPELEM* GetToneMapElem( int tonemapid );
static int ToneMapFirst( int scid, int texid );
static int ToneMapSecond( int scid, int texid );
static int ToneMapFinal( int scid, int texid );


static int SetMatProj( SWAPCHAINELEM* cursc );

static int SetPointListForShadow( CVec3List* plist, float aspect, int* hsidptr, int num );
static int ChkInViewAndTransformBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int passno, int needchkinview, int needtrans );
static int SetRenderTarget( LPDIRECT3DDEVICE9 pdev, int scid, int shadowmapflag, int skipflag );
static int RenderShadowMap0( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num );
static int RenderShadowMap1( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha );
static int SetTempFogEnable( LPDIRECT3DDEVICE9 pdev, int srcenable );
static int RestoreTempFogEnable();
static int RenderNormalBatch( int scid, int* hsidptr, int num, int needchkinview, int skipflag, int* modeptr );
static int RenderBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha, int* modeptr );

static int SetEffectDecl( LPDIRECT3DDEVICE9 pdev );
static IDirect3DVertexDeclaration9* s_effectdecl = 0;
static IDirect3DVertexDeclaration9* s_tlvdecl = 0;


static int RTBeginScene( LPDIRECT3DDEVICE9 pdev, int scid, int skipbeginscene, int needz );
static int SetRTRenderState( LPDIRECT3DDEVICE9 pdev, int needz );
static int SetGaussWeight( float dispersion );
static int SetGauss5x5Weight( float fw, float fh, float dispersion );
static int SetDownScale4x4Offset( float fw, float fh );
static int SetDownScale2x2Offset( float fw, float fh );
static int SetToneMapFirstOffset( float fw, float fh );


////////////////////////
/// plugin func

static int E3DInit();//0x00
static int E3DBye();//0x01
static int E3DSigLoad();//0x02
static int E3DCreateSwapChain();//0x03
static int E3DDestroySwapChain();//0x04
static int E3DRender();//0x05
static int E3DPresent();//0x06
static int E3DBeginScene();//0x07
static int E3DEndScene();//0x08
static int E3DCreateBG();//0x09
static int E3DSetBGU();//0x0A
static int E3DSetBGV();//0x0B
static int E3DDestroyBG();//0x0C
static int E3DAddMotion();//0x0D
static int E3DSetMotionKind();//0x0E
static int E3DGetMotionKind();//0x0F
static int E3DSetNewPose();//0x10
static int E3DSetMotionStep();//0x11
static int E3DChkConflict();//0x12
static int E3DChkConflict2();//0x13

static int E3DCreateAfterImage();//0x14
static int E3DDestroyAfterImage();//0x15
static int E3DSetAlpha();//0x16
static int E3DSetAlpha2();//0x17
static int E3DSetPos();//0x18
static int E3DGetPos();//0x19
static int E3DSetDir();//0x1A
static int E3DRotateInit();//0x1B
static int E3DRotateX();//0x1C
static int E3DRotateY();//0x1D
static int E3DRotateZ();//0x1E
static int E3DTwist();//0x1F
static int E3DPosForward();//0x20
static int E3DCloseTo();//0x21
static int E3DDirToTheOtherXZ();//0x22
static int E3DDirToTheOther();//0x23
static int E3DSeparateFrom();//0x24
static int E3DGetCameraPos();//0x25
static int E3DSetCameraPos();//0x26
static int E3DGetCameraDeg();//0x27
static int E3DSetCameraDeg();//0x28
static int E3DSetCameraTarget();//0x29
static int E3DChkInView();//0x2A
static int E3DEnableDbgFile();//0x2B
static int E3DSetProjection();//0x2C
static int E3DGetKeyboardState();//0x2D

static int E3DCos();//0x2E
static int E3DSin();//0x2F
static int E3DACos();//0x30
static int E3DDot();//0x31
static int E3DCross();//0x32
static int E3DAtan();//0x33
static int E3DAtan2();//0x34
static int E3DSqrt();//0x35
static int E3DDrawText();//0x36
static int E3DDrawBigText();//0x37
static int E3DRand();//0x38
static int E3DSeed();//0x39
static int E3DGetFPS();//0x3A
static int E3DWaitbyFPS();//0x3B
static int E3DCreateLight();//0x3C
static int E3DSetDirectionalLight();//0x3D
static int E3DSetPointLight();//0x3E
static int E3DSetSpotLight();//0x3F
static int E3DDestroyLight();//0x40
static int E3DClearZ();//0x41
static int E3DDestroyHandlerSet();//0x42
static int E3DSetDispSwitch();//0x43
static int E3DSetMotionFrameNo();//0x44
static int E3DCreateSprite();//0x45
static int E3DBeginSprite();//0x46
static int E3DEndSprite();//0x47
static int E3DRenderSprite();//0x48
static int E3DGetSpriteSize();//0x49
static int E3DSetSpriteRotation();//0x4A
static int E3DSetSpriteARGB();//0x4B
static int E3DDestroySprite();//0x4C
static int E3DChkConfBillboard2();//0x4D
static int E3DChkConfBillboard();//0x4E
static int E3DChkConfBySphere();//0x4F
static int E3DChkConfBySphere2();//0x50
static int E3DCreateProgressBar();//0x051
static int E3DSetProgressBar();//0x052
static int E3DDestroyProgressBar();//0x053
static int E3DLoadGroundBMP();//0x054
static int E3DCameraPosForward();//0x055
static int E3DSetBeforePos();//0x056
static int E3DChkConfGround();//0x057
static int E3DChkConfGround2();//0x058
static int E3DGetPartNoByName();//0x059
static int E3DGetVertNumOfPart();//0x05A
static int E3DGetVertPos();//0x05B
static int E3DSetVertPos();//0x05C
static int E3DSetPosOnGround();//0x05D
static int E3DSetPosOnGroundPart();//0x05E
static int E3DCreateBillboard();//0x05F
static int E3DRenderBillboard();//0x060
static int E3DSetBillboardPos();//0x061
static int E3DSetBillboardOnGround();//0x062
static int E3DDestroyBillboard();//0x063

static int E3DLoadMQOFileAsGround();//0x064
static int E3DSaveMQOFile();//0x065
static int E3DGetBillboardInfo();//0x066
static int E3DGetNearBillboard();//0x067
static int E3DGetInvisibleFlag();//0x068
static int E3DSetInvisibleFlag();//0x069
static int E3DSetMovableArea();//0x06A
static int E3DChkConfWall();//0x06B
static int E3DChkConfWall2();//0x06C
static int E3DVec3Normalize();//0x06D
static int E3DVec2CCW();//0x06E
static int E3DVec3RotateY();//0x06F
static int E3DLoadMQOFileAsMovableArea();//0x070
static int E3DLoadSound();//0x071
static int E3DPlaySound();//0x072
static int E3DStopSound();//0x073
static int E3DSetSoundLoop();//0x074
static int E3DSetSoundVolume();//0x075
static int E3DSetSoundTempo();//0x076
static int E3DDestroySound();//0x077
static int E3DSetSoundFrequency();//0x078
static int E3DGetSoundVolume();//0x079
static int E3DGetSoundFrequency();//0x07A
static int E3DSet3DSoundListener();//0x07B
static int E3DSet3DSoundListenerMovement();//0x07C
static int E3DSet3DSoundDistance();//0x07D
static int E3DSet3DSoundMovement();//0x07E
static int E3DCreateNaviLine();//0x07F
static int E3DDestroyNaviLine();//0x080
static int E3DAddNaviPoint();//0x081
static int E3DRemoveNaviPoint();//0x082
static int E3DGetNaviPointPos();//0x083
static int E3DSetNaviPointPos();//0x084
static int E3DGetNaviPointOwnerID();//0x085
static int E3DSetNaviPointOwnerID();//0x086
static int E3DGetNextNaviPoint();//0x087
static int E3DGetPrevNaviPoint();//0x088
static int E3DGetNearestNaviPoint();//0x089
static int E3DFillUpNaviLine();//0x08A
static int E3DSetNaviLineOnGround();//0x08B
static int E3DControlByNaviLine();//0x08C
static int E3DSetDirQ();//0x08D
static int E3DGetDirQ();//0x08E
static int E3DSetWallOnGround();//0x08F
static int E3DCreateNaviPointClearFlag();//0x090
static int E3DDestroyNaviPointClearFlag();//0x091
static int E3DInitNaviPointClearFlag();//0x092
static int E3DSetNaviPointClearFlag();//0x093
static int E3DGetOrder();//0x094
static int E3DDestroyAllBillboard();//0x095
static int E3DSetValidFlag();//0x096
static int E3DSetDiffuse();//0x097
static int E3DSetSpecular();//0x098
static int E3DSetAmbient();//0x099
static int E3DSetBlendingMode();//0x09A
static int E3DSetRenderState();//0x09B
static int E3DSetScale();//0x09C
static int E3DGetScreenPos();//0x09D
static int E3DGetScreenPos2();//0x09E
static int E3DCreateQ();//0x09F
static int E3DDestroyQ();//0x0A0
static int E3DInitQ();//0x0A1
static int E3DSetQAxisAndDeg();//0x0A2
static int E3DGetQAxisAndDeg();//0x0A3
static int E3DRotateQX();//0x0A4
static int E3DRotateQY();//0x0A5
static int E3DRotateQZ();//0x0A6
static int E3DMultQ();//0x0A7
static int E3DNormalizeQ();//0x0A8
static int E3DCopyQ();//0x0A9


static int E3DGetBoneNoByName();//0x0AA
static int E3DGetNextMP();//0x0AB
static int E3DSetMPInfo();//0x0AC
static int E3DIsExistMP();//0x0AD
static int E3DGetMotionFrameLength();//0x0AE
static int E3DSetMotionFrameLength();//0x0AF
static int E3DAddMP();//0x0B0
static int E3DDeleteMP();//0x0B1
static int E3DFillUpMotion();//0x0B2
static int E3DCopyMotionFrame();//0x0B3
static int E3DGetMPInfo();//0x0B4
static int E3DGetDirQ2();//0x0B5
static int E3DSetDirQ2();//0x0B6
static int E3DLookAtQ();//0x0B7
static int E3DMultQVec();//0x0B8
static int E3DTwistQ();//0x0B9
static int E3DInitTwistQ();//0x0BA
static int E3DGetTwistQ();//0x0BB
static int E3DRotateQLocalX();//0x0BC
static int E3DRotateQLocalY();//0x0BD
static int E3DRotateQLocalZ();//0x0BE
static int E3DGetBonePos();//0x0BF
static int E3DCreateLine();//0x0C0
static int E3DDestroyLine();//0x0C1
static int E3DSetLineColor();//0x0C2
static int E3DAddPoint2Line();//0x0C3
static int E3DDeletePointOfLine();//0x0C4
static int E3DSetPointPosOfLine();//0x0C5
static int E3DGetPointPosOfLine();//0x0C6
static int E3DGetNextPointOfLine();//0x0C7
static int E3DGetPrevPointOfLine();//0x0C8
static int E3DWriteDisplay2BMP();//0x0C9
static int E3DCreateAVIFile();//0x0CA
static int E3DWriteData2AVIFile();//0x0CB
static int E3DCompleteAVIFile();//0x0CC
static int E3DCameraLookAt();//0x0CD
static int E3DCameraOnNaviLine();//0x0CE
static int E3DCameraDirUp();//0x0CF
static int E3DCameraDirDown();//0x0D0
static int E3DCameraDirRight();//0x0D1
static int E3DCameraDirLeft();//0x0D2
static int E3DCreateFont();//0x0D3
static int E3DDrawTextByFontID();//0x0D4
static int E3DDrawTextByFontIDWithCnt();//0x0D5
static int E3DDrawTextByFontIDWithCnt2();//0x0D6
static int E3DGetCharacterNum();//0x0D7
static int E3DDestroyFont();//0x0D8
static int E3DSlerpQ();//0x0D9
static int E3DSquadQ();//0x0DA
static int E3DSplineVec();//0x0DB
static int E3DDbgOut();//0x0DC
static int E3DGetDiffuse();//0x0DD
static int E3DGetAmbient();//0x0DE
static int E3DGetSpecular();//0x0DF
static int E3DGetAlpha();//0x0E0
static int E3DSaveQuaFile();//0x0E1
static int E3DSaveSigFile();//0x0E2
static int E3DSetMipMapParams();//0x0E3
static int E3DPickVert();//0x0E4
static int E3DGetCullingFlag();//0x0E5
static int E3DGetOrgVertNo();//0x0E6
static int E3DChkIM2Status();//0x0E7
static int E3DLoadIM2File();//0x0E8
static int E3DSaveIM2File();//0x0E9
static int E3DGetJointNum();//0x0EA
static int E3DGetJointInfo();//0x0EB
static int E3DGetFirstJointNo();//0x0EC
static int E3DGetDispObjNum();//0x0ED
static int E3DGetDispObjInfo();//0x0EE
static int E3DEnableTexture();//0x0EF
static int E3DJointAddToTree();//0x0F0
static int E3DJointRemoveFromParent();//0x0F1
static int E3DJointRemake();//0x0F2
static int E3DSigImport();//0x0F3
static int E3DSigLoadFromBuf();//0x0F4
static int E3DSigImportFromBuf();//0x0F5
static int E3DAddMotionFromBuf();//0x0F6
static int E3DCheckFullScreenParams();//0x0F7
static int E3DGetMaxMultiSampleNum();//0x0F8
static int E3DChkConfLineAndFace();//0x0F9
static int E3DPickFace();//0x0FA
static int E3DGetBBox();//0x0FB
static int E3DGetVertNoOfFace();//0x0FC
static int E3DGetSamePosVert();//0x0FD
static int E3DRdtscStart();//0x0FE
static int E3DRdtscStop();//0x0FF
static int E3DSaveSig2Buf();//0x100
static int E3DSaveQua2Buf();//0x101
static int E3DCameraShiftLeft();//0x102
static int E3DCameraShiftRight();//0x103
static int E3DCameraShiftUp();//0x104
static int E3DCameraShiftDown();//0x105
static int E3DGetCameraQ();//0x106
static int E3DInvQ();//0x107
static int E3DSetCameraTwist();//0x108


static int E3DIKRotateBeta();//0x109
static int E3DIKRotateBeta2D();//0x10A
static int E3DGetMotionType();//0x10B
static int E3DSetMotionType();//0x10C
static int E3DGetIKTransFlag();//0x10D
static int E3DSetIKTransFlag();//0x10E
static int E3DDestroyAllMotion();//0x10F
static int E3DGetUserInt1OfPart();//0x110
static int E3DSetUserInt1OfPart();//0x111
static int E3DGetBSphere();//0x112
static int E3DGetChildJoint();//0x113
static int E3DDestroyMotionFrame();//0x114
static int E3DGetKeyFrameNo();//0x115
static int E3DConvScreenTo3D();//0x116
static int E3DVec3Length();//0x117
static int E3DSetUV();//0x118
static int E3DGetUV();//0x119
static int E3DPickBone();//0x11A
static int E3DShiftBoneTree2D();//0x11B
static int E3DGetDispSwitch();//0x11C
static int E3DRotateBillboard();//0x11D
static int E3DSetBillboardUV();//0x11E
static int E3DCreateTexture();//0x11F
static int E3DGetTextureInfo();//0x120
static int E3DCopyTextureToTexture();//0x121
static int E3DGetTextureFromDispObj();//0x122
static int E3DSetTextureToDispObj();//0x123
static int E3DRasterScrollTexture();//0x124
static int E3DCopyTextureToBackBuffer();//0x125
static int E3DDestroyTexture();//0x126
static int E3DSetLightAmbient();//0x127
static int E3DSetLightSpecular();//0x128
static int E3DInvColTexture();//0x129
static int E3DSaveGndFile();//0x12A
static int E3DLoadGndFile();//0x12B
static int E3DLoadGndFileFromBuf();//0x12C
static int E3DCreateTextureFromBuf();//0x12D
static int E3DLoadSoundFromBuf();//0x12E
static int E3DTwistBone();//0x12F
static int E3DSetStartPointOfSound();//0x130


static int E3DGetBoneQ();//0x131
static int E3DSetBoneQ();//0x132
static int E3DIsSoundPlaying();//0x133
static int E3DIKTranslate();//0x134
static int E3DSetUVTile();//0x135
static int E3DImportMQOFileAsGround();//0x136
static int E3DLoadMQOFileAsMovableAreaFromBuf();//0x137
static int E3DChkThreadWorking();//0x138
static int E3DLoadMQOFileAsGroundThread();//0x139
static int E3DSigLoadThread();//0x13A
static int E3DSigLoadFromBufThread();//0x13B
static int E3DLoadMQOFileAsMovableAreaThread();//0x13C
static int E3DLoadMQOFileAsMovableAreaFromBufThread();//0x13D
static int E3DLoadGroundBMPThread();//0x13E
static int E3DLoadGndFileThread();//0x13F
static int E3DLoadGndFileFromBufThread();//0x140
static int E3DAddMotionThread();//0x141
static int E3DAddMotionFromBufThread();//0x142
static int E3DGetShaderType();//0x143
static int E3DSetShaderType();//0x144
static int E3DSetLightBlendMode();//0x145
static int E3DGetLightBlendMode();//0x146
static int E3DSetEmissive();//0x147
static int E3DSetSpecularPower();//0x148
static int E3DGetEmissive();//0x149
static int E3DGetSpecularPower();//0x14A
static int E3DGetInfElemNum();//0x14B
static int E3DGetInfElem();//0x14C
static int E3DSetInfElem();//0x14D
static int E3DDeleteInfElem();//0x14E
static int E3DSetInfElemDefault();//0x14F
static int E3DNormalizeInfElem();//0x150
static int E3DGetVisiblePolygonNum();//0x151
static int E3DChkConfGroundPart();//0x152
static int E3DChkConfGroundPart2();//0x153
static int E3DGetMidiMusicTime();//0x154
static int E3DSetNextMotionFrameNo();//0x155

static int E3DSetLinearFogParams();//0x156
static int E3DSetToon0Params();//0x157

static int E3DChkConflict3();//0x158
static int E3DChkConfBySphere3();//0x159

static int E3DSetMovableAreaThread();//0x15A
static int E3DCreateSpriteFromBMSCR();//0x15B

static int E3DEncodeBeta();//$15C
static int E3DDecodeBeta();//$15D
static int E3DChkVertexShader();//$15E

static int E3DGetReferenceTime();//$15F
static int E3DLoadMOAFile();//$160
static int E3DSetNewPoseByMOA();//$161

static int E3DGetKeyboardCnt();//$162
static int E3DResetKeyboardCnt();//$163
static int E3DGetMotionFrameNo();//$164

static int E3DGetMoaInfo();//$165
static int E3DGetNextMotionFrameNo();//$166
static int E3DGetScreenPos3();//$167

static int E3DEnableToonEdge();//$168
static int E3DSetToonEdge0Color();//$169
static int E3DSetToonEdge0Width();//$16A
static int E3DGetToonEdge0Color();//$16B
static int E3DGetToonEdge0Width();//$16C

static int E3DCreateParticle();//$16D
static int E3DSetParticlePos();//$16E
static int E3DSetParticleGravity();//$16F
static int E3DSetParticleLife();//$170
static int E3DSetParticleEmitNum();//$171
static int E3DSetParticleVel0();//$172
static int E3DRenderParticle();//$173
static int E3DSetParticleRotation();//$174
static int E3DSetParticleDiffuse();//$175
static int E3DDestroyParticle();//$176
static int E3DSetParticleAlpha();//$177
static int E3DSetParticleUVTile();//$178
static int E3DInitParticle();//$179

static int E3DClone();//$17A
static int E3DPickFace2();//$17B
static int E3DChkConfWall3();//$17C
static int E3DGetMotionIDByName();//$17D
static int E3DDot2();//$17E
static int E3DGetMotionNum();//$17F

static int E3DChkConfParticle();//$180

static int E3DLoadSigFileAsGround();//$181
static int E3DLoadSigFileAsGroundFromBuf();//$182
static int E3DGetCenterPos();//$183

static int E3DGetFaceNum();//$184
static int E3DGetFaceNormal();//$185
static int E3DCreateEmptyMotion();//$186
static int E3DSetTextureMinMagFilter();//$187
static int E3DGetMotionName();//$188
static int E3DSetMotionName();//$189

static int E3DGetMaterialNoByName();//$18A
static int E3DGetMaterialAlpha();//$18B
static int E3DGetMaterialDiffuse();//$18C
static int E3DGetMaterialSpecular();//$18D
static int E3DGetMaterialAmbient();//$18E
static int E3DGetMaterialEmissive();//$18F
static int E3DSetMaterialAlpha();//$190
static int E3DSetMaterialDiffuse();//$191
static int E3DSetMaterialSpecular();//$192
static int E3DSetMaterialAmbient();//$193
static int E3DSetMaterialEmissive();//$194
static int E3DGetTextureFromMaterial();//$195
static int E3DSetTextureToMaterial();//$196
static int E3DGetMaterialPower();//$197
static int E3DSetMaterialPower();//$198
static int E3DGetMaterialBlendingMode();//$199
static int E3DSetMaterialBlendingMode();//$19A

static int E3DGetMaterialNo();//$19B
static int E3DSetMotionKindML();//$19C
static int E3DSetMotionFrameNoML();//$19D
static int E3DSetNewPoseML();//$19E
static int E3DSetMOAEventNoML();//$19F
static int E3DSetNextMotionFrameNoML();//$1A0
static int E3DGetMotionFrameNoML();//$1A1
static int E3DGetNextMotionFrameNoML();//$1A2

static int E3DGetCurrentBonePos();//$1A3
static int E3DGetCurrentBoneQ();//$1A4

static int E3DChkBumpMapEnable();//$1A5
static int E3DEnableBumpMap();//$1A6
static int E3DConvHeight2NormalMap();//$1A7
static int E3DSetBumpMapToMaterial();//$1A8

static int E3DDumpQ();//$1A9
static int E3DSetDbgQ();// $1AA

static int E3DGetMOATrunkInfo();// $1AB
static int E3DGetMOABranchInfo();// $1AC
static int E3DSetMOABranchFrame1();// $1AD
static int E3DSetMOABranchFrame2();// $1AE

static int E3DSetDispSwitch2();// $1AF

static int E3DQtoEuler();// $1B0
static int E3DEnablePhongEdge();// $1B1
static int E3DSetPhongEdge0Params();// $1B2

static int E3DGetDispSwitch2();// $1B3
static int E3DFreeThread();// $1B4
static int E3DLoadSigFileAsGroundThread();// $1B5
static int E3DLoadSigFileAsGroundFromBufThread();// $1B6
static int E3DSetLightIdOfBumpMap();// $1B7
static int E3DSetSpriteUV();// $1B8

static int E3DCreateRenderTargetTexture();// $1B9
static int E3DDestroyRenderTargetTexture();// $1BA
static int E3DSetDSFillUpMode();// $1BB
static int E3DSetTexFillUpMode();// $1BC

static int E3DSetShadowMapCamera();// $1BD
static int E3DSetShadowMapProjOrtho();// $1BE
static int E3DRenderWithShadow();// $1BF
static int E3DSetShadowBias();// $1C0
static int E3DCheckRTFormat();// $1C1
static int E3DChkShadowEnable();// $1C2
static int E3DEnableShadow();// $1C3

static int E3DRenderBatch();// $1C4
static int E3DSetVertPosBatch();// $1C5

static int E3DSetShadowDarkness();// $1C6
static int E3DSetShadowMapLightDir();// $1C7
static int E3DRenderBatchMode();// $1C8
static int E3DGlobalToLocal();// $1C9
static int E3DLocalToGlobal();// $1CA
static int E3DSetVertPosBatchAOrder();// $1CB
static int E3DSetShadowMapMode();// $1CC

static int E3DLoadTextureFromBuf();// $1CD
static int E3DLoadSpriteFromBMSCR();// $1CE

static int E3DSetShadowMapInterFlag();// $1CF
static int E3DTransformBillboard();// $1D0
static int E3DSetMaterialAlphaTest();// $1D1
static int E3DCalcMLMotion();// $1D2

static int E3DCreateSkinMat();// $1D3
static int E3DSetSymInfElem();// $1D4
static int E3DUpdateSymInfElem();// $1D5
static int E3DGetVersion();// $1D6
static int E3DSetJointInitialPos();// $1D7
static int E3DSetLightId2OfBumpMap();// $1D8
static int E3DSetLightId3OfBumpMap();// $1D9
static int E3DGetCharaDir();// $1DA
static int E3DGetStrSizeByFontID();// $1DB
static int E3DSetUVBatch();// $1DC
static int E3DSetPresentInterval();// $1DD
static int E3DGetScreenPosDouble();// $1DE
static int E3DSetBillboardSize();// $1DF
static int E3DSetHeightFogParams();// $1E0
static int E3DSetTextureToBG();// $1E1
static int E3DGetTextureFromBG();// $1E2
static int E3DChangeMoaIdling();// $1E3

static int E3DSetMotionBlur();// $1E4
static int E3DSetBeforeBlur();// $1E5
static int E3DRenderMotionBlur();// $1E6 
static int E3DInitBeforeBlur();// $1E7
static int E3DSetMotionBlurPart();// $1E8

static int E3DBlurTexture16Box();// $1E9
static int E3DBlurTexture9Corn();// $1EA
static int E3DBlurTextureGauss5x5();// $1EB
static int E3DBlurTextureGauss();// $1EC
static int E3DBlendRTTexture();// $1ED
static int E3DSetClearCol();// $1EE
static int E3DSetMotionBlurMinAlpha();// $1EF

static int E3DRenderGlow();// $1F0
static int E3DSetMaterialGlowParams();// $1F1
static int E3DGetMaterialTransparent();// $1F2
static int E3DSetMaterialTransparent();// $1F3
static int E3DGetMaterialGlowParams();// $1F4

static int E3DChkHDR();// $1F5
static int E3DSetHDRLightDiffuse();// $1F6
static int E3DSetHDRLightSpecular();// $1F7
static int E3DDownScaleRTTexture4x4();// $1F8
static int E3DDownScaleRTTexture2x2();// $1F9
static int E3DCreateToneMapTexture();// $1FA
static int E3DRenderToneMap();// $1FB
static int E3DBrightPassRTTextureHDR();// $1FC
static int E3DDestroyToneMapTexture();// $1FD
static int E3DRenderHDRFinal();// $1FE
static int E3DDisablePostEffect();// $1FF
static int E3DCheckSpec();// $200
static int E3DMonoRTTexture();// $201
static int E3DCbCrRTTexture();// $202

static int E3DShaderConstOffset16();// $203
static int E3DShaderConstWeight16();// $204
static int E3DShaderConstUserFL4();// $205
static int E3DShaderConstUserTex();// $206
static int E3DCallUserShader();// $207

static int E3DCreateBone();// $208
static int E3DDestroyBone();// $209

static int E3DGetMorphBaseNum();// $20A
static int E3DGetMorphBaseInfo();// $20B
static int E3DGetMorphTargetInfo();// $20C
static int E3DGetMorphKeyNum();// $20D
static int E3DGetMorphKey();// $20E
static int E3DGetCurMorphInfo();// $20F
static int E3DCreateMorph();// $210
static int E3DAddMorphTarget();// $211
static int E3DSetMorphKey();// $212
static int E3DDestroyMorphKey();// $213

static int E3DGetToon1MaterialNum();// $214
static int E3DGetToon1Material();// $215
static int E3DSetToon1Name();// $216
static int E3DSetToon1Diffuse();// $217
static int E3DSetToon1Specular();// $218
static int E3DSetToon1Ambient();// $219
static int E3DSetToon1NL();// $21A
static int E3DSetToon1Edge0();// $21B

static int E3DChkConfLineAndFacePart();// $21C

static int E3DLoadCameraFile(); //$21D
static int E3DDestroyCameraAnim(); //$21E
static int E3DSaveCameraFile(); //$21F
static int E3DSetCameraAnimNo(); //$220
static int E3DGetCameraAnimNo(); //$221
static int E3DSetCameraFrameNo(); //$222
static int E3DGetCameraFrameNo(); //$223
static int E3DSetNewCamera(); //$224
static int E3DSetNextCameraAnimFrameNo(); //$225
static int E3DGetNextCameraAnimFrameNo(); //$226
static int E3DCreateNewCameraAnim(); //$227
static int E3DSetCameraFrameNum(); //$228
static int E3DGetCameraFrameNum(); //$229
static int E3DSetCameraAnimName(); //$22A
static int E3DGetCameraAnimName(); //$22B
static int E3DSetCameraAnimType(); //$22C
static int E3DGetCameraAnimType(); //$22D
static int E3DSetCameraKey(); //$22E
static int E3DGetCameraKey(); //$22F
static int E3DDeleteCameraKey(); //$230
static int E3DGetCameraKeyframeNoRange(); //$231
static int E3DSetCameraSWKey(); //$232
static int E3DGetCameraSWKey(); //$233
static int E3DDeleteCameraSWKey(); //$234
static int E3DGetCameraSWKeyframeNoRange(); //$235

static int E3DSetOrtho(); //$236

static int E3DLoadGPFile(); //$237
static int E3DDestroyGPAnim(); //$238
static int E3DSaveGPFile(); //$239
static int E3DSetGPAnimNo(); //$23A
static int E3DGetGPAnimNo(); //$23B
static int E3DSetGPFrameNo(); //$23C
static int E3DGetGPFrameNo(); //$23D
static int E3DSetNewGP(); //$23E
static int E3DSetNextGPAnimFrameNo(); //$23F
static int E3DGetNextGPAnimFrameNo(); //$240
static int E3DCreateNewGPAnim(); //$241
static int E3DSetGPFrameNum(); //$242
static int E3DGetGPFrameNum(); //$243
static int E3DSetGPAnimName(); //$244
static int E3DGetGPAnimName(); //$245
static int E3DSetGPAnimType(); //$246
static int E3DGetGPAnimType(); //$247
static int E3DSetGPKey(); //$248
static int E3DGetGPKey(); //$249
static int E3DDeleteGPKey(); //$24A
static int E3DGetGPKeyframeNoRange(); //$24B
static int E3DCalcGPFullFrame(); //$24C
static int E3DSetGPGroundHSID(); //$24D
static int E3DGetGPGroundHSID(); //$24E

static int E3DLoadMOEFile(); //$24F
static int E3DDestroyMOEAnim(); //$250
static int E3DSaveMOEFile(); //$251
static int E3DSetMOEAnimNo(); //$252
static int E3DGetMOEAnimNo(); //$253
static int E3DSetMOEFrameNo(); //$254
static int E3DGetMOEFrameNo(); //$255
static int E3DSetNewMOE(); //$256
static int E3DSetNextMOEAnimFrameNo(); //$257
static int E3DGetNextMOEAnimFrameNo(); //$258
static int E3DCreateNewMOEAnim(); //$259
static int E3DSetMOEFrameNum(); //$25A
static int E3DGetMOEFrameNum(); //$25B
static int E3DSetMOEAnimName(); //$25C
static int E3DGetMOEAnimName(); //$25D
static int E3DSetMOEAnimType(); //$25E
static int E3DGetMOEAnimType(); //$25F
static int E3DGetMOEKeyframeNoRange(); //$260
static int E3DGetMOEElemNum(); //$261
static int E3DGetMOEElem(); //$262
static int E3DAddMOEElem(); //$263
static int E3DDeleteMOEElem(); //$264
static int E3DSetMOEKeyIdling(); //$265
static int E3DGetMOEKeyIdling(); //$266
static int E3DDeleteMOEKey(); //$267
static int E3DCalcMOEFullFrame(); //$268

static int E3DUpdateSound();// $269
static int E3DGetSoundSamplesPlayed();// $26A
static int E3DSet3DSoundEmiDist();// $26B

 /*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

/***
static void newcmd2( void )
{
	//		newcmd2 ���߂̏���
	//
	//		(newcmd2 p1,p2)
	//		p1�Ŏw�肵���ϐ���0�`(p2-1)�܂ł̗�������
	//		HSP 2.61�܂ł�rnd���߂Ɠ�������
	//
	PVal *pval;
	APTR aptr;
	aptr = code_getva( &pval );						// �ϐ��̎擾(PVal��APTR�|�C���^)
	p1=code_getdi(100);								// �����l���擾(�f�t�H���g100)
	if (p1==0) puterror( HSPERR_DIVIDED_BY_ZERO );	// 0�Ȃ�΃G���[
	p2 = rand()%p1;									// �����𔭐�������
	code_setva( pval, aptr, TYPE_INUM, &p2 );		// �ϐ��ɒl����(�����l)
}
static void newcmd3( void )
{
	//		newcmd3 ���߂̏���
	//
	//		(newcmd3 "�t�@�C����",p1)
	//		"�t�@�C����"�Ŏw�肵���t�@�C����p1�̓��e�������o��
	//		p1�Ɏw�肳���^�́A�����A�����A������̂����ꂩ�ō\��Ȃ�
	//
	char *p;
	char pathname[_MAX_PATH];
	char out[64];
	int chk;

	p = code_gets();								// ��������擾
	strncpy( pathname, p, _MAX_PATH-1 );			// �擾������������R�s�[

	chk = code_getprm();							// �p�����[�^�[���擾(�^�͖��Ȃ�)
	if ( chk<=PARAM_END ) {
		return;										// �p�����[�^�[�ȗ����̏���
	}

	int type = mpval->flag;							// �p�����[�^�[�̌^���擾
	switch( type ) {
	case TYPE_STRING:								// �p�����[�^�[�������񂾂�����
		{
		char *str = (char *)mpval->pt;
		sprintf( out,"%s\n",str );
		break;
		}
	case TYPE_DNUM:									// �p�����[�^�[��������������
		{
		double *ptr = (double *)mpval->pt;
		sprintf( out,"%f\n",*ptr );
		break;
		}
	case TYPE_INUM:									// �p�����[�^�[��������������
		{
		int *ptr = (int *)mpval->pt;
		sprintf( out,"%d\n",*ptr );
		break;
		}
	default:
		puterror( HSPERR_TYPE_MISMATCH );			// �T�|�[�g���Ă��Ȃ��^�Ȃ�΃G���[
	}

	//	������out���t�@�C���ɒǋL�ŏ����o��
	//
	FILE *fp;
	fp = fopen( pathname, "a" );
	fputs( out, fp );
	fclose( fp );
}


static void newcmd4( void )
{
	//		newcmd4 ���߂̏���
	//
	//		(newcmd4 x1,y1,x2,y2,color)
	//		(x1,y1)����(x2,y2)�܂�color�Ŏw�肵���F�̐�������
	//
	BMSCR *bm;

	p1 = code_getdi(0);		// �p�����[�^1:���l
	p2 = code_getdi(0);		// �p�����[�^2:���l
	p3 = code_getdi(0);		// �p�����[�^3:���l
	p4 = code_getdi(0);		// �p�����[�^4:���l
	p5 = code_getdi(0);		// �p�����[�^5:���l

	bm = (BMSCR *)getbmscr( active_window );	// ���݂�BMSCR���擾
	DrawLine( bm, p1, p2, p3, p4, p5 );
	bms_send( bm, 0, 0, bm->sx, bm->sy );		// �S��ʂ��X�V
}
static void newcmd10( void )
{
	char testarray[5][256];
	int cnt1, cnt2;
	for( cnt1 = 0; cnt1 < 5; cnt1++ ){
		for( cnt2 = 0; cnt2 < 255; cnt2++ ){
			testarray[cnt1][cnt2] = '4' - cnt1;
		}
		testarray[cnt1][255] = 0;
		strcpy( &(testarray[cnt1][252]), "��" );
	}


	APTR aptr1;
	PVal* pval1;

	aptr1 = code_getva( &pval1 );
	
	code_setva( pval1, aptr1, HSPVAR_FLAG_STR, (void*)testarray );
	code_setva( pval1, aptr1 + 1, HSPVAR_FLAG_STR, (void*)( &testarray[1][0] ) );
	code_setva( pval1, aptr1 + 2, HSPVAR_FLAG_STR, (void*)( &testarray[2][0] ) );
	code_setva( pval1, aptr1 + 3, HSPVAR_FLAG_STR, (void*)( &testarray[3][0] ) );
	code_setva( pval1, aptr1 + 4, HSPVAR_FLAG_STR, (void*)( &testarray[4][0] ) );

}
***/


/*------------------------------------------------------------*/

static int cmdfunc( int cmd )
{
	//		���s���� (���ߎ��s���ɌĂ΂�܂�)
	//
	code_next();							// ���̃R�[�h���擾(�ŏ��ɕK���K�v�ł�)

	int ret;
	switch( cmd ) {							// �T�u�R�}���h���Ƃ̕���

	case 0x00:
		ret = E3DInit();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x01:			
		ret = E3DBye();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x02:				
		ret = E3DSigLoad();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x03:		
		ret = E3DCreateSwapChain();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x04:
		ret = E3DDestroySwapChain();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x05:
		ret = E3DRender();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x06:
		ret = E3DPresent();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x07:
		ret = E3DBeginScene();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x08:
		ret = E3DEndScene();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x09:
		ret = E3DCreateBG();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0A:
		ret = E3DSetBGU();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0B:
		ret = E3DSetBGV();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0C:
		ret = E3DDestroyBG();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0D:
		ret = E3DAddMotion();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0E:
		ret = E3DSetMotionKind();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x0F:
		ret = E3DGetMotionKind();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10:
		ret = E3DSetNewPose();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11:
		ret = E3DSetMotionStep();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12:
		ret = E3DChkConflict();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13:
		ret = E3DChkConflict2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14:
		ret = E3DCreateAfterImage();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15:
		ret = E3DDestroyAfterImage();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16:
		ret = E3DSetAlpha();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17:
		ret = E3DSetAlpha2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18:
		ret = E3DSetPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19:
		ret = E3DGetPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A:
		ret = E3DSetDir();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B:
		ret = E3DRotateInit();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C:
		ret = E3DRotateX();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D:
		ret = E3DRotateY();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E:
		ret = E3DRotateZ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F:
		ret = E3DTwist();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20:
		ret = E3DPosForward();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21:
		ret = E3DCloseTo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22:
		ret = E3DDirToTheOtherXZ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23:
		ret = E3DDirToTheOther();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24:
		ret = E3DSeparateFrom();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25:
		ret = E3DGetCameraPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x26:
		ret = E3DSetCameraPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x27:
		ret = E3DGetCameraDeg();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x28:
		ret = E3DSetCameraDeg();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x29:
		ret = E3DSetCameraTarget();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2A:
		ret = E3DChkInView();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2B:
		ret = E3DEnableDbgFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2C:
		ret = E3DSetProjection();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2D:
		ret = E3DGetKeyboardState();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2E:
		ret = E3DCos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x2F:
		ret = E3DSin();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x30:
		ret = E3DACos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x31:
		ret = E3DDot();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x32:
		ret = E3DCross();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x33:
		ret = E3DAtan();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x34:
		ret = E3DAtan2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x35:
		ret = E3DSqrt();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x36:
		ret = E3DDrawText();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x37:
		ret = E3DDrawBigText();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x38:
		ret = E3DRand();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x39:
		ret = E3DSeed();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3A:
		ret = E3DGetFPS();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3B:
		ret = E3DWaitbyFPS();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3C:
		ret = E3DCreateLight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3D:
		ret = E3DSetDirectionalLight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3E:
		ret = E3DSetPointLight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x3F:
		ret = E3DSetSpotLight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x40:
		ret = E3DDestroyLight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x41:
		ret = E3DClearZ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x42:
		ret = E3DDestroyHandlerSet();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x43:
		ret = E3DSetDispSwitch();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x44:
		ret = E3DSetMotionFrameNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x45:
		ret = E3DCreateSprite();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x46:
		ret = E3DBeginSprite();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x47:
		ret = E3DEndSprite();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x48:
		ret = E3DRenderSprite();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x49:
		ret = E3DGetSpriteSize();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4A:
		ret = E3DSetSpriteRotation();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4B:
		ret = E3DSetSpriteARGB();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4C:
		ret = E3DDestroySprite();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4D:
		ret = E3DChkConfBillboard2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4E:
		ret = E3DChkConfBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x4F:
		ret = E3DChkConfBySphere();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x50:
		ret = E3DChkConfBySphere2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x51:
		ret = E3DCreateProgressBar();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x52:
		ret = E3DSetProgressBar();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x53:
		ret = E3DDestroyProgressBar();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x54:
		ret = E3DLoadGroundBMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x55:
		ret = E3DCameraPosForward();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x56:
		ret = E3DSetBeforePos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x57:
		ret = E3DChkConfGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x58:
		ret = E3DChkConfGround2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x59:
		ret = E3DGetPartNoByName();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5A:
		ret = E3DGetVertNumOfPart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5B:
		ret = E3DGetVertPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5C:
		ret = E3DSetVertPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5D:
		ret = E3DSetPosOnGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5E:
		ret = E3DSetPosOnGroundPart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x5F:
		ret = E3DCreateBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x60:
		ret = E3DRenderBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x61:
		ret = E3DSetBillboardPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x62:
		ret = E3DSetBillboardOnGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x63:
		ret = E3DDestroyBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x64:
		ret = E3DLoadMQOFileAsGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x65:
		ret = E3DSaveMQOFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x66:
		ret = E3DGetBillboardInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x67:
		ret = E3DGetNearBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x68:
		ret = E3DGetInvisibleFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x69:
		ret = E3DSetInvisibleFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6A:
		ret = E3DSetMovableArea();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6B:
		ret = E3DChkConfWall();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6C:
		ret = E3DChkConfWall2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6D:
		ret = E3DVec3Normalize();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6E:
		ret = E3DVec2CCW();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x6F:
		ret = E3DVec3RotateY();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x70:
		ret = E3DLoadMQOFileAsMovableArea();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x71:
		ret = E3DLoadSound();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x72:
		ret = E3DPlaySound();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x73:
		ret = E3DStopSound();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x74:
		ret = E3DSetSoundLoop();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x75:
		ret = E3DSetSoundVolume();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x76:
		ret = E3DSetSoundTempo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x77:
		ret = E3DDestroySound();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x78:
		ret = E3DSetSoundFrequency();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x79:
		ret = E3DGetSoundVolume();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7A:
		ret = E3DGetSoundFrequency();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7B:
		ret = E3DSet3DSoundListener();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7C:
		ret = E3DSet3DSoundListenerMovement();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7D:
		ret = E3DSet3DSoundDistance();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7E:
		ret = E3DSet3DSoundMovement();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x7F:
		ret = E3DCreateNaviLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x80:
		ret = E3DDestroyNaviLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x81:
		ret = E3DAddNaviPoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x82:
		ret = E3DRemoveNaviPoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x83:
		ret = E3DGetNaviPointPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x84:
		ret = E3DSetNaviPointPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x85:
		ret = E3DGetNaviPointOwnerID();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x86:
		ret = E3DSetNaviPointOwnerID();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x87:
		ret = E3DGetNextNaviPoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x88:
		ret = E3DGetPrevNaviPoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x89:
		ret = E3DGetNearestNaviPoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8A:
		ret = E3DFillUpNaviLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8B:
		ret = E3DSetNaviLineOnGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8C:
		ret = E3DControlByNaviLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8D:
		ret = E3DSetDirQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8E:
		ret = E3DGetDirQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x8F:
		ret = E3DSetWallOnGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x90:
		ret = E3DCreateNaviPointClearFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x91:
		ret = E3DDestroyNaviPointClearFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x92:
		ret = E3DInitNaviPointClearFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x93:
		ret = E3DSetNaviPointClearFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x94:
		ret = E3DGetOrder();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x95:
		ret = E3DDestroyAllBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x96:
		ret = E3DSetValidFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x97:
		ret = E3DSetDiffuse();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x98:
		ret = E3DSetSpecular();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x99:
		ret = E3DSetAmbient();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9A:
		ret = E3DSetBlendingMode();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9B:
		ret = E3DSetRenderState();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9C:
		ret = E3DSetScale();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9D:
		ret = E3DGetScreenPos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9E:
		ret = E3DGetScreenPos2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x9F:
		ret = E3DCreateQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA0:
		ret = E3DDestroyQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA1:
		ret = E3DInitQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA2:
		ret = E3DSetQAxisAndDeg();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA3:
		ret = E3DGetQAxisAndDeg();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA4:
		ret = E3DRotateQX();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA5:
		ret = E3DRotateQY();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA6:
		ret = E3DRotateQZ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA7:
		ret = E3DMultQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA8:
		ret = E3DNormalizeQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xA9:
		ret = E3DCopyQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAA:
		ret = E3DGetBoneNoByName();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAB:
		ret = E3DGetNextMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAC:
		ret = E3DSetMPInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAD:
		ret = E3DIsExistMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAE:
		ret = E3DGetMotionFrameLength();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xAF:
		ret = E3DSetMotionFrameLength();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB0:
		ret = E3DAddMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB1:
		ret = E3DDeleteMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB2:
		ret = E3DFillUpMotion();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB3:
		ret = E3DCopyMotionFrame();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB4:
		ret = E3DGetMPInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB5:
		ret = E3DGetDirQ2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB6:
		ret = E3DSetDirQ2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB7:
		ret = E3DLookAtQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB8:
		ret = E3DMultQVec();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xB9:
		ret = E3DTwistQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBA:
		ret = E3DInitTwistQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBB:
		ret = E3DGetTwistQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBC:
		ret = E3DRotateQLocalX();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBD:
		ret = E3DRotateQLocalY();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBE:
		ret = E3DRotateQLocalZ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xBF:
		ret = E3DGetBonePos();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC0:
		ret = E3DCreateLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC1:
		ret = E3DDestroyLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC2:
		ret = E3DSetLineColor();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC3:
		ret = E3DAddPoint2Line();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC4:
		ret = E3DDeletePointOfLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC5:
		ret = E3DSetPointPosOfLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC6:
		ret = E3DGetPointPosOfLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC7:
		ret = E3DGetNextPointOfLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC8:
		ret = E3DGetPrevPointOfLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xC9:
		ret = E3DWriteDisplay2BMP();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCA:
		ret = E3DCreateAVIFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCB:
		ret = E3DWriteData2AVIFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCC:
		ret = E3DCompleteAVIFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCD:
		ret = E3DCameraLookAt();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCE:
		ret = E3DCameraOnNaviLine();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xCF:
		ret = E3DCameraDirUp();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD0:
		ret = E3DCameraDirDown();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD1:
		ret = E3DCameraDirRight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD2:
		ret = E3DCameraDirLeft();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD3:
		ret = E3DCreateFont();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD4:
		ret = E3DDrawTextByFontID();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD5:
		ret = E3DDrawTextByFontIDWithCnt();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD6:
		ret = E3DDrawTextByFontIDWithCnt2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD7:
		ret = E3DGetCharacterNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD8:
		ret = E3DDestroyFont();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xD9:
		ret = E3DSlerpQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDA:
		ret = E3DSquadQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDB:
		ret = E3DSplineVec();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDC:
		ret = E3DDbgOut();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDD:
		ret = E3DGetDiffuse();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDE:
		ret = E3DGetAmbient();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xDF:
		ret = E3DGetSpecular();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE0:
		ret = E3DGetAlpha();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE1:
		ret = E3DSaveQuaFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE2:
		ret = E3DSaveSigFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE3:
		ret = E3DSetMipMapParams();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE4:
		ret = E3DPickVert();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE5:
		ret = E3DGetCullingFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE6:
		ret = E3DGetOrgVertNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE7:
		ret = E3DChkIM2Status();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE8:
		ret = E3DLoadIM2File();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xE9:
		ret = E3DSaveIM2File();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xEA:
		ret = E3DGetJointNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xEB:
		ret = E3DGetJointInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xEC:
		ret = E3DGetFirstJointNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xED:
		ret = E3DGetDispObjNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xEE:
		ret = E3DGetDispObjInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xEF:
		ret = E3DEnableTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF0:
		ret = E3DJointAddToTree();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF1:
		ret = E3DJointRemoveFromParent();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF2:
		ret = E3DJointRemake();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF3:
		ret = E3DSigImport();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF4:
		ret = E3DSigLoadFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF5:
		ret = E3DSigImportFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF6:
		ret = E3DAddMotionFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF7:
		ret = E3DCheckFullScreenParams();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF8:
		ret = E3DGetMaxMultiSampleNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xF9:
		ret = E3DChkConfLineAndFace();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFA:
		ret = E3DPickFace();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFB:
		ret = E3DGetBBox();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFC:
		ret = E3DGetVertNoOfFace();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFD:
		ret = E3DGetSamePosVert();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFE:
		ret = E3DRdtscStart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0xFF:
		ret = E3DRdtscStop();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x100:
		ret = E3DSaveSig2Buf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x101:
		ret = E3DSaveQua2Buf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x102:
		ret = E3DCameraShiftLeft();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x103:
		ret = E3DCameraShiftRight();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x104:
		ret = E3DCameraShiftUp();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x105:
		ret = E3DCameraShiftDown();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x106:
		ret = E3DGetCameraQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x107:
		ret = E3DInvQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x108:
		ret = E3DSetCameraTwist();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x109:
		ret = E3DIKRotateBeta();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10A:
		ret = E3DIKRotateBeta2D();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10B:
		ret = E3DGetMotionType();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10C:
		ret = E3DSetMotionType();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10D:
		ret = E3DGetIKTransFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10E:
		ret = E3DSetIKTransFlag();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x10F:
		ret = E3DDestroyAllMotion();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x110:
		ret = E3DGetUserInt1OfPart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x111:
		ret = E3DSetUserInt1OfPart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x112:
		ret = E3DGetBSphere();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x113:
		ret = E3DGetChildJoint();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x114:
		ret = E3DDestroyMotionFrame();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x115:
		ret = E3DGetKeyFrameNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x116:
		ret = E3DConvScreenTo3D();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x117:
		ret = E3DVec3Length();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x118:
		ret = E3DSetUV();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x119:
		ret = E3DGetUV();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11A:
		ret = E3DPickBone();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11B:
		ret = E3DShiftBoneTree2D();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11C:
		ret = E3DGetDispSwitch();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11D:
		ret = E3DRotateBillboard();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11E:
		ret = E3DSetBillboardUV();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x11F:
		ret = E3DCreateTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x120:
		ret = E3DGetTextureInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x121:
		ret = E3DCopyTextureToTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x122:
		ret = E3DGetTextureFromDispObj();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x123:
		ret = E3DSetTextureToDispObj();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x124:
		ret = E3DRasterScrollTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x125:
		ret = E3DCopyTextureToBackBuffer();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x126:
		ret = E3DDestroyTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x127:
		ret = E3DSetLightAmbient();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x128:
		ret = E3DSetLightSpecular();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x129:
		ret = E3DInvColTexture();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12A:
		ret = E3DSaveGndFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12B:
		ret = E3DLoadGndFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12C:
		ret = E3DLoadGndFileFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12D:
		ret = E3DCreateTextureFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12E:
		ret = E3DLoadSoundFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x12F:
		ret = E3DTwistBone();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x130:
		ret = E3DSetStartPointOfSound();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x131:
		ret = E3DGetBoneQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x132:
		ret = E3DSetBoneQ();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x133:
		ret = E3DIsSoundPlaying();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x134:
		ret = E3DIKTranslate();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x135:
		ret = E3DSetUVTile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x136:
		ret = E3DImportMQOFileAsGround();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x137:
		ret = E3DLoadMQOFileAsMovableAreaFromBuf();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x138:
		ret = E3DChkThreadWorking();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x139:
		ret = E3DLoadMQOFileAsGroundThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13A:
		ret = E3DSigLoadThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13B:
		ret = E3DSigLoadFromBufThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13C:
		ret = E3DLoadMQOFileAsMovableAreaThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13D:
		ret = E3DLoadMQOFileAsMovableAreaFromBufThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13E:
		ret = E3DLoadGroundBMPThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x13F:
		ret = E3DLoadGndFileThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x140:
		ret = E3DLoadGndFileFromBufThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x141:
		ret = E3DAddMotionThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x142:
		ret = E3DAddMotionFromBufThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x143:
		ret = E3DGetShaderType();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x144:
		ret = E3DSetShaderType();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x145:
		ret = E3DSetLightBlendMode();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x146:
		ret = E3DGetLightBlendMode();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x147:
		ret = E3DSetEmissive();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x148:
		ret = E3DSetSpecularPower();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x149:
		ret = E3DGetEmissive();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14A:
		ret = E3DGetSpecularPower();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14B:
		ret = E3DGetInfElemNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14C:
		ret = E3DGetInfElem();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14D:
		ret = E3DSetInfElem();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14E:
		ret = E3DDeleteInfElem();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x14F:
		ret = E3DSetInfElemDefault();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x150:
		ret = E3DNormalizeInfElem();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x151:
		ret = E3DGetVisiblePolygonNum();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x152:
		ret = E3DChkConfGroundPart();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x153:
		ret = E3DChkConfGroundPart2();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x154:
		ret = E3DGetMidiMusicTime();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x155:
		ret = E3DSetNextMotionFrameNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x156:
		ret = E3DSetLinearFogParams();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x157:
		ret = E3DSetToon0Params();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x158:
		ret = E3DChkConflict3();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x159:
		ret = E3DChkConfBySphere3();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15A:
		ret = E3DSetMovableAreaThread();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15B:
		ret = E3DCreateSpriteFromBMSCR();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x15C:
		ret = E3DEncodeBeta();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15D:
		ret = E3DDecodeBeta();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15E:
		ret = E3DChkVertexShader();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x15F:
		ret = E3DGetReferenceTime();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x160:
		ret = E3DLoadMOAFile();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x161:
		ret = E3DSetNewPoseByMOA();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x162:
		ret = E3DGetKeyboardCnt();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x163:
		ret = E3DResetKeyboardCnt();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x164:
		ret = E3DGetMotionFrameNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x165:
		ret = E3DGetMoaInfo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x166:
		ret = E3DGetNextMotionFrameNo();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x167:
		ret = E3DGetScreenPos3();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x168:
		ret = E3DEnableToonEdge();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x169:
		ret = E3DSetToonEdge0Color();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16A:
		ret = E3DSetToonEdge0Width();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16B:
		ret = E3DGetToonEdge0Color();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16C:
		ret = E3DGetToonEdge0Width();
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	case 0x16D:
		ret = E3DCreateParticle();//$16D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16E:
		ret = E3DSetParticlePos();//$16E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x16F:
		ret = E3DSetParticleGravity();//$16F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x170:
		ret = E3DSetParticleLife();//$170
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x171:
		ret = E3DSetParticleEmitNum();//$171
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x172:
		ret = E3DSetParticleVel0();//$172
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x173:
		ret = E3DRenderParticle();//$173
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x174:
		ret = E3DSetParticleRotation();//$174
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x175:
		ret = E3DSetParticleDiffuse();//$175
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x176:
		ret = E3DDestroyParticle();//$176
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x177:
		ret = E3DSetParticleAlpha();//$177
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x178:
		ret = E3DSetParticleUVTile();//$178
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x179:
		ret = E3DInitParticle();//$179
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17A:
		ret = E3DClone();//$17A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17B:	
		ret = E3DPickFace2();//$17B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17C:
		ret = E3DChkConfWall3();//$17C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17D:
		ret = E3DGetMotionIDByName();//$17D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17E:
		ret = E3DDot2();//$17E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x17F:
		ret = E3DGetMotionNum();//$17F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x180:
		ret = E3DChkConfParticle();//$180
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x181:
		ret = E3DLoadSigFileAsGround();//$181
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x182:
		ret = E3DLoadSigFileAsGroundFromBuf();//$182
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x183:
		ret = E3DGetCenterPos();//$183
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x184:
		ret = E3DGetFaceNum();//$184
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x185:
		ret = E3DGetFaceNormal();//$185
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x186:
		ret = E3DCreateEmptyMotion();//$186
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x187:
		ret = E3DSetTextureMinMagFilter();//$187
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x188:
		ret = E3DGetMotionName();//$188
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x189:
		ret = E3DSetMotionName();//$189
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18A:
		ret = E3DGetMaterialNoByName();//$18A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18B:
		ret = E3DGetMaterialAlpha();//$18B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18C:
		ret = E3DGetMaterialDiffuse();//$18C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18D:
		ret = E3DGetMaterialSpecular();//$18D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18E:
		ret = E3DGetMaterialAmbient();//$18E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x18F:
		ret = E3DGetMaterialEmissive();//$18F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x190:
		ret = E3DSetMaterialAlpha();//$190
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x191:
		ret = E3DSetMaterialDiffuse();//$191
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x192:
		ret = E3DSetMaterialSpecular();//$192
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x193:
		ret = E3DSetMaterialAmbient();//$193
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x194:
		ret = E3DSetMaterialEmissive();//$194
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x195:
		ret = E3DGetTextureFromMaterial();//$195
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x196:
		ret = E3DSetTextureToMaterial();//$196
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x197:
		ret = E3DGetMaterialPower();//$197
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x198:
		ret = E3DSetMaterialPower();//$198
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x199:
		ret = E3DGetMaterialBlendingMode();//$199
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19A:
		ret = E3DSetMaterialBlendingMode();//$19A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19B:
		ret = E3DGetMaterialNo();//$19B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19C:
		ret = E3DSetMotionKindML();//$19C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19D:
		ret = E3DSetMotionFrameNoML();//$19D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19E:
		ret = E3DSetNewPoseML();//$19E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x19F:
		ret = E3DSetMOAEventNoML();//$19F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A0:
		ret = E3DSetNextMotionFrameNoML();//$1A0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A1:
		ret = E3DGetMotionFrameNoML();//$1A1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A2:
		ret = E3DGetNextMotionFrameNoML();//$1A2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A3:
		ret = E3DGetCurrentBonePos();//$1A3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A4:
		ret = E3DGetCurrentBoneQ();//$1A4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A5:
		ret = E3DChkBumpMapEnable();//$1A5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A6:
		ret = E3DEnableBumpMap();//$1A6
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A7:
		ret = E3DConvHeight2NormalMap();//$1A7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1A8:
		ret = E3DSetBumpMapToMaterial();//$1A8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
		
	case 0x1A9:
		ret = E3DDumpQ();//$1A9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AA:
		ret = E3DSetDbgQ();//$1AA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AB:
		ret = E3DGetMOATrunkInfo();// $1AB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AC:
		ret = E3DGetMOABranchInfo();// $1AC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AD:
		ret = E3DSetMOABranchFrame1();// $1AD
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AE:
		ret = E3DSetMOABranchFrame2();// $1AE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1AF:
		ret = E3DSetDispSwitch2();// $1AF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B0:
		ret = E3DQtoEuler();// $1B0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B1:
		ret = E3DEnablePhongEdge();// $1B1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B2:
		ret = E3DSetPhongEdge0Params();// $1B2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B3:
		ret = E3DGetDispSwitch2();// $1B3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B4:
		ret = E3DFreeThread();// $1B4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B5:
		ret = E3DLoadSigFileAsGroundThread();// $1B5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B6:
		ret = E3DLoadSigFileAsGroundFromBufThread();// $1B6
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B7:
		ret = E3DSetLightIdOfBumpMap();// $1B7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B8:
		ret = E3DSetSpriteUV();// $1B8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1B9:
		ret = E3DCreateRenderTargetTexture();// $1B9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BA:
		ret = E3DDestroyRenderTargetTexture();// $1BA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BB:
		ret = E3DSetDSFillUpMode();// $1BB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BC:
		ret = E3DSetTexFillUpMode();// $1BC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BD:
		ret = E3DSetShadowMapCamera();// $1BD
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BE:
		ret = E3DSetShadowMapProjOrtho();// $1BE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1BF:
		ret = E3DRenderWithShadow();// $1BF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C0:
		ret = E3DSetShadowBias();// $1C0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C1:
		ret = E3DCheckRTFormat();// $1C1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C2:
		ret = E3DChkShadowEnable();// $1C2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C3:
		ret = E3DEnableShadow();// $1C3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C4:
		ret = E3DRenderBatch();// $1C4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C5:
		ret = E3DSetVertPosBatch();// $1C5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C6:
		ret = E3DSetShadowDarkness();// $1C6
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C7:
		ret = E3DSetShadowMapLightDir();// $1C7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C8:
		ret = E3DRenderBatchMode();// $1C8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1C9:
		ret = E3DGlobalToLocal();// $1C9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CA:
		ret = E3DLocalToGlobal();// $1CA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CB:
		ret = E3DSetVertPosBatchAOrder();// $1CB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CC:
		ret = E3DSetShadowMapMode();// $1CC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CD:
		ret = E3DLoadTextureFromBuf();// $1CD
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CE:
		ret = E3DLoadSpriteFromBMSCR();// $1CE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1CF:
		ret = E3DSetShadowMapInterFlag();// $1CF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D0:
		ret = E3DTransformBillboard();// $1D0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D1:
		ret = E3DSetMaterialAlphaTest();// $1D1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D2:
		ret = E3DCalcMLMotion();// $1D2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D3:
		ret = E3DCreateSkinMat();// $1D3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D4:
		ret = E3DSetSymInfElem();// $1D4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D5:
		ret = E3DUpdateSymInfElem();// $1D5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D6:
		ret = E3DGetVersion();// $1D6
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D7:
		ret = E3DSetJointInitialPos();// $1D7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D8:
		ret = E3DSetLightId2OfBumpMap();// $1D8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1D9:
		ret = E3DSetLightId3OfBumpMap();// $1D9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DA:
		ret = E3DGetCharaDir();// $1DA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DB:
		ret = E3DGetStrSizeByFontID();// $1DB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DC:
		ret = E3DSetUVBatch();// $1DC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DD:
		ret = E3DSetPresentInterval();// $1DD
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DE:
		ret = E3DGetScreenPosDouble();// $1DE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1DF:
		ret = E3DSetBillboardSize();// $1DF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E0:
		ret = E3DSetHeightFogParams();// $1E0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E1:
		ret = E3DSetTextureToBG();// $1E1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E2:
		ret = E3DGetTextureFromBG();// $1E2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E3:
		ret = E3DChangeMoaIdling();// $1E3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E4:
		ret = E3DSetMotionBlur();// $1E4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E5:
		ret = E3DSetBeforeBlur();// $1E5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E6:
		ret = E3DRenderMotionBlur();// $1E6 
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E7:
		ret = E3DInitBeforeBlur();// $1E7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E8:
		ret = E3DSetMotionBlurPart();// $1E8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1E9:
		ret = E3DBlurTexture16Box();// $1E9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1EA:
		ret = E3DBlurTexture9Corn();// $1EA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1EB:
		ret = E3DBlurTextureGauss5x5();// $1EB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1EC:
		ret = E3DBlurTextureGauss();// $1EC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1ED:
		ret = E3DBlendRTTexture();// $1ED
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1EE:
		ret = E3DSetClearCol();// $1EE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1EF:
		ret = E3DSetMotionBlurMinAlpha();// $1EF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F0:
		ret = E3DRenderGlow();// $1F0
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F1:
		ret = E3DSetMaterialGlowParams();// $1F1
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F2:
		ret = E3DGetMaterialTransparent();// $1F2
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F3:
		ret = E3DSetMaterialTransparent();// $1F3
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F4:
		ret = E3DGetMaterialGlowParams();// $1F4
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F5:
		ret = E3DChkHDR();// $1F5
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F6:
		ret = E3DSetHDRLightDiffuse();// $1F6
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F7:
		ret = E3DSetHDRLightSpecular();// $1F7
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F8:
		ret = E3DDownScaleRTTexture4x4();// $1F8
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1F9:
		ret = E3DDownScaleRTTexture2x2();// $1F9
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FA:
		ret = E3DCreateToneMapTexture();// $1FA
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FB:
		ret = E3DRenderToneMap();// $1FB
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FC:
		ret = E3DBrightPassRTTextureHDR();// $1FC
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FD:
		ret = E3DDestroyToneMapTexture();// $1FD
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FE:
		ret = E3DRenderHDRFinal();// $1FE
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x1FF:
		ret = E3DDisablePostEffect();// $1FF
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x200:
		ret = E3DCheckSpec();// $200
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x201:
		ret = E3DMonoRTTexture();// $201
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x202:
		ret = E3DCbCrRTTexture();// $202
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x203:
		ret = E3DShaderConstOffset16();// $203
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x204:
		ret = E3DShaderConstWeight16();// $204
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x205:
		ret = E3DShaderConstUserFL4();// $205
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x206:
		ret = E3DShaderConstUserTex();// $206
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x207:
		ret = E3DCallUserShader();// $207
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x208:
		ret = E3DCreateBone();// $208
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x209:
		ret = E3DDestroyBone();// $209
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20A:
		ret = E3DGetMorphBaseNum();// $20A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20B:
		ret = E3DGetMorphBaseInfo();// $20B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20C:
		ret = E3DGetMorphTargetInfo();// $20C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20D:
		ret = E3DGetMorphKeyNum();// $20D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20E:
		ret = E3DGetMorphKey();// $20E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x20F:
		ret = E3DGetCurMorphInfo();// $20F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x210:
		ret = E3DCreateMorph();// $210
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x211:
		ret = E3DAddMorphTarget();// $211
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x212:
		ret = E3DSetMorphKey();// $212
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x213:
		ret = E3DDestroyMorphKey();// $213
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x214:
		ret = E3DGetToon1MaterialNum();// $214
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x215:
		ret = E3DGetToon1Material();// $215
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x216:
		ret = E3DSetToon1Name();// $216
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x217:
		ret = E3DSetToon1Diffuse();// $217
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x218:
		ret = E3DSetToon1Specular();// $218
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x219:
		ret = E3DSetToon1Ambient();// $219
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21A:
		ret = E3DSetToon1NL();// $21A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21B:
		ret = E3DSetToon1Edge0();// $21B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21C:
		ret = E3DChkConfLineAndFacePart();// $21C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21D:
		ret = E3DLoadCameraFile(); //$21D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21E:
		ret = E3DDestroyCameraAnim(); //$21E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x21F:
		ret = E3DSaveCameraFile(); //$21F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x220:
		ret = E3DSetCameraAnimNo(); //$220
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x221:
		ret = E3DGetCameraAnimNo(); //$221
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x222:
		ret = E3DSetCameraFrameNo(); //$222
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x223:
		ret = E3DGetCameraFrameNo(); //$223
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x224:
		ret = E3DSetNewCamera(); //$224
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x225:
		ret = E3DSetNextCameraAnimFrameNo(); //$225
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x226:
		ret = E3DGetNextCameraAnimFrameNo(); //$226
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x227:
		ret = E3DCreateNewCameraAnim(); //$227
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x228:
		ret = E3DSetCameraFrameNum(); //$228
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x229:
		ret = E3DGetCameraFrameNum(); //$229
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22A:
		ret = E3DSetCameraAnimName(); //$22A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22B:
		ret = E3DGetCameraAnimName(); //$22B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22C:
		ret = E3DSetCameraAnimType(); //$22C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22D:
		ret = E3DGetCameraAnimType(); //$22D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22E:
		ret = E3DSetCameraKey(); //$22E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x22F:
		ret = E3DGetCameraKey(); //$22F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x230:
		ret = E3DDeleteCameraKey(); //$230
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x231:
		ret = E3DGetCameraKeyframeNoRange(); //$231
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x232:
		ret = E3DSetCameraSWKey(); //$232
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x233:
		ret = E3DGetCameraSWKey(); //$233
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x234:
		ret = E3DDeleteCameraSWKey(); //$234
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x235:
		ret = E3DGetCameraSWKeyframeNoRange(); //$235
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x236:
		ret = E3DSetOrtho(); //$236
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x237:
		ret = E3DLoadGPFile(); //$237
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x238:
		ret = E3DDestroyGPAnim(); //$238
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x239:
		ret = E3DSaveGPFile(); //$239
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23A:
		ret = E3DSetGPAnimNo(); //$23A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23B:
		ret = E3DGetGPAnimNo(); //$23B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23C:
		ret = E3DSetGPFrameNo(); //$23C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23D:
		ret = E3DGetGPFrameNo(); //$23D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23E:
		ret = E3DSetNewGP(); //$23E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x23F:
		ret = E3DSetNextGPAnimFrameNo(); //$23F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x240:
		ret = E3DGetNextGPAnimFrameNo(); //$240
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x241:
		ret = E3DCreateNewGPAnim(); //$241
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x242:
		ret = E3DSetGPFrameNum(); //$242
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x243:
		ret = E3DGetGPFrameNum(); //$243
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x244:
		ret = E3DSetGPAnimName(); //$244
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x245:
		ret = E3DGetGPAnimName(); //$245
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x246:
		ret = E3DSetGPAnimType(); //$246
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x247:
		ret = E3DGetGPAnimType(); //$247
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x248:
		ret = E3DSetGPKey(); //$248
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x249:
		ret = E3DGetGPKey(); //$249
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24A:
		ret = E3DDeleteGPKey(); //$24A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24B:
		ret = E3DGetGPKeyframeNoRange(); //$24B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24C:
		ret = E3DCalcGPFullFrame(); //$24C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24D:
		ret = E3DSetGPGroundHSID(); //$24D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24E:
		ret = E3DGetGPGroundHSID(); //$24E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x24F:
		ret = E3DLoadMOEFile(); //$24F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x250:
		ret = E3DDestroyMOEAnim(); //$250
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x251:
		ret = E3DSaveMOEFile(); //$251
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x252:
		ret = E3DSetMOEAnimNo(); //$252
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x253:
		ret = E3DGetMOEAnimNo(); //$253
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x254:
		ret = E3DSetMOEFrameNo(); //$254
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x255:
		ret = E3DGetMOEFrameNo(); //$255
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x256:
		ret = E3DSetNewMOE(); //$256
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x257:
		ret = E3DSetNextMOEAnimFrameNo(); //$257
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x258:
		ret = E3DGetNextMOEAnimFrameNo(); //$258
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x259:
		ret = E3DCreateNewMOEAnim(); //$259
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25A:
		ret = E3DSetMOEFrameNum(); //$25A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25B:
		ret = E3DGetMOEFrameNum(); //$25B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25C:
		ret = E3DSetMOEAnimName(); //$25C
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25D:
		ret = E3DGetMOEAnimName(); //$25D
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25E:
		ret = E3DSetMOEAnimType(); //$25E
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x25F:
		ret = E3DGetMOEAnimType(); //$25F
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x260:
		ret = E3DGetMOEKeyframeNoRange(); //$260
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x261:
		ret = E3DGetMOEElemNum(); //$261
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x262:
		ret = E3DGetMOEElem(); //$262
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x263:
		ret = E3DAddMOEElem(); //$263
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x264:
		ret = E3DDeleteMOEElem(); //$264
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x265:
		ret = E3DSetMOEKeyIdling(); //$265
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x266:
		ret = E3DGetMOEKeyIdling(); //$266
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x267:
		ret = E3DDeleteMOEKey(); //$267
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x268:
		ret = E3DCalcMOEFullFrame(); //$268
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x269:
		ret = E3DUpdateSound();// $269
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x26A:
		ret = E3DGetSoundSamplesPlayed();// $26A
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;
	case 0x26B:
		ret = E3DSet3DSoundEmiDist();// $26B
		if( ret ){
			puterror( HSPERR_DLL_ERROR );
			return RUNMODE_STOP;
		}
		break;

	default:
		puterror( HSPERR_UNSUPPORTED_FUNCTION );
		break;
	}
	return RUNMODE_RUN;
}


/*------------------------------------------------------------*/

static int ref_ival;						// �Ԓl�̂��߂̕ϐ�

static void *reffunc( int *type_res, int cmd )
{
	//		�֐��E�V�X�e���ϐ��̎��s���� (�l�̎Q�Ǝ��ɌĂ΂�܂�)
	//
	//			'('�Ŏn�܂邩�𒲂ׂ�
	//
	if ( *type != TYPE_MARK ) puterror( HSPERR_INVALID_FUNCPARAM );
	if ( *val != '(' ) puterror( HSPERR_INVALID_FUNCPARAM );
	code_next();


	switch( cmd ) {							// �T�u�R�}���h���Ƃ̕���

	case 0x00:								// newcmd

		p1 = code_geti();				// �����l���擾(�f�t�H���g�Ȃ�)
		ref_ival = p1 * 2;				// �Ԓl��ival�ɐݒ�
		break;

	default:
		puterror( HSPERR_UNSUPPORTED_FUNCTION );
	}

	//			'('�ŏI��邩�𒲂ׂ�
	//
	if ( *type != TYPE_MARK ) puterror( HSPERR_INVALID_FUNCPARAM );
	if ( *val != ')' ) puterror( HSPERR_INVALID_FUNCPARAM );
	code_next();

	*type_res = HSPVAR_FLAG_INT;			// �Ԓl�̃^�C�v�𐮐��Ɏw�肷��
	return (void *)&ref_ival;
}


/*------------------------------------------------------------*/

static int termfunc( int option )
{
	//		�I������ (�A�v���P�[�V�����I�����ɌĂ΂�܂�)
	//
	E3DBye();

	return 0;
}

/*------------------------------------------------------------*/

static int eventfunc( int event, int prm1, int prm2, void *prm3 )
{
	//		�C�x���g���� (HSP�C�x���g�������ɌĂ΂�܂�)
	//
	switch( event ) {
	case HSPEVENT_GETKEY:
		{
		int *ival;
		ival = (int *)prm3;
		*ival = 123;
		return 1;
		}
	}
	return 0;
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	//		DLL�G���g���[ (��������K�v�͂���܂���)
	//
	return TRUE;
}


EXPORT void WINAPI hsp3cmdinit( HSP3TYPEINFO *info )
{
	//		�v���O�C�������� (���s�E�I��������o�^���܂�)
	//
	hsp3sdk_init( info );			// SDK�̏�����(�ŏ��ɍs�Ȃ��ĉ�����)
	info->cmdfunc = cmdfunc;		// ���s�֐�(cmdfunc)�̓o�^
	info->reffunc = reffunc;		// �Q�Ɗ֐�(reffunc)�̓o�^
	info->termfunc = termfunc;		// �I���֐�(termfunc)�̓o�^

//	s_ctx = info->hspctx;//!!!!!!!!!!!!!!!!! stat�i�[�p

	/*
	//	�C�x���g�R�[���o�b�N�𔭐�������C�x���g��ʂ�ݒ肷��
    info->option = HSPEVENT_ENABLE_GETKEY;
	info->eventfunc = eventfunc;	// �C�x���g�֐�(eventfunc)�̓o�^
	*/
}

//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////

int e3dInvalidateDeviceObject( int resetflag )
{
	int ret;

	
//handlerset
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		HSIDELEM* curhse = s_hsidelem + hsidno;
		if( curhse->validflag && curhse->hsptr ){
			ret = curhse->hsptr->Invalidate( resetflag );
			if( ret ){
				DbgOut( "e3dInvalidateDeviceObject : curhs Invalidate error!!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

//sprite
	if( s_d3dxsprite != NULL ){
		s_d3dxsprite->Release();
		s_d3dxsprite = NULL;
	}
	
	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}
	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->Invalidate( resetflag );
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : cursprite InvalidateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		cursprite = cursprite->next;
	}

//billboard
	if( g_bbhs ){
		ret = g_bbhs->Invalidate( resetflag );
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : bbhs Invalidate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//bgdisp restore�ŉ󂷁B


//font english
	//g_pD3DApp->InvalidateDeviceObjects();


//font fontid
	CXFont* curfont;
	curfont = xfonthead;
	while( curfont ){
		ret = curfont->Invalidate();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : curfont Invalidate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curfont = curfont->next;
	}

//swapchain
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			DestroySwapChainElem( &(s_sctable[scno]), 0 );
		}
	}

// texbank RenderTarget
	if( g_texbnk ){
		ret = g_texbnk->InvalidatePoolDefault();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : texbank InvalidatePoolDefault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



//effect
	if( g_pEffect ){
		HRESULT hres;
		hres = g_pEffect->OnLostDevice();
		_ASSERT( hres == D3D_OK );
	}
	if( g_pPostEffect ){
		HRESULT hres;
		hres = g_pPostEffect->OnLostDevice();
		_ASSERT( hres == D3D_OK );
	}

	return 0;
}


int e3dRestoreDeviceObject()
{
	int ret;
	HRESULT hr;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

//effect
	HRESULT hres;
	if( g_pEffect ){
		hres = g_pEffect->OnResetDevice();
		_ASSERT( hres == D3D_OK );
	}
	if( g_pPostEffect ){
		hres = g_pPostEffect->OnResetDevice();
		_ASSERT( hres == D3D_OK );
	}

	ret = SetEffectDecl( pdev );
	_ASSERT( !ret );

//texbank RenderTarget
	if( g_texbnk ){
		ret = g_texbnk->RestorePoolDefault( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : texbank RestorePoolDefault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//handlerset	
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
			ret = SetResDir( curhs->GetResDir(), curhs );
			_ASSERT( !ret );

			ret = Restore( curhs, -1 );
			if( ret ){
				DbgOut( "e3dRestoreDeviceObject error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

//sprite
	if( s_d3dxsprite == NULL ){
		hr = D3DXCreateSprite( pdev, &s_d3dxsprite );
		if( hr != D3D_OK ){
			DbgOut( "e3dRestoreDeviceObject : D3DXCreateSprite error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->RestoreTexture( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : cursprite RestoreTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		cursprite = cursprite->next;
	}

//billboard
	if( g_bbhs ){
		ret = SetResDir( g_bbhs->GetResDir(), g_bbhs );
		_ASSERT( !ret );
		
		ret = Restore( g_bbhs, -2 );// -2�̓r���{�[�h�S��
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : bbhs Restore error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//bgdisp  invalidate���s���B

	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		BGDISPELEM* curbgelem;
		curbgelem = &(s_bgdisp[scno]);

		if( (curbgelem->scid >= 0) && (curbgelem->bgdisp) ){
			CBgDisp2* newbg;
			newbg = new CBgDisp2( pdev, curbgelem->bgdisp->texname, curbgelem->bgdisp->transparent1, 
				curbgelem->bgdisp->texname2, curbgelem->bgdisp->transparent2,
				curbgelem->bgdisp->uanime,
				curbgelem->bgdisp->vanime, curbgelem->bgdisp->col, curbgelem->bgdisp->isround, curbgelem->bgdisp->m_fogdist,
				curbgelem->bgdisp->m_startuv, curbgelem->bgdisp->m_enduv, 0 );
			if( !newbg ){
				DbgOut( "e3dRestoreDeviceObject : newbg alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			delete curbgelem->bgdisp;
			curbgelem->bgdisp = newbg;
		}

	}

//font english
	//g_pD3DApp->RestoreDeviceObjects();


//font fontid

	CXFont* curfont;
	curfont = xfonthead;
	while( curfont ){
		ret = curfont->Restore( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : curfont Restore error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curfont = curfont->next;
	}

//renderstate
	SetDefaultRenderState();

	SetPartialRenderState( pdev );

	/***
	int i;
	int rsno;
	int curvalue;
	for( i = 0; i < 80; i++ ){
		rsno = g_rsindex[i];

		if( rsno >= 0 ){
			curvalue = g_renderstate[ rsno ];
			hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)rsno, curvalue );
			if( hr != D3D_OK ){
				DbgOut( "e3d : RestoreDeviceObject : SetRenderState : %d error !!!\r\n", rsno );
			}

		}
	}
	***/

//swapchain
	//int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			ret = CreateSwapChainElem( s_sctable[scno].scid, &(s_sctable[scno]), 
				s_sctable[scno].wid, s_sctable[scno].objid, s_sctable[scno].texid, &(s_sctable[scno].clearcol) );
			if( ret ){
				DbgOut( "RestoreDeviceObject : CreateSwapChainElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	s_sctable[0].pSwapChain = NULL;
	s_sctable[0].pZSurf = g_pD3DApp->m_pZSurf;
	s_sctable[0].pBackBuffer = g_pD3DApp->m_pBackBuffer;


	return 0;
}
int SetResDir0()
{
	char filename[1024];
	char* endptr = 0;

	ZeroMemory( filename, 1024 );
	int ret;

	ret = GetModuleFileName( NULL, filename, 1024 );
	if( ret == 0 ){
		_ASSERT( 0 );
		DbgOut( "SetResDir : GetModuleFileName error !!!\n" );
		return 1;
	}

	int ch = '\\';
	char* lasten = 0;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		DbgOut( "SetResDir : strrchr error !!!\n" );
		return 1;
	}

	*lasten = 0;

	char* last2en = 0;
	char* last3en = 0;
	last2en = strrchr( filename, ch );
	if( last2en ){
		*last2en = 0;
		last3en = strrchr( filename, ch );
		if( last3en ){
			if( (strcmp( last2en + 1, "Debug" ) == 0) ||
				(strcmp( last2en + 1, "DEBUG" ) == 0) ||
				(strcmp( last2en + 1, "Release" ) == 0) ||
				(strcmp( last2en + 1, "RELEASE" ) == 0)
				){

				endptr = last2en;
			}else{
				endptr = lasten;
			}
		}else{
			endptr = lasten;
		}
	}else{
		endptr = lasten;
	}

	*lasten = '\\';
	if( last2en )
		*last2en = '\\';
	if( last3en )
		*last3en = '\\';

	int leng;
	char resdir[1024];
	ZeroMemory( resdir, 1024 );
	leng = (int)( endptr - filename + 1 );
	if( leng >= 1024 )
		leng = 1023;
	strncpy( resdir, filename, leng );

	::SetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPCTSTR)resdir );

	DbgOut( "SetResDir : %s\n", resdir );

	return 0;
}

int SetResDir( char* pathname, CHandlerSet* hsptr )
{

	if( pathname ){
		char* lastmark;
		lastmark = strrchr( pathname, '\\' );
		if( !lastmark )
			return 1;

		int dirleng;
		char dirname[_MAX_PATH];
		dirleng = (int)(lastmark - pathname + 1);
		ZeroMemory( dirname, _MAX_PATH );
		strncpy( dirname, pathname, dirleng );

		::SetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPCTSTR)dirname );
		DbgOut( "e3d : SetResDir : RESDIR : %s\n", dirname );
	}else{
		SetResDir0();
	}

	if( hsptr && pathname ){
		//int namelen;
		//namelen = (int)strlen( pathname );
		//if( namelen < MAX_PATH ){
		//	strcpy( hsptr->m_resdir, pathname );
		//}else{
		//	DbgOut( "e3d : SetResDir : name too long warning !!! %s \n", pathname );
		//}
		int ret;
		ret = hsptr->SetResDir( pathname );
		if( ret ){
			DbgOut( "e3d : SetResDir : hs SetResDir error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int SetModuleDir()
{
	char filename[1024];
	ZeroMemory( filename, 1024 );
	int ret;

	ret = GetModuleFileName( NULL, filename, 1024 );
	if( ret == 0 ){
		_ASSERT( 0 );
		DbgOut( "SetModuleDir : GetModuleFileName error !!!\n" );
		return 1;
	}
	int ch = '\\';
	char* lasten;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		DbgOut( "SetModuleDir : strrchr error !!!\n" );
		return 1;
	}
	int leng;
	char moduledir[1024];
	ZeroMemory( moduledir, 1024 );
	leng = (int)( lasten - filename + 1 );
	if( leng >= 1024 )
		leng = 1023;
	strncpy( moduledir, filename, leng );

	::SetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPCTSTR)moduledir );
	
	DbgOut( "SetModuleDir : %s\n", moduledir );
	return 0;
}

int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev )
{

	HRESULT hr;
	int pno;
	int prs;
	for( pno = 0; pno < 28; pno++ ){
		prs = s_partialrs[ pno ];

		if( prs >= 0 ){
			hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)prs, g_renderstate[ prs ] );
			if( hr != D3D_OK ){
				DbgOut( "e3d : SetPartialRenderState : SetRenderState : %d error !!!\r\n", prs );
			}
		}
	}

	///////////////////

    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, g_minfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MINFILTER 0 : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, g_magfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MAGFILTER 0 : error !!!\r\n" );
	}

    hr = pdev->SetSamplerState( 1, D3DSAMP_MINFILTER, g_minfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MINFILTER 1 : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 1, D3DSAMP_MAGFILTER, g_magfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MAGFILTER 1 : error !!!\r\n" );
	}

	return 0;
}


int SetDefaultRenderState()
{
	ZeroMemory( g_renderstate, sizeof( int ) * (D3DRS_BLENDOP + 1) );


	float fogdensity = 1.0f;
	float pointsize = 1.0f;
	float pointscale = 1.0f;
	float pointscaleb = 0.0f;
	float pointscalec = 0.0f;
	float numsegments = 0.0f;
	float tweenfactor = 1.0f;

	g_renderstate[D3DRS_ZENABLE] = D3DZB_TRUE;
	g_renderstate[D3DRS_FILLMODE] =	D3DFILL_SOLID;
	g_renderstate[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	//g_renderstate[D3DRS_LINEPATTERN] = 0;
	g_renderstate[D3DRS_ZWRITEENABLE] = TRUE;
	g_renderstate[D3DRS_ALPHATESTENABLE] = TRUE;
	g_renderstate[D3DRS_LASTPIXEL] = TRUE;
	//g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ZERO ;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_SRCALPHA;
g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_INVSRCALPHA;
	//g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ONE;


	g_renderstate[D3DRS_CULLMODE] = D3DCULL_CCW;

	g_renderstate[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
	g_renderstate[D3DRS_ALPHAREF] = 0x08;
	g_renderstate[D3DRS_ALPHAFUNC] = D3DCMP_GREATEREQUAL;
	g_renderstate[D3DRS_DITHERENABLE] = FALSE;//<------------
	g_renderstate[D3DRS_ALPHABLENDENABLE] = FALSE;
	//g_renderstate[D3DRS_FOGENABLE] = FALSE;
g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
	g_renderstate[D3DRS_SPECULARENABLE] = TRUE;//<----------
	//g_renderstate[D3DRS_ZVISIBLE] = 0;
g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
	g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;


g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
	g_renderstate[D3DRS_FOGDENSITY ] = *((DWORD*)&fogdensity);
	//g_renderstate[D3DRS_EDGEANTIALIAS] = FALSE;
	//g_renderstate[D3DRS_ZBIAS] = 0;
	g_renderstate[D3DRS_RANGEFOGENABLE] = FALSE;
	g_renderstate[D3DRS_STENCILENABLE] = FALSE;
	g_renderstate[D3DRS_STENCILFAIL] = D3DSTENCILOP_KEEP;
	g_renderstate[D3DRS_STENCILZFAIL] = D3DSTENCILOP_KEEP;
	g_renderstate[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;

	g_renderstate[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
	g_renderstate[D3DRS_STENCILREF] = 0;
	g_renderstate[D3DRS_STENCILMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_STENCILWRITEMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_TEXTUREFACTOR] = 0xFFFFFFFF;//!!!
	g_renderstate[D3DRS_WRAP0] = 0;
	g_renderstate[D3DRS_WRAP1] = 0;
	g_renderstate[D3DRS_WRAP2] = 0;
	g_renderstate[D3DRS_WRAP3] = 0;
	g_renderstate[D3DRS_WRAP4] = 0;


	g_renderstate[D3DRS_WRAP5] = 0;
	g_renderstate[D3DRS_WRAP6] = 0;
	g_renderstate[D3DRS_WRAP7] = 0;
	g_renderstate[D3DRS_CLIPPING] = TRUE;
	//g_renderstate[D3DRS_LIGHTING] = FALSE;
	g_renderstate[D3DRS_LIGHTING] = TRUE;
	//g_renderstate[D3DRS_AMBIENT] = 0x40404040;
	//g_renderstate[D3DRS_AMBIENT] = D3DCOLOR_COLORVALUE( 0.25, 0.25, 0.25, 1.0 );
	g_renderstate[D3DRS_AMBIENT] = D3DCOLOR_COLORVALUE( 0.40, 0.40, 0.40, 1.0 );
g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
	g_renderstate[D3DRS_COLORVERTEX] = TRUE;
	g_renderstate[D3DRS_LOCALVIEWER] = TRUE;
	g_renderstate[D3DRS_NORMALIZENORMALS] = FALSE;

	g_renderstate[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
	g_renderstate[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
	g_renderstate[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
	g_renderstate[D3DRS_CLIPPLANEENABLE] = 0;
	//g_renderstate[D3DRS_SOFTWAREVERTEXPROCESSING] = FALSE;//!!!
	g_renderstate[D3DRS_POINTSIZE] = *((DWORD*)&pointsize);
	g_renderstate[D3DRS_POINTSIZE_MIN] = *((DWORD*)&pointsize);
	g_renderstate[D3DRS_POINTSPRITEENABLE] = FALSE;


	g_renderstate[D3DRS_POINTSCALEENABLE] = FALSE;
	g_renderstate[D3DRS_POINTSCALE_A] = *((DWORD*)&pointscale);
	g_renderstate[D3DRS_POINTSCALE_B] = *((DWORD*)&pointscaleb);
	g_renderstate[D3DRS_POINTSCALE_C] = *((DWORD*)&pointscalec);
	//g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = FALSE;//!!!	

	if( s_multisample > 0 ){
		g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = TRUE;
	}else{
		g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = FALSE;//!!!
	}
	
	g_renderstate[D3DRS_MULTISAMPLEMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_PATCHEDGESTYLE] = 0;
	//g_renderstate[D3DRS_PATCHSEGMENTS] = *((DWORD*)&numsegments);
	g_renderstate[D3DRS_DEBUGMONITORTOKEN] = D3DDMT_ENABLE;
	g_renderstate[D3DRS_POINTSIZE_MAX] = *((DWORD*)&pointsize);


	g_renderstate[D3DRS_INDEXEDVERTEXBLENDENABLE] = TRUE;//!!!
	g_renderstate[D3DRS_COLORWRITEENABLE] = 0x0000000F;
	g_renderstate[D3DRS_TWEENFACTOR] = *((DWORD*)&tweenfactor);
	g_renderstate[D3DRS_BLENDOP] = D3DBLENDOP_ADD;

////////

	g_minfilter = D3DTEXF_LINEAR;
	g_magfilter = D3DTEXF_LINEAR;

	return 0;

}
int Restore( CHandlerSet* hsptr, int billboardid )
{

	EnterCriticalSection( &g_crit_restore );//######## start crit

//	SetResDir( hsptr->m_resdir, hsptr );//!!!!!!!!!!!!!!!

	int ret;
	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

	/***
	ret = E3DRestore( m_girl1.handlerid, m_pd3dDevice, m_hWnd );
	if( ret ){
		_ASSERT( 0 );
		return -1;
	}

	if( m_girl1.afterimage ){
		ret = E3DCreateAfterImage( m_girl1.handlerid, m_pd3dDevice, m_girl1.afterimage );
		if( ret ){
			_ASSERT( 0 );
			return -1;
		}
	}
	***/
	

	_ASSERT( s_hwnd );

	RECT clirect;
	GetClientRect( s_hwnd, &clirect );
	s_clientWidth = clirect.right - clirect.left;
	s_clientHeight = clirect.bottom - clirect.top;

	ret = hsptr->Restore( pdev, s_hwnd, billboardid );
	if( ret ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}


	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ADDRESSV : error !!!\r\n" );
	}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	SetPartialRenderState( pdev );


/// light�̃Z�b�g

    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );


	CLightData* curlight = g_lighthead;
	while( curlight ){
		switch( curlight->Type ){
		case D3DLIGHT_DIRECTIONAL:

			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Direction = curlight->orgDirection;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

//DbgOut( "e3dhsp : Restore : directionallight : dir %f %f %f, rgb %f %f %f\n", 
//	   light.Direction.x, light.Direction.y, light.Direction.z,
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b );

			break;
		case D3DLIGHT_POINT:
			light.Type = D3DLIGHT_POINT;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

			light.Position = curlight->orgPosition;

			if( curlight->Range != 0.0f )
				light.Range = sqrtf( curlight->Range );

			light.Attenuation0 = 0.4f;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

//DbgOut( "e3dhsp : Restore : pointlight : col %f %f %f, pos %f %f %f, range %f\n",
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b,
//	   light.Position.x, light.Position.y, light.Position.z,
//	   light.Range );

			break;
		case D3DLIGHT_SPOT:
			light.Type = D3DLIGHT_SPOT;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;
			
			light.Position = curlight->orgPosition;

			light.Direction = curlight->orgDirection;

			light.Range = sqrtf( curlight->Range );

			light.Falloff = 1.0f;
			light.Attenuation0 = 1.0f;
    
			light.Theta        = curlight->Phi;
			light.Phi          = curlight->Phi;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

			
			break;
		default:
			DbgOut( "e3dhsp : Restore : invalid light type warning %d\n", curlight->Type );

			pdev->LightEnable( curlight->lightid, FALSE );

			break;
		}

		curlight = curlight->nextlight;
	}


	LeaveCriticalSection( &g_crit_restore );//###### end crit

    
	return 0;
}

int DestroySoundObject()
{
	if( s_HS ){
		delete s_HS;
		s_HS = 0;
	}

	return 0;
}
int SetFovIndex()
{
	int i;
	float minrate = 1.0f;
	float maxrate = 1.5f;

	float ratestep;
	ratestep = ( maxrate - minrate ) / (float)FOVINDEXSIZE;

	for( i = 0; i <= FOVINDEXSIZE; i++ ){
		float currate = minrate + ratestep * (float)i;
		g_fovindex[ i ] = (float)g_proj_fov * (float)DEG2PAI * currate;

		//DbgOut( "SetFovIndex %d : currate %f\n", i, currate );
	}

	return 0;
}

///////////////////////////////
///////////////////////////////
///////////////////////////////


int E3DInit()
{
	//type 2
	
	s_callinitflag = 1;

	//DbgOut( "e3dinit : WM_ACTIVATEAPP %d, WM_SYSCOMMAND %d\r\n", WM_ACTIVATEAPP, WM_SYSCOMMAND );

	g_proj_near = 100.0f;
	g_proj_far = 10000.0f;
	g_proj_fov = 45.0f;
	g_ortho = 0;
	g_orthosize = 3000.0f;

	g_fogenable = 0;
	g_fogcolor = 0x00FFFFFF;
	g_fogstart = 0.0f;
	g_fogend = 10000.0f;
	g_fogtype = 0.0f;
	s_savefogenable = g_fogenable;


	DbgOut( "E3DHSP VERSION %d\r\n", E3DHSPVERSION );

	if( m_cah ){
		delete m_cah;
		m_cah = 0;
	}

	m_cah = new CCameraAnimHandler();
	if( !m_cah ){
		DbgOut( "E3DInit : cah alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int thno;
	for( thno = 0; thno < THMAX; thno++ ){
		InitThTableElem( thno );
	}
	//s_threadnum = 0;

	int tmno;
	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;
		
		curtm->tonemapid = -1;
		curtm->tonenum = 0;
		curtm->pscid = 0;
		curtm->ptexid = 0;
	}
	s_usertonemapcnt = 0;


	InitializeCriticalSection( &g_crit_hshead );
	//InitializeCriticalSection( &g_crit_tempmp );
	InitializeCriticalSection( &g_crit_restore );

	ZeroMemory( s_keyboardcnt, sizeof( int ) * 256 );

	HRESULT hr;


	int wid;
	int objid;
	int fullscflag;// = p2;
	int bits;// = p3;
	int multisamplenum;// = p4;
	int gpuflag;
	int enablereverb;
	
	wid = code_getdi( 0 );
	objid = code_getdi( -1 );
	fullscflag = code_getdi( 0 );
	bits = code_getdi( 16 );
	multisamplenum = code_getdi( 0 );

	PVal *pval;
	APTR aptr;
	aptr = code_getva( &pval );						
	int retscid = 0;
	code_setva( pval, aptr, TYPE_INUM, &retscid );

	gpuflag = code_getdi( 1 );//!!!!

	enablereverb = code_getdi( 1 );//!!!!!!!
	g_index32bit = code_getdi( 0 );//!!!!!!!


	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}

	if( multisamplenum < 2 )
		multisamplenum = 0;
	if( multisamplenum > 16 )
		multisamplenum = 16;


	s_multisample = multisamplenum;


	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	g_cpuinfo.CheckCPU();
	DbgOut( 
		"CPU Vender: \"%s\"\n"
		"Family: %d  Model: %d  Stepping ID: %d\n"
		"Supported CPUID: %d\n"
		"Supported MMX: %d\n"
		"Supported SSE: %d\n"
		"Supported SSE2: %d\n"
		"Supported 3DNow!: %d\n"
		"Supported Enhanced 3DNow!: %d\n"
		, g_cpuinfo.vd.id
		, g_cpuinfo.dwFamily, g_cpuinfo.dwModel, g_cpuinfo.dwSteppingId
		, g_cpuinfo.bCPUID, g_cpuinfo.bMMX, g_cpuinfo.bSSE, g_cpuinfo.bSSE2, g_cpuinfo.b3DNow, g_cpuinfo.bE3DNow
	);

	/***
	if( g_cpuinfo.bSSE ){
		AsmSetMXCSR( -1 );
		AsmSetMXCSR( 1 );
	}
	***/

	//OpenDbgFile();	
		
	SetModuleDir();
	SetResDir( 0, 0 );

	LARGE_INTEGER timerfreq;
	QueryPerformanceFrequency( &timerfreq );
	s_timerfreq = timerfreq.QuadPart;

	if( g_pD3DApp ){
		//���łɍ쐬�ςȂ̂ŁAreturn
		return 0;
	}

	g_pD3DApp = new CD3DApplication();
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	BMSCR* bm;
	bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
	if( !bm ){
		DbgOut( "e3dinit : bmscr NULL : window id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_inst = (HINSTANCE)(bm->hInst);

	if( objid == -1 ){
		s_hwnd = bm->hwnd;

	}else{
		HSPOBJINFO objinfo;
		getobj( wid, objid, &objinfo );
		s_hwnd = objinfo.hCld;
	}


	//InitCommonControls(); 	// for progress bar
	INITCOMMONCONTROLSEX cominfo;
	cominfo.dwSize = sizeof( INITCOMMONCONTROLSEX );
	cominfo.dwICC = ICC_PROGRESS_CLASS;

	InitCommonControlsEx( &cominfo ); 	// for progress bar

	s_PBwnd = 0;


    if( FAILED( g_pD3DApp->Create( (HINSTANCE)bm->hInst, s_hwnd, fullscflag, bits, s_multisample, gpuflag ) ) ){
		_ASSERT( 0 );
        return 1;
	}	
		

	RECT clirect;
	int clientWidth, clientHeight;
	GetClientRect( s_hwnd, &clirect );
	clientWidth = clirect.right - clirect.left;
	clientHeight = clirect.bottom - clirect.top;

	DbgOut( "E3DInit : Width %d, Height %d\n", clientWidth, clientHeight );


	//�����resize�͕K�v�H�H�H�H
    //return d3dApp.Run();

	int ret;
	//HRESULT hr;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


//effect



	s_hMod = GetModuleHandle( "e3dhsp3.dll" );//!!!!!!!!!! LoadMOAFile�ł��g�p����B
	DbgOut( "\r\nE3DInit : ModuleName : e3dhsp3.dll\r\n\r\n" );
	_ASSERT( s_hMod );

	if( g_useGPU == 1 ){
		HRESULT hr;


		DWORD dwShaderFlags = 0;
		#ifdef DEBUG_VS
			dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
		#endif
		#ifdef DEBUG_PS
			dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
		#endif

        //dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

		//if( !g_bEnablePreshader )
		//	dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

		
		if( s_hMod == NULL ){
			::MessageBox( NULL, "���W���[�������Ⴂ�܂��B�I�����܂��B", "�G���[", MB_OK );
			DbgOut( "E3DInit : GetModuleHandle error !!!\n" );
			_ASSERT( 0 );
			g_useGPU = 0;
			return 1;
		}else{

			//hRs = FindResource( hMod, MAKEINTRESOURCE(IDR_FX2), RT_RCDATA );//����Œʂ�Ȃ��Ǝg���Ȃ�
			//if( hRs == NULL ){
			//	DbgOut( "E3DInit : FindResource error !!!\n" );
			//	_ASSERT( 0 );
			//	return 1;
			//}

			/***
			LPD3DXBUFFER pxbuff;
			hr = D3DXCreateBuffer( 7000, &pxbuff );
			if( hr != D3D_OK ){
				DbgOut( "E3DInit : D3DXCreateBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			CIncludeManager im;
			hr = D3DXCreateEffectFromResource( pdev, s_hMod, MAKEINTRESOURCE(IDR_RCDATA1), NULL, NULL, dwShaderFlags, NULL, &g_pEffect, &pxbuff );
			if( hr != D3D_OK ){
				::MessageBox( NULL, "�G�t�F�N�g�t�@�C���ɃG���[������܂��B\n�\�t�g�E�F�A�����ɐ؂�ւ��܂��B", "�m�F", MB_OK );
				DbgOut( "\n\nEffect File error %x !!!\n", hr );
				char* bufptr;
				if( pxbuff ){
					bufptr = (char*)pxbuff->GetBufferPointer();
					if( bufptr )
						DbgOut( bufptr );
					DbgOut( "\n\n" );
				}
				_ASSERT( 0 );
				g_useGPU = 0;
			}
			if( pxbuff ){
				pxbuff->Release();
			}
			***/
			if( (g_chkVS >= 20) || (g_chkVS == 0) ){
//DbgOut( "E3DInit : 000\r\n" );
				hr = D3DXCreateEffectFromResource( pdev, s_hMod, MAKEINTRESOURCE(IDR_RCDATA1), NULL, NULL, dwShaderFlags, NULL, &g_pEffect, NULL );
				if( hr != D3D_OK ){
					::MessageBox( NULL, "�G�t�F�N�g�t�@�C���ɃG���[������܂��B\n�I�����܂��B", "�m�F", MB_OK );
					DbgOut( "\n\nEffect File error %x !!!\n", hr );
					_ASSERT( 0 );
					g_useGPU = 0;
					return 1;
				}
//DbgOut( "E3DInit : 001\r\n" );

				if( g_usePostEffect ){
					char fxname[2048];
					char moduledir[2048];
					int leng;
					ZeroMemory( moduledir, 2048 );
					ZeroMemory( fxname, 2048 );
					leng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, 2048 );
					if( (leng <= 0) || (leng >= 2048) ){
						DbgOut( "e3dhsp : E3DInit : GetEnvironmentVariable error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					strcpy( fxname, moduledir );
					strcat( fxname, "\\E3D_HLSL\\posteffect.fx" );

					int existflag;
					existflag = _access( fxname, 0 );
					if( existflag != 0 ){
						::MessageBox( NULL, "fx�t�@�C����������܂���B\ndll�Ɠ����ꏊ��E3D_HLSL�t�H���_���R�s�[���Ă��������B",
							"�G���[", MB_OK );
						_ASSERT( 0 );
						return 1;
					}
//DbgOut( "E3DInit : 002\r\n" );
//_ASSERT( 0 );


					WCHAR widename[MAX_PATH];
					ZeroMemory( widename, sizeof( WCHAR ) * MAX_PATH );
					ret = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, fxname, (int)strlen( fxname ), widename, MAX_PATH );
					_ASSERT( ret != 0 );
					LPD3DXBUFFER pErr;
					hr = D3DXCreateEffectFromFileW(
						pdev, widename, NULL, NULL, 
						D3DXSHADER_DEBUG , NULL, &g_pPostEffect, &pErr );
					if( hr != D3D_OK ){
						::MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
						DbgOut( "\n\nPostEffect File error !!! %x\n", hr );
						_ASSERT( 0 );
						return 1;
					}

/***
					LPD3DXBUFFER pErr;
					hr = D3DXCreateEffectFromFile(
						pdev, fxname, NULL, NULL, 
						D3DXSHADER_DEBUG , NULL, &g_pPostEffect, &pErr );
					if( hr != D3D_OK ){
						::MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
						DbgOut( "\n\nPostEffect File error !!! %x\n", hr );
						_ASSERT( 0 );
						return 1;
					}
***/
//DbgOut( "E3DInit : 003\r\n" );

				}else{
					g_pPostEffect = 0;
				}

			}else{
				::MessageBox( NULL, "�n�[�h�E�F�A���v���𖞂����Ȃ��̂Ŏ��s�ł��܂���B\n�I�����܂��B", "�m�F", MB_OK );
				DbgOut( "\n\nHardware too old error !!!\n" );
				_ASSERT( 0 );
				g_useGPU = 0;
				return 1;
			}
			
			ret = GetShaderHandle();
			if( ret ){
				DbgOut( "GetShaderHandle error !!!\n" );
				_ASSERT( 0 );
				g_useGPU = 0;
				return 1;
			}
		}
		

		/***
		hr = D3DXCreateEffectFromFile( pdev, "c:\\disp2\\FX\\sigshaderb4.fx", NULL, NULL, dwShaderFlags, NULL, &g_pEffect, &pxbuff );
		if( hr != D3D_OK ){
			::MessageBox( NULL, "�G�t�F�N�g�t�@�C����������܂���B\n�\�t�g�E�F�A�����ɐ؂�ւ��܂��B", "�m�F", MB_OK );
			char* bufptr;
			bufptr = (char*)pxbuff->GetBufferPointer();
			DbgOut( "\n\nEffect File error !!!\n" );
			DbgOut( bufptr );
			DbgOut( "\n\n" );

			_ASSERT( 0 );
			g_useGPU = 0;
		}

		if( pxbuff ){
			pxbuff->Release();
		}
		***/

	}



//d3dxsprite
//	DbgOut( "E3DInit : aft load effect\r\n" );

	if( !s_d3dxsprite ){

		hr = D3DXCreateSprite( pdev, &s_d3dxsprite );
		if( (hr != D3D_OK) || !s_d3dxsprite ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
//billboard
	//g_bbhs
	if( !g_bbhs ){
		g_bbhs = new CHandlerSet();
		if( !g_bbhs ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateHandler();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		g_bbhs->m_billboardflag = 1;// !!!!! ver1036 2004/3/14

		ret = g_bbhs->CreateBillboard();
		if( ret ){
			DbgOut( "E3DInit : g_bbhs CreateBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}

// directmusic

//DbgOut( "E3DInit 0\r\n" );

	DestroySoundObject();

//DbgOut( "E3DInit 1\r\n" );

    // Initialize COM <---- for direct music
	HRESULT hr1;
    //hr1 = CoInitialize(NULL);
	hr1 = S_OK;

	if( hr1 == S_OK ){
		s_HS = new CHuSound();
//DbgOut( "E3DInit 2\r\n" );

		if( !s_HS ){
			DbgOut( "E3DInit : HuSound alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "E3DInit 3\r\n" );

		int ret;
		ret = s_HS->Init( s_hwnd, enablereverb );
		if( ret ){
			DbgOut( "E3DInit : HS Init error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		::MessageBox( s_hwnd, "COM�̏������Ɏ��s���܂����B\n���͖点�܂���B", "�G���[�ł��B", MB_OK );
	}

//DbgOut( "E3DInit 4\r\n" );


////
	ret = SetFovIndex();
	_ASSERT( !ret );


////
	SetDefaultRenderState();

    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}

/***
    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}
***/
	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ADDRESSV : error !!!\r\n" );
	}

////

	if( !s_qh ){
		s_qh = new CQHandler();
		if( !s_qh ){
			DbgOut( "E3DInit : s_qh alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = s_qh->InitHandler();
		if( ret ){
			DbgOut( "E3DInit : s_qh InitHandler error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

/// texbank
	if( !g_texbnk ){
		g_texbnk = new CTexBank( g_miplevels, g_mipfilter );
		if( !g_texbnk ){
			DbgOut( "E3DInit : new CTexBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

// swapchain
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		s_sctable[scno].scid = -1;
		s_sctable[scno].wid = -1;
		s_sctable[scno].objid = -1;
		s_sctable[scno].hwnd = 0;
		s_sctable[scno].texid = -1;
		s_sctable[scno].texsize.x = 0;
		s_sctable[scno].texsize.y = 0;
		s_sctable[scno].pSwapChain = NULL;
		s_sctable[scno].pZSurf = NULL;
		s_sctable[scno].pBackBuffer = NULL;
		s_sctable[scno].clearcol = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	}

	s_sctable[0].scid = 0;
	s_sctable[0].wid = wid;
	s_sctable[0].objid = objid;
	s_sctable[0].hwnd = s_hwnd;
	s_sctable[0].pSwapChain = NULL;
	s_sctable[0].pZSurf = g_pD3DApp->m_pZSurf;
	s_sctable[0].pBackBuffer = g_pD3DApp->m_pBackBuffer;
	s_sctable[0].clearcol = D3DCOLOR_ARGB( 0, 0, 0, 255 );

	s_swapchaincnt++;

//hsidelem
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		s_hsidelem[hsidno].hsid = hsidno;
		s_hsidelem[hsidno].validflag = 0;
		s_hsidelem[hsidno].hsptr = 0;
	}

//bg
	for( scno = 0; scno < SCMAX; scno++ ){
		s_bgdisp[scno].scid = -1;
		s_bgdisp[scno].bgdisp = 0;
	}

//bumplight
	g_LightOfBump[0] = 0;
	g_LightOfBump[1] = -1;
	g_LightOfBump[2] = -1;


//affinity
	HANDLE hproc;
	hproc = GetCurrentProcess();
	SetProcessAffinityMask( hproc, 1 );


///
	byeflag = 0;
	g_activateapp = 1;

DbgOut( "E3DInit sccess\r\n" );

	return 0;
}


int E3DBye()
{
	if( s_callinitflag == 0 )
		return 0;//!!!!!!!!!!!!!!!!

	if( byeflag == 1 )
		return 0;//!!!!!!!!!!!!!!!!

	//if( s_threadnum > 0 ){
	int tableno;
	for( tableno = 0; tableno < THMAX; tableno++ ){

		FreeThTableElem( tableno );		
	}

	if( m_cah ){
		delete m_cah;
		m_cah = 0;
	}


	int tmno;
	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;

		curtm->tonemapid = -1;
		curtm->tonenum = 0;
		if( curtm->pscid ){
			free( curtm->pscid );
			curtm->pscid = 0;
		}
		if( curtm->ptexid ){
			free( curtm->ptexid );
			curtm->ptexid = 0;
		}
	}
	s_usertonemapcnt = 0;

	//}


	//if( srcwnd )
	//	DestroyWindow( srcwnd );

	FreeHandlerSet();


	CAviData* curavi;
	CAviData* nextavi;
	curavi = avihead;
	while( curavi ){
		nextavi = curavi->next;

		delete curavi;
		curavi = nextavi;
	}
	avihead = 0;


	CXFont* curfont;
	CXFont* nextfont;
	curfont = xfonthead;
	while( curfont ){
		nextfont = curfont->next;

		delete curfont;
		curfont = nextfont;
	}
	xfonthead = 0;


	CLightData* curlight = g_lighthead;
	CLightData* nextlight;
	while( curlight ){
		nextlight = curlight->nextlight;

		delete curlight;
		curlight = nextlight;
	}
	g_lighthead = 0;


	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid >= 0 ){
			delete s_bgdisp[scno].bgdisp;
			s_bgdisp[scno].bgdisp = 0;
			s_bgdisp[scno].scid = -1;
		}
	}
	

	if( s_d3dxsprite ){
		s_d3dxsprite->Release();
		s_d3dxsprite = 0;
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}
	CMySprite* cursprite = s_spritehead;
	CMySprite* nextsprite;
	while( cursprite ){
		nextsprite = cursprite->next;

		delete cursprite;
		cursprite = nextsprite;
	}
	s_spritehead = 0;



	CNaviLine* curnl = g_nlhead;
	CNaviLine* nextnl;
	while( curnl ){
		nextnl = curnl->next;

		delete curnl;
		curnl = nextnl;
	}
	g_nlhead = 0;



	if( s_PBwnd ){
		DestroyWindow( s_PBwnd );
		s_PBwnd = 0;
	}

	if( s_qh ){
		delete s_qh;
		s_qh = 0;
	}

	if( g_texbnk ){
		delete g_texbnk;
		g_texbnk = 0;
	}


	DestroySoundObject();

	//PostQuitMessage(0);//<----- DestroyWindow�̌�ł́A���Ԃ�Ӗ������B

	if( s_effectdecl ){
		s_effectdecl->Release();
		s_effectdecl = 0;
	}
	if( s_tlvdecl ){
		s_tlvdecl->Release();
		s_tlvdecl = 0;
	}

	if( g_pEffect ){
		g_pEffect->Release();
		g_pEffect = 0;
	}
	if( g_pPostEffect ){
		g_pPostEffect->Release();
		g_pPostEffect = 0;
	}

//swapchain
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			DestroySwapChainElem( &(s_sctable[scno]), 1 );
		}
	}


	if( g_pD3DApp ){

		//_ASSERT( s_hwnd == srcwnd );

		//clean up
		g_pD3DApp->Cleanup3DEnvironment();
		//DestroyMenu( GetMenu(hWnd) );

		delete g_pD3DApp;
		g_pD3DApp = 0;
	
	}

	CloseDbgFile();

	
    // Close down COM
    //CoUninitialize();

	DeleteCriticalSection( &g_crit_hshead );
	//DeleteCriticalSection( &g_crit_tempmp );
	DeleteCriticalSection( &g_crit_restore );

	byeflag = 1;

	return 0;
}
int FreeHandlerSet()
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		HSIDELEM* curhse = s_hsidelem + hsidno;
		if( curhse->validflag && curhse->hsptr ){
			delete curhse->hsptr;
		}
		curhse->hsid = hsidno;
		curhse->validflag = 0;
		curhse->hsptr = 0;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	if( g_bbhs ){
		delete g_bbhs;
		g_bbhs = 0;
	}


	return 0;
}

int ChainNewHandlerSet( CHandlerSet* newhs )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int freehsid = -1;
	int hsidno;
	for( hsidno = 1; hsidno < HSIDMAX; hsidno++ ){// 1����I�I�I�@�O�͎g��Ȃ��B
		if( s_hsidelem[hsidno].validflag == 0 ){
			freehsid = hsidno;
			break;
		}
	}

	if( freehsid <= 0 ){
		DbgOut( "ChainNewHandlerSet : freehsid error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	s_hsidelem[freehsid].hsptr = newhs;
	s_hsidelem[freehsid].validflag = 1;
	newhs->serialno = s_hsidelem[freehsid].hsid;//!!!!!!!!!!!!!


	LeaveCriticalSection( &g_crit_hshead );//###### end crit

	return 0;

}

int E3DSigLoad()
{
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult
//first
	
	int ret;

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigLoad : sig name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//second	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

		
// 3:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 4:
	float mult;
	mult = (float)code_getdd(1.0);

//////

	int temphsid = -1;
	ret = SigLoad_F( tempname, mult, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", tempname );
		_ASSERT( 0 );
		int retval = -1;
		code_setva( pvalhsid, aptrhsid, TYPE_INUM, &retval );
		return 1;
	}

	DbgOut( "E3DSigLoad : temphsid %d\r\n", temphsid );

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	
	return 0;
}

int SigLoad_F( char* tempname, float mult, int adjustuvflag, int* hsidptr )
{
	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!



	ret = newhs->LoadSigFile( tempname, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : E3DCreateHandler : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DSigLoad : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;


	return 0;
}

int E3DCreateSwapChain()
{
/***
static int s_swapchaincnt = 0;
typedef struct tag_swapchainelem
{
	int scid;
	int wid;
	int objid;
	IDirect3DSwapChain9* pSwapChain;
	IDirect3DSurface9* pZSurf;
	IDirect3DSurface9* pBackBuffer;

} SWAPCHAINELEM;
#define SCMAX	256
static SWAPCHAIN s_sctable[ SCMAX ];
***/
	int wid, objid;

	wid = code_getdi( 0 );
	objid = code_getdi( -1 );

	PVal *pvalscid;
	APTR aptrscid;
	aptrscid = code_getva( &pvalscid );

	SWAPCHAINELEM* cursc = 0;
	cursc = GetFreeSwapChainElem();
	if( !cursc ){
		DbgOut( "CreateSwapChainElem : GetFreeSwapChainElem NULL error : swap chain num overflow !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = CreateSwapChainElem( s_swapchaincnt, cursc, wid, objid, 0, 0 );
	if( ret ){
		DbgOut( "E3DCreateSwapChain : CreateSwapChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalscid, aptrscid, TYPE_INUM, &cursc->scid );

	s_swapchaincnt++;

	return 0;
}

int CreateSwapChainElem( int scid, SWAPCHAINELEM* cursc, int wid, int objid, int texid, D3DCOLOR* clearcol )
{
	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "CreateSwapChainElem : GetD3DDevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HWND hwnd = 0;

	if( texid <= 0 ){
		BMSCR* bm;
		bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
		if( !bm ){
			DbgOut( "CreateSwapChainElem : bmscr NULL : window id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( objid == -1 ){
			hwnd = bm->hwnd;
		}else{
			HSPOBJINFO objinfo;
			getobj( wid, objid, &objinfo );
			hwnd = objinfo.hCld;
		}

		RECT rc;
		GetClientRect( hwnd, &rc );

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp = g_pD3DApp->m_d3dpp;
		d3dpp.BackBufferWidth = rc.right - rc.left;
		d3dpp.BackBufferHeight = rc.bottom - rc.top;
		d3dpp.hDeviceWindow = (HWND)hwnd;
		d3dpp.EnableAutoDepthStencil = FALSE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.PresentationInterval = g_presentinterval;
		d3dpp.BackBufferCount = 1;//!!!!

		HRESULT hres;
		hres = pdev->CreateAdditionalSwapChain( &d3dpp, &(cursc->pSwapChain) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : dev CreateAdditionalSwapChain error %x !!!\n", hres );
			_ASSERT( 0 );
			return 1;
		}


		hres = cursc->pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &(cursc->pBackBuffer) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : swapchain GetBackBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		D3DFORMAT fmt;
		fmt = D3DFMT_D16;
//		fmt = D3DFMT_D24S8;
//		fmt = D3DFMT_D32;
		//hres = pdev->CreateDepthStencilSurface( d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, fmt, d3dpp.MultiSampleType, &(cursc->pZSurf), NULL );
		hres = pdev->CreateDepthStencilSurface( d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, fmt, 
			d3dpp.MultiSampleType,
			//max( 0, d3dpp.MultiSampleQuality - 1 ), 
			d3dpp.MultiSampleQuality,
			FALSE, &(cursc->pZSurf), NULL ); 
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : dev CreateDepthStencilSurface error %x !!!\n", hres );
			_ASSERT( 0 );
			return 1;
		}


		cursc->texid = -1;
		cursc->texsize.x = 0;
		cursc->texsize.y = 0;

		if( clearcol ){
			cursc->clearcol = *clearcol;
		}else{
			cursc->clearcol = D3DCOLOR_ARGB( 0, 0, 0, 255 );
		}

	}else{

		if( !g_texbnk ){
			DbgOut( "CreateSwapChainElem : texbank NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		LPDIRECT3DTEXTURE9 curtex = 0;
		ret = g_texbnk->GetTexDataBySerial( texid, &curtex );
		if( ret || !curtex ){
			DbgOut( "CreateSwapChainElem : texbank GetTexDataBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = curtex->GetSurfaceLevel( 0, &(cursc->pBackBuffer) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : tex GetSrufaceLevel error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int width, height, pool, transparent, format;
		ret = g_texbnk->GetTextureInfoBySerial( texid, &width, &height, &pool, &transparent, &format );
		if( ret ){
			DbgOut( "CreateSwapChainElem : texbank GetTextureSizeBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		D3DFORMAT fmt;
		fmt = D3DFMT_D16;
//		fmt = D3DFMT_D24S8;
//		fmt = D3DFMT_D32;
		hres = pdev->CreateDepthStencilSurface( width, height, fmt, 
			D3DMULTISAMPLE_NONE,
			0,
			FALSE, &(cursc->pZSurf), NULL ); 
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : dev CreateDepthStencilSurface error %x !!!\n", hres );
			_ASSERT( 0 );
			return 1;
		}

		cursc->texid = texid;
		cursc->texsize.x = width;
		cursc->texsize.y = height;

		if( clearcol ){
			cursc->clearcol = *clearcol;
		}else{
			cursc->clearcol = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		}
	}
	
	//cursc->scid = s_swapchaincnt;
	cursc->scid = scid;
	cursc->wid = wid;
	cursc->objid = objid;
	cursc->hwnd = hwnd;

	return 0;
}

SWAPCHAINELEM* GetFreeSwapChainElem()
{
	SWAPCHAINELEM* retelem = 0;

	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid == -1 ){
			retelem = &(s_sctable[scno]);
			break;
		}
	}
	return retelem;
}

int E3DDestroySwapChain()
{

	int scid;
	scid = code_getdi( 1 );
	if( scid <= 0 ){
		DbgOut( "E3DDestroySwapChain : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		return 0;
	}

	int ret;
	ret = DestroySwapChainElem( cursc, 1 );
	if( ret ){
		DbgOut( "E3DDestroySwapChain : DestroySwapChainElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

SWAPCHAINELEM* GetSwapChainElem( int scid )
{


	SWAPCHAINELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid == scid ){
			retelem = &(s_sctable[scno]);
			break;
		}

	}

	if( retelem ){
		SetMatProj( retelem );//!!!!!!!!!!!!!!
	}


	return retelem;
}

int DestroySwapChainElem( SWAPCHAINELEM* cursc, int initflag )
{

	if( initflag ){
		cursc->scid = -1;
		cursc->wid = -1;
		cursc->objid = -1;
		cursc->hwnd = 0;
		cursc->texid = -1;
		cursc->texsize.x = 0;
		cursc->texsize.y = 0;
	}

	if( cursc->pZSurf ){
		cursc->pZSurf->Release();
		cursc->pZSurf = 0;
	}

	if( cursc->pBackBuffer ){
		cursc->pBackBuffer->Release();
		cursc->pBackBuffer = 0;
	}

	if( cursc->pSwapChain ){
		cursc->pSwapChain->Release();
		cursc->pSwapChain = 0;
	}

	return 0;

}

int E3DRender()
{
	// first : int handlerset no.
	// second : int withalpha
	// third : int framecnt <-- for afterimage
	// 4th : projmode
	// lastparent
	// siglightflag

	int scid, hsid, withalpha, framecnt, projmode, lastparent, siglightflag, transskip;

	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DRender : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hsid = code_getdi( 0 );
	withalpha = code_getdi( 0 );
	framecnt = code_getdi( 0 );
	projmode = code_getdi( 0 );
	lastparent = code_getdi( 0 );
	siglightflag = code_getdi( 1 );
	transskip = code_getdi( 0 );

//DbgOut( "e3dhsp : E3DRender : hsid %d, withalpha %d, framecnt %d, projmode %d\n",
//	   hsid, withalpha, framecnt, projmode );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRender : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	s_matView�̃Z�b�g�́AE3DBeginScene�Ɉړ��I�I�I
	***/

	//LARGE_INTEGER lbeftime1;
	//LARGE_INTEGER lafttime1;

	//LARGE_INTEGER lbeftime2;
	//LARGE_INTEGER lafttime2;


	//QueryPerformanceCounter( &lbeftime1 );
	int* bonearray;
	if( lastparent != 0 ){
		curhs->SetBoneArray( lastparent );
		bonearray = curhs->m_bonearray;
	}else{
		bonearray = 0;
	}

	if( transskip == 0 ){
		ret = curhs->Transform( s_camerapos, pdev, s_matView, siglightflag, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

//DbgOut( "check!!!: E3DRender : camerapos %f %f %f\r\n", s_camerapos.x, s_camerapos.y, s_camerapos.z );
//DbgOut( "check!!!: E3DRender : targetpos %f %f %f\r\n", s_cameratarget.x, s_cameratarget.y, s_cameratarget.z );
//DbgOut( "check!!!: E3DRender : upvec %f %f %f\r\n", s_cameraupvec.x, s_cameraupvec.y, s_cameraupvec.z );


	//QueryPerformanceCounter( &lafttime1 );


	//QueryPerformanceCounter( &lbeftime2 );
	ret = curhs->Render( 0, pdev, withalpha, s_matView, s_camerapos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//QueryPerformanceCounter( &lafttime2 );


	/***
	LONGLONG difftime1, difftime2;
	difftime1 = lafttime1.QuadPart - lbeftime1.QuadPart;
	difftime2 = lafttime2.QuadPart - lbeftime2.QuadPart;

	float time1, time2;
	time1 = (float)difftime1 / (float)s_timerfreq;
	time2 = (float)difftime2 / (float)s_timerfreq;


	unsigned char tempstate[256];
	GetKeyboardState( tempstate );
	if( (tempstate[VK_SHIFT] & 0x80) && (tempstate['D'] & 0x80) ){
		DbgOut( "Check Time : Transform Time %f, Render Time %f\n", time1, time2 );
	}
	***/

    //if( pdev && g_pD3DApp->m_bReady ){
	//	pdev->Present( NULL, NULL, NULL, NULL );
	//}

	return 0;
}

CHandlerSet* GetHandlerSet( int hsid )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	if( (hsid < 0) || (hsid >= HSIDMAX) ){
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 0;
	}

	if( s_hsidelem[hsid].validflag ){
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return s_hsidelem[hsid].hsptr;
	}else{
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 0;
	}

}

int E3DPresent()
{

	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DPresent : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPresent : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

    if( pdev && g_pD3DApp->m_bReady ){
		if( (scid == 0) && cursc->hwnd ){
			pdev->Present( NULL, NULL, cursc->hwnd, NULL );
		}else if( cursc->pSwapChain ){
			cursc->pSwapChain->Present( NULL, NULL, cursc->hwnd, NULL, D3DPRESENT_DONOTWAIT );
		}
	}

	g_rendercnt = 0;

	return 0;
}
int E3DBeginScene()
{
	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int skipflag;
	skipflag = code_getdi(0);

	int zscid;
	zscid = code_getdi(-1);

//////////

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* zsc = 0;
	int zsizex, zsizey;
	if( zscid >= 0 ){
		zsc = GetSwapChainElem( zscid );
		zsizex = g_scsize.x;
		zsizey = g_scsize.y;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int rendersizex, rendersizey;
	rendersizex = g_scsize.x;
	rendersizey = g_scsize.y;

	int clearzflag = 1;
	IDirect3DSurface9* zsurf = 0;
	if( zsc ){
		if( (zsizex == rendersizex) && (zsizey == rendersizey) ){
			zsurf = zsc->pZSurf;
			clearzflag = 0;
		}else{
			zsurf = cursc->pZSurf;
			clearzflag = 1;
		}
	}else{
		zsurf = cursc->pZSurf;
		clearzflag = 1;
	}


	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DBeginScene : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	//hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
	hres = pdev->SetDepthStencilSurface( zsurf );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (skipflag & 2) == 0 ){
		CalcMatView();

		ret = SetShaderConst( 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

   if( FAILED( pdev->BeginScene() ) ){
		_ASSERT( 0 );
        return 1;
	}

    // Clear the viewport

	if( (skipflag & 1) == 0 ){
		if( clearzflag ){
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 cursc->clearcol, 1.0f, 0L );
		}else{
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET, 
								 cursc->clearcol, 1.0f, 0L );
		}
	   if( hres != D3D_OK ){
			//_ASSERT( 0 );
		   DbgOut( "E3DBegin : Clear error !!!\n" );
			return 1;
	   }
	}

//!!!!!!!!!!!!!!!!
//render bgdisp
//!!!!!!!!!!!!!!!!

	if( ((skipflag & 1) == 0) && ((skipflag & 4) == 0) ){

		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}
	return 0;
}

int E3DEndScene()
{
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return -1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return -1;
	}


    pdev->EndScene();

	return 0;
}


int E3DCreateBG()
{
	int ret;
	int scid;
	scid = code_getdi( 0 );//!!!!!!!!
	if( scid < 0 ){
		DbgOut( "E3DCreateBG : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateBG : file name1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char tempname2[MAX_PATH];
	ret = GetName( tempname2, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateBG : file name2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	


	float uanim;
	uanim = (float)code_getdd(0.0);

	float vanim;
	vanim = (float)code_getdd(0.0);

	int isround;
	isround = code_getdi(0);

	float fogdist;
	fogdist = (float)code_getdd(g_proj_far);

///////////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	BGDISPELEM* oldbgelem;
	oldbgelem = GetBGDispElem( scid );
	if( oldbgelem ){
		if( oldbgelem->bgdisp != 0 ){
			ret = g_texbnk->ResetDirtyFlag();
			ret = DestroyTexture( -2, -2, -2, 1, scid );
			if( ret ){
				DbgOut( "E3DCreateBG : DestroyTexture error !!!\n" );
				_ASSERT( 0 );
				//return 1;
			}

			delete oldbgelem->bgdisp;
			oldbgelem->bgdisp = 0;
			oldbgelem->scid = -1;
		}
	}

	BGDISPELEM* newbgelem;
	if( oldbgelem ){
		newbgelem = oldbgelem;
		newbgelem->scid = scid;
	}else{
		newbgelem = GetFreeBGDispElem();
	}
	if( !newbgelem ){
		DbgOut( "E3DCreateBG : newbg NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	UV startuv = { 0.0f, 0.0f };
	UV enduv = { 1.0f, 1.0f };

	CBgDisp2* newbg;
	newbg = new CBgDisp2( pdev, tempname1, 0, tempname2, 0, uanim, vanim, 0xFFFFFFFF, isround, fogdist, startuv, enduv, 0 );
	if( !newbg ){
		DbgOut( "E3DSetBG : newbg alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	newbgelem->bgdisp = newbg;
	newbgelem->scid = scid;

/////////

	return 0;
}

//E3DSetBGU startu10000, endu10000
int E3DSetBGU()
{

//1
	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	float startu;
	startu = (float)code_getdd(0.0);

//3
	float endu;
	endu = (float)code_getdd(0.0);

/////////
	BGDISPELEM* curbgelem;
	curbgelem = GetBGDispElem( scid );
	if( !curbgelem ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( curbgelem->bgdisp ){
		ret = curbgelem->bgdisp->SetU( startu, endu );
		if( ret ){
			DbgOut( "E3DSetBGU : bgdisp SetU error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

int E3DSetBGV()
{

//1
	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	float startv;
	startv = (float)code_getdd(0.0);

//3
	float endv;
	endv = (float)code_getdd(0.0);

/////////

	BGDISPELEM* curbgelem;
	curbgelem = GetBGDispElem( scid );
	if( !curbgelem ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	if( curbgelem->bgdisp ){
		ret = curbgelem->bgdisp->SetV( startv, endv );
		if( ret ){
			DbgOut( "E3DSetBGV : bgdisp SetV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}


int E3DDestroyBG()
{
	int ret;
	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DDestroyBG : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	BGDISPELEM* delbgelem;
	delbgelem = GetBGDispElem( scid );
	if( delbgelem ){
		ret = g_texbnk->ResetDirtyFlag();
		ret = DestroyTexture( -2, -2, -2, 1, scid );
		if( ret ){
			DbgOut( "E3DDestroyBG : DestroyTexture error !!!\n" );
			_ASSERT( 0 );
			//return 1;
		}

		delbgelem->scid = -1;
		if( delbgelem->bgdisp != 0 ){
			delete delbgelem->bgdisp;
			delbgelem->bgdisp = 0;
		}
	}
	return 0;
}

BGDISPELEM* GetFreeBGDispElem()
{
	BGDISPELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid == -1 ){
			retelem = &(s_bgdisp[scno]);
			break;
		}
	}
	return retelem;
}
BGDISPELEM* GetBGDispElem( int scid )
{
	BGDISPELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid == scid ){
			retelem = &(s_bgdisp[scno]);
			break;
		}
	}
	return retelem;

}
int CalcMatView()
{
	float fDeg2Pai = (float)DEG2PAI;
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

		s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
		s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		D3DXMATRIX tempmatView;
		float cx, cy, cz;

		D3DXMatrixLookAtLH( &tempmatView, &s_camerapos, &s_cameratarget, &vUpVec );
		cx = tempmatView._41;
		cy = tempmatView._42;
		cz = tempmatView._43;
		tempmatView._41 = 0.0f;
		tempmatView._42 = 0.0f;
		tempmatView._43 = 0.0f;

		D3DXVECTOR3 axisvec;
		CQuaternion2 addrot;
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
		
		D3DXMATRIX addrotmat;
		addrotmat = addrot.MakeRotMatX();

		s_matView = tempmatView * addrotmat;
		s_matView._41 = cx;
		s_matView._42 = cy; 
		s_matView._43 = cz;

	}else{

		D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &s_cameraupvec );
	}

	return 0;

}
int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec )
{
	float mag;
	mag = srcvec->x * srcvec->x + srcvec->y * srcvec->y + srcvec->z * srcvec->z;

	float sqmag;
	sqmag = (float)sqrt( mag );

	if( sqmag != 0.0f ){
		float magdiv;
		magdiv = 1.0f / sqmag;
		dstvec->x *= magdiv;
		dstvec->y *= magdiv;
		dstvec->z *= magdiv;

	}else{
//		DbgOut( "q2 : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
//		_ASSERT( 0 );
	}

	return 0;
}

int E3DAddMotion()
{
	//1 : handlerset id
	//2 : filename
	//3 : [out] motcookie
	//4 : [out] maxnum
	//5 : mvmult

	int ret;
	int hsid;
	hsid = code_getdi(0);

	
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DAddMotion : file name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//
	PVal *pvalcookie;
	APTR aptrcookie;
	aptrcookie = code_getva( &pvalcookie );

	
	//
	PVal *pvalmotnum;
	APTR aptrmotnum;
	aptrmotnum = code_getva( &pvalmotnum );


	float mvmult;
	mvmult = (float)code_getdd(1.0);

////////

	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotion_F( hsid, tempname, &tempmotid, &tempnum, mvmult );
	if( ret ){
		DbgOut( "E3DAddMotion : AddMotoin_F error %s !!!\n", tempname );
		_ASSERT( 0 );

		tempmotid = -1; tempnum = 0;
		code_setva( pvalcookie, aptrcookie, TYPE_INUM, &tempmotid );
		code_setva( pvalmotnum, aptrmotnum, TYPE_INUM, &tempnum );
		return 1;
	}

	code_setva( pvalcookie, aptrcookie, TYPE_INUM, &tempmotid );
	code_setva( pvalmotnum, aptrmotnum, TYPE_INUM, &tempnum );

	return 0;

}

int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr, float mvmult )
{

	char* ext;
	int ch = '.';
	int filekind = -1;
	ext = strrchr( tempname, ch );
	if( !ext ){
		DbgOut( "E3DAddMotion : file not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int cmp1, cmp2;
	cmp1 = strcmp( ext + 1, "qua" );
	cmp2 = strcmp( ext + 1, "mot" );
	if( (cmp1 != 0) && (cmp2 != 0) ){
		DbgOut( "E3DAddMotion : file not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;		
	}

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


EnterCriticalSection( &(curhs->m_crit_addmotion) );//######## start crit
	
	int ret = 0;
//	if( cmp1 == 0 ){
		ret = curhs->LoadQuaFile( tempname, cookieptr, maxnumptr, mvmult );	
//	}else if( cmp2 == 0 ){
//		ret = curhs->LoadMotFile( tempname, cookieptr, maxnumptr );
//	}

LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit


	if( ret ){
		DbgOut( "E3DAddMotion : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMotionKind()
{
	//1 : handlerset id
	//2 : motkind
	int hsid;
	hsid = code_getdi(0);

	int motkind;
	motkind = code_getdi(0);
	
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetMotionKind( motkind );
	if( ret ){
		DbgOut( "E3DSetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMotionKind()
{
	//1 : handlerset id
	//2 : motkindptr
	int hsid;
	hsid = code_getdi(0);


	PVal *pvalmotkind;
	APTR aptrmotkind;
	aptrmotkind = code_getva( &pvalmotkind );



///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind;
	ret = curhs->GetMotionKind( &dstmotkind );
	if( ret ){
		DbgOut( "E3DGetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalmotkind, aptrmotkind, TYPE_INUM, &dstmotkind );


	return 0;
}

int E3DSetNewPose()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);
	
	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

///////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPose : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextno;
	nextno = curhs->SetNewPose();
	_ASSERT( nextno >= 0 );

	code_setva( pvalframe, aptrframe, TYPE_INUM, &nextno );
	return 0;
}

int E3DSetMotionStep()
{
	//1 : handlerset id
	//2 : motkindptr
	//3 : stepptr
	int hsid;
	hsid = code_getdi(0);

	int motkind;
	motkind = code_getdi(0);

	int step;
	step = code_getdi(0);
////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionStep : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret = 0;
	ret = curhs->SetMotionStep( motkind, step );
	if( ret ){
		DbgOut( "E3DSetMotionStep : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DChkConflict()
{
	// type $202
	// 1 : src handlerset id
	// 2 : chk handlerset id
	// (3,4 : rate ��ŁA�g�����邩���H)
	int srchsid;
	srchsid = code_getdi(0);

	int chkhsid;
	chkhsid = code_getdi(0);

	PVal *pvalconf;
	APTR aptrconf;
	aptrconf = code_getva( &pvalconf );

	PVal *pvalinview;
	APTR aptrinview;
	aptrinview = code_getva( &pvalinview );


////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflict : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflict : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict( chkhs, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflict : srchs ChkConflict error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalconf, aptrconf, TYPE_INUM, &conf );
	code_setva( pvalinview, aptrinview, TYPE_INUM, &inviewflag );
	return 0;
}

int E3DChkConflict2()
{
	// type $202
	// 1 : hsid1
	// 2 : partno1
	// 3 : hsid2
	// 4 : partno2
	// 5 : inviewflag

	int srchsid;
	srchsid = code_getdi(0);

	int srcpartno;
	srcpartno = code_getdi(0);

	int chkhsid;
	chkhsid = code_getdi(0);

	int chkpartno;
	chkpartno = code_getdi(0);


	PVal *pvalconf;
	APTR aptrconf;
	aptrconf = code_getva( &pvalconf );

	PVal *pvalinview;
	APTR aptrinview;
	aptrinview = code_getva( &pvalinview );

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict2( srcpartno, chkhs, chkpartno, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflict2 : srchs ChkConflict2 error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	code_setva( pvalconf, aptrconf, TYPE_INUM, &conf );
	code_setva( pvalinview, aptrinview, TYPE_INUM, &inviewflag );

	return 0;
}

int E3DCreateAfterImage()
{
	// 1 : handlerset id
	// 2 : imagenumptr
	int hsid;
	hsid = code_getdi(0);

	int imagenum;
	imagenum = code_getdi(0);

////////

	if( g_useGPU == 0 ){
		int ret;
		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		_ASSERT( !ret );

		CreateAfterImage( hsid, pdev, imagenum );
	}
	return 0;
}

int CreateAfterImage( int hsid, LPDIRECT3DDEVICE9 pdev, int imagenum )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "CreateAfterImage : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->CreateAfterImage( pdev, imagenum );
	if( ret ){
		DbgOut( "E3DCreateAfterImage : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;

}


int E3DDestroyAfterImage()
{
	//1 : handlerset id

	int hsid;
	hsid = code_getdi(0);

///////

	if( g_useGPU == 0 ){
		CHandlerSet* curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DDestroyAfterImage : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int ret = 0;
		ret = curhs->DestroyAfterImage();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

int E3DSetAlpha()
{
	// 1 : handlerset id
	// 2 : usealphaflag

	int hsid;
	hsid = code_getdi(0);

	int usealphaflag;
	usealphaflag = code_getdi(0);

	int updateflag;
	updateflag = code_getdi(1);

/////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float alpha;
	if( usealphaflag == 0 ){
		alpha = 1.0f;
	}else{
		alpha = 0.5f;
	}
	int ret = 0;
	ret = curhs->SetAlpha( alpha, -1, updateflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int E3DSetAlpha2()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : alphaval
	float alphaval;
	alphaval = (float)code_getdd(0.0);

	// 3 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	//4
	int updateflag;
	updateflag = code_getdi(1);


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetAlpha2 : hsid error !!!\n" );
			DbgOut( "e3dhsp : SetAlpha2 : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetAlpha2 : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( alphaval < 0.0f )
		alphaval = 0.0f;
	if( alphaval > 1.0f )
		alphaval = 1.0f;

	
	int ret = 0;
	ret = curhs->SetAlpha( alphaval, partno, updateflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int E3DSetPos()
{
	// 1 : handlerset id
	// 2 : posxptr
	// 3 : posyptr
	// 4 : poszptr

	int hsid;
	hsid = code_getdi(0);

	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.pos = D3DXVECTOR3( posx, posy, posz );
	curhs->m_gpd.CalcMatWorld();

	return 0;
}
/////////////
int E3DGetPos()
{
	// 1 : handlerset id
	// 2 : posxptr
	// 3 : posyptr
	// 4 : poszptr
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalposx;
	APTR aptrposx;
	aptrposx = code_getva( &pvalposx );

	PVal *pvalposy;
	APTR aptrposy;
	aptrposy = code_getva( &pvalposy );

	PVal *pvalposz;
	APTR aptrposz;
	aptrposz = code_getva( &pvalposz );
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dposx, dposy, dposz;
	dposx = (double)curhs->m_gpd.m_gpe.pos.x;
	dposy = (double)curhs->m_gpd.m_gpe.pos.y;
	dposz = (double)curhs->m_gpd.m_gpe.pos.z;

	code_setva( pvalposx, aptrposx, TYPE_DNUM, &dposx );
	code_setva( pvalposy, aptrposy, TYPE_DNUM, &dposy );
	code_setva( pvalposz, aptrposz, TYPE_DNUM, &dposz );

	return 0;

}


int E3DSetDir()
{
	// 1 : handlerset id
	// 2 : dirxptr (deg)
	// 3 : diryptr (deg)
	// 4 : dirzptr (deg)
	int hsid;
	hsid = code_getdi(0);

	float dirx;
	dirx = (float)code_getdd(0.0);

	float diry;
	diry = (float)code_getdd(0.0);

	float dirz;
	dirz = (float)code_getdd(0.0);
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDir : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.rot = D3DXVECTOR3( dirx, diry, dirz );
	curhs->m_gpd.CalcMatWorld();

	return 0;
}

int E3DRotateInit()
{
	// 1 : handlerset id

	int hsid;
	hsid = code_getdi(0);

	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateInit : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	curhs->m_gpd.CalcMatWorld();

	return 0;
}

int E3DRotateX()
{
	// type $202
	// 1 : handlerset id
	// 2 : degx (deg)

	int hsid;
	hsid = code_getdi(0);

	float dirx;
	dirx = (float)code_getdd(0.0);
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateX : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisx( 1.0f, 0.0f, 0.0f );
	CQuaternion qx, newq;
	qx.SetAxisAndRot( axisx, dirx * (float)DEG2PAI );
	newq = qx * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );

	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );

	curhs->m_gpd.m_gpe.rot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}
int E3DRotateY()
{
	// 1 : handlerset id
	// 2 : diry (deg)

	int hsid;
	hsid = code_getdi(0);

	float diry;
	diry = (float)code_getdd(0.0);
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateY : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisy( 0.0f, 1.0f, 0.0f );
	CQuaternion qy, newq;
	qy.SetAxisAndRot( axisy, diry * (float)DEG2PAI );
	newq = qy * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );
	curhs->m_gpd.m_gpe.rot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}

int E3DRotateZ()
{
	// 1 : handlerset id
	// 2 : dirzptr (deg)

	int hsid;
	hsid = code_getdi(0);

	float dirz;
	dirz = (float)code_getdd(0.0);
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisz( 0.0f, 0.0f, 1.0f );
	CQuaternion qz, newq;
	qz.SetAxisAndRot( axisz, dirz * (float)DEG2PAI );
	newq = qz * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );
	curhs->m_gpd.m_gpe.rot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}


int E3DTwist()
{
	// type $202
	// 1 : handlerset id
	// 2 : stepptr
	int hsid;
	hsid = code_getdi(0);

	float step;
	step = (float)code_getdd(0.0);

///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwist : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float steprad;
	D3DXVECTOR3 axis;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );

	ret = curhs->m_gpd.m_q.Rotate( &axis, orgvec );
	_ASSERT( !ret );
	DXVec3Normalize( &axis, &axis );

	steprad = (float)step * (float)DEG2PAI;//�ω���
	CQuaternion newq, multq;

	multq.SetAxisAndRot( axis, steprad );
	newq = multq * curhs->m_gpd.m_q;

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );
	_ASSERT( !ret );
	curhs->m_gpd.m_gpe.rot = neweul;

	curhs->m_gpd.CalcMatWorld();


	float savetwist;
	savetwist = curhs->m_twist;
	// 0 - 360
	float finaltwist, settwist;
	finaltwist = savetwist + step;

	if( finaltwist >= 360.0f ){
		while( finaltwist >= 360.0f )
			finaltwist -= 360.0f;
		settwist = finaltwist;
	}else if( finaltwist < 0.0f ){
		while( finaltwist < 0.0f )
			finaltwist += 360.0f;
		settwist = finaltwist;
	}else{
		settwist = finaltwist;
	}
	curhs->m_twist = settwist;



	return 0;
}

int E3DPosForward()
{
	// 1 : handlerset id
	// 2 : stepptr
	int hsid;
	hsid = code_getdi(0);

	float step;
	step = (float)code_getdd(0.0);

///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPosForward : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 stepvec( 0.0f, 0.0f, 1.0f );
	
	ret = curhs->m_gpd.m_q.Rotate( &stepvec, orgvec );
	_ASSERT( !ret );

	curhs->m_gpd.m_gpe.pos += step * stepvec;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}

////////////////
int E3DCloseTo()
{
	// 1 : change handlerset id
	// 2 : src handlerset id
	int chghsid;
	chghsid = code_getdi(0);

	int srchsid;
	srchsid = code_getdi(0);

	float movestep;
	movestep = (float)code_getdd(100.0);

////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DCloseTo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DCloseTo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// position
	float distmag;
	int zeroflag = 0;
	D3DXVECTOR3 vec3, nvec3;
	vec3 = srchs->m_gpd.m_gpe.pos - chghs->m_gpd.m_gpe.pos;
	if( (vec3.x != 0.0f) || (vec3.y != 0.0f) && (vec3.z != 0.0f) )
		D3DXVec3Normalize( &nvec3, &vec3 );
	
	distmag = ( vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z );
	if( distmag > (movestep * movestep) ){
		chghs->m_gpd.m_gpe.pos += movestep * nvec3;
	}else{
		zeroflag = 1;//!!!!!!!!!!
		chghs->m_gpd.m_gpe.pos = srchs->m_gpd.m_gpe.pos;
	}

	chghs->m_gpd.CalcMatWorld();
	int ret;
	if( zeroflag == 0 ){
		ret = DirToTheOther( &(chghs->m_gpd), srchs->m_gpd );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int DirToTheOtherXZ(  CGPData* set1, CGPData set2 )
{
	D3DXVECTOR2 basevec;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	
	basevec.x = 0.0f;
	basevec.y = -1.0f;

	vec.x = set2.m_gpe.pos.x - set1->m_gpe.pos.x;
	vec.y = set2.m_gpe.pos.z - set1->m_gpe.pos.z;

	D3DXVec2Normalize( &nvec, &vec );
	float dot;
	dot = D3DXVec2Dot( &basevec, &nvec );

	if( dot <= -1.0f )
		dot = -1.0f;
	else if( dot >= 1.0f )
		dot = 1.0f;

	float retrad;
	if( dot >= 0.999999619f ){
		retrad = 0.0f;
	}else if( dot <= -0.999999619f ){
		retrad = (float)PI;
	}else{
		retrad = (float)acos( dot );
	}

//	rad = (float)acos( dot );
	float ccw;
	ccw = D3DXVec2CCW( &basevec, &nvec );

	float roty;
	if( ccw >= 0.0f ){
		roty = -retrad;
	}else{
		roty = retrad;
	}

	set1->m_gpe.rot.x = 0.0f;
	set1->m_gpe.rot.y = roty * (float)PAI2DEG;
	set1->m_gpe.rot.z = 0.0f;

	set1->CalcMatWorld();
	

	return 0;
}

int DirToTheOther(  CGPData* set1, CGPData set2 )
{
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 vec;
	
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = -1.0f;

	vec = set2.m_gpe.pos - set1->m_gpe.pos;
	D3DXVec3Normalize( &vec, &vec );

	D3DXVECTOR3 axis;

	D3DXVec3Cross( &axis, &basevec, &vec );
	D3DXVec3Normalize( &axis, &axis );

	float dot;
	dot = D3DXVec3Dot( &basevec, &vec );
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;

	float retrad;
	if( dot >= 0.999999619f ){
		retrad = 0.0f;
	}else if( dot <= -0.999999619f ){
		retrad = (float)PI;
	}else{
		retrad = (float)acos( dot );
	}

//	rad = (float)acos( dot );

	CQuaternion q0, q1;
	q0.SetAxisAndRot( axis, retrad );
	q1.SetAxisAndRot( axis, -retrad );

	D3DXVECTOR3 testvec0, testvec1;
	q0.Rotate( &testvec0, basevec );
	q1.Rotate( &testvec1, basevec );

	float testdot0, testdot1;
	testdot0 = D3DXVec3Dot( &vec, &testvec0 );
	testdot1 = D3DXVec3Dot( &vec, &testvec1 );

	CQuaternion q;
	if( testdot0 >= testdot1 )
		q = q0;
	else
		q = q1;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &q, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &set1->m_gpe.rot );
	_ASSERT( !ret );
	set1->m_gpe.rot = neweul;

	set1->CalcMatWorld();

	return 0;
}

int E3DDirToTheOtherXZ()
{
	// 1 : change handlerset id
	// 2 : src handlerset id

	int chghsid;
	chghsid = code_getdi(0);

	int srchsid;
	srchsid = code_getdi(0);

////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DDirToTheOtherXZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DDirToTheOtherXZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = DirToTheOtherXZ( &(chghs->m_gpd), srchs->m_gpd );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DDirToTheOther()
{
	// 1 : change handlerset id
	// 2 : src handlerset id
	int chghsid;
	chghsid = code_getdi(0);

	int srchsid;
	srchsid = code_getdi(0);

////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DDirToTheOther : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DDirToTheOther : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = DirToTheOther( &(chghs->m_gpd), srchs->m_gpd );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSeparateFrom()
{
	// 1 : chg handlerset id
	// 2 : src handlerset id

	int chghsid;
	chghsid = code_getdi(0);

	int srchsid;
	srchsid = code_getdi(0);

	float dist;
	dist = (float)code_getdd(100.0);

////////

	int ret;
	ret = SeparateFrom( chghsid, srchsid, dist );
	_ASSERT( !ret );

	return 0;
}

int SeparateFrom( int hsid1, int hsid2, float dist ){

	CHandlerSet* chghs = GetHandlerSet( hsid1 );
	if( !chghs ){
		DbgOut( "E3D SeparateFrom : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( hsid2 );
	if( !srchs ){
		DbgOut( "E3D SeparateFrom : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;

	vec.x = srchs->m_gpd.m_gpe.pos.x - chghs->m_gpd.m_gpe.pos.x;
	vec.y = srchs->m_gpd.m_gpe.pos.z - chghs->m_gpd.m_gpe.pos.z;

	D3DXVec2Normalize( &nvec, &vec );
	chghs->m_gpd.m_gpe.pos.x -= dist * nvec.x;
	chghs->m_gpd.m_gpe.pos.z -= dist * nvec.y;

	chghs->m_gpd.CalcMatWorld();


	return 0;
}

int E3DGetCameraPos()
{

	// 1 : posxptr
	// 2 : posyptr
	// 3 : poszptr

	PVal *pvalposx;
	APTR aptrposx;
	aptrposx = code_getva( &pvalposx );

	PVal *pvalposy;
	APTR aptrposy;
	aptrposy = code_getva( &pvalposy );

	PVal *pvalposz;
	APTR aptrposz;
	aptrposz = code_getva( &pvalposz );
////////
	
	double dposx, dposy, dposz;
	dposx = (double)s_camerapos.x;
	dposy = (double)s_camerapos.y;
	dposz = (double)s_camerapos.z;


	code_setva( pvalposx, aptrposx, TYPE_DNUM, &dposx );
	code_setva( pvalposy, aptrposy, TYPE_DNUM, &dposy );
	code_setva( pvalposz, aptrposz, TYPE_DNUM, &dposz );

	return 0;
}

int E3DSetCameraPos()
{
	// 1 : posxptr
	// 2 : posyptr
	// 3 : poszptr
	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);
////////

	s_camerapos.x = posx;
	s_camerapos.y = posy;
	s_camerapos.z = posz;

	return 0;
}

int E3DGetCameraDeg()
{

	// 1 : degxzptr
	// 2 : degyptr
	
	PVal *pvaldegxz;
	APTR aptrdegxz;
	aptrdegxz = code_getva( &pvaldegxz );

	PVal *pvaldegy;
	APTR aptrdegy;
	aptrdegy = code_getva( &pvaldegy );

////////
	double ddegxz, ddegy;
	ddegxz = (double)s_camera_degxz;
	ddegy = (double)s_camera_degy;

	code_setva( pvaldegxz, aptrdegxz, TYPE_DNUM, &ddegxz );
	code_setva( pvaldegy, aptrdegy, TYPE_DNUM, &ddegy );

	return 0;
}

int E3DSetCameraDeg()
{
	// 1 : degxzptr
	// 2 : degyptr
	float degxz;
	degxz = (float)code_getdd(0.0);

	float degy;
	degy = (float)code_getdd(0.0);

////////

	s_camera_degxz = degxz;
	s_camera_degy = degy;

	s_cameratype = 0;
	
	return 0;
}
int E3DSetCameraTarget()
{
	// 1 : targetxptr
	// 2 : targetyptr
	// 3 : targetzptr
	// 4 : upvecxptr
	// 5 : upvecyptr
	// 6 : upveczptr
	
	float targetx;
	targetx = (float)code_getdd(0.0);

	float targety;
	targety = (float)code_getdd(0.0);

	float targetz;
	targetz = (float)code_getdd(0.0);


	float upvecx;
	upvecx = (float)code_getdd(0.0);

	float upvecy;
	upvecy = (float)code_getdd(1.0);

	float upvecz;
	upvecz = (float)code_getdd(0.0);

////////

	s_cameratarget.x = targetx;
	s_cameratarget.y = targety;
	s_cameratarget.z = targetz;

	s_cameraupvec.x = upvecx;//�ꎞ�I�ɁB��Ńc�C�X�g����B
	s_cameraupvec.y = upvecy;
	s_cameraupvec.z = upvecz;

	D3DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );

	s_cameratype = 1;


	CalcMatView();//�c�C�X�g�O��matView


// twist������s_cameraupvec�����߂�B
	D3DXMATRIX tempmatView;
	float cx, cy, cz;

	tempmatView = s_matView;
	cx = tempmatView._41;
	cy = tempmatView._42;
	cz = tempmatView._43;
	tempmatView._41 = 0.0f;
	tempmatView._42 = 0.0f;
	tempmatView._43 = 0.0f;



	D3DXVECTOR3 axisvec;
	CQuaternion2 addrot;
	axisvec = s_cameratarget - s_camerapos;
	DXVec3Normalize( &axisvec, &axisvec );
	addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
	
	
	D3DXQUATERNION viewqx;
	D3DXQuaternionRotationMatrix( &viewqx, &tempmatView );

	CQuaternion2 viewq;
	viewq.x = viewqx.x;
	viewq.y = viewqx.y;
	viewq.z = viewqx.z;
	viewq.w = viewqx.w;
	
	CQuaternion2 invview;
	viewq.inv( &invview );

	CQuaternion2 multq;
	multq = addrot * invview;

	D3DXVECTOR3 upv( 0.0f, 1.0f, 0.0f );
	multq.Rotate( &s_cameraupvec, upv );

	DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );


	return 0;
}

CAviData* GetAviData( int aviid )
{
	CAviData* curavi = avihead;
	while( curavi ){
		if( curavi->serialno == aviid ) 
			return curavi; // find
		curavi = curavi->next;
	}
	return 0;
}

CXFont* GetXFont( int fontid )
{
	CXFont* curfont = xfonthead;
	while( curfont ){
		if( curfont->m_serialno == fontid ) 
			return curfont; // find
		curfont = curfont->next;
	}
	return 0;
}

CNaviLine* GetNaviLine( int nlid )
{
	CNaviLine* curnl = g_nlhead;
	while( curnl ){
		if( curnl->lineid == nlid ){
			return curnl; // find !!!
		}
		curnl = curnl->next;
	}

	return 0; // not found
}

int E3DChkInView()
{
	int scid;
	scid = code_getdi( 0 );
	if( scid < 0 ){
		DbgOut( "E3DChkInView : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int hsid;
	hsid = code_getdi(0);

////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkInView : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DChkInView : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int retflag = 0;
	//ret = curhs->ChkInView( s_camerapos, viewvec, (float)g_proj_near, (float)g_proj_far, (float)g_proj_fov * fDeg2Pai );
	ret = curhs->ChkInView( s_matView, &retflag, 0 );
	if( ret ){
		DbgOut( "E3DRender : ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( retflag != 0 ){
		stat = 1;
	}else{
		stat = 0;
	}
	return 0;
}

int E3DEnableDbgFile()
{

	OpenDbgFile();	
	return 0;

}

int E3DSetProjection()
{
	// 1 : nearptr
	// 2 : farptr
	// 3 : fovptr

	float nearclip;
	nearclip = (float)code_getdd(0.0);

	float farclip;
	farclip = (float)code_getdd(0.0);

	float fov;
	fov = (float)code_getdd(0.0);

////////
	g_ortho = 0;
	g_proj_near = nearclip;
	g_proj_far = farclip;
	g_proj_fov = fov;

	if( g_proj_fov == 0.0f )
		g_proj_fov = 45.0f; //default�l
	
	if( g_proj_fov < 0.0f )
		g_proj_fov *= -1.0f;

	if( g_proj_fov >= 180.0f )
		g_proj_fov = 179.0f; // fov / 2 ���@90�ȏ�ɂȂ�Ȃ��悤�ɂ���B90�͔��U����̂ŋ֎~�B

	if( g_proj_near >= g_proj_far ){
		g_proj_near = 100.0f; //default
		g_proj_far = 10000.0f;//default
	}

	int ret;
	ret = SetFovIndex();
	_ASSERT( !ret );

//// fog
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );

	g_fogend = g_proj_far;
	g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
	if( pdev ){
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
	}

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
		_ASSERT( !ret );
	}


	return 0;
}

int E3DGetKeyboardState()
{
	//type $202

	PVal *pvalstate;
	APTR aptrstate;
	aptrstate = code_getva( &pvalstate );

////
/////////
/////////
	unsigned char tempstate[256];


	GetKeyboardState( tempstate );

	int cno;
	for( cno = 0; cno < 256; cno++ ){
		if( tempstate[cno] & 0x80 ){
			//*( stateptr + cno ) = 1;
			int setval = 1;
			code_setva( pvalstate, aptrstate + cno, TYPE_INUM, &setval );
		}else{
			//*( stateptr + cno ) = 0;
			int setval = 0;
			code_setva( pvalstate, aptrstate + cno, TYPE_INUM, &setval );
		}
	}

	return 0;
}

int E3DGetKeyboardCnt()
{
	PVal *pvalstate;
	APTR aptrstate;
	aptrstate = code_getva( &pvalstate );

////
/////////
/////////
	unsigned char tempstate[256];
	GetKeyboardState( tempstate );


	int cno;
	for( cno = 0; cno < 256; cno++ ){
		if( tempstate[cno] & 0x80 ){
			//*( stateptr + cno ) = 1;
			(s_keyboardcnt[cno])++;
			int setval = s_keyboardcnt[cno];
			code_setva( pvalstate, aptrstate + cno, TYPE_INUM, &setval );
		}else{
			//*( stateptr + cno ) = 0;
			int setval = 0;
			s_keyboardcnt[cno] = 0;
			code_setva( pvalstate, aptrstate + cno, TYPE_INUM, &setval );
		}
	}

	return 0;
}
int E3DResetKeyboardCnt()
{

	ZeroMemory( s_keyboardcnt, sizeof( int ) * 256 );

	return 0;
}



int GetName( char* dstname, int srcleng )
{
	int chk;
	chk = code_getprm();
	if ( chk<=PARAM_END ) {
		DbgOut( "GetName : parameter is not set error !!!\n" );
		_ASSERT( 0 );
		return 1;										// �p�����[�^�[�ȗ����̏���
	}

	char* nameptr;
	//PVal *pvalname;
	//APTR aptrname;
	//aptrname = code_getva( &pvalname );

	//DbgOut( "GetName : mpval->mode %d\r\n", mpval->mode );

	if( mpval->flag != HSPVAR_FLAG_STR ){
		DbgOut( "GetName : this parameter is not str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mpval->offset == 0 ){
		nameptr = (char*)mpval->pt;
	}else{
		nameptr = *((char**)mpval->master + mpval->offset);
	}

	if( !nameptr ){
		DbgOut( "GetName : nameptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)strlen( nameptr );
	if( leng >= srcleng ){
		DbgOut( "str size too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( dstname, nameptr );

	return 0;
}

int E3DCos()
{
	// 1 : deg
	// 3 : retptr

	float deg;
	deg = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	float rad = deg * (float)DEG2PAI;
	
	double temp;
	temp = cosf( rad );

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );


	return 0;

}

int E3DSin()
{
	// type $202
	// 1 : deg
	// 3 : retptr

	float deg;
	deg = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

/////////

	float rad = deg * (float)DEG2PAI;
	double temp;
	temp = sinf( rad );

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}


int E3DACos()
{
	// type $202
	// 1 : deg
	// 3 : retptr
	float dot;
	dot = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
////////

	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;

	float rad;
	rad = (float)acos( dot );

	double degree;
	degree = rad * (float)PAI2DEG;

	code_setva( pvalret, aptrret, TYPE_DNUM, &degree );

	return 0;

}



int E3DDot()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);


	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	DXVec3Normalize( &vec1, &vec1 );
	DXVec3Normalize( &vec2, &vec2 );

	double dot;
	dot = (double)D3DXVec3Dot( &vec1, &vec2 );

	code_setva( pvalret, aptrret, TYPE_DNUM, &dot );

	return 0;
}

int E3DDot2()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);


	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	//DXVec3Normalize( &vec1, &vec1 );
	//DXVec3Normalize( &vec2, &vec2 );

	double dot;
	dot = (double)D3DXVec3Dot( &vec1, &vec2 );

	code_setva( pvalret, aptrret, TYPE_DNUM, &dot );

	return 0;
}



int E3DCross()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retxptr
	// 9 : retyptr
	// 10 : retzptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);

	PVal *pvalretx;
	APTR aptrretx;
	aptrretx = code_getva( &pvalretx );

	PVal *pvalrety;
	APTR aptrrety;
	aptrrety = code_getva( &pvalrety );

	PVal *pvalretz;
	APTR aptrretz;
	aptrretz = code_getva( &pvalretz );


////////

	//�I�I�I�{���́A�O�ςƂ́A�قȂ�B�I�I�I
	// vec1, vec2�̗����ɐ����ŁA�傫����*multptr�ȃx�N�g�����v�Z����B

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	DXVec3Normalize( &vec1, &vec1 );
	DXVec3Normalize( &vec2, &vec2 );

	D3DXVECTOR3 crossvec;
	D3DXVec3Cross( &crossvec, &vec1, &vec2 );

	double dx, dy, dz;
	dx = (double)crossvec.x;
	dy = (double)crossvec.y;
	dz = (double)crossvec.z;

	code_setva( pvalretx, aptrretx, TYPE_DNUM, &dx );
	code_setva( pvalrety, aptrrety, TYPE_DNUM, &dy );
	code_setva( pvalretz, aptrretz, TYPE_DNUM, &dz );


	return 0;
}

int E3DAtan()
{
	// 1 : valptr
	// 3 : retptr

	float val;
	val = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );


////////
	float retrad;
	retrad = (float)atan( val );

	double temp;
	temp = retrad * (float)PAI2DEG;

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}

int E3DAtan2()
{
	// 1 : yptr
	// 2 : xptr
	// 3 : retptr
	float y;
	y = (float)code_getdd(0.0);

	float x;
	x = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
	
////////

	float retrad;
	retrad = (float)atan2( y, x );

	double temp;
	temp = retrad * (float)PAI2DEG;
	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}

int E3DSqrt()
{
	// 1 : valptr
	// 3 : retptr

	float val;
	val = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	//E3DSqrt val, mult, ret
	//	sqrt( val ) * mult
	double temp;	
	if( val <= 0.0f ){
		temp = 0.0;
	}else{
		temp = sqrt( val );
	}
	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );


	return 0;

}

int E3DDrawText()
{
	// type $202
	// 1 : posxptr
	// 2 : posyptr
	
	// 3 : scaleptr

	// 4 : rptr
	// 5 : gptr
	// 6 : bptr

	// 7 : strptr


	int posx;
	posx = code_getdi(0);

	int posy;
	posy = code_getdi(0);

	float scale;
	scale = (float)code_getdd(0.0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

	int ret;
	char tempname[2049];
	ret = GetName( tempname, 2049 );
	if( ret ){
		DbgOut( "E3DDrawText : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFont ){
		_ASSERT( 0 );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	(g_pD3DApp->m_pFont)->m_fTextScale = scale;
	(g_pD3DApp->m_pFont)->DrawText( (float)posx, (float)posy, D3DCOLOR_ARGB(255, r, g, b), tempname );
	
	return 0;
}

int E3DDrawBigText()
{
	// 1 : posxptr
	// 2 : posyptr
	
	// 3 : scaleptr

	// 4 : rptr
	// 5 : gptr
	// 6 : bptr

	// 7 : strptr


	int posx;
	posx = code_getdi(0);

	int posy;
	posy = code_getdi(0);



	float scale;
	scale = (float)code_getdd(0.0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

	int ret;
	char tempname[2049];
	ret = GetName( tempname, 2049 );
	if( ret ){
		DbgOut( "E3DDrawBigText : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}
	
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFontBig ){
		_ASSERT( 0 );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	(g_pD3DApp->m_pFontBig)->m_fTextScale = scale;
	(g_pD3DApp->m_pFontBig)->DrawText( (float)posx, (float)posy, D3DCOLOR_ARGB(255, r, g, b), tempname );
	
	return 0;
}

int E3DRand()
{
	// 1 : rangeptr
	// 2 : retptr
	int range;
	range = code_getdi(0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
////////
			
	int tempval;
	tempval = rand();

	int rangeval;
	rangeval = range;
	if( rangeval < 0 ){
		rangeval *= -1;
	}else if( rangeval == 0 ){
		rangeval = 1;
	}

	int temp;
	temp = tempval % rangeval;

	code_setva( pvalret, aptrret, TYPE_INUM, &temp );

	return 0;
}

int E3DSeed()
{
	// type $202
	// 1 : seedptr

	int seed;
	seed = code_getdi(0);


////////


	srand( (unsigned)seed );


	return 0;
}

int E3DGetFPS()
{

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

/////////
	static DWORD beftime = 0;
	DWORD curtime;
	DWORD difftime;

	static LARGE_INTEGER lbeftime = {0, 0};
	LARGE_INTEGER lcurtime;
	LONGLONG lldifftime;
	
	DWORD retfps;

	if( s_timerfreq != 0 ){
		QueryPerformanceCounter( &lcurtime );

		//HighPart �́@diff �́A�O�Ɖ���I�I�I
		lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		
		retfps = (DWORD)( 1.0 / ((float)lldifftime / (float)s_timerfreq) );

		lbeftime.LowPart = lcurtime.LowPart;
		lbeftime.HighPart = lcurtime.HighPart;
	}else{
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );

		difftime = curtime - beftime;

		retfps = (DWORD)( 1.0 / ((float)difftime / 1000.0) );
		
		beftime = curtime;
	}

	code_setva( pvalret, aptrret, TYPE_INUM, &retfps );

	return 0;

}

int E3DWaitbyFPS()
{
	int srcfps;
	srcfps = code_getdi(0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );


//////////
	DWORD srcmsec;
	if( srcfps <= 0 ){
		srcmsec = 0;
	}else{
		srcmsec = 1000 / srcfps;
	}

	DWORD curmsec = 0;

	static DWORD beftime = 0;
	DWORD curtime;
	DWORD difftime = 0;

	static LARGE_INTEGER lbeftime = {0, 0};
	LARGE_INTEGER lcurtime;
	LONGLONG lldifftime = 0;
	
	int retfps;


	if( s_timerfreq != 0 ){

		QueryPerformanceCounter( &lcurtime );

		//HighPart �́@diff �́A�O�Ɖ���I�I�I
		if( lcurtime.QuadPart > lbeftime.QuadPart )
			lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		else
			lldifftime = 0xFFFFFFFF + lcurtime.QuadPart - lbeftime.QuadPart;
			//lldifftime = s_timerfreq * 1000;
			//lldifftime = lbeftime.QuadPart;

		if( lldifftime < 0 ){
			lldifftime = 0;
		}

		curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);

		if( srcmsec >= curmsec ){
			DWORD sleepmsec;
			sleepmsec = srcmsec - curmsec;
			if( sleepmsec > 1000 ){
				sleepmsec = 1000;
			}
			timeBeginPeriod( 1 );
			//::MsgWaitForMultipleObjects( 0, NULL, FALSE, sleepmsec, 0 );
			Sleep( sleepmsec );
			timeEndPeriod( 1 );

			/***
			QueryPerformanceCounter( &lcurtime );
			//HighPart �́@diff �́A�O�Ɖ���I�I�I
			if( lcurtime.QuadPart > lbeftime.QuadPart )
				lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
			else
				lldifftime = lbeftime.QuadPart;
			curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);
			***/
		}

//////////
		QueryPerformanceCounter( &lcurtime );

		//HighPart �́@diff �́A�O�Ɖ���I�I�I
		if( lcurtime.QuadPart > lbeftime.QuadPart )
			lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		else
			lldifftime = s_timerfreq * 1000;
			//lldifftime = lbeftime.QuadPart;

		curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);
		if( lldifftime != 0 )
			retfps = (int)( 1.0 / ((float)lldifftime / (float)s_timerfreq) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : qpc : retfps %d\n", retfps );		

		lbeftime.LowPart = lcurtime.LowPart;
		lbeftime.HighPart = lcurtime.HighPart;

	}else{
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );

		if( curtime > beftime )
			difftime = curtime - beftime;
		else
			difftime = 1000;
			//difftime = beftime;
		
		if( srcmsec >= difftime ){
			DWORD sleepmsec;
			sleepmsec = srcmsec - difftime;
			if( sleepmsec > 1000 ){
				sleepmsec = 1000;
			}
			timeBeginPeriod( 1 );
			//::MsgWaitForMultipleObjects( 0, NULL, FALSE, sleepmsec, 0 );
			Sleep( sleepmsec );
			curtime = timeGetTime();
			timeEndPeriod( 1 );

			/***
			if( curtime > beftime )
				difftime = curtime - beftime;
			else
				difftime = beftime;
			***/
		}
//////////////
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );
		
		if( curtime > beftime )
			difftime = curtime - beftime;
		else
			difftime = 1000;
			//difftime = beftime;


		if( difftime != 0 )
			retfps = (int)( 1.0 / ((float)difftime / 1000.0) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : tgt : retfps %d\n", retfps );		

		beftime = curtime;
	}

	code_setva( pvalret, aptrret, TYPE_INUM, &retfps );
	return 0;

}

int E3DCreateLight()
{

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );


//////
	
	int lid = -1;
	CLightData* newlight = 0;

	newlight = new CLightData( &lid );
	if( !newlight ){
		DbgOut( "E3DCreateLight : newlight alloc error !!!\n" );
		return 1;
	}
	

//	newlight->nextlight = g_lighthead;
//	g_lighthead = newlight;


	//�Ō�ɒǉ�����B
	CLightData* curl = g_lighthead;
	CLightData* befl = 0;

	while( curl ){
		befl = curl;
		curl = curl->nextlight;
	}

	if( befl ){
		befl->nextlight = newlight;
	}else{
		_ASSERT( !g_lighthead );
		g_lighthead = newlight;
	}

	code_setva( pvalret, aptrret, TYPE_INUM, &lid );

	return 0;
}

CLightData* GetLightData( int lid )
{
	CLightData* retlight = 0;
	CLightData* curlight = g_lighthead;

	while( curlight ){
		if( curlight->lightid == lid ){
			retlight = curlight;
			break;
		}
		curlight = curlight->nextlight;
	}

	return retlight;
}

CMySprite* GetSprite( int spriteid )
{
	CMySprite* retsprite = 0;
	CMySprite* cursprite = s_spritehead;

	while( cursprite ){
		if( cursprite->serialno == spriteid ){
			retsprite = cursprite;
			break;
		}
		cursprite = cursprite->next;
	}

	return retsprite;
}



int E3DSetDirectionalLight()
{
	// 1 : light id

	// 2 : dir.x
	// 3 : dir.y
	// 4 : dir.z
	
	// 5 : r
	// 6 : g
	// 7 : b


	int lid;
	lid = code_getdi(0);

	float dirx;
	dirx = (float)code_getdd(0.0);

	float diry;
	diry = (float)code_getdd(0.0);

	float dirz;
	dirz = (float)code_getdd(0.0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);
	
///////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetDirectionalLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 dir;
	dir.x = dirx;
	dir.y = diry;
	dir.z = dirz;
	
	DXVec3Normalize( &dir, &dir );

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetDirectionalLight( dir, difcol );
	if( ret ){
		_ASSERT( !ret );
		return 1;
	}
////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_DIRECTIONAL;

    light.Direction = dir;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


	//DbgOut( "e3dhsp : SetDirectionalLight : lid %d, col %f %f %f, dir %f %f %f\n",
	//	lid, difcol.x, difcol.y, difcol.z, dir.x, dir.y, dir.z );

	return 0;
}


int E3DSetPointLight()
{
	// type $202

	int lid;
	lid = code_getdi(0);

	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);

	float dist;
	dist = (float)code_getdd(0.0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);


///////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetPointLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 pos;
	pos.x = posx;
	pos.y = posy;
	pos.z = posz;
	

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetPointLight( pos, dist, difcol );
	if( ret ){
		_ASSERT( 0 );
		DbgOut( "E3DSetPointLight : SetPointLight error !!!\n" );
		return 1;
	}

////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_POINT;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

	light.Position = pos;

	light.Range = dist;

	light.Attenuation0 = 0.4f;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


//DbgOut( "e3dhsp : SetPointLight : pointlight : col %f %f %f, pos %f %f %f, range %f\n",
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b,
//	   light.Position.x, light.Position.y, light.Position.z,
//	   light.Range );


	return 0;
}

int E3DSetSpotLight()
{
	// type $202

	int lid;
	lid = code_getdi(0);

	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);

	float dirx;
	dirx = (float)code_getdd(0.0);

	float diry;
	diry = (float)code_getdd(0.0);

	float dirz;
	dirz = (float)code_getdd(0.0);


	float dist;
	dist = (float)code_getdd(0.0);

	float angle;
	angle = (float)code_getdd(0.0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);



///////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetPointLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 pos;
	pos.x = posx;
	pos.y = posy;
	pos.z = posz;
	
	D3DXVECTOR3 dir;
	dir.x = dirx;
	dir.y = diry;
	dir.z = dirz;

	DXVec3Normalize( &dir, &dir );


	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetSpotLight( pos, dir, dist, angle, difcol );
	if( ret ){
		DbgOut( "E3DSetSpotLight : SetSpotLight error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////


	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_SPOT;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

	light.Position = pos;

	light.Direction = dir;

	light.Range = dist;

	light.Falloff = 1.0f;

    light.Attenuation0 = 1.0f;
    
	light.Theta        = angle * (float)DEG2PAI;
    light.Phi          = angle * (float)DEG2PAI;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


	return 0;
}


int E3DDestroyLight()
{

	int lid;
	lid = code_getdi(0);

///////
	
	CLightData* beflight = 0;
	CLightData* curlight = g_lighthead;
	CLightData* findlight = 0;

	while( curlight ){
		if( curlight->lightid == lid ){
			findlight = curlight;
			break;
		}
		beflight = curlight;
		curlight = curlight->nextlight;
	}

	if( findlight ){
		if( beflight ){
			beflight->nextlight = findlight->nextlight;
			delete findlight;
		}else{
			g_lighthead = findlight->nextlight;
			delete findlight;
		}

		int ret;
		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		if( ret || !pdev ){
			DbgOut( "e3dhsp : E3DDestroyLight : GetD3DDevice error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pdev->LightEnable( lid, FALSE );
	
	}else{
		return 0;
	}

	return 0;
}

int E3DClearZ()
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}
	
    // Clear Z buffer
	HRESULT hres;
	hres = pdev->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 
                         0,
						 //0,
						 1.0f, 0L );
   if( hres != D3D_OK ){
		//_ASSERT( 0 );
	   DbgOut( "E3DClearZ : Clear error !!!\n" );
		return 1;
   }

	return 0;
}

int E3DDestroyHandlerSet()
{
	// 1 : handlerset id

	int hsid;
	hsid = code_getdi(0);


///////

	int ret;
	ret = DestroyHandlerSet( hsid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyHandlerSet : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int DestroyHandlerSet( int hsid )
{

///// texture�̔j��

//	EnterCriticalSection( &g_crit_restore );//######## start crit
	
	int ret;
	ret = g_texbnk->ResetDirtyFlag();

	ret = DestroyTexture( hsid, -2, -2, 1, -2 );
	if( ret ){
		DbgOut( "DestroyHandlerSet : DestroyTexture error !!!\n" );
		_ASSERT( 0 );
		//return 1;
	}

//	LeaveCriticalSection( &g_crit_restore );//###### end crit


/////////////
	EnterCriticalSection( &g_crit_hshead );//######## start crit

	
	if( (hsid < 0) || (hsid >= HSIDMAX) ){
		DbgOut( "DestroyHandlerSet : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_hsidelem[hsid].validflag = 0;
	if( s_hsidelem[hsid].hsptr ){
		delete s_hsidelem[hsid].hsptr;
		s_hsidelem[hsid].hsptr = 0;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	return 0;
}

int DestroyTexture( int hsid, int bbid, int spriteid, int dirtyreset, int bgno )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//bbid == -1 �a�a�S�č폜�Abbid == -2 �a�a�͍폜���Ȃ�
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	int ret;

	CHandlerSet* delhs = 0;
	if( hsid > 0 ){
		delhs = GetHandlerSet( hsid );
		_ASSERT( delhs );
	}else{
		delhs = 0;
	}

	CMySprite* delsprite = 0;
	if( spriteid >= 0 ){
		delsprite = GetSprite( spriteid );
		_ASSERT( delsprite );
	}else{
		delsprite = 0;
	}

	BGDISPELEM* delbgelem = 0;
	if( bgno > 0 ){
		delbgelem = GetBGDispElem( bgno );
	}else{
		delbgelem = 0;
	}

//////////

EnterCriticalSection( &g_crit_restore );//######## start crit
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		BGDISPELEM* curbgelem;
		curbgelem = &(s_bgdisp[scno]);
		if( (curbgelem->scid >= 0) && (curbgelem->bgdisp) && (curbgelem != delbgelem) ){
			ret = curbgelem->bgdisp->SetTextureDirtyFlag();
			_ASSERT( !ret );
		}
	}

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			if( delhs != s_hsidelem[hsidno].hsptr ){
				ret = s_hsidelem[hsidno].hsptr->SetTextureDirtyFlag( -999999 );//�S�Ă�dirtyflag�Z�b�g�@
				_ASSERT( !ret );
			}
		}
	}


	if( g_bbhs ){
		if( bbid == -2 ){
			ret = g_bbhs->SetTextureDirtyFlag( -999999 );//�S�Ă�dirtyflag�Z�b�g
			_ASSERT( !ret );
		}else if( bbid == -1 ){
			//�������Ȃ�
		}else if( bbid >= 0 ){
			//ret = g_bbhs->SetTextureDirtyFlag( bbid );       //----> bug ---> //bbid�ɂ���dirtyflag�Z�b�g
															

			ret = g_bbhs->SetTextureDirtyFlag( -bbid );// bbid�ȊO�ɁAdirtyflag�Z�b�g


			_ASSERT( !ret );
		}
	}


//DbgOut( "!!! DestroyTexture : spriteid %d\r\n", spriteid );


	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		if( cursprite != delsprite ){
			ret = cursprite->SetTextureDirtyFlag();

//DbgOut( "!!! DestroyTexture : SetTextureDirtyFlag %d\r\n", cursprite->serialno );			
		}
		cursprite = cursprite->next;
	}

	//////////


	ret = g_texbnk->DestroyNotDirtyTexture( 1 );
	_ASSERT( !ret );


	if( dirtyreset ){
		if( g_texbnk ){
			g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
		}
	}

LeaveCriticalSection( &g_crit_restore );//###### end crit


	return 0;
}

int E3DSetDispSwitch()
{
	int hsid;
	hsid = code_getdi(0);

	int mk;
	mk = code_getdi(0);

	int swid;
	swid = code_getdi(0);

	int srcframeno;
	srcframeno = code_getdi(0);

	int swflag;
	swflag = code_getdi(0);
	
/***
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDispSwitch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SetDispSwitch( mk, swid, srcframeno, swflag );
	if( ret ){
		DbgOut( "E3DSetDispSwitch : SetDispSwitch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	return 0;
}

int E3DSetDispSwitch2()
{
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int mk;
	mk = code_getdi(0);

	int swid;
	swid = code_getdi(0);

	int srcframeno;
	srcframeno = code_getdi(0);

	int swflag;
	swflag = code_getdi(0);
	

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDispSwitch2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SetDispSwitch2( boneno, mk, swid, srcframeno, swflag );
	if( ret ){
		DbgOut( "E3DSetDispSwitch2 : SetDispSwitch2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMotionFrameNo()
{
	int hsid;
	hsid = code_getdi(0);

	int mk;
	mk = code_getdi(0);

	int srcframeno;
	srcframeno = code_getdi(0);


	
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionFrameNo( mk, srcframeno );
	if( ret ){
		DbgOut( "E3DSetMotionFrameNo : SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DCreateSprite()
{
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateSprite : file name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : transparent
	int transparentflag;
	transparentflag = code_getdi(0);

//3 : spriteid
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


//4 : tpr
	int tpr = code_getdi(0);
//5 : tpg
	int tpg = code_getdi(0);
//6 : tpb
	int tpb = code_getdi(0);


////////
	int spriteid;

	CMySprite* newsprite;
	newsprite = new CMySprite( &spriteid );
	if( !newsprite ){
		DbgOut( "E3DCreateSprite : newsprite alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newsprite->next = s_spritehead;
	s_spritehead = newsprite;


	ret = newsprite->SetSpriteParams( tempname, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
	if( ret ){
		DbgOut( "E3DCreateSprite : newsprite SetSpriteParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newsprite->RestoreTexture( pdev );
	if( ret ){
		DbgOut( "E3DCreateSprite : newsprite RestoreTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	code_setva( pvalid, aptrid, TYPE_INUM, &spriteid );

	return 0;
}

int E3DCreateSpriteFromBMSCR()
{

//1 : wid
	int wid;
	wid = code_getdi(0);

//2 : transparent
	int transparentflag;
	transparentflag = code_getdi(0);

//3 : spriteid
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


//4 : tpr
	int tpr = code_getdi(0);
//5 : tpg
	int tpg = code_getdi(0);
//6 : tpb
	int tpb = code_getdi(0);


////////
	int ret;
	int spriteid;

	CMySprite* newsprite;
	newsprite = new CMySprite( &spriteid );
	if( !newsprite ){
		DbgOut( "E3DCreateSpriteFromBMSCR : newsprite alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newsprite->next = s_spritehead;
	s_spritehead = newsprite;


	BMSCR* bm;
	bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
	if( !bm ){
		DbgOut( "E3DCreateSpriteFromBMSCR : bmscr NULL : window id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = newsprite->SetSpriteParams2( bm->pbi, bm->pBit, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
	if( ret ){
		DbgOut( "E3DCreateSpriteFromBMSCR : newsprite SetSpriteParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newsprite->RestoreTexture( pdev );
	if( ret ){
		DbgOut( "E3DCreateSpriteFromBMSCR : newsprite RestoreTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	code_setva( pvalid, aptrid, TYPE_INUM, &spriteid );

	return 0;
}


int E3DBeginSprite()
{

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !s_d3dxsprite ){
		DbgOut( "E3DBeginSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	HRESULT hr;
	//hr = s_d3dxsprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT );
	hr = s_d3dxsprite->Begin( D3DXSPRITE_ALPHABLEND );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	
	DisableFogState();

	return 0;
}
int E3DEndSprite()
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !s_d3dxsprite ){
		DbgOut( "E3DEndSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	hr = s_d3dxsprite->End();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}
int E3DRenderSprite()
{

	int spid;
	spid = code_getdi(0);

	float scalex;
	scalex = (float)code_getdd(0.0);

	float scaley;
	scaley = (float)code_getdd(0.0);

	int trax;
	trax = code_getdi(0);

	int tray;
	tray = code_getdi(0);

	float traz;
	traz = (float)code_getdd(0.0);
	
////

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !s_d3dxsprite ){
		DbgOut( "E3DRenderSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}



	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DRenderSprite : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR2 scale;
	scale.x = scalex; //!!!!!!!
	scale.y = scaley; //!!!!!!!

	D3DXVECTOR3 tra;
	tra.x = (float)trax;
	tra.y = (float)tray;
	tra.z = traz;

	ret = cursprite->Render( pdev, s_d3dxsprite, scale, tra );
	if( ret ){
		DbgOut( "E3DRenderSprite : Render error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DGetSpriteSize()
{

	int spid;
	spid = code_getdi(0);

	PVal *pvalsizex;
	APTR aptrsizex;
	aptrsizex = code_getva( &pvalsizex );


	PVal *pvalsizey;
	APTR aptrsizey;
	aptrsizey = code_getva( &pvalsizey );
////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DGetSpriteSize : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalsizex, aptrsizex, TYPE_INUM, &cursprite->texWidth );
	code_setva( pvalsizey, aptrsizey, TYPE_INUM, &cursprite->texHeight );


	return 0;
}

int E3DSetSpriteRotation()
{

	int spid;
	spid = code_getdi(0);

	float centerx;
	centerx = (float)code_getdd(0.0);

	float centery;
	centery = (float)code_getdd(0.0);

	float rotdeg;
	rotdeg = (float)code_getdd(0.0);

////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteRotation : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR2 cenvec;
	cenvec.x = centerx;
	cenvec.y = centery;

	ret = cursprite->SetRotation( cenvec, rotdeg );
	if( ret ){
		DbgOut( "E3DSetSpriteRotation : sprite SetRotation error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetSpriteARGB()
{

	int spid;
	spid = code_getdi(0);

	int alpha;
	alpha = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);


////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteARGB : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( alpha < 0 ){
		alpha = 0;
	}else if( alpha > 255 ){
		alpha = 255;
	}

	if( r < 0 ){
		r = 0;
	}else if( r > 255 ){
		r = 255;
	}

	if( g < 0 ){
		g = 0;
	}else if( g > 255 ){
		g = 255;
	}

	if( b < 0 ){
		b = 0;
	}else if( b > 255 ){
		b = 255;
	}

	int ret;

	ret = cursprite->SetARGB( alpha, r, g, b );
	if( ret ){
		DbgOut( "E3DSetSpriteARGB : sprite SetARGB error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DDestroySprite()
{

	int spid;
	spid = code_getdi(0);

////////

	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, -2, spid, dirtyreset, -2 );
	_ASSERT( !ret );
///////

//	if( g_texbnk ){
//		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
//	}

////////

	CMySprite* befsprite = 0;
	CMySprite* cursprite = s_spritehead;
	CMySprite* findsprite = 0;

	while( cursprite ){
		if( cursprite->serialno == spid ){
			findsprite = cursprite;
			break;
		}
		befsprite = cursprite;
		cursprite = cursprite->next;
	}

	if( findsprite ){
		if( befsprite ){
			befsprite->next = findsprite->next;
			delete findsprite;
		}else{
			s_spritehead = findsprite->next;
			delete findsprite;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}

/////////////
/***
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	cursprite = s_spritehead;
	while( cursprite ){
		cursprite->RestoreTexture( pdev );
		cursprite = cursprite->next;
	}
***/
	return 0;
}

//E3DChkConfBillboard2 centerx, centery, centerz, srcr, confrate, result
int E3DChkConfBillboard2()
{


// 1 : centerx
	float centerx;
	centerx = (float)code_getdd(0.0);

// 2 : centery
	float centery;
	centery = (float)code_getdd(0.0);

// 3 : centerz
	float centerz;
	centerz = (float)code_getdd(0.0);

// 4 : srcr
	float srcr;
	srcr = (float)code_getdd(0.0);

// 5 : confrate
	float confrate;
	confrate = (float)code_getdd(1.0);

// 6 : confflagptr
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

// 7 : confbbid
	PVal *pvalbbid;
	APTR aptrbbid;
	aptrbbid = code_getva( &pvalbbid );


// 8 : arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);

// 9 : confnumptr
	PVal *pvalconfnum;
	APTR aptrconfnum;
	aptrconfnum = code_getva( &pvalconfnum );


//

////////

	int confflag = 0;
	int confnum = 0;
	int* confbbid;

	confbbid = (int*)malloc( sizeof( int ) * arrayleng );
	if( !confbbid ){
		DbgOut( "E3DChkConfBillboard2 : confbbid alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard2( centerx, centery, centerz, srcr, &confflag, confrate, confbbid, arrayleng, &confnum );
		if( ret ){
			DbgOut( "E3DChkConfBillboard2 : bbhs ChkConfBillboard2 error !!!\n" );
			_ASSERT( 0 );
			free( confbbid );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboard2 : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		free( confbbid );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &confflag );
	code_setva( pvalconfnum, aptrconfnum, TYPE_INUM, &confnum );
	int bbno;
	for( bbno = 0; bbno < confnum; bbno++ ){
		code_setva( pvalbbid, aptrbbid + bbno, TYPE_INUM, confbbid + bbno );
	}

	free( confbbid );

	return 0;
}


//E3DChkConfBillboard hsid, confrate, result, confbbid, arrayleng, confnum
int E3DChkConfBillboard()
{
// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

// 2 : confrate
	float confrate;
	confrate = (float)code_getdd(0.0);

// 3 : confflagptr
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

// 4 : confbbid
	PVal *pvalbbid;
	APTR aptrbbid;
	aptrbbid = code_getva( &pvalbbid );


// 5 : arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);

// 6 : confnumptr
	PVal *pvalconfnum;
	APTR aptrconfnum;
	aptrconfnum = code_getva( &pvalconfnum );


////////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfBillboard : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}


	int confflag = 0;
	int confnum = 0;
	int* confbbid;

	confbbid = (int*)malloc( sizeof( int ) * arrayleng );
	if( !confbbid ){
		DbgOut( "E3DChkConfBillboard2 : confbbid alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard( curhs, &confflag, confrate, confbbid, arrayleng, &confnum );
		if( ret ){
			DbgOut( "E3DChkConfBySphere : bbhs ChkConfBillboard error !!!\n" );
			_ASSERT( 0 );
			free( confbbid );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		free( confbbid );
		return 1;
	}


	code_setva( pvalflag, aptrflag, TYPE_INUM, &confflag );
	code_setva( pvalconfnum, aptrconfnum, TYPE_INUM, &confnum );
	int bbno;
	for( bbno = 0; bbno < confnum; bbno++ ){
		code_setva( pvalbbid, aptrbbid + bbno, TYPE_INUM, confbbid + bbno );
	}

	free( confbbid );


	return 0;
}


int E3DChkConfBySphere()
{
	// 1 : src handlerset id
	// 2 : chk handlerset id
	// (3,4 : rate ��ŁA�g�����邩���H)
	int srchsid;
	srchsid = code_getdi(0);

	int chkhsid;
	chkhsid = code_getdi(0);

	int conflevel;
	conflevel = code_getdi(0);

	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );



////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConfBySphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConfBySphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere( chkhs, conflevel, &confflag );
	if( ret ){
		DbgOut( "E3DChkConfBySphere : srchs ChkConfBySphere error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &confflag );


	return 0;
}

//E3DChkConfBySphere2 hsid1, partno1, hsid2, partno2, confflag
int E3DChkConfBySphere2()
{
	// 1 : src handlerset id
	// 2 : srcpartno
	// 3 : chk handlerset id
	// 4 : chkpartno
	// 5 : confflag

	
	int srchsid;
	srchsid = code_getdi(0);

	int srcpartno;
	srcpartno = code_getdi(0);

	int chkhsid;
	chkhsid = code_getdi(0);

	int chkpartno;
	chkpartno = code_getdi(0);

	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );



////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere2( srcpartno, chkhs, chkpartno, &confflag );
	if( ret ){
		DbgOut( "E3DChkConfBySphere : srchs ChkConfBySphere2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &confflag );

	return 0;
}
int E3DCreateProgressBar()
{
	RECT rcClient;  // Client area of parent window 
    int cyVScroll;  // Height of scroll bar arrow 
 
 
	if( !s_hwnd || !s_inst ){
		DbgOut( "E3DCreateProgressBar : parent wnd NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_PBwnd ){
		DbgOut( "E3DCreateProgressBar : progress bar already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    // Ensure that the common control DLL is loaded
    // and create a progress bar along the bottom of
    // the client area of the parent window. 
    // Base the height of the progress bar on the
    // height of a scroll bar arrow. 
    GetClientRect(s_hwnd, &rcClient); 
    cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 
    s_PBwnd = CreateWindowEx(0, PROGRESS_CLASS,
	         (LPSTR)NULL, WS_CHILD | WS_VISIBLE,
             rcClient.left, rcClient.bottom - cyVScroll,
             rcClient.right - rcClient.left, cyVScroll, 
             s_hwnd, (HMENU) 0, s_inst, NULL); 

	if( !s_PBwnd ){
		_ASSERT( 0 );
		return 1;
	}

    //SendMessage(hwndPB, PBM_SETRANGE, 0,
    //            MAKELPARAM(0, cb / 2048)); 
    //SendMessage(hwndPB, PBM_SETSTEP, (WPARAM) 1, 0); 


    


	return 0;
}

int E3DSetProgressBar()
{
	int newpos;
	newpos = code_getdi(0);
	if( newpos < 0 )
		newpos = 0;
	if( newpos > 100 )
		newpos = 100;

//////////


	if( !s_PBwnd ){
		DbgOut( "E3DSetProgressBar : Progress bar still not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SendMessage( s_PBwnd, PBM_SETPOS, newpos, 0 );


	return 0;
}

int E3DDestroyProgressBar()
{

	if( s_PBwnd ){
		DestroyWindow( s_PBwnd );
		s_PBwnd = 0;
	}

	return 0;
}

int E3DLoadGroundBMP()
{
	//type $202
	// 1 : bmp file name1
	// 2 : bmp file name2
	// 3 : bmp file name3
	// 4 : texture file name
	// 5 : maxx
	// 6 : maxz
	// 7 : divx
	// 8 : divz
	// 9 : maxheight
	// 10 : pointer of handlerset id.

	// 11 : TLmode , ver1036

	int ret;
//1
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : filename 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	char tempname2[MAX_PATH];
	ret = GetName( tempname2, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : filename 2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//3
	char tempname3[MAX_PATH];
	ret = GetName( tempname3, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : filename 3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//4
	char tempname4[MAX_PATH];
	ret = GetName( tempname4, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : filename 4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//5
	float maxx;
	maxx = (float)code_getdd(0.0);
//6
	float maxz;
	maxz = (float)code_getdd(0.0);

//7	
	int divx;
	divx = code_getdi(0);
	if( divx < 2 )
		divx = 2;
//8
	int divz;
	divz = code_getdi(0);
	if( divz < 2 )
		divz = 2;
//9
	float maxheight;
	maxheight = (float)code_getdd(0.0);
	if( maxheight < 0.0f )
		maxheight = 0.0f;

//10	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );


//11
	int TLmode;
	TLmode = code_getdi(2);
	



//////


	int temphsid = -1;
	ret = LoadGroundBMP_F( tempname1, tempname2, tempname3, tempname4, maxx, maxz, divx, divz, maxheight, TLmode, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : LoadGroundBMP_F error !!!\n" );
		_ASSERT( 0 );

		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
	
	
	return 0;
}

int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, float maxx, float maxz, int divx, int divz, float maxheight, int TLmode, int* hsidptr )
{

	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	newhs->m_TLmode = TLmode;//!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadGroundBMP( tempname1, tempname2, tempname3, tempname4, maxx, maxz, divx, divz, maxheight );
	if( ret ){
		DbgOut( "easy3d : E3DCreateHandler : LoadGroundBMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;

	return 0;
}


int E3DCameraPosForward()
{
	float step;
	step = (float)code_getdd(0.0);

//////////
	
	
	float fDeg2Pai = (float)DEG2PAI;
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 dir( 0.0f, 0.0f, 0.0f );

		dir.x = sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		dir.y = sinf( s_camera_degy * fDeg2Pai );
		dir.z = -cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		s_camerapos.x += step * dir.x;
		s_camerapos.y += step * dir.y;
		s_camerapos.z += step * dir.z;

	}else{
		D3DXVECTOR3 dir( 0.0f, 0.0f, 0.0f );
		dir.x = s_cameratarget.x - s_camerapos.x;
		dir.y = s_cameratarget.y - s_camerapos.y;
		dir.z = s_cameratarget.z - s_camerapos.z;

		D3DXVec3Normalize( &dir, &dir );

		s_camerapos.x += step * dir.x;
		s_camerapos.y += step * dir.y;
		s_camerapos.z += step * dir.z;

		s_cameratarget.x += step * dir.x;
		s_cameratarget.y += step * dir.y;
		s_cameratarget.z += step * dir.z;
	}

	return 0;
}

int E3DSetBeforePos()
{

	int hsid;
	hsid = code_getdi(0);


//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBeforePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SetBeforePos();
	if( ret ){
		DbgOut( "E3DSetBeforePos : curhs SetBeforePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DChkConfGround charahsid, groundhsid, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
int E3DChkConfGround()
{
	// 1 : charahsid
	// 2 : groundhsid
	// 3 : mode
	// 4 : diffmaxy
	// 5 : mapminy
	// 6 : result
	// 7 : adjustx
	// 8 : adjusty
	// 9 : adjustz
	// 10 : nx
	// 11 : ny
	// 12 : nz

// 1 : charahsid
	int charahsid;
	charahsid = code_getdi(0);

// 2 : groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 3 : mode
	int mode;
	mode = code_getdi(0);

// 4 : diffmaxy
	float diffmaxy;
	diffmaxy = (float)code_getdd(0.0);

// 5 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

// 6 : result
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );

// 7 : adjustx
	PVal *pvaladx;
	APTR aptradx;
	aptradx = code_getva( &pvaladx );

// 8 : adjusty
	PVal *pvalady;
	APTR aptrady;
	aptrady = code_getva( &pvalady );

// 9 : adjustz
	PVal *pvaladz;
	APTR aptradz;
	aptradz = code_getva( &pvaladz );

// 10 : nx
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 11 : ny
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 12 : nz
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );


//////////

	int ret;

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result = 0;
	float adx, ady, adz;
	float nx, ny, nz;

	ret = groundhs->ChkConfGround( charahs, mode, diffmaxy, mapminy, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );


	double dadx, dady, dadz;
	dadx = (double)adx;
	dady = (double)ady;
	dadz = (double)adz;
	code_setva( pvaladx, aptradx, TYPE_DNUM, &dadx );
	code_setva( pvalady, aptrady, TYPE_DNUM, &dady );
	code_setva( pvaladz, aptradz, TYPE_DNUM, &dadz );


	double dnx, dny, dnz;
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	return 0;
}

//E3DChkConfGround2 befposx, befposy, befposz, newposx, newposy, newposz, groundhsid, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
int E3DChkConfGround2()
{
	// 1 : befposx
	// 2 : befposy
	// 3 : befposz

	// 4 : newposx
	// 5 : newposy
	// 6 : newposz

	// 7 : groundhsid
	// 8 : mode
	// 9 : diffmaxy
	// 10 : mapminy
	// 11 : result
	// 12 : adjustx
	// 13 : adjusty
	// 14 : adjustz
	// 15 : nx
	// 16 : ny
	// 17 : nz



// 1 : befposx
	float befposx;
	befposx = (float)code_getdd(0.0);

// 2 : befposy
	float befposy;
	befposy = (float)code_getdd(0.0);

// 3 : befposz
	float befposz;
	befposz = (float)code_getdd(0.0);

// 4 : newposx
	float newposx;
	newposx = (float)code_getdd(0.0);

// 5 : newposy
	float newposy;
	newposy = (float)code_getdd(0.0);

// 6 : newposz
	float newposz;
	newposz = (float)code_getdd(0.0);

// 7 : groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 8 : mode
	int mode;
	mode = code_getdi(0);

// 9 : diffmaxy
	float diffmaxy;
	diffmaxy = (float)code_getdd(0.0);

// 10 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

// 11 : result
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );

// 12 : adjustx
	PVal *pvaladx;
	APTR aptradx;
	aptradx = code_getva( &pvaladx );

// 13 : adjusty
	PVal *pvalady;
	APTR aptrady;
	aptrady = code_getva( &pvalady );

// 14 : adjustz
	PVal *pvaladz;
	APTR aptradz;
	aptradz = code_getva( &pvaladz );

// 15 : nx
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 16 : ny
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 17 : nz
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

//////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGround2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befv, newv;
	befv.x = (float)befposx;
	befv.y = (float)befposy;
	befv.z = (float)befposz;

	newv.x = (float)newposx;
	newv.y = (float)newposy;
	newv.z = (float)newposz;

	int result = 0;
	float adx, ady, adz;
	float nx, ny, nz;

	ret = groundhs->ChkConfGround2( befv, newv, mode, diffmaxy, mapminy, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	double dadx, dady, dadz;
	dadx = (double)adx;
	dady = (double)ady;
	dadz = (double)adz;
	code_setva( pvaladx, aptradx, TYPE_DNUM, &dadx );
	code_setva( pvalady, aptrady, TYPE_DNUM, &dady );
	code_setva( pvaladz, aptradz, TYPE_DNUM, &dadz );

	double dnx, dny, dnz;
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );


	return 0;
}

//E3DGetPartNoByName hsid, partname, partno
int E3DGetPartNoByName()
{
	//type $202

	int ret;
//1
	int hsid;
	hsid = code_getdi(0);

//2
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetPartNoByName : partname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//3

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );


//////////



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int partno = -1;
	ret = curhs->GetPartNoByName( tempname, &partno );
	if( ret ){
		DbgOut( "E3DGetPartNoByName : curhs GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &partno );

	return 0;
}

//E3DGetVertNumOfPart hsid, partno, vertnum
int E3DGetVertNumOfPart()
{

//1 : hsid
	int hsid;
	hsid = code_getdi(0);


//2 : partno
	int partno;
	partno = code_getdi(0);


//3 : vertnumptr
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNumOfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertnum = 0;
	ret = curhs->GetVertNumOfPart( partno, &vertnum );
	if( ret ){
		DbgOut( "E3DGetVertNumOfPart : curhs GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	code_setva( pvalnum, aptrnum, TYPE_INUM, &vertnum );
	

	return 0;
}

int E3DGetVertPos()
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : vxptr
	PVal *pvalvx;
	APTR aptrvx;
	aptrvx = code_getva( &pvalvx );

//5 : vyptr
	PVal *pvalvy;
	APTR aptrvy;
	aptrvy = code_getva( &pvalvy );

//6 : vzptr
	PVal *pvalvz;
	APTR aptrvz;
	aptrvz = code_getva( &pvalvz );

//7 : kind
	int kind;
	kind = code_getdi(0);

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float vx, vy, vz;
	ret = curhs->GetVertPos( kind, partno, vertno, &vx, &vy, &vz );
	if( ret ){
		DbgOut( "E3DGetVertPos : curhs GetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dvx, dvy, dvz;
	dvx = (double)vx;
	dvy = (double)vy;
	dvz = (double)vz;
	code_setva( pvalvx, aptrvx, TYPE_DNUM, &dvx );
	code_setva( pvalvy, aptrvy, TYPE_DNUM, &dvy );
	code_setva( pvalvz, aptrvz, TYPE_DNUM, &dvz );


	return 0;
}



int E3DSetVertPos()
{

//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : vertx
	float vertx;
	vertx = (float)code_getdd(0.0);

//5 : verty
	float verty;
	verty = (float)code_getdd(0.0);

//6 : vertz
	float vertz;
	vertz = (float)code_getdd(0.0);

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPos( partno, vertno, vertx, verty, vertz );
	if( ret ){
		DbgOut( "E3DSetVertPos : curhs SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DSetPosOnGround()
{

//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : groundid
	int groundid;
	groundid = code_getdi(0);

//3 : mapmaxy
	float mapmaxy;
	mapmaxy = (float)code_getdd(0.0);

//4 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

//5 : posx
	float posx;
	posx = (float)code_getdd(0.0);

//6 : posz
	float posz;
	posz = (float)code_getdd(0.0);

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPosOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetPosOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetPosOnGround( curhs, mapmaxy, mapminy, posx, posz );
	if( ret ){
		DbgOut( "E3DSetPosOnGround : groundhs SetPosOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetPosOnGroundPart hsid, groundid, mapmaxy, mapminy, posx, posz
int E3DSetPosOnGroundPart()
{

//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : groundid
	int groundid;
	groundid = code_getdi(0);

//3 : groundpart
	int groundpart;
	groundpart = code_getdi(0);

//4 : mapmaxy
	float mapmaxy;
	mapmaxy = (float)code_getdd(0.0);

//5 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

//6 : posx
	float posx;
	posx = (float)code_getdd(0.0);

//7 : posz
	float posz;
	posz = (float)code_getdd(0.0);

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPosOnGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetPosOnGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetPosOnGroundPart( groundpart, curhs, mapmaxy, mapminy, posx, posz );
	if( ret ){
		DbgOut( "E3DSetPosOnGroundPart : groundhs SetPosOnGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCreateBillboard bmpname, width, height, transparentflag, billboardid, dirmode
int E3DCreateBillboard()
{
	int ret;
//1 : bmpname
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateBillboard : bmpname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : width
	float width;
	width = (float)code_getdd(0.0);

//3 : height
	float height;
	height = (float)code_getdd(0.0);

//4 : transparentflag
	int transparentflag;
	transparentflag = code_getdi(0);


//5 : billboardidptr
	PVal *pvalbbid;
	APTR aptrbbid;
	aptrbbid = code_getva( &pvalbbid );

//6 : dirmode
	int dirmode;
	dirmode = code_getdi( 0 );

//7 : orgflag
	int orgflag;
	orgflag = code_getdi( 0 );



	
/////////
	

	if( !g_bbhs ){
		DbgOut( "E3DCreateBillboard : g_bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	SetResDir( tempname1, g_bbhs );//!!!!!!


	int bbid = -1;
	ret = g_bbhs->AddBillboard( tempname1, width, height, transparentflag, dirmode, orgflag, &bbid );
	if( ret || (bbid < 0) ){
		DbgOut( "E3DCreateBillboard : g_bbhs AddBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( bbid >= 0 ){
	//Create 3D Object
		ret = Restore( g_bbhs, bbid );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


	/////
		/***
		ret = g_bbhs->SetCurrentBSphereData();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		***/
	}


	code_setva( pvalbbid, aptrbbid, TYPE_INUM, &bbid );


	return 0;
}


//E3DRenderBillboard <--- dispflag�̕t���Ă���Billboard�S�Ă��A�K�؂ȏ��ԂɁi���̉�����j�`�悷��
int E3DRenderBillboard()
{

	int scid;
	scid = code_getdi( 0 );

	int transskip;
	transskip = code_getdi(0);

//////////

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderBillboard : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( g_bbhs ){
		
		if( !g_pD3DApp ){
			_ASSERT( 0 );
			return 1;
		}

		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


		CalcMatView();


		g_bbhs->m_shandler->m_bbtransskip = transskip;//!!!!!!!!!!!!!!


		int ret;

		if( transskip == 0 ){
			ret = g_bbhs->ChkInView( s_matView, 0, 0 );
			if( ret ){
				DbgOut( "E3DRenderBillboard : ChkInView error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
		//Transform

			ret = g_bbhs->TransformBillboard( pdev, s_matView, &s_camerapos, &s_cameratarget );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

		}
//Render		
		//ret = g_bbhs->Render( pdev, 0 );
		ret = g_bbhs->Render( 0, pdev, 1, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DRenderBillboard : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetBillboardPos billboardid, posx, posy, posz
int E3DSetBillboardPos()
{

//1 : bbid
	int bbid;
	bbid = code_getdi(0);
//2 : posx
	float posx;
	posx = (float)code_getdd(0.0);
//3 : posy
	float posy;
	posy = (float)code_getdd(0.0);
//4 : posz
	float posz;
	posz = (float)code_getdd(0.0);

/////////

	int ret;
	if( g_bbhs ){

		ret = g_bbhs->SetBillboardPos( bbid, posx, posy, posz );
		if( ret ){
			DbgOut( "E3DSetBillboardPos : bbhs SetBillboardPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//ret = g_bbhs->SetCurrentBSphereData();
		//if( ret ){
		//	_ASSERT( 0 );
		//	return 1;
		//}

	}else{
		DbgOut( "E3DSetBillboardPos : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetBillboardOnGround billboardid, groundid, mapmaxy, mapminy, posx, posz	 
int E3DSetBillboardOnGround()
{
	//type $202

//1 : bbid
	int bbid;
	bbid = code_getdi(0);

//2 : groundid
	int groundid;
	groundid = code_getdi(0);

//3 : mapmaxy
	float mapmaxy;
	mapmaxy = (float)code_getdd(0.0);

//4 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

//5 : posx
	float posx;
	posx = (float)code_getdd(0.0);

//6 : posz
	float posz;
	posz = (float)code_getdd(0.0);

/////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetBillboardOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_bbhs ){

		ret = g_bbhs->SetBillboardOnGround( bbid, groundhs, mapmaxy, mapminy, posx, posz );
		if( ret ){
			DbgOut( "E3DSetBillboardOnGround : bbhs SetBillboardOnGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DSetBillboardOnGround : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DDestroyBillboard billboardid <---- free�͍s��Ȃ��A�g���񂹂�悤�ɁA���g�p�̃t���O�𗧂Ă�B
int E3DDestroyBillboard()
{

//1 : hsid
	int bbid;
	bbid = code_getdi(0);

/////////

	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, bbid, -2, dirtyreset, -2 );
	_ASSERT( !ret );

//////////

	if( g_bbhs ){

		ret = g_bbhs->DestroyBillboard( bbid );
		if( ret ){
			DbgOut( "E3DDestroyBillboard : bbhs DestroyBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "E3DDestroyBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}

	return 0;
}

int E3DLoadMQOFileAsGround()
{
	// 1 : mqo file name1
	// 2 : mult
	// 3 : pointer of handlerset id.

	// 4 : adjustuvflag

	// 5 : bonetype

	int ret;
//1
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : mult
	float mult;
	mult = (float)code_getdd(1.0);


//3	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );


//4
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//5
	int bonetype;
	bonetype = code_getdi( BONETYPE_RDB2 );

//////	

	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( tempname1, mult, s_hwnd, adjustuvflag, &temphsid, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
	return 0;

}

int LoadMQOFileAsGround_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype )
{
	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	newhs->m_TLmode = 2;//!!!!!!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadMQOFileAsGround( tempname1, mult, hwnd, adjustuvflag, hsidptr, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : curhs LoadMQOFileAsGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;
	return 0;

}



//E3DSaveMQOFile( int hsid, char* filename );
int E3DSaveMQOFile()
{
	// 1 : hsid
	// 2 : file name1

	int ret;
// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveMQOFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SaveMQOFile( tempname1, s_hwnd );
	if( ret ){
		DbgOut( "E3DSaveMQOFile : curhs SaveMQOFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DGetBillboardInfo( bbid, posxptr, posyptr, poszptr, texname<-- 256, transparentptr, widthptr, heightptr );
int E3DGetBillboardInfo()
{

// 1: srcbbid
	int srcbbid;
	srcbbid = code_getdi(0);

// 2: posxptr
	PVal *pvalposx;
	APTR aptrposx;
	aptrposx = code_getva( &pvalposx );

// 3: posyptr
	PVal *pvalposy;
	APTR aptrposy;
	aptrposy = code_getva( &pvalposy );

// 4: poszptr
	PVal *pvalposz;
	APTR aptrposz;
	aptrposz = code_getva( &pvalposz );


// 5: texnameptr
	PVal *pvaltex;
	APTR aptrtex;
	aptrtex = code_getva( &pvaltex );

// 6: transparentptr
	PVal *pvaltrans;
	APTR aptrtrans;
	aptrtrans = code_getva( &pvaltrans );

// 7: widthptr
	PVal *pvalwidth;
	APTR aptrwidth;
	aptrwidth = code_getva( &pvalwidth );

// 8: heightptr
	PVal *pvalheight;
	APTR aptrheight;
	aptrheight = code_getva( &pvalheight );

////////
////////
	int ret;

	float posx, posy, posz, width, height;
	char texname[MAX_PATH];
	int transparent;

	if( g_bbhs ){
		ret = g_bbhs->GetBillboardInfo( srcbbid, &posx, &posy, &posz, texname, &transparent, &width, &height );
		if( ret ){
			DbgOut( "E3DGetBillboardInfo : bbhs GetBillboardInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DGetBillboardInfo : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dposx, dposy, dposz, dwidth, dheight;
	dposx = (double)posx;
	dposy = (double)posy;
	dposz = (double)posz;
	dwidth = (double)width;
	dheight = (double)height;

	code_setva( pvalposx, aptrposx, TYPE_DNUM, &dposx );
	code_setva( pvalposy, aptrposy, TYPE_DNUM, &dposy );
	code_setva( pvalposz, aptrposz, TYPE_DNUM, &dposz );
	
	code_setva( pvaltex, aptrtex, TYPE_STRING, texname );

	code_setva( pvaltrans, aptrtrans, TYPE_INUM, &transparent );

	code_setva( pvalwidth, aptrwidth, TYPE_DNUM, &dwidth );
	code_setva( pvalheight, aptrheight, TYPE_DNUM, &dheight );

	return 0;
}

int E3DGetNearBillboard()
{
// 1: srcposx
	float srcposx;
	srcposx = (float)code_getdd(0.0);

// 2: srcposy
	float srcposy;
	srcposy = (float)code_getdd(0.0);

// 3: srcposz
	float srcposz;
	srcposz = (float)code_getdd(0.0);

// 4: maxdist
	float maxdist;
	maxdist = (float)code_getdd(0.0);

// 5: bbidptr
	PVal *pvalbbid;
	APTR aptrbbid;
	aptrbbid = code_getva( &pvalbbid );

/////////
/////////

	int ret;
	int bbid = -1;
	if( g_bbhs ){
		ret = g_bbhs->GetNearBillboard( srcposx, srcposy, srcposz, maxdist, &bbid );
		if( ret ){
			DbgOut( "E3DGetNearBillboard : bbhs GetNearBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DGetNearBillboard : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalbbid, aptrbbid, TYPE_INUM, &bbid );

	return 0;
}


//E3DGetInvisibleFlag( -1, m_nearbbid, &invisibleflag );
int E3DGetInvisibleFlag()
{
// 1: hsid
	int hsid;
	hsid = code_getdi(0);

// 2: partno
	int partno;
	partno = code_getdi(0);

// 3: flagptr
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

/////////
/////////

	int ret;
	CHandlerSet* curhs;
	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DGetInvisibleFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	int flag = 0;
	ret = curhs->GetInvisibleFlag( partno, &flag );
	if( ret ){
		DbgOut( "E3DGetInvisibleFlag : curhs GetInvisibleFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &flag );


	return 0;
}

//E3DSetInvisibleFlag( -1, m_nearbbid, 1 );
int E3DSetInvisibleFlag()
{

// 1: hsid
	int hsid;
	hsid = code_getdi(0);

// 2: partno
	int partno;
	partno = code_getdi(0);

// 3: srcflag
	int srcflag;
	srcflag = code_getdi(0);

/////////
/////////

	int ret;
	CHandlerSet* curhs;
	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetInvisibleFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	ret = curhs->SetInvisibleFlag( partno, srcflag );
	if( ret ){
		DbgOut( "E3DSetInvisibleFlag : curhs GetInvisibleFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMovableArea()
{
	int ret;
// 1: bmpname
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2: maxx
	float maxx;
	maxx = (float)code_getdd(0.0);

// 3: maxz
	float maxz;
	maxz = (float)code_getdd(0.0);

// 4: divx
	int divx;
	divx = code_getdi(0);

// 5: divz
	int divz;
	divz = code_getdi(0);

// 6: wallheight
	float wallheight;
	wallheight = (float)code_getdd(0.0);

// 7: rethsid
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

/////////
/////////

	int temphsid = -1;
	ret = SetMovableArea_F( tempname1, maxx, maxz, divx, divz, wallheight, &temphsid );
	if( ret ){
		DbgOut( "E3DSetMovableArea : SetMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	return 0;
}

int SetMovableArea_F( char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr )
{
	int ret;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}



	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	ret = newhs->SetMovableArea( tempname1, maxx, maxz, divx, divz, wallheight );
	if( ret ){
		DbgOut( "easy3d : E3DSetMovableArea : SetMovableArea error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/

	*hsidptr = newhs->serialno;//!!!!!!!!!!!


	return 0;

}


//E3DChkConfWall( int charahsid, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv );
int E3DChkConfWall()
{
	int ret;

// 1: charahsid
	int charahsid;
	charahsid = code_getdi(0);

// 2: groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 3: dist
	float dist;
	dist = (float)code_getdd(0.0);

// 4: resultptr
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );


// 5: adjustxptr
	PVal *pvaladx;
	APTR aptradx;
	aptradx = code_getva( &pvaladx );

// 6: adjustyptr
	PVal *pvalady;
	APTR aptrady;
	aptrady = code_getva( &pvalady );

// 7: adjustzptr
	PVal *pvaladz;
	APTR aptradz;
	aptradz = code_getva( &pvaladz );

// 8: nx10000ptr
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 9: ny10000ptr
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 10: nz10000ptr
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

/////////
/////////

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfWall : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWall : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int result = 0;
	float adx, ady, adz, nx, ny, nz;

	ret = groundhs->ChkConfWall( charahs, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWall : groundhs ChkConfWall error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dadx, dady, dadz, dnx, dny, dnz;
	dadx = (double)adx;
	dady = (double)ady;
	dadz = (double)adz;
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	code_setva( pvaladx, aptradx, TYPE_DNUM, &dadx );
	code_setva( pvalady, aptrady, TYPE_DNUM, &dady );
	code_setva( pvaladz, aptradz, TYPE_DNUM, &dadz );

	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );


	return 0;
}

//E3DChkConfWall2( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv );
int E3DChkConfWall2()
{
	int ret;

// 1: befposx
	float befposx;
	befposx = (float)code_getdd(0.0);

// 2: befposy
	float befposy;
	befposy = (float)code_getdd(0.0);

// 3: befposz
	float befposz;
	befposz = (float)code_getdd(0.0);

// 4: newposx
	float newposx;
	newposx = (float)code_getdd(0.0);

// 5: newposy
	float newposy;
	newposy = (float)code_getdd(0.0);

// 6: newposz
	float newposz;
	newposz = (float)code_getdd(0.0);

// 7: groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 8: dist
	float dist;
	dist = (float)code_getdd(0.0);

// 9: resultptr
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );


// 10: adjustxptr
	PVal *pvaladx;
	APTR aptradx;
	aptradx = code_getva( &pvaladx );

// 11: adjustyptr
	PVal *pvalady;
	APTR aptrady;
	aptrady = code_getva( &pvalady );

// 12: adjustzptr
	PVal *pvaladz;
	APTR aptradz;
	aptradz = code_getva( &pvaladz );

// 13: nx10000ptr
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 14: ny10000ptr
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 15: nz10000ptr
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

/////////
/////////

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWall2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result = 0;
	float adx, ady, adz, nx, ny, nz;


	ret = groundhs->ChkConfWall2( 0, 0, befposx, befposy, befposz, newposx, newposy, newposz, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWall2 : groundhs ChkConfWall2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	double dadx, dady, dadz, dnx, dny, dnz;
	dadx = (double)adx;
	dady = (double)ady;
	dadz = (double)adz;
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	code_setva( pvaladx, aptradx, TYPE_DNUM, &dadx );
	code_setva( pvalady, aptrady, TYPE_DNUM, &dady );
	code_setva( pvaladz, aptradz, TYPE_DNUM, &dadz );

	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	return 0;
}

int E3DChkConfWall3()//����p�[�c�w��
{
	int ret;

// 1: charahsid
	int charahsid;
	charahsid = code_getdi(0);

// 2: groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 3: partarray
	PVal *pvalpartarray;
	APTR aptrpartarray;
	aptrpartarray = code_getva( &pvalpartarray );

// 4: partnum
	int partnum;
	partnum = code_getdi(0);

// 5: dist
	float dist;
	dist = (float)code_getdd(0.0);

// 6: resultptr
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );


// 7: adjustxptr
	PVal *pvaladx;
	APTR aptradx;
	aptradx = code_getva( &pvaladx );

// 8: adjustyptr
	PVal *pvalady;
	APTR aptrady;
	aptrady = code_getva( &pvalady );

// 9: adjustzptr
	PVal *pvaladz;
	APTR aptradz;
	aptradz = code_getva( &pvaladz );

// 10: nx10000ptr
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 11: ny10000ptr
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 12: nz10000ptr
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

/////////
/////////

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfWall3 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWall3 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int result = 0;
	float adx, ady, adz, nx, ny, nz;

	ret = groundhs->ChkConfWall3( charahs, (int*)( pvalpartarray->pt ) + aptrpartarray, partnum, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWall3 : groundhs ChkConfWall error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dadx, dady, dadz, dnx, dny, dnz;
	dadx = (double)adx;
	dady = (double)ady;
	dadz = (double)adz;
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	code_setva( pvaladx, aptradx, TYPE_DNUM, &dadx );
	code_setva( pvalady, aptrady, TYPE_DNUM, &dady );
	code_setva( pvaladz, aptradz, TYPE_DNUM, &dadz );

	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );


	return 0;
}



int E3DVec3Normalize()
{

// 1: befposx
	float befposx;
	befposx = (float)code_getdd(0.0);

// 2: befposy
	float befposy;
	befposy = (float)code_getdd(0.0);

// 3: befposz
	float befposz;
	befposz = (float)code_getdd(0.0);

// 4: newposxptr
	PVal *pvalnewx;
	APTR aptrnewx;
	aptrnewx = code_getva( &pvalnewx );

// 5: newposyptr
	PVal *pvalnewy;
	APTR aptrnewy;
	aptrnewy = code_getva( &pvalnewy );

// 6: newposzptr
	PVal *pvalnewz;
	APTR aptrnewz;
	aptrnewz = code_getva( &pvalnewz );

/////////
/////////

	D3DXVECTOR3 vec;
	vec.x = befposx;
	vec.y = befposy;
	vec.z = befposz;

	DXVec3Normalize( &vec, &vec );

	double dnewx, dnewy, dnewz;
	dnewx = (double)vec.x;
	dnewy = (double)vec.y;
	dnewz = (double)vec.z;

	code_setva( pvalnewx, aptrnewx, TYPE_DNUM, &dnewx );
	code_setva( pvalnewy, aptrnewy, TYPE_DNUM, &dnewy );
	code_setva( pvalnewz, aptrnewz, TYPE_DNUM, &dnewz );

	return 0;
}

//E3DVec2CCW nbasevecx, nbasevecz, nvecx, nvecz, ccw
int E3DVec2CCW()
{

// 1: posx1
	float posx1;
	posx1 = (float)code_getdd(0.0);

// 2: posy1
	float posy1;
	posy1 = (float)code_getdd(0.0);

// 3: posx2
	float posx2;
	posx2 = (float)code_getdd(0.0);

// 4: posy2
	float posy2;
	posy2 = (float)code_getdd(0.0);

// 5: ccwptr
	PVal *pvalccw;
	APTR aptrccw;
	aptrccw = code_getva( &pvalccw );

/////////
/////////

	D3DXVECTOR2 vec1, vec2;

	vec1.x = posx1;
	vec1.y = posy1;

	vec2.x = posx2;
	vec2.y = posy2;

	float ccw;
	ccw = D3DXVec2CCW( &vec1, &vec2 );
	
	int iccw;
	if( ccw >= 0.0f )
		iccw = 1;
	else
		iccw = -1;

	code_setva( pvalccw, aptrccw, TYPE_INUM, &iccw );


	return 0;
}

//E3DVec3RotateY nx1, ny1, nz1, 90, kabex1, kabey1, kabez1, degmult
int E3DVec3RotateY()
{

// 1: vecx
	float vecx;
	vecx = (float)code_getdd(0.0);

// 2: vexy
	float vecy;
	vecy = (float)code_getdd(0.0);

// 3: vecz
	float vecz;
	vecz = (float)code_getdd(0.0);

// 4: deg
	float deg;
	deg = (float)code_getdd(0.0);

// 5; xptr
	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

// 6: yptr
	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

// 7: zptr
	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

/////////
/////////
	int ret;
	CQuaternion2 dirq;
	D3DXMATRIX	dirm;


	ret = dirq.SetRotation( 0, deg, 0 );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();


	double daftx, dafty, daftz;
	daftx = dirm._11 * vecx + dirm._21 * vecy + dirm._31 * vecz + dirm._41;
	dafty = dirm._12 * vecx + dirm._22 * vecy + dirm._32 * vecz + dirm._42;
	daftz = dirm._13 * vecx + dirm._23 * vecy + dirm._33 * vecz + dirm._43;
	//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
	
	code_setva( pvalx, aptrx, TYPE_DNUM, &daftx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dafty );
	code_setva( pvalz, aptrz, TYPE_DNUM, &daftz );

	return 0;
}

//E3DLoadMQOFileAsMovableArea mqofile, mult100, rethsid
int E3DLoadMQOFileAsMovableArea()
{
	// 1 : mqo file name1
	// 2 : mult
	// 3 : pointer of handlerset id.

	int ret;
//1
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : mult
	float mult;
	mult = (float)code_getdd(1.0);


//3	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

//////	

	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( tempname1, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	return 0;
}

int LoadMQOFileAsMovableArea_F( char* tempname1, float mult, int* hsidptr )
{
	*hsidptr = -1;

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	
	ret = newhs->LoadMQOFileAsMovableArea( tempname1, mult, s_hwnd, hsidptr );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : curhs LoadMQOFileAsMovableArea error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/

	*hsidptr = newhs->serialno;

	return 0;
}



//E3DLoadSound filename, soundid, use3dflag, bufnum
int E3DLoadSound()
{
	int ret;
//1
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadSound : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//2
	PVal *pvalsid;
	APTR aptrsid;
	aptrsid = code_getva( &pvalsid );

//3
	int use3dflag;
	use3dflag = code_getdi(0);

//4
	int reverbflag;
	reverbflag = code_getdi(1);

//5
	int bufnum;
	bufnum = code_getdi(1);
//6
	int oggdatanum;
	oggdatanum = code_getdi(16);

//////	

	if( !s_HS ){
		DbgOut( "E3DLoadSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// �ǂݍ���
	int soundid = -1;

	ret = s_HS->LoadSoundFile( tempname1, use3dflag, reverbflag, bufnum, oggdatanum, &soundid );
	if( ret ){
		DbgOut( "E3DLoadSound : hs LoadSoundFile error %s !!!\n", tempname1 );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "E3DLoadSound : success : %s\n", tempname1 );

	code_setva( pvalsid, aptrsid, TYPE_INUM, &soundid );


	return 0;
}



//E3DPlaySound soundid, isprimary
int E3DPlaySound()
{

	int soundid;
	soundid = code_getdi(0);

	int startsample;
	startsample = code_getdi(0);

	int openo;
	openo = code_getdi(0);

	int numloops;
	numloops = code_getdi(1);

///////
	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;
	ret = s_HS->PlaySound( soundid, startsample, openo, numloops );
	if( ret ){
		DbgOut( "E3DPlaySound : hs PlaySound error %d!!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DStopSound soundid
int E3DStopSound()
{

	int soundid;
	soundid = code_getdi(0);

///////
	if( !s_HS ){
		DbgOut( "E3DStopSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->StopSound( soundid );
	if( ret ){
		DbgOut( "E3DStopSound : hs StopSound error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetSoundLoop soundid, loopflag
int E3DSetSoundLoop()
{

	int soundid;
	soundid = code_getdi(0);

	int loopflag;
	loopflag = code_getdi(0);

	/***
///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundLoop : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetRepeats( soundid, loopflag );
	if( ret ){
		DbgOut( "E3DSetSoundLoop : hs SetRepeats error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
	***/

	return 0;
}


//E3DSetSoundVolume volume
int E3DSetSoundVolume()
{
	//type $202

//1
	float volume;
	volume = (float)code_getdd(0.0);

//2
	int soundid;
	soundid = code_getdi(-1);//!!!!! default �́@-1 


///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundVolume : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = s_HS->SetVolume( soundid, volume );
	if( ret ){
		DbgOut( "E3DSetSoundVolume : hs SetVolume error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

//E3DSetSoundTempo tempo
int E3DSetSoundTempo()
{

	float tempo;
	tempo = (float)code_getdd(0.0);
/***
///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundTempo : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_HS->SetMasterTempo( tempo );
	if( ret ){
		DbgOut( "E3DSetSoundTempo : hs SetMasterTempo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}



//E3DDestroySound( int soundid );
int E3DDestroySound()
{

	int soundid;
	soundid = code_getdi(0);

///////
	if( !s_HS ){
		DbgOut( "E3DDestroySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->DestroySound( soundid );
	if( ret ){
		DbgOut( "E3DDestroySound : hs DestroySound error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetSoundFrequency( int soundid, DWORD freq );
int E3DSetSoundFrequency()
{

//1
	int soundid;
	soundid = code_getdi(0);
//2
	float freq;
	freq = (float)code_getdd(1.0);

///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetFrequency( soundid, freq );
	if( ret ){
		DbgOut( "E3DSetSoundFrequency : hs SetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetSoundVolume( int soundid, LONG* pvol );
int E3DGetSoundVolume()
{

//1
	int soundid;
	soundid = code_getdi(0);

//2
	PVal *pvalvol;
	APTR aptrvol;
	aptrvol = code_getva( &pvalvol );

///////
	if( !s_HS ){
		DbgOut( "E3DGetSoundVolume : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float vol = 0.0f;
	ret = s_HS->GetVolume( soundid, &vol );
	if( ret ){
		DbgOut( "E3DGetSoundVolume : hs GetVolume error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
	
	double dvol = (double)vol;
	code_setva( pvalvol, aptrvol, TYPE_DNUM, &dvol );

	return 0;
}


//E3DGetSoundFrequency( int soundid, DWORD* pfreq );
int E3DGetSoundFrequency()
{

//1
	int soundid;
	soundid = code_getdi(0);

//2
	PVal *pvalfreq;
	APTR aptrfreq;
	aptrfreq = code_getva( &pvalfreq );

///////
	if( !s_HS ){
		DbgOut( "E3DGetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float freq = 0.0f;
	ret = s_HS->GetFrequency( soundid, &freq );
	if( ret ){
		DbgOut( "E3DGetSoundFrequency : hs GetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	double dfreq = (double)freq;
	code_setva( pvalfreq, aptrfreq, TYPE_DNUM, &dfreq );

	return 0;
}



//E3DSet3DSoundListener( float fDopplerFactor, float fRolloffFactor );
int E3DSet3DSoundListener()
{

//1
	float doppler;
	doppler = (float)code_getdd(0.0);

//2
	float rolloff;
	rolloff = (float)code_getdd(0.0);

/***
///////
	
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundListener : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->Set3DListener( doppler, rolloff );
	if( ret ){
		DbgOut( "E3DSet3DSoundListener : hs Set3DListener error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}

int E3DSet3DSoundListenerMovement()
{

	int hsid;
	hsid = code_getdi(0);

///////
	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vel( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 updir( 0.0f, 1.0f, 0.0f );

	D3DXVECTOR3 aftdir, aftupdir;

	if( hsid > 0 ){
		CHandlerSet* curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DRender : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		pos = curhs->m_gpd.m_gpe.pos;

		curhs->m_gpd.m_q.Rotate( &aftdir, dir );
		curhs->m_gpd.m_q.Rotate( &aftupdir, updir );

		
		ret = s_HS->Set3DListenerMovement( &pos, &vel, &aftdir, &aftupdir );
		if( ret ){
			DbgOut( "E3DSet3DSoundListenerMovement : hs Set3DListenerMovement error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		pos.x = s_camerapos.x;
		pos.y = s_camerapos.y;
		pos.z = s_camerapos.z;
		

		float fDeg2Pai = (float)DEG2PAI;	
		if( s_cameratype == 0 ){
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

			s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
			s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
			s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

			aftdir = s_cameratarget - s_camerapos;
			D3DXVec3Normalize( &aftdir, &aftdir );

			//aftupdir = vUpVec;


			s_cameraupvec = vUpVec;
			CalcMatView();//�c�C�X�g�O��matView

		// twist������s_cameraupvec�����߂�B
			D3DXMATRIX tempmatView;
			float cx, cy, cz;

			tempmatView = s_matView;
			cx = tempmatView._41;
			cy = tempmatView._42;
			cz = tempmatView._43;
			tempmatView._41 = 0.0f;
			tempmatView._42 = 0.0f;
			tempmatView._43 = 0.0f;



			D3DXVECTOR3 axisvec;
			CQuaternion2 addrot;
			axisvec = s_cameratarget - s_camerapos;
			DXVec3Normalize( &axisvec, &axisvec );
			addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
			
			
			D3DXQUATERNION viewqx;
			D3DXQuaternionRotationMatrix( &viewqx, &tempmatView );

			CQuaternion2 viewq;
			viewq.x = viewqx.x;
			viewq.y = viewqx.y;
			viewq.z = viewqx.z;
			viewq.w = viewqx.w;
			
			CQuaternion2 invview;
			viewq.inv( &invview );

			CQuaternion2 multq;
			multq = addrot * invview;

			D3DXVECTOR3 upv( 0.0f, 1.0f, 0.0f );
			multq.Rotate( &s_cameraupvec, upv );

			DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );
			aftdir = s_cameraupvec;

		}else{

			aftdir = s_cameratarget - s_camerapos;
			D3DXVec3Normalize( &aftdir, &aftdir );

			aftupdir = s_cameraupvec;
		}




		ret = s_HS->Set3DListenerMovement( &pos, &vel, &aftdir, &aftupdir );
		if( ret ){
			DbgOut( "E3DSet3DSoundListenerMovement : hs Set3DListenerMovement error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

//E3DSet3DSoundDistance( int soundid, float fMinDistance, float fMaxDistance );
int E3DSet3DSoundDistance()
{
//1
	int soundid;
	soundid = code_getdi(0);

//2
	float min;
	min = (float)code_getdd(0.0);

//3
	float max;
	max = (float)code_getdd(0.0);
/***
///////
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundDistance : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->Set3DDistance( soundid, min, max );
	if( ret ){
		DbgOut( "E3DSet3DSoundDistance : hs Set3DDistance error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
***/

	return 0;
}
//E3DSet3DSoundMovement( int soundid, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );
int E3DSet3DSoundMovement()
{

//1
	int soundid;
	soundid = code_getdi(0);

//2
	float posx;
	posx = (float)code_getdd(0.0);

//3
	float posy;
	posy = (float)code_getdd(0.0);

//4
	float posz;
	posz = (float)code_getdd(0.0);

//5
	float vx;
	vx = (float)code_getdd(0.0);

//6
	float vy;
	vy = (float)code_getdd(0.0);

//7
	float vz;
	vz = (float)code_getdd(0.0);

///////
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundMovement : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;

	pos.x = posx;
	pos.y = posy;
	pos.z = posz;

	vel.x = vx;
	vel.y = vy;
	vel.z = vz;

	int ret;
	ret = s_HS->Set3DMovement( soundid, &pos, &vel );
	if( ret ){
		DbgOut( "E3DSet3DSoundMovement : hs Set3DMovement error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DCreateNaviLine()
{
	PVal *pvalnlid;
	APTR aptrnlid;
	aptrnlid = code_getva( &pvalnlid );

///////
	
	CNaviLine* newnl;

	newnl = new CNaviLine();
	if( !newnl ){
		DbgOut( "E3DCreateNaviLine : newnl alloc error !!!\n" );
		return 1;
	}

	newnl->next = g_nlhead;
	g_nlhead = newnl;

	code_setva( pvalnlid, aptrnlid, TYPE_INUM, &newnl->lineid );

	return 0;
}



//E3DDestroyNaviLine lineid
int E3DDestroyNaviLine()
{

	int nlid = code_getdi(0);

/////////

	CNaviLine* delnl = 0;
	CNaviLine* chknl = g_nlhead;
	CNaviLine* befnl = 0;
	int isfound = 0;

	while( (isfound == 0) && chknl ){
		if( chknl->lineid == nlid ){
			delnl = chknl;
			isfound = 1;
			break;
		}
		befnl = chknl;
		chknl = chknl->next;
	}
	
	if( isfound ){
		if( befnl ){
			befnl->next = delnl->next;
		}else{
			g_nlhead = delnl->next;
		}
		delete delnl;

	}else{
		DbgOut( "E3DDestroyNaviLine : isfound 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DAddNaviPoint lineid, befpointid, &pointid // befpointid�̎��Ƀ`�F�C������Bbef==-1�̂Ƃ��Ō�ɁAbef==-2�̂Ƃ��ŏ��Ƀ`�F�C������
int E3DAddNaviPoint()
{

///
	int nlid = code_getdi(0);
///
	int befnpid = code_getdi(0);
///
	PVal *pvalpid;
	APTR aptrpid;
	aptrpid = code_getva( &pvalpid );

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DAddNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int npid = -1;
	ret = curnl->AddNaviPoint( befnpid, 1, &npid );
	if( ret ){
		DbgOut( "E3DAddNaviPoint : curnl AddNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalpid, aptrpid, TYPE_INUM, &npid );

	return 0;
}



//E3DRemoveNaviPoint lineid, pointid
int E3DRemoveNaviPoint()
{

///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
	

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DRemoveNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curnl->RemoveNaviPoint( npid );
	if( ret ){
		DbgOut( "E3DRemoveNaviPoint : curnl RemoveNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}


//E3DGetNaviPointPos lineid, pointid, &pos	// lineid, pointid ���w�肵�āApoint�̍��W���擾����B
int E3DGetNaviPointPos()
{

///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

///
	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );
///
	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNaviPointPos : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	D3DXVECTOR3 dstpos;
	ret = curnl->GetNaviPointPos( npid, &dstpos );
	if( ret ){
		DbgOut( "E3DGetNaviPointPos : curnl GetNaviPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;

	code_setva( pvalx, aptrx, TYPE_DNUM, &dx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dy );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dz );

	return 0;
}
//E3DSetNaviPointPos lineid, pointid, srcpos
int E3DSetNaviPointPos()
{

///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	float posx = (float)code_getdd(0.0);
///
	float posy = (float)code_getdd(0.0);
///
	float posz = (float)code_getdd(0.0);
/////////

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviPointPos : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 srcpos;
	srcpos.x = posx;
	srcpos.y = posy;
	srcpos.z = posz;

	ret = curnl->SetNaviPointPos( npid, &srcpos );
	if( ret ){
		DbgOut( "E3DSetNaviPointPos : curnl SetNaviPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	

	return 0;
}


//E3DGetNaviPointOwnerID lineid, pointid, &ownerid
int E3DGetNaviPointOwnerID()
{

	///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	PVal *pvaloid;
	APTR aptroid;
	aptroid = code_getva( &pvaloid );

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNaviPointOwnerID : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int oid = -1;
	ret = curnl->GetNaviPointOwnerID( npid, &oid );
	if( ret ){
		DbgOut( "E3DGetNaviPointOwnerID : curnl GetNaviPointOwnerID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaloid, aptroid, TYPE_INUM, &oid );

	return 0;
}

//E3DSetNaviPointOwnerID lineid, pointid, ownerid
int E3DSetNaviPointOwnerID()
{

	///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	int oid = code_getdi(0);
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviPointOwnerID : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->SetNaviPointOwnerID( npid, oid );
	if( ret ){
		DbgOut( "E3DSetNaviPointOwnerID : curnl SetNaviPointOwnerID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}

//E3DGetNextNaviPoint lineid, pointid, &nextid 
//pointid ��next �`�F�C����pointid��Ԃ��Bpointid == -1 �̂Ƃ��A�擪point��Ԃ�nextid == -1�̂Ƃ��́Anext�Ȃ��A
int E3DGetNextNaviPoint()
{

	///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	PVal *pvalnext;
	APTR aptrnext;
	aptrnext = code_getva( &pvalnext );

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNextNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int next = -1;
	ret = curnl->GetNextNaviPoint( npid, &next );
	if( ret ){
		DbgOut( "E3DGetNextNaviPoint : curnl GetNextNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnext, aptrnext, TYPE_INUM, &next );

	return 0;
}

//E3DGetPrevNaviPoint lineid, pointid, &befid�@// befid == -1�̂Ƃ��́Abef�Ȃ�
int E3DGetPrevNaviPoint()
{

	///
	int nlid = code_getdi(0);
///
	int npid = code_getdi(0);
///
	PVal *pvalprev;
	APTR aptrprev;
	aptrprev = code_getva( &pvalprev );

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prev = -1;
	ret = curnl->GetPrevNaviPoint( npid, &prev );
	if( ret ){
		DbgOut( "E3DGetPrevNaviPoint : curnl GetPrevNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalprev, aptrprev, TYPE_INUM, &prev );

	return 0;
}

//E3DGetNearestNaviPoint lineid, srcpos, &nearid, &previd, &nextid // ��ԋ߂�NaviPoint�ƁA���̑O���NaviPoint��pointid���擾����
int E3DGetNearestNaviPoint()
{
///
	int nlid = code_getdi(0);
///
	float posx = (float)code_getdd(0.0);
///
	float posy = (float)code_getdd(0.0);
///
	float posz = (float)code_getdd(0.0);
///
	PVal *pvalnear;
	APTR aptrnear;
	aptrnear = code_getva( &pvalnear );

///
	PVal *pvalprev;
	APTR aptrprev;
	aptrprev = code_getva( &pvalprev );

///
	PVal *pvalnext;
	APTR aptrnext;
	aptrnext = code_getva( &pvalnext );

/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 srcpos;
	srcpos.x = posx;
	srcpos.y = posy;
	srcpos.z = posz;

	int nearid, prev, next;

	ret = curnl->GetNearestNaviPoint( &srcpos, &nearid, &prev, &next );
	if( ret ){
		DbgOut( "E3DGetNearestNaviPoint : curnl GetNearestNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnear, aptrnear, TYPE_INUM, &nearid );
	code_setva( pvalprev, aptrprev, TYPE_INUM, &prev );
	code_setva( pvalnext, aptrnext, TYPE_INUM, &next );


	return 0;
}


//E3DFillUpNaviLine lineid, div, flag // 3�ȏ�_���Ȃ��Ƃ��́A�������Ȃ��Bpointid ��t���ւ��邩�ǂ����̃t���O���H�H
int E3DFillUpNaviLine()
{

	///
	int nlid = code_getdi(0);
///
	int div = code_getdi(0);
///
	int flag = code_getdi(0);

/////////

	if( div < 2 ){
		DbgOut( "E3DFillUpNaviLine : div too small warning : exit !!!\n" );
		return 0;
	}


	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DFillUpNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->FillUpNaviLine( div, flag );
	if( ret ){
		DbgOut( "E3DFillUpNaviLine : curnl FillUpNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DSetNaviLineOnGround lineid, groundid, mapmaxy, mapminy //�S�Ă�navipoint��n�ʂ̍����ɃZ�b�g����B
int E3DSetNaviLineOnGround()
{
	//type $202

//1 : nlid
	int nlid;
	nlid = code_getdi(0);

//2 : groundid
	int groundid;
	groundid = code_getdi(0);

//3 : mapmaxy
	float mapmaxy;
	mapmaxy = (float)code_getdd(0.0);

//4 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

//////////

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviLineOnGround : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetNaviLineOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetNaviLineOnGround( curnl, mapmaxy, mapminy );
	if( ret ){
		DbgOut( "E3DSetNaviLineOnGround : groundhs SetNaviLineOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DControlByNaviLine hsid, lineid, ctrlmode, roundflag, reverseflag, maxdist, posstep, dirstep100, &newpos, &newdirQ, &targetpointid
int E3DControlByNaviLine()
{

// 1 : hsid
	int hsid;
	hsid = code_getdi(0);

// 2 : nlid
	int nlid;
	nlid = code_getdi(0);

// 3 : ctrlmode
	int ctrlmode;
	ctrlmode = code_getdi(0);

// 4 : roundflag
	int roundflag;
	roundflag = code_getdi(0);

// 5 : reverseflag
	int reverseflag;
	reverseflag = code_getdi(0);

// 6 : maxdist
	float maxdist;
	maxdist = (float)code_getdd(0.0);

// 7 : posstep
	float posstep;
	posstep = (float)code_getdd(0.0);

// 8 : dirstep
	float dirstep;
	dirstep = (float)code_getdd(0.0);

// 9 : newposxptr
	PVal *pvalnewx;
	APTR aptrnewx;
	aptrnewx = code_getva( &pvalnewx );

// 10 : newposyptr
	PVal *pvalnewy;
	APTR aptrnewy;
	aptrnewy = code_getva( &pvalnewy );

// 11 : newposzptr
	PVal *pvalnewz;
	APTR aptrnewz;
	aptrnewz = code_getva( &pvalnewz );

// 12 : newqx
	PVal *pvalnewqx;
	APTR aptrnewqx;
	aptrnewqx = code_getva( &pvalnewqx );

// 13 : newqy
	PVal *pvalnewqy;
	APTR aptrnewqy;
	aptrnewqy = code_getva( &pvalnewqy );

// 14 : newqz
	PVal *pvalnewqz;
	APTR aptrnewqz;
	aptrnewqz = code_getva( &pvalnewqz );

// 15 : newqw
	PVal *pvalnewqw;
	APTR aptrnewqw;
	aptrnewqw = code_getva( &pvalnewqw );

// 16 : tpid
	PVal *pvaltpid;
	APTR aptrtpid;
	aptrtpid = code_getva( &pvaltpid );

//////////

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DControlByNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DControlByNaviLine : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//E3DControlByNaviLine hsid, lineid, ctrlmode, roundflag, reverseflag, posstep, dirstep, &newpos, &newdirQ, &targetpointid

	D3DXVECTOR3 offset( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 newpos;
	CQuaternion newq;


	CQuaternion tempdirq;
	tempdirq = curhs->m_gpd.m_q;

	int tpid;
	tpid = *((int*)pvaltpid->pt + aptrtpid);


	ret = curnl->ControlByNaviLine( curhs->m_gpd.m_gpe.pos, offset, tempdirq, 
			ctrlmode, roundflag, reverseflag,
			maxdist, 
			posstep, dirstep,
			&newpos, &newq, &tpid );
	if( ret ){
		DbgOut( "E3DControlByNaviLine : curnl ControlByNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltpid, aptrtpid, TYPE_INUM, &tpid );

	double dx, dy, dz;
	dx = (double)newpos.x;
	dy = (double)newpos.y;
	dz = (double)newpos.z;
	code_setva( pvalnewx, aptrnewx, TYPE_DNUM, &dx );
	code_setva( pvalnewy, aptrnewy, TYPE_DNUM, &dy );
	code_setva( pvalnewz, aptrnewz, TYPE_DNUM, &dz );

	double dqx, dqy, dqz, dqw;
	dqx = (double)newq.x;
	dqy = (double)newq.y;
	dqz = (double)newq.z;
	dqw = (double)newq.w;
	code_setva( pvalnewqx, aptrnewqx, TYPE_DNUM, &dqx );
	code_setva( pvalnewqy, aptrnewqy, TYPE_DNUM, &dqy );
	code_setva( pvalnewqz, aptrnewqz, TYPE_DNUM, &dqz );
	code_setva( pvalnewqw, aptrnewqw, TYPE_DNUM, &dqw );

	return 0;
}

//	�N�H�[�^�j�I���ɂ��A�����̃Z�b�g
//	���K��������B
int E3DSetDirQ()
{

// 1 : hsid	
	int hsid;
	hsid = code_getdi(0);

// 2 : qx
	float qx;
	qx = (float)code_getdd(0.0);

// 3 : qy
	float qy;
	qy = (float)code_getdd(0.0);

// 4 : qz
	float qz;
	qz = (float)code_getdd(0.0);

// 5 : qw
	float qw;
	qw = (float)code_getdd(0.0);

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion newq;
	newq.SetParams( qw, qx, qy, qz );

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );
	_ASSERT( !ret );
	curhs->m_gpd.m_gpe.rot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}




//	�p���������A�N�H�[�^�j�I���̎擾
int E3DGetDirQ()
{

// 1 : hsid
	int hsid;
	hsid = code_getdi(0);

// 2 : qx
	PVal *pvalqx;
	APTR aptrqx;
	aptrqx = code_getva( &pvalqx );

// 3 : qy
	PVal *pvalqy;
	APTR aptrqy;
	aptrqy = code_getva( &pvalqy );

// 4 : qz
	PVal *pvalqz;
	APTR aptrqz;
	aptrqz = code_getva( &pvalqz );

// 5 : qw
	PVal *pvalqw;
	APTR aptrqw;
	aptrqw = code_getva( &pvalqw );

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz, dw;
	dx = (double)curhs->m_gpd.m_q.x;
	dy = (double)curhs->m_gpd.m_q.y;
	dz = (double)curhs->m_gpd.m_q.z;
	dw = (double)curhs->m_gpd.m_q.w;

	code_setva( pvalqx, aptrqx, TYPE_DNUM, &dx );
	code_setva( pvalqy, aptrqy, TYPE_DNUM, &dy );
	code_setva( pvalqz, aptrqz, TYPE_DNUM, &dz );
	code_setva( pvalqw, aptrqw, TYPE_DNUM, &dw );

	return 0;
}



//E3DSetWallOnGround wallhsid, groundhsid, mapmaxy, mapminy, wallheight
int E3DSetWallOnGround()
{

//
	int whsid;
	whsid = code_getdi(0);
//
	int ghsid;
	ghsid = code_getdi(0);
//
	float mapmaxy;
	mapmaxy = (float)code_getdd(0.0);
//
	float mapminy;
	mapminy = (float)code_getdd(0.0);
//
	float wheight;
	wheight = (float)code_getdd(0.0);


//////////

	CHandlerSet* whs = GetHandlerSet( whsid );
	if( !whs ){
		DbgOut( "E3DSetWallOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* ghs = GetHandlerSet( ghsid );
	if( !ghs ){
		DbgOut( "E3DSetWallOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = ghs->SetWallOnGround( whs, mapmaxy, mapminy, wheight );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : ghs SetWallOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// Invalidate
	int resetflag = 1;
	ret = whs->Invalidate( resetflag );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : whs Invalidate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	ret = Restore( whs, -1 );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : whs Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////
	/***
	ret = whs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	return 0;
}


//E3DCreateNaviPointClearFlag hsid, lineno, roundnum
int E3DCreateNaviPointClearFlag()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: lineno
	int nlid;
	nlid = code_getdi(0);

//3: roundnum
	int roundnum;
	roundnum = code_getdi(0);

//////////

	if( roundnum <= 0 ){
		DbgOut( "E3DCreateNaviPointClearFlag : roundnum out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DCreateNaviPointClearFlag : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	ret = curhs->CreateNaviPointClearFlag( curnl, roundnum );
	if( ret ){
		DbgOut( "E3DCreateNaviPointClearFlag : curhs CreateNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DDestroyNaviPointClearFlag hsid
int E3DDestroyNaviPointClearFlag()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyNaviPointClearFlag();
	if( ret ){
		DbgOut( "E3DDestroyNaviPointClearFlag : curhs DestroyNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DInitNaviPointClearFlag hsid
int E3DInitNaviPointClearFlag()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DInitNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitNaviPointClearFlag();
	if( ret ){
		DbgOut( "E3DInitNaviPointClearFlag : curhs InitNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DSetNaviPointClearFlag hsid, srcposx, srcposy, srcposz, maxdist, &curp, &curr, &curdist
int E3DSetNaviPointClearFlag()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);
//2: srcposx
	float posx;
	posx = (float)code_getdd(0.0);

//3: srcposy
	float posy;
	posy = (float)code_getdd(0.0);

//4: srcposz
	float posz;
	posz = (float)code_getdd(0.0);

//5: maxdist
	float maxdist;
	maxdist = (float)code_getdd(0.0);

//6: npidptr
	PVal *pvalnpid;
	APTR aptrnpid;
	aptrnpid = code_getva( &pvalnpid );

//7: roundptr
	PVal *pvalround;
	APTR aptrround;
	aptrround = code_getva( &pvalround );

//8: distptr
	PVal *pvaldist;
	APTR aptrdist;
	aptrdist = code_getva( &pvaldist );

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	D3DXVECTOR3 srcpos;
	srcpos.x = posx;
	srcpos.y = posy;
	srcpos.z = posz;
	
	int npid, round;
	float dist;

	ret = curhs->SetNaviPointClearFlag( srcpos, maxdist, &npid, &round, &dist );
	if( ret ){
		DbgOut( "E3DSetNaviPointClearFlag : curhs SetNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnpid, aptrnpid, TYPE_INUM, &npid );
	code_setva( pvalround, aptrround, TYPE_INUM, &round );

	double ddist;
	ddist = (double)dist;
	code_setva( pvaldist, aptrdist, TYPE_DNUM, &ddist );

	return 0;
}


int SortNPCD( const VOID* arg1, const VOID* arg2 )
{
    CNPClearData** pp1 = (CNPClearData**)arg1;
    CNPClearData** pp2 = (CNPClearData**)arg2;
    
    CNPClearData* p1 = *pp1;
    CNPClearData* p2 = *pp2;


	int arno1, arno2;

	arno1 = p1->pointnum * p1->lastround + p1->lastpointarno;
	arno2 = p2->pointnum * p2->lastround + p2->lastpointarno;

	if( arno1 < arno2 ){
		return 1;
	}else if( arno1 > arno2 ){
		return -1;
	}else{
		// arno1 == arno2
		if( p1->lastdist > p2->lastdist ){
			return 1;
		}else{
			return -1;
		}
	}

    //float d1 = p1->pos.x * s_vDir.x + p1->pos.z * s_vDir.z;
    //float d2 = p2->pos.x * s_vDir.x + p2->pos.z * s_vDir.z;
    //if (d1 < d2)
    //  return +1;
    //return -1;
}


		
//E3DGetOrder hsidarray[], arrayleng, dstorder[]
int E3DGetOrder()
{

//1: hsidptr
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

//2: arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);

//3: dstorder
	PVal *pvalorder;
	APTR aptrorder;
	aptrorder = code_getva( &pvalorder );

//4: clearnoptr
	PVal *pvalclear;
	APTR aptrclear;
	aptrclear = code_getva( &pvalclear );

//////////

	if( arrayleng >= 500 ){
		DbgOut( "E3DSetOrder : arrayleng too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* hsarray[500];
	CNPClearData* npcdarray[500];

//npcd�̂�����	
	int hsno;
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		int curhsid;
		curhsid = *( (int*)pvalhsid->pt + aptrhsid + hsno );//!!!!!!!!!!!!!!!!!!

		hsarray[hsno] = GetHandlerSet( curhsid );
		if( !hsarray[hsno] ){
			DbgOut( "E3DSetOrder : hsarray : hsid not found error %d !!!\n", hsarray[hsno] );
			_ASSERT( 0 );
			return 1;
		}

		npcdarray[hsno] = &(hsarray[hsno]->m_npcd);
	}

//sort
	
	qsort( npcdarray, arrayleng, sizeof( CNPClearData* ), SortNPCD );

// order, clearno
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		
		int curhsid;
		curhsid = npcdarray[hsno]->ownerhsid;

		code_setva( pvalorder, aptrorder + hsno, TYPE_INUM, &curhsid );
		
		int clearno;
		clearno = npcdarray[hsno]->pointnum * npcdarray[hsno]->lastround + npcdarray[hsno]->lastpointarno;
		code_setva( pvalclear, aptrclear + hsno, TYPE_INUM, &clearno );

	}

	return 0;

}

int E3DDestroyAllBillboard()
{

	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, -1, -2, dirtyreset, -2 );
	_ASSERT( !ret );

///////////

	if( g_bbhs ){
		delete g_bbhs;
		g_bbhs = 0;
	}

	if( !g_bbhs ){
		g_bbhs = new CHandlerSet();
		if( !g_bbhs ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateHandler();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateBillboard();
		if( ret ){
			DbgOut( "E3DInit : g_bbhs CreateBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}	
		g_bbhs->m_billboardflag = 1;// !!!!! ver1036 2004/3/14
		
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}


	return 0;
}

int E3DSetValidFlag()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	// 3 : flag
	int flag;
	flag = code_getdi( 1 );//!!!!


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetValidFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetValidFlag : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->SetValidFlag( partno, flag );
	if( ret ){
		DbgOut( "e3dhsp : SetValidFlag : curhs SetValidFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DSetDiffuse()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : r
	int r;
	r = code_getdi(0);

	// 4 : g
	int g;
	g = code_getdi(0);

	// 5 : b
	int b;
	b = code_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = code_getdi(0);

	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//8 : updateflag
	int updateflag;
	updateflag = code_getdi(1);

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetDiffuse : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetDiffuse : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	int ret;
	ret = curhs->SetDispDiffuse( partno, r, g, b, setflag, vertno, updateflag );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

int E3DSetSpecular()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : r
	int r;
	r = code_getdi(0);

	// 4 : g
	int g;
	g = code_getdi(0);

	// 5 : b
	int b;
	b = code_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = code_getdi(0);


	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecular : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecular : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetDispSpecular( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecular : curhs SetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////////

	/***
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}

    D3DMATERIAL9 mtrl;
    //D3DUtil_InitMaterial( mtrl, 0.1f, 0.1f, 0.1f );
	D3DCOLORVALUE diff, amb, spec, emi;
	diff.r = 1.0f;
	diff.g = 1.0f;
	diff.b = 1.0f;
	diff.a = 1.0f;

	amb.r = 0.3f;
	amb.g = 0.3f;
	amb.b = 0.3f;
	amb.a = 1.0f;

	spec.r = r / 255.0f;
	spec.g = g / 255.0f;
	spec.b = b / 255.0f;
	spec.a = 1.0f;

	emi.r = 0.0f;
	emi.g = 0.0f;
	emi.b = 0.0f;
	emi.a = 1.0f;

	mtrl.Diffuse = diff;
	mtrl.Ambient = amb;
	mtrl.Specular = spec;
	mtrl.Emissive = emi;
	mtrl.Power = 100.0f;
    pdev->SetMaterial( &mtrl );
	***/
	
	return 0;

}

int E3DSetAmbient()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : r
	int r;
	r = code_getdi(0);

	// 4 : g
	int g;
	g = code_getdi(0);

	// 5 : b
	int b;
	b = code_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = code_getdi(0);



	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetAmbient : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetAmbient : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetDispAmbient( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetAmbient : curhs SetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

int E3DSetBlendingMode()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : bmode
	int bmode;
	bmode = code_getdi(0);

/////////

	CHandlerSet* curhs = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetBlendingMode : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetBlendingMode : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;

	if( curhs ){
		ret = curhs->SetBlendingMode( partno, bmode );
		if( ret ){
			DbgOut( "e3dhsp : SetBlendingMode : curhs SetBlendingMode error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		
		if( partno >= 0 ){
			CMySprite* cursprite;
			cursprite = GetSprite( partno );
			if( !cursprite ){
				DbgOut( "E3DSetBlendingMode : sprite id error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = cursprite->SetBlendingMode( bmode );
			if( ret ){
				DbgOut( "E3DSetBlendingMode : sprite SetBlendingMode error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CMySprite* cursprite = s_spritehead;
			while( cursprite ){

				ret = cursprite->SetBlendingMode( bmode );
				if( ret ){
					DbgOut( "E3DSetBlendingMode : sprite SetBlendingMode error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				cursprite = cursprite->next;
			}

		}
	}

	
	return 0;

}
int E3DSetRenderState()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : statetype
	int statetype;
	statetype = code_getdi(0);

	// 4 : value
	int value;
	value = code_getdi(0);

/////////

	CHandlerSet* curhs = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetRenderState : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetRenderState : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	int ret;
	if( curhs ){

		ret = curhs->SetRenderState( partno, statetype, value );
		if( ret ){
			DbgOut( "e3dhsp : SetRenderState : curhs SetRenderState error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		
		CMySprite* cursprite;
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DSetRenderState : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = cursprite->SetRenderState( statetype, value );
		if( ret ){
			DbgOut( "E3DSetRenderState : sprite SetRenderState error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	
	return 0;

}

//E3DSetScale hsid, partsno, scalex, scaley, scalez, mult, centerflag
int E3DSetScale()
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : scalex
	float scalex;
	scalex = (float)code_getdd(1.0);

	// 4 : scaley
	float scaley;
	scaley = (float)code_getdd(1.0);

	// 5 : scalez
	float scalez;
	scalez = (float)code_getdd(1.0);

	// 6 : centerflag
	int centerflag;
	centerflag = code_getdi(0);


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DXVECTOR3 scalevec;
	scalevec.x = scalex;
	scalevec.y = scaley;
	scalevec.z = scalez;

	int ret;
	ret = curhs->SetScale( partno, scalevec, centerflag );
	if( ret ){
		DbgOut( "e3dhsp : SetScale : curhs SetScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

/***
EXPORT BOOL WINAPI E3DTimeGetTime31bit( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
	curtype = *hei->nptype;
	int* timeptr;
	if( curtype == 4 ){
		timeptr = (int *)hei->HspFunc_prm_getv();	// �p�����[�^2:�ϐ�
		if( !timeptr ){
			DbgOut( "e3dhsp : TimeGetTime31bit : timeptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DTimeGetTime31bit : timeptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// �G���[�`�F�b�N

///////

	DWORD curtime;
	curtime = timeGetTime();

	*timeptr = curtime & 0x7FFFFFFF;

	return 0;
}
***/

//E3DGetScreenPos hsid, partno, &scx, &scy

int E3DGetScreenPos3()
{
	// 1 : scid
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPos3 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// 2 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 3 : partno
	int partno;
	partno = code_getdi(-1);
	
	//4 : scx
	PVal *pvalscx;
	APTR aptrscx;
	aptrscx = code_getva( &pvalscx );

	//5 : scy
	PVal *pvalscy;
	APTR aptrscy;
	aptrscy = code_getva( &pvalscy );

	//6 : scz
	PVal *pvalscz;
	APTR aptrscz;
	aptrscz = code_getva( &pvalscz );


	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);


	//7 : calcmode
	int calcmode;
	calcmode = code_getdi(1);


///////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPos3 : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPos3 : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	//!!!!!! ���f���S�̂̂Q�����W�擾���́Avertno��-1 !!!!!!!
	if( partno < 0 ){
		vertno = -1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPos3 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int scx, scy;
	float scz;

	ret = curhs->GetScreenPos3( partno, &scx, &scy, &scz, s_matView, vertno, calcmode );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos3 : curhs GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalscx, aptrscx, TYPE_INUM, &scx );
	code_setva( pvalscy, aptrscy, TYPE_INUM, &scy );

	double dscz = (double)scz;
	code_setva( pvalscz, aptrscz, TYPE_DNUM, &dscz );


	return 0;
}



int E3DGetScreenPos()
{
	// 1 : scid
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPos : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// 2 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 3 : partno
	int partno;
	partno = code_getdi(-1);
	
	//4 : scx
	PVal *pvalscx;
	APTR aptrscx;
	aptrscx = code_getva( &pvalscx );

	//5 : scy
	PVal *pvalscy;
	APTR aptrscy;
	aptrscy = code_getva( &pvalscy );


	//6 : vertno
	int vertno;
	vertno = code_getdi(-1);


	//7 : calcmode
	int calcmode;
	calcmode = code_getdi(1);


///////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	//!!!!!! ���f���S�̂̂Q�����W�擾���́Avertno��-1 !!!!!!!
	if( partno < 0 ){
		vertno = -1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPos : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int scx, scy;

	ret = curhs->GetScreenPos( partno, &scx, &scy, s_matView, vertno, calcmode );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos : curhs GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalscx, aptrscx, TYPE_INUM, &scx );
	code_setva( pvalscy, aptrscy, TYPE_INUM, &scy );


	return 0;
}


//E3DGetScreenPos2 x3d, y3d, z3d, x2dptr, y2dptr, validptr
int E3DGetScreenPos2()
{
	// 1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPos2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// 2
	float x3d;
	x3d = (float)code_getdd(0.0);

	// 3
	float y3d;
	y3d = (float)code_getdd(0.0);

	// 3
	float z3d;
	z3d = (float)code_getdd(0.0);

	
	//4
	PVal *pvalx2d;
	APTR aptrx2d;
	aptrx2d = code_getva( &pvalx2d );

	//5
	PVal *pvaly2d;
	APTR aptry2d;
	aptry2d = code_getva( &pvaly2d );

	//6
	PVal *pvalvalid;
	APTR aptrvalid;
	aptrvalid = code_getva( &pvalvalid );

///////
	
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPos2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 vec3d;
	vec3d.x = x3d;
	vec3d.y = y3d;
	vec3d.z = z3d;

	int ret;
	int valid, x2d, y2d;


	ret = GetScreenPos( vec3d, &x2d, &y2d, &valid );
	if( ret ){
		DbgOut( "E3DGetScreenPos2 : GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalx2d, aptrx2d, TYPE_INUM, &x2d );
	code_setva( pvaly2d, aptry2d, TYPE_INUM, &y2d );
	code_setva( pvalvalid, aptrvalid, TYPE_INUM, &valid );

	return 0;
}

int GetScreenPos( D3DXVECTOR3 vec3d, int* x2dptr, int* y2dptr, int* validptr )
{

	DWORD dwClipWidth = g_scsize.x / 2;
	DWORD dwClipHeight = g_scsize.y / 2;

	CBSphere bs;
	bs.tracenter = vec3d;

	float aspect;
	aspect = (float)dwClipWidth / (float)dwClipHeight;
	int ret;
	ret = bs.Transform2ScreenPos( dwClipWidth, dwClipHeight, s_matView, g_matProj, aspect, x2dptr, y2dptr );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos : bs Transform2ScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (*x2dptr == 0xFFFFFFFF) && (*y2dptr == 0xFFFFFFFF) ){
		*validptr = 0;
	}else{
		*validptr = 1;
	}

	return 0;
}

//E3DCreateQ &qid
int E3DCreateQ()
{
	PVal *pvalqid;
	APTR aptrqid;
	aptrqid = code_getva( &pvalqid );


///////

	if( !s_qh ){
		DbgOut( "E3DCreateQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int qid = -1;
	ret = s_qh->CreateQ( &qid );
	if( ret ){
		DbgOut( "E3DCreateQ : qh CreateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalqid, aptrqid, TYPE_INUM, &qid );

	return 0;	
}

//E3DDestroyQ qid
int E3DDestroyQ()
{

	int qid;
	qid = code_getdi(0);

///////

	if( !s_qh ){
		DbgOut( "E3DDestroyQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_qh->DestroyQ( qid );
	if( ret ){
		DbgOut( "E3DDestroyQ : qh DestroyQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DInitQ qid
int E3DInitQ()
{

	int qid;
	qid = code_getdi(0);

///////

	if( !s_qh ){
		DbgOut( "E3DInitQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_qh->InitQ( qid );
	if( ret ){
		DbgOut( "E3DInitQ : qh InitQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetQAxisAndDeg qid, axisx, axisy, axisz, deg, degmult
int E3DSetQAxisAndDeg()
{
	//1 ; qid
	int qid;
	qid = code_getdi(0);

	//2 : axisx
	float axisx;
	axisx = (float)code_getdd(0.0);
	
	//3 : axisy
	float axisy;
	axisy = (float)code_getdd(0.0);

	//4 : axisz
	float axisz;
	axisz = (float)code_getdd(0.0);

	//5 : deg
	float deg;
	deg = (float)code_getdd(0.0);

///////

	if( !s_qh ){
		DbgOut( "E3DSetQAxisAndDeg : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	axisvec.x = axisx;
	axisvec.y = axisy;
	axisvec.z = axisz;

	int ret;
	ret = s_qh->SetQAxisAndDeg( qid, axisvec, deg );
	if( ret ){
		DbgOut( "E3DSetQAxisAndDeg : qh SetQAxisAndDeg error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetQAxisAndDeg qid, &axisx10000, &axisy10000, &axisy10000, &deg10000
int E3DGetQAxisAndDeg()
{

	//1 ; qid
	int qid;
	qid = code_getdi(0);

	//2 : axisx		
	PVal *pvalax;
	APTR aptrax;
	aptrax = code_getva( &pvalax );

	//3 : axisy
	PVal *pvalay;
	APTR aptray;
	aptray = code_getva( &pvalay );

	//4 : axisz
	PVal *pvalaz;
	APTR aptraz;
	aptraz = code_getva( &pvalaz );

	//5 : deg
	PVal *pvaldeg;
	APTR aptrdeg;
	aptrdeg = code_getva( &pvaldeg );

///////

	if( !s_qh ){
		DbgOut( "E3DGetQAxisAndDeg : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	float deg;

	int ret;
	ret = s_qh->GetQAxisAndDeg( qid, &axisvec, &deg );
	if( ret ){
		DbgOut( "E3DGetQAxisAndDeg : qh GetQAxisAndDeg error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dax, day, daz;
	dax = (double)axisvec.x;
	day = (double)axisvec.y;
	daz = (double)axisvec.z;
	code_setva( pvalax, aptrax, TYPE_DNUM, &dax );
	code_setva( pvalay, aptray, TYPE_DNUM, &day );
	code_setva( pvalaz, aptraz, TYPE_DNUM, &daz );

	double ddeg;
	ddeg = (double)deg;
	code_setva( pvaldeg, aptrdeg, TYPE_DNUM, &ddeg );

	return 0;
}


//E3DRotateQX qid, degx, mult
int E3DRotateQX()
{
	//1 ; qid
	int qid;
	qid = code_getdi(0);

	//2 : degx
	float degx;
	degx = (float)code_getdd(0.0);

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQX : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 1.0f;
	axisvec.y = 0.0f;
	axisvec.z = 0.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degx );
	if( ret ){
		DbgOut( "E3DRotateQX : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DRotateQY qid, degy, mult
int E3DRotateQY()
{
	//1 ; qid
	int qid;
	qid = code_getdi(0);

	//2 : degx
	float degy;
	degy = (float)code_getdd(0.0);

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQY : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 0.0f;
	axisvec.y = 1.0f;
	axisvec.z = 0.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degy );
	if( ret ){
		DbgOut( "E3DRotateQY : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
//E3DRotateQZ qid, degz, mult
int E3DRotateQZ()
{
	//1 ; qid
	int qid;
	qid = code_getdi(0);

	//2 : degz
	float degz;
	degz = (float)code_getdd(0.0);

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQZ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 0.0f;
	axisvec.y = 0.0f;
	axisvec.z = 1.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degz );
	if( ret ){
		DbgOut( "E3DRotateQZ : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DMultQ resultqid, befqid, aftqid
int E3DMultQ()
{
	//1 ; resqid
	int resqid;
	resqid = code_getdi(0);

	//2 ; befqid
	int befqid;
	befqid = code_getdi(0);

	//3 ; aftqid
	int aftqid;
	aftqid = code_getdi(0);

///////

	if( !s_qh ){
		DbgOut( "E3DMultQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->MultQ( resqid, befqid, aftqid );
	if( ret ){
		DbgOut( "E3DMultQ : qh MultQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DNormalizeQ()
{
	//1 ; qid
	int qid;
	qid = code_getdi(0);

///////

	if( !s_qh ){
		DbgOut( "E3DNormalizeQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->NormalizeQ( qid );
	if( ret ){
		DbgOut( "E3DNormalizeQ : qh NormalizeQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DCopyQ()
{
	//1 ; dstqid
	int dstqid;
	dstqid = code_getdi(0);

	//2 ; srcqid
	int srcqid;
	srcqid = code_getdi(0);

///////

	if( !s_qh ){
		DbgOut( "E3DCopyQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->CopyQ( dstqid, srcqid );
	if( ret ){
		DbgOut( "E3DCopyQ : qh CopyQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetBoneNoByName hsid, name, &boneno
	//�Y�����Ȃ��ꍇ�́@-1
//!!! �G���h�W���C���g�́A����s�\�ɂ��邽�߂ɁA�擾�ł��Ȃ��悤�ɂ���B

int E3DGetBoneNoByName()
{
	int ret;
//1
	int hsid;
	hsid = code_getdi(0);

//2
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetBoneNoByName : bonename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//3
	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );


//////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetBoneNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int boneno = -1;
	ret = curhs->GetBoneNoByName( tempname, &boneno );
	if( ret ){
		DbgOut( "E3DGetBoneNoByName : curhs GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &boneno );

	return 0;
}


//E3DGetNextMP hsid, motionid, boneno, prevmpid, &mpid
	//prevmpid == -1 �̎��́A�ŏ���mp
	//prevmpid >= 0	�ŁA����mp�����݂��Ȃ��ꍇ�́A�G���[�B
	//�����ꍇ�́@-1
int E3DGetNextMP()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: prevmpid
	int prevmpid;
	prevmpid = code_getdi(0);

//5: mpidptr
	PVal *pvalmpid;
	APTR aptrmpid;
	aptrmpid = code_getva( &pvalmpid );

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetNextMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mpid = -1;
	ret = curhs->GetNextMP( motid, boneno, prevmpid, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : GetNextMP : curhs GetNextMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalmpid, aptrmpid, TYPE_INUM, &mpid );

	return 0;
}

//E3DGetMPInfo hsid, motionid, boneno, mpid, infoptr
	//infoptr.0 : qid(�Ăяo���O�ɏ���)
	//infoptr.1 : trax
	//infoptr.2 : tray
	//infoptr.3 : traz
	//infoptr.4 : frameno
	//infoptr.5 : displayswitch
	//infoptr.6 : interpolation
	
	//infoptr.7 : scalex
	//infoptr.8 : scaley
	//infoptr.9 : scalez
	//infoptr.10: userint1

int E3DGetMPInfo()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: mpid
	int mpid;
	mpid = code_getdi(0);

//5: infoptr
	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetMPInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion dstq;
	D3DXVECTOR3 dsttra;
	int dstframeno;
	DWORD dstds;
	int dstinterp;

	D3DXVECTOR3 dstsc;
	int dstuserint1;

	ret = curhs->GetMPInfo( motid, boneno, mpid, &dstq, &dsttra, &dstframeno, &dstds, &dstinterp, &dstsc, &dstuserint1 );
	//ret = curhs->GetMPInfo( motid, boneno, mpid, &dstq, &dsttra, &dstframeno, &dstds, &dstinterp );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetMPInfo : curhs GetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DGetMPInfo : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( *((int*)pvalinfo->pt + aptrinfo + MPI_QUA), &setq );
	if( ret ){
		DbgOut( "E3DGetMPInfo : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "E3DGetMPInfo : %f %f %f\r\n", dstsc.x, dstsc.y, dstsc.z );

	int itrax, itray, itraz;
	itrax = (int)dsttra.x;
	itray = (int)dsttra.y;
	itraz = (int)dsttra.z;
	code_setva( pvalinfo, aptrinfo + MPI_TRAX, TYPE_INUM, &itrax );
	code_setva( pvalinfo, aptrinfo + MPI_TRAY, TYPE_INUM, &itray );
	code_setva( pvalinfo, aptrinfo + MPI_TRAZ, TYPE_INUM, &itraz );

	code_setva( pvalinfo, aptrinfo + MPI_FRAMENO, TYPE_INUM, &dstframeno );

	int ids;
	ids = (int)dstds;
	code_setva( pvalinfo, aptrinfo + MPI_DISPSWITCH, TYPE_INUM, &ids );

	code_setva( pvalinfo, aptrinfo + MPI_INTERP, TYPE_INUM, &dstinterp );

	int iscx, iscy, iscz;
	iscx = (int)(dstsc.x * 1000.0f);
	iscy = (int)(dstsc.y * 1000.0f);
	iscz = (int)(dstsc.z * 1000.0f);
	code_setva( pvalinfo, aptrinfo + MPI_SCALEX, TYPE_INUM, &iscx );
	code_setva( pvalinfo, aptrinfo + MPI_SCALEY, TYPE_INUM, &iscy );
	code_setva( pvalinfo, aptrinfo + MPI_SCALEZ, TYPE_INUM, &iscz );

	code_setva( pvalinfo, aptrinfo + MPI_USERINT1, TYPE_INUM, &dstuserint1 );

	int val1000 = 1000;
	code_setva( pvalinfo, aptrinfo + MPI_SCALEDIV, TYPE_INUM, &val1000 );
	
	return 0;
}



//E3DSetMPInfo hsid, motionid, boneno, mpid, infoptr, infoflagptr ----> �K�؂ȃ^�C�~���O�ŁAE3DFillUpMP�K�v�B
	//infoptr.0 : qid(�Ăяo���O�ɏ���)
	//infoptr.1 : trax
	//infoptr.2 : tray
	//infoptr.3 : traz
	//infoptr.4 : frameno
	//infoptr.5 : displayswitch
	//infoptr.6 : interpolation

	//infoflagptr.0 ���P�̂Ƃ��́Ainfoptr.0 ��qid��L����
	//infoflagptr.1 ���P�̂Ƃ��́Ainfoptr.1��trax��L����
	//....


	//!!!! frameno ���s�K�؂Ȓl�łȂ����`�F�b�N���I�I�I�I
	// ����frameno�Ƀ��[�V�����|�C���g������ꍇ�ɂ��A�G���[

int E3DSetMPInfo()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: mpid
	int mpid;
	mpid = code_getdi(0);

//5: infoptr
	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );

//6: infoflagptr
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

//////////


	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMPInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	D3DXVECTOR3 srctra;
	int srcframeno;
	DWORD srcds;
	int srcinterp;

	D3DXVECTOR3 srcscale;
	int srcuserint1;

	if( !s_qh ){
		DbgOut( "E3DSetMPInfo : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( *((int*)pvalinfo->pt + aptrinfo + MPI_QUA), &srcqptr );
	if( ret ){
		DbgOut( "E3DSetMPInfo : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float scalediv;

	if( *((int*)pvalflag->pt + aptrflag + MPI_SCALEDIV) != 0 ){
		scalediv = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEDIV));
		if( scalediv == 0.0f )
			scalediv = 1.0f;
	}else{
		scalediv = 1.0f;
	}

	srctra.x = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAX));
	srctra.y = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAY));
	srctra.z = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAZ));
	srcframeno = *((int*)pvalinfo->pt + aptrinfo + MPI_FRAMENO);
	srcds = *((DWORD*)((int*)pvalinfo->pt + aptrinfo + MPI_DISPSWITCH));
	srcinterp = *((int*)pvalinfo->pt + aptrinfo + MPI_INTERP);

	srcscale.x = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEX)) / scalediv;
	srcscale.y = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEY)) / scalediv;
	srcscale.z = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEZ)) / scalediv;
	srcuserint1 = *((int*)pvalinfo->pt + aptrinfo + MPI_USERINT1);


//DbgOut( "E3DSetMPInfo : %f, %f, %f, %f : %d %d %d %d\r\n", srcscale.x, srcscale.y, srcscale.z, scalediv, *(infoflagptr + MPI_SCALEX), *(infoflagptr + MPI_SCALEY), *(infoflagptr + MPI_SCALEZ), *(infoflagptr + MPI_SCALEDIV));

	CQuaternion setq;
	setq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!

	ret = curhs->SetMPInfo( motid, boneno, mpid, &setq, &srctra, srcframeno, srcds, srcinterp, &srcscale, srcuserint1, (int*)pvalflag->pt + aptrflag );
	//ret = curhs->SetMPInfo( motid, boneno, mpid, &setq, &srctra, srcframeno, srcds, srcinterp, infoflagptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMPInfo : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DIsExistMP hsid, motionid, boneno, frameno, &mpid
		//frameno�ɁA���Ƀ��[�V�����|�C���g������΁A����id��������B
		//�����Ƃ��́Ampid �Ɂ@-1��������
int E3DIsExistMP()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: frameno
	int frameno;
	frameno = code_getdi(0);

//5: mpidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DIsExistMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mpid = -1;
	ret = curhs->IsExistMP( motid, boneno, frameno, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DIsExistMP : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &mpid );

	return 0;
}


//E3DGetMotionFrameLength hsid, motionid, &frameleng

		//�t���[���ԍ��̍ő�l�́Aleng - 1�Ȃ̂Œ���

int E3DGetMotionFrameLength()
{
//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: lengptr
	PVal *pvalleng;
	APTR aptrleng;
	aptrleng = code_getva( &pvalleng );

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int leng = 0;
	ret = curhs->GetMotionFrameLength( motid, &leng );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : curhs GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalleng, aptrleng, TYPE_INUM, &leng );

	return 0;
}

//E3DSetMotionFrameLength hsid, motionid, frameleng, initflag ----> �K�؂ȃ^�C�~���O�ŁAE3DFillUpMP�K�v�B
	//�t���[�������k������ꍇ�́A�͈͊O�ɂȂ�t���[���ԍ��������[�V�����|�C���g�́A�폜�����B
int E3DSetMotionFrameLength()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: frameleng
	int frameleng;
	frameleng = code_getdi(0);

//4; initflag
	int initflag;
	initflag = code_getdi(0);

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionFrameLength( motid, frameleng, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : curhs SetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DAddMP hsid, motionid, boneno, infoptr, &mpid ----> �K�؂ȃ^�C�~���O�ŁAE3DFillUpMP�K�v�B
		//���Ƀt���[���ԍ��Ƀ��[�V�����|�C���g���������ꍇ�́A�G���[
int E3DAddMP()
{
//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: infoptr
	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );

//5: mpidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DAddMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	D3DXVECTOR3 srctra;
	int srcframeno;
	DWORD srcds;
	int srcinterp;

	D3DXVECTOR3 srcsc;
	int srcuserint1;

	if( !s_qh ){
		DbgOut( "E3DAddMP : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( *((int*)pvalinfo->pt + aptrinfo), &srcqptr );
	if( ret ){
		DbgOut( "E3DAddMP : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float scalediv;
	scalediv = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEDIV));
	if( scalediv == 0.0f )
		scalediv = 1.0f;


	srctra.x = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAX));
	srctra.y = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAY));
	srctra.z = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_TRAZ));
	srcframeno = *((int*)pvalinfo->pt + aptrinfo + MPI_FRAMENO);
	srcds = *((DWORD*)((int*)pvalinfo->pt + aptrinfo + MPI_DISPSWITCH));
	srcinterp = *((int*)pvalinfo->pt + aptrinfo + MPI_INTERP);

	srcsc.x = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEX)) / scalediv;
	srcsc.y = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEY)) / scalediv;
	srcsc.z = (float)(*((int*)pvalinfo->pt + aptrinfo + MPI_SCALEZ)) / scalediv;
	srcuserint1 = *((int*)pvalinfo->pt + aptrinfo + MPI_USERINT1);


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	///MIP_SCALEDIV�͍l�����Ă��Ȃ��I�I�I�I�I�I�I�I�I�I�I�I
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	CQuaternion addq;
	addq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!!

	//ret = curhs->AddMotionPoint( motid, boneno, &addq, &(infoptr->tra), infoptr->frameno, infoptr->dispswitch, infoptr->interpolation, &(infoptr->scale), infoptr->userint1, mpidptr );
	int mpid = -1;
	ret = curhs->AddMotionPoint( motid, boneno, &addq, &srctra, srcframeno, srcds, srcinterp, &srcsc, srcuserint1, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DAddMP : curhs AddMotioinPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &mpid );

	return 0;
}


//E3DDeleteMP hsid, motionid, boneno, mpid ----> �K�؂ȃ^�C�~���O�ŁAE3DFillUpMP�K�v�B
int E3DDeleteMP()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: mpid
	int mpid;
	mpid = code_getdi(0);

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DDeleteMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DeleteMotionPoint( motid, boneno, mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDeleteMP : curhs DeleteMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DFillUpMP hsid, motionid, boneno, startmp, endmp, initflag
		//boneno�w��̏ꍇ�́Aboneno�ȉ���tree�̂ݍX�V�B
		//boneno == -1�̂Ƃ��́A�S�Ẵ{�[�����X�V�B
		//endmp == -1 �̂Ƃ��́A�Ō�̃t���[���܂ł��v�Z�B
		//initflag == 1 ---> �����蔻������X�V
int E3DFillUpMotion()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: motid
	int motid;
	motid = code_getdi(0);

//3: boneno
	int boneno;
	boneno = code_getdi(0);

//4: startframe
	int startframe;
	startframe = code_getdi(0);

//5: endframe
	int endframe;
	endframe = code_getdi(0);

//6: initflag
	int initflag;
	initflag = code_getdi(0);

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DFillUpMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->FillUpMotion( motid, boneno, startframe, endframe, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DFillUpMotion : curhs E3DFillUpMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCopyMotionFrame hsid, srcmotid, srcframeno, dstmotid, dstframeno
		// E3DFillUpMotion���Ȃ��ƁA���ʂ͔��f����Ȃ��B
		//srcframe�ɁA���[�V�����|�C���g�������ꍇ�́A��Ԍv�Z��������
		//dstframe���ɁA���[�V�����|�C���g�������ꍇ�́A�V�K�ɍ쐬����B
int E3DCopyMotionFrame()
{

//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: srcmotid
	int srcmotid;
	srcmotid = code_getdi(0);

//3: srcframe
	int srcframe;
	srcframe = code_getdi(0);

//4: dstmotid
	int dstmotid;
	dstmotid = code_getdi(0);

//5: dstframe
	int dstframe;
	dstframe = code_getdi(0);

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DCopyMotionFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CopyMotionFrame( srcmotid, srcframe, dstmotid, dstframe );
	if( ret ){
		DbgOut( "e3dhsp : E3DCopyMotionFrame : curhs CopyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//	E3DGetDirQ2 hsid, dstqid
int E3DGetDirQ2()
{
//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: dstqid
	int dstqid;
	dstqid = code_getdi(0);

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetDirQ2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DGetDirQ2 : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion q = curhs->m_gpd.m_q;
	CQuaternion2 setq;
	setq.SetParams( q.w, q.x, q.y, q.z, curhs->m_twist );
	ret = s_qh->SetQ( dstqid, &setq );
	if( ret ){
		DbgOut( "E3DGetDirQ2 : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//	E3DSetDirQ2 hsid, srcqid
int E3DSetDirQ2()
{
//1: hsid
	int hsid;
	hsid = code_getdi(0);

//2: srcqid
	int srcqid;
	srcqid = code_getdi(0);

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetDirQ2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DSetDirQ2 : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( srcqid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetDirQ2 : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !srcqptr ){
		DbgOut( "E3DSetDirQ2 : srcqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion newq;
	newq.SetParams( srcqptr->w, srcqptr->x, srcqptr->y, srcqptr->z );

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.rot );
	_ASSERT( !ret );
	curhs->m_gpd.m_gpe.rot = neweul;
	curhs->m_twist = srcqptr->twist;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}

//E3DLookAtQ qid, vecx, vecy, vecz, basex, basey, basez, upflag, divnum

	// �I�I�I�I�@qid�ɂ́A���O�̏�񂪓����Ă��邱�Ƃ�O��Ƃ���B

	//upflag == 0 --> �����
	//upflag == 1 --> ������
	//upflag == 2 --> ��������A���Ԃ胂�[�h
	//upflag == 3 --> ������ێ��Ȃ����[�h

int E3DLookAtQ()
{

//1: dstqid
	int dstqid;
	dstqid = code_getdi(0);

//2: vecx
	float vecx;
	vecx = (float)code_getdd(0.0);

//3: vecx
	float vecy;
	vecy = (float)code_getdd(0.0);

//4: vecz
	float vecz;
	vecz = (float)code_getdd(1.0);

//5: basevecx
	float basevecx;
	basevecx = (float)code_getdd(0.0);

//6: basevecx
	float basevecy;
	basevecy = (float)code_getdd(0.0);

//7: basevecz
	float basevecz;
	basevecz = (float)code_getdd(-1.0);


//8: upflag
	int upflag;
	upflag = code_getdi(2);

//9: divnum
	int divnum;
	divnum = code_getdi(1);

///////


	int ret;

	if( !s_qh ){
		DbgOut( "E3DLookAtQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "E3DLookAtQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !dstqptr ){
		DbgOut( "E3DLookAtQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//DbgOut( "e3dhsp : E3DLookAtQ : qid %d, savetwist %f\n", dstqid, savetwist );

	D3DXVECTOR3 basevec;
	basevec.x = basevecx;
	basevec.y = basevecy;
	basevec.z = basevecz;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}
	D3DXVECTOR3 tarvec;
	tarvec.x = vecx;
	tarvec.y = vecy;
	tarvec.z = vecz;
	DXVec3Normalize( &tarvec, &tarvec );


	ret = LookAtQ( dstqptr, tarvec, basevec, divnum, upflag );
	if( ret ){
		DbgOut( "E3DLookAtQ : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}
int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag )
{
	int ret;
	
	D3DXVECTOR3 newtarget;
	int newdivnum;

	float savetwist = dstqptr->twist;

	int saultflag = 0;

	if( ((tarvec.x == 0.0f) || (tarvec.x == -0.0f)) && 
		((tarvec.y == 0.0f) || (tarvec.y == -0.0f)) &&
		((tarvec.z == 0.0f) || (tarvec.z == -0.0f)) ){

		//zero vec���^�[�Q�b�g�̂Ƃ��́A�������^�[��
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	CQuaternion2 saveq0;
	saveq0 = *dstqptr;


	if( upflag == 2 ){

		D3DXVECTOR3 toppos;
		if( tarvec.y > 0.0f ){
			toppos.x = 0.0f;
			toppos.y = 1.0f;
			toppos.z = 0.0f;
		}else if( tarvec.y < 0.0f ){
			toppos.x = 0.0f;
			toppos.y = -1.0f;
			toppos.z = 0.0f;
		}else{
			toppos.x = 0.0f;//!!!!!!!!!!!! 2004/4/16
			toppos.y = 1.0f;
			toppos.z = 0.0f;
		}


		//sault�`�F�b�N
		ret = dstqptr->CheckSault( basevec, tarvec, upflag, &saultflag );
		if( ret ){
			DbgOut( "e3dhsp : LookAtQ : CheckSault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		// tarvec �x�V�t�g
		int shiftflag = 0;
		D3DXVECTOR3 shifttarvec;
		ret = dstqptr->ShiftVecNearY( basevec, tarvec, &shiftflag, &shifttarvec );
		if( ret ){
			DbgOut( "e3dhsp : LookAtQ : ShiftVecNearY error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( shiftflag == 1 )
			tarvec = shifttarvec;


		// toppos Y�V�t�g
		int shiftflag2 = 0;
		D3DXVECTOR3 shifttarvec2;
		ret = dstqptr->ShiftVecNearY( basevec, toppos, &shiftflag2, &shifttarvec2 );
		if( ret ){
			DbgOut( "e3dhsp : LookAtQ : ShiftVecNearY  2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( shiftflag2 == 1 )
			toppos = shifttarvec2;

//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, shift tarvec %f %f %f, toppos %f %f %f\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, toppos.x, toppos.y, toppos.z );


		if( (saultflag == 0) || (saultflag == 2) ){

			// �ړ��p�x���傫���ꍇ�́Atarget���߂��ɐݒ肵�����B���������A�ݒ肵�����B

			// ���ځAtarvec��ڎw���āA��]����B

			
			
			ret = dstqptr->GetDividedTarget( basevec, tarvec, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "e3dhsp : LookAtQ : GetDividedTarget error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			tarvec = newtarget;
			divnum = newdivnum;
			
			if( tarvec == toppos ){
				_ASSERT( 0 );
			}


//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, tarvec %f %f %f, divnum %d\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, divnum );


		}else if( saultflag == 1 ){
			
				
			// �ړ��p�x���傫���ꍇ�́Atarget���߂��ɐݒ肵�����B���������A�ݒ肵�����B

			// �܂��Atoppos���߂����āA��]����B

			ret = dstqptr->GetDividedTarget( basevec, toppos, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "e3dhsp : LookAtQ : GetDividedTarget error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			tarvec = newtarget;
			divnum = newdivnum;

			if( tarvec == toppos ){
				_ASSERT( 0 );
			}
			
		}
		
	}
		
	//DbgOut( "e3dhsp : LookAtQ : saultflag %d\n", saultflag );
	//DbgOut( "e3dhsp : LookAtQ : 0 : divnum %d\n", divnum );

	// target�ւ̉�]����
	CQuaternion2 finalq;
	CQuaternion2 rot1, rot2;
	D3DXVECTOR3 upvec;


	CQuaternion2 saveq;
	saveq = *dstqptr;


	finalq = *dstqptr;

	ret = finalq.LookAt( tarvec, basevec, 10000.0f, upflag, 0, 0, &upvec, &rot1, &rot2 );
	if( ret ){
		DbgOut( "e3dhsp : LookAtQ : fianlq LookAt error !!!\n" );
	}

	//
	//if( *dstqptr != finalq ){
	//	ret = finalq.GradUp( *dstqptr, basevec, upvec, rot1, rot2 );
	//	if( ret ){
	//		DbgOut( "e3dhsp : E3DLookAtQ : GradUp error !!!\n" );
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}
	



	CQuaternion2 firstq;
	firstq = *dstqptr;

	if( divnum >= 1 )
		*dstqptr = firstq.Slerp( finalq, divnum, 1 );
	else
		*dstqptr = finalq;

	{
		D3DXVECTOR3 finalvec;
		dstqptr->Rotate( &finalvec, basevec );
		DXVec3Normalize( &finalvec, &finalvec );

		D3DXVECTOR3 trabase;
		saveq.Rotate( &trabase, basevec );
		DXVec3Normalize( &trabase, &trabase );

		//if( finalvec == trabase ){
		//	DbgOut( "e3dhsp : E3DLookAtQ : not move warning %f %f %f!!!\n", finalvec.x, finalvec.y, finalvec.z );
		//}
		
		//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, tarvec %f %f %f, newtarget %f %f %f\n", saultflag, tarvec.x, tarvec.y, tarvec.z, newtarget.x, newtarget.y, newtarget.z );
	}


	dstqptr->twist = savetwist;


	//DbgOut( "e3dhsp : E3DLookAtQ : upflag %d, divnum %d\n", upflag, divnum );


	//*dstqptr = finalq;

	//CQuaternion2 firstq;
	//firstq = *dstqptr;
	//*dstqptr = firstq.Slerp( finalq, divnum, 1 );

	//dstqptr->twist = savetwist;


	dstqptr->normalize();
//!!!!! validate !!!!!!!!!!!!!
	if( (dstqptr->x >= -1.0f) && (dstqptr->x <= 1.0f) &&
		(dstqptr->y >= -1.0f) && (dstqptr->y <= 1.0f) &&
		(dstqptr->z >= -1.0f) && (dstqptr->z <= 1.0f) &&
		(dstqptr->w >= -1.0f) && (dstqptr->w <= 1.0f) ){

		// ���l�������Ă���
	}else{
		//���l�ȊO�̂��̂������Ă���\����

		*dstqptr = saveq0;

	}

	return 0;
}



/***
int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag )
{


	D3DXVECTOR3 upv, up0;
	D3DXVECTOR3 zm( 0.0f, 0.0f, -1.0f );
	CQuaternion upq;

	//twist inverse
	CQuaternion2 rotq0;
	rotq0.SetAxisAndRot( basevec, -dstqptr->twist * (float)DEG2PAI );
	*dstqptr = *dstqptr * rotq0;

	if( upflag == 0 ){
		upv = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	}else if( upflag == 1 ){
		upv = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	}else{
		up0 = D3DXVECTOR3( 0.0f, 1.0f, 0.0 );
		dstqptr->Rotate( &upv, up0 );
		DXVec3Normalize( &upv, &upv );
	}

	D3DXVECTOR3 xp( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 zp( 0.0f, 0.0f, 1.0f );

	D3DXVECTOR3 xz( basevec.x, 0.0f, basevec.z );
	CQuaternion baseq, invq;
	if( basevec != zp ){
		//baseq.RotationArc( zm, basevec );
		//baseq.RotationArc( basevec, zm );
		baseq.RotationArc( basevec, xz );
	}else{
		baseq.SetAxisAndRot( xp, 180.0f * (float)DEG2PAI );
	}
	D3DXVECTOR3 newtarget;
	baseq.Rotate( &newtarget, tarvec );
	DXVec3Normalize( &newtarget, &newtarget );
//	newtarget = tarvec;


	CQuaternion2 viewq, newq;

	if( upv != newtarget ){
		D3DXMATRIX viewmat;
		D3DXVECTOR3 eye( 0.0f, 0.0f, 0.0f );
		//if( basevec.y >= 0.9999997f ){
		//	D3DXMatrixLookAtLH( &viewmat, &eye, &newtarget, &upv );
		//}else{
			D3DXMatrixLookAtLH( &viewmat, &newtarget, &eye, &upv );
		//}
		D3DXQUATERNION viewqx;
		D3DXQuaternionRotationMatrix( &viewqx, &viewmat );
		viewq.SetParams( viewqx.w, viewqx.x, viewqx.y, viewqx.z, 0.0f );
		viewq.inv( &newq );
	}else{
		if( tarvec == newtarget ){
			newq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		}else{
			D3DXVECTOR3 axis;
			D3DXVec3Cross( &axis, &tarvec, &newtarget );
			DXVec3Normalize( &axis, &axis );
			CQuaternion axisq1, axisq2;
			axisq1.SetAxisAndRot( axis, 90.0f * (float)DEG2PAI );
			axisq2.SetAxisAndRot( axis, -90.0f * (float)DEG2PAI );
			D3DXVECTOR3 up1, up2;
			axisq1.Rotate( &up1, upv );
			axisq2.Rotate( &up2, upv );
			float dot1, dot2;
			dot1 = D3DXVec3Dot( &up1, &tarvec );
			dot2 = D3DXVec3Dot( &up2, &tarvec );
			D3DXVECTOR3 newup;
			if( dot1 < dot2 ){
				newup = up2;
			}else{
				newup = up1;
			}

			D3DXMATRIX viewmat;
			D3DXVECTOR3 eye( 0.0f, 0.0f, 0.0f );
			//D3DXMatrixLookAtLH( &viewmat, &eye, &newtarget, &newup );
			D3DXMatrixLookAtLH( &viewmat, &newtarget, &eye, &newup );
			D3DXQUATERNION viewqx;
			D3DXQuaternionRotationMatrix( &viewqx, &viewmat );
			viewq.SetParams( viewqx.w, viewqx.x, viewqx.y, viewqx.z, 0.0f );
			viewq.inv( &newq );
		}
		_ASSERT( 0 );
	}

	CQuaternion2 oldq = *dstqptr;

	if( divnum >= 1 )
		*dstqptr = oldq.Slerp( newq, divnum, 1 );
	else
		*dstqptr = newq;

	//twist
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( basevec, dstqptr->twist * (float)DEG2PAI );
	*dstqptr = *dstqptr * rotq;


	return 0;
}
***/

//E3DMultQVec qid, befvecx, befvecy, befvecz, aftvecx, aftvecy, aftvecz
	// ������Ԃ̃x�N�g����befvec�ɓn���΁A���݌����Ă�������x�N�g�������߂邱�Ƃ��o���܂��B

int E3DMultQVec()
{

//1: qid
	int qid;
	qid = code_getdi(0);

//2: befvecx
	float befvecx;
	befvecx = (float)code_getdd(0.0);

//3: befvecy
	float befvecy;
	befvecy = (float)code_getdd(0.0);

//4: befvecz
	float befvecz;
	befvecz = (float)code_getdd(0.0);


//5: aftvecx 
	PVal *pvalaftx;
	APTR aptraftx;
	aptraftx = code_getva( &pvalaftx );

//6: aftvecy 
	PVal *pvalafty;
	APTR aptrafty;
	aptrafty = code_getva( &pvalafty );

//7: aftvecz 
	PVal *pvalaftz;
	APTR aptraftz;
	aptraftz = code_getva( &pvalaftz );

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DMultQVec : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DMultQVec : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DMultQVec : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befvec, aftvec;
	befvec.x = befvecx;
	befvec.y = befvecy;
	befvec.z = befvecz;

	qptr->Rotate( &aftvec, befvec );

	double dx, dy, dz;
	dx = (double)aftvec.x;
	dy = (double)aftvec.y;
	dz = (double)aftvec.z;
	code_setva( pvalaftx, aptraftx, TYPE_DNUM, &dx );
	code_setva( pvalafty, aptrafty, TYPE_DNUM, &dy );
	code_setva( pvalaftz, aptraftz, TYPE_DNUM, &dz );

	return 0;
}


//E3DTwistQ qid, twistdeg, degmult, basex, basey, basez
int E3DTwistQ()
{

//1: qid
	int qid;
	qid = code_getdi(0);

//2: twistdeg
	float twistdeg;
	twistdeg = (float)code_getdd(0.0);

//3: basex
	float basex;
	basex = (float)code_getdd(0.0);

//4: basey
	float basey;
	basey = (float)code_getdd(0.0);

//5: basez
	float basez;
	basez = (float)code_getdd(0.0);

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 axis;

	basevec.x = basex;
	basevec.y = basey;
	basevec.z = basez;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}

	CQuaternion2 rotq;
	rotq.SetAxisAndRot( basevec, twistdeg * (float)DEG2PAI );

	*qptr = *qptr * rotq;


	float savetwist;
	savetwist = qptr->twist;
	// 0 - 360
	float finaltwist, settwist;
	finaltwist = savetwist + twistdeg;

	if( finaltwist >= 360.0f ){
		while( finaltwist >= 360.0f )
			finaltwist -= 360.0f;
		settwist = finaltwist;
	}else if( finaltwist < 0.0f ){
		while( finaltwist < 0.0f )
			finaltwist += 360.0f;
		settwist = finaltwist;
	}else{
		settwist = finaltwist;
	}
	qptr->twist = settwist;


	//DbgOut( "e3dhsp : E3DTwistQ : ftwist %f, qptr->twist %f\n", ftwist, qptr->twist );
		return 0;
}


//E3DInitTwistQ qid, basex, basey, basez
int E3DInitTwistQ()
{
	//type $202

//1: qid
	int qid;
	qid = code_getdi(0);

//2: basex
	float basex;
	basex = (float)code_getdd(0.0);

//3: basey
	float basey;
	basey = (float)code_getdd(0.0);

//4: basez
	float basez;
	basez = (float)code_getdd(0.0);

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DInitTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DInitTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DInitTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 axis;

	basevec.x = basex;
	basevec.y = basey;
	basevec.z = basez;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}
	D3DXVECTOR3 upvec( 0.0f, 1.0f, 0.0f );
	CQuaternion2 basecheckq;
	basecheckq.RotationArc( basevec, upvec );
	D3DXVECTOR3 chkbaseaxis;
	float chkbaserad;
	basecheckq.GetAxisAndRot( &chkbaseaxis, &chkbaserad );
	if( fabs( chkbaserad ) < 0.1f * (float)DEG2PAI ){
		D3DXVECTOR3 chgaxis( 1.0f, 0.0f, 0.0f );
		CQuaternion2 chgq;
		chgq.SetAxisAndRot( chgaxis, 0.1f * (float)DEG2PAI );
		chgq.Rotate( &basevec, basevec );

		DXVec3Normalize( &basevec, &basevec );
	}


	qptr->Rotate( &axis, basevec );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 twistq;
	float invtwist;
	invtwist = qptr->twist * (float)DEG2PAI * -1.0f;
	twistq.SetAxisAndRot( axis, invtwist );

//DbgOut( "e3dhsp : E3DInitTwistQ : qid %d, axis %f %f %f, rad of invtwist %f twist %f\n", qid, axis.x, axis.y, axis.z, invtwist, qptr->twist );

	*qptr = twistq * *qptr;
	
	qptr->twist = 0.0f;

//DbgOut( "e3dhsp : E3DInitTwistQ : afttwist %f\n", qptr->twist );

	return 0;
}


//E3DGetTwistQ qid, twist10000
int E3DGetTwistQ()
{

//1: qid
	int qid;
	qid = code_getdi(0);


//2: twist 
	PVal *pvaltwist;
	APTR aptrtwist;
	aptrtwist = code_getva( &pvaltwist );

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DGetTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DGetTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DGetTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dtwist;
	dtwist = (double)qptr->twist;
	code_setva( pvaltwist, aptrtwist, TYPE_DNUM, &dtwist );

	return 0;
}

//E3DRotateQLocalX qid, deg, degmult
int E3DRotateQLocalX()
{

//1: qid
	int qid;
	qid = code_getdi(0);

//2: deg
	float deg;
	deg = (float)code_getdd(0.0);

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalX : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalX : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalX : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		

	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 1.0f, 0.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}


//E3DRotateQLocalY qid, deg, degmult
int E3DRotateQLocalY()
{

//1: qid
	int qid;
	qid = code_getdi(0);

//2: deg
	float deg;
	deg = (float)code_getdd(0.0);

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalY : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalY : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalY : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		

	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}



//E3DRotateQLocalZ qid, deg, degmult
int E3DRotateQLocalZ()
{

//1: qid
	int qid;
	qid = code_getdi(0);

//2: deg
	float deg;
	deg = (float)code_getdd(0.0);

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalZ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalZ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalZ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 0.0f, 1.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}

//E3DGetBonePos hsid, boneno, poskind, motid, frameno, posx, posy, posz, scaleflag
//	local���W�ƃO���[�o�����W��2���
int E3DGetBonePos()
{
	// type $202
	// 1 : handlerset id
	// 2 : boneno
	// 3 : poskind
	// 4 : motid
	// 5 : frameno

	// 6 : posxptr
	// 7 : posyptr
	// 8 : poszptr

	// 9 : scaleflag

	
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int poskind;
	poskind = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

	int scaleflag;
	scaleflag = code_getdi(0);


///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetBonePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 dstpos;

	ret = curhs->GetBonePos( boneno, poskind, motid, frameno, scaleflag, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetBonePos : curhs GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;
	code_setva( pvalx, aptrx, TYPE_DNUM, &dx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dy );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dz );

	return 0;

}

//E3DCreateLine pointarray, pointnum, maxpointnum, linekind, &lineid
		//p[pointnum][3]
		//videomemory�́Amaxpointnum�ŃA���P�[�g����
		//linekind --> LINELIST = 0, LINESTRIP = 1
int E3DCreateLine()
{

	PVal *pvalpos;
	APTR aptrpos;
	aptrpos = code_getva( &pvalpos );


	int pointnum;
	pointnum = code_getdi(0);

	int maxpointnum;
	maxpointnum = code_getdi(0);

	int linekind;
	linekind = code_getdi(0);

	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


/////////////////////

	if( (linekind != 2) && (linekind != 3) ){
		DbgOut( "e3dhsp : E3DCreateLine : invalid linekind warning !!! : set default value \n" );
		_ASSERT( 0 );

		linekind = 3;//!!!!!!!
	}

	
	if( pointnum < 2 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum too small error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( maxpointnum < pointnum ){
		maxpointnum = pointnum;//!!!!!
	}
	
	if( pointnum > 2147483647 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum too large : fixed to 2147483647 : warning !!!\n" );
		pointnum = 2147483647;
	}
	
	if( maxpointnum > 2147483647 ){
		DbgOut( "e3dhsp : E3DCreateLine : maxpointnum too large : fixed to 2147483647 : warning !!!\n" );
		maxpointnum = 2147483647;
	}

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	newhs->m_extlineflag = 1;//!!!!!!!!

	ret = newhs->CreateLine( (double*)pvalpos->pt + aptrpos, pointnum, maxpointnum, linekind );
	if( ret ){
		DbgOut( "e3dhsp : E3DCreateLine : newhs CreateLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	//ret = newhs->SetCurrentBSphereData();
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	code_setva( pvalid, aptrid, TYPE_INUM, &newhs->serialno );


	return 0;
}
	

	//E3DDestroyLine lineid
int E3DDestroyLine()
{

	int hsid;
	hsid = code_getdi(0);
	
///////

	int ret;
	ret = DestroyHandlerSet( hsid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyLine : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetLineColor lineid, a, r, g, b
int E3DSetLineColor()
{

	int lid;
	lid = code_getdi(0);

	int a;
	a = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

/////////////////////

	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : SetLineColor : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( a < 0 )
		a = 0;
	else if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetExtLineColor( a, r, g, b );
	if( ret ){
		DbgOut( "e3dhsp : SetLineColor : curhs SetExtLineColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DAddPoint2Line lineid, prevpid, &newpid
		//previd == -2�̂Ƃ��擪�A-1�̂Ƃ��Ō�
int E3DAddPoint2Line()
{


	int lid;
	lid = code_getdi(0);

	int previd;
	previd = code_getdi(0);

	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : AddPoint2Line : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int newid = -1;
	ret = curhs->AddPoint2ExtLine( previd, &newid );
	if( ret ){
		DbgOut( "e3dhsp : AddPoint2Line : curhs AddPoint2ExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &newid );

	return 0;
}

//E3DDeletePointOfLine lineid, pid
int E3DDeletePointOfLine()
{
	int lid;
	lid = code_getdi(0);

	int pid;
	pid = code_getdi(0);
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : DeletePointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeletePointOfExtLine( pid );
	if( ret ){
		DbgOut( "e3dhsp : DeletePointOfLine : curhs DeletePointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetPointPosOfLine lineid, pid, pos[3]
int E3DSetPointPosOfLine()
{


	int lid;
	lid = code_getdi(0);

	int pid;
	pid = code_getdi(0);

	PVal *pvalpos;
	APTR aptrpos;
	aptrpos = code_getva( &pvalpos );

/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetPointPosOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcpos;
	srcpos.x = (float)( *((double*)pvalpos->pt + aptrpos) );
	srcpos.y = (float)( *((double*)pvalpos->pt + aptrpos + 1) );
	srcpos.z = (float)( *((double*)pvalpos->pt + aptrpos + 2) );

	int ret;
	ret = curhs->SetPointPosOfExtLine( pid, srcpos );
	if( ret ){
		DbgOut( "e3dhsp : SetPointPosOfLine : curhs SetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetPointPosOfLine lineid, pid, pos[3]
int E3DGetPointPosOfLine()
{


	int lid;
	lid = code_getdi(0);

	int pid;
	pid = code_getdi(0);

	PVal *pvalpos;
	APTR aptrpos;
	aptrpos = code_getva( &pvalpos );

/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetPointPosOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 dstpos;

	int ret;
	ret = curhs->GetPointPosOfExtLine( pid, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetPointPosOfLine : curhs GetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;
	code_setva( pvalpos, aptrpos, TYPE_DNUM, &dx );
	code_setva( pvalpos, aptrpos + 1, TYPE_DNUM, &dy );
	code_setva( pvalpos, aptrpos + 2, TYPE_DNUM, &dz );

	return 0;
}

//E3DGetNextPointOfLine lineid, prevpid, &nextpid
	//pointid ��next �`�F�C����pointid��Ԃ��Bpointid == -1 �̂Ƃ��A�擪point��Ԃ�nextid == -1�̂Ƃ��́Anext�Ȃ��A
int E3DGetNextPointOfLine()
{

	int lid;
	lid = code_getdi(0);

	int previd;
	previd = code_getdi(0);

	PVal *pvalnext;
	APTR aptrnext;
	aptrnext = code_getva( &pvalnext );

/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetNextPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int next = -1;
	ret = curhs->GetNextPointOfExtLine( previd, &next );
	if( ret ){
		DbgOut( "e3dhsp : GetNextPointOfLine : curhs GetNextPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnext, aptrnext, TYPE_INUM, &next );

	return 0;
}


//E3DGetPrevPointOfLine lineid, pid, &prevpid
	// befid == -1�̂Ƃ��́Abef�Ȃ�
int E3DGetPrevPointOfLine()
{

	int lid;
	lid = code_getdi(0);

	int pid;
	pid = code_getdi(0);

	PVal *pvalprev;
	APTR aptrprev;
	aptrprev = code_getva( &pvalprev );

/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetPrevPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prev = -1;
	ret = curhs->GetPrevPointOfExtLine( pid, &prev );
	if( ret ){
		DbgOut( "e3dhsp : GetPrevPointOfLine : curhs GetPrevPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalprev, aptrprev, TYPE_INUM, &prev );

	return 0;
}


//WriteDisplay2BMP bmpname
int E3DWriteDisplay2BMP()
{
	int ret;
	char bmpname[MAX_PATH];
	ret = GetName( bmpname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DWriteDisplay2BMP : bmpname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int scid;
	scid = code_getdi(0);

/////////////////////

	
	
	//�A���`���I���̎��́A�G���[�ɂ���B�i�ǂ����݂�Lock�ŃG���[�ɂȂ�j
	if( s_multisample != 0 ){
		DbgOut( "E3DWriteDisplay2BMP : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	LPDIRECT3DDEVICE9 pd3dDevice;
	ret = g_pD3DApp->GetD3DDevice( &pd3dDevice );
	if( ret || !pd3dDevice ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : d3dDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
/***
	LPDIRECT3DSURFACE9 pBack;
	HRESULT hr;
	hr = pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBack );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DWriteDisplay2BMP : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;

	HRESULT hr;
	D3DSURFACE_DESC desc;
	hr = pBack->GetDesc( &desc );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int backformat;
	UINT backWidth, backHeight;
	backformat = desc.Format;
	backWidth = desc.Width;
	backHeight = desc.Height;

	D3DLOCKED_RECT lockedrect;
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = backWidth;
	rect.bottom = backHeight;

////
	hr = pBack->LockRect( &lockedrect, &rect, 0 );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : pback LockRect error !!!\n" );		
		_ASSERT( 0 );
		pBack->Release();
		return 1;
	}

		//DbgOut( "MotDlg:WriteBmp : width %d, height %d, pitch %d\n",
		//	backWidth, backHeight, lockedrect.Pitch );

	int bmpwidth, bmpheight;
	bmpheight = backHeight;
	int wcnt;
	wcnt = backWidth / 4;
	bmpwidth = wcnt * 4;

	int enmark = '\\';
	char* enpos = 0;
	enpos = strchr( bmpname, enmark );

	char outputname[2048];//
	ZeroMemory( outputname, 2048 );

	if( !enpos ){

		char moduledir[2048];
		int leng;
		ZeroMemory( moduledir, 2048 );
		leng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, 2048 );
		if( (leng <= 0) || (leng >= 2048) ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetEnvironmentVariable error !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}


		int chkleng;
		chkleng = leng + (int)strlen( bmpname );

		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}

		strcpy( outputname, moduledir );
		strcat( outputname, bmpname );

		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : outputname0 %s\n", outputname );
	}else{
		int chkleng;
		chkleng = (int)strlen( bmpname );
		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}
		strcpy( outputname, bmpname );
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : outputname1 %s\n", outputname );
	
	}

	CDibFile* dibfile;
	dibfile = new CDibFile( outputname, bmpwidth, bmpheight, 24 );
	if( !dibfile ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile alloc error !!!\n" );
		_ASSERT( 0 );
		pBack->UnlockRect();
		pBack->Release();
		return 1;
	}


	BYTE* bitshead;
	int hno;
	for( hno = 0; hno < bmpheight; hno++ ){
		//bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		//�㉺���]����B
		bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		ret = dibfile->SetRGB( bmpheight - 1 - hno, bitshead, backformat );
		if( ret ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile SetRGB error !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			delete dibfile;
			return 1;
		}
	}

	ret = dibfile->CreateDibFile();
	if( ret ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile CreateFile error !!!\n" );
		_ASSERT( 0 );
		pBack->UnlockRect();
		pBack->Release();
		delete dibfile;
		return 1;
	}
	delete dibfile;

	hr = pBack->UnlockRect();
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : pback UnlockRect error !!!\n" );
		_ASSERT( 0 );
		pBack->Release();
		return 1;
	}
	//pd3dDevice->Present( NULL, NULL, NULL, NULL );


	pBack->Release();


	switch( backformat ){
	case D3DFMT_X1R5G5B5:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format X1R5G5B5\n" );
		break;
	case D3DFMT_R5G6B5:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format R5G6B5\n" );
		break;
	case D3DFMT_X8R8G8B8:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format X8R8G8B8\n" );
		break;
	case D3DFMT_A8R8G8B8:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format A8R8G8B8\n" );
		break;
	default:
		break;
	}

	return 0;
}

//E3DCreateAVIFile filename, datatype, compkind, framerate, frameleng, aviid
		// �t�@�C�����́A�g���q�Ȃ�
		// ���o�[�W�����ł́Adatatype �͂P�Fvideo�̂�
		// compkind
		//	0 --> ���k����
		//	1 --> cinepak Codec by Radius
		//	2 --> Microsoft Video 1
		//�I������complete���Ă��Ȃ�avi�́A�����I��complete����B(������pdev���L���ȊԂɂ��Ȃ��Ƃ��߁I�I�I)

int E3DCreateAVIFile()
{

	int ret;
//1
	char aviname[MAX_PATH];
	ret = GetName( aviname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateAVIFile : aviname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2:
	int datatype;
	datatype = code_getdi(1);

// 3:
	int compkind;
	compkind = code_getdi(1);

// 4:
	int framerate;
	framerate = code_getdi(0);

// 5:
	int frameleng;
	frameleng = code_getdi(0);

// 6:
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

// 7:
	int scid;
	scid = code_getdi(0);

//////

	//�A���`���I���̎��́A�G���[�ɂ���B�i�ǂ����݂�Lock�ŃG���[�ɂȂ�j
	if( s_multisample != 0 ){
		DbgOut( "E3DCreateAVIFile : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DCreateAVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;


	CAviData* newavi;
	newavi = new CAviData();
	if( !newavi ){
		DbgOut( "E3DCreateAVIFile : newavi alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DCreateAVIFile : this datatype not supported warning !!! set default value 1\n" );
		_ASSERT( 0 );
		datatype = 1;//!!!!!!!
	}

	if( (framerate < 0) || (framerate > 200) ){
		DbgOut( "E3DCreateAVIFile : framerate out of range warning !!! set default value 30 \n" );
		_ASSERT( 0 );
		framerate = 30;
	}

	if( frameleng < 0 ){
		DbgOut( "E3DCreateAVIFile : invalid frameleng warning !!! set default value 60 \n" );
		_ASSERT( 0 );
		frameleng = 60;
	}

	ret = newavi->CreateAVIFile( pBack, aviname, datatype, compkind, framerate, frameleng );
	if( ret ){
		DbgOut( "E3DCreateAVIFile : newavi CreateAVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CAviData* curavi = avihead;
	CAviData* largeravi = 0;
	CAviData* smalleravi = 0;

	while( curavi ){
		if( newavi->serialno < curavi->serialno ){
			largeravi = curavi;
			break;
		}else{
			smalleravi = curavi;
		}
		curavi = curavi->next;
	}

	if( !avihead ){
		//�擪�ɒǉ�
		newavi->ishead = 1;
		avihead = newavi;
	}else{
		if( largeravi ){
			//largermp�̑O�ɒǉ��B
			ret = largeravi->AddToPrev( newavi );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
			if( newavi->ishead )
				avihead = newavi;
		}else{
			//�Ō�ɒǉ��B(smallermp �̌�ɒǉ�)
			_ASSERT( smalleravi );
			ret = smalleravi->AddToNext( newavi );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &newavi->serialno );

	return 0;

}
//E3DWriteData2AVIFile aviid, datatype
		//completeflag�������Ă���ꍇ��A�f�[�^����frameleng�ɒB���Ă���ꍇ�́Areturn ����B
int E3DWriteData2AVIFile()
{

// 1:
	int aviid;
	aviid = code_getdi(0);

// 2:
	int datatype;
	datatype = code_getdi(1);
// 3:
	int scid;
	scid = code_getdi(0);

//////

	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DWriteData2AVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DWriteData2AVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;


	CAviData* curavi;
	curavi = GetAviData( aviid );
	if( !curavi ){
		DbgOut( "E3DWriteData2AVIFile : aviid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DWriteData2AVIFile : this datatype not supported warning !!! set default value 1 \n" );
		_ASSERT( 0 );
		datatype = 1;
	}


	ret = curavi->Write2AVIFile( pBack, datatype );
	if( ret ){
		DbgOut( "E3DWriteData2AVIFile : curavi Write2AVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}

//E3DCompleteAVIFile aviid
		//�f�[�^��������Ȃ��ꍇ�́A�_�~�[�f�[�^��t������complete����B
int E3DCompleteAVIFile()
{

// 1:
	int aviid;
	aviid = code_getdi(0);

// 2:
	int datatype;
	datatype = code_getdi(1);
// 3:
	int scid;
	scid = code_getdi(0);

//////

	int ret;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCompleteAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DCompleteAVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;

	CAviData* curavi;
	curavi = GetAviData( aviid );
	if( !curavi ){
		DbgOut( "E3DCompleteAVIFile : aviid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DCompleteAVIFile : this datatype not supported warning !!! set default value 1 \n" );
		_ASSERT( 0 );
		datatype = 1;
	}


	ret = curavi->CompleteAVIFile( pBack, datatype );
	if( ret ){
		DbgOut( "E3DCompleteAVIFile : curavi CompleteAVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCameraLookAt
//	E3DCameraLookAt vecx, vecy, vecz, upflag, divnum
int E3DCameraLookAt()
{
//1: vecx
	float vecx;
	vecx = (float)code_getdd(0.0);

//2: vecy
	float vecy;
	vecy = (float)code_getdd(0.0);

//3: vecz
	float vecz;
	vecz = (float)code_getdd(1.0);

//4: upflag
	int upflag;
	upflag = code_getdi(2);

//5: divnum
	int divnum;
	divnum = code_getdi(1);

///////

	D3DXVECTOR3 basevec;
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = 1.0f;
	DXVec3Normalize( &basevec, &basevec );

	D3DXVECTOR3 tarvec;
	tarvec.x = (float)vecx;
	tarvec.y = (float)vecy;
	tarvec.z = (float)vecz;
	DXVec3Normalize( &tarvec, &tarvec );

	int ret;
	CQuaternion2 invq;

	s_cameraq.inv( &invq );//!!!!!
	invq.twist = s_camera_twist;//!!!!!!!

	ret = LookAtQ( &invq, tarvec, basevec, divnum, upflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraLookAt : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	invq.inv( &s_cameraq );//!!!!!


	D3DXVECTOR3 dirvec;
	invq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

	//s_camerapos =
	s_cameratarget = s_camerapos + dirvec;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	return 0;
}


//E3DCameraOnNaviLine nlid, mode, roundflag, reverseflag, offx, offy, offz, posstep, dirdivnum, upflag, tpid
//	mode = 0 -> �ʒu�̂�
//	mode = 1 -> �ʒu�ƌ���
//	posstep : �}�C�i�X�ŁA�������w��B
//	dirdivnum : ������
int E3DCameraOnNaviLine()
{

//1: nlid
	int nlid;
	nlid = code_getdi(0);

//2: mode
	int mode;
	mode = code_getdi(1);

//3: roundflag
	int roundflag;
	roundflag = code_getdi(0);

//4: reverseflag
	int reverseflag;
	reverseflag = code_getdi(0);

//5: offx
	float offx;
	offx = (float)code_getdd(0.0);

//6: offy
	float offy;
	offy = (float)code_getdd(0.0);

//7: offz
	float offz;
	offz = (float)code_getdd(0.0);


//8: posstep
	float posstep;
	posstep = (float)code_getdd(100.0);

//9: dirdivnum
	int dirdivnum;
	dirdivnum = code_getdi(10);

//10: upflag
	int upflag;
	upflag = code_getdi(2);

//11: tpidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

///////

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DCameraOnNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offset;
	offset.x = offx;
	offset.y = offy;
	offset.z = offz;


	D3DXVECTOR3 newpos;
	CQuaternion newq;

	float fdegstep = 360.0f;

	CQuaternion tempdirq;
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	tempdirq.CopyFromCQuaternion2( &invq );//!!!!!!!!!

	int tpid = -1;
	ret = curnl->ControlByNaviLine( s_camerapos, offset, tempdirq, 
			1, roundflag, reverseflag,
			1000.0f, 
			posstep, fdegstep,
			&newpos, &newq, &tpid );
	if( ret ){
		DbgOut( "E3DCameraOnNaviLine : curnl ControlByNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 invq2;
	invq2.w = invq.w;
	invq2.x = invq.x;
	invq2.y = invq.y;
	invq2.z = invq.z;
	//invq2.twist = 0.0f;
	invq2.twist = s_camera_twist;//!!!!!!!!!

	D3DXVECTOR3 basevec;
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = 1.0f;


	if( mode == 1 ){
		D3DXVECTOR3 tarvec;
		tarvec = newpos - s_camerapos;
		float tarmag;
		tarmag = tarvec.x * tarvec.x + tarvec.y * tarvec.y + tarvec.z * tarvec.z;
		if( tarmag >= 0.001f ){
			DXVec3Normalize( &tarvec, &tarvec );
			ret = LookAtQ( &invq2, tarvec, basevec, dirdivnum, upflag );
			if( ret ){
				DbgOut( "e3dhsp : E3DCameraOnNaviLine : LookAtQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		invq2.inv( &s_cameraq );//!!!!!
	}

	D3DXVECTOR3 dirvec;
	invq2.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

	s_cameratarget = newpos + dirvec;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq2.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;

	s_camerapos = newpos;//!!!!!
	

	code_setva( pvalid, aptrid, TYPE_INUM, &tpid );

	return 0;
}


//	E3DCameraUp deg, divnum
int E3DCameraDirUp()
{

//1: deg
	float deg;
	deg = (float)code_getdd(0.0);

//2: divnum
	int divnum;
	divnum = code_getdi(10);

//// target�����߂�
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldy( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newz, newy;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newy, oldy );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newz, &newy );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////�@������𒲐�����B
	
	invq.twist = s_camera_twist;//!!!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );

	return 0;
}


//	E3DCameraDown deg, divnum
int E3DCameraDirDown()
{

//1: deg
	float deg;
	deg = (float)code_getdd(0.0);
//2: divnum
	int divnum;
	divnum = code_getdi(10);

//// target�����߂�
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldy( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newz, newy;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newy, oldy );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newz, &newy );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, -rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////�@������𒲐�����B
	
	invq.twist = s_camera_twist;//!!!!!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );


	return 0;
}
//	E3DCameraRight deg, divnum
int E3DCameraDirRight()
{

//1: deg
	float deg;
	deg = (float)code_getdd(0.0);
//2: divnum
	int divnum;
	divnum = code_getdi(10);

//// target�����߂�
	CQuaternion2 invq;
	s_cameraq.inv( &invq );

	CQuaternion2 saveinvq;
	saveinvq = invq;


	float rad;
	rad = deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldx( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 newz, newx;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newx, oldx );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newx, &newz );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, -rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////�@������𒲐�����B
	
	invq.twist = s_camera_twist;//!!!!!

/***
	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	if( divnum >= 1 )
		invq = saveinvq.Slerp( newq, divnum, 1 );


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );

	return 0;
}
//	E3DCameraLeft deg, divnum
int E3DCameraDirLeft()
{

//1: deg
	float deg;
	deg = (float)code_getdd(0.0);
//2: divnum
	int divnum;
	divnum = code_getdi(10);

//// target�����߂�
	CQuaternion2 invq;
	s_cameraq.inv( &invq );

	CQuaternion2 saveinvq;
	saveinvq = invq;

	float rad;
	rad = deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldx( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 newz, newx;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newx, oldx );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newx, &newz );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////�@������𒲐�����B
	
	invq.twist = s_camera_twist;//!!!!!!!!

	/***
	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
	if( divnum >= 1 )
		invq = saveinvq.Slerp( newq, divnum, 1 );


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );


	return 0;
}

//E3DCreateFont height, width, weight, bItalic, bUnderline, bStrikeout, fontname, &fontidptr
int E3DCreateFont()
{

	int ret;
//1
	int height;
	height = code_getdi(0);

//2
	int width;
	width = code_getdi(0);

//3
	int weight;
	weight = code_getdi(0);

//4
	int bItalic;
	bItalic = code_getdi(0);

//5
	int bUnderline;
	bUnderline = code_getdi(0);

//6
	int bStrikeout;
	bStrikeout = code_getdi(0);

//7
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateFont : name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//8
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateFont : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPD3DXSPRITE fontsprite;
	hr = D3DXCreateSprite( pdev, &fontsprite );//release��xfont�ōs���B
	if( hr != D3D_OK ){
		DbgOut( "e3dRestoreDeviceObject : D3DXCreateSprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CXFont* newfont;
	newfont = new CXFont();
	if( !newfont ){
		DbgOut( "E3DCreateFont : newfont alloc error !!!\n" );
		_ASSERT( 0 );
		fontsprite->Release();
		return 1;
	}

	ret = newfont->CreateFont( pdev, fontsprite, height, width, weight, bItalic, bUnderline, bStrikeout, tempname );
	if( ret ){
		DbgOut( "E3DCreateFont : newfont CreateFont error !!!\n" );
		_ASSERT( 0 );
		fontsprite->Release();
		return 1;
	}


/////////
	CXFont* curfont = xfonthead;
	CXFont* largerfont = 0;
	CXFont* smallerfont = 0;

	while( curfont ){
		if( newfont->m_serialno < curfont->m_serialno ){
			largerfont = curfont;
			break;
		}else{
			smallerfont = curfont;
		}
		curfont = curfont->next;
	}

	if( !xfonthead ){
		//�擪�ɒǉ�
		newfont->ishead = 1;
		xfonthead = newfont;
	}else{
		if( largerfont ){
			//largermp�̑O�ɒǉ��B
			ret = largerfont->AddToPrev( newfont );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
			if( newfont->ishead )
				xfonthead = newfont;
		}else{
			//�Ō�ɒǉ��B(smallermp �̌�ɒǉ�)
			_ASSERT( smallerfont );
			ret = smallerfont->AddToNext( newfont );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &newfont->m_serialno );

	return 0;
}

//E3DDrawTextByFontID fontid, posx, posy, textstr, a, r, g, b
int E3DDrawTextByFontID()
{
	int ret;
//1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontID : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	int fontid;
	fontid = code_getdi(0);

//3
	int posx;
	posx = code_getdi(0);

//4
	int posy;
	posy = code_getdi(0);

//5
	char tempname[4099];
	ret = GetName( tempname, 4097 );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//6
	int a;
	a = code_getdi(0);

//7
	int r;
	r = code_getdi(0);

//8
	int g;
	g = code_getdi(0);

//9
	int b;
	b = code_getdi(0);

//////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}



	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontID : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontID : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );


	DisableFogState();

	ret = curfont->DrawText( tempname, posx, posy, col );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

//E3DDrawTextByFontIDWithCnt fontid, posx, posy, textstr, a, r, g, b, eachcnt, curcnt
int E3DDrawTextByFontIDWithCnt()
{
//1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//2
	int fontid;
	fontid = code_getdi(0);

//3
	int posx;
	posx = code_getdi(0);

//4
	int posy;
	posy = code_getdi(0);

//5
	int ret;
	char tempname[4099];
	ret = GetName( tempname, 4097 );
	if( ret ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//6
	int a;
	a = code_getdi(0);

//7
	int r;
	r = code_getdi(0);

//8
	int g;
	g = code_getdi(0);

//9
	int b;
	b = code_getdi(0);

//10
	int eachcnt;
	eachcnt = code_getdi(0);

//11
	int curcnt;
	curcnt = code_getdi(0);

//////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}


	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );

	DisableFogState();

	ret = curfont->DrawTextWithCnt( tempname, posx, posy, col, eachcnt, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

//E3DDrawTextByFontIDWithCnt2 fontid, posx, posy, textstr, a, r, g, b, cntarray, arrayleng, curcnt
int E3DDrawTextByFontIDWithCnt2()
{

//1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : scid error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

//2
	int fontid;
	fontid = code_getdi(0);

//3
	int posx;
	posx = code_getdi(0);

//4
	int posy;
	posy = code_getdi(0);

//5
	int ret;
	char tempname[4099];
	ret = GetName( tempname, 4097 );
	if( ret ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//6
	int a;
	a = code_getdi(0);

//7
	int r;
	r = code_getdi(0);

//8
	int g;
	g = code_getdi(0);

//9
	int b;
	b = code_getdi(0);

//10
	PVal *pvalarray;
	APTR aptrarray;
	aptrarray = code_getva( &pvalarray );

//11
	int arrayleng;
	arrayleng = code_getdi(0);

//12
	int curcnt;
	curcnt = code_getdi(0);

//////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}


	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );

	DisableFogState();

	ret = curfont->DrawTextWithCnt2( tempname, posx, posy, col, (int*)pvalarray->pt + aptrarray, arrayleng, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID2 : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

//E3DGetCharacterNum textstr, numptr
int E3DGetCharacterNum()
{


//1
	int ret;
	char tempname[4097];
	ret = GetName( tempname, 4097 );
	if( ret ){
		DbgOut( "E3DGetCharacterNum : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2	
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

//////

	if( !tempname[0] ){
		int zeronum = 0;
		code_setva( pvalnum, aptrnum, TYPE_INUM, &zeronum );
		return 0;
	}

	unsigned char curc;
	int totalnum = 0;
	int curpos = 0;
	
	int orgleng;
	orgleng = (int)strlen( tempname );
	if( orgleng <= 0 )
		return 0;

	while( curpos < orgleng ){
		curc = tempname[curpos];

		if( curc == 0 )
			break;

		if( curc < 0x80 ){
			curpos++;
			//DbgOut( "GetCharaNum : 1byte : %c, %d\n", curc, curc );
		}else{
			curpos += 2;
			//DbgOut( "GetCharaNum : 2byte : %c, %d\n", curc, curc );
		}
		totalnum++;
	}


	//DbgOut( "GetCharaNum : \\r %d, \\n %d\n", '\r', '\n' ); 

	code_setva( pvalnum, aptrnum, TYPE_INUM, &totalnum );


	return 0;
}

//E3DDestroyFont fontid
int E3DDestroyFont()
{
//1
	int fontid;
	fontid = code_getdi(0);

//////

	CXFont* beffont = 0;
	CXFont* curfont = xfonthead;
	CXFont* findfont = 0;

	while( curfont && (findfont == 0) ){
		if( curfont->m_serialno == fontid ){
			findfont = curfont;
			break;
		}
		beffont = curfont;
		curfont = curfont->next;
	}

	if( findfont ){
		if( beffont ){
			beffont->next = findfont->next;
			if( findfont->next )
				findfont->next->prev = beffont;
			delete findfont;
		}else{
			xfonthead = findfont->next;
			if( findfont->next ){
				findfont->next->prev = 0;
				findfont->next->ishead = 1;
			}
			delete findfont;
		}
	}else{
		return 0;
	}

	return 0;
}

//E3DSlerpQ startqid, endqid, t, tdiv, resqid
int E3DSlerpQ()
{
//1
	int startqid;
	startqid = code_getdi(0);

//2
	int endqid;
	endqid = code_getdi(0);

//3
	float t;
	t = (float)code_getdd(0.0);

//4
	int resqid;
	resqid = code_getdi(0);

//////

	if( !s_qh ){
		DbgOut( "E3DSlerpQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* startq2;
	CQuaternion2* endq2;
	CQuaternion2* resq2;

	int ret;
	ret = s_qh->GetQ( startqid, &startq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ startq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( endqid, &endq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ endq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( resqid, &resq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ resq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resq2 = startq2->Slerp( *endq2, 10000, (int)( t * 10000.0f ) );
	

	return 0;
}


//E3DSquadQ befqid, startqid, endqid, aftqid, t, tdiv, resqid
int E3DSquadQ()
{
//1
	int befqid;
	befqid = code_getdi(0);

//2
	int startqid;
	startqid = code_getdi(0);

//3
	int endqid;
	endqid = code_getdi(0);

//4
	int aftqid;
	aftqid = code_getdi(0);

//5
	float t;
	t = (float)code_getdd(0.0);

//6
	int resqid;
	resqid = code_getdi(0);

//////

	if( !s_qh ){
		DbgOut( "E3DSquadQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* befq2;
	CQuaternion2* startq2;
	CQuaternion2* endq2;
	CQuaternion2* aftq2;
	CQuaternion2* resq2;

	int ret;
	ret = s_qh->GetQ( befqid, &befq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ befq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( startqid, &startq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ startq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( endqid, &endq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ endq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( aftqid, &aftq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ aftq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( resqid, &resq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ resq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = resq2->Squad( *befq2, *startq2, *endq2, *aftq2, t );
	if( ret ){
		DbgOut( "E3DSquadQ : resq Squad error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSplineVec pos0, pos1, pos2, pos3, t, tdiv, respos
int E3DSplineVec()
{

//1
	PVal *pvalpos0;
	APTR aptrpos0;
	aptrpos0 = code_getva( &pvalpos0 );

//2
	PVal *pvalpos1;
	APTR aptrpos1;
	aptrpos1 = code_getva( &pvalpos1 );

//3
	PVal *pvalpos2;
	APTR aptrpos2;
	aptrpos2 = code_getva( &pvalpos2 );

//4
	PVal *pvalpos3;
	APTR aptrpos3;
	aptrpos3 = code_getva( &pvalpos3 );

//5
	float t;
	t = (float)code_getdd(0.0);

//6
	PVal *pvalres;
	APTR aptrres;
	aptrres = code_getva( &pvalres );

//////


	D3DXVECTOR3 pos0, pos1, pos2, pos3, retpos;
	pos0.x = (float)( *((double*)pvalpos0->pt + aptrpos0) );
	pos0.y = (float)( *((double*)pvalpos0->pt + aptrpos0 + 1) );
	pos0.z = (float)( *((double*)pvalpos0->pt + aptrpos0 + 2) );

	pos1.x = (float)( *((double*)pvalpos1->pt + aptrpos1) );
	pos1.y = (float)( *((double*)pvalpos1->pt + aptrpos1 + 1) );
	pos1.z = (float)( *((double*)pvalpos1->pt + aptrpos1 + 2) );

	pos2.x = (float)( *((double*)pvalpos2->pt + aptrpos2) );
	pos2.y = (float)( *((double*)pvalpos2->pt + aptrpos2 + 1) );
	pos2.z = (float)( *((double*)pvalpos2->pt + aptrpos2 + 2) );

	pos3.x = (float)( *((double*)pvalpos3->pt + aptrpos3) );
	pos3.y = (float)( *((double*)pvalpos3->pt + aptrpos3 + 1) );
	pos3.z = (float)( *((double*)pvalpos3->pt + aptrpos3 + 2) );


	int ret;
	ret = CatmullRomPoint( &pos0, &pos1, &pos2, &pos3, t, &retpos );
	if( ret ){
		DbgOut( "E3DSplineVec : CatmullRomPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz;
	dx = (double)retpos.x;
	dy = (double)retpos.y;
	dz = (double)retpos.z;
	code_setva( pvalres, aptrres, TYPE_DNUM, &dx );
	code_setva( pvalres, aptrres + 1, TYPE_DNUM, &dy );
	code_setva( pvalres, aptrres + 2, TYPE_DNUM, &dz );


	return 0;
}

int CatmullRomPoint( D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* p3, D3DXVECTOR3* p4, float u, D3DXVECTOR3* resp )
{
	float coef1, coef2, coef3, coef4;



	coef1 = -0.5f * u * u * u + u * u - 0.5f * u;
	coef2 = 1.5f * u * u * u - 2.5f * u * u + 1.0f;
	coef3 = -1.5f * u * u * u + 2.0f * u * u + 0.5f * u;
	coef4 = 0.5f * u * u * u - 0.5f * u * u;

	*resp = *p1 * coef1 + *p2 * coef2 + *p3 * coef3 + *p4 * coef4;

	return 0;
}

int E3DDbgOut()
{
	int ret;
	char tempname[4098];
	ret = GetName( tempname, 4096 );
	if( ret ){
		DbgOut( "E3DDbgOut : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcat( tempname, "\r\n" );

//////////////

	DbgOut( tempname );

	return 0;

}

int E3DGetDiffuse()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : rptr
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	//5 : gptr
	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	//6 : bptr
	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetDiffuse : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetDiffuse : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	int r, g, b;
	ret = curhs->GetDiffuse( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetDiffuse : curhs GetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );


	return 0;
}

int E3DGetAmbient()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : rptr
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	//5 : gptr
	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	//6 : bptr
	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetAmbient : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetAmbient : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	int r, g, b;
	ret = curhs->GetAmbient( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetAmbient : curhs GetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );

	return 0;
}

int E3DGetSpecular()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : rptr
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	//5 : gptr
	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	//6 : bptr
	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecular : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecular : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	int r, g, b;
	ret = curhs->GetSpecular( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecular : curhs GetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );

	return 0;
}

int E3DGetAlpha()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : aptr
	PVal *pvala;
	APTR aptra;
	aptra = code_getva( &pvala );

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetAlpha : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetAlpha : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	int a;
	ret = curhs->GetAlpha( partno, vertno, &a );
	if( ret ){
		DbgOut( "e3dhsp : GetAlpha : curhs GetAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvala, aptra, TYPE_INUM, &a );

	return 0;
}


//E3DSaveQuaFile hsid, mkid, filename 
int E3DSaveQuaFile()
{
	// 1 : hsid
	// 2 : mkid
	// 3 : file name1

// 1
	int hsid;
	hsid = code_getdi(0);

// 2
	int mkid;
	mkid = code_getdi(0);
	
// 3
	char tempname1[MAX_PATH];
	int ret;
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveQuaFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
// 4
	int quatype;
	quatype = code_getdi(QUATYPE_NAME);

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveQuaFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SaveQuaFile( quatype, mkid, tempname1 );
	if( ret ){
		DbgOut( "E3DSaveQuaFile : curhs SaveQuaFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DSaveSigFile()
{
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	int ret;
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveSigFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SaveSigFile( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveSigFile : curhs SaveSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetMipMapParams miplevels, mipfilter
//UINT g_miplevels = 0;
//DWORD g_mipfilter = D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR;
int E3DSetMipMapParams()
{

// 1
	int miplevels;
	miplevels = code_getdi(0);
	
//2
	int mipfilter;
	mipfilter = code_getdi(-1);

////


	g_miplevels = (UINT)miplevels;

	if( mipfilter != -1 ){
		g_mipfilter = (DWORD)mipfilter;
	}else{
		g_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;
	}


	return 0;
}

//E3DPickVert hsid, posx, posy, rangex, rangey, pickpartarray, pickvertarray, arrayleng, getnum
int E3DPickVert()
{
// 1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DPickVert : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
// 2
	int hsid;
	hsid = code_getdi(0);
	
//3
	int posx;
	posx = code_getdi(0);

//4
	int posy;
	posy = code_getdi(0);

//5
	int rangex;
	rangex = code_getdi(0);

//6
	int rangey;
	rangey = code_getdi(0);

//7 : partptr
	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

//8 : vertptr
	PVal *pvalvert;
	APTR aptrvert;
	aptrvert = code_getva( &pvalvert );

//9
	int arrayleng;
	arrayleng = code_getdi(0);


//10 : getnumptr
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickVert : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->PickVert( s_matView, posx, posy, rangex, rangey, (int*)pvalpart->pt + aptrpart, (int*)pvalvert->pt + aptrvert, arrayleng, &getnum );
	if( ret ){
		DbgOut( "E3DPickVert : curhs PickVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}


//E3DGetCullingFlag hsid, partno, vertno, viewcullflag, revcullflag
int E3DGetCullingFlag()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int partno;
	partno = code_getdi(0);

//3
	int vertno;
	vertno = code_getdi(0);

//4 : viewcullptr
	PVal *pvalviewcull;
	APTR aptrviewcull;
	aptrviewcull = code_getva( &pvalviewcull );

//5 : revcullptr
	PVal *pvalrevcull;
	APTR aptrrevcull;
	aptrrevcull = code_getva( &pvalrevcull );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCullingFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	int viewcull, revcull;
	ret = curhs->GetCullingFlag( partno, vertno, &viewcull, &revcull );
	if( ret ){
		DbgOut( "E3DGetCullingFlag : curhs GetCullingFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalviewcull, aptrviewcull, TYPE_INUM, &viewcull );
	code_setva( pvalrevcull, aptrrevcull, TYPE_INUM, &revcull );


	return 0;
}

//E3DGetOrgVertNo hsid, partno, vertno, orgnoarray, arrayleng, getnum
int E3DGetOrgVertNo()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int partno;
	partno = code_getdi(0);

//3
	int vertno;
	vertno = code_getdi(0);

//4 : orgnoarray
	PVal *pvalorg;
	APTR aptrorg;
	aptrorg = code_getva( &pvalorg );

//5
	int arrayleng;
	arrayleng = code_getdi(0);


//6 : getnum
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCullingFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int num = 0;
	ret = curhs->GetOrgVertNo( partno, vertno, (int*)pvalorg->pt + aptrorg, arrayleng, &num );
	if( ret ){
		DbgOut( "E3DGetOrgVertNo : curhs GetOrgVertNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &num );

	return 0;
}



int E3DChkIM2Status()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2 : statusptr
	PVal *pvalstatus;
	APTR aptrstatus;
	aptrstatus = code_getva( &pvalstatus );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkIM2Status : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int status = 0;
	ret = curhs->ChkIM2Status( &status );
	if( ret ){
		DbgOut( "E3DChkIM2Status : curhs ChkIM2Status error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalstatus, aptrstatus, TYPE_INUM, &status );

	return 0;
}


int E3DLoadIM2File()
{
	// 1 : hsid
	// 2 : im2 file name

//1
	int hsid;
	hsid = code_getdi(0);

//2
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadIM2File : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : LoadIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	ret = curhs->LoadIM2File( tempname );
	if( ret ){
		DbgOut( "e3dhsp : LoadIM2File : curhs LoadBIMFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DSaveIM2File()
{
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int ret;
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveIM2File : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SaveIM2File( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveIM2File : curhs SaveIM2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int E3DGetJointNum()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );


////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int num = 0;
	ret = curhs->GetJointNum( &num );
	if( ret ){
		DbgOut( "E3DGetJointNum : curhs GetJointNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &num );

	return 0;
}

int E3DGetNextMotionFrameNo()
{
// 1
	int hsid;
	hsid = code_getdi(0);

//2
	int motid;
	motid = code_getdi(0);

//3
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//4
	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNextMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMotionFrameNo( motid, (int*)pvalid->pt + aptrid, (int*)pvalframe->pt + aptrframe );
	if( ret ){
		DbgOut( "E3DGetNextMotionFrameNo : curhs GetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DGetMoaInfo()
{
// 1
	int hsid;
	hsid = code_getdi(0);

// 2
	int maxnum;
	maxnum = code_getdi(0);

// 3
	PVal *pvalstr;
	APTR aptrstr;
	aptrstr = code_getva( &pvalstr );

// 4
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

// 5
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

/////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DMoaInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum;
	ret = curhs->GetMoaInfo( maxnum, pvalstr, aptrstr, (int*)pvalid->pt + aptrid, &getnum );
	if( ret ){
		DbgOut( "E3DGetMoaInfo : curhs GetMoaInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}

int E3DGetJointInfo()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
	
// 2
	int maxnum;
	maxnum = code_getdi(0);

// 3
	PVal *pvalstr;
	APTR aptrstr;
	aptrstr = code_getva( &pvalstr );

// 4
	PVal *pvalseri;
	APTR aptrseri;
	aptrseri = code_getva( &pvalseri );

// 5
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
////


	//strcpy( strptr, "��������������������0123" );
	//strcpy( strptr + 256, "����������" );
	//strcpy( strptr + 512, "abcdefghijk�����Ă�4567_" );
	//*getnumptr = 3;


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int getnum;
	ret = curhs->GetJointInfo( maxnum, pvalstr, aptrstr, (int*)pvalseri->pt + aptrseri, &getnum );
	if( ret ){
		DbgOut( "E3DGetJointInfo : curhs GetJointInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}


int E3DGetFirstJointNo()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFirstJointNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int no = 0;
	ret = curhs->GetFirstJointNo( &no );
	if( ret ){
		DbgOut( "E3DGetFirstJointNo : curhs GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &no );

	return 0;
}


//GetDispObjNum
int E3DGetDispObjNum()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int num = 0;
	ret = curhs->GetDispObjNum( &num );
	if( ret ){
		DbgOut( "E3DGetDispObjNum : curhs GetDispObjNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &num );

	return 0;
}


//GetDispObjInfo
int E3DGetDispObjInfo()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
	
// 2
	int maxnum;
	maxnum = code_getdi(0);

// 3
	PVal *pvalstr;
	APTR aptrstr;
	aptrstr = code_getva( &pvalstr );

// 4
	PVal *pvalseri;
	APTR aptrseri;
	aptrseri = code_getva( &pvalseri );

// 5
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->GetDispObjInfo( maxnum, pvalstr, aptrstr, (int*)pvalseri->pt + aptrseri, &getnum );
	if( ret ){
		DbgOut( "E3DGetDispObjInfo : curhs GetDispObjInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}

//E3DEnableTexture hsid, partno, enableflag
int E3DEnableTexture()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	int partno;
	partno = code_getdi(0);

// 3
	int enableflag;
	enableflag = code_getdi(0);

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIM2Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->EnableTexture( partno, enableflag );
	if( ret ){
		DbgOut( "E3DEnableTexture : curhs EnableTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DJointAddToTree hsid, parentjoint, childjoint, lastflag
int E3DJointAddToTree()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	int parentjoint;
	parentjoint = code_getdi(0);

// 3
	int childjoint;
	childjoint = code_getdi(0);

// 4
	int lastflag;
	lastflag = code_getdi(0);

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointAddToTree : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->JointAddToTree( parentjoint, childjoint, lastflag );
	if( ret ){
		DbgOut( "E3DJointAddToTree : hs JointAddToTree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DJointRemoveFromParent hsid, rmjoint
//	�@�@�q���W���C���g�͂��̂܂�
int E3DJointRemoveFromParent()
{


// 1
	int hsid;
	hsid = code_getdi(0);
	
// 2
	int rmjoint;
	rmjoint = code_getdi(0);

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointRemoveFromParent : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->JointRemoveFromParent( rmjoint );
	if( ret ){
		DbgOut( "E3DjointRemoveFromParent : hs JointRemoveFromParent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

//E3DJointRemake hsid
int E3DJointRemake()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointRemake : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->JointRemake();
	if( ret ){
		DbgOut( "E3DJointRemake : hs JointRemake error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


int E3DSigImport()
{

//1
	int hsid;
	hsid = code_getdi(0);

//2
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigImport : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//3
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//4
	float mult;
	mult = (float)code_getdd(1.0);



	float offsetx;
	offsetx = (float)code_getdd(0.0);
	float offsety;
	offsety = (float)code_getdd(0.0);
	float offsetz;
	offsetz = (float)code_getdd(0.0);


	float rotx;
	rotx = (float)code_getdd(0.0);
	float roty;
	roty = (float)code_getdd(0.0);
	float rotz;
	rotz = (float)code_getdd(0.0);


//////


	D3DXVECTOR3 offset;
	offset.x = offsetx;
	offset.y = offsety;
	offset.z = offsetz;

	D3DXVECTOR3 rot;
	rot.x = rotx;
	rot.y = roty;
	rot.z = rotz;




	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImport : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( tempname, curhs );//!!!!!!

	ret = curhs->ImportSigFile( tempname, adjustuvflag, mult, offset, rot );
	if( ret ){
		DbgOut( "E3DSigImport : hs ImportSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DSigImport : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////
	/***
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DSigImport : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
	return 0;
}


//E3DSigLoadFromBuf resdir, buf, bufsize, hsidptr, adjustuvflag
	//resdir�͍Ō��\\��Y�ꂸ�ɁB
int E3DSigLoadFromBuf()
{

//1
	char tempname[MAX_PATH];
	int ret;
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : resdir name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

//5
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//6
	float mult;
	mult = (float)code_getdd(1.0);
//////


	int temphsid = -1;
	ret = SigLoadFromBuf_F( tempname, (char*)pvalbuf->pt + aptrbuf, bufsize, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );

		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );


	return 0;
}

int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{


	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	ret = newhs->LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : E3DSigLoadFromBuf : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;
	
	return 0;
}



//E3DSigImportFromBuf hsid, resdir, buf, bufsize, adjustuvflag
int E3DSigImportFromBuf()
{

//1
	int hsid;
	hsid = code_getdi(0);

//2
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : resdir name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//3
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//4
	int bufsize;
	bufsize = code_getdi(0);


//5
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//6
	float mult;
	mult = (float)code_getdd(1.0);


	float offsetx;
	offsetx = (float)code_getdd(0.0);
	float offsety;
	offsety = (float)code_getdd(0.0);
	float offsetz;
	offsetz = (float)code_getdd(0.0);


	float rotx;
	rotx = (float)code_getdd(0.0);
	float roty;
	roty = (float)code_getdd(0.0);
	float rotz;
	rotz = (float)code_getdd(0.0);

//////

	D3DXVECTOR3 offset;
	offset.x = offsetx;
	offset.y = offsety;
	offset.z = offsetz;

	D3DXVECTOR3 rot;
	rot.x = rotx;
	rot.y = roty;
	rot.z = rotz;


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImportFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( tempname, curhs );//!!!!!!


//DWORD dwtime1, dwtime2, dwtime3;
//g_rtsc.Start();

	ret = curhs->ImportSigFileFromBuf( (char*)pvalbuf->pt + aptrbuf, bufsize, adjustuvflag, mult, offset, rot );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : hs ImportSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//dwtime1 = g_rtsc.Stop();

//g_rtsc.Start();

//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//dwtime2 = g_rtsc.Stop();

//g_rtsc.Start();

/////
	/***
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
//dwtime3 = g_rtsc.Stop();

//DbgOut( "e3dimportsig from buf : time1 %d, time2 %d, time3 %d\r\n", dwtime1, dwtime2, dwtime3 );
	
	return 0;
}


//E3DAddMotionFromBuf hsid, datakind, bufptr, bufsize, motcookie, maxnum
int E3DAddMotionFromBuf()
{

//1
	int hsid;
	hsid = code_getdi(0);

//2
	int datakind;
	datakind = code_getdi(0);

//3
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//4
	int bufsize;
	bufsize = code_getdi(0);

//5	
	PVal *pvalcookie;
	APTR aptrcookie;
	aptrcookie = code_getva( &pvalcookie );

//6
	PVal *pvalmaxnum;
	APTR aptrmaxnum;
	aptrmaxnum = code_getva( &pvalmaxnum );

//7
	float mvmult;
	mvmult = (float)code_getdd(1.0);

////////
	
	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotionFromBuf_F( hsid, datakind, (char*)pvalbuf->pt + aptrbuf, bufsize, &tempmotid, &tempnum, mvmult );
	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : AddMotionFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	code_setva( pvalcookie, aptrcookie, TYPE_INUM, &tempmotid );
	code_setva( pvalmaxnum, aptrmaxnum, TYPE_INUM, &tempnum );

	return 0;
}

int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr, float mvmult )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotionFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
//	if( datakind == 0 ){
		ret = curhs->LoadQuaFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr, mvmult );	
//	}else{
//		ret = curhs->LoadMotFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr );
//	}

	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCheckFullScreenParams w, h, bits, validflag, validw, validh, validbits
int E3DCheckFullScreenParams()
{

//1
	int w;
	w = code_getdi(640);

//2
	int h;
	h = code_getdi(480);

//3
	int bits;
	bits = code_getdi(16);

//4
	PVal *pvalvalid;
	APTR aptrvalid;
	aptrvalid = code_getva( &pvalvalid );
		
//5
	PVal *pvalvalidw;
	APTR aptrvalidw;
	aptrvalidw = code_getva( &pvalvalidw );

//6
	PVal *pvalvalidh;
	APTR aptrvalidh;
	aptrvalidh = code_getva( &pvalvalidh );


//7
	PVal *pvalvalidbits;
	APTR aptrvalidbits;
	aptrvalidbits = code_getva( &pvalvalidbits );

////////

	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}


	CD3DApplication* tmpapp;

	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int validflag, validw, validh, validbits;
	ret = tmpapp->CheckFullScreenParams( (DWORD)w, (DWORD)h, bits, &validflag, &validw, &validh, &validbits );
	if( ret ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp CheckFullScreenParams error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}

	code_setva( pvalvalid, aptrvalid, TYPE_INUM, &validflag );
	code_setva( pvalvalidw, aptrvalidw, TYPE_INUM, &validw );
	code_setva( pvalvalidh, aptrvalidh, TYPE_INUM, &validh );
	code_setva( pvalvalidbits, aptrvalidbits, TYPE_INUM, &validbits );


	delete tmpapp;

	return 0;
}

//E3DGetMaxMultiSampleNum bits, iswindowmode, multisamplenumptr
int E3DGetMaxMultiSampleNum()
{

//1
	int bits;
	bits = code_getdi(16);

//2
	int iswindowmode;
	iswindowmode = code_getdi(0);

//3
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////////

	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}

	if( iswindowmode != 0 )
		iswindowmode = 1;

//DbgOut( "check!!!, multisample 0\r\n" );
	CD3DApplication* tmpapp;
	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DGetMaxMultiSampleNum : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//DbgOut( "check!!!, multisample 1\r\n" );
	int ret;
	int num = 1;
	ret = tmpapp->GetMaxMultiSampleNum( bits, iswindowmode, &num );
	if( ret ){
		DbgOut( "E3DGetMaxMultiSampleNum : tmpapp GetMaxMultiSampleNum error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}
//DbgOut( "check!!!, multisample 2\r\n" );
	code_setva( pvalnum, aptrnum, TYPE_INUM, &num );

//DbgOut( "check!!!, multisample 3\r\n" );

	delete tmpapp;

//DbgOut( "check!!!, multisample 4\r\n" );
	return 0;
}

int E3DChkConfLineAndFacePart()// $21C
{
// 1
	float posx1;
	posx1 = (float)code_getdd(0.0);
// 2
	float posy1;
	posy1 = (float)code_getdd(0.0);
// 3
	float posz1;
	posz1 = (float)code_getdd(0.0);


// 4
	float posx2;
	posx2 = (float)code_getdd(0.0);
// 5
	float posy2;
	posy2 = (float)code_getdd(0.0);
// 6
	float posz2;
	posz2 = (float)code_getdd(0.0);


// 7
	int hsid;
	hsid = code_getdi(0);

//8
	int chkpartno;
	chkpartno = code_getdi(0);

// 9
	int needtrans;
	needtrans = code_getdi(0);

// 10
	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

// 11
	PVal *pvalface;
	APTR aptrface;
	aptrface = code_getva( &pvalface );


// 12
	PVal *pvalconfx;
	APTR aptrconfx;
	aptrconfx = code_getva( &pvalconfx );

// 13
	PVal *pvalconfy;
	APTR aptrconfy;
	aptrconfy = code_getva( &pvalconfy );

// 14
	PVal *pvalconfz;
	APTR aptrconfz;
	aptrconfz = code_getva( &pvalconfz );



// 15
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );
// 16
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );
// 17
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );


// 18
	PVal *pvalrevface;
	APTR aptrrevface;
	aptrrevface = code_getva( &pvalrevface );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfLineAndFacePart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 srcpos1, srcpos2;
	D3DXVECTOR3 dstpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dstn( 0.0f, 0.0f, 0.0f );

	srcpos1.x = (float)posx1;
	srcpos1.y = (float)posy1;
	srcpos1.z = (float)posz1;

	srcpos2.x = (float)posx2;
	srcpos2.y = (float)posy2;
	srcpos2.z = (float)posz2;

	D3DXVECTOR3 camdir;
	camdir = s_cameratarget - s_camerapos;

	int ret;
	int part, face, revface;
	ret = curhs->ChkConfLineAndFacePart( chkpartno, camdir, s_matView, srcpos1, srcpos2, needtrans, &part, &face, &dstpos, &dstn, &revface );
	if( ret ){
		DbgOut( "E3DChkConfLineAndFacePart : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalpart, aptrpart, TYPE_INUM, &part );
	code_setva( pvalface, aptrface, TYPE_INUM, &face );
	code_setva( pvalrevface, aptrrevface, TYPE_INUM, &revface );

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;
	code_setva( pvalconfx, aptrconfx, TYPE_DNUM, &dx );
	code_setva( pvalconfy, aptrconfy, TYPE_DNUM, &dy );
	code_setva( pvalconfz, aptrconfz, TYPE_DNUM, &dz );


	if( part >= 0 ){
		D3DXVec3Normalize( &dstn, &dstn );
	}

	double dnx, dny, dnz;
	dnx = (double)dstn.x;
	dny = (double)dstn.y;
	dnz = (double)dstn.z;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );


	return 0;
}

//E3DChkConfLineAndFace posx1, posy1, posz1, posx2, posy2, posz2, hsid, needtrans, 
//	partno, faceno, confx, confy, confz, nx10000, ny10000, nz10000, revfaceflag
int E3DChkConfLineAndFace()
{
// 1
	float posx1;
	posx1 = (float)code_getdd(0.0);
// 2
	float posy1;
	posy1 = (float)code_getdd(0.0);
// 3
	float posz1;
	posz1 = (float)code_getdd(0.0);


// 4
	float posx2;
	posx2 = (float)code_getdd(0.0);
// 5
	float posy2;
	posy2 = (float)code_getdd(0.0);
// 6
	float posz2;
	posz2 = (float)code_getdd(0.0);


// 7
	int hsid;
	hsid = code_getdi(0);
// 8
	int needtrans;
	needtrans = code_getdi(0);

// 9
	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

// 10
	PVal *pvalface;
	APTR aptrface;
	aptrface = code_getva( &pvalface );


// 11
	PVal *pvalconfx;
	APTR aptrconfx;
	aptrconfx = code_getva( &pvalconfx );

// 12
	PVal *pvalconfy;
	APTR aptrconfy;
	aptrconfy = code_getva( &pvalconfy );

// 13
	PVal *pvalconfz;
	APTR aptrconfz;
	aptrconfz = code_getva( &pvalconfz );



// 14
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );
// 15
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );
// 16
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );


// 17
	PVal *pvalrevface;
	APTR aptrrevface;
	aptrrevface = code_getva( &pvalrevface );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfLineAndFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 srcpos1, srcpos2;
	D3DXVECTOR3 dstpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dstn( 0.0f, 0.0f, 0.0f );

	srcpos1.x = (float)posx1;
	srcpos1.y = (float)posy1;
	srcpos1.z = (float)posz1;

	srcpos2.x = (float)posx2;
	srcpos2.y = (float)posy2;
	srcpos2.z = (float)posz2;

	D3DXVECTOR3 camdir;
	camdir = s_cameratarget - s_camerapos;

	int ret;
	int part, face, revface;
	ret = curhs->ChkConfLineAndFace( camdir, s_matView, srcpos1, srcpos2, needtrans, &part, &face, &dstpos, &dstn, &revface );
	if( ret ){
		DbgOut( "E3DChkConfLineAndFace : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalpart, aptrpart, TYPE_INUM, &part );
	code_setva( pvalface, aptrface, TYPE_INUM, &face );
	code_setva( pvalrevface, aptrrevface, TYPE_INUM, &revface );

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;
	code_setva( pvalconfx, aptrconfx, TYPE_DNUM, &dx );
	code_setva( pvalconfy, aptrconfy, TYPE_DNUM, &dy );
	code_setva( pvalconfz, aptrconfz, TYPE_DNUM, &dz );


	if( part >= 0 ){
		D3DXVec3Normalize( &dstn, &dstn );
	}

	double dnx, dny, dnz;
	dnx = (double)dstn.x;
	dny = (double)dstn.y;
	dnz = (double)dstn.z;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	return 0;
}

//E3DPickFace hsid, pos2x, pos2y, maxdist, partnoptr, facenoptr, pos3xptr, pos3yptr, pos3zptr, nxptr, nyptr, nzptr, distptr, calcmode
int E3DPickFace()
{
// 1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DPickFace : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// 2
	int hsid;
	hsid = code_getdi(0);
	
//3
	int pos2x;
	pos2x = code_getdi(0);

//4
	int pos2y;
	pos2y = code_getdi(0);

//5
	float maxdist;
	maxdist = (float)code_getdd(10000.0);

// 6
	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

// 7
	PVal *pvalface;
	APTR aptrface;
	aptrface = code_getva( &pvalface );


// 8
	PVal *pvalconfx;
	APTR aptrconfx;
	aptrconfx = code_getva( &pvalconfx );

// 9
	PVal *pvalconfy;
	APTR aptrconfy;
	aptrconfy = code_getva( &pvalconfy );

// 10
	PVal *pvalconfz;
	APTR aptrconfz;
	aptrconfz = code_getva( &pvalconfz );



// 11
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );
// 12
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );
// 13
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );


//14 : distptr
	PVal *pvaldist;
	APTR aptrdist;
	aptrdist = code_getva( &pvaldist );

//15
	int calcmode;
	calcmode = code_getdi(1);


////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 pos3;
	D3DXVECTOR3 nvec;


	if( maxdist < 0 ){
		maxdist = 10000.0f;
	}

	float fdist;

	if( calcmode == 1 ){
		CalcMatView();//!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickFace : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int part, face;
	ret = curhs->PickFace( s_matView, maxdist, pos2x, pos2y, &part, &face, &pos3, &nvec, &fdist, calcmode );
	if( ret ){
		DbgOut( "E3DPickFace : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	


	code_setva( pvalpart, aptrpart, TYPE_INUM, &part );
	code_setva( pvalface, aptrface, TYPE_INUM, &face );

	double dx, dy, dz;
	dx = (double)pos3.x;
	dy = (double)pos3.y;
	dz = (double)pos3.z;
	code_setva( pvalconfx, aptrconfx, TYPE_DNUM, &dx );
	code_setva( pvalconfy, aptrconfy, TYPE_DNUM, &dy );
	code_setva( pvalconfz, aptrconfz, TYPE_DNUM, &dz );


	if( part >= 0 ){
		D3DXVec3Normalize( &nvec, &nvec );
	}

	double dnx, dny, dnz;
	dnx = (double)nvec.x;
	dny = (double)nvec.y;
	dnz = (double)nvec.z;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	double ddist;
	if( part < 0 ){
		ddist = 900000000.0;
	}else{
		ddist = (double)fdist;
	}
	code_setva( pvaldist, aptrdist, TYPE_DNUM, &ddist );


	return 0;
}

int E3DPickFace2()
{
// 1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DPickFace2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// 2
	int hsid;
	hsid = code_getdi(0);
	
// 3
	PVal *pvalpartarray;
	APTR aptrpartarray;
	aptrpartarray = code_getva( &pvalpartarray );

// 4
	int partnum;
	partnum = code_getdi(0);

//5
	int pos2x;
	pos2x = code_getdi(0);

//6
	int pos2y;
	pos2y = code_getdi(0);

//7
	float maxdist;
	maxdist = (float)code_getdd(10000.0);

// 8
	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

// 9
	PVal *pvalface;
	APTR aptrface;
	aptrface = code_getva( &pvalface );


// 10
	PVal *pvalconfx;
	APTR aptrconfx;
	aptrconfx = code_getva( &pvalconfx );

// 11
	PVal *pvalconfy;
	APTR aptrconfy;
	aptrconfy = code_getva( &pvalconfy );

// 12
	PVal *pvalconfz;
	APTR aptrconfz;
	aptrconfz = code_getva( &pvalconfz );



// 11
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );
// 12
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );
// 13
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );


//14 : distptr
	PVal *pvaldist;
	APTR aptrdist;
	aptrdist = code_getva( &pvaldist );

//15
	int calcmode;
	calcmode = code_getdi(1);


////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickFace2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 pos3;
	D3DXVECTOR3 nvec;


	if( maxdist < 0 ){
		maxdist = 10000.0f;
	}

	float fdist;

	if( calcmode == 1 ){
		CalcMatView();//!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickFace2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int part, face;
	ret = curhs->PickFace2( s_matView, (int*)(pvalpartarray->pt + aptrpartarray), partnum, maxdist, pos2x, pos2y, &part, &face, &pos3, &nvec, &fdist, calcmode );
	if( ret ){
		DbgOut( "E3DPickFace2 : curhs PickFace2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	


	code_setva( pvalpart, aptrpart, TYPE_INUM, &part );
	code_setva( pvalface, aptrface, TYPE_INUM, &face );

	double dx, dy, dz;
	dx = (double)pos3.x;
	dy = (double)pos3.y;
	dz = (double)pos3.z;
	code_setva( pvalconfx, aptrconfx, TYPE_DNUM, &dx );
	code_setva( pvalconfy, aptrconfy, TYPE_DNUM, &dy );
	code_setva( pvalconfz, aptrconfz, TYPE_DNUM, &dz );


	if( part >= 0 ){
		D3DXVec3Normalize( &nvec, &nvec );
	}

	double dnx, dny, dnz;
	dnx = (double)nvec.x;
	dny = (double)nvec.y;
	dnz = (double)nvec.z;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	double ddist;
	if( part < 0 ){
		ddist = 900000000.0;
	}else{
		ddist = (double)fdist;
	}
	code_setva( pvaldist, aptrdist, TYPE_DNUM, &ddist );


	return 0;
}

//E3DGetBBox hsid, partno, mode, minx, maxx, miny, maxy, minz, maxz
int E3DGetBBox()
{
// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int partno;
	partno = code_getdi(0);

//3
	int mode;
	mode = code_getdi(0);

//4 : minxptra
	PVal *pvalminx;
	APTR aptrminx;
	aptrminx = code_getva( &pvalminx );

//5 : maxxptr
	PVal *pvalmaxx;
	APTR aptrmaxx;
	aptrmaxx = code_getva( &pvalmaxx );

//6 : minyptr
	PVal *pvalminy;
	APTR aptrminy;
	aptrminy = code_getva( &pvalminy );

//7 : maxyptr
	PVal *pvalmaxy;
	APTR aptrmaxy;
	aptrmaxy = code_getva( &pvalmaxy );

//8 : minxptr
	PVal *pvalminz;
	APTR aptrminz;
	aptrminz = code_getva( &pvalminz );

//9 : maxxptr
	PVal *pvalmaxz;
	APTR aptrmaxz;
	aptrmaxz = code_getva( &pvalmaxz );

////
	CHandlerSet* curhs = 0;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DGetBBox : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "E3DGetBBox : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	float fminx, fmaxx, fminy, fmaxy, fminz, fmaxz;
	int ret;

	ret = curhs->GetBBox( partno, mode, &fminx, &fmaxx, &fminy, &fmaxy, &fminz, &fmaxz );
	if( ret ){
		DbgOut( "E3DGetBBox : curhs GetBBox error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dminx, dmaxx, dminy, dmaxy, dminz, dmaxz;
	dminx = (double)fminx;
	dmaxx = (double)fmaxx;
	dminy = (double)fminy;
	dmaxy = (double)fmaxy;
	dminz = (double)fminz;
	dmaxz = (double)fmaxz;

	code_setva( pvalminx, aptrminx, TYPE_DNUM, &dminx );
	code_setva( pvalmaxx, aptrmaxx, TYPE_DNUM, &dmaxx );

	code_setva( pvalminy, aptrminy, TYPE_DNUM, &dminy );
	code_setva( pvalmaxy, aptrmaxy, TYPE_DNUM, &dmaxy );

	code_setva( pvalminz, aptrminz, TYPE_DNUM, &dminz );
	code_setva( pvalmaxz, aptrmaxz, TYPE_DNUM, &dmaxz );

	return 0;
}

//E3DGetVertNoOfFace hsid, partno, faceno, vert1, vert2, vert3
int E3DGetVertNoOfFace()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int partno;
	partno = code_getdi(0);

//3
	int faceno;
	faceno = code_getdi(0);

//4 : vert1ptr
	PVal *pvalv1;
	APTR aptrv1;
	aptrv1 = code_getva( &pvalv1 );

//5 : vert2ptr
	PVal *pvalv2;
	APTR aptrv2;
	aptrv2 = code_getva( &pvalv2 );


//6 : vert3ptr
	PVal *pvalv3;
	APTR aptrv3;
	aptrv3 = code_getva( &pvalv3 );

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNoOfFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int v1, v2, v3;
	ret = curhs->GetVertNoOfFace( partno, faceno, &v1, &v2, &v3 );
	if( ret ){
		DbgOut( "E3DGetVertNoOfFace : hs GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalv1, aptrv1, TYPE_INUM, &v1 );
	code_setva( pvalv2, aptrv2, TYPE_INUM, &v2 );
	code_setva( pvalv3, aptrv3, TYPE_INUM, &v3 );

	return 0;
}

//E3DGetSamePosVert hsid, partno, vertno, sameptr, arrayleng, samenum
int E3DGetSamePosVert()
{

// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	int partno;
	partno = code_getdi(0);

//3
	int vertno;
	vertno = code_getdi(0);

//4 : sameptr
	PVal *pvalsame;
	APTR aptrsame;
	aptrsame = code_getva( &pvalsame );

//5
	int arrayleng;
	arrayleng = code_getdi(0);


//6 : samenumptr
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );


////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetSamePosVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int samenum = 0;
	ret = curhs->GetSamePosVert( partno, vertno, (int*)pvalsame->pt + aptrsame, arrayleng, &samenum );
	if( ret ){
		DbgOut( "E3DGetSamePosVert : hs GetSamePosVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &samenum );

	return 0;
}

//E3DRdtscStart
int E3DRdtscStart()
{

	g_rtsc.Start();

	return 0;
}



//E3DRdtscStop time
int E3DRdtscStop()
{

//1 : timeptr
	PVal *pvaltime;
	APTR aptrtime;
	aptrtime = code_getva( &pvaltime );

////


	DWORD dwret;
	dwret = g_rtsc.Stop();

	double dtime;
	dtime = (double)dwret;
	code_setva( pvaltime, aptrtime, TYPE_DNUM, &dtime );

	return 0;
}

//E3DSaveSig2Buf hsid, buf, bufsize, writesize 
//bufsize�ɂO���w�肵���ꍇ�ɂ́Awritesize�ɕK�v�ȃo�b�t�@����������̂�
int E3DSaveSig2Buf()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : buf
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3 : bufsize
	int bufsize;
	bufsize = code_getdi(0);

//4 : writesize
	PVal *pvalsize;
	APTR aptrsize;
	aptrsize = code_getva( &pvalsize );

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int writesize = 0;
	ret = curhs->SaveSig2Buf( (char*)pvalbuf->pt + aptrbuf, bufsize, &writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalsize, aptrsize, TYPE_INUM, &writesize );


	return 0;
}


//E3DSaveQua2Buf hsid, mkid, buf, bufsize, writesize
//bufsize�ɂO���w�肵���ꍇ�ɂ́Awritesize�ɕK�v�ȃo�b�t�@����������̂�
int E3DSaveQua2Buf()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 mkid
	int mkid;
	mkid = code_getdi(0);

//3 : buf
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//4 : bufsize
	int bufsize;
	bufsize = code_getdi(0);

//5 : writesize
	PVal *pvalsize;
	APTR aptrsize;
	aptrsize = code_getva( &pvalsize );

//6
	int quatype;
	quatype = code_getdi(QUATYPE_NAME);

////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int writesize = 0;
	ret = curhs->SaveQua2Buf( quatype, mkid, (char*)pvalbuf->pt + aptrbuf, bufsize, &writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalsize, aptrsize, TYPE_INUM, &writesize );

	return 0;
}


//E3DCameraShiftLeft	shift
int E3DCameraShiftLeft()
{
//1
	float fshift;
	fshift = (float)code_getdd(0.0);

////

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	CQuaternion2 rotq;
	D3DXVECTOR3 rotaxis( 0.0f, 1.0f, 0.0f );
	float rotrad = 90.0f * (float)DEG2PAI;
	rotq.SetAxisAndRot( rotaxis, rotrad );

	CQuaternion2 newq;
	newq = invview * rotq;
	newq.normalize();

	D3DXVECTOR3 zdir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 shiftdir;
	

	newq.Rotate( &shiftdir, zdir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DCameraShiftRight shift
int E3DCameraShiftRight()
{

//1 
	float fshift;
	fshift = (float)code_getdd(0.0);


	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	CQuaternion2 rotq;
	D3DXVECTOR3 rotaxis( 0.0f, 1.0f, 0.0f );
	float rotrad = -90.0f * (float)DEG2PAI;
	rotq.SetAxisAndRot( rotaxis, rotrad );

	CQuaternion2 newq;
	newq = invview * rotq;
	newq.normalize();

	D3DXVECTOR3 zdir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 shiftdir;
	

	newq.Rotate( &shiftdir, zdir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}
//E3DCameraShiftUp shift
int E3DCameraShiftUp()
{

	float fshift;
	fshift = (float)code_getdd(0.0);


	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DCameraShiftDown shift
int E3DCameraShiftDown()
{

	float fshift;
	fshift = (float)code_getdd(0.0);


	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, -1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DGetCameraQ dstqid
int E3DGetCameraQ()
{
//1 
	int dstqid;
	dstqid = code_getdi(0);


////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DGetCameraQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( dstqid, &qptr );
	if( ret ){
		DbgOut( "E3DGetCameraQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXMATRIX vm;
	vm = s_matView;
	vm._41 = 0.0f;
	vm._42 = 0.0f;
	vm._43 = 0.0f;

	D3DXQUATERNION vmq;
	D3DXQuaternionRotationMatrix( &vmq, &vm );

	qptr->x = vmq.x;
	qptr->y = vmq.y;
	qptr->z = vmq.z;
	qptr->w = vmq.w;

	return 0;
}


//E3DInvQ srcqid, dstqid
int E3DInvQ()
{

//1
	int srcqid;
	srcqid = code_getdi(0);
//2
	int dstqid;
	dstqid = code_getdi(0);

////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DInvQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( srcqid, &srcqptr );
	if( ret ){
		DbgOut( "E3DInvQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "E3DInvQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	srcqptr->inv( dstqptr );

	return 0;
}

int E3DSetCameraTwist()
{

//1
	float twistdeg;
	twistdeg = (float)code_getdd(0.0);

////

	s_camera_twist = twistdeg;

	return 0;
}

//E3DIKRotateBeta hsid, jointno, motid, frameno, iklevel, axiskind, axisx, axisy, axisz, calclevel, targetx, targety, targetz, resx, resy, resz, lastparent
//	iklevel : �K�w��
//	axiskind : 0-->�����ɕ��s�Ȏ�
//	�@�@�@�@�@�@1-->�w�肵����
//	�@�@�@�@�@�@2-->Auto�I��
//	axisx, y, z : axiskind�ɂP���w�肵���Ƃ��̂ݗL��
//	calclevel : �v�Z�ׂ̍����A�����傫���قǍׂ����B
//	targetx, y, z : �{�[���̖ڕW�n�_�B
//	resx, y, z : �v�Z��̃W���C���g�̈ʒu�B		
//	lastparent : �v�Z�ŕύX�̂�������ԏ�̊K�w�̃W���C���g�̔ԍ��BE3DRender�ɓn���ƍ������o����B
int E3DIKRotateBeta()
{
//1 scid
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DIKRotateBeta : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 hsid
	int hsid;
	hsid = code_getdi(0);

//3 jointno
	int jointno;
	jointno = code_getdi(0);

//4 motid
	int motid;
	motid = code_getdi(0);

//5 frameno
	int frameno;
	frameno = code_getdi(0);

//6 iklevel
	int iklevel;
	iklevel = code_getdi(0);

//7 axiskind
	int axiskind;
	axiskind = code_getdi(0);

//8 axisx
	float axisx;
	axisx = (float)code_getdd(0.0);
//9 axisy
	float axisy;
	axisy = (float)code_getdd(0.0);
//10 axisz
	float axisz;
	axisz = (float)code_getdd(0.0);

//11 calclevel
	int calclevel;
	calclevel = code_getdi(0);

//12 targetx
	float targetx;
	targetx = (float)code_getdd(0.0);
//13 targety
	float targety;
	targety = (float)code_getdd(0.0);
//14 targetz
	float targetz;
	targetz = (float)code_getdd(0.0);

//15 : resx
	PVal *pvalresx;
	APTR aptrresx;
	aptrresx = code_getva( &pvalresx );
//16 : resy
	PVal *pvalresy;
	APTR aptrresy;
	aptrresy = code_getva( &pvalresy );
//17 : resz
	PVal *pvalresz;
	APTR aptrresz;
	aptrresz = code_getva( &pvalresz );

//18 : lastparent
	PVal *pvallastparent;
	APTR aptrlastparent;
	aptrlastparent = code_getva( &pvallastparent );

////
	int ret;

	
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXVECTOR3 axisvec;
	switch( axiskind ){
	case 0:
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 1:
		axisvec.x = axisx;
		axisvec.y = axisy;
		axisvec.z = axisz;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 2:
		axisvec.x = 0.0f;//�_�~�[�f�[�^
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	default:
		axiskind = 2;//!!!!!!
		axisvec.x = 0.0f;//�_�~�[�f�[�^
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	}
	
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DIKRotateBeta : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 targetpos;
	targetpos.x = targetx;
	targetpos.y = targety;
	targetpos.z = targetz;

	D3DXVECTOR3 respos(0.0f, 0.0f, 0.0f);

//DbgOut( "E3DIKRotateBeta : axisvec %f %f %f\r\n", axisvec.x, axisvec.y, axisvec.z );
	int lastparent = 0;
	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, jointno, motid, frameno, iklevel, axiskind, axisvec, calclevel, targetpos, &respos, &lastparent );
	if( ret ){
		DbgOut( "E3DIKRotateBeta : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvallastparent, aptrlastparent, TYPE_INUM, &lastparent );

	double dx, dy, dz;
	dx = (double)respos.x;
	dy = (double)respos.y;
	dz = (double)respos.z;
	code_setva( pvalresx, aptrresx, TYPE_DNUM, &dx );
	code_setva( pvalresy, aptrresy, TYPE_DNUM, &dy );
	code_setva( pvalresz, aptrresz, TYPE_DNUM, &dz );

	return 0;
}

//E3DIKRotateBeta2D hsid, jointno, motid, frameno, iklevel, axiskind, axisx, axisy, axisz, calclevel, 
//		target2dx, target2dy, resx, resy, resz, lastparent
int E3DIKRotateBeta2D()
{
//1 scid
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DIKRotateBeta2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 hsid
	int hsid;
	hsid = code_getdi(0);

//3 jointno
	int jointno;
	jointno = code_getdi(0);

//4 motid
	int motid;
	motid = code_getdi(0);

//5 frameno
	int frameno;
	frameno = code_getdi(0);

//6 iklevel
	int iklevel;
	iklevel = code_getdi(0);

//7 axiskind
	int axiskind;
	axiskind = code_getdi(0);

//8 axisx
	float axisx;
	axisx = (float)code_getdd(0.0);
//9 axisy
	float axisy;
	axisy = (float)code_getdd(0.0);
//10 axisz
	float axisz;
	axisz = (float)code_getdd(0.0);

//11 calclevel
	int calclevel;
	calclevel = code_getdi(0);

//12 target2dx
	int target2dx;
	target2dx = code_getdi(0);
//13 target2dy
	int target2dy;
	target2dy = code_getdi(0);

//14 : resx
	PVal *pvalresx;
	APTR aptrresx;
	aptrresx = code_getva( &pvalresx );
//15 : resy
	PVal *pvalresy;
	APTR aptrresy;
	aptrresy = code_getva( &pvalresy );
//16 : resz
	PVal *pvalresz;
	APTR aptrresz;
	aptrresz = code_getva( &pvalresz );

//17 : lastparent
	PVal *pvallastparent;
	APTR aptrlastparent;
	aptrlastparent = code_getva( &pvallastparent );

////
	int ret;

	
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta2D : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXVECTOR3 axisvec;
	switch( axiskind ){
	case 0:
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 1:
		axisvec.x = axisx;
		axisvec.y = axisy;
		axisvec.z = axisz;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 2:
		axisvec.x = 0.0f;//�_�~�[�f�[�^
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	default:
		axiskind = 2;//!!!!!!
		axisvec.x = 0.0f;//�_�~�[�f�[�^
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	}
	
	D3DXVECTOR2 targetpos2d;
	targetpos2d.x = (float)target2dx;
	targetpos2d.y = (float)target2dy;

	D3DXVECTOR3 respos(0.0f, 0.0f, 0.0f);

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DIKRotateBeta2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


/////////////////

	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( jointno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = targetpos2d.x;
	targetsc.y = targetpos2d.y;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = targetpos2d.x;
	targetsc.y = targetpos2d.y;
	targetsc.z = 1.0f;
	ret = ScreenToObj( &targetobj1, targetsc );
	if( ret )
		return 0;

	D3DXVECTOR3 targetdir;
	targetdir = targetobj1 - targetobj0;
	DXVec3Normalize( &targetdir, &targetdir );


	D3DXVECTOR3 planeaxis;
	planeaxis = s_cameratarget - s_camerapos;	
	DXVec3Normalize( &planeaxis, &planeaxis );


	D3DXVECTOR3 temptarget;
	ret = GetShadow( &temptarget, planeaxis, mvobj, targetobj0, targetdir );
	if( ret )
		return 0;

	//targetobj = mvobj + ( temptarget - mvobj ) / (float)calclevel;


/////////////////



	int lastparent = 0;
	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, jointno, motid, frameno, iklevel, axiskind, axisvec, calclevel, temptarget, &respos, &lastparent );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvallastparent, aptrlastparent, TYPE_INUM, &lastparent );

	double dx, dy, dz;
	dx = (double)respos.x;
	dy = (double)respos.y;
	dz = (double)respos.z;
	code_setva( pvalresx, aptrresx, TYPE_DNUM, &dx );
	code_setva( pvalresy, aptrresy, TYPE_DNUM, &dy );
	code_setva( pvalresz, aptrresz, TYPE_DNUM, &dz );

	return 0;
}



//E3DGetMotiontype hsid, motid, typeptr
int E3DGetMotionType()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 motid
	int motid;
	motid = code_getdi(0);

//3 : typeptr
	PVal *pvaltype;
	APTR aptrtype;
	aptrtype = code_getva( &pvaltype );

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int type = 0;
	ret = curhs->GetMotionType( motid, &type );
	if( ret ){
		DbgOut( "E3DGetMotionType : hs GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	code_setva( pvaltype, aptrtype, TYPE_INUM, &type );

	return 0;
}

//E3DSetMotionType hsid, motid, type
int E3DSetMotionType()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 motid
	int motid;
	motid = code_getdi(0);

//3 type
	int type;
	type = code_getdi(0);

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SetMotionType( motid, type );
	if( ret ){
		DbgOut( "E3DSetMotionType : hs SetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

//E3DGetIKTransFlag hsid, jointno, flagptr
int E3DGetIKTransFlag()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 jointno
	int jointno;
	jointno = code_getdi(0);

//3 : flagptr
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int flag = 0;
	ret = curhs->GetIKTransFlag( jointno, &flag );
	if( ret ){
		DbgOut( "E3DGetIKTransFlag : hs GetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &flag );

	return 0;
}


//E3DSetIKTransFlag hsid, jointno, flag
int E3DSetIKTransFlag()
{


//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 jointno
	int jointno;
	jointno = code_getdi(0);

//3 flag
	int flag;
	flag = code_getdi(0);

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetIKTransFlag( jointno, flag );
	if( ret ){
		DbgOut( "E3DSetIKTransFlag : hs SetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DDestroyAllMotion hsid
int E3DDestroyAllMotion()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyAllMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->DestroyAllMotion();
	if( ret ){
		DbgOut( "E3DDestroyAllMotion : hs DestroyAllMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}


	
//E3DGetUserInt1OfPart hsid, partno, userintptr
int E3DGetUserInt1OfPart()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 partno
	int partno;
	partno = code_getdi(0);

//3 : userint1ptr
	PVal *pvaluserint;
	APTR aptruserint;
	aptruserint = code_getva( &pvaluserint );

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetUserInt1OfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int userint1 = 0;
	ret = curhs->GetUserInt1OfPart( partno, &userint1 );
	if( ret ){
		DbgOut( "E3DGetUserInt1OfPart : hs GetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaluserint, aptruserint, TYPE_INUM, &userint1 );

	return 0;
}

//E3DSetUserInt1OfPart hsid, partno, userint
int E3DSetUserInt1OfPart()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 partno
	int partno;
	partno = code_getdi(0);

//3 userint1
	int userint1;
	userint1 = code_getdi(0);

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUserInt1OfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUserInt1OfPart( partno, userint1 );
	if( ret ){
		DbgOut( "E3DSetUserInt1OfPart : hs SetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetBSphere hsid, partno, cenx, ceny, cenz, r
int E3DGetBSphere()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 partno
	int partno;
	partno = code_getdi(0);

//3 : cenxptr
	PVal *pvalcenx;
	APTR aptrcenx;
	aptrcenx = code_getva( &pvalcenx );

//4 : cenyptr
	PVal *pvalceny;
	APTR aptrceny;
	aptrceny = code_getva( &pvalceny );

//5 : cenzptr
	PVal *pvalcenz;
	APTR aptrcenz;
	aptrcenz = code_getva( &pvalcenz );

//6 : rptr
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );
	

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 dstcenter;
	float dstr;

	ret = curhs->GetBSphere( partno, &dstcenter, &dstr );
	if( ret ){
		DbgOut( "E3DGetBSphere : hs GetBSphere error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dr;
	dr = (double)dstr;
	code_setva( pvalr, aptrr, TYPE_DNUM, &dr );

	double dx, dy, dz;
	dx = (double)dstcenter.x;
	dy = (double)dstcenter.y;
	dz = (double)dstcenter.z;
	code_setva( pvalcenx, aptrcenx, TYPE_DNUM, &dx );
	code_setva( pvalceny, aptrceny, TYPE_DNUM, &dy );
	code_setva( pvalcenz, aptrcenz, TYPE_DNUM, &dz );

	return 0;
}


int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos )
{

	float clipw, cliph, asp;

	clipw = (float)g_scsize.x / 2.0f;
	cliph = (float)g_scsize.y / 2.0f;

	/***
	clipw = (float)s_clientWidth / 2.0f;
	cliph = (float)s_clientHeight / 2.0f;
	//Resotre���܂����s���Ă��Ȃ��\��������̂ŁAs_clientWidth�́A�g���Ȃ��B
	***/
  
	//asp = 1.0f;//!!!!!!!!!!!!
	asp = clipw / cliph;


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ���ˉe��L���ɂ���ꍇ�ɂ́Aasp���ȉ��̂悤�ɐݒ肷��K�v�L��I�I�I
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/***
	//float calaspect;
	if( m_matProj._43 < -1.0f ){
		asp = m_shandler->m_aspect;
	}else{
		//���ˉe
		asp = clipw / cliph;
	}
	***/



	D3DXMATRIX mat, invmat;
	D3DXMATRIX* retmat;

	//mat = m_matWorld * m_matView * m_matProj;

	CalcMatView();

	mat = s_matView * g_matProj;//world == ini

	retmat = D3DXMatrixInverse( &invmat, NULL, &mat );
	if( retmat == NULL ){
		_ASSERT( 0 );
		return 1;
	}
	

	float res1;
	D3DXVECTOR3 screenpos;

	screenpos.x = ( scpos.x - clipw ) / ( asp * cliph );
	screenpos.y = ( cliph - scpos.y ) / cliph;
	screenpos.z = scpos.z;

	objpos->x = screenpos.x * invmat._11 + screenpos.y * invmat._21 + screenpos.z * invmat._31 + invmat._41;
	objpos->y = screenpos.x * invmat._12 + screenpos.y * invmat._22 + screenpos.z * invmat._32 + invmat._42;
	objpos->z = screenpos.x * invmat._13 + screenpos.y * invmat._23 + screenpos.z * invmat._33 + invmat._43;
	res1 =		screenpos.x * invmat._14 + screenpos.y * invmat._24 + screenpos.z * invmat._34 + invmat._44;

	
	if( res1 == 0.0f ){
		_ASSERT( 0 );
		return 1;
	}

	objpos->x /= res1;
	objpos->y /= res1;
	objpos->z /= res1;


	return 0;
}
int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir )
{

		//planep��ʂ�Aplanedir��@���x�N�g���Ƃ��镽�ʁFax + by + cz + d = 0;
	float a, b, c, d;
	a = planedir.x; b = planedir.y; c = planedir.z;
	d = -a * planep.x - b * planep.y - c * planep.z;

		//����ax+by+cz+d=0�ƁA�_pobj1��ʂ�����x�N�g��direction�̒����Ƃ̌�_�Atarget�����߂�B
		//OpenGL�Ԗ{p402�Q��
	D3DXMATRIX mat;
	mat._11 = b * srcdir.y + c * srcdir.z;
	mat._12 = -a * srcdir.y;
	mat._13 = -a * srcdir.z;
	mat._14 = 0.0f;

	mat._21 = -b * srcdir.x;
	mat._22 = a * srcdir.x + c * srcdir.z;
	mat._23 = -b * srcdir.z;
	mat._24 = 0.0f;

	mat._31 = -c * srcdir.x;
	mat._32 = -c * srcdir.y;
	mat._33 = a * srcdir.x + b * srcdir.y;
	mat._34 = 0.0f;

	mat._41 = -d * srcdir.x;
	mat._42 = -d * srcdir.y;
	mat._43 = -d * srcdir.z;
	mat._44 = a * srcdir.x + b * srcdir.y + c * srcdir.z;

	if( mat._44 == 0.0f )
		return 1;

	dstshadow->x = (srcp.x * mat._11 + srcp.y * mat._21 + srcp.z * mat._31 + mat._41) / mat._44;
	dstshadow->y = (srcp.x * mat._12 + srcp.y * mat._22 + srcp.z * mat._32 + mat._42) / mat._44;
	dstshadow->z = (srcp.x * mat._13 + srcp.y * mat._23 + srcp.z * mat._33 + mat._43) / mat._44;


	return 0;
}

//GetChildJoint hsid, parentno, arrayleng, childarray, childnum
int E3DGetChildJoint()
{
//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 parentno
	int parentno;
	parentno = code_getdi(0);

//3 arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);


//4 : childarray
	PVal *pvalarray;
	APTR aptrarray;
	aptrarray = code_getva( &pvalarray );

//5 : childnum
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int childnum = 0;
	ret = curhs->GetChildJoint( parentno, arrayleng, (int*)pvalarray->pt + aptrarray, &childnum );
	if( ret ){
		DbgOut( "E3DGetChildJoint : hs GetChildJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &childnum );

	return 0;
}


//E3DDestroyMotionFrame hsid, motid, frameno
int E3DDestroyMotionFrame()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 motid
	int motid;
	motid = code_getdi(0);

//3 frameno
	int frameno;
	frameno = code_getdi(0);

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyMotionFrame( motid, frameno );
	if( ret ){
		DbgOut( "E3DDestroyMotionFrame : hs DestroyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetKeyFrameNo( int no, int hsid, int motid, int boneno, int* framearray, int arrayleng, int* framenumptr );
int E3DGetKeyFrameNo()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 motid
	int motid;
	motid = code_getdi(0);

//3 boneno
	int boneno;
	boneno = code_getdi(0);


//4 : framearray
	PVal *pvalarray;
	APTR aptrarray;
	aptrarray = code_getva( &pvalarray );


//5 arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);


//6 : framenumptr
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetKeyFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int framenum = 0;
	ret = curhs->HuGetKeyFrameNo( motid, boneno, (int*)pvalarray->pt + aptrarray, arrayleng, &framenum );
	if( ret ){
		DbgOut( "E3DGetKeyframeNo : hs HuGetKeyFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	code_setva( pvalnum, aptrnum, TYPE_INUM, &framenum );

	return 0;
}


//E3DConvScreenTo3D x2d, y2d, z100000, x3d, y3d, z3d
int E3DConvScreenTo3D()
{

//1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DConvScreenTo3D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//2 x2d
	int x2d;
	x2d = code_getdi(0);
//3 y2d
	int y2d;
	y2d = code_getdi(0);
//4 z
	float z;
	z = (float)code_getdd(0.0);




//5 : x3d
	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

//6 : y3d
	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

//7 : z3d
	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );
////

	int ret;

	D3DXVECTOR3 scpos;
	D3DXVECTOR3 objpos( 0.0f, 0.0f, 0.0f );

	scpos.x = (float)x2d;
	scpos.y = (float)y2d;
	scpos.z = z;

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DConvScreenTo3D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ScreenToObj( &objpos, scpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DConvScreenTo3D ; ScreenToObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

//DbgOut( "e3dhsp : E3DConvScreenTo3D : scpos %f %f %f, objpos %f %f %f\r\n",
//	   scpos.x, scpos.y, scpos.z,
//	   objpos.x, objpos.y, objpos.z );

	double dx, dy, dz;
	dx = (double)objpos.x;
	dy = (double)objpos.y;
	dz = (double)objpos.z;
	code_setva( pvalx, aptrx, TYPE_DNUM, &dx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dy );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dz );


//DbgOut( "e3dhsp : E3DConvScreenTo3D : scpos %f %f %f, objpos %f %f %f, ret %d %d %d\r\n",
//	   scpos.x, scpos.y, scpos.z,
//	   objpos.x, objpos.y, objpos.z,
//	   *x3d, *y3d, *z3d );


	return 0;
}

int E3DVec3Length()
{

//1 vecx
	float vecx;
	vecx = (float)code_getdd(0.0);
//2 vecy
	float vecy;
	vecy = (float)code_getdd(0.0);
//3 vecz
	float vecz;
	vecz = (float)code_getdd(0.0);

//4 : lengptr
	PVal *pvalleng;
	APTR aptrleng;
	aptrleng = code_getva( &pvalleng );

////

	D3DXVECTOR3 srcvec;

	srcvec.x = vecx;
	srcvec.y = vecy;
	srcvec.z = vecz;

	double dleng;
	dleng = (double)D3DXVec3Length( &srcvec );
	code_setva( pvalleng, aptrleng, TYPE_DNUM, &dleng );
	
	return 0;
}

//E3DSetUV hsid, partno, vertno, u10000, v10000, setflag
int E3DSetUV()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);
//2 partno
	int partno;
	partno = code_getdi(0);
//3 vertno
	int vertno;
	vertno = code_getdi(0);


//4 u10000
	float u;
	u = (float)code_getdd(0.0);
//5 v10000
	float v;
	v = (float)code_getdd(0.0);

//6 setflag
	int setflag;
	setflag = code_getdi(0);
//7 clampflag
	int clampflag;
	clampflag = code_getdi(0);

////

	int ret;

	CHandlerSet* curhs = 0;
	if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		curhs = GetHandlerSet( hsid );
	}
	if( !curhs ){
		DbgOut( "E3DSetUV : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUV( partno, vertno, u, v, setflag, clampflag );
	if( ret ){
		DbgOut( "E3DSetUV : hs SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	return 0;
}


//E3DGetUV hsid, partno, vertno, u10000, v10000
int E3DGetUV()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);
//2 partno
	int partno;
	partno = code_getdi(0);
//3 vertno
	int vertno;
	vertno = code_getdi(0);



//4 : u
	PVal *pvalu;
	APTR aptru;
	aptru = code_getva( &pvalu );


//5 : v
	PVal *pvalv;
	APTR aptrv;
	aptrv = code_getva( &pvalv );
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUV : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fu, fv;

	ret = curhs->GetUV( partno, vertno, &fu, &fv );
	if( ret ){
		DbgOut( "E3DGetUV : hs GetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double du, dv;
	du = (double)fu;
	dv = (double)fv;
	code_setva( pvalu, aptru, TYPE_DNUM, &du );
	code_setva( pvalv, aptrv, TYPE_DNUM, &dv );

	return 0;
}
//E3DPickBone hsid, posx, posy, boneno
int E3DPickBone()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);


//2 posx
	int posx;
	posx = code_getdi(0);
//3 posy
	int posy;
	posy = code_getdi(0);



//4 : bonenoptr
	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int boneno = -1;
	ret = curhs->PickBone( s_matView, posx, posy, &boneno );
	if( ret ){
		DbgOut( "E3DPickBone : hs PickBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &boneno );

	return 0;
}


//E3DShiftBoneTree2D hsid, jointno, motid, frameno, target2dx, target2dy 
int E3DShiftBoneTree2D()
{

//1
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DShiftBoneTree2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 hsid
	int hsid;
	hsid = code_getdi(0);

//3 jointno
	int jointno;
	jointno = code_getdi(0);

//4 motid
	int motid;
	motid = code_getdi(0);
//5 frameno
	int frameno;
	frameno = code_getdi(0);


//6 target2dx
	int target2dx;
	target2dx = code_getdi(0);
//7 target2dy
	int target2dy;
	target2dy = code_getdi(0);

////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DShiftBoneTree2D : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DShiftBoneTree2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( jointno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = (float)target2dx;
	targetsc.y = (float)target2dy;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = (float)target2dx;
	targetsc.y = (float)target2dy;
	targetsc.z = 1.0f;
	ret = ScreenToObj( &targetobj1, targetsc );
	if( ret )
		return 0;

	D3DXVECTOR3 targetdir;
	targetdir = targetobj1 - targetobj0;
	DXVec3Normalize( &targetdir, &targetdir );


	D3DXVECTOR3 planeaxis;
	planeaxis = s_cameratarget - s_camerapos;	
	DXVec3Normalize( &planeaxis, &planeaxis );


	D3DXVECTOR3 temptarget;
	ret = GetShadow( &temptarget, planeaxis, mvobj, targetobj0, targetdir );
	if( ret )
		return 0;

	D3DXVECTOR3 mvdiff;
	mvdiff = temptarget - mvobj;

/////////////
	ret = curhs->ShiftBoneTreeDiff( jointno, mvdiff, motid, frameno );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : hs ShiftBoneTreeDiff error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetDispSwitch hsid, motid, frameno, dswitch
int E3DGetDispSwitch()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);


//2 motid
	int motid;
	motid = code_getdi(0);
//3 frameno
	int frameno;
	frameno = code_getdi(0);


//4 : dswitch
	PVal *pvalds;
	APTR aptrds;
	aptrds = code_getva( &pvalds );

////

/***
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispSwitch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int dswitch = 0;
	ret = curhs->GetDispSwitch( motid, frameno, &dswitch );
	if( ret ){
		DbgOut( "E3DGetDispSwitch : hs GetDispSwitch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	int dswitch = 0;
	code_setva( pvalds, aptrds, TYPE_INUM, &dswitch );

	return 0;
}

//E3DRotateBillboard bbid, rotdeg, mult, rotkind
int E3DRotateBillboard()
{

//1 bbid
	int bbid;
	bbid = code_getdi(0);

//2 rotdeg
	float rotdeg;
	rotdeg = (float)code_getdd(0.0);

//3 rotkind
	int rotkind;
	rotkind = code_getdi(0);
////


	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DRotateBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_bbhs->RotateBillboard( bbid, rotdeg, rotkind );
	if( ret ){
		DbgOut( "E3DRotateBillboard : bbhs RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetBillboardUV bbid, unum, vnum, texno
int E3DSetBillboardUV()
{

//1 bbid
	int bbid;
	bbid = code_getdi(0);

//2 unum
	int unum;
	unum = code_getdi(1);

//3 vnum
	int vnum;
	vnum = code_getdi(1);

//4 texno
	int texno;
	texno = code_getdi(0);

//5 revuflag
	int revuflag;
	revuflag = code_getdi(0);

////

	if( unum == 0 )
		unum = 1;
	if( vnum == 0 )
		vnum = 1;

	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DSetBillboardUV : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_bbhs->SetBillboardUV( bbid, unum, vnum, texno, revuflag );
	if( ret ){
		DbgOut( "E3DSetBillboardUV : bbhs SetBillboardUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
//E3DCreateTexture�@filename, pool, transparent, texid
int E3DCreateTexture()
{
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateTexture : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 pool
	int pool;
	pool = code_getdi( D3DPOOL_MANAGED );


//3 transparent
	int transparent;
	transparent = code_getdi( 0 );


//4 texidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

////

	if( (pool != D3DPOOL_DEFAULT) && (pool != D3DPOOL_MANAGED) && (pool != D3DPOOL_SYSTEMMEM) ){
		pool = D3DPOOL_DEFAULT;
	}
	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateTexture : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DCreateTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH + 256 ];
	s_usertexcnt++;


	int cmp1;
	cmp1 = strcmp( tempname, "MainWindow" );
	if( cmp1 != 0 ){
		
		sprintf( usertexname, "%s_TEXID%d", tempname, s_usertexcnt );		
	}else{
		pool = D3DPOOL_SYSTEMMEM;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!�����ݒ�
		if( transparent == 1 ){
			transparent = 2;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!�����ݒ�
		}
		sprintf( usertexname, "%s_%d", tempname, s_usertexcnt );
	}

EnterCriticalSection( &g_crit_restore );//######## start crit

	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "E3DCreateTexture : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "E3DCreateTexture : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}

	ret = g_texbnk->CreateTexData( newindex, pdev );
	if( ret ){
		DbgOut( "E3DCreateTexture : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &newserial );


LeaveCriticalSection( &g_crit_restore );//###### end crit
	
	
	return 0;

/***
		filename�ɁA"MainWindow"���w�肷��ƁA�o�b�N�o�b�t�@�Ɠ������e�ŁA�����傫���̃e�N�X�`�����쐬�B
			�������A���̂Ƃ��́Apool�ɉ����w�肵�Ă��A�V�X�e���������ɍ����B
			�i�T�C�Y�����̂��߁j

		�e�N�X�`���������́A"filename_TEXID%d", texid�@�ɂ���B
		(�܂��́A"MainWindow_TEXID%d")

		D3DPOOL_DEFAULT                 = 0,
		D3DPOOL_MANAGED                 = 1,
		D3DPOOL_SYSTEMMEM               = 2,
***/

}

//E3DGetTextureInfo texid, width, height, pool, transparent, format
int E3DGetTextureInfo()
{
//1 texid
	int texid;
	texid = code_getdi( 0 );

//2 widthptr
	PVal *pvalw;
	APTR aptrw;
	aptrw = code_getva( &pvalw );

//3 heightptr
	PVal *pvalh;
	APTR aptrh;
	aptrh = code_getva( &pvalh );

//4 poolptr
	PVal *pvalpool;
	APTR aptrpool;
	aptrpool = code_getva( &pvalpool );

//5 transparentptr
	PVal *pvaltrans;
	APTR aptrtrans;
	aptrtrans = code_getva( &pvaltrans );

//6 formatptr
	PVal *pvalformat;
	APTR aptrformat;
	aptrformat = code_getva( &pvalformat );

////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureInfo : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( texid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DGetTextureInfo : texbank GetTextureSizeBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalw, aptrw, TYPE_INUM, &width );
	code_setva( pvalh, aptrh, TYPE_INUM, &height );
	code_setva( pvalpool, aptrpool, TYPE_INUM, &pool );
	code_setva( pvaltrans, aptrtrans, TYPE_INUM, &transparent );
	code_setva( pvalformat, aptrformat, TYPE_INUM, &format );

	return 0;
}


//E3DCopyTextureToTexture srctexid, desttexid
int E3DCopyTextureToTexture()
{


//1 srctexid
	int srctexid;
	srctexid = code_getdi( 0 );

//2 dsttexid
	int dsttexid;
	dsttexid = code_getdi( 0 );
////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTexture2Texture( srctexid, dsttexid );
	if( ret ){
		DbgOut( "E3DCopyTextureToTexture : texbank : CopyTexture2Texture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetTextureFromDispObj hsid, partno, texid
int E3DGetTextureFromDispObj()
{
//1 hsid
	int hsid;
	hsid = code_getdi( 0 );

//2 partno
	int partno;
	partno = code_getdi( 0 );

//3 texidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureFromDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	CMySprite* cursprite = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DGetTextureFromDispObj : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DGetTextureFromDispObj : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int texid = -1;
	if( curhs ){
		ret = curhs->GetTextureFromDispObj( partno, &texid );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : hs GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( cursprite ){
		ret = cursprite->GetTextureFromDispObj( &texid );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : sprite GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &texid );


	return 0;
}

//E3DSetTextureToDispObj hsid, partno, texid, transparent				
int E3DSetTextureToDispObj()
{
//1 hsid
	int hsid;
	hsid = code_getdi( 0 );

//2 partno
	int partno;
	partno = code_getdi( 0 );

//3 texid
	int texid;
	texid = code_getdi( 0 );

////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}


	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	CMySprite* cursprite = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetTextureToDispObj : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DGetTextureFromDispObj : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

////////
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToDispObj : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToDispObj : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	if( curhs ){
		ret = curhs->SetTextureToDispObj( partno, texid );
		if( ret ){
			DbgOut( "E3DSetTextureToDispObj : hs SetTextureToDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( cursprite ){
		ret = cursprite->SetTextureToDispObj( texid );
		if( ret ){
			DbgOut( "E3DSetTextureToDispObj : cursprite SetTextureToDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

//E3DRasterScrollTexture srctexid, desttexid, t, param1, param1div, param2, param2div
int E3DRasterScrollTexture()
{

//1 srctexid
	int srctexid;
	srctexid = code_getdi( 0 );

//2 desttexid
	int desttexid;
	desttexid = code_getdi( 0 );

//3 t
	int t;
	t = code_getdi( 0 );

//4 param1
	float param1;
	param1 = (float)code_getdd(0.0);

//5 param2
	float param2;
	param2 = (float)code_getdd(0.0);

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DRasterScrollTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DRasterScrollTexture : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->RasterScrollTexture( pdev, srctexid, desttexid, t, param1, param2 );
	if( ret ){
		DbgOut( "E3DRasterScrollTexture : texbank RasterScrollTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCopyTextureToBackBuffer texid
int E3DCopyTextureToBackBuffer()
{

//1 texid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCopyTextureToBackBuffer : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTextureToBackBuffer( pdev, texid );
	if( ret ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank CopyTextureToBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DDestroyTexture texid
int E3DDestroyTexture()
{

//1 texid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DDestroyTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "E3DDestroyTexture : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "E3DDestroyTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex;
	texindex = g_texbnk->FindName( texname, transparent );
	if( texindex < 0 ){
		DbgOut( "E3DDestroyTexture : texbank FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////

	ret = g_texbnk->DestroyTexture( texindex );
	_ASSERT( !ret );

/***	
	CHandlerSet* curhs;
	curhs = hshead;
	while( curhs ){
		ret = curhs->SetTextureDirtyFlag( -999999 );//�S�Ă�dirtyflag�Z�b�g�@
		_ASSERT( !ret );
		curhs = curhs->next;
	}

	if( g_bbhs ){
		ret = g_bbhs->SetTextureDirtyFlag( -999999 );//�S�Ă�dirtyflag�Z�b�g
		_ASSERT( !ret );
	}

	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->SetTextureDirtyFlag();
		cursprite = cursprite->next;
	}

////////////
	int dirtyflag;

	dirtyflag = g_texbnk->GetDirtyFlag( texindex );

	if( dirtyflag == 0 ){
		ret = g_texbnk->DestroyTexture( texindex );
		_ASSERT( !ret );
	}
***/

	return 0;
}

//E3DSetLightAmbient lightid, r, g, b
int E3DSetLightAmbient()
{

//1 lid
	int lid;
	lid = code_getdi( 0 );

//2 r
	int r;
	r = code_getdi( 0 );

//3 g
	int g;
	g = code_getdi( 0 );

//4 b
	int b;
	b = code_getdi( 0 );
////
	
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightAmbient : lightID not found error !!!\n" );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f ambcol;
	ambcol.x = (float)r / 255.0f;
	ambcol.y = (float)g / 255.0f;
	ambcol.z = (float)b / 255.0f;

	curlight->Ambient = ambcol;

////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetLightAmbient : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}


//E3DSetLightSpecular lightid, r, g, b
int E3DSetLightSpecular()
{

//1 lid
	int lid;
	lid = code_getdi( 0 );

//2 r
	int r;
	r = code_getdi( 0 );

//3 g
	int g;
	g = code_getdi( 0 );

//4 b
	int b;
	b = code_getdi( 0 );

////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightSpecular : lightID not found error !!!\n" );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f speccol;
	speccol.x = (float)r / 255.0f;
	speccol.y = (float)g / 255.0f;
	speccol.z = (float)b / 255.0f;

	curlight->Specular = speccol;

////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetLightSpecular : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}

//E3DInvColTexture texid
int E3DInvColTexture()
{

//1 lid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DInvColTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DInvColTexture : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->InvColTexture( pdev, texid );
	if( ret ){
		DbgOut( "E3DInvColTexture : texbank InvColTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSaveGndFile()
{
	//type $202
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = code_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	int ret;
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveGNDFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


////

	return 0;//  !!!!!!!!!!! �T�|�[�g�I���@!!!!!!!!!!!!!

/***	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveGNDFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SaveGNDFile( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveGNDFile : curhs SaveGNDFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
***/
}

int E3DLoadGndFile()
{

	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult
//first
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGndFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//second	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

// 3:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 4:
	float mult;
	mult = (float)code_getdd(1.0);

//////



	int temphsid = -1;
	ret = LoadGNDFile_F( tempname, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFile : LoadGNDFile_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	
	return 0;
}

int LoadGNDFile_F( char* tempname, int adjustuvflag, float mult, int* hsidptr )
{

	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!



	ret = newhs->LoadGNDFile( tempname, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : E3DLoadGNDFile : LoadGNDFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;

	return 0;
}

int E3DLoadSigFileAsGround()
{
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult
//first
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//second	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

// 3:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 4:
	float mult;
	mult = (float)code_getdd(1.0);

//////

	int temphsid = -1;
	ret = LoadSigFileAsGround_F( tempname, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGround : LoadSigFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	return 0;
}

int LoadSigFileAsGround_F( char* tempname, int adjustuvflag, float mult, int* hsidptr )
{
	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!

	ret = newhs->LoadSigFile( tempname, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : LoadSigFileAsGround_F : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : LoadSigFileAsGround_F : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;

	return 0;
}



//E3DLoadGNDFileFromBuf resdir, buf, bufsize, hsidptr, adjustuvflag
	//resdir�͍Ō��\\��Y�ꂸ�ɁB
int E3DLoadGndFileFromBuf()
{

//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGndFileFromBuf : resdir name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );
//5
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//////

	int temphsid = -1;
	ret = LoadGNDFileFromBuf_F( tempname, (char*)pvalbuf->pt + aptrbuf, bufsize, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	return 0;
}

int LoadGNDFileFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int* hsidptr )
{

	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!


	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	ret = newhs->LoadGNDFileFromBuf( bufptr, bufsize, adjustuvflag );
	if( ret ){
		DbgOut( "easy3d : E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );

	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;


	return 0;
}

int E3DLoadSigFileAsGroundFromBuf()
{
//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGroundFromBuf : resdir name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );
//5
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 6:
	float mult;
	mult = (float)code_getdd(1.0);

//////

	int temphsid = -1;
	ret = LoadSigFileAsGroundFromBuf_F( tempname, (char*)pvalbuf->pt + aptrbuf, bufsize, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGroundFromBuf : E3DLoadSigFileAsGroundFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );


	return 0;
}

int LoadSigFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!


	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	ret = newhs->LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : LoadSigFileAsGroundFromBuf_F : LoadSigFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );

	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : LoadSigFileAsGroundFromBuf_F : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;


	return 0;
}

//E3DCreateTextureFromBuf buf, bufleng, pool, transparent, texid
int E3DCreateTextureFromBuf()
{

//1
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//2
	int bufsize;
	bufsize = code_getdi(0);

//3 pool
	int pool;
	pool = code_getdi( D3DPOOL_MANAGED );


//4 transparent
	int transparent;
	transparent = code_getdi( 0 );


//5 texidptr
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


////
	int ret;


	if( bufsize <= 0 ){
		DbgOut( "E3DCreateTextureFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (pool != D3DPOOL_DEFAULT) && (pool != D3DPOOL_MANAGED) && (pool != D3DPOOL_SYSTEMMEM) ){
		pool = D3DPOOL_DEFAULT;
	}
	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateTextureFromBuf : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DCreateTextureFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH + 256 ];
	s_usertexcnt++;


	sprintf( usertexname, "FromBuf_TEXID%d", s_usertexcnt );


	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->SetBufData( newindex, (char*)pvalbuf->pt + aptrbuf, bufsize );
	if( ret ){
		DbgOut( "E3DCreateTextureFromBuf : texbank SetBufData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CreateTexData( newindex, pdev );
	if( ret ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &newserial );

	return 0;

}

//E3DLoadSoundFromBuf buf, bufleng, type, use3dflag, bufnum, soundid
//type 0 --> wav, type 1 --> midi
int E3DLoadSoundFromBuf()
{
//1
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//2
	int bufsize;
	bufsize = code_getdi(0);

//3 
	int type;
	type = code_getdi(0);

//4
	int use3dflag;
	use3dflag = code_getdi(0);

//5
	int reverbflag;
	reverbflag = code_getdi(1);

//6
	int bufnum;
	bufnum = code_getdi(1);


//7
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//8
	int oggdatanum;
	oggdatanum = code_getdi(16);


//////	
	int ret;

	if( !s_HS ){
		DbgOut( "E3DLoadSoundFromBuf : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	type = 0;//midi, ogg�֎~

// �ǂݍ���
	int soundid = -1;
	ret = s_HS->LoadSoundFileFromBuf( (char*)pvalbuf->pt + aptrbuf, bufsize, type, use3dflag, reverbflag, bufnum, oggdatanum, &soundid );
	if( ret ){
		DbgOut( "E3DLoadSoundFromBuf : hs LoadSoundFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &soundid );

	return 0;
}

//E3DTwistBone hsid, motid, frameno, boneno, twistdeg, divdeg
int E3DTwistBone()
{

	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	float twistdeg;
	twistdeg = (float)code_getdd(0.0);
/////////////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwistBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->TwistBone( motid, frameno, boneno, twistdeg );
	if( ret ){
		DbgOut( "E3DTwistBone : hs TwistBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetStartPointOfSound soundid, sec1000
int E3DSetStartPointOfSound()
{

	int soundid;
	soundid = code_getdi(0);

	int sec;
	sec = code_getdi(0);	
/////////////
	/***

	if( sec < 0 ){
		sec = 0;//!!!!!!!!!!!!!!!!!!!
	}


	if( !s_HS ){
		DbgOut( "E3DSetStartPointOfSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetStartPointOfSound( soundid, sec );
	if( ret ){
		DbgOut( "E3DSetStartPointOfSound : hs SetTartPointOfSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}

//E3DGetBoneQ hsid, boneno, motid, frameno, kind, qid
//	kind 0 --> �e�̉e���̂Ȃ��N�H�[�^�j�I��
//	kind 1 --> �e�̉e�����l�������N�H�[�^�j�I��
//  kind 2 --> �e�̉e���{���f���S�̂̃N�H�[�^�j�I���̉e��
int E3DGetBoneQ()
{

	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int kind;
	kind = code_getdi(0);
	
	int qid;
	qid = code_getdi(0);

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DGetBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion dstq;
	ret = curhs->GetBoneQ( boneno, motid, frameno, kind, &dstq );
	if( ret ){
		DbgOut( "E3DGetBoneQ : hs GetBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( qid, &setq );
	if( ret ){
		DbgOut( "E3DGetBoneQ : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




//E3DSetBoneQ hsid, boneno, motid, frameno, qid
int E3DSetBoneQ()
{
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);
	
	int qid;
	qid = code_getdi(0);

/////////////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DSetBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetBoneQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion setq;
	setq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!


	ret = curhs->SetQ( boneno, motid, frameno, setq );
	if( ret ){
		DbgOut( "E3DSetBoneQ : hs SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DIsSoundPlaying soundid, playing
//	�Đ����@playing 1
//	��~���@playing 0
int E3DIsSoundPlaying()
{

	int soundid;
	soundid = code_getdi(0);

	PVal *pvalplaying;
	APTR aptrplaying;
	aptrplaying = code_getva( &pvalplaying );

/////////////
	int ret;

	if( !s_HS ){
		DbgOut( "E3DIsSoundPlaying : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int playing = 0;
	ret = s_HS->IsSoundPlaying( soundid, &playing );
	if( ret ){
		DbgOut( "E3DIsSoundPlaying : hs IsSoundPlaying error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalplaying, aptrplaying, TYPE_INUM, &playing );

	return 0;
}


//E3DIKTranslate hsid, boneno, motid, frameno, posx, posy, posz
int E3DIKTranslate()
{

	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);
	
	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);

/////////////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKTranslate : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcpos;
	srcpos.x = posx;
	srcpos.y = posy;
	srcpos.z = posz;


	ret = curhs->IKTranslate( boneno, motid, frameno, srcpos );
	if( ret ){
		DbgOut( "E3DIKTranslate : hs IKTranslate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetUVTile hsid, partno, texrule, unum, vnum, tileno
//	texrule 0 --> �w�����e
//		 1 --> �x�����e
//		 2 --> �y�����e
//		 3 --> �~���\��t��
//		 4 --> ���\��t��
int E3DSetUVTile()
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	int texrule;
	texrule = code_getdi(0);

	int unum;
	unum = code_getdi(1);
	
	int vnum;
	vnum = code_getdi(1);

	int tileno;
	tileno = code_getdi(0);

/////////////
	int ret;

	if( unum == 0 )
		unum = 1;
	if( vnum == 0 )
		vnum = 1;

	if( (texrule < TEXRULE_X) || (texrule > TEXRULE_SPHERE) ){
		DbgOut( "E3DSetUVTile : illeagal texrule : set default value TEXRULE_Z warning !!!\n" );
		texrule = TEXRULE_Z;//!!!!!
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUVTile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUVTile( partno, texrule, unum, vnum, tileno );
	if( ret ){
		DbgOut( "E3DSetUVTile : hs SetUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DImportMQOFileAsGround hsid, filename, adjustuvflagg, mult100, offsetx, offsety, offsetz
int E3DImportMQOFileAsGround()
{
	
	int hsid;
	hsid = code_getdi(0);

/////////
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

	float mult;
	mult = (float)code_getdd(1.0);


	float offsetx;
	offsetx = (float)code_getdd(0.0);
	float offsety;
	offsety = (float)code_getdd(0.0);
	float offsetz;
	offsetz = (float)code_getdd(0.0);


	float rotx;
	rotx = (float)code_getdd(0.0);
	float roty;
	roty = (float)code_getdd(0.0);
	float rotz;
	rotz = (float)code_getdd(0.0);
/////////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DImportMQOFileAsGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 offset;
	offset.x = offsetx;
	offset.y = offsety;
	offset.z = offsetz;

	D3DXVECTOR3 rot;
	rot.x = rotx;
	rot.y = roty;
	rot.z = rotz;

	ret = curhs->ImportMQOFileAsGround( tempname, s_hwnd, adjustuvflag, mult, offset, rot );
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : hs ImportMOQFileAsGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////////

//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////
	/***
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
	return 0;
}


//E3DLoadMQOFileAsMovableAreaFromBuf buf, bufsize, mult100, hsidptr 
int E3DLoadMQOFileAsMovableAreaFromBuf()
{
//1
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//2
	int bufsize;
	bufsize = code_getdi(0);

//3
	float mult;
	mult = (float)code_getdd(1.0);

//4	
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

//////

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( (char*)pvalbuf->pt + aptrbuf, bufsize, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
	}
	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );

	return 0;

}

int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, float mult, int* hsidptr )
{


	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->LoadMQOFileAsMovableAreaFromBuf( s_hwnd, bufptr, bufsize, mult );
	if( ret ){
		DbgOut( "LoadMQOFileAsMovableAreaFromBuf : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	/***
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/
	*hsidptr = newhs->serialno;

	return 0;
}


//E3DChkThreadWorking threadid, working, returnval
int E3DChkThreadWorking()
{

//1
	int threadid;
	threadid = code_getdi(0);


//2
	PVal *pvalworking;
	APTR aptrworking;
	aptrworking = code_getva( &pvalworking );

//3
	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

//4
	PVal *pvalret2;
	APTR aptrret2;
	aptrret2 = code_getva( &pvalret2 );

////////////////

	if( (threadid < 0) || (threadid >= THMAX) ){
		DbgOut( "E3DChkThreadWorking : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_thtable[threadid].useflag == 0 ){
		DbgOut( "E3DChkThreadWorking : this thread is invalid : useflag == 0 : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int working, ret, ret2;

	if( s_thtable[threadid].hThread ){
		DWORD dwret;
		dwret = WaitForSingleObject( s_thtable[threadid].hThread, 0 );

		switch( dwret ){
		case WAIT_OBJECT_0:
			working = 0;
			ret = s_thtable[threadid].returnval;
			ret2 = s_thtable[threadid].returnval2;
			break;
		case WAIT_TIMEOUT:
			working = 1;
			ret = -1;
			ret2 = -1;
			break;
		case WAIT_FAILED:
			DbgOut( "E3DChkWorkingThread : WaitForSigleObject WAIT_FAILED warning !!!\n" );
			_ASSERT( 0 );
			working = 0;
			ret = -1;
			ret2 = -1;
			break;
		default:
			_ASSERT( 0 );
			working = 0;
			ret = -1;
			ret2 = -1;
			break;
		}
	}else{
		working = 0;
		ret = s_thtable[threadid].returnval;
		ret2 = s_thtable[threadid].returnval2;
	}

	code_setva( pvalworking, aptrworking, TYPE_INUM, &working );
	code_setva( pvalret, aptrret, TYPE_INUM, &ret );
	code_setva( pvalret2, aptrret2, TYPE_INUM, &ret2 );


	return 0;
}


//E3DLoadMQOFileAsGroundThread( char* filename, int mult100, int adjustuvflag, int* threadid );
int E3DLoadMQOFileAsGroundThread()
{
//1
	int ret;
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : mult
	float mult;
	mult = (float)code_getdd(1.0);

//3
	int adjustuvflag;
	adjustuvflag = code_getdi(0);


//4	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////	

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DMQOFileAsGroundThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;
	
	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname1 );


	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsGround, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 0;
	}


	//*threadidptr = s_thtable[thno].threadid;

	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );


	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( curtable->filename, curtable->mult, s_hwnd, curtable->adjustuvflag, &temphsid, BONETYPE_RDB2 );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;

}

int E3DSigLoadThread()
{
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult100
//first
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigLoadThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 3:
	float mult;
	mult = (float)code_getdd(1.0);

//4	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSigLoadThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoad, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSigLoadThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;

}

DWORD WINAPI ThreadSigLoad( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = SigLoad_F( curtable->filename, curtable->mult, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DSigLoadFromBufThread()
{

//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSigLoadFromBufThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//5
	float mult;
	mult = (float)code_getdd(1.0);

//6	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );


//////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSigLoadFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].bufptr = (char*)pvalbuf->pt + aptrbuf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoadFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSigLoadFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}

DWORD WINAPI ThreadSigLoadFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = SigLoadFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DLoadMQOFileAsMovableAreaThread()
{
	// 1 : mqo file name1
	// 2 : mult100
	// 3 : pointer of handlerset id.

//1
	int ret;
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLOadMQOFileAsMovableAreaThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2 : mult
	float mult;
	mult = (float)code_getdd(1.0);


//3	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////	

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname1 );

	s_thtable[thno].mult = mult;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableArea, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	
	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( curtable->filename, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DLoadMQOFileAsMovableAreaFromBufThread()
{

//1
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//2
	int bufsize;
	bufsize = code_getdi(0);

//3
	float mult;
	mult = (float)code_getdd(1.0);

//4	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;

	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;


	s_thtable[thno].mult = mult;
	s_thtable[thno].bufptr = (char*)pvalbuf->pt + aptrbuf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableAreaFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;

}

DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( curtable->bufptr, curtable->bufsize, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = temphsid;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DLoadGroundBMPThread()
{
	// 1 : bmp file name1
	// 2 : bmp file name2
	// 3 : bmp file name3
	// 4 : texture file name
	// 5 : maxx
	// 6 : maxz
	// 7 : divx
	// 8 : divz
	// 9 : maxheight
	// 10 : pointer of handlerset id.

	// 11 : TLmode , ver1036

//1
	int ret;
	char tempname1[MAX_PATH];
	ret = GetName( tempname1, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMPThread : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//2
	char tempname2[MAX_PATH];
	ret = GetName( tempname2, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMPThread : filename2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//3
	char tempname3[MAX_PATH];
	ret = GetName( tempname3, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMPThread : filename3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//4
	char tempname4[MAX_PATH];
	ret = GetName( tempname4, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGroundBMPThread : filename4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//5
	float maxx;
	maxx = (float)code_getdd(0.0);
//6
	float maxz;
	maxz = (float)code_getdd(0.0);

//7	
	int divx;
	divx = code_getdi(0);
	if( divx < 2 )
		divx = 2;
//8
	int divz;
	divz = code_getdi(0);
	if( divz < 2 )
		divz = 2;
//9
	float maxheight;
	maxheight = (float)code_getdd(0.0);
	if( maxheight < 0 )
		maxheight = 0;
//10	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadGroundBMPThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;


	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname1 );


	leng = (int)strlen( tempname2 );
	s_thtable[thno].filename2 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename2) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename2, tempname2 );


	leng = (int)strlen( tempname3 );
	s_thtable[thno].filename3 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename3) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename3, tempname3 );

	leng = (int)strlen( tempname4 );
	s_thtable[thno].filename4 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename4) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename4, tempname4 );


	s_thtable[thno].maxx = maxx;
	s_thtable[thno].maxz = maxz;
	s_thtable[thno].divx = divx;
	s_thtable[thno].divz = divz;
	s_thtable[thno].maxheight = maxheight;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGroundBMP, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadGroundBMPThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}

DWORD WINAPI ThreadLoadGroundBMP( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGroundBMP_F( curtable->filename, curtable->filename2, curtable->filename3, curtable->filename4, curtable->maxx, curtable->maxz, curtable->divx, curtable->divz, curtable->maxheight, 2, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : LoadGroundBMP_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DLoadGndFileThread()
{

//first
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGndFileThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 3:
	float mult;
	mult = (float)code_getdd(1.0);

//4	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadGNDFileThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadGNDFileThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGNDFile, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadGNDFileThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}

DWORD WINAPI ThreadLoadGNDFile( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGNDFile_F( curtable->filename, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFile : LoadGNDFile_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}
	curtable->returnval = temphsid;

	return 0;
}

int E3DLoadGndFileFromBufThread()
{
//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGndFileFromBufThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//5	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

//////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadGNDFileFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadGNDFileFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	//s_thtable[thno].mult100 = mult100;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].bufptr = (char*)pvalbuf->pt + aptrbuf;
	s_thtable[thno].bufsize = bufsize;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGNDFileFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadGNDFileFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}

DWORD WINAPI ThreadLoadGNDFileFromBuf( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGNDFileFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DAddMotionThread()
{

	int hsid;
	hsid = code_getdi(0);

//////////
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DAddMotionThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DAddMotionThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DAddMotionThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].hsid = hsid;//!!!!!!!!!!!!!!!!!!!!!!

	//s_thtable[thno].adjustuvflag = adjustuvflag;

	//s_thtable[thno].bufptr = bufptr;
	//s_thtable[thno].bufsize = bufsize;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotion, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DAddMotionThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		s_thtable[thno].returnval2 = -1;
		return 1;
	}

	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;

}

DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotion_F( curtable->hsid, curtable->filename, &tempmotid, &tempnum, 1.0f );
	if( ret ){
		DbgOut( "E3DAddMotion : AddMotoin_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		curtable->returnval2 = 0;
		return 0;
	}
	curtable->returnval = tempmotid;
	curtable->returnval2 = tempnum;

	return 0;
}

int E3DAddMotionFromBufThread()
{

//1
	int hsid;
	hsid = code_getdi(0);

//2
	int datakind;
	datakind = code_getdi(0);

//3
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//4
	int bufsize;
	bufsize = code_getdi(0);

//5	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DAddMotionFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;
	

	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	s_thtable[thno].hsid = hsid;//!!!!!!!!!!!!!!!!!!!!!!
	s_thtable[thno].adjustuvflag = datakind;

	s_thtable[thno].bufptr = (char*)pvalbuf->pt + aptrbuf;
	s_thtable[thno].bufsize = bufsize;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotionFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DAddMotionFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		s_thtable[thno].returnval2 = -1;
		return 1;
	}

	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}

DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotionFromBuf_F( curtable->hsid, curtable->adjustuvflag, curtable->bufptr, curtable->bufsize, &tempmotid, &tempnum, 1.0f );
	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : AddMotionFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		curtable->returnval2 = 0;
		return 0;
	}

	curtable->returnval = tempmotid;
	curtable->returnval2 = tempnum;

	return 0;
}

//E3DGetShaderType hsid, shader, overflow
int E3DGetShaderType()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);


//2 : shader
	PVal *pvalshader;
	APTR aptrshader;
	aptrshader = code_getva( &pvalshader );

//3 : overflow
	PVal *pvaloverflow;
	APTR aptroverflow;
	aptroverflow = code_getva( &pvaloverflow );

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int shader, overflow;
	ret = curhs->GetShaderType( &shader, &overflow );
	if( ret ){
		DbgOut( "E3DGetShaderType : hs GetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalshader, aptrshader, TYPE_INUM, &shader );
	code_setva( pvaloverflow, aptroverflow, TYPE_INUM, &overflow );

	return 0;
}

//E3DSetShaderType hsid, shader, overflow
int E3DSetShaderType()
{


//1 hsid
	int hsid;
	hsid = code_getdi(0);


//2 : shader
	int shader;
	shader = code_getdi(0);

//3 : overflow
	int overflow;
	overflow = code_getdi(0);

////
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DSetShaderType : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetShaderType( pdev, shader, overflow );
	if( ret ){
		DbgOut( "E3DSetShaderType : hs SetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetLightBlendMode lid, mode
int E3DSetLightBlendMode()
{

//1 lid
	int lid;
	lid = code_getdi(0);

//2 : mode
	int mode;
	mode = code_getdi(0);

////


	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightBlendMode : lightID not found error !!!\n" );
		return 1;
	}

	if( (mode >= LBLEND_MULT) && (mode < LBLEND_MAX) ){
		curlight->lblend = mode;
	}

	return 0;
}

//E3DGetLightBlendMode lid, mode
int E3DGetLightBlendMode()
{

//1 lid
	int lid;
	lid = code_getdi(0);

//2 : mode
	PVal *pvalmode;
	APTR aptrmode;
	aptrmode = code_getva( &pvalmode );

////

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DGetLightBlendMode : lightID not found error !!!\n" );
		return 1;
	}

	code_setva( pvalmode, aptrmode, TYPE_INUM, &curlight->lblend );

	return 0;
}

//E3DSetEmissive hsid, partno, r, g, b, setflag, vertno
int E3DSetEmissive()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : r
	int r;
	r = code_getdi(0);

	// 4 : g
	int g;
	g = code_getdi(0);

	// 5 : b
	int b;
	b = code_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = code_getdi(0);

	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetEmissive : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetEmissive : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	int ret;
	ret = curhs->SetDispEmissive( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetEmissive : curhs SetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
//E3DSetSpecularPower hsid, partno, pow, mult, setflag, vertno
int E3DSetSpecularPower()
{
	
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	// 3 : pow
	float pow;
	pow = (float)code_getdd(0.0);


	// 4 : setflag
	int setflag;
	setflag = code_getdi(0);

	// 5 : vertno
	int vertno;
	vertno = code_getdi(-1);

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecularPower : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecularPower : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( pow < 0.0f )
		pow = 0.0f;
	if( pow > 100.0f )
		pow = 100.0f;


	int ret;
	ret = curhs->SetDispSpecularPower( partno, pow, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecularPower : curhs SetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
//E3DGetEmissive hsid, partno, vertno, r, g, b
int E3DGetEmissive()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : rptr
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	//5 : gptr
	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	//6 : bptr
	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetEmissive : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetEmissive : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	int r, g, b;
	ret = curhs->GetEmissive( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetEmissive : curhs GetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );


	return 0;
}


//E3DGetSpecularPower hsid, partno, vertno, pow100
int E3DGetSpecularPower()
{

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//4 : powptr
	PVal *pvalpow;
	APTR aptrpow;
	aptrpow = code_getva( &pvalpow );

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecularPower : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecularPower : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	float fpow = 0.0f;

	int ret;
	ret = curhs->GetSpecularPower( partno, vertno, &fpow );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecularPower : curhs GetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dpow;
	dpow = (double)fpow;
	code_setva( pvalpow, aptrpow, TYPE_DNUM, &dpow );

	return 0;
}


//E3DGetInfElemNum hsid, partno, vertno, num
int E3DGetInfElemNum()
{
//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : numptr
	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElemNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int num = 0;
	ret = curhs->GetInfElemNum( partno, vertno, &num );
	if( ret ){
		DbgOut( "E3DGetInfElemNum : curhs GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &num );

	return 0;

}

//E3DGetInfElem hsid, partno, vertno, infno, bonenoptr, calcmodeptr, userrate10000ptr, orginf10000ptr, dispinf10000ptr
int E3DGetInfElem()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : infno
	int infno;
	infno = code_getdi(0);

//5 : childjointptr
	PVal *pvalchild;
	APTR aptrchild;
	aptrchild = code_getva( &pvalchild );

//6 : calcmodeptr
	PVal *pvalmode;
	APTR aptrmode;
	aptrmode = code_getva( &pvalmode );


//7 : userrate
	PVal *pvaluserrate;
	APTR aptruserrate;
	aptruserrate = code_getva( &pvaluserrate );

//8 : orginf
	PVal *pvalorginf;
	APTR aptrorginf;
	aptrorginf = code_getva( &pvalorginf );

//9 : dispinf
	PVal *pvaldispinf;
	APTR aptrdispinf;
	aptrdispinf = code_getva( &pvaldispinf );

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int childjoint, calcmode;
	float userrate, orginf, dispinf;
	ret = curhs->GetInfElem( partno, vertno, infno, &childjoint, &calcmode, &userrate, &orginf, &dispinf );
	if( ret ){
		DbgOut( "E3DGetInfElem : curhs GetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalchild, aptrchild, TYPE_INUM, &childjoint );
	code_setva( pvalmode, aptrmode, TYPE_INUM, &calcmode );

	double duserrate, dorginf, ddispinf;
	duserrate = (double)userrate * 100.0f;//!!!! %
	dorginf = (double)orginf;
	ddispinf = (double)dispinf;
	code_setva( pvaluserrate, aptruserrate, TYPE_DNUM, &duserrate );
	code_setva( pvalorginf, aptrorginf, TYPE_DNUM, &dorginf );
	code_setva( pvaldispinf, aptrdispinf, TYPE_DNUM, &ddispinf );

	return 0;

}


//E3DSetInfElem hsid, partno, vertno, childjointno, calcmode, paintmode, normalizeflag, userrate10000, directval10000
int E3DSetInfElem()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : childjointno
	int childjointno;
	childjointno = code_getdi(0);

//5 : calcmode
	int calcmode;
	calcmode = code_getdi(0);

//6 : paintmode
	int paintmode;
	paintmode = code_getdi(0);

//7 : normalizeflag
	int normalizeflag;
	normalizeflag = code_getdi(0);

//8 : userrate
	float userrate;
	userrate = (float)code_getdd(0.0);

//9 : directval
	float directval;
	directval = (float)code_getdd(0.0);

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fuserrate;
	fuserrate = userrate / 100.0f;// % --> ���l

	if( fuserrate < 0.0f ){
		_ASSERT( 0 );
		fuserrate = 0.0f;
	}

	if( directval < 0.0f ){
		_ASSERT( 0 );
		directval = 0.0f;
	}


	ret = curhs->SetInfElem( partno, vertno, childjointno, calcmode, paintmode, normalizeflag, fuserrate, directval );
	if( ret ){
		DbgOut( "E3DSetInfElem : curhs SetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DDeleteInfElem hsid, partno, vertno, childjointno, normalizeflag
int E3DDeleteInfElem()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

//4 : childjointno
	int childjointno;
	childjointno = code_getdi(0);

//5 : normalizeflag
	int normalizeflag;
	normalizeflag = code_getdi(0);


////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DeleteInfElem( partno, vertno, childjointno, normalizeflag );
	if( ret ){
		DbgOut( "E3DDeleteInfElem : curhs DeleteInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DSetInfElemDefault hsid, partno, vertno
int E3DSetInfElemDefault()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfElemDefault : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetInfElemDefault( partno, vertno );
	if( ret ){
		DbgOut( "E3DSetInfElemDefault : curhs SetInfElemDefault error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DNormalizeInfElem hsid, partno, vertno
int E3DNormalizeInfElem()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertno
	int vertno;
	vertno = code_getdi(0);

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNormalizeInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->NormalizeInfElem( partno, vertno );
	if( ret ){
		DbgOut( "E3DNormalizeInfElem : curhs NormalizeInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetVisiblePolygonNum hsid, num1, num2
int E3DGetVisiblePolygonNum()
{

//1 hsid
	int hsid;
	hsid = code_getdi(0);

//2 num1
	PVal *pvalnum1;
	APTR aptrnum1;
	aptrnum1 = code_getva( &pvalnum1 );

//3 num2
	PVal *pvalnum2;
	APTR aptrnum2;
	aptrnum2 = code_getva( &pvalnum2 );

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVisiblePolygonNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int num1 = 0;
	int num2 = 0;
	ret = curhs->GetVisiblePolygonNum( &num1, &num2 );
	if( ret ){
		DbgOut( "E3DGetVisiblePolygonNum : curhs GetVisiblePolygonNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum1, aptrnum1, TYPE_INUM, &num1 );
	code_setva( pvalnum2, aptrnum2, TYPE_INUM, &num2 );


	return 0;
}


//E3DChkConfGroundPart charaid, groundid, groundpart, mode, diffmaxy, mapminy, result, adjustx, adjusty, adjustz, nx10000, ny10000, nz10000
int E3DChkConfGroundPart()
{
	// 1 : charahsid
	// 2 : groundhsid
	// 3 : groundpart
	// 4 : mode
	// 5 : diffmaxy
	// 6 : mapminy
	// 7 : result
	// 8 : adjustx
	// 9 : adjusty
	// 10 : adjustz
	// 11 : nx10000
	// 12 : ny10000
	// 13 : nz10000


// 1 : charahsid
	int charahsid;
	charahsid = code_getdi(0);

// 2 : groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 3 : groundpart
	int groundpart;
	groundpart = code_getdi(0);

// 4 : mode
	int mode;
	mode = code_getdi(0);

// 5 : diffmaxy
	float diffmaxy;
	diffmaxy = (float)code_getdd(0.0);

// 6 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

// 7 : result
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );

// 8 : adjustx
	PVal *pvalax;
	APTR aptrax;
	aptrax = code_getva( &pvalax );

// 9 : adjusty
	PVal *pvalay;
	APTR aptray;
	aptray = code_getva( &pvalay );

// 10 : adjustz
	PVal *pvalaz;
	APTR aptraz;
	aptraz = code_getva( &pvalaz );

// 11 : nx
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 12 : ny
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 13 : nz
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

//////////

	int ret;

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result;
	float ax, ay, az, nx, ny, nz;
	ret = groundhs->ChkConfGroundPart( charahs, groundpart, mode, diffmaxy, mapminy, &result, &ax, &ay, &az, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGroundPart : groundhs ChkConfGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	double dax, day, daz, dnx, dny, dnz;
	dax = (double)ax;
	day = (double)ay;
	daz = (double)az;
	code_setva( pvalax, aptrax, TYPE_DNUM, &dax );
	code_setva( pvalay, aptray, TYPE_DNUM, &day );
	code_setva( pvalaz, aptraz, TYPE_DNUM, &daz );
	
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );

	return 0;
}


//E3DChkConfGroundPart2 befposx, befposy, befposz, newposx, newposy, newposz, groundhsid, groundpart, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
int E3DChkConfGroundPart2()
{
	// 1 : befposx
	// 2 : befposy
	// 3 : befposz

	// 4 : newposx
	// 5 : newposy
	// 6 : newposz

	// 7 : groundhsid
	// 8 : groundpart

	// 9 : mode
	// 10 : diffmaxy
	// 11 : mapminy
	// 12 : result
	// 13 : adjustx
	// 14 : adjusty
	// 15 : adjustz
	// 16 : nx10000
	// 17 : ny10000
	// 18 : nz10000

// 1 : befposx
	float befposx;
	befposx = (float)code_getdd(0.0);

// 2 : befposy
	float befposy;
	befposy = (float)code_getdd(0.0);

// 3 : befposz
	float befposz;
	befposz = (float)code_getdd(0.0);

// 4 : newposx
	float newposx;
	newposx = (float)code_getdd(0.0);

// 5 : newposy
	float newposy;
	newposy = (float)code_getdd(0.0);

// 6 : newposz
	float newposz;
	newposz = (float)code_getdd(0.0);

// 7 : groundhsid
	int groundhsid;
	groundhsid = code_getdi(0);

// 8 : groundpart
	int groundpart;
	groundpart = code_getdi(0);

// 9 : mode
	int mode;
	mode = code_getdi(0);

// 10 : diffmaxy
	float diffmaxy;
	diffmaxy = (float)code_getdd(0.0);

// 11 : mapminy
	float mapminy;
	mapminy = (float)code_getdd(0.0);

// 12 : result
	PVal *pvalresult;
	APTR aptrresult;
	aptrresult = code_getva( &pvalresult );

// 13 : adjustx
	PVal *pvalax;
	APTR aptrax;
	aptrax = code_getva( &pvalax );

// 14 : adjusty
	PVal *pvalay;
	APTR aptray;
	aptray = code_getva( &pvalay );

// 15 : adjustz
	PVal *pvalaz;
	APTR aptraz;
	aptraz = code_getva( &pvalaz );

// 16 : nx
	PVal *pvalnx;
	APTR aptrnx;
	aptrnx = code_getva( &pvalnx );

// 17 : ny
	PVal *pvalny;
	APTR aptrny;
	aptrny = code_getva( &pvalny );

// 18 : nz
	PVal *pvalnz;
	APTR aptrnz;
	aptrnz = code_getva( &pvalnz );

//////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundPart2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befv, newv;
	befv.x = befposx;
	befv.y = befposy;
	befv.z = befposz;

	newv.x = newposx;
	newv.y = newposy;
	newv.z = newposz;

	int result;
	float ax, ay, az, nx, ny, nz;
	ret = groundhs->ChkConfGroundPart2( groundpart, befv, newv, mode, diffmaxy, mapminy, &result, &ax, &ay, &az, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalresult, aptrresult, TYPE_INUM, &result );

	double dax, day, daz, dnx, dny, dnz;
	dax = (double)ax;
	day = (double)ay;
	daz = (double)az;
	code_setva( pvalax, aptrax, TYPE_DNUM, &dax );
	code_setva( pvalay, aptray, TYPE_DNUM, &day );
	code_setva( pvalaz, aptraz, TYPE_DNUM, &daz );
	
	dnx = (double)nx;
	dny = (double)ny;
	dnz = (double)nz;
	code_setva( pvalnx, aptrnx, TYPE_DNUM, &dnx );
	code_setva( pvalny, aptrny, TYPE_DNUM, &dny );
	code_setva( pvalnz, aptrnz, TYPE_DNUM, &dnz );


	return 0;
}


int E3DGetMidiMusicTime()
{

// 1 : soundid
	int soundid;
	soundid = code_getdi(0);

// 2 : mtimeptr
	PVal *pvaltime;
	APTR aptrtime;
	aptrtime = code_getva( &pvaltime );
/***
//////////

	if( !s_HS ){
		DbgOut( "E3DGetMidiMusicTime : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int mtime;
	double rtime;
	ret = s_HS->GetMusicTime( soundid, &mtime, &rtime );
	if( ret ){
		DbgOut( "E3DGetMidiMusicTime : hs GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltime, aptrtime, TYPE_INUM, &mtime );
	//code_setva( pvalrtime, aptrrtime, TYPE_DNUM, &rtime );
***/
	return 0;
}


int E3DGetReferenceTime()
{

// 1 : soundid
	int soundid;
	soundid = code_getdi(0);

// 2 : mtimeptr
	PVal *pvaltime;
	APTR aptrtime;
	aptrtime = code_getva( &pvaltime );

// 3 : reftimeptr
	PVal *pvalrtime;
	APTR aptrrtime;
	aptrrtime = code_getva( &pvalrtime );

//////////
/***
	if( !s_HS ){
		DbgOut( "E3DGetReferenceTime : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int mtime;
	double rtime;
	ret = s_HS->GetMusicTime( soundid, &mtime, &rtime );
	if( ret ){
		DbgOut( "E3DGetReferenceTime : hs GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltime, aptrtime, TYPE_INUM, &mtime );
	code_setva( pvalrtime, aptrrtime, TYPE_DNUM, &rtime );
***/
	return 0;
}

int E3DSetNextMotionFrameNo()
{
	int hsid;
	hsid = code_getdi(0);

	int mk;
	mk = code_getdi(0);

	int nextmk;
	nextmk = code_getdi(0);

	int nextframeno;
	nextframeno = code_getdi(0);

	int befframeno;
	befframeno = code_getdi( -1 );

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextMotionFrameNo( mk, nextmk, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextMotionFrameNo : hs SetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DSetLinearFogParams()
{

// 1 : enable
	int enable;
	enable = code_getdi(0);

// 2 : r
	int r;
	r = code_getdi(255);

// 3 : g
	int g;
	g = code_getdi(255);

// 4 : b
	int b;
	b = code_getdi(255);

// 5 : start
	float start;
	start = (float)code_getdd(0.0);

// 6 : end
	float end;
	end = (float)code_getdd(60000.0);

// 7 : srchsid
	int srchsid;
	srchsid = code_getdi( -1 );

//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* seths = 0;
	if( srchsid >= 0 ){
		seths = GetHandlerSet( srchsid );
		if( !seths ){
			DbgOut( "E3DSetLinearFogParams : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	r = max( 0, r );
	r = min( 255, r );

	g = max( 0, g );
	g = min( 255, g );

	b = max( 0, b );
	b = min( 255, b );

	
	if( seths == 0 ){
		if( enable ){
			g_fogenable = 1;
		}else{
			g_fogenable = 0;
		}
		g_fogcolor = D3DCOLOR_ARGB( 0, r, g, b );
		g_fogstart = start;
		g_fogend = end;
		g_fogtype = 0.0f;
	
		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pdev->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pdev->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pdev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pdev->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );

		int hsidno;
		for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
			if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
				CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
				ret = curhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
				_ASSERT( !ret );
			}
		}

		if( g_bbhs ){
			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );

		}

	}else{

		int tmpfogenable;
		D3DCOLOR tmpfogcolor;
		float tmpfogstart, tmpfogend;

		if( enable ){
			tmpfogenable = 1;
		}else{
			tmpfogenable = 0;
		}
		tmpfogcolor = D3DCOLOR_ARGB( 0, r, g, b );
		tmpfogstart = start;
		tmpfogend = end;
		g_fogtype = 0.0f;

		ret = seths->SetRenderState( -1, D3DRS_FOGENABLE, tmpfogenable );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGCOLOR, tmpfogcolor );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&tmpfogstart)) );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&tmpfogend)) );
		_ASSERT( !ret );

	}

	return 0;
}

int E3DSetToon0Params()
{
//1 : hsid
	int hsid;
	hsid = code_getdi( 0 );

//2 : darkrate
	float darkrate;
	darkrate = (float)code_getdd( 0.7 );

//3 : brightrate
	float brightrate;
	brightrate = (float)code_getdd( 1.3 );

//4 : darkdeg
	float darkdeg;
	darkdeg = (float)code_getdd( 66.4 );

//5 : brightdeg
	float brightdeg;
	brightdeg = (float)code_getdd( 25.8 );

// 6 : partno
	int partno;
	partno = code_getdi( -1 );

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float darkcos, brightcos;
	darkcos = cosf( darkdeg * (float)DEG2PAI );
	if( darkcos < 0.0f )
		darkcos = 0.0f;

	brightcos = cosf( brightdeg * (float)DEG2PAI );
	if( brightcos < 0.0f )
		brightcos = 0.0f;


	int ret;
	ret = curhs->SetToon0Params( partno, darkrate, brightrate, darkcos, brightcos );
	if( ret ){
		DbgOut( "E3DSetToon0Params : hs SetToon0Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int DisableFogState()
{

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	s_savefogenable = g_fogenable;

	if( g_fogenable != 0 ){
		g_renderstate[D3DRS_FOGENABLE] = 0;
		pdev->SetRenderState( D3DRS_FOGENABLE, 0 );
	}
	return 0;
}
int RollbackFogState()
{
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	if( s_savefogenable != g_fogenable ){
		g_fogenable = s_savefogenable;

		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

	}
	return 0;
}

int E3DChkConflict3()
{
	// 1 : hsid1
	// 2 : partno1
	// 3 : partnum1
	// 4 : hsid2
	// 5 : partno2
	// 6 : partnum2
	// 7 : confflag
	// 8 : inviewflag

	int srchsid;
	srchsid = code_getdi(0);

	PVal *pvalsrcpart;
	APTR aptrsrcpart;
	aptrsrcpart = code_getva( &pvalsrcpart );

	int srcpartnum;
	srcpartnum = code_getdi(1);

	int chkhsid;
	chkhsid = code_getdi(0);

	PVal *pvalchkpart;
	APTR aptrchkpart;
	aptrchkpart = code_getva( &pvalchkpart );
	
	int chkpartnum;
	chkpartnum = code_getdi(1);

	PVal *pvalconf;
	APTR aptrconf;
	aptrconf = code_getva( &pvalconf );

	PVal *pvalinview;
	APTR aptrinview;
	aptrinview = code_getva( &pvalinview );

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict3( (int*)(pvalsrcpart->pt + aptrsrcpart), srcpartnum, 
		chkhs, (int*)(pvalchkpart->pt + aptrchkpart), chkpartnum,
		fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflict2 : srchs ChkConflict3 error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	code_setva( pvalconf, aptrconf, TYPE_INUM, &conf );
	code_setva( pvalinview, aptrinview, TYPE_INUM, &inviewflag );

	return 0;
}
int E3DChkConfBySphere3()
{
	// 1 : src handlerset id
	// 2 : srcpartno
	// 3 : srcpartnum
	// 4 : chk handlerset id
	// 5 : chkpartno
	// 6 : chkpartnum
	// 7 : confflag

	
	int srchsid;
	srchsid = code_getdi(0);

	PVal *pvalsrcpart;
	APTR aptrsrcpart;
	aptrsrcpart = code_getva( &pvalsrcpart );

	int srcpartnum;
	srcpartnum = code_getdi(1);

	int chkhsid;
	chkhsid = code_getdi(0);

	PVal *pvalchkpart;
	APTR aptrchkpart;
	aptrchkpart = code_getva( &pvalchkpart );

	int chkpartnum;
	chkpartnum = code_getdi(1);

	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );



////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere3( (int*)(pvalsrcpart->pt + aptrsrcpart), srcpartnum, 
		chkhs, (int*)(pvalchkpart->pt + aptrchkpart), chkpartnum, &confflag );
	if( ret ){
		DbgOut( "E3DChkConfBySphere : srchs ChkConfBySphere3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &confflag );

	return 0;
}

int E3DSetMovableAreaThread()
{
	int ret;
// 1: bmpname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2: maxx
	float maxx;
	maxx = (float)code_getdd(0.0);

// 3: maxz
	float maxz;
	maxz = (float)code_getdd(0.0);

// 4: divx
	int divx;
	divx = code_getdi(0);

// 5: divz
	int divz;
	divz = code_getdi(0);

// 6: wallheight
	float wallheight;
	wallheight = (float)code_getdd(0.0);

// 7: threadid
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

/////////
/////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSetMovableAreaThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].maxx = maxx;
	s_thtable[thno].maxz = maxz;
	s_thtable[thno].divx = divx;
	s_thtable[thno].divz = divz;
	s_thtable[thno].maxheight = wallheight;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSetMovableArea, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSetMovableAreaThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );

	return 0;
}


DWORD WINAPI ThreadSetMovableArea( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = SetMovableArea_F( curtable->filename, curtable->maxx, curtable->maxz, curtable->divx, curtable->divz, curtable->maxheight, &temphsid );
	if( ret ){
		DbgOut( "ThreadSetMovableArea : SetMovableArea_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DEncodeBeta()
{



	// 1: filename
	// 2: xor10

	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DEncodeBeta : file name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	PVal *pvalxor;
	APTR aptrxor;
	aptrxor = code_getva( &pvalxor );
	
	int* xorptr;
	xorptr = (int*)pvalxor->pt + aptrxor;

//////////////
	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)tempname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "E3DEncodeBeta : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "E3DEncodeBeta :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "E3DEncodeBeta :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* newbuf;
	newbuf = (unsigned char*)malloc( sizeof( char ) * bufleng + 1 );//bufleng + 1
	if( !newbuf ){
		DbgOut( "E3DEncodeBeta : newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "E3DEncodeBeta : ReadFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}
	
	//////////////

	ret = XORFuncBeta( newbuf, bufleng, xorptr );
	if( ret ){
		DbgOut( "E3DEncodeBeta : XORFuncBeta error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	//////////////

	SetFilePointer( hfile, 0, NULL, FILE_BEGIN );
	if( GetLastError() != NO_ERROR ){
		DbgOut( "E3DEncodeBeta : SetFilePointer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	unsigned long writeleng;
	WriteFile( hfile, newbuf, bufleng, &writeleng, NULL );
	if( bufleng != writeleng ){
		DbgOut( "E3DEncodeBeta : WriteFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	FlushFileBuffers( hfile );
	SetEndOfFile( hfile );
	CloseHandle( hfile );	
	free( newbuf );

	return 0;
}

int XORFuncBeta( unsigned char* newbuf, int bufleng, int* xorptr )
{

	int xorindex = 0;
	int curpos;
	for( curpos = 0; curpos < bufleng; curpos++ ){
		unsigned char curuc;
		curuc = *(newbuf + curpos);
		curuc = curuc ^ (unsigned char)(*( xorptr + xorindex ));
		*(newbuf + curpos) = curuc;

		xorindex++;
		if( xorindex >= 10 ){
			xorindex = 0;
		}
	}
	return 0;
}

int E3DDecodeBeta()
{

	//1 : filename
	//2 : xor10
	//3 : dstbuffer

	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DDecodeBeta : file name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	PVal *pvalxor;
	APTR aptrxor;
	aptrxor = code_getva( &pvalxor );
	
	int* xorptr;
	xorptr = (int*)pvalxor->pt + aptrxor;



	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );
	
	char* bufptr;
	if( pvalbuf->offset == 0 ){
		bufptr = (char*)pvalbuf->pt;
	}else{
		bufptr = *((char**)pvalbuf->master + pvalbuf->offset);
	}

	///////////////////

	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)tempname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "E3DDecodeBeta : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "E3DDecodeBeta :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "E3DDecodeBeta :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* newbuf;
	newbuf = (unsigned char*)malloc( sizeof( char ) * bufleng + 1 );//bufleng + 1
	if( !newbuf ){
		DbgOut( "E3DDecodeBeta : newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "E3DDecodeBeta : ReadFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}
	
	//////////////

	ret = XORFuncBeta( newbuf, bufleng, xorptr );
	if( ret ){
		DbgOut( "E3DDecodeBeta : XORFuncBeta error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	/////////////

	memcpy( bufptr, newbuf, bufleng );

	/////////////

	CloseHandle( hfile );	
	free( newbuf );


	return 0;
}

int E3DChkVertexShader()
{

	PVal *pvalVS;
	APTR aptrVS;
	aptrVS = code_getva( &pvalVS );

	code_setva( pvalVS, aptrVS, TYPE_INUM, &g_chkVS );

	return 0;
}

int E3DLoadMOAFile()
{
	int ret;

// 1: hsid
	int hsid;
	hsid = code_getdi(0);

// 2: mafname
	char mafname[MAX_PATH];
	ret = GetName( mafname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadMAFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 3: fillupleng
	int fillupleng;
	fillupleng = code_getdi( 10 );

	if( fillupleng <= 0 ){
		fillupleng = 1;
	}else if( fillupleng >= 60 ){
		fillupleng = 60;
	}

// 4
	float mvmult;
	mvmult = (float)code_getdd(1.0);

//////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadMAFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = curhs->LoadMAFile( mafname, fillupleng, s_hwnd, s_hMod, mvmult );
	if( ret ){
		DbgOut( "E3DLoadMAFile : hs LoadMAFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetNewPoseByMOA()
{
	int ret;

// 1: hsid
	int hsid;
	hsid = code_getdi(0);

// 2: eventno
	int eventno;
	eventno = code_getdi(0);

/////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPoseByMOA : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNewPoseByMOA( eventno );
	if( ret ){
		DbgOut( "E3DSetnewPoseByMOA : hs SetNewPoseByMOA error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMotionFrameNo()
{
	//1 : handlerset id
	//2 : motkindptr
	//3 : motframeptr
	int hsid;
	hsid = code_getdi(0);


	PVal *pvalmotkind;
	APTR aptrmotkind;
	aptrmotkind = code_getva( &pvalmotkind );

	PVal *pvalmotframe;
	APTR aptrmotframe;
	aptrmotframe = code_getva( &pvalmotframe );

///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind, dstmotframe;
	ret = curhs->GetMotionFrameNo( &dstmotkind, &dstmotframe );
	if( ret ){
		DbgOut( "E3DGetMotionFrameNo : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalmotkind, aptrmotkind, TYPE_INUM, &dstmotkind );
	code_setva( pvalmotframe, aptrmotframe, TYPE_INUM, &dstmotframe );

	return 0;
}


//E3DEnableToonEdge hsid, partno, type, flag
int E3DEnableToonEdge()
{
	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	//3
	int type;
	type = code_getdi(0);

	//4
	int flag;
	flag = code_getdi(0);

/////////
	int ret;

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DEnableToonEdge : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->EnableToonEdge( partno, type, flag );
	if( ret ){
		DbgOut( "E3DEnableToonEdge : hs EnableToonEdge error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetToonEdge0Color hsid, partno, materialname, r, g, b
int E3DSetToonEdge0Color()
{

	int ret;

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	// 3 : materialname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Color : material name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//4
	int r;
	r = code_getdi(0);

	//5
	int g;
	g = code_getdi(0);

	//6
	int b;
	b = code_getdi(0);

/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonEdge0Color : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToonEdge0Color( partno, tempname, r, g, b );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Color : hs SetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DSetToonEdge0Width hsid, partno, materialname, width
int E3DSetToonEdge0Width()
{
	int ret;

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	// 3 : materialname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Width : material name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//4
	float width;
	width = (float)code_getdd(8.0);

/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonEdge0Width : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToonEdge0Width( partno, tempname, width );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Width : hs SetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetToonEdge0Color()
{
	int ret;

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	// 3 : materialname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Color : material name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//4
	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	//5
	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	//6
	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );

/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToonEdge0Color : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fr = 0.0f;
	float fg = 0.0f;
	float fb = 0.0f;
	ret = curhs->GetToonEdge0Color( partno, tempname, &fr, &fg, &fb );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Color : hs GetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ir, ig, ib;
	ir = (int)( fr * 255.0f );
	ig = (int)( fg * 255.0f );
	ib = (int)( fb * 255.0f );

	code_setva( pvalr, aptrr, TYPE_INUM, &ir );
	code_setva( pvalg, aptrg, TYPE_INUM, &ig );
	code_setva( pvalb, aptrb, TYPE_INUM, &ib );


	return 0;
}
int E3DGetToonEdge0Width()
{
	int ret;

	// 1 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 2 : partno
	int partno;
	partno = code_getdi( -1 );//!!!!

	// 3 : materialname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Width : material name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//4
	PVal *pvalw;
	APTR aptrw;
	aptrw = code_getva( &pvalw );

/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToonEdge0Width : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float dstw = 0.0f;
	ret = curhs->GetToonEdge0Width( partno, tempname, &dstw );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Width : hs GetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dwidth;
	dwidth = (double)dstw;

	code_setva( pvalw, aptrw, TYPE_DNUM, &dwidth );


	return 0;
}

//E3DCreateParticle maxnum, texname, width, height, particleid
int E3DCreateParticle()
{
	int ret;

	// 1 : maxnum
	int maxnum;
	maxnum = code_getdi(100);

	// 2 : materialname
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateParticle : texture name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// 3 : width
	float width;
	width = (float)code_getdd(32.0);

	// 4 : height
	float height;
	height = (float)code_getdd(32.0);

	// 5 : blendmode
	int blendmode;
	blendmode = code_getdi(2);


	//6
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	//7
	int transparentflag;
	transparentflag = code_getdi(1);

	//8
	int cmpalways;
	cmpalways = code_getdi(0);

///////////////////////

	if( maxnum > MAX_BILLBOARD_NUM ){
		DbgOut( "E3DCreateParticle : maxnum too big error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DCreateParticle : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	newhs->m_billboardflag = 2;// !!!!!

	int particleflag = 1;
	ret = newhs->CreateBillboard( particleflag, cmpalways );
	if( ret ){
		DbgOut( "E3DCreateParticle : newhs CreateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////////

	//SetResDir( tempname, newhs );//!!!!!!
	//int transparentflag = 1;
	int dirmode = 1;
	int orgflag = 1;

	int bbno;
	for( bbno = 0; bbno < maxnum; bbno++ ){
		int bbid = -1;
		ret = newhs->AddBillboard( tempname, width, height, transparentflag, dirmode, orgflag, &bbid );
		if( ret || (bbid < 0) ){
			DbgOut( "E3DCreateParticle : newhs AddBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bbid >= 0 ){
			//Create 3D Object
			ret = Restore( newhs, bbid );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = newhs->SetValidFlag( bbid, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}


	ret = newhs->SetBlendingMode( -1, blendmode );
	if( ret ){
		DbgOut( "e3dhsp : E3DCreateParitcle : hs SetBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


///////////////

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &newhs->serialno );

	return 0;
}

//E3DDestroyParticle particleid
int E3DDestroyParticle()
{

	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

///////

	int ret;
	ret = DestroyHandlerSet( ptclid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyParticle : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetParticlePos particleid, posx, posy, posz
int E3DSetParticlePos()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : posx
	float posx;
	posx = (float)code_getdd(0.0);

	// 3 : posy
	float posy;
	posy = (float)code_getdd(0.0);

	// 4 : posz
	float posz;
	posz = (float)code_getdd(0.0);

//////////////////

	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticlePos : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 pos;
	pos.x = posx;
	pos.y = posy;
	pos.z = posz;
	ret = curhs->SetParticlePos( pos );
	if( ret ){
		DbgOut( "E3DSetParticlePos : hs SetParticlePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetParticleGravity particleid, gravity;
int E3DSetParticleGravity()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : gravity
	float gravity;
	gravity = (float)code_getdd(0.0);

//////////////////

	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleGravity : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleGravity( gravity );
	if( ret ){
		DbgOut( "E3DSetParticleGravity : hs SetParticleGravity error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetParticleLife particleid, life
int E3DSetParticleLife()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : life
	float life;
	life = (float)code_getdd(0.0);

//////////////////

	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleLife : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleLife( life );
	if( ret ){
		DbgOut( "E3DSetParticleLife : hs SetParticleLife error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetParticleEmitNum particleid, emitnum
int E3DSetParticleEmitNum()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : emitnum
	float emitnum;
	emitnum = (float)code_getdd(0.0);

//////////////////

	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleEmitNum : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleEmitNum( emitnum );
	if( ret ){
		DbgOut( "E3DSetParticleEmitNum : hs SetParticleEmitNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetParticleVel0 particleid, minvelx, minvely, minvelz, maxveclx, maxvely, maxvelz
int E3DSetParticleVel0()
{

	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : minvelx
	float minvelx;
	minvelx = (float)code_getdd(0.0);

	// 3 : minvely
	float minvely;
	minvely = (float)code_getdd(0.0);

	// 4 : minvelz
	float minvelz;
	minvelz = (float)code_getdd(0.0);

	// 5 : maxvelx
	float maxvelx;
	maxvelx = (float)code_getdd(0.0);

	// 6 : maxvely
	float maxvely;
	maxvely = (float)code_getdd(0.0);

	// 7 : maxvelz
	float maxvelz;
	maxvelz = (float)code_getdd(0.0);

	//////////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleVel0 : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 minvel, maxvel;
	minvel.x = minvelx;
	minvel.y = minvely;
	minvel.z = minvelz;

	maxvel.x = maxvelx;
	maxvel.y = maxvely;
	maxvel.z = maxvelz;

	ret = curhs->SetParticleVel0( minvel, maxvel );
	if( ret ){
		DbgOut( "E3DSetParticleVel0 : hs SetParticleVel0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetParticleRotation particleid, rotation
int E3DSetParticleRotation()
{

	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : rotation
	float rotation;
	rotation = (float)code_getdd(0.0);

	//////////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleRotation : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int rotkind = 0;
	ret = curhs->RotateBillboard( -1, rotation, rotkind );
	if( ret ){
		DbgOut( "E3DSetParticleRotation : bbhs RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetParticleDiffuse particleid, r, g, b
int E3DSetParticleDiffuse()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : r
	int r;
	r = code_getdi(0);

	// 3 : g
	int g;
	g = code_getdi(0);

	// 4 : b
	int b;
	b = code_getdi(0);

////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleDiffuse : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;


	int setflag = 0;
	ret = curhs->SetDispDiffuse( -1, r, g, b, setflag, -1, 1 );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DRenderParticle particleid, fps, scid
int E3DRenderParticle()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	// 2 : fps
	int fps;
	fps = code_getdi( 60 );

	// 3 : scid
	int scid;
	scid = code_getdi( 0 );
	
	//4 : onlyupdate
	int onlyupdate;
	onlyupdate = code_getdi(0);

//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderParticle : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DRenderParticle : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
		
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	CalcMatView();


	ret = curhs->ChkInView( s_matView, 0, 0 );
	if( ret ){
		DbgOut( "E3DRenderParticle : ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
//Transform

	ret = curhs->UpdateParticle( s_matView, &s_camerapos, &s_cameratarget, fps );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//Render
	if( onlyupdate == 0 ){
		ret = curhs->Render( 0, pdev, 1, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

//E3DSetParticleAlpha particleid, mintime, maxtime, alpha
int E3DSetParticleAlpha()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	float mintime;
	mintime = (float)code_getdd(0.0);

	float maxtime;
	maxtime = (float)code_getdd(0.0);

	float alpha;
	alpha = (float)code_getdd(0.0);


	//////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleAlpha : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleAlpha( mintime, maxtime, alpha );
	if( ret ){
		DbgOut( "E3DSetParticleAlpha : hs SetParticleAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetParticleUVTile particleid, mintime, maxtime, unum, vnum, tileno
int E3DSetParticleUVTile()
{
	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	float mintime;
	mintime = (float)code_getdd(0.0);

	float maxtime;
	maxtime = (float)code_getdd(0.0);

	int unum;
	unum = code_getdi(1);

	int vnum;
	vnum = code_getdi(1);

	int tileno;
	tileno = code_getdi(0);


	//////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DSetParticleUVTile : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleUVTile( mintime, maxtime, unum, vnum, tileno );
	if( ret ){
		DbgOut( "E3DSetParticleUVTile : hs SetParticleUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DInitParticle()
{

	// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);

	//////////////
	CHandlerSet* curhs = GetHandlerSet( ptclid );
	if( !curhs ){
		DbgOut( "E3DInitParticle : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitParticle();
	if( ret ){
		DbgOut( "E3DInitParticle : hs InitParticle error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DClone()
{



	return 0;
}

int E3DGetMotionIDByName()
{
	
	int ret;
//1
	int hsid;
	hsid = code_getdi(0);

//2
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetMotionIDByName : motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//3

	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );


//////////



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionIDByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int motid = -1;
	ret = curhs->GetMotionIDByName( tempname, &motid );
	if( ret ){
		DbgOut( "E3DGetMotinIDByName : hs GetMotionIDByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &motid );

	return 0;
}

int E3DGetMotionNum()
{

	int ret;
//1
	int hsid;
	hsid = code_getdi(0);
//2

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int motnum =0;
	ret = curhs->GetMotionNum( &motnum );
	if( ret ){
		DbgOut( "E3DGetMotionNum : hs GetMotionNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &motnum );


	return 0;
}

int E3DChkConfParticle()
{
// 1 : particle id
	int ptclid;
	ptclid = code_getdi(0);
// 2
	int hsid;
	hsid = code_getdi(0);

// 3
	float rate;
	rate = (float)code_getdd( 1.0 );

//4
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

///////////

	CHandlerSet* curptcl = GetHandlerSet( ptclid );
	if( !curptcl ){
		DbgOut( "E3DChkConfParticle : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfParticle : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int flag = 0;
	ret = curptcl->ChkConfParticle( curhs, rate, &flag );
	if( ret ){
		DbgOut( "E3DChkConfParticle : ptcl ChkConfParticle error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &flag );

	return 0;
}

int E3DGetCenterPos()
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(-1);

	PVal *pvalposx;
	APTR aptrposx;
	aptrposx = code_getva( &pvalposx );

	PVal *pvalposy;
	APTR aptrposy;
	aptrposy = code_getva( &pvalposy );

	PVal *pvalposz;
	APTR aptrposz;
	aptrposz = code_getva( &pvalposz );
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCenterPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ret = curhs->GetCenterPos( partno, &pos );
	if( ret ){
		DbgOut( "E3DGetCenterPos : hs GetCenterPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dposx, dposy, dposz;
	dposx = (double)pos.x;
	dposy = (double)pos.y;
	dposz = (double)pos.z;

	code_setva( pvalposx, aptrposx, TYPE_DNUM, &dposx );
	code_setva( pvalposy, aptrposy, TYPE_DNUM, &dposy );
	code_setva( pvalposz, aptrposz, TYPE_DNUM, &dposz );

	return 0;
}

int E3DGetFaceNum()//$184
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(-1);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFaceNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int facenum = 0;
	ret = curhs->GetFaceNum( partno, &facenum );
	if( ret ){
		DbgOut( "E3DGetFaceNum : hs GetFaceNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &facenum );

	return 0;
}

int E3DGetFaceNormal()//$185
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(-1);

	int faceno;
	faceno = code_getdi(0);

	PVal *pvalposx;
	APTR aptrposx;
	aptrposx = code_getva( &pvalposx );

	PVal *pvalposy;
	APTR aptrposy;
	aptrposy = code_getva( &pvalposy );

	PVal *pvalposz;
	APTR aptrposz;
	aptrposz = code_getva( &pvalposz );
	
///////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFaceNormal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 n( 0.0f, 0.0f, 0.0f );
	ret = curhs->GetFaceNormal( partno, faceno, &n );
	if( ret ){
		DbgOut( "E3DGetFaceNormal : hs GetFaceNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dnx, dny, dnz;
	dnx = (double)n.x;
	dny = (double)n.y;
	dnz = (double)n.z;

	code_setva( pvalposx, aptrposx, TYPE_DNUM, &dnx );
	code_setva( pvalposy, aptrposy, TYPE_DNUM, &dny );
	code_setva( pvalposz, aptrposz, TYPE_DNUM, &dnz );

	return 0;
}

int E3DCreateEmptyMotion()
{
	
	int ret;
	int hsid;
	int frameleng;

	hsid = code_getdi(0);

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateEmptyMotion : motion name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	frameleng = code_getdi(60);

	//
	PVal *pvalcookie;
	APTR aptrcookie;
	aptrcookie = code_getva( &pvalcookie );


/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateEmptyMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

EnterCriticalSection( &(curhs->m_crit_addmotion) );//######## start crit
	
	int tempmotid = -1;
	ret = curhs->CreateEmptyMotion( tempname, frameleng, &tempmotid );
	if( ret ){
		DbgOut( "E3DCreateEmptyMotion : hs CreateEmptyMotion error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit
		return 1;
	}

LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit


	code_setva( pvalcookie, aptrcookie, TYPE_INUM, &tempmotid );

	return 0;
}

int E3DSetTextureMinMagFilter()
{
	int hsid = code_getdi(0);
	int partno = code_getdi(0);
	int minfilter = code_getdi( D3DTEXF_LINEAR );
	int magfilter = code_getdi( D3DTEXF_LINEAR );
	int scid = code_getdi(0);


//////////////
	CHandlerSet* curhs = 0;
	int bgflag = 0;

	if( hsid > 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == 0 ){
		bgflag = 1;
	}


	int ret;
	if( curhs ){

		ret = curhs->SetTextureMinMagFilter( partno, minfilter, magfilter );
		if( ret ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : curhs SetTextureMinMagFilter error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( bgflag == 0 ){
		
		CMySprite* cursprite;
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DSetTextureMinMagFilter : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		cursprite->m_minfilter = minfilter;
		cursprite->m_magfilter = magfilter;


	}else if( bgflag == 1 ){
		BGDISPELEM* bgelem;
		bgelem = GetBGDispElem( scid );
		if( !bgelem ){
			DbgOut( "E3DSetTextureMinMagFilter : scid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		bgelem->bgdisp->m_minfilter = minfilter;
		bgelem->bgdisp->m_magfilter = magfilter;

	}
	return 0;
}

int E3DGetMotionName()
{
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	PVal *pvalstr;
	APTR aptrstr;
	aptrstr = code_getva( &pvalstr );


//////////////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMotionName( pvalstr, aptrstr, motid );
	if( ret ){
		DbgOut( "E3DGetMotionName : hs GetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMotionName()
{

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetMotionName : motion name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionName( motid, tempname );
	if( ret ){
		DbgOut( "E3DSetMotionName : hs SetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMaterialNoByName()//$18A
{
	//E3DGetMaterialNoByName hsid, name, matno

	int ret;
	int hsid;
	hsid = code_getdi(0);

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DGetMaterialNoByName: material name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno = -3;
	ret = curhs->GetMaterialNoByName( tempname, &matno );
	if( ret ){
		DbgOut( "E3DGetMaterialNoByName : hs GetMaterialNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &matno );

	return 0;
}
int E3DGetMaterialAlpha()//$18B
{
	//E3DGetMaterialAlpha	hsid, matno, alpha

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalalpha;
	APTR aptralpha;
	aptralpha = code_getva( &pvalalpha );

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float falpha = 1.0f;
	ret = curhs->GetMaterialAlpha( matno, &falpha );
	if( ret ){
		DbgOut( "E3DGetMaterialAlpha : hs GetMaterialAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dalpha = falpha;
	code_setva( pvalalpha, aptralpha, TYPE_DNUM, &dalpha );


	return 0;
}
int E3DGetMaterialDiffuse()//$18C
{
	//E3DGetMaterialDiffuse hsid, matno, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialDiffuse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialDiffuse( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialDiffuse : hs GetMaterialDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );


	return 0;
}
int E3DGetMaterialSpecular()//$18D
{
	//E3DGetMaterialSpecular hsid, matno, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialSpecular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialSpecular( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialSpecular : hs GetMaterialSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );

	return 0;
}
int E3DGetMaterialAmbient()//$18E
{
	//E3DGetMaterialAmbient hsid, matno, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAmbient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialAmbient( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialAmbient : hs GetMaterialAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );


	return 0;
}
int E3DGetMaterialEmissive()//$18F
{
	//E3DGetMaterialEmissive hsid, matno, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalr;
	APTR aptrr;
	aptrr = code_getva( &pvalr );

	PVal *pvalg;
	APTR aptrg;
	aptrg = code_getva( &pvalg );

	PVal *pvalb;
	APTR aptrb;
	aptrb = code_getva( &pvalb );


///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialEmissive : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialEmissive( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialEmissive : hs GetMaterialEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalr, aptrr, TYPE_INUM, &r );
	code_setva( pvalg, aptrg, TYPE_INUM, &g );
	code_setva( pvalb, aptrb, TYPE_INUM, &b );


	return 0;
}
int E3DGetMaterialPower()//$197
{
	//E3DGetMaterialPower hsid, matno, power


	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalpow;
	APTR aptrpow;
	aptrpow = code_getva( &pvalpow );

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialPower : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fpow;
	ret = curhs->GetMaterialPower( matno, &fpow );
	if( ret ){
		DbgOut( "E3DGetMaterialPower : hs GetMaterialPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dpow = fpow;
	code_setva( pvalpow, aptrpow, TYPE_DNUM, &dpow );

	return 0;
}


int E3DSetMaterialAlpha()//$190
{
	//E3DSetMaterialAlpha	hsid, matno, alpha

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	float alpha;
	alpha = (float)code_getdd(0.0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialAlpha( matno, alpha );
	if( ret ){
		DbgOut( "E3DSetMaterialAlpha : hs SetMaterialAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMaterialDiffuse()//$191
{
	//E3DSetMaterialDiffuse hsid, matno, setflag, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int setflag;
	setflag = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialDiffuse( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "E3DSetMaterialDiffuse : hs SetMaterialDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMaterialSpecular()//$192
{
	//E3DSetMaterialSpecular hsid, matno, setflag, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int setflag;
	setflag = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialSpecular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialSpecular( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "E3DSetMaterialSpecular : hs SetMaterialSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMaterialAmbient()//$193
{
	//E3DSetMaterialAmbient hsid, matno, setflag, r, g, b

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int setflag;
	setflag = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAmbient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialAmbient( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "E3DSetMaterialAmbient : hs SetMaterialAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetMaterialEmissive()//$194
{
	//E3DSetMaterialEmissive hsid, matno, setflag, r, g, b	

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int setflag;
	setflag = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialEmissive : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialEmissive( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "E3DSetMaterialEmissive : hs SetMaterialEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetMaterialPower()//$198
{
	//E3DSetMaterialPower hsid, matno, setflag, power

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int setflag;
	setflag = code_getdi(0);

	float power;
	power = (float)code_getdd(0.0);

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialPower : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialPower( matno, setflag, power );
	if( ret ){
		DbgOut( "E3DSetMaterialPower : hs SetMaterialPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int E3DGetTextureFromMaterial()//$195
{
	//E3DGetTextureFromMaterial hsid, matno, texid

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetTextureFromMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texid = -1;
	ret = curhs->GetTextureFromMaterial( matno, &texid );
	if( ret ){
		DbgOut( "E3DGetTextureFromMaterial : hs GetTextureFromMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &texid );

	return 0;
}
int E3DSetTextureToMaterial()//$196
{
	//E3DSetTextureToMaterial hsid, matno, texid

//1 hsid
	int hsid;
	hsid = code_getdi( 0 );

//2 matno
	int matno;
	matno = code_getdi(0);

//3 texid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetTextureToMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToMaterial : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToMaterial : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetTextureToMaterial( matno, texid );
	if( ret ){
		DbgOut( "E3DSetTextureToMaterial : hs SetTextureToMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DGetMaterialBlendingMode()//$199
{
	//E3DGetMaterialBlendingMode hsid, matno, mode

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalmode;
	APTR aptrmode;
	aptrmode = code_getva( &pvalmode );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialBlendingMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mode = 0;
	ret = curhs->GetMaterialBlendingMode( matno, &mode );
	if( ret ){
		DbgOut( "E3DGetMaterialBlendingMode : hs GetMaterialBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalmode, aptrmode, TYPE_INUM, &mode );

	return 0;
}
int E3DSetMaterialBlendingMode()//$19A
{
	//E3DSetMaterialBlendingMode hsid, matno, mode

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int mode;
	mode = code_getdi(0);

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialBlendingMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialBlendingMode( matno, mode );
	if( ret ){
		DbgOut( "E3DSetMaterialBlendingMode : hs SetMaterialBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMaterialNo()//$19B
{
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	int faceno;
	faceno = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno = -1;
	ret = curhs->GetMaterialNo( partno, faceno, &matno );
	if( ret ){
		DbgOut( "E3DGetMaterialNo : hs GetMaterialNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &matno );

	return 0;
}

int E3DSetMotionKindML()//$19C
{
	//E3DSetMotionKindML hsid, motid, list, notlist

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnot;
	APTR aptrnot;
	aptrnot = code_getva( &pvalnot );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionKindML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionKindML( motid, (int*)( pvallist->pt ) + aptrlist, (int*)( pvalnot->pt ) + aptrnot );
	if( ret ){
		DbgOut( "E3DSetMotionKindML : hs SetMotionKindML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMotionFrameNoML()//$19D
{
	//E3DSetMotionFrameNoML hsid, motid, frameno, list, notlist
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnot;
	APTR aptrnot;
	aptrnot = code_getva( &pvalnot );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionFrameNoML( motid, frameno, (int*)( pvallist->pt ) + aptrlist, (int*)( pvalnot->pt ) + aptrnot );
	if( ret ){
		DbgOut( "E3DSetMotionFrameNoML : hs SetMotionFrameNoML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetNewPoseML()//$19E
{
	//E3DSetNewPoseML hsid

	int hsid;
	hsid = code_getdi(0);
///////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPoseML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewPoseML();
	if( ret ){
		DbgOut( "E3DSetNewPoseML : hs SetNewPoseML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMOAEventNoML()//$19F
{
	//E3DSetMOAEventNoML hsid, eventno, list, notlist

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int eventno;
	eventno = code_getdi(0);

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnot;
	APTR aptrnot;
	aptrnot = code_getva( &pvalnot );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOAEventNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNewPoseByMOAML( eventno, (int*)( pvallist->pt ) + aptrlist, (int*)( pvalnot->pt ) + aptrnot );
	if( ret ){
		DbgOut( "E3DSetMOAEventNoML : hs SetNewPoseByMOAML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetNextMotionFrameNoML()//$1A0
{
	//E3DSetNextMotionFrameNoML hsid, motid, nextmk, nextframe, befframe, list, notlist

	int ret;
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int nextmk;
	nextmk = code_getdi(0);

	int nextframe;
	nextframe = code_getdi(0);

	int befframe;
	befframe = code_getdi(0);

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnot;
	APTR aptrnot;
	aptrnot = code_getva( &pvalnot );

//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNextMotionFrameNoML( motid, nextmk, nextframe, befframe, (int*)( pvallist->pt ) + aptrlist, (int*)( pvalnot->pt ) + aptrnot );
	if( ret ){
		DbgOut( "E3DSetNextMotionFrameNoML : hs SetNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMotionFrameNoML()//$1A1
{
	//E3DGetMotionFrameNoML hsid, boneno, motid, frameno
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	PVal *pvalmotkind;
	APTR aptrmotkind;
	aptrmotkind = code_getva( &pvalmotkind );

	PVal *pvalmotframe;
	APTR aptrmotframe;
	aptrmotframe = code_getva( &pvalmotframe );

///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind, dstmotframe;
	ret = curhs->GetMotionFrameNoML( boneno, &dstmotkind, &dstmotframe );
	if( ret ){
		DbgOut( "E3DGetMotionFrameNoML : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalmotkind, aptrmotkind, TYPE_INUM, &dstmotkind );
	code_setva( pvalmotframe, aptrmotframe, TYPE_INUM, &dstmotframe );

	return 0;
}
int E3DGetNextMotionFrameNoML()//$1A2
{
	//E3DGetNextMotionFrameNoML hsid, boneno, curmotid, nextmotid, nextframeno

// 1
	int hsid;
	hsid = code_getdi(0);

//2
	int boneno;
	boneno = code_getdi(0);

//3
	int motid;
	motid = code_getdi(0);

//4
	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

//5
	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNextMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMotionFrameNoML( boneno, motid, (int*)pvalid->pt + aptrid, (int*)pvalframe->pt + aptrframe );
	if( ret ){
		DbgOut( "E3DGetNextMotionFrameNoML : curhs GetNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DGetCurrentBonePos()//$1A3
{
	//E3DGetCurrentBonePos hsid, boneno, poskind, posx, posy, posz

	// 1 : handlerset id
	// 2 : boneno
	// 3 : poskind
	// 4 : posxptr
	// 5 : posyptr
	// 6 : poszptr
	
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int poskind;
	poskind = code_getdi(0);

	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );


///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetCurrentBonePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 dstpos;

	ret = curhs->GetCurrentBonePos( boneno, poskind, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetCurrentBonePos : curhs GetCurrentBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dx, dy, dz;
	dx = (double)dstpos.x;
	dy = (double)dstpos.y;
	dz = (double)dstpos.z;
	code_setva( pvalx, aptrx, TYPE_DNUM, &dx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dy );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dz );

	return 0;
}
int E3DGetCurrentBoneQ()//$1A4
{
	//E3DGetCurrentBoneQ hsid, boneno, kind, qid

	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

	int kind;
	kind = code_getdi(0);
	
	int qid;
	qid = code_getdi(0);

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCurrentBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DGetCurrentBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion dstq;
	ret = curhs->GetCurrentBoneQ( boneno, kind, &dstq );
	if( ret ){
		DbgOut( "E3DGetCurrentBoneQ : hs GetCurrentBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( qid, &setq );
	if( ret ){
		DbgOut( "E3DGetCurrentBoneQ : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DChkBumpMapEnable()//$1A5
{
	//E3DChkBumpMapEnable flag

	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );
/////////////


	code_setva( pvalflag, aptrflag, TYPE_INUM, &g_bumpEnable );

	return 0;
}

int E3DEnableBumpMap()//$1A6
{
	//E3DEnableBumpMap flag

	int flag;
	flag = code_getdi(0);

//////////////

	if( flag != 0 ){
		if( g_bumpEnable != 0 ){
			g_bumpUse = 1;
		}else{
			g_bumpUse = 0;
		}
	}else{
		g_bumpUse = 0;
	}

	return 0;
}

int E3DChkShadowEnable()// $1C2
{
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );
/////////////


	code_setva( pvalflag, aptrflag, TYPE_INUM, &g_shadowEnable );

	return 0;
}
int E3DEnableShadow()// $1C3
{
	int flag;
	flag = code_getdi(0);

//////////////

	if( flag != 0 ){
		if( g_shadowEnable != 0 ){
			g_shadowUse = 1;
		}else{
			g_shadowUse = 0;
		}
	}else{
		g_shadowUse = 0;
	}

	return 0;
}


int E3DConvHeight2NormalMap()//$1A7
{
	//E3DConvHeight2NormalMap hmapname, nmapname, hparam
	int ret;
	char hname[MAX_PATH];
	ret = GetName( hname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DConvHeight2NormalMap : hmapname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	char nname[MAX_PATH];
	ret = GetName( nname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DConvHeight2NormalMap : nmapname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float hparam;
	hparam = (float)code_getdd(1.0);

//////////////////////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DConvHeight2NormalMap : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DTEXTURE9 phmap = NULL;
	HRESULT hr;

	hr = D3DXCreateTextureFromFileEx( pdev, (LPTSTR)hname, 
							D3DX_DEFAULT, D3DX_DEFAULT, 
							1,//!!!!!!! 
							0, D3DFMT_A8R8G8B8, 
							D3DPOOL_MANAGED, 
							D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
							//D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
							g_mipfilter,//!!!!!!! 
							0, NULL, NULL, &phmap );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : height map texture create error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DSURFACE_DESC desc;
	hr = phmap->GetLevelDesc( 0, &desc );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : hmap GetLevelDesc error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		return 1;
	}


	LPDIRECT3DTEXTURE9 pnmap = NULL;
	hr = D3DXCreateTexture( pdev, desc.Width, desc.Height, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pnmap );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map texture create error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		return 1;
	}

	_ASSERT( phmap );
	_ASSERT( pnmap );

	hr = D3DXComputeNormalMap( pnmap, phmap, NULL, 0, D3DX_CHANNEL_RED, hparam );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map convert error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		pnmap->Release();
		return 1;
	}

	hr = D3DXSaveTextureToFile( (LPTSTR)nname,
			D3DXIFF_BMP,
			(LPDIRECT3DBASETEXTURE9)pnmap,
			NULL );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map write error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		pnmap->Release();
		return 1;
	}
		

	phmap->Release();
	pnmap->Release();

	return 0;
}
int E3DSetBumpMapToMaterial()//$1A8
{
	//E3DSetBumpMapToMaterial hsid, matno, texid

//1 hsid
	int hsid;
	hsid = code_getdi( 0 );

//2 matno
	int matno;
	matno = code_getdi(0);

//3 texid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetBumpMapToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBumpMapToMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	
	int texindex;
	int textype;
	if( texid >= 0 ){
		texindex = g_texbnk->GetIndexBySerial( texid );
		if( texindex < 0 ){
			DbgOut( "E3DSetBumpMapToMaterial : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		textype = g_texbnk->GetType( texindex );
		if( textype != TEXTYPE_TEXTURE ){
			DbgOut( "E3DSetBumpMapToMaterial : this is not texture %d error !!!\n", textype );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curhs->SetBumpMapToMaterial( matno, texid );
	if( ret ){
		DbgOut( "E3DSetBumpMapToMaterial : hs SetBumpMapToMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DDumpQ()//$1A9
{
	int qid;
	qid = code_getdi(0);

//////////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DDumpQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DDumpQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DDumpQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "dumpQ : qid %d, ( %1.12f %1.12f %1.12f %1.12f ) twist %f\r\n", qid, qptr->x, qptr->y, qptr->z, qptr->w, qptr->twist );

	return 0;
}

int E3DSetDbgQ()// $1AA
{
	int qid;
	qid = code_getdi(0);

//////////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DSetDbgQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DSetDbgQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DSetDbgQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	qptr->x = 0.000000f;
//	qptr->y = 0.706566f;
//	qptr->z = 0.000000f;
//	qptr->w = 0.707647f;
//
//	D3DXVECTOR3 axis;
//	float rad;
//	ret = qptr->GetAxisAndRot( &axis, &rad );
//	_ASSERT( !ret );
//	float deg;
//	deg = rad * (float)PAI2DEG;
//	DbgOut( "setdbgq : axis %f %f %f, rad %f, deg %f\r\n", axis.x, axis.y, axis.z, rad, deg );

	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
	qptr->SetAxisAndRot( axis, 90.0f * (float)DEG2PAI );


	return 0;
}

int E3DGetMOATrunkInfo()// $1AB
{
	//E3DGetMOATrunkInfo hsid, motid, trunkinfo
	
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );

////////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOATrunkInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOATrunkInfo( motid, (int*)( pvalinfo->pt ) + aptrinfo );
	if( ret ){
		DbgOut( "E3DGetMOATrunkInfo : hs GetMOATrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOABranchInfo()// $1AC
{
	//E3DGetMOABranchInfo hsid, motid, branchinfo, branchnum, getnum

	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );

	int branchnum;
	branchnum = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

///////////////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOABranchInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->GetMOABranchInfo( motid, (int*)( pvalinfo->pt ) + aptrinfo, branchnum, &getnum );
	if( ret ){
		DbgOut( "E3DGetMOABranchInfo : hs GetMOABranchInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}
int E3DSetMOABranchFrame1()// $1AD
{
	//E3DSetMOABranchFrame1 hsid, tmotid, bmotid, frame1
	
	int hsid;
	hsid = code_getdi(0);

	int tmotid;
	tmotid = code_getdi(0);

	int bmotid;
	bmotid = code_getdi(0);

	int frame1;
	frame1 = code_getdi(0);

//////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOABranchFrame1 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->SetMOABranchFrame1( tmotid, bmotid, frame1 );
	if( ret ){
		DbgOut( "E3DSetMOABranchFrame1 : hs SetMOABranchFrame1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMOABranchFrame2()// $1AE
{
	//E3DSetMOABranchFrame2 hsid, tmotid, bmotid, frame2

	int hsid;
	hsid = code_getdi(0);

	int tmotid;
	tmotid = code_getdi(0);

	int bmotid;
	bmotid = code_getdi(0);

	int frame2;
	frame2 = code_getdi(0);

//////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOABranchFrame2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->SetMOABranchFrame2( tmotid, bmotid, frame2 );
	if( ret ){
		DbgOut( "E3DSetMOABranchFrame2 : hs SetMOABranchFrame2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DQtoEuler()// $1B0
{
	//E3DQtoEuler qid, newx, newy, newz, oldx, oldy, oldz
	int qid;
	qid = code_getdi(0);

	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

	float oldx;
	oldx = (float)code_getdd(0);

	float oldy;
	oldy = (float)code_getdd(0);

	float oldz;
	oldz = (float)code_getdd(0);


/////////
	if( !s_qh ){
		DbgOut( "E3DQtoEuler : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DQtoEuler : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion tmpq;
	tmpq.x = srcqptr->x;
	tmpq.y = srcqptr->y;
	tmpq.z = srcqptr->z;
	tmpq.w = srcqptr->w;

	D3DXVECTOR3 neweul, befeul;
	befeul.x = oldx;
	befeul.y = oldy;
	befeul.z = oldz;

	ret = qToEuler( 0, &tmpq, &neweul );
	if( ret ){
		DbgOut( "E3DQtoEuler : qToEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = modifyEuler( &neweul, &befeul );
	_ASSERT( !ret );

	double newdx, newdy, newdz;
	newdx = (double)neweul.x;
	newdy = (double)neweul.y;
	newdz = (double)neweul.z;

	code_setva( pvalx, aptrx, TYPE_DNUM, &newdx );
	code_setva( pvaly, aptry, TYPE_DNUM, &newdy );
	code_setva( pvalz, aptrz, TYPE_DNUM, &newdz );

	return 0;
}
int E3DEnablePhongEdge()// $1B1
{
	//E3DEnablePhongEdge hsid, flag
	int hsid;
	hsid = code_getdi(0);

	int flag;
	flag = code_getdi(0);

//////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DEnablePhongEdge : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->EnablePhongEdge0( flag );
	if( ret ){
		DbgOut( "E3DEnablePhongEdge : hs EnablePhongEdge0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetPhongEdge0Params()// $1B2
{
	//E3DSetPhongEdge0Params hsid, r, g, b, width, blendmode, alpha
	int hsid;
	hsid = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

	float width;
	width = (float)code_getdd(0);

	int blendmode;
	blendmode = code_getdi(3);

	float alpha;
	alpha = (float)code_getdd(1.0);

///////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPhongEdge0Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetPhongEdge0Params( r, g, b, width, blendmode, alpha );
	if( ret ){
		DbgOut( "E3DSetPhongEdge0Params : hs SetPhongEdge0Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetDispSwitch2()// $1B3
{
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalsw;
	APTR aptrsw;
	aptrsw = code_getva( &pvalsw );

	int arleng;
	arleng = code_getdi(0);

////////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispSwitch2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDispSwitch2( (int*)( pvalsw->pt ) + aptrsw, arleng );
	if( ret ){
		DbgOut( "E3DGetDispSwitch2 : hs GetDispSwitch2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int InitThTableElem( int thno )
{
	if( (thno < 0) || (thno >= THMAX) ){
		DbgOut( "e3d : InitThTableElem : thno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	s_thtable[thno].threadid = -1;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].dwId = -1;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	s_thtable[thno].filename = 0;
	s_thtable[thno].filename2 = 0;
	s_thtable[thno].filename3 = 0;
	s_thtable[thno].filename4 = 0;
	s_thtable[thno].mult = 1.0f;
	s_thtable[thno].adjustuvflag = 0;

	s_thtable[thno].bufptr = 0;
	s_thtable[thno].bufsize = 0;

	s_thtable[thno].maxx = 0;
	s_thtable[thno].maxz = 0;
	s_thtable[thno].divx = 0;
	s_thtable[thno].divz = 0;
	s_thtable[thno].maxheight = 0;

	s_thtable[thno].hsid = -1;

	s_thtable[thno].useflag = 0;

	return 0;
}
int GetFreeThTable()
{
	int retth = -1;

	int thno;
	for( thno = 0; thno < THMAX; thno++ ){
		if( s_thtable[thno].useflag == 0 ){
			retth = thno;
			InitThTableElem( thno );//!!!!!!!!!!!!!!
			break;
		}
	}

	return retth;
}
int FreeThTableElem( int tableno )
{
	if( (tableno < 0) || (tableno >= THMAX) ){
		DbgOut( "e3d : FreeThTableElem : thno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_thtable[tableno].hThread ){
		WaitForSingleObject( s_thtable[tableno].hThread, 600000 );
		CloseHandle( s_thtable[tableno].hThread );
		s_thtable[tableno].hThread = NULL;
	}
	
	if( s_thtable[tableno].filename ){
		free( s_thtable[tableno].filename );
		s_thtable[tableno].filename = 0;
	}
	if( s_thtable[tableno].filename2 ){
		free( s_thtable[tableno].filename2 );
		s_thtable[tableno].filename2 = 0;
	}
	if( s_thtable[tableno].filename3 ){
		free( s_thtable[tableno].filename3 );
		s_thtable[tableno].filename3 = 0;
	}
	if( s_thtable[tableno].filename4 ){
		free( s_thtable[tableno].filename4 );
		s_thtable[tableno].filename4 = 0;
	}

	s_thtable[tableno].useflag = 0;//!!!!!!!!!!!!!!!

	return 0;
}

int E3DFreeThread()// $1B4
{
	int thno;
	thno = code_getdi(0);
///////////
	if( (thno < 0) || (thno >= THMAX) ){
		DbgOut( "E3DFreeThread : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = FreeThTableElem( thno );
	if( ret ){
		DbgOut( "E3DFreeThread : FreeThTableElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DLoadSigFileAsGroundThread()// $1B5
{
//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGroundThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// 2:
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

// 3:
	float mult;
	mult = (float)code_getdd(1.0);

//4	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

///////////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadSigFileAsGroundThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadSigFileAsGroundThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadSigFileAsGround, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadSigFileAsGroundThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );


	return 0;
}
DWORD WINAPI ThreadLoadSigFileAsGround( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = LoadSigFileAsGround_F( curtable->filename, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "ThreadLoadSigFileAsGround : LoadSigFileAsGround_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}




int E3DLoadSigFileAsGroundFromBufThread()// $1B6
{
//1
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : resdir name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4
	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//5
	float mult;
	mult = (float)code_getdd(1.0);

//6	
	PVal *pvalthread;
	APTR aptrthread;
	aptrthread = code_getva( &pvalthread );

///////////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[thno].filename, tempname );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].bufptr = (char*)pvalbuf->pt + aptrbuf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadSigFileAsGroundFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	//*threadidptr = s_thtable[thno].threadid;
	code_setva( pvalthread, aptrthread, TYPE_INUM, &(s_thtable[thno].threadid) );


	return 0;
}

DWORD WINAPI ThreadLoadSigFileAsGroundFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadSigFileAsGroundFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "ThreadLoadSigFileAsGroundFromBuf : LoadSigFileAsGroundFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

int E3DSetLightIdOfBumpMap()// $1B7
{
	int lid;
	lid = code_getdi(0);

//////////

	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightIdOfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[0] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;
}

int E3DSetLightId2OfBumpMap()// $1D8
{
	int lid;
	lid = code_getdi(0);

//////////

	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightId2OfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[1] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;
}
int E3DSetLightId3OfBumpMap()// $1D9
{
	int lid;
	lid = code_getdi(0);

//////////

	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightId3OfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[2] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;
}

int SetMatProj( SWAPCHAINELEM* cursc )
{
    D3DXMatrixIdentity( &g_matProj );
	RECT clirect;

	if( cursc->texid <= 0 ){
		GetClientRect( cursc->hwnd, &clirect );
	}else{
		clirect.top = 0;
		clirect.left = 0;
		clirect.bottom = cursc->texsize.y;
		clirect.right = cursc->texsize.x;
	}

	if( g_ortho == 0 ){
		DWORD dwClipWidth = (clirect.right - clirect.left) / 2;
		DWORD dwClipHeight = (clirect.bottom - clirect.top) / 2;
		float fov = (float)g_proj_fov * (float)DEG2PAI;
		float aspect;
		aspect = (float)dwClipWidth / (float)dwClipHeight;
		D3DXMatrixPerspectiveFovLH( &g_matProj, fov, aspect, g_proj_near, g_proj_far );
	}else{
		float vw, vh;
		vw = g_orthosize;
		vh = g_orthosize * ((float)(clirect.bottom - clirect.top) / (float)(clirect.right - clirect.left));
		D3DXMatrixOrthoLH( &g_matProj, vw, vh, g_proj_near, g_proj_far );		
	}


	g_scsize.x = clirect.right - clirect.left;
	g_scsize.y = clirect.bottom - clirect.top;

	return 0;
}



int SetShaderConst( int shadowflag )
{
	HRESULT hres;


	if( shadowflag == 0 ){

		hres = g_pEffect->SetMatrix( g_hmView, &s_matView );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : view set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pEffect->SetMatrix( g_hmProj, &g_matProj );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : proj set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&s_camerapos), sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : eyepos set error !!!\n" );
			_ASSERT( 0 );
			return 1;	
		}


	//float3 mLightParams[5][7];
	//0: x=validflag, y=type, z=divRange
	//1: x=cosPhi, y=divCosPhi
	//2: xyz=diffuse
	//3: xyz=specular
	//4: xyz=position
	//5: xyz=direction

		float tmplparam[5][6][4];
		ZeroMemory( tmplparam, sizeof( float ) * 5 * 6 * 4 );

		CLightData* curl;
		curl = g_lighthead;
		int setlno = 0;
		int lno;
		for( lno = 0; lno < 5; lno++ ){
			if( curl ){
				tmplparam[lno][0][0] = 1.0f;

				tmplparam[lno][2][0] = curl->Diffuse.x;
				tmplparam[lno][2][1] = curl->Diffuse.y;
				tmplparam[lno][2][2] = curl->Diffuse.z;

				tmplparam[lno][3][0] = curl->Specular.x;
				tmplparam[lno][3][1] = curl->Specular.y;
				tmplparam[lno][3][2] = curl->Specular.z;

				switch( curl->Type ){
				case D3DLIGHT_DIRECTIONAL:
					tmplparam[lno][0][1] = 0.0f;

					tmplparam[lno][5][0] = curl->orgDirection.x;
					tmplparam[lno][5][1] = curl->orgDirection.y;
					tmplparam[lno][5][2] = curl->orgDirection.z;

					break;
				case D3DLIGHT_POINT:
					tmplparam[lno][0][1] = 1.0f;

					tmplparam[lno][0][2] = curl->divRange;


					tmplparam[lno][4][0] = curl->orgPosition.x;
					tmplparam[lno][4][1] = curl->orgPosition.y;
					tmplparam[lno][4][2] = curl->orgPosition.z;

					break;
				case D3DLIGHT_SPOT:
					tmplparam[lno][0][1] = 2.0f;

					tmplparam[lno][0][2] = curl->divRange;
					tmplparam[lno][1][0] = curl->cosPhi;
					tmplparam[lno][1][1] = curl->divcosPhi;

					tmplparam[lno][4][0] = curl->orgPosition.x;
					tmplparam[lno][4][1] = curl->orgPosition.y;
					tmplparam[lno][4][2] = curl->orgPosition.z;

					tmplparam[lno][5][0] = curl->orgDirection.x;
					tmplparam[lno][5][1] = curl->orgDirection.y;
					tmplparam[lno][5][2] = curl->orgDirection.z;

					break;
				}
					
				curl = curl->nextlight;
				setlno++;

			}else{
				tmplparam[lno][0][0] = 0.0f;
				tmplparam[lno][0][1] = -1.0f;
			}
		}

		float lbump[3][6][4];
		ZeroMemory( lbump, sizeof( float ) * 3 * 6 * 4 );

		int bulno;
		int bulsetno = 0;
		for( bulno = 0; bulno < 3; bulno++ ){
			CLightData* lbptr;
			lbptr = GetLightData( g_LightOfBump[ bulno ] );
			if( (bulno == 0) && !lbptr && g_lighthead ){
				lbptr = g_lighthead;//���݂��Ȃ�������擪���C�g
			}

			if( lbptr ){
				lbump[bulsetno][0][0] = 1.0f;

				lbump[bulsetno][2][0] = lbptr->Diffuse.x;
				lbump[bulsetno][2][1] = lbptr->Diffuse.y;
				lbump[bulsetno][2][2] = lbptr->Diffuse.z;

				lbump[bulsetno][3][0] = lbptr->Specular.x;
				lbump[bulsetno][3][1] = lbptr->Specular.y;
				lbump[bulsetno][3][2] = lbptr->Specular.z;

				switch( lbptr->Type ){
				case D3DLIGHT_DIRECTIONAL:
					lbump[bulsetno][0][1] = 0.0f;

					lbump[bulsetno][5][0] = lbptr->orgDirection.x;
					lbump[bulsetno][5][1] = lbptr->orgDirection.y;
					lbump[bulsetno][5][2] = lbptr->orgDirection.z;

					break;
				case D3DLIGHT_POINT:
					lbump[bulsetno][0][1] = 1.0f;

					lbump[bulsetno][0][2] = lbptr->divRange;


					lbump[bulsetno][4][0] = lbptr->orgPosition.x;
					lbump[bulsetno][4][1] = lbptr->orgPosition.y;
					lbump[bulsetno][4][2] = lbptr->orgPosition.z;

					break;
				case D3DLIGHT_SPOT:
					lbump[bulsetno][0][1] = 2.0f;

					lbump[bulsetno][0][2] = lbptr->divRange;
					lbump[bulsetno][1][0] = lbptr->cosPhi;
					lbump[bulsetno][1][1] = lbptr->divcosPhi;

					lbump[bulsetno][4][0] = lbptr->orgPosition.x;
					lbump[bulsetno][4][1] = lbptr->orgPosition.y;
					lbump[bulsetno][4][2] = lbptr->orgPosition.z;

					lbump[bulsetno][5][0] = lbptr->orgDirection.x;
					lbump[bulsetno][5][1] = lbptr->orgDirection.y;
					lbump[bulsetno][5][2] = lbptr->orgDirection.z;

					break;
				}

				bulsetno++;
			}
		}
		g_bumplightnum = bulsetno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if( g_bumplightnum == 0 ){

			//dummy data
			lbump[0][0][0] = 1.0f;

			lbump[0][2][0] = 0.0f;
			lbump[0][2][1] = 0.0f;
			lbump[0][2][2] = 0.0f;

			lbump[0][3][0] = 0.0f;
			lbump[0][3][1] = 0.0f;
			lbump[0][3][2] = 0.0f;

			///////directional
			lbump[0][0][1] = 0.0f;

			lbump[0][5][0] = 0.0f;
			lbump[0][5][1] = 0.0f;
			lbump[0][5][2] = 1.0f;

			g_bumplightnum = 1;
		}


		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 3 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmBumpLight, &(lbump[0][0][0]), sizeof( float ) * 3 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mBumpLight SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 1 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		g_lightnum = setlno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	}else{
		//shadowmap
		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			D3DXMATRIX matLP;
			matLP = s_matViewShadow * s_matProjShadow;
			hres = g_pEffect->SetMatrix( g_hmLP, &matLP );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLP SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			D3DXMATRIX matLPB;
			matLPB = matLP * s_matBiasShadow;
			hres = g_pEffect->SetMatrix( g_hmLPB, &matLPB );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLPB SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmShadowCoef, &(s_shadowcoef[0]), sizeof( float ) * 2 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mShadowCoef SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			hres = g_pEffect->SetTexture( g_hShadowMap, s_ShadowMapTex );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : ShadowMap SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int GetShaderHandle()
{
	if( !g_pEffect ){
		_ASSERT( 0 );
		return 1;
	}

	g_hRenderSceneBone4 = g_pEffect->GetTechniqueByName( "RenderSceneBone4" );
	g_hRenderSceneBone1 = g_pEffect->GetTechniqueByName( "RenderSceneBone1" );
	g_hRenderSceneBone0 = g_pEffect->GetTechniqueByName( "RenderSceneBone0" );
	g_hRenderSceneBone4Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone4Toon" );
	g_hRenderSceneBone1Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone1Toon" );
	g_hRenderSceneBone0Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone0Toon" );
	g_hRenderSceneBone4ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone4ToonNoTex" );
	g_hRenderSceneBone1ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone1ToonNoTex" );
	g_hRenderSceneBone0ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone0ToonNoTex" );
	g_hRenderSceneBone4Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone4Toon1" );
	g_hRenderSceneBone1Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone1Toon1" );
	g_hRenderSceneBone0Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone0Toon1" );
	g_hRenderSceneBone4Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone4Edge0" );
	g_hRenderSceneBone1Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone1Edge0" );
	g_hRenderSceneBone0Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone0Edge0" );
	g_hRenderSceneBone4Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone4Bump" );//vs2
	g_hRenderSceneBone1Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone1Bump" );//vs2
	g_hRenderSceneBone0Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone0Bump" );//vs2
	g_hRenderSceneNormalShadow = g_pEffect->GetTechniqueByName( "RenderSceneNormalShadow" );//vs2
	g_hRenderSceneBumpShadow = g_pEffect->GetTechniqueByName( "RenderSceneBumpShadow" );//vs2
	g_hRenderSceneToon1Shadow = g_pEffect->GetTechniqueByName( "RenderSceneToon1Shadow" );//vs2
	g_hRenderSceneToon0Shadow = g_pEffect->GetTechniqueByName( "RenderSceneToon0Shadow" );//vs2
	g_hRenderSceneBone4PP = g_pEffect->GetTechniqueByName( "RenderSceneBone4PP" );
	g_hRenderSceneBone0PP = g_pEffect->GetTechniqueByName( "RenderSceneBone0PP" );
	g_hRenderScenePPShadow = g_pEffect->GetTechniqueByName( "RenderScenePPShadow" );



	g_hmWorldMatrixArray = g_pEffect->GetParameterByName( NULL, "mWorldMatrixArray" );
	g_hmView = g_pEffect->GetParameterByName( NULL, "mView" );
	g_hmProj = g_pEffect->GetParameterByName( NULL, "mProj" );
	g_hmEyePos = g_pEffect->GetParameterByName( NULL, "mEyePos" );
	g_hmLightParams = g_pEffect->GetParameterByName( NULL, "mLightParams" );
	g_hmFogParams = g_pEffect->GetParameterByName( NULL, "mFogParams" );
	g_hmToonParams = g_pEffect->GetParameterByName( NULL, "mToonParams" );
	g_hmLightNum = g_pEffect->GetParameterByName( NULL, "mLightNum" );
	g_hmEdgeCol0 = g_pEffect->GetParameterByName( NULL, "mEdgeCol0" );
	g_hmAlpha = g_pEffect->GetParameterByName( NULL, "mAlpha" );
	g_hmBumpLight = g_pEffect->GetParameterByName( NULL, "mBumpLight" );//vs2
	g_hNormalMap = g_pEffect->GetParameterByName( NULL, "NormalMap" );//vs2
	g_hDecaleTex = g_pEffect->GetParameterByName( NULL, "DecaleTex" );//vs2
	g_hDecaleTex1 = g_pEffect->GetParameterByName( NULL, "DecaleTex1" );//vs2
	g_hmLP = g_pEffect->GetParameterByName( NULL, "mLP" );//vs2
	g_hmLPB = g_pEffect->GetParameterByName( NULL, "mLPB" );//vs2
	g_hShadowMap = g_pEffect->GetParameterByName( NULL, "ShadowMap" );//vs2
	g_hmShadowCoef = g_pEffect->GetParameterByName( NULL, "mShadowCoef" );//vs2
	g_hmoldView = g_pEffect->GetParameterByName( NULL, "moldView" );//vs2
	g_hmMinAlpha = g_pEffect->GetParameterByName( NULL, "mMinAlpha" );//vs2
	g_hmGlowMult = g_pEffect->GetParameterByName( NULL, "mGlowMult" );
	g_hmToonDiffuse = g_pEffect->GetParameterByName( NULL, "mToonDiffuse" );
	g_hmToonAmbient = g_pEffect->GetParameterByName( NULL, "mToonAmbient" );
	g_hmToonSpecular = g_pEffect->GetParameterByName( NULL, "mToonSpecular" );

/////////////// PostEffect
	if( g_pPostEffect ){
		g_hPostEffectBlur0 = g_pPostEffect->GetTechniqueByName( "PostEffectBlur0" );
		g_hPostEffectGauss = g_pPostEffect->GetTechniqueByName( "PostEffectBlurGauss" );
		g_hPostEffectTLV = g_pPostEffect->GetTechniqueByName( "PostEffectTLV" );
		g_hPostEffectDownScale = g_pPostEffect->GetTechniqueByName( "PostEffectDownScale" );
		g_hPostEffectToneMap = g_pPostEffect->GetTechniqueByName( "PostEffectToneMap" );
		g_hPostEffectBrightPass = g_pPostEffect->GetTechniqueByName( "PostEffectBrightPass" );
		g_hPostEffectHDRFinal = g_pPostEffect->GetTechniqueByName( "PostEffectHDRFinal" );
		g_hPostEffectMono = g_pPostEffect->GetTechniqueByName( "PostEffectMono" );
		g_hPostEffectCbCr = g_pPostEffect->GetTechniqueByName( "PostEffectCbCr" );
		g_hPostEffectUser[0] = g_pPostEffect->GetTechniqueByName( "PostEffectUser0" );
		g_hPostEffectUser[1] = g_pPostEffect->GetTechniqueByName( "PostEffectUser1" );
		g_hPostEffectUser[2] = g_pPostEffect->GetTechniqueByName( "PostEffectUser2" );
		g_hPostEffectUser[3] = g_pPostEffect->GetTechniqueByName( "PostEffectUser3" );
		g_hPostEffectUser[4] = g_pPostEffect->GetTechniqueByName( "PostEffectUser4" );

		g_hpeWidth0 = g_pPostEffect->GetParameterByName( NULL, "WIDTH0" );
		g_hpeHeight0 = g_pPostEffect->GetParameterByName( NULL, "HEIGHT0" );
		g_hpeWidth1 = g_pPostEffect->GetParameterByName( NULL, "WIDTH1" );
		g_hpeHeight1 = g_pPostEffect->GetParameterByName( NULL, "HEIGHT1" );
		g_hpeTexture0 = g_pPostEffect->GetParameterByName( NULL, "Texture0" );
		g_hpeTexture1 = g_pPostEffect->GetParameterByName( NULL, "Texture1" );
		g_hpeTexture2 = g_pPostEffect->GetParameterByName( NULL, "Texture2" );
		g_hpeWeight = g_pPostEffect->GetParameterByName( NULL, "weight" );
		g_hpeOffsetX = g_pPostEffect->GetParameterByName( NULL, "offsetX" );
		g_hpeOffsetY = g_pPostEffect->GetParameterByName( NULL, "offsetY" );
		g_hpeSampleWeight = g_pPostEffect->GetParameterByName( NULL, "avSampleWeights" );
		g_hpeSampleOffset = g_pPostEffect->GetParameterByName( NULL, "avSampleOffsets" );
		g_hpeMiddleGray = g_pPostEffect->GetParameterByName( NULL, "fMiddleGray" );
		g_hpeBrightPassThreshold = g_pPostEffect->GetParameterByName( NULL, "fBrightPassThreshold" );
		g_hpeBrightPassOffset = g_pPostEffect->GetParameterByName( NULL, "fBrightPassOffset" );
		g_hpeColMult = g_pPostEffect->GetParameterByName( NULL, "colmult" );
		g_hpeCbCr = g_pPostEffect->GetParameterByName( NULL, "CbCr" );
		g_hpeUserFl4[0] = g_pPostEffect->GetParameterByName( NULL, "userFL4_0" );
		g_hpeUserFl4[1] = g_pPostEffect->GetParameterByName( NULL, "userFL4_1" );
		g_hpeUserFl4[2] = g_pPostEffect->GetParameterByName( NULL, "userFL4_2" );
		g_hpeUserFl4[3] = g_pPostEffect->GetParameterByName( NULL, "userFL4_3" );
		g_hpeUserFl4[4] = g_pPostEffect->GetParameterByName( NULL, "userFL4_4" );
		g_hpeUserFl4[5] = g_pPostEffect->GetParameterByName( NULL, "userFL4_5" );
		g_hpeUserFl4[6] = g_pPostEffect->GetParameterByName( NULL, "userFL4_6" );
		g_hpeUserFl4[7] = g_pPostEffect->GetParameterByName( NULL, "userFL4_7" );
		g_hpeUserFl4[8] = g_pPostEffect->GetParameterByName( NULL, "userFL4_8" );
		g_hpeUserFl4[9] = g_pPostEffect->GetParameterByName( NULL, "userFL4_9" );
		g_hpeUserTex[0] = g_pPostEffect->GetParameterByName( NULL, "usertex0" );
		g_hpeUserTex[1] = g_pPostEffect->GetParameterByName( NULL, "usertex1" );
		g_hpeUserTex[2] = g_pPostEffect->GetParameterByName( NULL, "usertex2" );
		g_hpeUserTex[3] = g_pPostEffect->GetParameterByName( NULL, "usertex3" );
		g_hpeUserTex[4] = g_pPostEffect->GetParameterByName( NULL, "usertex4" );
	}

	return 0;
}

int E3DSetSpriteUV()// $1B8
{
	int spid;
	spid = code_getdi(0);

	float su;
	su = (float)code_getdd(0.0);

	float eu;
	eu = (float)code_getdd(1.0);

	float sv;
	sv = (float)code_getdd(0.0);

	float ev;
	ev = (float)code_getdd(1.0);

/////////////
	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteUV : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursprite->SetSpriteUV( su, eu, sv, ev );
	if( ret ){
		DbgOut( "E3DSetSpriteUV : sprite SetSpriteUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DCreateRenderTargetTexture()// $1B9
{
	//E3DCreateRenderTargetTexture sizex, sizey, scid, texid, okflag

	int sizex;
	sizex = code_getdi(256);

	int sizey;
	sizey = code_getdi(256);

	PVal *pvalscid;
	APTR aptrscid;
	aptrscid = code_getva( &pvalscid );

	PVal *pvaltexid;
	APTR aptrtexid;
	aptrtexid = code_getva( &pvaltexid );

	PVal *pvalok;
	APTR aptrok;
	aptrok = code_getva( &pvalok );

	int srcfmt;
	srcfmt = code_getdi( D3DFMT_A8R8G8B8 );

///////////////////////
	int ret;
	int okflag = 0;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );//!!!!!!!!!!!

	int rettexid = -1;
	int retscid = -1;
	code_setva( pvaltexid, aptrtexid, TYPE_INUM, &rettexid );
	code_setva( pvalscid, aptrscid, TYPE_INUM, &retscid );


	ret = CreateRenderTargetTexture( sizex, sizey, srcfmt, &retscid, &rettexid );
	if( ret ){
		DbgOut( "E3DCreateRenderTargetTexture : CreateRenderTargetTexture error !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!! okflag 0�Ń��^�[��
	}

	///
	code_setva( pvaltexid, aptrtexid, TYPE_INUM, &rettexid );
	code_setva( pvalscid, aptrscid, TYPE_INUM, &retscid );
	okflag = 1;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );//!!!!!!!!!!!


	return 0;
}

int CreateRenderTargetTexture( int sizex, int sizey, int srcfmt, int* scidptr, int* texidptr )
{
	int ret;
	///texture

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "CreateRenderTargetTexture : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "CreateRenderTargetTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH ];
	s_usertexcnt++;
	sprintf_s( usertexname, MAX_PATH, "RenderTarget_%d", s_usertexcnt );

EnterCriticalSection( &g_crit_restore );//######## start crit

	int transparent = 0;
	int pool = D3DPOOL_DEFAULT;

	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "CreateRenderTargetTexture : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "CreateRenderTargetTexture : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}

	ret = g_texbnk->SetFMT( newindex, srcfmt );
	_ASSERT( !ret );


	ret = g_texbnk->CreateTexData( newindex, pdev, sizex, sizey );
	if( ret ){
		DbgOut( "CreateRenderTargetTexture : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}

LeaveCriticalSection( &g_crit_restore );//###### end crit


	//swapchain
	SWAPCHAINELEM* cursc = 0;
	cursc = GetFreeSwapChainElem();
	if( !cursc ){
		DbgOut( "CreateRenderTargetTexture : GetFreeSwapChainElem NULL error : swap chain num overflow !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CreateSwapChainElem( s_swapchaincnt, cursc, -1, -1, newserial, 0 );
	if( ret ){
		DbgOut( "CreateRenderTargetTexture : CreateSwapChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_swapchaincnt++;
//////////////
	*scidptr = cursc->scid;
	*texidptr = newserial;

	return 0;
}

int DestroyRenderTargetTexture( int scid, int texid )
{
//swapchain
	if( scid <= 0 ){
		DbgOut( "DestroyRenderTargetTexture : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		return 0;
	}

	int ret;
	ret = DestroySwapChainElem( cursc, 1 );
	if( ret ){
		DbgOut( "DestroyRenderTargetTexture : DestroySwapChainElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//texbank
	if( !g_texbnk ){
		DbgOut( "DestroyRenderTargetTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "DestroyRenderTargetTexture : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "DestroyRenderTargetTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex;
	texindex = g_texbnk->FindName( texname, transparent );
	if( texindex < 0 ){
		DbgOut( "DestroyRenderTargetTexture : texbank FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->DestroyTexture( texindex );
	_ASSERT( !ret );


	return 0;
}


int E3DDestroyRenderTargetTexture()// $1BA
{
	int scid;
	scid = code_getdi(0);

	int texid;
	texid = code_getdi(0);

///////////
	int ret;
	ret = DestroyRenderTargetTexture( scid, texid );
	if( ret ){
		DbgOut( "E3DDestroyRenderTargetTexture : DestroyRenderTargetTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetDSFillUpMode()// $1BB
{
	int hsid;
	hsid = code_getdi(0);

	int mode;
	mode = code_getdi(0);

	//////////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDSFillUpMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetDSFillUpMode( mode );
	if( ret ){
		DbgOut( "E3DSetDSFillUpMode : hs SetDSFillUpMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DSetTexFillUpMode()// $1BC
{

	int hsid;
	hsid = code_getdi(0);

	int mode;
	mode = code_getdi(0);

	//////////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetTexFillUpMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetTexFillUpMode( mode );
	if( ret ){
		DbgOut( "E3DSetTexFillUpMode : hs SetTexFillUpMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int E3DSetShadowMapCamera()// $1BD
{
	//E3DSetShadowMapCamera campos, camtarget, camupvec

	PVal *pvalpos;
	APTR aptrpos;
	aptrpos = code_getva( &pvalpos );

	PVal *pvaltar;
	APTR aptrtar;
	aptrtar = code_getva( &pvaltar );

	PVal *pvalup;
	APTR aptrup;
	aptrup = code_getva( &pvalup );

////////////////
	
	D3DXVECTOR3 pos;
	pos.x = (float)( *((double*)pvalpos->pt + aptrpos) );
	pos.y = (float)( *((double*)pvalpos->pt + aptrpos + 1) );
	pos.z = (float)( *((double*)pvalpos->pt + aptrpos + 2) );
	
	D3DXVECTOR3 tar;
	tar.x = (float)( *((double*)pvaltar->pt + aptrtar) );
	tar.y = (float)( *((double*)pvaltar->pt + aptrtar + 1) );
	tar.z = (float)( *((double*)pvaltar->pt + aptrtar + 2) );

	D3DXVECTOR3 up;
	up.x = (float)( *((double*)pvalup->pt + aptrup) );
	up.y = (float)( *((double*)pvalup->pt + aptrup + 1) );
	up.z = (float)( *((double*)pvalup->pt + aptrup + 2) );
	
	D3DXMatrixLookAtLH( &s_matViewShadow, &pos, &tar, &up );

	s_camposshadow = pos;
	s_camtarshadow = tar;
	s_camupshadow = up;
	

	return 0;
}
int E3DSetShadowMapProjOrtho()// $1BE
{
	//E3DSetShadowMapProjection rttexid, near, far, fov

	int rttexid;
	rttexid = code_getdi(0);

	float pnear;
	pnear = (float)code_getdd(1000.0);

	float pfar;
	pfar = (float)code_getdd(50000.0);

	float volsize;
	volsize = (float)code_getdd(3000.0);

//////////////////
	
	int ret;
	if( !g_texbnk ){
		DbgOut( "E3DSetShadowMapProjection : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( rttexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DSetShadowMapProjection : texbank GetTextureSizeBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	float aspect;
//	aspect = (float)width / (float)height;
//	float fovr;
//	fovr = fov * (float)DEG2PAI;
//	D3DXMatrixPerspectiveFovLH( &s_matProjShadow, fovr, aspect, pnear, pfar );


	float vw, vh;
	vw = volsize;
	vh = volsize * ((float)height / (float)width);
	D3DXMatrixOrthoLH( &s_matProjShadow, vw, vh, pnear, pfar );


	// �ˉe��Ԃ���A�e�N�X�`���[�̋�Ԃɕϊ�����
	float fOffsetX = 0.5f + (0.5f / (float)width);
	float fOffsetY = 0.5f + (0.5f / (float)height);
	s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
							0.0f,    -0.5f,     0.0f,   0.0f,
							0.0f,     0.0f,     0.0f,	0.0f,
							fOffsetX, fOffsetY, 0.0f,   1.0f );

//	s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
//							0.0f,    -0.5f,     0.0f,   0.0f,
//							0.0f,     0.0f,     0.0f,	0.0f,
//							0.5f,	  0.5f,     0.0f,   1.0f );
	
	return 0;
}
int E3DRenderWithShadow()// $1BF
{
	//E3DRenderWithShadow scid, rtscid, rttexid, hsidarray, arnum

	int scid;
	scid = code_getdi(0);

	int rtscid;
	rtscid = code_getdi(0);

	int rttexid;
	rttexid = code_getdi(0);

	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	int num;
	num = code_getdi(0);

	int skipflag;
	skipflag = code_getdi(0);

//////////////////////


	int ret;

	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderWithShadow : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_shadowUse == 0 ){
		ret = RenderNormalBatch( scid, (int*)pvalhsid->pt + aptrhsid, num, 1, skipflag, 0 );
		_ASSERT( !ret );
		return ret;//!!!!!!!!!!!!!!!
	}



	if( !g_pD3DApp || !g_texbnk ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->GetTexDataBySerial( rttexid, &s_ShadowMapTex );
	if( ret || !s_ShadowMapTex ){
		DbgOut( "RenderWithShadow : texbank GetTexDataBySerial shadowmap error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->BeginScene();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}


	SWAPCHAINELEM* tmpelem;
	tmpelem = GetSwapChainElem( scid );
	float aspect0 = (float)g_scsize.x / (float)g_scsize.y;


//render to shadowmap
	//rendertarget, zbuffer, viewport, clear
	ret = SetRenderTarget( pdev, rtscid, 1, 0 );
	if( ret ){
		DbgOut( "RenderWithShadow : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();

	if( s_shadowmode == 0 ){

		ret = ChkInViewAndTransformBatch( pdev, (int*)pvalhsid->pt + aptrhsid, num, 1, 1, 0 );
		if( ret ){
			DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  shadow 0  error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CVec3List	cList;
		ret = SetPointListForShadow( &cList, aspect0, (int*)pvalhsid->pt + aptrhsid, num );
		if( ret ){
			DbgOut( "E3DRenderWithShadow : SetPointListForShadow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// �ϊ��s������߂�
		g_sStatus.vEyePos = s_camerapos;
		D3DXVECTOR3 vdir = s_cameratarget - s_camerapos;
		D3DXVec3Normalize( &vdir, &vdir );
		g_sStatus.vViewDir = vdir;
		g_sStatus.fNearDist = g_proj_near;
		g_sStatus.vLightDir = s_lightdirshadow;
		CalcLispMatrix(&g_sStatus, &cList);
		s_matViewShadow = g_sStatus.matView;
		s_matProjShadow = g_sStatus.matProj;

		// �ˉe��Ԃ���A�e�N�X�`���[�̋�Ԃɕϊ�����
		float fOffsetX = 0.5f + (0.5f / (float)g_scsize.x);
		float fOffsetY = 0.5f + (0.5f / (float)g_scsize.y);
		s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
								0.0f,    -0.5f,     0.0f,   0.0f,
								0.0f,     0.0f,     0.0f,	0.0f,
								fOffsetX, fOffsetY, 0.0f,   1.0f );
	}

	//Shader Const
	ret = SetShaderConst( 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : SetShaderConst 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, (int*)pvalhsid->pt + aptrhsid, num, 0, 1, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetTempFogEnable( pdev, 0 );


	//withalpha 0, 1 �܂Ƃ߂āB�A���t�@�e�X�g�L��B
	ret = RenderShadowMap0( pdev, (int*)pvalhsid->pt + aptrhsid, num );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap0  pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//render to screen
	//rendertarget, zbuffer, viewport, clear
	ret = SetRenderTarget( pdev, scid, 0, skipflag );
	if( ret ){
		DbgOut( "RenderWithShadow : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = SetShaderConst( 0 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : SetShaderConst 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, (int*)pvalhsid->pt + aptrhsid, num, 1, 1, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  pass1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RestoreTempFogEnable();


	//withalpha 0
	ret = RenderShadowMap1( pdev, (int*)pvalhsid->pt + aptrhsid, num, 0 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap1 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( skipflag == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	//withalpha 1
	ret = RenderShadowMap1( pdev, (int*)pvalhsid->pt + aptrhsid, num, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap1 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	hr = pdev->EndScene();
	_ASSERT( hr == D3D_OK );

	return 0;
}

int RenderNormalBatch( int scid, int* hsidptr, int num, int needchkinview, int skipflag, int* modeptr )
{

	int ret;
	if( !g_pD3DApp || !g_texbnk ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->BeginScene();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_rendercnt == 0 ){
		//�X�v���C�g�`��ŃX�e�[�g���s��ɂȂ�̂ł����ł̌Ăяo�����K�v
		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
	}	
	g_rendercnt++;


	ret = SetRenderTarget( pdev, scid, 0, skipflag );
	if( ret ){
		DbgOut( "RenderNormalBatch : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();

	ret = SetShaderConst( 0 );
	if( ret ){
		DbgOut( "RenderNormalBatch : SetShaderConst 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, hsidptr, num, 1, needchkinview, 1 );
	if( ret ){
		DbgOut( "RenderNormalBatch : ChkInViewAndTransformBatch  pass1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	//withalpha 0
	ret = RenderBatch( pdev, hsidptr, num, 0, modeptr );
	if( ret ){
		DbgOut( "RenderNormalBatch : RenderShadowMap1 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( skipflag == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	//withalpha 1
	ret = RenderBatch( pdev, hsidptr, num, 1, modeptr );
	if( ret ){
		DbgOut( "RenderNormalBatch : RenderShadowMap1 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	hr = pdev->EndScene();
	_ASSERT( hr == D3D_OK );

	return 0;
}

int SetPointListForShadow( CVec3List* plist, float aspect, int* hsidptr, int num )
{
	// �r���[�{�����[�������߂�
	float fov = g_proj_fov * (float)DEG2PAI;
	CalcPointsOfViewBolume( plist, s_matView, fov, aspect, g_proj_near, g_proj_far );

//	CalcPointsOfViewBolume( plist, s_matView, 90.0f, aspect, g_proj_near, g_proj_far );

	int ret;
	int i;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
			if( curhs && (curhs->m_billboardflag) != 0 ){
				bbflag = 1;
			}
		}

		if( curhs ){
			ret = curhs->SetPointListForShadow( plist );
			if( ret ){
				DbgOut( "SetPointListForShadow : hs SetPointListForShadow error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int ChkInViewAndTransformBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int passno, int needchkinview, int needtrans )
{
	int ret;
	int i;

	D3DXMATRIX curmatview;
	D3DXVECTOR3 curcampos;
	D3DXVECTOR3 curcamtar;
	if( passno == 0 ){
		curmatview = s_matViewShadow;
		if( s_shadowmode == 0 ){
			curcampos = g_sStatus.newpos;
			curcamtar = g_sStatus.newpos + g_sStatus.vLightDir;
		}else{
			curcampos = s_camposshadow;
			curcamtar = s_camtarshadow;
		}
	}else{
		curmatview = s_matView;
		curcampos = s_camerapos;
		curcamtar = s_cameratarget;
	}

	/***
	if( renderflag ){
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

		pdev->SetRenderState( D3DRS_ALPHAREF,        0x08 );
		g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

		pdev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
		pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
	}
	***/

	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
			if( curhs && (curhs->m_billboardflag) != 0 ){
				bbflag = 1;
			}
		}

		if( curhs ){

			int inviewflag = 1;

			if( needchkinview ){
				if( passno == 0 ){
					ret = curhs->ChkInView( curmatview, &inviewflag, &s_matProjShadow );
				}else{
					ret = curhs->ChkInView( curmatview, &inviewflag, 0 );
				}
				if( ret ){
					DbgOut( "ChkInViewAndTransformBatch : hs ChkInView error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( needtrans && inviewflag ){
				if( bbflag == 0 ){
					ret = curhs->Transform( curcampos, pdev, curmatview, 1, 0 );
					if( ret ){
						DbgOut( "ChkInViewAndTransformBatch : hs Transform error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( bbflag != 0 ){
					ret = curhs->TransformBillboard( pdev, curmatview, &curcampos, &curcamtar );
					if( ret ){
						DbgOut( "ChkInViewAndTransformBatch : hs TransformBillboard  error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}		
			}

			/***
			if( renderflag && inviewflag ){
				ret = curhs->Render( 1, pdev, 0, s_matView, s_camerapos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
			***/
		}
	}

	return 0;
}

int SetRenderTarget( LPDIRECT3DDEVICE9 pdev, int scid, int shadowmapflag, int skipflag )
{
	//int ret;
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "SetRenderTarget : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	ret = g_pD3DApp->TestCooperative();
//	_ASSERT( !ret );

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    // Clear the viewport

	if( skipflag == 0 ){
		if( shadowmapflag == 0 ){
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 cursc->clearcol,
								 //0,
								 1.0f, 0L );
		}else{
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 0xFFFFFFFF,
								 //0x00000000,
								 1.0f, 0L );
		}
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			DbgOut( "SetRenderTarget : Clear error !!!\n" );
			return 1;
		}
	}

	return 0;
}

int RenderShadowMap0( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num )
{
	int ret;
	int i;
	//HRESULT hres;


	if( g_rendercnt == 0 ){
		//�X�v���C�g�`��ŃX�e�[�g���s��ɂȂ�̂ł����ł̌Ăяo�����K�v
		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
	}
	g_rendercnt++;



	pdev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

	pdev->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

	pdev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

	pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
	pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
	pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
	pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
	g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
	g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
	g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
		}

		if( curhs ){
			if( s_shadowmode == 0 ){
				ret = curhs->RenderShadowMap0( pdev, &s_matViewShadow, &s_matProjShadow, g_sStatus.newpos );
			}else{
				ret = curhs->RenderShadowMap0( pdev, &s_matViewShadow, &s_matProjShadow, s_camposshadow );
			}
			if( ret ){
				DbgOut( "RenderShadowMap0 : hs RenderShadowMap0 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int RenderShadowMap1( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha )
{
	int i;
	int ret;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			if( withalpha == 1 ){//!!!!!!!!!!!
				curhs = g_bbhs;
				bbflag = 1;
			}
		}else{
			curhs = GetHandlerSet( curhsid );
		}

		if( curhs ){
			ret = curhs->RenderShadowMap1( pdev, withalpha, &s_matView, &g_matProj, s_camerapos );
			if( ret ){
				DbgOut( "RenderShadowMap1 : hs RenderShadowMap1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int RenderBatch( LPDIRECT3DDEVICE9 pd3dDevice, int* hsidptr, int num, int withalpha, int* modeptr )
{
	if( withalpha == 0 ){

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
		g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	}else{

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

////////////////////////////
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	}

	int i;
	int ret;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );

		int renderflag;
		if( modeptr == 0 ){
			renderflag = 1;
		}else{
			int curmode;
			curmode = *( modeptr + i );
			renderflag = ((withalpha == 0) && (curmode & 1)) || ((withalpha != 0) && (curmode & 2));
		}

		int bbflag = 0;
		if( renderflag ){

			CHandlerSet* curhs = 0;
			if( curhsid == -1 ){
				if( withalpha == 1 ){//!!!!!!!!!!!
					curhs = g_bbhs;
					bbflag = 1;
				}
			}else{
				curhs = GetHandlerSet( curhsid );
			}

			if( curhs ){			
				ret = curhs->Render( 1, pd3dDevice, withalpha, s_matView, s_camerapos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}


	return 0;
}



int E3DSetShadowBias()// $1C0
{
	s_shadowcoef[0] = (float)code_getdd(0.005);

	return 0;
}

int SetTempFogEnable( LPDIRECT3DDEVICE9 pdev, int srcenable )
{
	int ret;

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->SetTempFogEnable( srcenable );
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->SetTempFogEnable( srcenable );
		_ASSERT( !ret );
	}

	g_renderstate[D3DRS_FOGENABLE] = srcenable;
	pdev->SetRenderState( D3DRS_FOGENABLE, srcenable );


	return 0;
}
int RestoreTempFogEnable()
{
	int ret;

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->RestoreTempFogEnable();
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->RestoreTempFogEnable();
		_ASSERT( !ret );
	}

	return 0;
}


int E3DCheckRTFormat()// $1C1
{
	int srcfmt;
	srcfmt = code_getdi( D3DFMT_A8R8G8B8 );

	PVal *pvalok;
	APTR aptrok;
	aptrok = code_getva( &pvalok );

////////////


	int ret;
	int okflag = 0;
	ret = g_pD3DApp->CheckRTFormat( srcfmt, &okflag );
	if( ret ){
		DbgOut( "E3DCheckRTFormat : app CheckRTFormat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );

	return 0;
}

int E3DRenderBatchMode()// $1C8
{
	int scid;
	scid = code_getdi(0);

	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	int num;
	num = code_getdi(0);

	int needchkinview;
	needchkinview = code_getdi(1);

	int skipflag;
	skipflag = code_getdi(0);

	PVal *pvalmode;
	APTR aptrmode;
	aptrmode = code_getva( &pvalmode );

//////////////////////
	int ret;

	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderBatchMode : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = RenderNormalBatch( scid, (int*)pvalhsid->pt + aptrhsid, num, needchkinview, skipflag, (int*)pvalmode->pt + aptrmode );
	if( ret ){
		DbgOut( "E3DRenderBatchMode : RenderNormalBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int E3DRenderBatch()// $1C4
{
	int scid;
	scid = code_getdi(0);

	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	int num;
	num = code_getdi(0);

	int needchkinview;
	needchkinview = code_getdi(1);

	int skipflag;
	skipflag = code_getdi(0);

//////////////////////
	int ret;

	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderBatch : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = RenderNormalBatch( scid, (int*)pvalhsid->pt + aptrhsid, num, needchkinview, skipflag, 0 );
	if( ret ){
		DbgOut( "E3DRenderBatch : RenderNormalBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetVertPosBatchAOrder()// $1CB
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertnoarray
	PVal *pvalvno;
	APTR aptrvno;
	aptrvno = code_getva( &pvalvno );

//4 : vertnum
	int vertnum;
	vertnum = code_getdi(0);

//5 : vert
	PVal *pvalv;
	APTR aptrv;
	aptrv = code_getva( &pvalv );

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPosBatchAOrder : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPosBatchDouble( partno, (int*)( pvalvno->pt ) + aptrvno, vertnum, (double*)( pvalv->pt ) + aptrv, ORDER_C );
	if( ret ){
		DbgOut( "E3DSetVertPosBatchAOrder : hs SetVertPosBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetVertPosBatch()// $1C5
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertnoarray
	PVal *pvalvno;
	APTR aptrvno;
	aptrvno = code_getva( &pvalvno );

//4 : vertnum
	int vertnum;
	vertnum = code_getdi(0);

//5 : vert
	PVal *pvalv;
	APTR aptrv;
	aptrv = code_getva( &pvalv );

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPosBatch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPosBatchDouble( partno, (int*)( pvalvno->pt ) + aptrvno, vertnum, (double*)( pvalv->pt ) + aptrv, ORDER_HSP );
	if( ret ){
		DbgOut( "E3DSetVertPosBatch : hs SetVertPosBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int E3DSetShadowDarkness()// $1C6
{
	s_shadowcoef[1] = (float)code_getdd(1.0);

	return 0;
}

int E3DSetShadowMapLightDir()// $1C7
{
	float x;
	x = (float)code_getdd(0.0);

	float y;
	y = (float)code_getdd(-1.0);

	float z;
	z = (float)code_getdd(1.0);

////////////


	s_lightdirshadow.x = x;
	s_lightdirshadow.y = y;
	s_lightdirshadow.z = z;

	D3DXVec3Normalize( &s_lightdirshadow, &s_lightdirshadow );


	return 0;
}

int E3DGlobalToLocal()// $1C9
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : srcx
	float srcx;
	srcx = (float)code_getdd(0.0);

//3 : srcy
	float srcy;
	srcy = (float)code_getdd(0.0);

//4 : srcz
	float srcz;
	srcz = (float)code_getdd(0.0);

//5 : dstx
	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

//6 : dsty
	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

//7 : dstz
	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGlobalToLocal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcv;
	D3DXVECTOR3 dstv( 0.0f, 0.0f, 0.0f );
	srcv.x = srcx;
	srcv.y = srcy;
	srcv.z = srcz;

	ret = curhs->GlobalToLocal( srcv, &dstv );
	if( ret ){
		DbgOut( "E3DGlobalToLocal : hs GlobalToLocal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dstx, dsty, dstz;
	dstx = (double)dstv.x;
	dsty = (double)dstv.y;
	dstz = (double)dstv.z;

	code_setva( pvalx, aptrx, TYPE_DNUM, &dstx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dsty );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dstz );

	return 0;
}
int E3DLocalToGlobal()// $1CA
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : srcx
	float srcx;
	srcx = (float)code_getdd(0.0);

//3 : srcy
	float srcy;
	srcy = (float)code_getdd(0.0);

//4 : srcz
	float srcz;
	srcz = (float)code_getdd(0.0);

//5 : dstx
	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

//6 : dsty
	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

//7 : dstz
	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLocalToGlobal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcv;
	D3DXVECTOR3 dstv( 0.0f, 0.0f, 0.0f );
	srcv.x = srcx;
	srcv.y = srcy;
	srcv.z = srcz;

	ret = curhs->LocalToGlobal( srcv, &dstv );
	if( ret ){
		DbgOut( "E3DLocalToGlobal : hs LocalToGlobal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dstx, dsty, dstz;
	dstx = (double)dstv.x;
	dsty = (double)dstv.y;
	dstz = (double)dstv.z;

	code_setva( pvalx, aptrx, TYPE_DNUM, &dstx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dsty );
	code_setva( pvalz, aptrz, TYPE_DNUM, &dstz );


	return 0;
}

int E3DSetShadowMapMode()// $1CC
{

	s_shadowmode = code_getdi(0);

	return 0;
}

int E3DLoadTextureFromBuf()// $1CD
{
//1 
	int texid;
	texid = code_getdi(0);
	
//2
	PVal *pvalbuf;
	APTR aptrbuf;
	aptrbuf = code_getva( &pvalbuf );

//3
	int bufsize;
	bufsize = code_getdi(0);

//4 transparent
	int transparent;
	transparent = code_getdi( 0 );

////
	int ret;


	if( bufsize <= 0 ){
		DbgOut( "E3DLoadTextureFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}

	if( !g_texbnk ){
		DbgOut( "E3DLoadTextureFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////
	int texindex = -1;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DLoadTextureFromBuf : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->SetBufData( texindex, (char*)pvalbuf->pt + aptrbuf, bufsize );
	if( ret ){
		DbgOut( "E3DLoadTextureFromBuf : texbank SetBufData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DCOLOR colkey;
	if( transparent == 1 ){
		colkey = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	}else{
		colkey = 0;
	}

	ret = g_texbnk->LoadTextureFromBuf( texindex, colkey );
	if( ret ){
		DbgOut( "E3DLoadTextureFromBuf : texbank LoadTextureFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DLoadSpriteFromBMSCR()// $1CE
{
//1
	int spriteid;
	spriteid = code_getdi(0);

//2 : wid
	int wid;
	wid = code_getdi(0);

//3 : transparent
	int transparentflag;
	transparentflag = code_getdi(0);

//4 : tpr
	int tpr = code_getdi(0);
//5 : tpg
	int tpg = code_getdi(0);
//6 : tpb
	int tpb = code_getdi(0);


////////
	int ret;


	BMSCR* bm;
	bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
	if( !bm ){
		DbgOut( "E3DLoadSpriteFromBMSCR : bmscr NULL : window id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMySprite* cursprite = 0;
	cursprite = GetSprite( spriteid );
	if( !cursprite ){
		DbgOut( "E3DLoadSpriteFromBMSCR : GetSprite spriteid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = cursprite->SetSpriteParams2( bm->pbi, bm->pBit, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
	if( ret ){
		DbgOut( "E3DLoadSpriteFromBMSCR : newsprite SetSpriteParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = cursprite->LoadSpriteFromBuf();
	if( ret ){
		DbgOut( "E3DLoadSpriteFromBMSCR : cursprite LoadSpriteFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DSetShadowMapInterFlag()// $1CF
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(-1);

	int srcflag;
	srcflag = code_getdi(0);

//////////////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetShadowMpaInterFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetShadowInterFlag( partno, srcflag );
	if( ret ){
		DbgOut( "E3DSetShadowMapInterFlag : hs SetShadowInterFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DTransformBillboard()// $1D0
{
	int ret;

	if( g_bbhs ){

		if( !g_pD3DApp ){
			_ASSERT( 0 );
			return 1;
		}

		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


		CalcMatView();

		ret = g_bbhs->ChkInView( s_matView, 0, 0 );
		if( ret ){
			DbgOut( "E3DRenderBillboard : ChkInView error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = g_bbhs->TransformBillboard( pdev, s_matView, &s_camerapos, &s_cameratarget );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->TransformBillboardDispData( s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int E3DSetMaterialAlphaTest()
{
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int alphatest0;
	alphatest0 = code_getdi(1);

	int alphaval0;
	alphaval0 = code_getdi(8);

	int alphatest1;
	alphatest1 = code_getdi(0);

	int alphaval1;
	alphaval1 = code_getdi(8);

/////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlphaTest : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	alphaval0 = min( 255, alphaval0 );
	alphaval0 = max( 0, alphaval0 );

	alphaval1 = min( 255, alphaval1 );
	alphaval1 = max( 0, alphaval1 );

	ret = curhs->SetMaterialAlphaTest( matno, alphatest0, alphaval0, alphatest1, alphaval1 );
	if( ret ){
		DbgOut( "E3DSetMaterialAlphaTest : hs SetMaterialAlphaTest error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DCalcMLMotion()// $1D2
{
	int hsid;
	hsid = code_getdi(0);
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcMLMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcMLMotion();
	if( ret ){
		DbgOut( "E3DCalcMLMotion : hs CalcMLMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DCreateSkinMat()// $1D3
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateSkinMat : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateSkinMat( partno );
	if( ret ){
		DbgOut( "E3DCreateSkinMat : hs CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetSymInfElem()// $1D4
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	int vertno;
	vertno = code_getdi(0);

	int symaxis;
	symaxis = code_getdi(SYMAXIS_X);

	float symdist;
	symdist = (float)code_getdd(100.0);

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetSymInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetSymInfElem( partno, vertno, symaxis, symdist );
	if( ret ){
		DbgOut( "E3DSetSymInfElem : hs SetSymInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DUpdateSymInfElem()// $1D5
{
	int hsid;
	hsid = code_getdi(0);

	int excludeflag;
	excludeflag = code_getdi(0);

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DUpdateSymInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->UpdateSymInfElem( excludeflag );
	if( ret ){
		DbgOut( "E3DUpdateSymInfElem : hs UpdateSymInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetVersion()
{
	PVal *pvalver;
	APTR aptrver;
	aptrver = code_getva( &pvalver );
//////////

	int retver = E3DHSPVERSION;

	code_setva( pvalver, aptrver, TYPE_INUM, &retver );

	return 0;
}

//int E3DSetJointInitialPos( int hsid, int jointno, D3DXVECTOR3 pos, int calcflag, int excludeflag )
int E3DSetJointInitialPos()// $1D7
{
	int hsid;
	hsid = code_getdi(0);

	int jointno;
	jointno = code_getdi(0);

	float posx;
	posx = (float)code_getdd(0.0);

	float posy;
	posy = (float)code_getdd(0.0);

	float posz;
	posz = (float)code_getdd(0.0);

	int calcflag;
	calcflag = code_getdi(1);

	int excludeflag;
	excludeflag = code_getdi(1);

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetJointInitialPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 pos;
	pos.x = posx;
	pos.y = posy;
	pos.z = posz;

	int ret;
	ret = curhs->SetJointInitialPos( jointno, pos, calcflag, excludeflag );
	if( ret ){
		DbgOut( "E3DSetJointInitialPos : hs SetJointInitialPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetCharaDir()// $1DA
{
	int hsid;
	hsid = code_getdi(0);

	float degxz;
	degxz = (float)code_getdd(0.0);

	float degy;
	degy = (float)code_getdd(0.0);

	PVal *pvalx;
	APTR aptrx;
	aptrx = code_getva( &pvalx );

	PVal *pvaly;
	APTR aptry;
	aptry = code_getva( &pvaly );

	PVal *pvalz;
	APTR aptrz;
	aptrz = code_getva( &pvalz );

///////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCharaDir : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fDeg2Pai = (float)DEG2PAI;
	D3DXVECTOR3 orgdir;
	float vecx, vecy, vecz;
	vecx = sinf( degxz * fDeg2Pai ) * cosf( degy * fDeg2Pai );
	vecy = sinf( degy * fDeg2Pai );
	vecz = -cosf( degxz * fDeg2Pai ) * cosf( degy * fDeg2Pai );

	CalcMatView();

	D3DXMATRIX dirm;
	dirm = curhs->m_gpd.m_matWorld;
	dirm._41 = 0.0f;
	dirm._42 = 0.0f;
	dirm._43 = 0.0f;


	D3DXVECTOR3 aftvec;
	aftvec.x = dirm._11 * vecx + dirm._21 * vecy + dirm._31 * vecz;// + dirm._41;
	aftvec.y = dirm._12 * vecx + dirm._22 * vecy + dirm._32 * vecz;// + dirm._42;
	aftvec.z = dirm._13 * vecx + dirm._23 * vecy + dirm._33 * vecz;// + dirm._43;
	//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
	
	DXVec3Normalize( &aftvec, &aftvec );


	double daftx, dafty, daftz;
	daftx = (double)aftvec.x;
	dafty = (double)aftvec.y;
	daftz = (double)aftvec.z;

	code_setva( pvalx, aptrx, TYPE_DNUM, &daftx );
	code_setva( pvaly, aptry, TYPE_DNUM, &dafty );
	code_setva( pvalz, aptrz, TYPE_DNUM, &daftz );

	return 0;
}

int E3DGetStrSizeByFontID()// $1DB
{
	int ret;
//1
	int fontid;
	fontid = code_getdi(0);

//2
	char tempname[4099];
	ret = GetName( tempname, 4097 );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalw;
	APTR aptrw;
	aptrw = code_getva( &pvalw );

	PVal *pvalh;
	APTR aptrh;
	aptrh = code_getva( &pvalh );

//////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DGetStrSizeByFontID : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SIZE strsize = {0, 0};
	ret = curfont->CalcStrSize( tempname, &strsize );
	if( ret ){
		DbgOut( "E3DGetStrSizeByFontID : curfont CalcStrSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int retw = (int)strsize.cx;
	int reth = (int)strsize.cy;

	code_setva( pvalw, aptrw, TYPE_INUM, &retw );
	code_setva( pvalh, aptrh, TYPE_INUM, &reth );


	return 0;
}

int E3DSetUVBatch()// $1DC
{
//1 : hsid
	int hsid;
	hsid = code_getdi(0);

//2 : partno
	int partno;
	partno = code_getdi(0);

//3 : vertnoarray
	PVal *pvalvno;
	APTR aptrvno;
	aptrvno = code_getva( &pvalvno );

//4 : vertnum
	int vertnum;
	vertnum = code_getdi(0);

//5 : vert
	PVal *pvaluv;
	APTR aptruv;
	aptruv = code_getva( &pvaluv );

//6 : setflag
	int setflag;
	setflag = code_getdi(0);

//7 : clampflag
	int clampflag;
	clampflag = code_getdi(0);

//////////

	int ret;

	CHandlerSet* curhs = 0;
	if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		curhs = GetHandlerSet( hsid );
	}
	if( !curhs ){
		DbgOut( "E3DSetUVBatch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUVBatchDouble( partno, (int*)( pvalvno->pt ) + aptrvno, vertnum, (double*)( pvaluv->pt ) + aptruv, setflag, clampflag );
	if( ret ){
		DbgOut( "E3DSetUVBatch : hs SetUVBatchDouble error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetPresentInterval()// $1DD
{
	int interval;
	interval = code_getdi(0);

	if( interval == 0 ){
		g_presentinterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}else{
		g_presentinterval = D3DPRESENT_INTERVAL_ONE;
	}

	return 0;
}
int E3DGetScreenPosDouble()// $1DE
{
	// 1 : scid
	int scid;
	scid = code_getdi(0);
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPosDouble : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// 2 : handlerset id
	int hsid;
	hsid = code_getdi(0);

	// 3 : partno
	int partno;
	partno = code_getdi(-1);
	
	//4 : scx
	PVal *pvalscx;
	APTR aptrscx;
	aptrscx = code_getva( &pvalscx );

	//5 : scy
	PVal *pvalscy;
	APTR aptrscy;
	aptrscy = code_getva( &pvalscy );

	//6 : scz
	PVal *pvalscz;
	APTR aptrscz;
	aptrscz = code_getva( &pvalscz );

	//7 : vertno
	int vertno;
	vertno = code_getdi(-1);

	//8 : calcmode
	int calcmode;
	calcmode = code_getdi(1);


///////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPosDouble : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPosDouble : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	//!!!!!! ���f���S�̂̂Q�����W�擾���́Avertno��-1 !!!!!!!
	if( partno < 0 ){
		vertno = -1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPosDouble : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 scpos;

	ret = curhs->GetScreenPos3F( partno, &scpos, s_matView, vertno, calcmode );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPosDouble : curhs GetScreenPosFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dscx = (double)scpos.x;
	double dscy = (double)scpos.y;
	double dscz = (double)scpos.z;
	code_setva( pvalscx, aptrscx, TYPE_DNUM, &dscx );
	code_setva( pvalscy, aptrscy, TYPE_DNUM, &dscy );
	code_setva( pvalscz, aptrscz, TYPE_DNUM, &dscz );

	return 0;
}

int E3DSetBillboardSize()// $1DF
{
	int bbid;
	bbid = code_getdi( 0 );

	float width;
	width = (float)code_getdd( 0.0 );

	float height;
	height = (float)code_getdd( 0.0 );

	int dirmode;
	dirmode = code_getdi( 0 );

	int orgflag;
	orgflag = code_getdi( 0 );

/////////////////

	int ret;
	ret = g_bbhs->SetBillboardSize( bbid, width, height, dirmode, orgflag );
	if( ret ){
		DbgOut( "E3DSetBillboardSize : hs SetBillboardSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetHeightFogParams()// $1E0
{
// 1 : enable
	int enable;
	enable = code_getdi(0);

// 2 : r
	int r;
	r = code_getdi(255);

// 3 : g
	int g;
	g = code_getdi(255);

// 4 : b
	int b;
	b = code_getdi(255);

// 5 : start
	float start;
	start = (float)code_getdd(0.0);

// 6 : end
	float end;
	end = (float)code_getdd(60000.0);

// 7 : srchsid
	int srchsid;
	srchsid = code_getdi( -1 );

//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* seths = 0;
	if( srchsid >= 0 ){
		seths = GetHandlerSet( srchsid );
		if( !seths ){
			DbgOut( "E3DSetLinearFogParams : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	r = max( 0, r );
	r = min( 255, r );

	g = max( 0, g );
	g = min( 255, g );

	b = max( 0, b );
	b = min( 255, b );

	
	if( seths == 0 ){
		if( enable ){
			g_fogenable = 1;
		}else{
			g_fogenable = 0;
		}
		g_fogcolor = D3DCOLOR_ARGB( 0, r, g, b );
		g_fogstart = start;
		g_fogend = end;
		g_fogtype = 1.0f;

		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pdev->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pdev->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pdev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pdev->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );

		int hsidno;
		for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
			if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
				CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
				ret = curhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
				_ASSERT( !ret );
			}
		}

		if( g_bbhs ){
			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );

		}

	}else{

		int tmpfogenable;
		D3DCOLOR tmpfogcolor;
		float tmpfogstart, tmpfogend;

		if( enable ){
			tmpfogenable = 1;
		}else{
			tmpfogenable = 0;
		}
		tmpfogcolor = D3DCOLOR_ARGB( 0, r, g, b );
		tmpfogstart = start;
		tmpfogend = end;
		g_fogtype = 1.0f;

		ret = seths->SetRenderState( -1, D3DRS_FOGENABLE, tmpfogenable );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGCOLOR, tmpfogcolor );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&tmpfogstart)) );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&tmpfogend)) );
		_ASSERT( !ret );

	}

	return 0;
}

int E3DSetTextureToBG()// $1E1
{
	int scid;
	scid = code_getdi(0);

	int stageno;
	stageno = code_getdi(0);

	int texid;
	texid = code_getdi(0);

/////////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}

	int ret;
	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToBG : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToBG : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToBG : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DSetTextureToBG : device not set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	BGDISPELEM* bgelem;
	bgelem = GetBGDispElem( scid );
	if( bgelem ){
		if( bgelem->bgdisp != 0 ){
			char* texname;
			int transparent;
			ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
			if( ret ){
				DbgOut( "E3DSetTextureToBG : texbank GetNameBySerial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( !texname ){
				DbgOut( "E3DSetTextureToBG : texid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			if( stageno == 0 ){
				strcpy_s( bgelem->bgdisp->texname, _MAX_PATH, texname );
				bgelem->bgdisp->transparent1 = transparent;
			}else{
				strcpy_s( bgelem->bgdisp->texname2, _MAX_PATH, texname );
				bgelem->bgdisp->transparent2 = transparent;
			}

			ret = bgelem->bgdisp->CreateTexture( pdev );
			if( ret ){
				DbgOut( "E3DSetTextureToBG : bgdisp CreateTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}

	return 0;
}

int E3DGetTextureFromBG()// $1E2
{
	int scid;
	scid = code_getdi(0);

	int stageno;
	stageno = code_getdi(0);

	PVal *pvalid;
	APTR aptrid;
	aptrid = code_getva( &pvalid );

////
	if( !g_texbnk ){
		DbgOut( "E3DGetTextureFromBG : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texid = -1;
	int texindex = -1;
	BGDISPELEM* bgelem;
	bgelem = GetBGDispElem( scid );
	if( bgelem && (bgelem->bgdisp != 0) ){
		if( stageno == 0 ){
			if( *(bgelem->bgdisp->texname) != 0 ){
				texindex = g_texbnk->FindName( bgelem->bgdisp->texname, bgelem->bgdisp->transparent1 );
				if( texindex >= 0 ){
					texid = g_texbnk->GetSerial( texindex );
				}else{
					texid = -1;
				}
			}else{
				texid = -1;
			}
		}else{
			if( *(bgelem->bgdisp->texname2) != 0 ){
				texindex = g_texbnk->FindName( bgelem->bgdisp->texname2, bgelem->bgdisp->transparent2 );
				if( texindex >= 0 ){
					texid = g_texbnk->GetSerial( texindex );
				}else{
					texid = -1;
				}
			}else{
				texid = -1;
			}
		}
	}else{
		texid = -1;
	}

	code_setva( pvalid, aptrid, TYPE_INUM, &texid );

	return 0;
}

int E3DChangeMoaIdling()// $1E3
{
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

////////
	if( motid <= 0 ){
		DbgOut( "E3DChangeMaoIdlilng : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChangeMoaIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->ChangeMoaIdling( motid );
	if( ret ){
		DbgOut( "E3DChangeMoaIdling : hs ChangeMoaIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMotionBlur()// $1E4
{
	int hsid;
	hsid = code_getdi(0);

	int mode;
	mode = code_getdi(0);

	int blurtime;
	blurtime = code_getdi(1);

/////////
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetMotinBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mode < 0 ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( mode >= BLUR_MAX ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( blurtime <= 0 ){
		blurtime = 1;
	}
	if( blurtime > BLURTIMEMAX ){
		blurtime = BLURTIMEMAX;
	}

	int ret;
	ret = curhs->SetMotionBlur( mode, blurtime );
	if( ret ){
		DbgOut( "E3DSetMotionBlur : hs SetMotionBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetBeforeBlur()// $1E5
{
	int hsid;
	hsid = code_getdi(0);
////////
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetBeforeBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetBeforeBlur();
	if( ret ){
		DbgOut( "E3DSetBeforeBlur : hs SetBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DRenderMotionBlur()// $1E6 
{

	int scid;
	scid = code_getdi(0);

	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	int num;
	num = code_getdi(0);

//////////////////////
	int ret;

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderMotionBlur : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int hsidno;
	for( hsidno = 0; hsidno < num; hsidno++ ){
		int curhsid;
		curhsid = *( (int*)pvalhsid->pt + aptrhsid + hsidno );

		CHandlerSet* curhs;
		if( curhsid != -1 ){
			curhs = GetHandlerSet( curhsid );
			if( !curhs ){
				DbgOut( "E3DRenderMotionBlur : hsid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			curhs = g_bbhs;
			if( !curhs ){
				DbgOut( "E3DRenderMotionBlur : hsid bb error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = curhs->RenderMotionBlur( pdev );
		if( ret ){
			DbgOut( "E3DRenderMotionBlur : hs RenderMotionBlur error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int E3DInitBeforeBlur()// $1E7
{
	int hsid;
	hsid = code_getdi(0);
////////
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DInitBeforeBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitBeforeBlur();
	if( ret ){
		DbgOut( "E3DInitBeforeBlur : hs InitBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DSetMotionBlurPart()// $1E8
{
	int hsid;
	hsid = code_getdi(0);

	int mode;
	mode = code_getdi(0);

	int blurtime;
	blurtime = code_getdi(1);

	PVal *pvalpart;
	APTR aptrpart;
	aptrpart = code_getva( &pvalpart );

	int num;
	num = code_getdi(0);

/////////////////
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetMotinBlurPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mode < 0 ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( mode >= BLUR_MAX ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( blurtime <= 0 ){
		blurtime = 1;
	}
	if( blurtime > BLURTIMEMAX ){
		blurtime = BLURTIMEMAX;
	}

	int ret;
	ret = curhs->SetMotionBlurPart( mode, blurtime, (int*)pvalpart->pt + aptrpart, num );
	if( ret ){
		DbgOut( "E3DSetMotionBlurPart : hs SetMotionBlurPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int RTBeginScene( LPDIRECT3DDEVICE9 pdev, int scid, int skipbeginscene, int needz )
{
	int ret;
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "RTBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	if( needz ){
		hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		hres = pdev->SetDepthStencilSurface( NULL );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( skipbeginscene == 0 ){
		if( FAILED( pdev->BeginScene() ) ){
			_ASSERT( 0 );
			return 1;
		}
	}

    // Clear the viewport
	if( needz ){
		hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
							 cursc->clearcol,
							 1.0f, 0L );
	}else{
		hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET,// | D3DCLEAR_ZBUFFER, 
							 cursc->clearcol,
							 1.0f, 0L );
	}
	if( hres != D3D_OK ){
		//_ASSERT( 0 );
	   DbgOut( "RTBeginScene : Clear error !!!\n" );
		return 1;
	}

	if( skipbeginscene == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}


	return 0;
}

int SetRTRenderState( LPDIRECT3DDEVICE9 pdev, int needz )
{
	if( needz ){
		pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
	}else{
		pdev->SetRenderState( D3DRS_ZENABLE, FALSE );
		g_renderstate[D3DRS_ZENABLE] = 0;	
	}


	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_cop1 != D3DTOP_DISABLE ){
		pdev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
		g_cop1 = D3DTOP_DISABLE;
	}
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}
	if( g_aop1 != D3DTOP_DISABLE ){
		pdev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
		g_aop1 = D3DTOP_DISABLE;
	}

	return 0;
}


int E3DBlurTexture16Box()// $1E9
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTexture16Box : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTexture16Box : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTexture16Box : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTexture16Box : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_pPostEffect != NULL )
	{

		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}
int E3DBlurTexture9Corn()// $1EA
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTexture9Corn : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;

	if( !g_texbnk ){
		DbgOut( "E3DBlurTexture9Corn : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTexture9Corn : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTexture9Corn : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------
		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}


int SetEffectDecl( LPDIRECT3DDEVICE9 pdev )
{
	D3DVERTEXELEMENT9 ve[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//tex1[2]//tex0
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};
	
	D3DVERTEXELEMENT9 vetlv[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//diffuse
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

		//tex0
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};


	if( s_effectdecl ){
		s_effectdecl->Release();
		s_effectdecl = 0;
	}
	if( s_tlvdecl ){
		s_tlvdecl->Release();
		s_tlvdecl = 0;
	}

	HRESULT hr;
	hr = pdev->CreateVertexDeclaration( ve, &s_effectdecl );
	if( hr != D3D_OK ){
		DbgOut( "SetEffectDecl : CreateVertexDeclaration effectdecl error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->CreateVertexDeclaration( vetlv, &s_tlvdecl );
	if( hr != D3D_OK ){
		DbgOut( "SetEffectDecl : CreateVertexDeclaration tlv error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int E3DBlurTextureGauss5x5()// $1EB
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	float dispersion;
	dispersion = (float)code_getdd(1.0);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss5x5 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTextureGauss5x5 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTextureGauss5x5 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss5x5 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetGauss5x5Weight( fw, fh, dispersion );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss5x5 : SetGauss5x5Weight error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 2 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------
		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}


		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}

int SetGauss5x5Weight( float fw, float fh, float dispersion )
{
	HRESULT hres;

	D3DXVECTOR4 avSampleWeight[16];
	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avSampleWeight, sizeof( D3DXVECTOR4 ) * 16 );
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
	
	float tu = 1.0f / fw;
    float tv = 1.0f / fh;

    float totalWeight = 0.0f;
    int index=0;
    for( int x = -2; x <= 2; x++ ) {
        for( int y = -2; y <= 2; y++ ) {
			// �W�����������Ȃ镔���͏���
            if( 2 < abs(x) + abs(y) ) continue;

            avTexCoordOffset[index] = D3DXVECTOR2( x * tu, y * tv );
			float fx = (FLOAT)x;
			float fy = (FLOAT)y;
			avSampleWeight[index].x = avSampleWeight[index].y =
			avSampleWeight[index].z = avSampleWeight[index].w
				= expf( -( fx * fx + fy * fy ) / ( 2.0f * dispersion ) );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

	// �d�݂̍��v�� 1.0f �ɂ���
	float divtotal = 1.0f / totalWeight;
	for( int i=0; i < index; i++ ){
		avSampleWeight[i] *= divtotal;
	}
	
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleWeight, avSampleWeight, sizeof( D3DXVECTOR4 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetGauss5x5Weight : sampleweight SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetGauss5x5Weight : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int SetDownScale4x4Offset( float fw, float fh )
{
	HRESULT hres;
    float tU = 1.0f / fw;
    float tV = 1.0f / fh;

	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );


    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index=0;
    for( int y=0; y < 4; y++ )
    {
        for( int x=0; x < 4; x++ )
        {
            avTexCoordOffset[ index ].x = ( (float)x - 1.5f) * tU;
            avTexCoordOffset[ index ].y = ( (float)y - 1.5f) * tV;
                                                      
            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetDownScale4x4Offset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int SetDownScale2x2Offset( float fw, float fh )
{
	HRESULT hres;
    float tU = 1.0f / fw;
    float tV = 1.0f / fh;

	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index=0;
    for( int y=0; y < 2; y++ )
    {
        for( int x=0; x < 2; x++ )
        {
            avTexCoordOffset[ index ].x = ( (float)x - 0.5f ) * tU;
            avTexCoordOffset[ index ].y = ( (float)y - 0.5f ) * tV;
                                                      
            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetDownScale2x2Offset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int SetToneMapFirstOffset( float fw, float fh )
{
    // Initialize the sample offsets for the initial luminance pass.

	HRESULT hres;
	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		
	float tU, tV;
    tU = 1.0f / (3.0f * fw);
    tV = 1.0f / (3.0f * fh);
    
    int index = 0;
	int x, y;
    for( x = -1; x <= 1; x++ )
    {
        for( y = -1; y <= 1; y++ )
        {
            avTexCoordOffset[index].x = x * tU;
            avTexCoordOffset[index].y = y * tV;

            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetToneMapFirstOffset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int SetGaussWeight( float dispersion )
{
    DWORD i;
	float tbl[8];

    float total=0.0f;
    for( i=0; i<8; i++ ){
        float pos = 1.0f + 2.0f * (float)i;
        tbl[i] = expf( -0.5f * (float)( pos * pos ) / dispersion );
        total += 2.0f * tbl[i];
    }
    // �K�i��
	for( i=0; i<8; i++ ){
		tbl[i] /= total;
	}

	if( g_pPostEffect){
		HRESULT hres;
		hres = g_pPostEffect->SetValue( g_hpeWeight, tbl, sizeof( float ) * 8 );
		if( hres != D3D_OK ){
			DbgOut( "SetGaussWeight : posteffect setvalue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int E3DBlurTextureGauss()// $1EC
{
	int finalscid;
	finalscid = code_getdi(0);

	int workscid;
	workscid = code_getdi(0);

	int worktexid;
	worktexid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	float dispersion;
	dispersion = (float)code_getdd(5.0);

	float colmult;
	colmult = (float)code_getdd(1.0);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

//////////////////
	int ret;
	HRESULT hres;

	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}



	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

/////////////
// �P�p�X��
	ret = RTBeginScene( pdev, workscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss : RTBeginScene workscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw1, screenh1;
	screenw1 = (float)g_scsize.x;
	screenh1 = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTextureGauss : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTextureGauss : srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss : GetTextureInfoBySerial srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectGauss );
		_ASSERT( hres == D3D_OK );

		ret = SetGaussWeight( dispersion );
		_ASSERT( !ret );


		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		D3DXVECTOR2 offsetX;
		offsetX.x = 16.0f / fw;
		offsetX.y = 0.0f / fh;
		hres = g_pPostEffect->SetValue( g_hpeOffsetX, &offsetX, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :offsetX SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeColMult, &colmult, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :ColMult SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );


		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw1;
			offsetv1 = 0.5f / screenh1;
		}
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

/////////////////////
// �Q�p�X��
		ret = RTBeginScene( pdev, finalscid, 1, 0 );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss : RTBeginScene finalscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float screenw2, screenh2;
		screenw2 = (float)g_scsize.x;
		screenh2 = (float)g_scsize.y;


		LPDIRECT3DTEXTURE9 ptex2 = 0;
		ret = g_texbnk->GetTexDataBySerial( worktexid, &ptex2 );
		if( ret || !ptex2 ){
			DbgOut( "E3DBlurTextureGauss : worktexid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int width2, height2, pool2, transparent2, format2;
		ret = g_texbnk->GetTextureInfoBySerial( worktexid, &width2, &height2, &pool2, &transparent2, &format2 );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss : GetTextureInfoBySerial worktexid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float fw2 = (float)width2;
		float fh2 = (float)height2;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw2, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :width0 2 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh2, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :height0 2 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :texture0 2 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		D3DXVECTOR2 offsetY;
		offsetY.x = 0.0f / fw;
		offsetY.y = 16.0f / fh;
		hres = g_pPostEffect->SetValue( g_hpeOffsetY, &offsetY, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :offsetY SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );


		float offsetu2, offsetv2;
		if( adjustuvflag == 0 ){
			offsetu2 = 0.0f;
			offsetv2 = 0.0f;
		}else if( adjustuvflag == 1 ){
			offsetu2 = 0.5f / screenw2;
			offsetv2 = 0.5f / screenh2;
		}
		TVERTEX Vertex2[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu2, 0.0f + offsetv2},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu2, 0.0f + offsetv2},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu2, 1.0f + offsetv2},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu2, 1.0f + offsetv2}
		};


		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex2, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

/////////////


		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}

int E3DBlendRTTexture()// $1ED
{
//E3DBlendRTTexture scid4, texid1, 1.0, texid3, 0.5, D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE
	int finalscid;
	finalscid = code_getdi(0);

	int texid1;
	texid1 = code_getdi(0);

	PVal *pvalcol1;
	APTR aptrcol1;
	aptrcol1 = code_getva( &pvalcol1 );

	int texf1;
	texf1 = code_getdi(D3DTEXF_POINT);

	int texid2;
	texid2 = code_getdi(0);

	PVal *pvalcol2;
	APTR aptrcol2;
	aptrcol2 = code_getva( &pvalcol2 );

	int texf2;
	texf2 = code_getdi(D3DTEXF_POINT);

	int blendop;
	blendop = code_getdi( D3DBLENDOP_ADD );

	int srcblend;
	srcblend = code_getdi( D3DBLEND_SRCALPHA );

	int destblend;
	destblend = code_getdi( D3DBLEND_ONE );

	int adjustuvflag;
	adjustuvflag = code_getdi( -1 );

/////////////////

	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	int needz = 0;
	ret = RTBeginScene( pdev, finalscid, 0, needz );
	if( ret ){
		DbgOut( "E3DBlendRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw1, screenh1;
	float offsetu1 = 0.0f;
	float offsetv1 = 0.0f;

	if( !g_texbnk ){
		DbgOut( "E3DBlendRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DTEXTURE9 ptex1 = 0;
	int width1, height1, pool1, transparent1, format1;
	if( texid1 >= 0 ){
		ret = g_texbnk->GetTexDataBySerial( texid1, &ptex1 );
		if( ret || !ptex1 ){
			DbgOut( "E3DBlendRTTexture : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = g_texbnk->GetTextureInfoBySerial( texid1, &width1, &height1, &pool1, &transparent1, &format1 );
		if( ret ){
			DbgOut( "E3DBlendRTTexture : GetTextureInfoBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		screenw1 = (float)width1;
		screenh1 = (float)height1;

		if( adjustuvflag < 0 ){
			if( texf1 == D3DTEXF_POINT ){
				offsetu1 = 0.5f / screenw1;
				offsetv1 = 0.5f / screenh1;
			}else{
				offsetu1 = 0.0f;
				offsetv1 = 0.0f;
			}
		}else if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw1;
			offsetv1 = 0.5f / screenh1;
		}
	}


	LPDIRECT3DTEXTURE9 ptex2 = 0;
	int width2, height2, pool2, transparent2, format2;
	if( texid2 >= 0 ){
		ret = g_texbnk->GetTexDataBySerial( texid2, &ptex2 );
		if( ret || !ptex2 ){
			DbgOut( "E3DBlendRTTexture : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = g_texbnk->GetTextureInfoBySerial( texid2, &width2, &height2, &pool2, &transparent2, &format2 );
		if( ret ){
			DbgOut( "E3DBlendRTTexture : GetTextureInfoBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, needz );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;
		
		pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

//		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
//
//		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;

		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectTLV );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );

	///////////
		if( texid1 >= 0 ){
			float fw1 = (float)width1;
			float fh1 = (float)height1;
			hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw1, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :width0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh1, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :height0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex1 );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :texture0 SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int passno1;
			if( texf1 == D3DTEXF_POINT ){
				passno1 = 0;
			}else{
				passno1 = 1;
			}

			hres = g_pPostEffect->BeginPass( passno1 );
			_ASSERT( hres == D3D_OK );


			float r1, g1, b1, a1;
			r1 = (float)( *((double*)pvalcol1->pt + aptrcol1) );
			g1 = (float)( *((double*)pvalcol1->pt + aptrcol1 + 1) );
			b1 = (float)( *((double*)pvalcol1->pt + aptrcol1 + 2) );
			a1 = (float)( *((double*)pvalcol1->pt + aptrcol1 + 3) );

			EFFECTTLV Vertex1[4] = {
				//   x    y     z      w     u0 v0
				{{-1.0f, +1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {0.0f + offsetu1, 0.0f + offsetv1} },
				{{+1.0f, +1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {1.0f + offsetu1, 0.0f + offsetv1} },
				{{+1.0f, -1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {1.0f + offsetu1, 1.0f + offsetv1} },
				{{-1.0f, -1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {0.0f + offsetu1, 1.0f + offsetv1} }
			};

			hres = pdev->SetVertexDeclaration( s_tlvdecl );
			_ASSERT( hres == D3D_OK );

			hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( EFFECTTLV ) );
			_ASSERT( hres == D3D_OK );

			hres = g_pPostEffect->EndPass();
			_ASSERT( hres == D3D_OK );

		}
////////////
		pdev->SetRenderState( D3DRS_BLENDOP, blendop );
		g_renderstate[ D3DRS_BLENDOP ] = blendop;

		pdev->SetRenderState( D3DRS_SRCBLEND, srcblend );
		g_renderstate[ D3DRS_SRCBLEND ] = srcblend;

		pdev->SetRenderState( D3DRS_DESTBLEND, destblend );
		g_renderstate[ D3DRS_DESTBLEND ] = destblend;

		float offsetu2 = 0.0f;
		float offsetv2 = 0.0f;

		if( texid2 >= 0 ){
			float fw2 = (float)width2;
			float fh2 = (float)height2;

			hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw2, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :width0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh2, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :height0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex2 );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :texture0 SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int passno2;
			if( texf2 == D3DTEXF_POINT ){
				passno2 = 0;
			}else{
				passno2 = 1;
			}
			float offsetu2, offsetv2;
			if( adjustuvflag < 0 ){
				if( texf2 == D3DTEXF_POINT ){
					offsetu2 = 0.5f / fw2;
					offsetv2 = 0.5f / fh2;
				}else{
					offsetu2 = 0.0f;
					offsetv2 = 0.0f;
				}
			}else if( adjustuvflag == 0 ){
				offsetu2 = 0.0f;
				offsetv2 = 0.0f;

			}else if( adjustuvflag == 1 ){
				offsetu2 = 0.5f / fw2;
				offsetv2 = 0.5f / fh2;
			}
			hres = g_pPostEffect->BeginPass( passno2 );
			_ASSERT( hres == D3D_OK );

			float r2, g2, b2, a2;
			r2 = (float)( *((double*)pvalcol2->pt + aptrcol1) );
			g2 = (float)( *((double*)pvalcol2->pt + aptrcol1 + 1) );
			b2 = (float)( *((double*)pvalcol2->pt + aptrcol1 + 2) );
			a2 = (float)( *((double*)pvalcol2->pt + aptrcol1 + 3) );

			EFFECTTLV Vertex2[4] = {
				//   x    y     z      w     u0 v0
				{{-1.0f, +1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {0.0f + offsetu2, 0.0f + offsetv2} },
				{{+1.0f, +1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {1.0f + offsetu2, 0.0f + offsetv2} },
				{{+1.0f, -1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {1.0f + offsetu2, 1.0f + offsetv2} },
				{{-1.0f, -1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {0.0f + offsetu2, 1.0f + offsetv2} }
			};

			
			hres = pdev->SetVertexDeclaration( s_tlvdecl );
			_ASSERT( hres == D3D_OK );

			hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( EFFECTTLV ) );
			_ASSERT( hres == D3D_OK );

			hres = g_pPostEffect->EndPass();
			_ASSERT( hres == D3D_OK );
		}
////////////

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}

int E3DSetClearCol()// $1EE
{
	int scid;
	scid = code_getdi(0);

	int a;
	a = code_getdi(0);

	int r;
	r = code_getdi(0);

	int g;
	g = code_getdi(0);

	int b;
	b = code_getdi(0);

//////////////
	a = min( 255, a );
	a = max( 0, a );

	r = min( 255, r );
	r = max( 0, r );

	g = min( 255, g );
	g = max( 0, g );

	b = min( 255, b );
	b = max( 0, b );

	if( scid < 0 ){
		DbgOut( "E3DSetClearCol : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DSetClearCol : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	cursc->clearcol = D3DCOLOR_ARGB( a, r, g, b );


	return 0;
}
int E3DSetMotionBlurMinAlpha()// $1EF
{
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	float minalpha;
	minalpha = (float)code_getdd(0.2);

///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionBlurMinAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionBlurMinAlpha( partno, minalpha );
	if( ret ){
		DbgOut( "E3DSetMotionBlurMinAlpha : hs SetMotionBlurMinAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DRenderGlow()// $1F0
{
//1: hsidptr
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

//2: arrayleng
	int arrayleng;
	arrayleng = code_getdi(0);

//////////
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );


//	pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//	g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;


	//�s�����O���[�̕`��
	int hsno;
	int withalpha;
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		int curhsid;
		curhsid = *( (int*)pvalhsid->pt + aptrhsid + hsno );//!!!!!!!!!!!!!!!!!!

		CHandlerSet* curhs;
		curhs = GetHandlerSet( curhsid );
		if( !curhs ){
			DbgOut( "E3DRenderGlow : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		withalpha = 0;
		ret = curhs->RenderGlow( pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			DbgOut( "E3DRenderGlow : hs RenderGlow 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	//�������O���[�̕`��
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		int curhsid;
		curhsid = *( (int*)pvalhsid->pt + aptrhsid + hsno );//!!!!!!!!!!!!!!!!!!

		CHandlerSet* curhs;
		curhs = GetHandlerSet( curhsid );
		if( !curhs ){
			DbgOut( "E3DRenderGlow : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		withalpha = 1;
		ret = curhs->RenderGlow( pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			DbgOut( "E3DRenderGlow : hs RenderGlow 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}
int E3DSetMaterialGlowParams()// $1F1
{

	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int glowmode;
	glowmode = code_getdi(0);

	int setcolflag;
	setcolflag = code_getdi(1);

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );

///////////////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DCOLOR4F tempcol;
	tempcol.a = 1.0f;
	tempcol.r = (float)*( (double*)pvalcol->pt + aptrcol );
	tempcol.g = (float)*( (double*)pvalcol->pt + aptrcol + 1 );
	tempcol.b = (float)*( (double*)pvalcol->pt + aptrcol + 2 );

	ret = curhs->SetMaterialGlowParams( matno, glowmode, setcolflag, &tempcol );
	if( ret ){
		DbgOut( "E3DSetMaterialGlowParams : hs SetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
int E3DGetMaterialGlowParams()// $1F4
{
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvalmode;
	APTR aptrmode;
	aptrmode = code_getva( &pvalmode );

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DCOLOR4F tempcol;
	tempcol.a = 1.0f;
	tempcol.r = 1.0f;
	tempcol.g = 1.0f;
	tempcol.b = 1.0f;

	int ret;
	ret = curhs->GetMaterialGlowParams( matno, (int*)pvalmode->pt + aptrmode, &tempcol );
	if( ret ){
		DbgOut( "E3DGetMaterialGlowParams : hs GetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double dr, dg, db;
	dr = tempcol.r;
	dg = tempcol.g;
	db = tempcol.b;

	code_setva( pvalcol, aptrcol, TYPE_DNUM, &dr );
	code_setva( pvalcol, aptrcol + 1, TYPE_DNUM, &dg );
	code_setva( pvalcol, aptrcol + 2, TYPE_DNUM, &db );

	return 0;
}
int E3DGetMaterialTransparent()// $1F2
{
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	PVal *pvaltra;
	APTR aptrtra;
	aptrtra = code_getva( &pvaltra );

////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialTransparent : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int transparent = 0;
	int ret;
	ret = curhs->GetMaterialTransparent( matno, &transparent );
	if( ret ){
		DbgOut( "E3DGetMaterialTransparent : hs GetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltra, aptrtra, TYPE_INUM, &transparent );

	return 0;
}
int E3DSetMaterialTransparent()// $1F3
{
	int hsid;
	hsid = code_getdi(0);

	int matno;
	matno = code_getdi(0);

	int transparent;
	transparent = code_getdi(0);

/////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialTransparent( matno, transparent );
	if( ret ){
		DbgOut( "E3DSetMaterialTransparent : hs SetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DChkHDR()// $1F5
{

	PVal *pvalok;
	APTR aptrok;
	aptrok = code_getva( &pvalok );

	PVal *pvalhdrfmt;
	APTR aptrhdrfmt;
	aptrhdrfmt = code_getva( &pvalhdrfmt );

	PVal *pvallumifmt;
	APTR aptrlumifmt;
	aptrlumifmt = code_getva( &pvallumifmt );

//////////////
	int okflag = 0;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );// !!!!!!!!!!!

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	int hdrfmt = 0;
	int lumifmt = 0;
	ret = g_pD3DApp->CheckHDR( &okflag, &hdrfmt, &lumifmt );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );
	code_setva( pvalhdrfmt, aptrhdrfmt, TYPE_INUM, &hdrfmt );
	code_setva( pvallumifmt, aptrlumifmt, TYPE_INUM, &lumifmt );

	return 0;
}
int E3DSetHDRLightDiffuse()// $1F6
{
	int lid;
	lid = code_getdi(0);

	float r;
	r = (float)code_getdd(1.0);
	float g;
	g = (float)code_getdd(1.0);
	float b;
	b = (float)code_getdd(1.0);
///////////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetHDRLightDiffuse : lightID not found error !!!\n" );
		return 1;
	}

	CVec3f lcol;
	lcol.x = r;
	lcol.y = g;
	lcol.z = b;

	curlight->Diffuse = lcol;

////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : E3DSetHDRLightDiffuse : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}
int E3DSetHDRLightSpecular()// $1F7
{
	int lid;
	lid = code_getdi(0);

	float r;
	r = (float)code_getdd(1.0);
	float g;
	g = (float)code_getdd(1.0);
	float b;
	b = (float)code_getdd(1.0);
///////////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetHDRLightSpecular : lightID not found error !!!\n" );
		return 1;
	}

	CVec3f lcol;
	lcol.x = r;
	lcol.y = g;
	lcol.z = b;

	curlight->Specular = lcol;

////////

	//resotre�ł����s����

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : E3DSetHDRLightSpecular : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}
int E3DDownScaleRTTexture4x4()// $1F8
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture4x4 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DDownScaleRTTexture4x4 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DDownScaleRTTexture4x4 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture4x4 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectDownScale );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "E3DDownScaleRTTexture4x4 : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}
int E3DDownScaleRTTexture2x2()// $1F9
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture2x2 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DDownScaleRTTexture2x2 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DDownScaleRTTexture2x2 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture2x2 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectDownScale );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale2x2Offset( fw, fh );
		if( ret ){
			DbgOut( "E3DDownScaleRTTexture2x2 : SetDownScale2x2Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}
int E3DCreateToneMapTexture()// $1FA
{
	int tonenum;
	tonenum = code_getdi(3);

	int tonefmt;
	tonefmt = code_getdi(D3DFMT_R32F);

	PVal *pvaltoneid;
	APTR aptrtoneid;
	aptrtoneid = code_getva( &pvaltoneid );

	PVal *pvalok;
	APTR aptrok;
	aptrok = code_getva( &pvalok );

////////
	int ret;
	int okflag = 0;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );

	TONEMAPELEM* newtm = 0;
	newtm = GetFreeToneMapElem();
	if( !newtm ){
		DbgOut( "E3DCreateToneMapTexture : tonemap memory overflow error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (tonefmt != D3DFMT_R32F) && (tonefmt != D3DFMT_R16F) ){
		_ASSERT( 0 );
		tonefmt = D3DFMT_R32F;
	}

	if( tonenum < 3 ){
		_ASSERT( 0 );
		tonenum = 3;
	}

	int* pscid = 0;
	pscid = (int*)malloc( sizeof( int ) * tonenum );
	if( !pscid ){
		DbgOut( "E3DCreateToneMapTexture : pscid alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	ZeroMemory( pscid, sizeof( int ) * tonenum );

	int* ptexid = 0;
	ptexid = (int*)malloc( sizeof( int ) * tonenum );
	if( !ptexid ){
		DbgOut( "E3DCreateToneMapTexture : ptexid alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	ZeroMemory( ptexid, sizeof( int ) * tonenum );

	int allocno;
	for( allocno = 0; allocno < tonenum; allocno++ ){
		int texsize = 1 << ( 2 * allocno );
		ret = CreateRenderTargetTexture( texsize, texsize, tonefmt, pscid + allocno, ptexid + allocno );
		if( ret ){
			DbgOut( "E3DCreateToneMapTexture : CreateRenderTargetTexture %d error !!!\n", allocno );
			_ASSERT( 0 );
			free( pscid );
			free( ptexid );
			return 0;
		}
	}
	
	newtm->tonenum = tonenum;
	newtm->pscid = pscid;
	newtm->ptexid = ptexid;

	code_setva( pvaltoneid, aptrtoneid, TYPE_INUM, &(newtm->tonemapid) );
	okflag = 1;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );

	return 0;
}
int E3DRenderToneMap()// $1FB
{
	int tonemapid;
	tonemapid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

//////////////
	int ret;

	if( !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DRenderToneMap : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DRenderToneMap : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int curtmno = tmptr->tonenum - 1;
	int curscid = *( tmptr->pscid + curtmno );
	int curtexid = srctexid;
	ret = ToneMapFirst( curscid, curtexid );
	if( ret ){
		DbgOut( "E3DRenderToneMap : ToneMapFirst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curtmno--;
	while( curtmno > 0 ){
		curscid = *( tmptr->pscid + curtmno );
		curtexid = *( tmptr->ptexid + curtmno + 1 );
		ret = ToneMapSecond( curscid, curtexid );
		if( ret ){
			DbgOut( "E3DRenderToneMap : ToneMapSecond error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curtmno--;
	}

	curtmno = 0;
	curscid = *( tmptr->pscid + 0 );
	curtexid = *( tmptr->ptexid + 1 );
	ret = ToneMapFinal( curscid, curtexid );
	if( ret ){
		DbgOut( "E3DRenderToneMap : ToneMapFinal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DBrightPassRTTextureHDR()// $1FC
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	float middlegray;
	middlegray = (float)code_getdd(1.0);

	float threshold;
	threshold = (float)code_getdd(5.0);

	float offset;
	offset = (float)code_getdd(10.0);

	int tonemapid;
	tonemapid = code_getdi(0);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

///////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	if( !g_texbnk ){
		DbgOut( "E3DBrightPassRTTextureHDR : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBrightPassRTTextureHDR : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBrightPassRTTextureHDR : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( tmptr->tonenum <= 0 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonenum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex1 = 0;
	ret = g_texbnk->GetTexDataBySerial( *( tmptr->ptexid + 0 ), &ptex1 );
	if( ret || !ptex1 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tone texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBrightPassRTTextureHDR : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBrightPass );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture1, ptex1 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :texture1 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->SetValue( g_hpeMiddleGray, &middlegray, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :middlegray SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeBrightPassThreshold, &threshold, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :threshold SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeBrightPassOffset, &offset, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :BrightPassOffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = SetGauss5x5Weight( fw, fh, 10.0f );
		if( ret ){
			DbgOut( "E3DBrightPassRTTextureHDR : SetGauss5x5Weight error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int E3DDestroyToneMapTexture()// $1FD
{
	int tonemapid;
	tonemapid = code_getdi(0);
////////

	int ret;
	if( tonemapid < 0 ){
		_ASSERT( 0 );
		return 0;
	}

	TONEMAPELEM* findelem = 0;
	findelem = GetToneMapElem( tonemapid );
	if( !findelem ){
		_ASSERT( 0 );
		return 0;
	}

	int tmno;
	for( tmno = 0; tmno < findelem->tonenum; tmno++ ){
		int curscid, curtexid;
		curscid = *( findelem->pscid + tmno );
		curtexid = *( findelem->ptexid + tmno );
		ret = DestroyRenderTargetTexture( curscid, curtexid );
		if( ret ){
			DbgOut( "E3DDestroyToneMapTexture : DestroyRenderTargetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( findelem->pscid ){
		free( findelem->pscid );
		findelem->pscid = 0;
	}
	if( findelem->ptexid ){
		free( findelem->ptexid );
		findelem->ptexid = 0;
	}
	findelem->tonemapid = -1;
	findelem->tonenum = 0;


	return 0;
}


TONEMAPELEM* GetFreeToneMapElem()
{
	int tmno;
	TONEMAPELEM* findelem = 0;

	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;

		if( curtm->tonemapid < 0 ){
			s_usertonemapcnt++;
			curtm->tonemapid = s_usertonemapcnt;
			curtm->tonenum = 0;
			if( curtm->pscid ){
				free( curtm->pscid );
				curtm->pscid = 0;
			}
			if( curtm->ptexid ){
				free( curtm->ptexid );
				curtm->ptexid = 0;
			}

			findelem = curtm;
			break;
		}
	}

	return findelem;
}
TONEMAPELEM* GetToneMapElem( int tonemapid )
{
	int tmno;
	TONEMAPELEM* findelem = 0;

	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;
		if( curtm->tonemapid == tonemapid ){
			findelem = curtm;
			break;
		}
	}
	return findelem;
}
int ToneMapFirst( int dstscid, int srctexid )
{
//g_hPostEffectToneMap
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapFirst : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapFirst : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneMapFirst : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneMapFirst : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetToneMapFirstOffset( screenw, screenh );
		if( ret ){
			DbgOut( "ToneMapFirst : SetToneMapFirstOffset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int ToneMapSecond( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapSecond : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapSecond : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneSecond : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneSecond : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "ToneMapSecond : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int ToneMapFinal( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapFinal : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapFinal : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneMapFinal : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneMapFinal : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "ToneMapFinal : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 2 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}

int E3DRenderHDRFinal()// $1FE
{	
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid0;
	srctexid0 = code_getdi(0);

	int srctexid1;
	srctexid1 = code_getdi(0);

	int tonemapid;
	tonemapid = code_getdi(0);

	float middlegray;
	middlegray = (float)code_getdd(1.0);

/////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	if( !g_texbnk ){
		DbgOut( "E3DRenderHDRFinal : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex0 = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid0, &ptex0 );
	if( ret || !ptex0 ){
		DbgOut( "E3DRenderHDRFinal : texid0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex1 = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid1, &ptex1 );
	if( ret || !ptex1 ){
		DbgOut( "E3DRenderHDRFinal : texid1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DRenderHDRFinal : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DRenderHDRFinal : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( tmptr->tonenum <= 0 ){
		DbgOut( "E3DRenderHDRFinal : tonenum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex2 = 0;
	ret = g_texbnk->GetTexDataBySerial( *( tmptr->ptexid + 0 ), &ptex2 );
	if( ret || !ptex2 ){
		DbgOut( "E3DRenderHDRFinal : tone texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DRenderHDRFinal : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectHDRFinal );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex0 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture1, ptex1 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture1 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture2, ptex2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture2 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->SetValue( g_hpeMiddleGray, &middlegray, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :middlegray SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}

int E3DDisablePostEffect()// $1FF
{
	g_usePostEffect = 0;

	return 0;
}
int E3DCheckSpec()// $200
{
	int wid;
	wid = code_getdi(0);

	PVal *pvalok;
	APTR aptrok;
	aptrok = code_getva( &pvalok );
/////////
	int okflag = 0;
	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );

	HINSTANCE hinst = 0;
	HWND hwnd = 0;

	BMSCR* bm;
	bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
	if( !bm ){
		DbgOut( "E3DCheckSpec : bmscr NULL : window id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	hinst = (HINSTANCE)(bm->hInst);
	hwnd = bm->hwnd;
	_ASSERT( hinst );
	_ASSERT( hwnd );

	CD3DApplication* tmpapp;
	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DCheckSpec : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = tmpapp->CheckSpec( hinst, hwnd, &okflag );
	if( ret ){
		DbgOut( "E3DCheckSpec : app CheckSpec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalok, aptrok, TYPE_INUM, &okflag );
	delete tmpapp;

	return 0;
}

int E3DMonoRTTexture()// $201
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DMonoRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DMonoRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DMonoRTTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DMonoRTTexture : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectMono );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int E3DCbCrRTTexture()// $202
{
	int dstscid;
	dstscid = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);

	float srccb;
	srccb = (float)code_getdd(-0.2);

	float srccr;
	srccr = (float)code_getdd(0.1);

	int adjustuvflag;
	adjustuvflag = code_getdi(0);

///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DCbCrRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DCbCrRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DCbCrRTTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DCbCrRTTexture : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectCbCr );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpcbcr[2];
		tmpcbcr[0] = srccb;
		tmpcbcr[1] = srccr;
		hres = g_pPostEffect->SetValue( g_hpeCbCr, tmpcbcr, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :CbCr SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}

int E3DShaderConstOffset16()// $203
{
	PVal *pvaloff;
	APTR aptroff;
	aptroff = code_getva( &pvaloff );

///////////
	D3DXVECTOR2 tmpoffset[16];
	ZeroMemory( tmpoffset, sizeof( D3DXVECTOR2 ) * 16 );

	double* topptr = (double*)pvaloff->pt + aptroff;

	int i;
	for( i = 0; i < 16; i++ ){
		tmpoffset[i].x = (float)(*(topptr + 2 * i));
		tmpoffset[i].y = (float)(*(topptr + 2 * i + 1));
	}

	HRESULT hres;
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, tmpoffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "E3DShaderConstOffset16 : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DShaderConstWeight16()// $204
{
	PVal *pvalw;
	APTR aptrw;
	aptrw = code_getva( &pvalw );

///////////
	D3DXVECTOR4 tmpweight[16];
	ZeroMemory( tmpweight, sizeof( D3DXVECTOR4 ) * 16 );

	double* topptr = (double*)pvalw + aptrw;

	int i;
	for( i = 0; i < 16; i++ ){
		tmpweight[i].x = (float)(*(topptr + 4 * i));
		tmpweight[i].y = (float)(*(topptr + 4 * i + 1));
		tmpweight[i].z = (float)(*(topptr + 4 * i + 2));
		tmpweight[i].w = (float)(*(topptr + 4 * i + 3));
	}

	HRESULT hres;
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleWeight, tmpweight, sizeof( D3DXVECTOR4 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "E3DShaderConstWeight16 : sampleweight SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DShaderConstUserFL4()// $205
{
	int datano;
	datano = code_getdi(0);

	PVal *pvalv;
	APTR aptrv;
	aptrv = code_getva( &pvalv );
////////////////
	if( (datano < 0) || (datano > 9) ){
		DbgOut( "E3DShaderConstUserFL4 : datano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pPostEffect ){
		DbgOut( "E3DShaderConstUserFL4 : posteffect NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	double* valptr = (double*)pvalv->pt + aptrv;
	float tmpval[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	int i;
	for( i = 0; i < 4; i++ ){
		tmpval[i] = (float)(*(valptr + i));
	}

	HRESULT hres;
	hres = g_pPostEffect->SetValue( g_hpeUserFl4[datano], tmpval, sizeof( float ) * 4 );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstUserFL4 : fl4 SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DShaderConstUserTex()// $206
{
	int datano;
	datano = code_getdi(0);

	int srctexid;
	srctexid = code_getdi(0);
///////////
	int ret;
	HRESULT hres;

	if( !g_pPostEffect ){
		DbgOut( "E3DShaderConstUserTex : posteffect NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (datano < 0) || (datano > 4) ){
		DbgOut( "E3DShaderConstUserTex : datano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DShaderConstUserTex : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DShaderConstUserTex : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	hres = g_pPostEffect->SetTexture( g_hpeUserTex[datano], ptex );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstUserTex :tex SetTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DCallUserShader()// $207
{
	int shaderno;
	shaderno = code_getdi(0);

	int passno;
	passno = code_getdi(0);

	int dstscid;
	dstscid = code_getdi(0);
///////////////////
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	if( (shaderno < 0) || (shaderno > 4) ){
		DbgOut( "E3DCallUserShader : shaderno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DCallUserShader : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// �V�F�[�_�̐ݒ�
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectUser[shaderno] );
		_ASSERT( hres == D3D_OK );

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		UINT passnum = 0;
		hres = g_pPostEffect->Begin( &passnum, 0 );
		_ASSERT( hres == D3D_OK );

		if( passno >= (int)passnum ){
			DbgOut( "E3DCallUserShader : passno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->BeginPass( passno );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// �t�B���^�����O
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}

int E3DCreateBone()// $208
{
	int ret;

	int hsid;
	hsid = code_getdi(0);

	char tempname[256];
	ret = GetName( tempname, 256 );
	if( ret ){
		DbgOut( "E3DCreateBone : sig name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int parentid;
	parentid = code_getdi(-1);

	PVal *pvalseri;
	APTR aptrseri;
	aptrseri = code_getva( &pvalseri );

///////////////
	int newseri = -1;
	code_setva( pvalseri, aptrseri, TYPE_INUM, &newseri );

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateBone : pdev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tempname[0] == 0 ){
		DbgOut( "E3DCreateBone : bonename NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CreateBone( pdev, s_hwnd, tempname, parentid, &newseri );
	if( ret || (newseri <= 0) ){
		DbgOut( "E3DCreateBone : hs CreateBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalseri, aptrseri, TYPE_INUM, &newseri );

	return 0;
}
int E3DDestroyBone()// $209
{
	int hsid;
	hsid = code_getdi(0);

	int boneno;
	boneno = code_getdi(0);

////////////
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DDestroyBone : pdev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyBone( pdev, s_hwnd, boneno );
	if( ret ){
		DbgOut( "E3DDestroyBone : hs DestroyBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetMorphBaseNum()// $20A
{
//E3DGetMorphBaseNum hsid, basenum
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
/////////
	int basenum = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &basenum );

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphBaseNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphBaseNum( &basenum );
	if( ret ){
		DbgOut( "E3DGetMorphBaseNum : hs GetMorphBaseNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &basenum );

	return 0;
}
int E3DGetMorphBaseInfo()// $20B
{
//E3DGetMorphBaseInfo hsid, arrayleng, basename, baseinfo
//sdim basename, 256, basenum
//#define global MBI_DISPNO		0
//#define global MBI_BONENO		1
//#define global MBI_TARGETNUM	2
//#define global MBI_MAX			3
//dim baseinfo, MBI_MAX, basenum

	int hsid;
	hsid = code_getdi(0);

	int arrayleng;
	arrayleng = code_getdi(1);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	PVal *pvalinfo;
	APTR aptrinfo;
	aptrinfo = code_getva( &pvalinfo );
/////////////
	if( arrayleng <= 0 ){
		DbgOut( "E3DGetMorphBaseInfo : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DMORPHBASE* baseinfoptr;
	baseinfoptr = (E3DMORPHBASE*)malloc( sizeof( E3DMORPHBASE ) * arrayleng );
	if( !baseinfoptr ){
		DbgOut( "E3DGetMorphBaseInfo : baseinfoptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( baseinfoptr, sizeof( E3DMORPHBASE ) * arrayleng );

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphBaseInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		free( baseinfoptr );
		return 1;
	}

	ret = curhs->GetMorphBaseInfo( baseinfoptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphBaseInfo : hs GetMorphBaseInfo error !!!\n" );
		_ASSERT( 0 );
		free( baseinfoptr );
		return 1;
	}


	int* dstinfoptr = (int*)pvalinfo->pt + aptrinfo;
	int basecnt;
	for( basecnt = 0; basecnt < arrayleng; basecnt++ ){
		E3DMORPHBASE* srcinfo = baseinfoptr + basecnt;

		code_setva( pvalname, aptrname + basecnt, TYPE_STRING, srcinfo->name );
		*(dstinfoptr + MBI_MAX * basecnt + MBI_DISPNO) = srcinfo->dispno;
		*(dstinfoptr + MBI_MAX * basecnt + MBI_BONENO) = srcinfo->boneno;
		*(dstinfoptr + MBI_MAX * basecnt + MBI_TARGETNUM) = srcinfo->targetnum;
	}

	free( baseinfoptr );

	return 0;
}
int E3DGetMorphTargetInfo()// $20C
{
//E3DGetMorphTargetInfo hsid, baseid, arrayleng, targetname, targetdispno
//sdim targetname, 256, targetnum
//dim targetdispno, targetnum

	int hsid;
	hsid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int arrayleng;
	arrayleng = code_getdi(1);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
////////////////

	int ret;

	if( arrayleng <= 0 ){
		DbgOut( "E3DGetMorphTargetInfo : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DMORPHTARGET* targetinfoptr;
	targetinfoptr = (E3DMORPHTARGET*)malloc( sizeof( E3DMORPHTARGET ) * arrayleng );
	if( !targetinfoptr ){
		DbgOut( "E3DMorphTargetInfo : targetinfoptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( targetinfoptr, sizeof( E3DMORPHTARGET ) * arrayleng );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphTargetInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		free( targetinfoptr );
		return 1;
	}

	ret = curhs->GetMorphTargetInfo( baseid, targetinfoptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphTargetInfo : hs GetMorphTargetInfo error !!!\n" );
		_ASSERT( 0 );
		free( targetinfoptr );
		return 1;
	}

	int* dstnoptr = (int*)pvalno->pt + aptrno;
	int targetcnt;
	for( targetcnt = 0; targetcnt < arrayleng; targetcnt++ ){
		E3DMORPHTARGET* srcinfo = targetinfoptr + targetcnt;

		code_setva( pvalname, aptrname + targetcnt, TYPE_STRING, srcinfo->name );
		*(dstnoptr + targetcnt) = srcinfo->dispno;
	}


	free( targetinfoptr );

	return 0;
}
int E3DGetMorphKeyNum()// $20D
{
//E3DGetMorphKeyNum hsid, motid, boneid, baseid, targetid, keynum

	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int targetid;
	targetid = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
///////////////
	int keynum = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &keynum );

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphKeyNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphKeyNum( motid, boneid, baseid, targetid, &keynum );
	if( ret ){
		DbgOut( "E3DGetMorphKeyNum : hs GetMorphKeyNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &keynum );

	return 0;
}
int E3DGetMorphKey()// $20E
{
//E3DGetMorphKey hsid, motid, boneid, baseid, targetid, arrayleng, framenoarray, ratearray
//dim framenoarray, keynum
//ddim ratearray, keynum

	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int targetid;
	targetid = code_getdi(0);

	int arrayleng;
	arrayleng = code_getdi(1);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	PVal *pvalrate;
	APTR aptrrate;
	aptrrate = code_getva( &pvalrate );
////////////////

	int ret;
	if( arrayleng <= 0 ){
		DbgOut( "E3DGetMorphKey : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DMORPHMOTION* motionptr;
	motionptr = (E3DMORPHMOTION*)malloc( sizeof( E3DMORPHMOTION ) * arrayleng );
	if( !motionptr ){
		DbgOut( "E3DGetMorphKey : motionptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( motionptr, sizeof( E3DMORPHMOTION ) * arrayleng );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		free( motionptr );
		return 1;
	}

	ret = curhs->GetMorphKey( motid, boneid, baseid, targetid, motionptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphKey : hs GetMorphKey error !!!\n" );
		_ASSERT( 0 );
		free( motionptr );
		return 1;
	}

	int* topframe = (int*)pvalframe->pt + aptrframe;
	double* toprate = (double*)pvalrate->pt + aptrrate;
	int keycnt;
	for( keycnt = 0; keycnt < arrayleng; keycnt++ ){
		E3DMORPHMOTION* curmotion = motionptr + keycnt;

		*( topframe + keycnt ) = curmotion->frameno;
		*( toprate + keycnt ) = (double)curmotion->blendrate;
	}

	free( motionptr );

	return 0;
}
int E3DGetCurMorphInfo()// $20F
{
//E3DGetCurMorphInfo hsid, boneid, baseid, targetid, frameno, rate

	int hsid;
	hsid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int targetid;
	targetid = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	PVal *pvalrate;
	APTR aptrrate;
	aptrrate = code_getva( &pvalrate );
////////////////////

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCurMorphInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DMORPHMOTION motioninfo;
	ZeroMemory( &motioninfo, sizeof( E3DMORPHMOTION ) );

	ret = curhs->GetCurMorphInfo( boneid, baseid, targetid, &motioninfo );
	if( ret ){
		DbgOut( "E3DGetCurMorphInfo : hs GetCurMorphInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* topframe = (int*)pvalframe->pt + aptrframe;
	double* toprate = (double*)pvalrate->pt + aptrrate;
	*topframe = motioninfo.frameno;
	*toprate = (double)motioninfo.blendrate;

	return 0;
}
int E3DCreateMorph()// $210
{
//E3DCreateMorph hsid, baseid, boneid

	int hsid;
	hsid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);
/////////////////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateMorph : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
	}
	if( !pdev ){
		DbgOut( "E3DCreateMorph : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CreateMorph( pdev, s_hwnd, baseid, boneid );
	if( ret ){
		DbgOut( "E3DCreateMorph : hs CreateMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DAddMorphTarget()// $211
{
//E3DAddMorphTarget hsid, baseid, targetid

	int hsid;
	hsid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int targetid;
	targetid = code_getdi(0);
//////////////////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMorphTarget : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->AddMorphTarget( baseid, targetid );
	if( ret ){
		DbgOut( "E3DAddMorphTarget : hs AddMorphTarget error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMorphKey()// $212
{
//E3DSetMorphKey hsid, motid, boneid, baseid, targetid, frameno, rate
	
	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);

	int targetid;
	targetid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	float rate;
	rate = (float)code_getdd(0.0);
///////////////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	E3DMORPHMOTION motioninfo;
	motioninfo.frameno = frameno;
	motioninfo.blendrate = rate;

	ret = curhs->SetMorphKey( motid, boneid, baseid, targetid, &motioninfo );
	if( ret ){
		DbgOut( "E3DSetMorphKey : hs SetMorphKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DDestroyMorphKey()// $213
{
//E3DDestroyMorphKey hsid, motid, boneid, frameno, baseid

	int hsid;
	hsid = code_getdi(0);

	int motid;
	motid = code_getdi(0);

	int boneid;
	boneid = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int baseid;
	baseid = code_getdi(0);
/////////////////////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyMorphKey( motid, boneid, frameno, baseid );
	if( ret ){
		DbgOut( "E3DDestroyMorphKey : hs DestroyMorphKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetToon1MaterialNum()// $214
{
	//E3DGetToon1MaterialNum hsid, partno, matnum

	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
/////////
	int matnum = 0;

	code_setva( pvalnum, aptrnum, TYPE_INUM, &matnum );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToon1MaterialNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetToon1MaterialNum( partno, &matnum );
	if( ret ){
		DbgOut( "E3DGetToon1MaterialNum : hs GetToon1MaterialNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &matnum );

	return 0;
}
int E3DGetToon1Material()// $215
{

	//sdim matname, 32, matnum
	//ddim matdinfo, TMD_MAX, matnum
	//dim matiinfo, TMI_MAX, matnum
	//E3DGetToon1Material hsid, partno, matname, matdinfo, matiinfo, arrayleng
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	PVal *pvaldinfo;
	APTR aptrdinfo;
	aptrdinfo = code_getva( &pvaldinfo );

	PVal *pvaliinfo;
	APTR aptriinfo;
	aptriinfo = code_getva( &pvaliinfo );

	int arrayleng;
	arrayleng = code_getdi(1);
////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToon1Material : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( arrayleng <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	E3DTOON1MATERIAL* toon1ptr;
	toon1ptr = (E3DTOON1MATERIAL*)malloc( sizeof( E3DTOON1MATERIAL ) * arrayleng );
	if( !toon1ptr ){
		DbgOut( "E3DGetToon1Material : toon1ptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( toon1ptr, sizeof( E3DTOON1MATERIAL ) * arrayleng );

	ret = curhs->GetToon1Material( partno, toon1ptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetToon1Material : hs GetToon1Material error !!!\n" );
		_ASSERT( 0 );
		free( toon1ptr );
		return 1;
	}

	double* firstdi = (double*)pvaldinfo->pt + aptrdinfo;
	int* firstii = (int*)pvaliinfo->pt + aptriinfo;
	int matno;
	for( matno = 0; matno < arrayleng; matno++ ){
		double* curdi = firstdi + matno * TMD_MAX;
		int* curii = firstii + matno * TMI_MAX;
		E3DTOON1MATERIAL* curtoon1 = toon1ptr + matno;

		code_setva( pvalname, aptrname + matno, TYPE_STRING, curtoon1->name );


		*(curdi + TMD_DIFFUSE_R) = (double)curtoon1->diffuse.r;
		*(curdi + TMD_DIFFUSE_G) = (double)curtoon1->diffuse.g;
		*(curdi + TMD_DIFFUSE_B) = (double)curtoon1->diffuse.b;
		
		*(curdi + TMD_SPECULAR_R) = (double)curtoon1->specular.r;
		*(curdi + TMD_SPECULAR_G) = (double)curtoon1->specular.g;
		*(curdi + TMD_SPECULAR_B) = (double)curtoon1->specular.b;

		*(curdi + TMD_AMBIENT_R) = (double)curtoon1->ambient.r;
		*(curdi + TMD_AMBIENT_G) = (double)curtoon1->ambient.g;
		*(curdi + TMD_AMBIENT_B) = (double)curtoon1->ambient.b;

		*(curdi + TMD_DARKNL) = (double)curtoon1->darknl;
		*(curdi + TMD_BRIGHTNL) = (double)curtoon1->brightnl;

		*(curdi + TMD_EDGECOL0_R) = (double)curtoon1->edgecol0.r;
		*(curdi + TMD_EDGECOL0_G) = (double)curtoon1->edgecol0.g;
		*(curdi + TMD_EDGECOL0_B) = (double)curtoon1->edgecol0.b;

		*(curdi + TMD_EDGEWIDTH0) = (double)curtoon1->edgewidth0;

		*(curii + TMI_VALID0) = curtoon1->edgevalid0;
		*(curii + TMI_INV0) = curtoon1->edgeinv0;
	}

	free( toon1ptr );
//#define global TMD_DIFFUSE_R		0
//#define global TMD_DIFFUSE_G		1
//#define global TMD_DIFFUSE_B		2
//#define global TMD_SPECULAR_R	3
//#define global TMD_SPECULAR_G	4
//#define global TMD_SPECULAR_B	5
//#define global TMD_AMBIENT_R		6
//#define global TMD_AMBIENT_G		7
//#define global TMD_AMBIENT_B		8
//#define global TMD_DARKNL		9
//#define global TMD_BRIGHTNL		10
//#define global TMD_EDGECOL0_R	11
//#define global TMD_EDGECOL0_G	12
//#define global TMD_EDGECOL0_B	13
//#define global TMD_EDGEWIDTH0	14
//#define global TMD_MAX			15
//
//#define global TMI_VALID0		0
//#define global TMI_INV0			1
//#define global TMI_MAX			2

	return 0;
}
int E3DSetToon1Name()// $216
{
	//E3DSetToon1Name hsid, partno, oldname, newname
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char oldname[32];
	ret = GetName( oldname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Name : oldname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char newname[32];
	ret = GetName( newname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Name : newname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Name : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Name( partno, oldname, newname );
	if( ret ){
		DbgOut( "E3DSetToon1Name : hs SetToon1Name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetToon1Diffuse()// $217
{
	//E3DSetToon1Diffuse hsid, partno, matname, col
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char matname[32];
	ret = GetName( matname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Diffuse : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );
///////////////
	double* srccolptr = (double*)pvalcol->pt + aptrcol;
	RDBColor3f col;
	col.r = (float)(*(srccolptr + 0));
	col.g = (float)(*(srccolptr + 1));
	col.b = (float)(*(srccolptr + 2));

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Diffuse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Diffuse( partno, matname, col );
	if( ret ){
		DbgOut( "E3DSetToon1Diffuse : hs SetToon1Diffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}
int E3DSetToon1Specular()// $218
{
	//E3DSetToon1Specular hsid, partno, matname, col
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char matname[32];
	ret = GetName( matname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Specular : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );
///////////////
	double* srccolptr = (double*)pvalcol->pt + aptrcol;
	RDBColor3f col;
	col.r = (float)(*(srccolptr + 0));
	col.g = (float)(*(srccolptr + 1));
	col.b = (float)(*(srccolptr + 2));

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Specular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Specular( partno, matname, col );
	if( ret ){
		DbgOut( "E3DSetToon1Specular : hs SetToon1Specular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetToon1Ambient()// $219
{
	//E3DSetToon1Ambient hsid, partno, matname, col
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char matname[32];
	ret = GetName( matname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Ambient : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );
///////////////
	double* srccolptr = (double*)pvalcol->pt + aptrcol;
	RDBColor3f col;
	col.r = (float)(*(srccolptr + 0));
	col.g = (float)(*(srccolptr + 1));
	col.b = (float)(*(srccolptr + 2));

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Ambient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Ambient( partno, matname, col );
	if( ret ){
		DbgOut( "E3DSetToon1Ambient : hs SetToon1Ambient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int E3DSetToon1NL()// $21A
{
	//E3DSetToon1NL hsid, partno, matname, darknl, brightnl
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char matname[32];
	ret = GetName( matname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1NL : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float darknl;
	darknl = (float)code_getdd(0.0);

	float brightnl;
	brightnl = (float)code_getdd(0.0);
//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1NL : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1NL( partno, matname, darknl, brightnl );
	if( ret ){
		DbgOut( "E3DSetToon1NL : hs SetToon1NL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetToon1Edge0()// $21B
{
	//E3DSetToon1Edge0 hsid, partno, matname, col, validflag, invflag, width
	int ret;
	int hsid;
	hsid = code_getdi(0);

	int partno;
	partno = code_getdi(0);

	char matname[32];
	ret = GetName( matname, 32 );
	if( ret ){
		DbgOut( "E3DSetToon1Diffuse : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalcol;
	APTR aptrcol;
	aptrcol = code_getva( &pvalcol );

	int validflag;
	validflag = code_getdi(1);

	int invflag;
	invflag = code_getdi(0);

	float width;
	width = (float)code_getdd(8.0);
	
///////////////
	double* srccolptr = (double*)pvalcol->pt + aptrcol;
	RDBColor3f col;
	col.r = (float)(*(srccolptr + 0));
	col.g = (float)(*(srccolptr + 1));
	col.b = (float)(*(srccolptr + 2));

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Edge0 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Edge0( partno, matname, col, validflag, invflag, width );
	if( ret ){
		DbgOut( "E3DSetToon1Edge0 : hs SetToon1Edge0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DLoadCameraFile()//$21D
{
//E3DLoadCameraFile( char* filename, int* animnoptr );
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadCameraFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
//////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	_ASSERT( m_cah );
	CCameraFile camfile;
	ret = camfile.LoadCameraFile( tempname, m_cah, &animno );
	if( ret || (animno < 0) ){
		DbgOut( "E3DLoadCameraFile : camerafile LoadCameraFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	return 0;
}
int E3DDestroyCameraAnim()//$21E
{
//E3DDestroyCameraAnim( int animno );
	int animno;
	animno = code_getdi(0);

//////////
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->DestroyAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyCameraAnim : cah DestroyAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSaveCameraFile()//$21F
{
//E3DSaveCameraFile( char* filename, int animno );
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadCameraFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animno;
	animno = code_getdi(0);
//////////

	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DSaveCameraFile : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSaveCameraFile : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	CCameraFile camfile;
	ret = camfile.WriteCameraFile( tempname, anim );
	if( ret ){
		DbgOut( "E3DSaveCameraFile : camfile WriteCameraFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetCameraAnimNo()//$220
{
//E3DSetCameraAnimNo( int animno );
	int animno;
	animno = code_getdi(0);
/////////

	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetCurrentAnim( animno );
	if( ret ){
		DbgOut( "E3DSetCameraAnimNo : cah SetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );

	return 0;
}
int E3DGetCameraAnimNo()//$221
{
//E3DGetCameraAnimNo( int* animnoptr );
	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
//////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DGetCameraAnimNo : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	animno = m_cah->m_animno;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DSetCameraFrameNo()//$222
{
//E3DSetCameraFrameNo( int animno, int camerano, int frameno, int isfirst );
	int animno;
	animno = code_getdi(0);

	int camerano;
	camerano = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int isfirst;
	isfirst = code_getdi(0);
////////////

	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetAnimFrameNo( animno, camerano, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetCameraFrameNo : cah SetAinmFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );

	return 0;
}
int E3DGetCameraFrameNo()//$223
{
//E3DGetCameraFrameNo( int* animnoptr, int* camnotpr, int* framenoptr );
	PVal *pvalanim;
	APTR aptranim;
	aptranim = code_getva( &pvalanim );

	PVal *pvalcam;
	APTR aptrcam;
	aptrcam = code_getva( &pvalcam );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

//////////

	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DGetCameraFrameNo : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	int animno, camno, frameno;
	ret = m_cah->GetAnimFrameNo( &animno, &camno, &frameno );
	if( ret ){
		DbgOut( "E3DGetCameraFrameNo : cah GetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalanim, aptranim, TYPE_INUM, &animno );
	code_setva( pvalcam, aptrcam, TYPE_INUM, &camno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &frameno );

	return 0;
}
int E3DSetNewCamera()//$224
{
//E3DSetNewCamera( int camno );
	int camno;
	camno = code_getdi(-1);
////////

	_ASSERT( m_cah );
	int ret;
	if( m_cah->m_animnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}
	ret = m_cah->SetNewCamera( camno );
	if( ret ){
		DbgOut( "E3DSetNewCamera : cah SetNewCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );

	return 0;
}
int E3DSetNextCameraAnimFrameNo()//$225
{
//E3DSetNextCameraAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno );
	int animno;
	animno = code_getdi(0);

	int nextanimno;
	nextanimno = code_getdi(0);

	int nextframeno;
	nextframeno = code_getdi(0);

	int befframeno;
	befframeno = code_getdi(-1);
////////////


	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetNextAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextCameraAnimFrameNo : cah SetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetNextCameraAnimFrameNo()//$226
{
//E3DGetNextCameraAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr );
	int animno;
	animno = code_getdi(0);

	PVal *pvalanim;
	APTR aptranim;
	aptranim = code_getva( &pvalanim );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
//////////

	_ASSERT( m_cah );
	int ret;
	int nextanim, nextframe;
	ret = m_cah->GetNextAnimFrameNo( animno, &nextanim, &nextframe );
	if( ret ){
		DbgOut( "E3DGetNextCameraAnimFrameNo : cah GetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalanim, aptranim, TYPE_INUM, &nextanim );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &nextframe );

	return 0;
}
int E3DCreateNewCameraAnim()//$227
{
//E3DCreateNewCameraAnim( char* animname, int animtype, int framenum, int* animnoptr );
	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DCreateNewCameraAnim : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animtype;
	animtype = code_getdi(0);

	int framenum;
	framenum = code_getdi(60);

	PVal *pvalanim;
	APTR aptranim;
	aptranim = code_getva( &pvalanim );

///////////
	int animno = -1;
	code_setva( pvalanim, aptranim, TYPE_INUM, &animno );

	_ASSERT( m_cah );
	CCameraAnim* newanim;
	newanim = m_cah->AddAnim( tempname, animtype, framenum, 0 );
	if( !newanim ){
		DbgOut( "E3DCreatenewCameraAnim : cah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	animno = newanim->m_motkind;
	code_setva( pvalanim, aptranim, TYPE_INUM, &animno );

	return 0;
}
int E3DSetCameraFrameNum()//$228
{
//E3DSetCameraFrameNum( int animno, int newtotal );
	int animno;
	animno = code_getdi(0);

	int newtotal;
	newtotal = code_getdi(60);
////////////

	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetTotalFrame( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetCameraFrameNum : cah SetTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraFrameNum()//$229
{
//E3DGetCameraFrameNum( int animno, int* framenumptr );
	int animno;
	animno = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
/////////

	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraFrameNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int framenum = anim->m_maxframe + 1;
	code_setva( pvalframe, aptrframe, TYPE_INUM, &framenum );

	return 0;
}
int E3DSetCameraAnimName()//$22A
{
//E3DSetCameraAnimName( int animno, char* srcname );
	int animno;
	animno = code_getdi(0);

	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSetCameraAnimName : animname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////////

	_ASSERT( m_cah );
	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = anim->SetAnimName( tempname );
	if( ret ){
		DbgOut( "E3DSetCameraAnimName : anim SetAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraAnimName()//$22B
{
//E3DGetCameraAnimName( int animno, char* dstname, int arrayleng );
	int animno;
	animno = code_getdi(0);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	int arrayleng;
	arrayleng = code_getdi(0);


	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = strcpy_s( (char*)pvalname->pt + aptrname, arrayleng, anim->m_animname );
	_ASSERT( !ret );

	return 0;
}
int E3DSetCameraAnimType()//$22C
{
//E3DSetCameraAnimType( int animno, int type );
	int animno;
	animno = code_getdi(0);

	int type;
	type = code_getdi(0);
/////////

	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	anim->m_animtype = type;

	return 0;
}
int E3DGetCameraAnimType()//$22D
{
//E3DGetCameraAnimType( int animno, int* typeptr );
	int animno;
	animno = code_getdi(0);

	PVal *pvaltype;
	APTR aptrtype;
	aptrtype = code_getva( &pvaltype );
////////

	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int type = anim->m_animtype;
	code_setva( pvaltype, aptrtype, TYPE_INUM, &type );

	return 0;
}
int E3DSetCameraKey()//$22E
{
//E3DSetCameraKey animno, camno, frameno, cami, camd
	//camno -1�֎~

	int animno;
	animno = code_getdi(0);

	int camno;
	camno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalcami;
	APTR aptrcami;
	aptrcami = code_getva( &pvalcami );

	PVal *pvalcamd;
	APTR aptrcamd;
	aptrcamd = code_getva( &pvalcamd );
///////////

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* camiptr = (int*)pvalcami->pt + aptrcami;
	double* camdptr = (double*)pvalcamd->pt + aptrcamd;

	CAMERAELEM ce;
	ce.looktype = *( camiptr + CAMI_LOOKTYPE );
	ce.pos.x = (float)( *( camdptr + CAMD_POSX ) );
	ce.pos.y = (float)( *( camdptr + CAMD_POSY ) );
	ce.pos.z = (float)( *( camdptr + CAMD_POSZ ) );
	ce.target.x = (float)( *( camdptr + CAMD_TARX ) );
	ce.target.y = (float)( *( camdptr + CAMD_TARY ) );
	ce.target.z = (float)( *( camdptr + CAMD_TARZ ) );
	ce.up.x = (float)( *( camdptr + CAMD_UPX ) );
	ce.up.y = (float)( *( camdptr + CAMD_UPY ) );
	ce.up.z = (float)( *( camdptr + CAMD_UPZ ) );
	ce.dist = (float)( *( camdptr + CAMD_DIST ) );
	ce.hsid = *( camiptr + CAMI_HSID );
	ce.boneno = *( camiptr + CAMI_BONENO );
	ce.nearz = (float)( *( camdptr + CAMD_NEARZ ) );
	ce.farz = (float)( *( camdptr + CAMD_FARZ ) );
	ce.fov = (float)( *( camdptr + CAMD_FOV ) );
	
	int interp = *( camiptr + CAMI_INTERP );

	ret = anim->SetCameraKey( camno, frameno, ce, interp );
	if( ret ){
		DbgOut( "E3DSetCameraKey : anim SetCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraKey()//$22F
{
//E3DGetCameraKey animno, camno, frameno, cami, camd, existptr
	int animno;
	animno = code_getdi(0);

	int camno;
	camno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalcami;
	APTR aptrcami;
	aptrcami = code_getva( &pvalcami );

	PVal *pvalcamd;
	APTR aptrcamd;
	aptrcamd = code_getva( &pvalcamd );

	PVal *pvalexist;
	APTR aptrexist;
	aptrexist = code_getva( &pvalexist );

///////////
	int existflag = 0;
	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMERAELEM ce;
	ZeroMemory( &ce, sizeof( CAMERAELEM ) );
	int interp;
	ret = anim->GetCameraAnim( &ce, &interp, camno, frameno, &existflag );
	if( ret ){
		DbgOut( "E3DGetCameraKey : anim GetCameraAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* camiptr = (int*)pvalcami->pt + aptrcami;
	double* camdptr = (double*)pvalcamd->pt + aptrcamd;

	*( camiptr + CAMI_LOOKTYPE ) = ce.looktype;
	*( camdptr + CAMD_POSX ) = (double)ce.pos.x;
	*( camdptr + CAMD_POSY ) = (double)ce.pos.y;
	*( camdptr + CAMD_POSZ ) = (double)ce.pos.z;
	*( camdptr + CAMD_TARX ) = (double)ce.target.x;
	*( camdptr + CAMD_TARY ) = (double)ce.target.y;
	*( camdptr + CAMD_TARZ ) = (double)ce.target.z;
	*( camdptr + CAMD_UPX ) = (double)ce.up.x;
	*( camdptr + CAMD_UPY ) = (double)ce.up.y;
	*( camdptr + CAMD_UPZ ) = (double)ce.up.z;
	*( camdptr + CAMD_DIST ) = (double)ce.dist;
	*( camiptr + CAMI_HSID ) = ce.hsid;
	*( camiptr + CAMI_BONENO ) = ce.boneno;
	*( camdptr + CAMD_NEARZ ) = (double)ce.nearz;
	*( camdptr + CAMD_FARZ ) = (double)ce.farz;
	*( camdptr + CAMD_FOV ) = (double)ce.fov;
	
	*( camiptr + CAMI_INTERP ) = interp;
	*( camiptr + CAMI_ORTHO ) = 0;
	*( camdptr + CAMD_ORTHOSIZE ) = 3000.0;

	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );


	return 0;
}
int E3DDeleteCameraKey()//$230
{
//E3DDeleteCameraKey( int animno, int camno, int frameno );
	int animno;
	animno = code_getdi(0);

	int camno;
	camno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);
//////////////


	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DDeleteCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->DeleteCameraKey( camno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteCameraKey : anim DeleteCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraKeyframeNoRange()//$231
{
//E3DGetCameraKeyframeNoRange( int animno, int camno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
	int animno;
	animno = code_getdi(0);

	int camno;
	camno = code_getdi(0);

	int start;
	start = code_getdi(0);

	int end;
	end = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	int arrayleng;
	arrayleng = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
////////////
	int getnum = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraKeyframeNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* framearray;
	framearray = (int*)pvalframe->pt + aptrframe;

	ret = anim->GetCameraKeyframeNoRange( camno, start, end, framearray, arrayleng, &getnum );
	if( ret ){
		DbgOut( "E3DGetCameraKeyframeNoRange : anim GetCameraKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}
int E3DSetCameraSWKey()//$232
{
//E3DSetCameraSWKey( int animno, int frameno, int camno );
	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int camno;
	camno = code_getdi(0);
///////////

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->SetSWKey( frameno, camno );
	if( ret ){
		DbgOut( "E3DSetCameraSWKey : anim SetSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraSWKey()//$233
{
//E3DGetCameraSWKey( int animno, int frameno, int* camnoptr, int* existptr );
	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalcam;
	APTR aptrcam;
	aptrcam = code_getva( &pvalcam );

	PVal *pvalexist;
	APTR aptrexist;
	aptrexist = code_getva( &pvalexist );
///////////////
	int camno = 0;
	int existflag = 0;
	code_setva( pvalcam, aptrcam, TYPE_INUM, &camno );
	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetSWAnim( &camno, frameno, &existflag );
	if( ret ){
		DbgOut( "E3DGetCameraSWKey : anim GetSWAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalcam, aptrcam, TYPE_INUM, &camno );
	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );

	return 0;
}
int E3DDeleteCameraSWKey()//$234
{
//E3DDeleteCameraSWKey( int animno, int frameno );
	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

///////////

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DDeleteCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->DeleteSWKey( frameno );
	if( ret ){
		DbgOut( "E3DDeleteCameraSWKey : anim DeleteSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetCameraSWKeyframeNoRange()//$235
{
//E3DGetCameraSWKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr );
	int animno;
	animno = code_getdi(0);

	int start;
	start = code_getdi(0);

	int end;
	end = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	int arrayleng;
	arrayleng = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
////////////
	int getnum = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraSWKeyframeNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* framearray;
	framearray = (int*)pvalframe->pt + aptrframe;

	ret = anim->GetSWKeyframeNoRange( start, end, framearray, arrayleng, &getnum );
	if( ret ){
		DbgOut( "E3DGetCameraSWKeyframeNoRange : anim GetSWKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );

	return 0;
}
int CameraElemToParams( CAMERAELEM ce )
{
	s_cameratype = 3;
	s_camerapos = ce.pos;
	s_cameratarget = ce.target;
	s_cameraupvec = ce.up;
	s_camera_twist = 0.0f;

	D3DXMATRIX tmpview;
	D3DXMatrixLookAtLH( &tmpview, &s_camerapos, &s_cameratarget, &s_cameraupvec );
	
	tmpview._41 = 0.0f;
	tmpview._42 = 0.0f;
	tmpview._43 = 0.0f;

	D3DXQUATERNION qx;
	D3DXQuaternionRotationMatrix( &qx, &tmpview );
	s_cameraq.x = qx.x;
	s_cameraq.y = qx.y;
	s_cameraq.z = qx.z;
	s_cameraq.w = qx.w;

	g_proj_near = ce.nearz;
	g_proj_far = ce.farz;
	g_proj_fov = ce.fov;
	g_ortho = ce.ortho;
	g_orthosize = ce.orthosize;

	return 0;
}

int E3DSetOrtho() //$236
{

	float nearclip;
	nearclip = (float)code_getdd(0.0);

	float farclip;
	farclip = (float)code_getdd(0.0);

	float orthosize;
	orthosize = (float)code_getdd(0.0);

////////
	g_ortho = 1;//!!!!!!!!!!!!!!!
	g_proj_near = nearclip;
	g_proj_far = farclip;
	g_orthosize = orthosize;

	if( g_proj_fov == 0.0f )
		g_proj_fov = 45.0f; //default�l
	
	if( g_orthosize < 0.0f )
		g_orthosize = 3000.0f;

	if( g_proj_near >= g_proj_far ){
		g_proj_near = 100.0f; //default
		g_proj_far = 10000.0f;//default
	}

	return 0;
}


int E3DLoadGPFile()
{
	//( int hsid, const char* filename, int* animnoptr )
	int ret;

	int hsid;
	hsid = code_getdi(0);

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadGPFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
//////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadGPFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->LoadGPFile( tempname, &animno );
	if( ret ){
		DbgOut( "E3DLoadGPFile : hs LoadGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DDestroyGPAnim()
{
	//( int hsid, int animno )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);
/////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyGPAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyGPAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyGPAnim : hs DestroyGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSaveGPFile()
{
	//( int hsid, const char* filename, int animno )
	int ret;

	int hsid;
	hsid = code_getdi(0);

	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3D : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animno;
	animno = code_getdi(0);
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveGPFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SaveGPFile( tempname, animno );
	if( ret ){
		DbgOut( "E3DSaveGPFile : hs SaveGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetGPAnimNo()
{
	//( int hsid, int animno )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPAnimNo( animno );
	if( ret ){
		DbgOut( "E3DSetGPAnimNo : hs SetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetGPAnimNo()
{
	//( int hsid, int* animnoptr )
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
///////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPAnimNo( &animno );
	if( ret ){
		DbgOut( "E3DGetGPAnimNo : hs GetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DSetGPFrameNo()
{
	//( int hsid, int animno, int frameno, int isfirst )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int isfirst;
	isfirst = code_getdi(0);
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPFrameNo( animno, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetGPFrameNo : hs SetGPFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetGPFrameNo()
{
	//( int hsid, int* animnoptr, int* framenoptr )
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	int animno, frameno;
	ret = curhs->GetGPFrameNo( &animno, &frameno );
	if( ret ){
		DbgOut( "E3DGetGPGFrameNo : hs GetGPFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &frameno );	

	return 0;
}
int E3DSetNewGP()
{
	//( int hsid )
	int hsid;
	hsid = code_getdi(0);

///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewGP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewGP();
	if( ret ){
		DbgOut( "E3DSetNewGP : hs SetNewGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetNextGPAnimFrameNo()
{
	//( int hsid, int animno, int nextanimno, int nextframeno, int befframeno )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int nextanimno;
	nextanimno = code_getdi(0);

	int nextframeno;
	nextframeno = code_getdi(0);

	int befframeno;
	befframeno = code_getdi(-1);
///////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextGPAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextGPAnimFarmeNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextGPAnimFrameNo : hs SetNextGPAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetNextGPAnimFrameNo()
{
	//( int hsid, int animno, int* nextanimptr, int* nextframenoptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

/////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetNextGPanimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int nextanim, nextframe;
	ret = curhs->GetNextGPAnimFrameNo( animno, &nextanim, &nextframe );
	if( ret ){
		DbgOut( "E3DGetNextGPAnimFrameNo : hs GetNextGPAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &nextanim );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &nextframe );

	return 0;
}
int E3DCreateNewGPAnim()
{
	//( int hsid, const char* animname, int animtype, int framenum, int* animnoptr )
	int ret;
	int hsid;
	hsid = code_getdi(0);
	
	char tempname[256];
	ret = GetName( tempname, 256 );
	if( ret ){
		DbgOut( "E3D : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animtype;
	animtype = code_getdi(0);

	int framenum;
	framenum = code_getdi(1);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNewGPAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animno = -1;
	ret = curhs->CreateNewGPAnim( tempname, animtype, framenum, &animno );
	if( ret ){
		DbgOut( "E3DCreateNewGPAnim : hs CreateNewGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DSetGPFrameNum()
{
	//( int hsid, int animno, int newtotal )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int newtotal;
	newtotal = code_getdi(1);

///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetGPFrameNum( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetGPFrameNum : hs SetGPFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetGPFrameNum()
{
	//( int hsid, int animno, int* framenumptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int framenum;
	ret = curhs->GetGPFrameNum( animno, &framenum );
	if( ret ){
		DbgOut( "E3DGetGPFrameNum : hs GetGPFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalframe, aptrframe, TYPE_INUM, &framenum );


	return 0;
}
int E3DSetGPAnimName()
{
	//( int hsid, int animno, char* srcname )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int ret;
	char tempname[256];
	ret = GetName( tempname, 256 );
	if( ret ){
		DbgOut( "E3D : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetGPAnimName( animno, tempname );
	if( ret ){
		DbgOut( "E3DSetGPAnimName : hs SetGPAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetGPAnimName()
{
	//( int hsid, int animno, char* dstname, int arrayleng )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	int arrayleng;
	arrayleng = code_getdi(0);
//////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	char animname[256];
	ZeroMemory( animname, sizeof( char ) * 256 );
	ret = curhs->GetGPAnimName( animno, animname, 256 );
	if( ret ){
		DbgOut( "E3DGetGPAnimName : hs GetGPAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* dstptr = (char*)pvalname->pt + aptrname;
	strcpy_s( dstptr, arrayleng, animname );

	return 0;
}
int E3DSetGPAnimType()
{
	//( int hsid, int animno, int type )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int type;
	type = code_getdi(0);
////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPAnimType( animno, type );
	if( ret ){
		DbgOut( "E3DSetGPAnimType : hs SetGPAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetGPAnimType()
{
	//( int hsid, int animno, int* typeptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvaltype;
	APTR aptrtype;
	aptrtype = code_getva( &pvaltype );
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int type;
	ret = curhs->GetGPAnimType( animno, &type );
	if( ret ){
		DbgOut( "E3DGetGPAnimType : hs GetGPAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltype, aptrtype, TYPE_INUM, &type );

	return 0;
}
int E3DSetGPKey()
{
	//( int hsid, int animno, int frameno, E3DGPSTATE gpstate )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalgpi;
	APTR aptrgpi;
	aptrgpi = code_getva( &pvalgpi );

	PVal *pvalgpd;
	APTR aptrgpd;
	aptrgpd = code_getva( &pvalgpd );
////////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* gpiptr = (int*)pvalgpi->pt + aptrgpi;
	double* gpdptr = (double*)pvalgpd->pt + aptrgpd;
	E3DGPSTATE gps;
	gps.pos.x = (float)(*( gpdptr + GPD_POSX ));
	gps.pos.y = (float)(*( gpdptr + GPD_POSY ));
	gps.pos.z = (float)(*( gpdptr + GPD_POSZ ));
	gps.rot.x = (float)(*( gpdptr + GPD_ROTX ));
	gps.rot.y = (float)(*( gpdptr + GPD_ROTY ));
	gps.rot.z = (float)(*( gpdptr + GPD_ROTZ ));
	gps.ongmode = *( gpiptr + GPI_ONGMODE );
	gps.rayy = (float)(*( gpdptr + GPD_RAYY ));
	gps.rayleng = (float)(*( gpdptr + GPD_RAYLENG ));
	gps.offsety = (float)(*( gpdptr + GPD_OFFSETY ));
	gps.interp = *( gpiptr + GPI_INTERP );

	int ret;
	ret = curhs->SetGPKey( animno, frameno, gps );
	if( ret ){
		DbgOut( "E3DSetGPKey : hs SetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetGPKey()
{
	//( int hsid, int animno, int frameno, E3DGPSTATE* gpstateptr, int* existptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalgpi;
	APTR aptrgpi;
	aptrgpi = code_getva( &pvalgpi );

	PVal *pvalgpd;
	APTR aptrgpd;
	aptrgpd = code_getva( &pvalgpd );

	PVal *pvalexist;
	APTR aptrexist;
	aptrexist = code_getva( &pvalexist );
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	E3DGPSTATE gps;
	ZeroMemory( &gps, sizeof( E3DGPSTATE ) );
	int existflag = 0;
	ret = curhs->GetGPKey( animno, frameno, &gps, &existflag );
	if( ret ){
		DbgOut( "E3DGetGPKey : hs GetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );
	
	int* gpiptr = (int*)pvalgpi->pt + aptrgpi;
	double* gpdptr = (double*)pvalgpd->pt + aptrgpd;
	
	*( gpdptr + GPD_POSX ) = (double)gps.pos.x;
	*( gpdptr + GPD_POSY ) = (double)gps.pos.y;
	*( gpdptr + GPD_POSZ ) = (double)gps.pos.z;
	*( gpdptr + GPD_ROTX ) = (double)gps.rot.x;
	*( gpdptr + GPD_ROTY ) = (double)gps.rot.y;
	*( gpdptr + GPD_ROTZ ) = (double)gps.rot.z;
	*( gpiptr + GPI_ONGMODE ) = gps.ongmode;
	*( gpdptr + GPD_RAYY ) = (double)gps.rayy;
	*( gpdptr + GPD_RAYLENG ) = (double)gps.rayleng;
	*( gpdptr + GPD_OFFSETY ) = (double)gps.offsety;
	*( gpiptr + GPI_INTERP ) = gps.interp;

	return 0;
}
int E3DDeleteGPKey()
{
	//( int hsid, int animno, int frameno )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteGPKey( animno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteGPKey : hs DeleteGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetGPKeyframeNoRange()
{
	//( int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int start;
	start = code_getdi(0);

	int end;
	end = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	int arrayleng;
	arrayleng = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
///////////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int* framearray = (int*)pvalframe + aptrframe;
	int getnum = 0;
	ret = curhs->GetGPKeyframeNoRange( animno, start, end, framearray, arrayleng, &getnum );
	if( ret ){
		DbgOut( "E3DGetGPKeyframeNoRange : hs GetGPKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );
	
	return 0;
}
int E3DCalcGPFullFrame()
{
	//( int hsid, int animno )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);
////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcGPFullFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcGPFullFrame( animno );
	if( ret ){
		DbgOut( "E3DCalcGPFullFrame : hs CalcGPFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetGPGroundHSID()
{
	//( int hsid, int animno, int ghsid )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int ghsid;
	ghsid = code_getdi(0);
////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPGroundHSID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPGroundHSID( animno, ghsid );
	if( ret ){
		DbgOut( "E3DSetGPGroundHSID : hs SetGPGroundHSID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int E3DGetGPGroundHSID()
{
	//( int hsid, int animno, int* ghsidptr )
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPGroundHSID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int ghsid = 0;
	ret = curhs->GetGPGroundHSID( animno, &ghsid );
	if( ret ){
		DbgOut( "E3DGetGPGroundHSID : hs GetGPGroundHSID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &ghsid );
	
	return 0;
}


int E3DLoadMOEFile()
{
	//int hsid, const char* filename, int* animnoptr
	int hsid;
	hsid = code_getdi(0);

	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DLoadMOEFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
////////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadMOEFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->LoadMOEFile( tempname, &animno );
	if( ret ){
		DbgOut( "E3D : hs LoadMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DDestroyMOEAnim()
{
	// int hsid, int animno 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);
////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyMOEAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyMOEAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyMOEAnim : hs DestroyMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSaveMOEFile()
{
	// int hsid, const char* filename, int animno 
	int hsid;
	hsid = code_getdi(0);

	int ret;
	char tempname[MAX_PATH];
	ret = GetName( tempname, MAX_PATH );
	if( ret ){
		DbgOut( "E3DSaveMOEFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animno;
	animno = code_getdi(0);
////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMOEFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SaveMOEFile( tempname, animno );
	if( ret ){
		DbgOut( "E3DSaveMOEFile : hs SaveMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMOEAnimNo()
{
	// int hsid, int animno 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEAnimNo( animno );
	if( ret ){
		DbgOut( "E3DSetMOEAnimNo : hs SetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEAnimNo()
{
	// int hsid, int* animnoptr 
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimNo( &animno );
	if( ret ){
		DbgOut( "E3DGetMOEAnimNo : hs GetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );


	return 0;
}
int E3DSetMOEFrameNo()
{
	// int hsid, int animno, int frameno, int isfirst 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int isfirst;
	isfirst = code_getdi(0);
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEFrameNo( animno, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetMOEFrameNo : hs  error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEFrameNo()
{
	// int hsid, int* animnoptr, int* framenoptr 
	int hsid;
	hsid = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
//////////////
	int animno = -1;
	int frameno = 0;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &frameno );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEFrameNo( &animno, &frameno );
	if( ret ){
		DbgOut( "E3DGetMOEFrameNo : hs GetMOEFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &frameno );


	return 0;
}
int E3DSetNewMOE()
{
	// int hsid 
	int hsid;
	hsid = code_getdi(0);

///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewMOE : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewMOE();
	if( ret ){
		DbgOut( "E3DSetNewMOE : hs SetNewMOE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetNextMOEAnimFrameNo()
{
	// int hsid, int animno, int nextanimno, int nextframeno, int befframeno 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int nextanimno;
	nextanimno = code_getdi(0);

	int nextframeno;
	nextframeno = code_getdi(0);

	int befframeno;
	befframeno = code_getdi(0);
/////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMOEAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextMOEAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextMOEAnimFrameNo : hs SetNextMOEAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetNextMOEAnimFrameNo()
{
	// int hsid, int animno, int* nextanimptr, int* nextframenoptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
////////////
	int nextanimno = -1;
	int nextframeno = 0;
	code_setva( pvalno, aptrno, TYPE_INUM, &nextanimno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &nextframeno );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetNextMOEAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMOEAnimFrameNo( animno, &nextanimno, &nextframeno );
	if( ret ){
		DbgOut( "E3DGetNextMOEAnimFrameNo : hs GetNextMOEAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &nextanimno );
	code_setva( pvalframe, aptrframe, TYPE_INUM, &nextframeno );


	return 0;
}
int E3DCreateNewMOEAnim()
{
	// int hsid, char* animname, int animtype, int framenum, int* animnoptr 
	int hsid;
	hsid = code_getdi(0);

	int ret;
	char tempname[256];
	ret = GetName( tempname, 256 );
	if( ret ){
		DbgOut( "E3DCreateNewMOEAnim : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int animtype;
	animtype = code_getdi(0);

	int framenum;
	framenum = code_getdi(60);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
/////////////
	int animno = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &animno );



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNewMOEAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->CreateNewMOEAnim( tempname, animtype, framenum, &animno );
	if( ret ){
		DbgOut( "E3DCreateNewMOEAnim : hs CreateNewMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &animno );

	return 0;
}
int E3DSetMOEFrameNum()
{
	// int hsid, int animno, int newtotal 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int newtotal;
	newtotal = code_getdi(60);

//////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEFrameNum( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetMOEFrameNum : hs SetMOEFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEFrameNum()
{
	// int hsid, int animno, int* framenumptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);
	
	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );
//////////
	int framenum = 0;
	code_setva( pvalframe, aptrframe, TYPE_INUM, &framenum );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEFrameNum( animno, &framenum );
	if( ret ){
		DbgOut( "E3DGetMOEFrameNum : hs GetMOEFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalframe, aptrframe, TYPE_INUM, &framenum );

	return 0;
}
int E3DSetMOEAnimName()
{
	// int hsid, int animno, char* srcname 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int ret;
	char tempname[256];
	ret = GetName( tempname, 256 );
	if( ret ){
		DbgOut( "E3DSetMOEAnimName : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SetMOEAnimName( animno, tempname );
	if( ret ){
		DbgOut( "E3DSetMOEAnimName : hs SetMOEAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEAnimName()
{
	// int hsid, int animno, char* dstname, int arrayleng 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvalname;
	APTR aptrname;
	aptrname = code_getva( &pvalname );

	int arrayleng;
	arrayleng = code_getdi(0);
//////////
	char* nameptr = (char*)pvalname->pt + aptrname;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimName( animno, nameptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMOEAnimName : hs GetMOEAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMOEAnimType()
{
	// int hsid, int animno, int type 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int type;
	type = code_getdi(0);
////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEAnimType( animno, type );
	if( ret ){
		DbgOut( "E3DSetMOEAnimType : hs SetMOEAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEAnimType()
{
	// int hsid, int animno, int* typeptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	PVal *pvaltype;
	APTR aptrtype;
	aptrtype = code_getva( &pvaltype );
////////////
	int type = 0;
	code_setva( pvaltype, aptrtype, TYPE_INUM, &type );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimType( animno, &type );
	if( ret ){
		DbgOut( "E3DGetMOEAnimType : hs GetMOEAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvaltype, aptrtype, TYPE_INUM, &type );

	return 0;
}
int E3DGetMOEKeyframeNoRange()
{
	// int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int start;
	start = code_getdi(0);

	int end;
	end = code_getdi(0);

	PVal *pvalframe;
	APTR aptrframe;
	aptrframe = code_getva( &pvalframe );

	int arrayleng;
	arrayleng = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );
//////////////
	int getnum = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );
	int* framearray = (int*)pvalframe->pt + aptrframe;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEKeyframeNoRange( animno, start, end, framearray, arrayleng, &getnum );
	if( ret ){
		DbgOut( "E3DGetMOEKeyframeNoRange : hs GetMOEKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &getnum );


	return 0;
}
int E3DGetMOEElemNum()
{
	// int hsid, int animno, int frameno, int* moeenumptr, int* existkeyptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalnum;
	APTR aptrnum;
	aptrnum = code_getva( &pvalnum );

	PVal *pvalexist;
	APTR aptrexist;
	aptrexist = code_getva( &pvalexist );
/////////////
	int moeenum = 0;
	int existflag = 0;
	code_setva( pvalnum, aptrnum, TYPE_INUM, &moeenum );
	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEElemNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEElemNum( animno, frameno, &moeenum, &existflag );
	if( ret ){
		DbgOut( "E3DGetMOEElemNum : hs GetMOEElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalnum, aptrnum, TYPE_INUM, &moeenum );
	code_setva( pvalexist, aptrexist, TYPE_INUM, &existflag );


	return 0;
}
int E3DGetMOEElem()
{
	// int hsid, int animno, int frameno, int moeeindex, MOEELEM* moeeptr 
	//E3DGetMOEElem hsid, animno, frameno, moeindex, moei, list, notlist

	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int moeeindex;
	moeeindex = code_getdi(0);

	PVal *pvalmoei;
	APTR aptrmoei;
	aptrmoei = code_getva( &pvalmoei );

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnlist;
	APTR aptrnlist;
	aptrnlist = code_getva( &pvalnlist );
/////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MOEELEM moee;
	ZeroMemory( &moee, sizeof( MOEELEM ) );

	int ret;
	ret = curhs->GetMOEElem( animno, frameno, moeeindex, &moee );
	if( ret ){
		DbgOut( "E3DGetMOEElem : hs GetMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* moeiptr = (int*)pvalmoei->pt + aptrmoei;
	int* listptr = (int*)pvallist->pt + aptrlist;
	int* nlistptr = (int*)pvalnlist->pt + aptrnlist;

	*( moeiptr + MOEI_LISTNUM ) = moee.listnum;
	*( moeiptr + MOEI_NOTLISTNUM ) = moee.notlistnum;
	*( moeiptr + MOEI_EVENTNO ) = moee.eventno;

	MoveMemory( listptr, moee.list, sizeof( int ) * MOELISTLENG );
	MoveMemory( nlistptr, moee.notlist, sizeof( int ) * MOELISTLENG );

	return 0;
}
int E3DAddMOEElem()
{
	// int hsid, int animno, int frameno, MOEELEM moee 
	//E3DAddMOEElem hsid, animno, frameno, moei, list, notlist
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalmoei;
	APTR aptrmoei;
	aptrmoei = code_getva( &pvalmoei );

	PVal *pvallist;
	APTR aptrlist;
	aptrlist = code_getva( &pvallist );

	PVal *pvalnlist;
	APTR aptrnlist;
	aptrnlist = code_getva( &pvalnlist );
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* moeiptr = (int*)pvalmoei->pt + aptrmoei;
	int* listptr = (int*)pvallist->pt + aptrlist;
	int* nlistptr = (int*)pvalnlist->pt + aptrnlist;

	MOEELEM moee;
	ZeroMemory( &moee, sizeof( MOEELEM ) );
	moee.eventno = *( moeiptr + MOEI_EVENTNO );
	moee.listnum = *( moeiptr + MOEI_LISTNUM );
	moee.notlistnum = *( moeiptr + MOEI_NOTLISTNUM );
	MoveMemory( moee.list, listptr, sizeof( int ) * MOELISTLENG );
	MoveMemory( moee.notlist, nlistptr, sizeof( int ) * MOELISTLENG );

	int ret;
	ret = curhs->AddMOEElem( animno, frameno, moee );
	if( ret ){
		DbgOut( "E3DAddMOEElem : hs AddMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DDeleteMOEElem()
{
	// int hsid, int animno, int frameno, int moeeindex 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int moeeindex;
	moeeindex = code_getdi(0);
/////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteMOEElem( animno, frameno, moeeindex );
	if( ret ){
		DbgOut( "E3DDeleteMOEElem : hs DeleteMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DSetMOEKeyIdling()
{
	// int hsid, int animno, int frameno, int motid 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	int motid;
	motid = code_getdi(0);
////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEKeyIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEKeyIdling( animno, frameno, motid );
	if( ret ){
		DbgOut( "E3DSetMOEKeyIdling : hs SetMOEKeyIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DGetMOEKeyIdling()
{
	// int hsid, int animno, int frameno, int* motidptr 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

	PVal *pvalno;
	APTR aptrno;
	aptrno = code_getva( &pvalno );
///////////////
	int motid = -1;
	code_setva( pvalno, aptrno, TYPE_INUM, &motid );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEKeyIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEKeyIdling( animno, frameno, &motid );
	if( ret ){
		DbgOut( "E3DGetMOEKeyIdling : hs GetMOEKeyIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	code_setva( pvalno, aptrno, TYPE_INUM, &motid );

	return 0;
}
int E3DDeleteMOEKey()
{
	// int hsid, int animno, int frameno 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

	int frameno;
	frameno = code_getdi(0);

///////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteMOEKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteMOEKey( animno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteMOEKey : hs DeleteMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DCalcMOEFullFrame()
{
	// int hsid, int animno 
	int hsid;
	hsid = code_getdi(0);

	int animno;
	animno = code_getdi(0);

///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcMOEFullFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcMOEFullFrame( animno );
	if( ret ){
		DbgOut( "E3DCalcMOEFullFrame : hs CalcMOEFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int E3DUpdateSound()// $269
{
	int ret;

	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}
	ret = s_HS->Update();
	if( ret ){
		DbgOut( "E3DUpdateSound : hs Update error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int E3DGetSoundSamplesPlayed()// $26A
{
	int soundid;
	soundid = code_getdi(0);

	PVal *pvalsmp;
	APTR aptrsmp;
	aptrsmp = code_getva( &pvalsmp );
//////////////
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	double dsmp = 0.0;
	ret = s_HS->GetSoundSamplesPlayed( soundid, &dsmp );
	if( ret ){
		DbgOut( "E3DGetSoundSamplesPlayed : hs GetSoundSamplesPlayed error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	code_setva( pvalsmp, aptrsmp, TYPE_DNUM, &dsmp );

	return 0;
}

int E3DSet3DSoundEmiDist()// $26B
{
	int soundid;
	soundid = code_getdi(0);

	float dist;
	dist = (float)code_getdd(14.0);

////////////
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->Set3DEmiDist( soundid, dist );
	if( ret ){
		DbgOut( "E3DSet3DSoundEmiDist : hs Set3DEmiDist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

/***
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
***/

/***
int E3DTestCooperativeLevel()// $1B3
{
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

////////////

	if( !g_pD3DApp ){
		DbgOut( "E3DTestCooperative : d3dapp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int notrender;
	notrender = g_pD3DApp->TestCooperativeAndReset();

	int renderok;
	if( notrender == 0 ){
		renderok = 1;
	}else{
		renderok = 0;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &renderok );

	return 0;
}

int E3DActivateApp()// $1B4
{
	int wparam;
	wparam = code_getdi(0);
///////

	DbgOut( "E3DActivateApp : %d !!!\n", wparam );

	if( !g_pD3DApp ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( wparam == 0 ){
		if( g_pD3DApp->m_bReady ){
			g_pD3DApp->InvalidateDeviceObjects();
			g_pD3DApp->m_pd3dDevice->Reset( &g_pD3DApp->m_d3dpp );
			//g_pD3DApp->m_bReady = 0;
			g_activateapp = 0;
		}
	}else{
		if( g_pD3DApp->m_bReady ){
			//g_pD3DApp->Resize3DEnvironment();
			//g_pD3DApp->m_bReady = 1;
			g_activateapp = 1;
		}
	}

	return 0;
}
***/
/***
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
***/
