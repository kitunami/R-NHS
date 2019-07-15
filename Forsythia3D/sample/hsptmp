; レースゲームサンプル
; カーソルキーで移動、スペースキーで中継カメラ

; ※実行には、dsoundex が必要です

#include "forsythia.as"
#include "dsoundex.as"

	;bgscr 2,dispx,dispy,1:mouse -1
	title "Forsythia3D sample"
	dim sin1, 65536 : fr_getsintbl sin1, 65536
	dim cos1, 65536 : fr_getcostbl cos1, 65536

	ds_init : fr_init
	ds_load "data\\engine.wav", 0
	ds_load "data\\shock.wav", 1
	ds_setf 0, 100 : ds_vol 0, 200 : ds_loop 0

	; 平行光源
	fr_addlight    0, D3DLIGHT_DIRECTIONAL
	fr_setlightdir 0, 1<<14,-1<<16,-1<<16
	fr_setlightcol 0, 1<<16, 1<<16, 1<<16
	fr_setlightspq 0, 1<<16, 1<<16, 1<<16

	; モデルデータの読み込み
	fr_load "data\\course1.x", 0
	fr_load "data\\ship1.x", 1

	fr_loadtex "particle.bmp", 0

	; レンダリング設定
	fr_setrs D3DRS_SPECULARENABLE, 1
	fr_setrs D3DRS_ALPHABLENDENABLE, 1
	fr_setrs D3DRS_POINTSPRITEENABLE, 1
	fr_setrs D3DRS_POINTSCALEENABLE, 1
	fr_setrs D3DRS_POINTSIZE, 0.1f
	fr_setrs D3DRS_POINTSIZE_MIN, 0.0f
	fr_setrs D3DRS_POINTSCALE_A, 0.0f
	fr_setrs D3DRS_POINTSCALE_B, 0.0f
	fr_setrs D3DRS_POINTSCALE_C, 5.0f

	; フォント設定
	fontparam = 0, 20, 1 
	fr_setfont fontparam, "Times New Roman"

	; スケール情報の初期化
	obj  = 0, 0, 0, 0, 0, 0, 1<<20, 1<<20, 1<<20
	ship = 0, 0, 0, 0, 0, 0, 1<<13, 1<<13, 1<<13

	nextchk = 1
	al = 0 : dm = 1<<16

*MAIN
	gosub *CAMERA

	fr_begin
	gosub *BG
	gosub *JIKI
	gosub *PTCL
	fr_end

	await 1
goto *MAIN

*BG
	fr_setposture obj, 0 : fr_draw 0
return

*PTCL
	fr_settex 0
	fr_setrs D3DRS_SRCBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_ONE
	fr_setrs D3DRS_ZWRITEENABLE, 0
	fr_setrs D3DRS_LIGHTING, 0
	frp_draw 0 : frp_update 0
	fr_setrs D3DRS_SRCBLEND, D3DBLEND_SRCALPHA
	fr_setrs D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA
	fr_setrs D3DRS_ZWRITEENABLE, 1
	fr_setrs D3DRS_LIGHTING, 1
return

*GRAZE
	param = 32, vec.0, vec.1, vec.2
	rnd t,100 : param.4 = xv/2+t-50
	rnd t,20  : param.5 = t+5<<4
	rnd t,100 : param.6 = zv/2+t-50
	param.7 = 0, -10, 0
	param.10 = 0xff8040, 0x78787f
	frp_add param
return

*SHOCK
	ds_play 1 : dm=0
	param = 64, x, y, z
	repeat 40
		rnd t,1000 : param.4 = t-500
		rnd t,1600 : param.5 = t
		rnd t,1000 : param.6 = t-500
		param.7 = 0, -20, 0
		param.10 = 0xff8040, 0x7c7e7f
		frp_add param
	loop
return

