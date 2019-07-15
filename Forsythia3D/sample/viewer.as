; モデルビューワ

#include "forsythia.as"

	title "Forsythia3D sample"
	dialog "x",16 : if stat=0 : end
	sdim fln,512 : fln = refstr

	fr_init

	; 平行光源
	fr_addlight    0, D3DLIGHT_DIRECTIONAL
	fr_setlightdir 0, 0<<16, 0<<16, 1<<16
	fr_setlightcol 0, 1<<16, 1<<16, 1<<16
	fr_setlightspq 0, 1<<16, 1<<16, 1<<16

	; モデルデータの読み込み
	fr_load fln, 0

	; レンダリング設定
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_SPECULARENABLE, 1
	fr_bgcolor 0x40, 0x40, 0x40

	; カメラ設定
	fr_setcampos 0, 0, -3<<16
	fr_setcamtarget 0, 0, 0

	; スケール情報の初期化
	obj.6 = 1<<16, 1<<16, 1<<16

*MAIN
	stick bt,95
	if bt&128 : end

	vx = (bt&1!0)-(bt&4!0) * 1000
	vy = (bt&8!0)-(bt&2!0) * 1000
	if bt&64 : z += vy : vy=0
	if bt&16 {
		x += vx : y += vy
	} else {
		rx += vy/2 : ry += vx/2
	}
	obj = x, y, z, rx, ry, rz

	obj.9 += 4	; モーション再生

	fr_begin
	fr_setposture obj, 0 : fr_draw 0
	fr_end

	await 1
goto *MAIN
