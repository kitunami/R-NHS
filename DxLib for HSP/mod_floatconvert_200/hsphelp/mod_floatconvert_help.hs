;/*==========================================================================*/
;//--------------------------------------------------------------------------//

%type
拡張モジュール

%dll
mod_floatconvert.hsp

%author
naznyark

%ver
2.00

%date
2008/06/12

%url
http://hp.vector.co.jp/authors/VA043120/

%note
mod_floatconvert.hsp をインクルードすること

;//--------------------------------------------------------------------------//

; _template

;%index

;%prm

;%group

;%inst

;%sample

;%href

;//------------------------------------------------------------//

; dtof

%index
dtof
double 型の値を float 型の値に変換

%prm
( double p1 )
double p1       : 変換したい double 型の値

%group
拡張入出力関数

%inst
double 型の値(倍精度実数値、64bit)を float 型の値(単精度実数値、32bit)に変換します。

　通常の HSP プログラミングでは使用する必要はありませんが、HSP 外部のコンポーネントを使用するプログラムで float 型の値が必要な場合などに使用します。

　p1 に変換したい double 型の値を指定します。

　実行後は変換された float 型の値のメモリイメージが格納された int 型の値を返します。
HSP では float 型の値を直接扱うことはできないので float 型の値のメモリイメージが格納された int 型の値が返されます。

%sample
#include "mod_floatconvert.hsp"
    mes "double 型の値を float 型の値のメモリイメージに変換"
    d = 1.0
    input d, 128, 32
    button gosub "変換", *cvt
    stop
*cvt
    mes strf( "%1.18g", d ) + strf( " -> %08X", dtof( d ) )
    return

%href
dtof
ftod

;//------------------------------------------------------------//

; ftod

%index
ftod
float 型の値を double 型の値に変換

%prm
( int p1 )
int p1           : 変換したい float 型の値のメモリイメージが格納された int 型の値

%group
拡張入出力関数

%inst
float 型の値(単精度実数値、32bit)を double 型の値(倍精度実数値、64bit)に変換します。

　通常の HSP プログラミングでは使用する必要はありませんが、HSP 外部のコンポーネントを使用するプログラムで float 型の値を取得した場合などに使用します。

　p1 に変換したい float 型の値のメモリイメージが格納された int 型の値を指定します。
HSP では float 型の値を直接扱うことはできないので float 型の値のメモリイメージが格納された int 型の値を引数として渡します。

　実行後は変換された double 型の値を返します。

%sample
#include "mod_floatconvert.hsp"
    mes "float 型の値のメモリイメージを double 型に変換"
    f = "$3F800000"
    input f, 128, 32
    button gosub "変換", *cvt
    stop
*cvt
    mes strf( "%08X", int( f ) ) + strf( " -> %1.18g", ftod( int( f ) ) )
    return

%href
dtof
ftod

;//------------------------------------------------------------//

;//--------------------------------------------------------------------------//
;/*==========================================================================*/
