; 画面いっぱいにテクスチャを表示する

#include "forsythia.as"

	title "Forsythia3D sample"

	fr_init
	fr_loadtex "obi.bmp", 0
	fr_settex 0

*MAIN
	stick bt : if bt&128 : end

	fr_begin

	param = 0, 0, winx, winy, 0, 0, 1<<16, 1<<16
	fr_drawsprite param

	fr_end
	await 1
goto *MAIN

