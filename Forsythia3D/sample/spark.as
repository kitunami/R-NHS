; パーティクル サンプル
; カーソルキーで移動

#include "forsythia.as"

	randomize
	bgscr 2,dispx,dispy,1 : title "Forsythia3D sample"
	dim sin, 65536 : fr_getsintbl sin, 65536
	dim cos, 65536 : fr_getcostbl cos, 65536

	fr_init
	fr_loadtex "particle.bmp", 0

	; レンダリング設定
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_ZWRITEENABLE, 0
	fr_setrs D3DRS_LIGHTING, 0
	fr_setrs D3DRS_POINTSPRITEENABLE, 1
	fr_setrs D3DRS_POINTSIZE, 20.0f

*MAIN
	gosub *KEYIN
	gosub *CAMERA

	fr_begin
	gosub *PTCL
	if bt&64=0 : gosub *PTCG
	fr_end
	await 1
goto *MAIN

*PTCL
	fr_settex 0
	frp_draw
return

*PTCG
	frp_update
	param = 64,0,-3<<16,0
	param.7 = 0, -140, 0
	param.10 = 0xff8040, 0x7c7e7f
	repeat 20
		rnd t,5000 : param.5 = t+5000
		rnd t,2000 : param.6 = t-1000<<1
		rnd t,2000 : param.4 = t-1000<<1
		frp_add param
	loop
return

*KEYIN
	stick bt,79 : if bt&128 : end

	yt+= (bt&1!0)-(bt&4!0) * 200 : yr += yt-yr>>4
	ct+= (bt&2!0)-(bt&8!0) * 4000: cy += ct-cy>>4
return


*CAMERA
	t = yr & 65535
	cx = sin.t<<3 : cz = cos.t<<3
	fr_setcampos cx, cy, cz
	fr_setcamtarget 0, 0, 0
	fr_setcamup 0, 1, 0
return


