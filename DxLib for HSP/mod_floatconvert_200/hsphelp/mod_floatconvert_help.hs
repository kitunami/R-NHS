;/*==========================================================================*/
;//--------------------------------------------------------------------------//

%type
�g�����W���[��

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
mod_floatconvert.hsp ���C���N���[�h���邱��

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
double �^�̒l�� float �^�̒l�ɕϊ�

%prm
( double p1 )
double p1       : �ϊ������� double �^�̒l

%group
�g�����o�͊֐�

%inst
double �^�̒l(�{���x�����l�A64bit)�� float �^�̒l(�P���x�����l�A32bit)�ɕϊ����܂��B

�@�ʏ�� HSP �v���O���~���O�ł͎g�p����K�v�͂���܂��񂪁AHSP �O���̃R���|�[�l���g���g�p����v���O������ float �^�̒l���K�v�ȏꍇ�ȂǂɎg�p���܂��B

�@p1 �ɕϊ������� double �^�̒l���w�肵�܂��B

�@���s��͕ϊ����ꂽ float �^�̒l�̃������C���[�W���i�[���ꂽ int �^�̒l��Ԃ��܂��B
HSP �ł� float �^�̒l�𒼐ڈ������Ƃ͂ł��Ȃ��̂� float �^�̒l�̃������C���[�W���i�[���ꂽ int �^�̒l���Ԃ���܂��B

%sample
#include "mod_floatconvert.hsp"
    mes "double �^�̒l�� float �^�̒l�̃������C���[�W�ɕϊ�"
    d = 1.0
    input d, 128, 32
    button gosub "�ϊ�", *cvt
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
float �^�̒l�� double �^�̒l�ɕϊ�

%prm
( int p1 )
int p1           : �ϊ������� float �^�̒l�̃������C���[�W���i�[���ꂽ int �^�̒l

%group
�g�����o�͊֐�

%inst
float �^�̒l(�P���x�����l�A32bit)�� double �^�̒l(�{���x�����l�A64bit)�ɕϊ����܂��B

�@�ʏ�� HSP �v���O���~���O�ł͎g�p����K�v�͂���܂��񂪁AHSP �O���̃R���|�[�l���g���g�p����v���O������ float �^�̒l���擾�����ꍇ�ȂǂɎg�p���܂��B

�@p1 �ɕϊ������� float �^�̒l�̃������C���[�W���i�[���ꂽ int �^�̒l���w�肵�܂��B
HSP �ł� float �^�̒l�𒼐ڈ������Ƃ͂ł��Ȃ��̂� float �^�̒l�̃������C���[�W���i�[���ꂽ int �^�̒l�������Ƃ��ēn���܂��B

�@���s��͕ϊ����ꂽ double �^�̒l��Ԃ��܂��B

%sample
#include "mod_floatconvert.hsp"
    mes "float �^�̒l�̃������C���[�W�� double �^�ɕϊ�"
    f = "$3F800000"
    input f, 128, 32
    button gosub "�ϊ�", *cvt
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
