; shoot.as にパーティクルをつけたもの
; カーソルキーで移動、Ctrl キーでショット
; Enter キーで機内カメラに切り替え

#include "forsythia.as"

	randomize
	bgscr 2,dispx,dispy,1 : title "Forsythia3D sample"
	dim sin, 65536 : fr_getsintbl sin, 65536
	dim cos, 65536 : fr_getcostbl cos, 65536

	fr_init

	; 平行光源
	fr_addlight    0, D3DLIGHT_DIRECTIONAL
	fr_setlightdir 0, 0,-1<<16,-1<<16
	fr_setlightcol 0, 1<<15, 1<<15, 1<<15
	fr_setlightspq 0, 1<<16, 1<<16, 1<<16

	; エフェクト用点光源
	fr_addlight    1, D3DLIGHT_POINT
	fr_setlightatt 1, 0, 1<<14

	; モデルデータの読み込み
	fr_load "data\\ship1.x", 0
	fr_load "data\\enemy.x", 1
	fr_load "data\\tama.x", 2
	fr_load "data\\bomb.x", 3
	fr_load "data\\bg.x", 4


	fr_loadtex "particle.bmp", 0

	; レンダリング設定
	fr_setrs D3DRS_SPECULARENABLE, 1
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_POINTSPRITEENABLE, 1
	fr_setrs D3DRS_POINTSCALEENABLE, 1
	fr_setrs D3DRS_POINTSIZE,     1.0f
	fr_setrs D3DRS_POINTSIZE_MIN, 2.0f
	fr_setrs D3DRS_POINTSIZE_MAX, 256.0f
	fr_setrs D3DRS_POINTSCALE_A,  0.0f
	fr_setrs D3DRS_POINTSCALE_B,  0.0f
	fr_setrs D3DRS_POINTSCALE_C,  40.0f

	; フォント設定
	fontparam = 0, 25, 1 
	fr_setfont fontparam, "Times New Roman"
	fr_setcolor 255,255,255,200

	; スケール情報の初期化
	obj_tama = 0, 0, 0, 0, 0, 0, 1<<15, 1<<15, 1<<15
	obj_jiki = 0, 0, 0, 0, 0, 0, 1<<15, 1<<15, 1<<15
	obj_teki = 0, 0, 0, 0, 0, 0, 1<<16, 1<<16, 1<<16
	obj_bg   = 0, 0, 0, 0, 0, 0, 8<<16, 8<<16, 8<<16
	obj_bomb = 0, 0, 0, 0, 0, 0, 2<<16, 2<<16, 2<<16

	cy = 10<<16

*MAIN
	gosub *KEYIN
	gosub *CAMERA

	; エフェクト用ライトの色設定
	fr_setlightcol 1, bpower/4, bpower/2, bpower
	bpower = bpower/2

	fr_begin
	gosub *BG
	gosub *TAMA
	gosub *JIKI
	gosub *TEKI
	gosub *PTCL
	fr_mes "score = "+score

	fr_end
	await 1
goto *MAIN

*BG
	frm++
	repeat 16
		zz = -cnt-1<<4+(frm&15)<<14
		obj_bg = 0, 0, zz, 0, 0, frm<<5
		fr_setposture obj_bg, 4 : fr_draw 4
	loop
return

*PTCL
	fr_settex 0
	fr_setrs D3DRS_SRCBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_ZWRITEENABLE, 0
	fr_setrs D3DRS_LIGHTING, 0
	frp_update : frp_draw
	fr_setrs D3DRS_SRCBLEND, D3DBLEND_SRCALPHA
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA
	fr_setrs D3DRS_ZWRITEENABLE, 1
	fr_setrs D3DRS_LIGHTING, 1
return

*PTCG
	param = 256, px, py, pz
	rnd t,800 : param.4 = t-400
	rnd t,800 : param.5 = t-400
	rnd t,2000 : param.6 = t-1000
	param.7 = 0, 0, 100, 0xffffff, 0x7d7e7f
	frp_add param
