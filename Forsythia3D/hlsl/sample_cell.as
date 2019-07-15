; セルシェーディングのサンプル
; ※テクスチャ無しモデル用です

; 注意！
; ビデオカードによっては正常に動作しません。

; HLSL でピクセルシェーダを使用しているため、
; GeForce3 以降のビデオカードが必要です。


#include "forsythia.as"

	bgscr 2,ginfo_dispx,ginfo_dispy,1
	title "Forsythia3D sample"

	fr_init
	fr_bgcolor 128,128,128

	; シェーダの読み込み
	fr_loadshader "data\\hlsl.fx", 0
	if stat : dialog refstr : end

	; 環境マップ用テクスチャの読み込み
	fr_loadtex "data\\cell.bmp", 0

	; モデルデータの読み込み
	dialog "x",16 : if stat=0 : end
	fr_load refstr, 0

	; カメラ設定
	fr_setcampos 0, 0, -3<<16

	; シェーダの選択
	fr_seltech 0, "CellShaderNT"
	fr_selshader 0

	; シェーダに与えるテクスチャの設定
	fr_setshadertex 0, 0, 0

	; シェーダに与えるベクトルの設定
	param = -100000, 100000, -100000, 600
	fr_setshaderarg param, 0, 0

	; スケール情報の初期化
	obj.6 = 1<<16, 1<<16, 1<<16

*MAIN
	stick bt,95 : if bt&128 : end

	vx = (bt&1!0)-(bt&4!0) * 1000
	vy = (bt&8!0)-(bt&2!0) * 1000
	if bt&64 : z += vy : vy=0
	if bt&16 {
		x += vx : y += vy
	} else {
		rx += vy/4 : ry += vx/4
	}
	obj = x, y, z, rx, ry, rz
	obj.9+=4 ; モーション再生

	fr_begin
	fr_setposture obj, 0 : fr_draw 0
	fr_end

	await 1
goto *MAIN