*JIKI
	stick bt,79 : if bt&16 : viewmode = 1-viewmode
	if bt&128 : end

	dm += 1<<16-dm>>2 : al += 1<<16-al>>5
	col = 1<<16, dm, dm, al
	fr_setobjcolor col, 1

	; 速度と回転の入力
	yr = yr & 65535
	cr = cos1.yr>>4 : sr = sin1.yr>>4
	v += (bt&8!0)-(bt&2!0)<<16 - v >> 8
	xr+= 1<<6
	f = 5000-v+df : if f<1000 : f=1000
	if f>80000 : f=80000
	ds_setf 0, f
	xv = v * sr >> 16 : zv = v * cr >> 16
	oy = y : z += zv : x += xv : y += yv
	vw = (bt&4!0)-(bt&1!0) * 200
	ytr += vw
	ycr += ytr-ycr>>4
	zr -= vw*3
	yr = ycr & 65535
	xy = y+(1<<12)

	; 壁判定
	inf = 0, 0, 0, 0, 0,-1<<16 : fr_intersectL inf, 0, 1
	inb = 0, 0, 0, 0, 0, 1<<16 : fr_intersectL inb, 0, 1
	inl = x, xy, z, cr<<4, 0,-sr<<4 : fr_intersect inl, 0
	inr = x, xy, z,-cr<<4, 0, sr<<4 : fr_intersect inr, 0

	; 壁に前後から当たった
	if inf.7>=0 : if inf.6<(1<<16) {
		if v<0 {
			if v<-8000 : gosub *SHOCK
			x-=xv*2:z-=zv*2:v=v/2
		}
	}
	if inb.7>=0 : if inb.6<(1<<16) {
		if v>0 {
			if v>8000 : gosub *SHOCK
			x-=xv*2:z-=zv*2:v=v/2
		}
	}

	; 壁に左右から当たった
	if inl.7>=0 : if inl.6<(1<<13) {
		d = 1<<13-inl.6/8
		x -= d*cr>>12 : z += d*sr>>12
		ycr+=d:ytr+=d/2:zr-=d*2
	}
	if inr.7>=0 : if inr.6<(1<<13) {
		d = 1<<13-inr.6/8
		x += d*cr>>12 : z -= d*sr>>12
		ycr-=d:ytr-=d/2:zr+=d*2
	}

	; 周囲の地形状況の取得
	in0 = x, xy, z, 0, -65536, 0 : fr_intersect in0, 0

	; 状態表示
	sdim ms,512
	ms = "Height "+in0.6+"\nHitFace "+in0.7+"\nMaterial "+in0.8
	ms += "\nPos ("+x+", "+y+", "+z+")"
	fr_setcolor   0,  0,0,255 : fr_mes ms, 2, 2
	fr_setcolor 255,255,0,255 : fr_mes ms, 0, 0

	; 姿勢の作成と描画
	if in0.7<0 : in0.6=99999
	if in0.6<4200 {
		in1 = x-cr, xy, z+sr, 0, -65536, 0 : fr_intersect in1, 0
		in2 = x+cr, xy, z-sr, 0, -65536, 0 : fr_intersect in2, 0
		in3 = x-sr, xy, z-cr, 0, -65536, 0 : fr_intersect in3, 0
		in4 = x+sr, xy, z+cr, 0, -65536, 0 : fr_intersect in4, 0
		fr_atan xrr, (in4.6-in3.6), 1<<13 : xr += xrr-xr>>3
		fr_atan zrr, (in2.6-in1.6), 1<<13 : zr += zrr-zr>>3
		ytr -= zrr>>5
		v += xrr>>5 : y += (1<<12-in0.6)>>4
		if in0.6<2000 : oy=y-(1<<8)
		if zrr-zr>4000 : {
			vec = -55000, 18000, 25000 : fr_vecftow vec, 1
			gosub *GRAZE
		}
		if zrr-zr<-4000 {
			vec = 55000, 18000, 25000 : fr_vecftow vec, 1
			gosub *GRAZE
		}
		if in0.8 = 0  : v -= 12000
	} else {
		xr = xr*19/20 : zr = zr*19/20
		y -= 20
	}
	yv = y - oy : if yv>1000 : yv=1000
	ship = x, y+(1<<11), z, xr, yr, -zr
	fr_setposture ship, 1 : fr_draw 1
	if y<(-4<<16) : x=0 : y=0 : z=0 : v=0 : ytr=0 : ycr=0 : yv=0 : al=0

	; ジェット軌跡
	param = 16, -8000, 8500, 40000, xv/2, yv/2, zv/2, 0, 0, 0, 0x4080ff, 0x707070
	fr_vecftow param,1,1,1 : frp_add param
	param.1 = 8000, 8500, 40000
	fr_vecftow param,1,1,1 : frp_add param
	param.1 = 8000, -1500, 40000
	fr_vecftow param,1,1,1 : frp_add param
	param.1 = -8000, -1500, 40000
	fr_vecftow param,1,1,1 : frp_add param
return 


*CAMERA
	xd = sr<<2 : yd = 1<<13 : zd = cr<<2

	if viewmode=0 {
		cx += x-cx>>2 : cy += y-cy>>2 : cz += z-cz>>2
		fr_setcampos cx+xd, cy+yd, cz+zd
		fr_setcamtarget x, y+yd, z
		vv=200 : ds_vol 0, vv : ds_vol 1, vv
		df = 0
	} else {
		ol=cl : fr_len cl, cx-x, cy-y, cz-z
		fr_setcamtarget x, y, z
		if 6<<16 < cl {
			cx = x - (sin1.yr*3)
			cy = y + 50000
			cz = z - (cos1.yr*3)
			fr_setcampos cx, cy, cz
		}
		vv = (6<<16 - cl)>>10 : if vv<0 : vv=0
		ds_vol 0, vv : ds_vol 1, vv
		df = ol-cl*12
	}
return