return

*KEYIN
	stick bt,79
	if bt&128 : end
	if bt&32 : viewmode = 1-viewmode:xr=0:xrn=0

	xv = (bt&1!0)-(bt&4!0) * 8000
	yv = (bt&2!0)-(bt&8!0) * 8000
	x += xr : xr += xv-xr>>5
	y += yr : yr += yv-yr>>5
	if x<(-3<<16) : x=-3<<16
	if x>( 3<<16) : x= 3<<16
	if y<(-3<<16) : y=-3<<16
	if y>( 3<<16) : y= 3<<16
return 


*CAMERA
	if viewmode {
		t = xr*2 & 65535
		cx = sin.t/5+x : cy = cos.t/5+y
		fr_setcampos cx, cy, z
		fr_setcamtarget cx, cy, z-1
		fr_setcamup sin.t, cos.t, 0
	} else {
		cx += x*9/10-cx>>4 : cy += y*9/10-cy>>4
		fr_setcampos cx, cy, 1<<16
		fr_setcamtarget x, y, -2<<16
		fr_setcamup 0, 1, 0
	}
return


*JIKI
	obj_jiki = x, y, z, yr>>3, 0, -xr*2
	fr_setposture obj_jiki, 0
	fr_draw 0
return


*TEKI
	repeat 16
		if ef.cnt=0 {
			ef.cnt=1
			rnd t,40 : ex.cnt=t-20<<13
			rnd t,40 : ey.cnt=t-20<<13
			rnd t,20 : ez.cnt=t-40<<16
			ezv.cnt=0
		}
		ez.cnt+=ezv.cnt
		if ezv.cnt<8000 : ezv.cnt += 200
		obj_teki = ex.cnt, ey.cnt, ez.cnt, er.cnt
		erv.cnt = erv.cnt*19/20 : er.cnt += erv.cnt
		col = 1<<16, ecol.cnt, ecol.cnt, 1<<16
		fr_setobjcolor col, 1
		fr_setposture obj_teki, 1
		fr_draw 1
		if 1<<16<ez.cnt : ef.cnt=0
		ecol.cnt += 1<<16-ecol.cnt>>2
		n=cnt : gosub *HITCHECK
	loop
return

*HITCHECK
	repeat 16 : if tf.cnt=0 : continue
		dx = tx.cnt-ex.n : if dx<0 : dx=-dx
		dy = ty.cnt-ey.n : ddy=dy : if dy<0 : dy=-dy
		dz = tz.cnt-ez.n : if dz<0 : dz=-dz
		if dz>(1<<16) : continue
		if dy>(1<<16) : continue
		if dx>(1<<16) : continue
		tf.cnt = 0 : rnd t,256:t=t<<8
		ezv.n = -1<<14 : erv.n -= ddy>>5
		obj_bomb = tx.cnt, ty.cnt, tz.cnt, 0,0,t
		fr_setposture obj_bomb,3 : fr_draw 3
		fr_setlightpos 1, tx.cnt, ty.cnt, tz.cnt
		px = tx.cnt : py = ty.cnt : pz = tz.cnt
		repeat 10 : gosub *PTCG : loop
		ecol.n = 0
		bpower = 1<<16 : rnd t,30000 : score+=t
		break
	loop
return


*TAMA
	; 加算合成モードに設定
	fr_setrs D3DRS_SRCBLEND , D3DBLEND_ONE
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_ONE
	wt--
	if bt&64 {
		if wt<0 {
			tf.tp=1 : tx.tp=x : ty.tp=y : tz.tp=z-(1<<14)
			wt=2 : tp=tp+1&15
		}
	}
	repeat 16 : if tf.cnt=0 : continue
		tz.cnt -= 1<<15
		obj_tama = tx.cnt, ty.cnt, tz.cnt
		fr_setposture obj_tama, 2
		fr_draw 2
	loop
	; 元に戻す
	fr_setrs D3DRS_SRCBLEND , D3DBLEND_SRCALPHA
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA
return
