; Forsythia 時計

; 右下の緑のオブジェが現在時刻を表していますが、読めないかも。


#include "forsythia.as"

	gsel 0,-1

	bgscr 2,dispx,dispy,1:title "Forsythia3D sample"

	fr_init

	; 平行光源
	fr_addlight    0, D3DLIGHT_DIRECTIONAL
	fr_setlightdir 0, 0<<16, 0<<16, 1<<16
	fr_setlightcol 0, 1<<16, 1<<16, 1<<16
	fr_setlightspq 0, 1<<16, 1<<16, 1<<16

	; モデルデータの読み込み
	fr_load "data\\ring_y0.x", 0

	; レンダリング設定
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_SRCBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_ZWRITEENABLE, 0
	fr_setrs D3DRS_LIGHTING, 1

	col = 1<<14, 1<<15, 1<<16, 1<<16
	fr_setobjcolor col, 0

	; カメラ設定
	fr_setcampos 0, 0, -3<<16
	fr_setcamtarget 0, 0, 0
	clc = 1<<13, 1<<15, 1<<14, 1<<16
	clk = 3<<15, -1<<16, 0, 0, 0, 0, 1<<16, 1<<16, 1<<16 

*MAIN
	stick bt,95, 1
	if bt&128 : end

	fr_begin

	col = 1<<14, 1<<15, 1<<16, 1<<16
	fr_setobjcolor col, 0
	repeat 16
		r.cnt += cnt+1*3 : z.cnt = cnt+1<<13
		obj = 0, 0, 0, r.cnt, r.cnt, r.cnt, z.cnt, z.cnt, 1<<16
		fr_setposture obj, 0 : fr_draw 0
	loop

	gettime ms,7
	gettime s,6
	gettime m,5
	gettime h,4
	hr  = -(h*60+m*60+s/3      << 16 / 14400)
	mr  = -(m*60+s*1000+ms/120 << 16 / 30000)
	sr  = -(s*1000+ms/10       << 16 / 6000)

	fr_setobjcolor clc, 0
	z = 21<<10 : clk.5 = hr, z/4,z
	fr_setposture clk, 0 : fr_draw 0
	z = 25<<10 : clk.5 = mr, z/4,z
	fr_setposture clk, 0 : fr_draw 0
	z = 30<<10 : clk.5 = sr, z/4,z
	fr_setposture clk, 0 : fr_draw 0

	fr_end
	await 1
goto *MAIN
