; 当たり判定・フォグ・カメラのサンプル
; カーソルキーで移動、スペースキーで中継カメラ

#include "forsythia.as"

	bgscr 2,ginfo_dispx,ginfo_dispy,1
	title "Forsythia3D sample"
	dim sin1, 65536 : fr_getsintbl sin1, 65536
	dim cos1, 65536 : fr_getcostbl cos1, 65536

	fr_init

	; 平行光源
	fr_addlight    0, D3DLIGHT_DIRECTIONAL
	fr_setlightdir 0, 1<<16,-1<<16,-1<<16
	fr_setlightcol 0, 1<<16, 1<<16, 1<<16
	fr_setlightspq 0, 1<<16, 1<<16, 1<<16

	; モデルデータの読み込み
	fr_load "data\\ground.x", 0
	fr_load "data\\ship1.x", 1

	; レンダリング設定
	fr_setrs D3DRS_SPECULARENABLE, 1
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_FOGENABLE, 1
	fr_setrs D3DRS_FOGTABLEMODE, D3DFOG_EXP2
	fr_setrs D3DRS_FOGDENSITY, 0.3f
	fr_setrs D3DRS_FOGCOLOR, 0x804020
	fr_bgcolor 0x80, 0x40, 0x20

	; スケール情報の初期化
	obj  = 0, 0, 0, 0, 0, 0, 1<<19, 1<<19, 1<<19
	mark = 0, 0, 0, 0, 0, 0, 1<<13, 1<<13, 1<<13

*MAIN
	gosub *CAMERA

	fr_begin
	gosub *BG
	gosub *JIKI
	fr_end

	await 1
goto *MAIN

*BG
	fr_setposture obj, 0 : fr_draw 0
return

*JIKI
	stick bt,95
	if bt&128 : end

	; 速度と回転の入力
	cr = cos1.yr>>4 : sr = sin1.yr>>4
	v += (bt&8!0)-(bt&2!0)<<16 - v >> 8
	z += v * cr >> 16 : x += v * sr >> 16
	yr+= (bt&4!0)-(bt&1!0) * 200
	yr = yr & 65535

	; 周囲の地形状況の取得
	oy = y + (1<<12)
	in1 = x-cr, oy, z+sr, 0, -65536, 0 : fr_intersect in1, 0
	in2 = x+cr, oy, z-sr, 0, -65536, 0 : fr_intersect in2, 0
	in3 = x-sr, oy, z-cr, 0, -65536, 0 : fr_intersect in3, 0
	in4 = x+sr, oy, z+cr, 0, -65536, 0 : fr_intersect in4, 0

	; 姿勢の作成と描画
	y += (1<<11) - (in1.6+in2.6+in3.6+in4.6/4-(1<<12))
	fr_atan xrr, (in4.6-in3.6), 1<<13 : xr += xrr-xr>>3
	fr_atan zrr, (in2.6-in1.6), 1<<13 : zr += zrr-zr>>3
	mark = x, y, z, xr, yr, -zr
	fr_setposture mark, 1 : fr_draw 1
return 


*CAMERA
	xd = sr<<2 : yd = 1<<13 : zd = cr<<2
	if bt&16=0 {
		cx += x-cx>>2 : cy += y-cy>>2 : cz += z-cz>>2
		fr_setcampos cx+xd, cy+yd, cz+zd
		proj = 1<<16,winx,winy,1<<13,50<<16
		fr_setproj proj
		fr_setcamtarget x, y+yd, z
	} else {
		vx += x-vx>>2 : vy += y-vy>>2 : vz += z-vz>>2
		fr_len l, cx-vx, cy-vy, cz-vz
		fov=1<<30/l : if fov>(1<<16) : fov = 1<<16
		proj = fov,winx,winy,1<<13,50<<16
		fr_setproj proj
		fr_setcamtarget vx, vy, vz
	}
	fr_setcamup 0, 1, 0
return

