;--------------------------------------------------
; HSP �w���v �\�[�X�t�@�C�� (hs �`��)
; hs �t�@�C���̏����ɂ��ẮAHS_BIBLE.txt ���Q�Ƃ��Ă��������B

;--------------------------------------------------
; ���� hs �t�@�C���̃f�t�H���g�̃t�B�[���h�l��ݒ�

%dll
DxLib

%ver
3.14

%date
2019/09/16

%author
�R�c�I

%url
https://dxlib.xsrv.jp/

%note
DxLib.as���C���N���[�h���邱�ƁB

%type
; �^�C�v ���L��

%port
; �Ή��� ���L��

%portinfo
; �ڐA�̃q���g ���L��

%index
DxLib_Init
���C�u�����̏�����

%prm
; �p�����[�^���X�g ���L��
; �p�����[�^������ ���L��

%inst
�@�c�w���C�u�������g�p�\�ȏ�Ԃɏ��������܂��B
�c�w���C�u�������g�p����\�t�g�E�F�A�͂܂����̊֐����Ăяo���K�v������܂��B
�@���̊֐����g���Ă���-1�i�G���[�j���Ԃ��Ă����ꍇ�͒����Ƀ\�t�g���I�����Ȃ���΂Ȃ�܂���B

stat�̖߂�l
�O�F����
�|�P�F�G���[����

%sample
; �T���v���X�N���v�g ���L��

%href
DxLib_End

%group
�g�p�K�{�֐�

%index
DxLib_End
���C�u�����g�p�̏I���֐�

%prm
; �p�����[�^���X�g ���L��
; �p�����[�^������ ���L��

%inst
�@�c�w���C�u�����̎g�p���I������֐��ł��B
�c�w���C�u�������g�p���Ă���\�t�g�͂��ׂă\�t�g���I��������O�ɂ��̊֐����ĂԕK�v������܂��B
�@�����ɁA�c�w���C�u�����̓\�t�g�̃E�C���h�E�̐�������Ă���̂ŁA���̊֐����Ăяo������͂Ȃ�ׂ������Ƀv���O�������I�������Ȃ���΂Ȃ�܂���B

stat�̖߂�l
�O�F����
�|�P�F�G���[����

%sample
; �T���v���X�N���v�g ���L��

%href
DxLib_Init

%group
�g�p�K�{�֐�

%index
ChangeWindowMode
�E�C���h�E���[�h�E�t���X�N���[�����[�h�̕ύX���s��

%prm
Flag
Flag (int) : �E�C���h�E���[�h�ŋN�����邩�̃t���O���
	TRUE : �E�C���h���[�h�ŋN��
	FALSE : �t���X�N���[�����[�h�ŋN���i�f�t�H���g�j

%inst
�@�\�t�g���E�C���h�E���[�h�Ŏ��s���邩�A�t���X�N���[�����[�h�Ŏ��s����̂���ύX���܂��B ��ʂɂ̓E�C���h�E���[�h���t���X�N���[���Ŏ��s���������������͗ǂ��Ȃ�̂ł����A �\�t�g�̃f�o�b�O��Ƃ��t���X�N���[���ōs���̂͂ǂ����Ă�����Ȃ̂ŁA ���쒆�̂݃E�C���h�E���[�h�Ŏ��s���A�����o�[�W�����ł̓t���X�N���[�����s�ɂ���A ���̗p�r�Ɏg���܂��B
�@�����\�t�g���ŃE�C���h�E���[�h�ƃt���X�N���[�����[�h�̕ύX���o����悤�ɂ���ꍇ�̓t���X�N���[���ƃE�C���h�E���[�h�Ƃ̃J���[�r�b�g���̈Ⴂ�ɋC��t����K�v������܂��B ���ɂQ�T�U�F���[�h�Ŏ��s����\�t�g�ł̔z�z�łł̕ύX�͖]�܂�������܂���B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�E�C���h�E���[�h�E�t���X�N���[�����[�h�ύX�p�֐�

%index
SetUserWindow
�c�w���C�u�����Ŏg�p����E�C���h�E�̃n���h�����Z�b�g����

%prm
hwnd
hwnd (int) : �E�C���h�E�̃n���h��

%inst
�@�c�w���C�u�����Ŏg�p����E�C���h�E�̃n���h�����Z�b�g���܂��B( DxLib_Init �����s����ȑO�ł̂ݗL�� )
�@�Ƃ��ɗ��R���Ȃ�����hwnd���g�p���Ă��������B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�B���֐�

%index
SetUserWindowMessageProcessDXLibFlag
���b�Z�[�W���[�v�������c�w���C�u�����ōs�����ǂ�����ݒ肷��

%prm
Flag
Flag (int) : ���b�Z�[�W���[�v�����ݒ�̃t���O
	TRUE : �c�w���C�u�����ōs��( �f�t�H���g )
	FALSE : �c�w���C�u�����ł͍s��Ȃ�

%inst
�@SetUseWindow �Őݒ肵���E�C���h�E�̃��b�Z�[�W���[�v�������c�w���C�u�����ōs�����ǂ�����ݒ肵�܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�B���֐�

%index
SetUseFPUPreserveFlag
FPU�̐��x�̐ݒ������

%prm
Flag
Flag (int) : ���x�̃t���O
	TRUE : �g�p����(���x�������Ȃ�)
	FALSE : �g�p���Ȃ�(���x�𗎂Ƃ�(�f�t�H���g))

%inst
�@FPU�̐��x�𗎂Ƃ��Ȃ��ݒ���g�p���邩�ǂ�����ݒ肷�邵�܂��B
�@DxLib_Init ���Ăяo���O�̂ݗL��

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�B���֐�

%index
SetDrawScreen
�`���O���t�B�b�N�̈�̎w��

%prm
DrawScreen
DrawScreen (int) : �`�悷��ΏۂƂȂ�O���t�B�b�N�̈���w�肵�܂��B
	DX_SCREEN_FRONT : �\�̉�ʁi�\������Ă����ʁj
	DX_SCREEN_BACK : ���̉�ʁi�\������Ă��Ȃ���ʁj

%inst
�@�A�j���[�V��������摜��\������Ƃ��ď�ɕ\������Ă����ʂɑ΂��ĕ`�揈�����s���ƁA ��ʂ֕`�悵�Ă���Ƃ����A�`�悵�����̂������Ă���Œ��̏�Ԃ������Ă��܂����ʂƂ��ĉ�ʂ�������Ă���悤�Ɍ����Ă��܂��܂��B �i �� �c �O���t�B�b�N�J�[�h�ɂ���Ă͂�����Ȃ��ꍇ������܂����A��ʓI�ɂ͂�����܂� �j
�@�����ŉ�ʂւ̕`�撆�͌����Ȃ��悤�ɂ��āA�`�揈�����I������Ō�����悤�ɂ���Ή�ʂ̂�����͏�����͂��ł��B ������������邽�߂ɂ��̊֐�������܂��B
�@�f�t�H���g�ł͕`���� DX_SCREEN_FRONT �i�\�̉�ʁj�ƂȂ��Ă��ĕ`�揈�����������Ă��܂���ʂ͂�����܂����A �`���� DX_SCREEN_BACK �i���̉�ʁj���w�肷��ƕ`��悪���i�͌����Ȃ����̉�ʂɑ΂��čs���܂��B
�@�`�悪�I������Ŏ��Ɏ����֐��wScreenFlip�x���Ăяo���΁A ����ʂ̓��e���\��ʂɔ��f����A����ʂɕ`�悵�Ă������e�����ۂɕ\������܂��B
�@���̊֐��͂��̕`�����ǂ���̉�ʂɂ��邩�A���w�肷�邽�߂̊֐��ł��B

�@���̑��AMakeScreen �֐��ō쐬�����O���t�B�b�N�n���h���������ɓn���ƁA ���̃O���t�B�b�N�n���h����`���Ƃ��邱�Ƃ��ł��܂��B
( �O���t�B�b�N�n���h����`���Ƃ��邱�Ƃ��ł��邱�Ƃ̗��_�ɂ��Ă� MakeScreen �֐��̉�����Q�Ƃ��Ă������� )

�����Ӂ�
�@���̊֐����g�p���ĕ`��Ώۂ�ύX����ƁASetDrawArea �Őݒ肵���`��\�͈͂ƁA �R�c�`��Ŏg�p����J�����̐ݒ�i SetCameraPositionAndTarget_UpVecY �Ȃǂ̊֐��Ńp�����[�^��ݒ肷�� )�����Z�b�g����܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���̑���ʑ���n�֐�

%index
SetGraphMode
��ʃ��[�h�̕ύX

%prm
Sizex, SizeY, ColorBitNum
SizeX , SizeY (int) : ��ʂ̉𑜓x
ColorBitNum (int) : �J���[�r�b�g��

%inst
�@��ʂ̉𑜓x��ő�\���F����ύX���܂��B

�����Ӂ�
�@���̊֐������s����ƃ��[�h�������ׂẴO���t�B�b�N�n���h���ƂR�c���f���n���h���A �쐬�����t�H���g�n���h���͎����I�ɍ폜����ASetDrawArea, SetDrawScreen, SetDrawMode, SetDrawBlendMode, SetDrawBright ���̕`��Ɋ֌W����ݒ���s���֐��ɂ��ݒ���S�ď�����Ԃɖ߂�܂��̂ŁA ��ʃ��[�h�ύX�� LoadGraph�֐��� CreateFontToHandle�֐����ōēx�n���h�����쐬�������A �`��\�̈�A�`��Ώۉ�ʓ��̊e��`��n�̐ݒ���ēx�s���K�v������܂��B

�@��ʂ̉𑜓x�͉�ʂ�\������̂Ɏg�p�����h�b�g�i�_�j�̐��ł��B
�@�p�\�R���̃��j�^�[���Ή����Ă���𑜓x�Ƃ��Ă͎��

�@�@640�~480�@800�~600�@1024�~768�@1280�~1024�@1280�~720�@1920�~1080

�@������܂��B�i �f�t�H���g��640�~480�ł� �j�B
�@��ʂ̉𑜓x���グ��Ɖ�ʂ��\������_�̐��������\���͂������Ȃ�܂����A ���̂Ԃ��ʂ̋L���ɕK�v�ȃ������e�ʂ������܂��̂Œ��ӂ��Ă��������B

�@�w�肵���𑜓x�����j�^�[��O���t�B�b�N�X�f�o�C�X���Ή����Ă��Ȃ��ꍇ�� SetFullScreenResolutionMode �Őݒ�ł���𑜓x���[�h�ɉ����ă��j�^�[���Ή����Ă���ʂ̉𑜓x�Ɋg�傳��ĕ\������܂��B ( �ʂ̉𑜓x�Ɋg�傷�鏈���͏������ׂ��������������̂ŁA���j�^�[���Ή����Ă���𑜓x���g�p���邱�Ƃ������߂��܂� )

�@�J���[�r�b�g���Ƃ͎g�p�����ʂ̐F�̐��ł��B
�@�r�b�g�Ƃ͂Q�i���̂P���̎��ŁA�P�U�r�b�g�łU�T�T�R�U�F�iTrue�J���[�j �Q�S�r�b�g�łP�U�V�V���F�i�t���J���[�j�\���ł��܂��B �r�b�g�����オ��Ώオ��قǕ\���ł���F�̐��͑����܂����A���̕��K�v�ȃf�[�^�ʂ������܂��̂Œ��ӂ��Ă��������B

�@���̃��C�u�����Ŏw��ł���J���[�r�b�g���͂P�U�r�b�g�ƂR�Q�r�b�g�̓�ɂȂ�܂��B

�@�w�肵���J���[�r�b�g���ɑΉ�����R�c�@�\�������Ȃ��O���t�B�b�N�X�f�o�C�X�ł͂R�c�@�\���g�p�s�ɂȂ�܂��B
�@�P�U�r�b�g�͂c�w���C�u�����̕W���F�r�b�g���ŁA�U�T�T�R�U�F���g���ĉ摜��\�����܂��B
�@�R�Q�r�b�g�͓����I�ɂ͂Q�S�r�b�g���g���ĂP�U�U�V���F�ŉ摜��\�����܂��B�i �c��̂W�r�b�g�͎g�p����܂��� �j ���̃��[�h��I������ƃO���t�B�b�N�f�[�^�̃T�C�Y���P�Ubit���[�h�ɔ�ׂĂQ�{�ɂȂ�܂��B

�@�߂�l�́A�ύX�����������ꍇ�� DX_CAHNGESCREEN_OK�@�����s���Č��̉�ʃ��[�h�ɖ߂��ꂽ�ꍇ�� DX_CHANGESCREEN_RETURN �����s���Č��̉�ʂɂ��߂����A�f�t�H���g�̉�ʃ��[�h( 640x480 16bit color)�ɕύX���ꂽ�ꍇ�� DX_CHANGESCREEN_DEFAULT ���A ���ꂷ������s�����ꍇ�̓\�t�g�������I�����܂��B

�@�Ȃ����̊֐��� DxLib_Init �֐����g�p����O�ɌĂяo�����Ƃɂ�菉����Ԃ̉�ʃ��[�h��ݒ肷�邱�Ƃ��o���܂��B
���̏ꍇ�͉�ʃ��[�h�̕ύX�� DxLib_Init ���Ă΂ꂽ�ۂɍs����̂ŉ�ʃ��[�h�̕ύX���������邩�ǂ��������炸�A �֐��͕K�� DX_CHANGESCREEN_OK ��Ԃ��܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���̑���ʑ���n�֐�

%index
MV1LoadModel
���f���̓ǂݍ���

%prm
FileName
FileName (str) �F�@���[�h����R�c���f���t�@�C���̃p�X������̃A�h���X

%inst
�@�R�c���f���t�@�C�����������Ƀ��[�h���܂��B
�c�w���C�u�����łR�c���f���������Ƃ��ɕK���g�p����֐��ł��B
�@���̊֐�����������ƃ��f���n���h���Ƃ������̂��Ԃ��Ă��܂��B
����̓������ɓǂݍ��񂾂R�c���f���t�@�C���̎��ʔԍ��� int �^�̐��l�ł��B
�@�ǂݍ��񂾃��f���������ۂɂ��̎��ʔԍ����g�p���邱�ƂɂȂ�܂��̂ŁA MV1LoadModel �̖߂�l�͕K���Ȃ�炩�̕ϐ��Ɋi�[���Ă����K�v������܂��B

��@�@DxChara.x �����[�h���āA�߂�l�ł��郂�f���n���h����
�@�@int �^�ϐ� MHandle �ɕۑ����܂�

    MV1LoadModel "DxChara.x"

    MHandle = stat

�ǂݍ��ނ��Ƃ̂ł��郂�f���t�@�C���`���� x, mqo, mv1, pmd( + vmd ), pmx( + vmd ) �̂S��ނł��B
( �A���Apmx �� pmd �����̋@�\�������g�p���Ă����ꍇ�̂ݐ���ɓǂݍ��߂鉼�Ή���Ԃł� )

�@���A�`����ƃA�j���[�V����(�A�j���[�V����)���̓t�@�C���̓��e�ʂ�ɓǂݍ��߂܂����A �}�e���A�����͊e�t�@�C���`�����ꂼ��ňقȂ�\�������Ă�����̂𖳗���c�w���C�u�����̃}�e���A���\���ň������Ƃ��邽�߁A ���̏ꍇ���f�����O�\�t�g��Ƃ͈قȂ��������ڂɂȂ��Ă��܂��܂��B
�@�Ȃ̂ŁA���̂悤�ȏꍇ�͂c�w���C�u�����ɍ��킹�ă��f���t�@�C���̃}�e���A���𒲐����Ă����������A �c�w���C�u�����̂R�c�c�[���Ń}�e���A���𒲐����Ă��烉�C�u�����̐�p�`���ł��� mv1 �`���ŕۑ����āA���̃t�@�C�����g���Ă����������ƂɂȂ�܂��B
�@�܂��A���f���t�@�C���Ŏg�p����Ă���e�N�X�`���̓��f���t�@�C���̒��ɂ͊܂܂�܂���̂ŁA ���f���t�@�C���Ŏw�肳��Ă���t�H���_�Ƀe�N�X�`���t�@�C�����i�[���Ă����K�v������܂��B


MMD( MikuMikuDance )�̃��f���t�@�C��( pmd or pmx )�ƃ��[�V�����t�@�C��( vmd )�ɂ��āB

���Ή��x�ɂ��ā�

�@�ꉞ���[�V�����̍Đ��Ǝ��ɔ����h�j�A�������Z�ɑΉ����Ă��܂����A���S�ɖ{�� MikuMikuDance �Ɠ����Ƃ����킯�ł͂���܂���B


���ǂݍ��݂ɂ��ā�

�@MikuMikuDance �ł̓g�D�[���p�̃e�N�X�`��( toon01.bmp �� )�̓��f���t�@�C��( pmd or pmx )�����݂���e�N�X�`���Ƃ͕ʂ̃t�H���_�ɂ����Ă����Ȃ��ǂݍ��ނ��Ƃ��o���܂����A �c�w���C�u�����ł̓g�D�[���p�̃e�N�X�`�������f���t�@�C��( pmd or pmx )�Ɠ����t�H���_�Ɋi�[���Ă����K�v������܂��B( �g�D�[���p�̃f�t�H���g�e�N�X�`���� MikuMikuDance �� Data�t�H���_�̒��ɂ���܂� )

�@�܂��A�c�w���C�u�����ł� MMD �̃��f���t�@�C���`��( pmd or pmx )�ƃ��[�V�����t�@�C���`��( vmd )�̓ǂݍ��݂ɑΉ����Ă��܂����A ���[�V�����t�@�C��( vmd )�̓��f���t�@�C��( pmd or pmx )��ǂݍ��ލۂɈꏏ�ɓǂݍ��܂��悤�ɂȂ��Ă��܂��B
�@�����AMV1LoadModel �ɂ̓��[�V�����t�@�C���̃t�@�C������n�������͂���܂���̂ŁA ���̂悤�ȃ��[���Ń��f���t�@�C��( pmd or pmx )�p�̃��[�V�����t�@�C�����������܂��B

�@�P�D���f���t�@�C�����ɂR���̔ԍ����������[�V�����t�@�C�������邩�������āA��������ǂݍ���
�@�@�@( ��������ԍ��� 000 ���� )

�@�@�@�@�@�Ⴆ�΁AMiku.pmd ( �Ⴕ���� Miku.pmx ) �Ƃ����t�@�C������ FileName �Ƃ��ēn�����ꍇ�́A
�@�@�@�@�@�ŏ��� Miku000.vmd �Ƃ������[�V�����t�@�C�������݂��邩���ׂ܂��B

�@�Q�D��������ԍ���000���珇�ɂP�Â��₵�Ă����A���݂��Ȃ��t�@�C�����ɂȂ�܂œǂݍ���

�@�@�@�@�@�Ⴆ�΁AMiku000.vmd�AMiku001.vmd�AMiku002.vmd �Ɛ����̌q�������R�̃��[�V�����t�@�C����
�@�@�@�@�@�������ꍇ�͂R�Ƃ��ǂݍ��܂�܂��B
�@�@�@�@�@���� Miku000.vmd, Miku001.vmd, Miku005.vmd �̂悤�ɁA�ԍ����r�؂�Ă�����AMiku000.vmd ��
�@�@�@�@�@Miku001.vmd �̓�����ǂݍ��܂�AMiku005.vmd �͓ǂݍ��܂�܂���B

�@���A�ǂݍ��ݎ��ɂh�j�v�Z���s���܂��̂ŁAx�t�@�C����mv1�t�@�C���ɔ�ׂēǂݍ��ݎ��Ԃ����ɒ����Ȃ��Ă��܂��B


�����[�v�Đ����郂�[�V�����ɂ��ā�

�@���[�V�����̒��ɂ͕����⑖��Ƃ��������[�v�����čĐ����s���p�r�̃��[�V����������Ǝv���܂��B

�@���̂悤�ȃ��[�V������ vmd �t�@�C���́A���ǂݍ��݂ɂ��ā��̉���ɂ������t�@�C�����̕t�����ɂ���R���̃��[�V�����̔ԍ��̍Ō�ɔ��p�� L �����Ă��������B

��FMiku000.vmd �` Miku002.vmd �̂R�̃t�@�C��������AMiku000.vmd �� Miku002.vmd ��
�@�@���[�v�Đ��p�r�̃��[�V�����̏ꍇ

�@Miku000.vmd�@Miku001.vmd�@Miku002.vmd

�@�@�@�@�@�@�@�@�@��

�@Miku000L.vmd�@Miku001.vmd�@Miku002L.vmd


�@L ��t���邱�ƂŊ֐� MV1SetLoadModelUsePhysicsMode �̐ݒ肪�u�ǂݍ��ݎ��ɕ������Z���s���v DX_LOADMODEL_PHYSICS_LOADCALC �ƂȂ��Ă���ꍇ( �f�t�H���g�̐ݒ�� DX_LOADMODEL_PHYSICS_LOADCALC �ł� )�̕������Z�����[�v�Đ��p�r�̃��[�V�����p�Ƀ��[�V�����̍ŏ��ƍŌ�̃t���[�����Y��Ɍq���莩�R�ȃ��[�v�Đ����ł���悤�ɏ�������܂��B
�@�t�Ƀ��[�v�p�r�̃��[�V�����ł͂Ȃ��̂� L ��t���Ă��܂��ƃ��[�V�����̍Ō�̕������ςɂȂ��Ă��܂��̂Œ��ӂ��Ă��������B
�@�܂��AL ��t���Ă��������Z�ɂ���ē��������̓������������Ƃ��͂���ς胋�[�v���ɕs���R�Ȍ������ɂȂ��Ă��܂��܂��̂ŁA ���̍ۂ̓��A���^�C���������Z�̐ݒ�ł��� DX_LOADMODEL_PHYSICS_REALTIME �����g�����������B


�����[�V�����̍Đ��ɂ��ā�

�@���[�V�����̍Đ��͑��̌`���Ɠ����悤�� MV1AttachAnim ���g�p���܂�( AnimIndex �� vmd �ɂ��Ă���ԍ����w�肵�܂� )
�@�t���[���\���̈Ⴄ���̃��f���̃��[�V�������g�p����ꍇ�� MV1AttachAnim �� NameCheck �� TRUE �ɂ��Ă��������B

stat�̖߂�l
�|�P�@�@�@�@�F�@�G���[����
�|�P�ȊO�@�F�@���f���̃n���h��

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�R�c���f���֌W�̊֐�

%index
MV1AttachAnim
�A�j���[�V�������A�^�b�`����

%prm
MHandle, AnimIndex, AnimSrcMhandle, NameCheck
MHandle (int) �F �A�j���[�V�������A�^�b�`���郂�f���̃n���h��
AnimIndex (int) �F �A�^�b�`����A�j���[�V�����ԍ�
AnimSrcMHandle (int) �F �A�^�b�`����A�j���[�V�����������Ă��郂�f���̃n���h��
�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@( -1 ��n���� MHandle �Ɠ������f���n���h�����g�p����܂� )
NameCheck (int) �F AnimSrcMHandle �� -1 �ȊO�̏ꍇ�ɃA�^�b�`����A�j���[�V������
�@�@�@�@�@�@�@�@�@�@�@�@�@�@�t���[���̖��O�ƃA�^�b�`����鑤�̃��f���̃t���[���̖��O��
�@�@�@�@�@�@�@�@�@�@�@�@�@�@��v���Ă��Ȃ��ꍇ�A�^�b�`���Ȃ����ǂ���
�@�@�@�@�@�@�@�@�@�@�@�@�@�@( TRUE�F�A�^�b�`���Ȃ��@FALSE�F�A�^�b�`���� )
�@�@�@�@�@�@�@�@�@�@�@�@�@�@���̈����� AnimSrcMHandle �� -1 �̏ꍇ�͖�������܂�

%inst
�@MHandle �̃��f���n���h�����������f���ɃA�j���[�V�������A�^�b�`���܂��B

�@�A�j���[�V�����Ƃ͎�Ƀ��f�����̃t���[���ɑ΂��铮���̃f�[�^�ŁA �L�����N�^�[���f�����������葖������Ƃ������\��������ۂɎg�p���܂��B ( �A�j���[�V�����̃f�[�^���̂͗\�߃A�j���[�V�����f�[�^���쐬�ł���\�t�g���g�p���č쐬���Ă����K�v������܂� )

�@�c�w���C�u�����ł̓A�j���[�V�������Đ�����ۂɁA �ŏ��ɍĐ��������A�j���[�V�������w�肷���Ɓu�A�j���[�V�����̃A�^�b�`�v������K�v������܂��B

�@�A�^�b�`���̂͊ȒP�ŁA���̊֐��ŃA�^�b�`�������A�j���[�V�����̔ԍ���n�������ł��B

�@�����āA�A�j���[�V�����̃A�^�b�`��������������Ɩ߂�l�Ƃ��ăA�^�b�`�ԍ����Ԃ��Ă��܂��B
�@����́u�n���h���v�ƌĂׂ�قǂ̂��̂ł͂���܂��񂪁A ���f���n���h���ȂǂƓ��l�ɃA�^�b�`�����A�j���[�V�����Ɋւ��鑀��͂��ׂĂ��̃A�^�b�`�ԍ����g�p���čs���܂��̂ŁA �A�^�b�`�ԍ������f���n���h���ȂǂƓ����悤�ɉ����̕ϐ��ɂƂ��Ă����K�v������܂��B

�@�A�j���[�V�����̓A�^�b�`�������_�ŃA�^�b�`�����A�j���[�V�����̃J�E���g�O�̏�Ԃ����f���ɔ��f����܂��B
�@�����āA�A�j���[�V�����̍Đ��J�E���g��ύX����ۂ� MV1SetAttachAnimTime ���g�p���܂��B
�@�c�w���C�u�����ɂ͎����I�ɃJ�E���^��i�߂�d�g�݂͂���܂���̂ŁA �uMV1PlayAnim�v �� �uMV1StopAnim�v �̂悤�Ȗ��̂̊֐��͑��݂��܂���B
�@�Ȃ̂ŁAMV1SetAttachAnimTime �ɃZ�b�g����Đ����Ԃ����X�ɐi�߂邱�ƂŃA�j���[�V�������Đ����܂��B

�@���� AnimSrcMHandle �͓����t���[���\���������� MHandle �Ƃ͕ʂ̃��f���̃��f���Ɋ܂܂�Ă���A�j���[�V������ MHandle �̃��f���ōĐ�����ۂɎg�p���܂��B ( �����A�j���[�V�������g�p���錩���ڂ̈Ⴄ�L�����N�^�[���f������������ꍇ�Ȃǂ́A �A�j���[�V���������̃t�@�C���ƌ����ڂ̈Ⴄ�t���[���\�����������b�V�������̃t�@�C����ʁX�ɗp�ӂ��邱�ƂŊe�L�����N�^�[���f���̃t�@�C���ɂ̓A�j���[�V�����f�[�^���܂߂�K�v���Ȃ��Ȃ�̂ŁA �A�j���[�V�����f�[�^�̗e�ʕ������f�[�^�T�C�Y���팸���邱�Ƃ��ł��܂� )

�@���� NameCheck �� AnimSrcMHandle ���g�p���ĕʂ̃��f���t�@�C���Ɋ܂܂��A�j���[�V�������A�^�b�`����ꍇ�ɁA �A�j���[�V�����f�[�^���̃t���[���̖��O�ƃ��[�V�������A�^�b�`���郂�f���f�[�^���̃t���[���̖��O���r���Ĉ�����ꍇ�̓A�^�b�`���Ȃ��Ƃ������������邩�ǂ������w�肷������ŁA TRUE ��n���Ɣ�r���āAFALSE ��n���Ɣ�r���܂���B

�@���O���r���Ȃ��ꍇ�̓t���[���̊K�w�\���Ɗe�K�w�̃t���[���̐������f�����ƃA�j���[�V�������Ŋ��S�Ɉ�v���Ă��Ȃ��Ɛ���ɃA�j���[�V�������Đ����邱�Ƃ��ł��܂���B
�@���O���r����ꍇ�̓t���[���̊K�w�\���Ɩ��O����v���Ă���΃��f�����ɂ����ăA�j���[�V�������ɖ����t���[���������Ă�����ɃA�j���[�V�������Đ����邱�Ƃ��ł��܂��B ( �������A�����K�w�ɓ����̃t���[������������ꍇ�͐���ɃA�j���[�V�������Đ����邱�Ƃ͂ł��܂��� )

�@���O��r���s�����ǂ����̓A�^�b�`���郂�[�V�����f�[�^�ɂ��킹�Ĕ��f���Ă��������B

stat�̖߂�l
�|�P�ȊO�F�A�j���[�V�����A�^�b�`�ԍ�
�|�P�F�G���[����

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�A�j���[�V�����֐�
%index
MV1SetAttachAnimTime
�A�^�b�`���Ă���A�j���[�V�����̍Đ����Ԃ�ݒ肷��

%prm
MHandle, AttachIndex, Time
MHandle (int) �F ���f���̃n���h��
AttachIndex (int) �F �Đ����Ԃ�ݒ肷��A�j���[�V�����̃A�^�b�`�ԍ�
Time (float) �F �Đ�����

%inst
�@MHandle �̃��f���n���h�����������f���ɃA�^�b�`�����A�j���[�V�����̍Đ����Ԃ�ݒ肵�܂��B

�@�c�w���C�u�����̃A�j���[�V�����@�\�ɂ͎����I�ɃA�j���[�V�������Đ�����@�\�͂���܂���̂ŁA ���̊֐����g�p���ăA�j���[�V�����̍Đ����Ԃ�ݒ肵�܂��B

�@�A�j���[�V�����̃L�[�����݂��Ȃ����Ԃ��w�肳�ꂽ�ꍇ�́A �w�肳�ꂽ���Ԃ̑O��ɂ���L�[����̕�Ԓl�����f���ɔ��f����܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�A�j���[�V�����֐�

%index
SetCameraNearFar
�J������ ��O�N���b�v������ ���N���b�v������ݒ肷��

%prm
Near, Far
float Near �F ��O( Near )�N���b�v����( 0.0f ���傫�� Far��菬���Ȓl )
float Far �F ��( Far )�N���b�v����( Near���傫�Ȓl )

%inst
�@�R�c��Ԃɉ�����`�悷��ۂɁA�J��������ǂꂾ�����ꂽ�Ƃ���( Near )����A �ǂ��܂�( Far )�̂��̂�`�悷�邩��ݒ肵�܂��B

�@���̊֐��̐ݒ�l�͂��Ȃ�d�v�ŁA�y�o�b�t�@�̐��x�ɂ��ւ���Ă��܂��̂Ŏg�p����R�c��Ԃ͈̔͂ɍ��킹�ēK�؂Ȓl��ݒ肷��K�v������܂��B
�@�Ⴆ�ΐl�ԃ��f���P�̑傫���� 200.0f ���炢�ő�̉�ʉ������� 10000.0f ���炢�܂ňړ����āA �w�i�͉�ʉ������� 15000.0f ���炢�܂ő��݂��āA���J�������� 100.0f ���߂��ɗ����猩���Ȃ��悤�ɂ������ꍇ��
SetCameraNearFar( 100.0f, 15000.0f ) ;
�@�Ƃ��܂��B

�@�l�ԃ��f����̑傫���������Ə������A1.0f ���炢�ŁA�w�i���f������ʉ������� 150.0f ���炢�܂ŕ`��ł���΂悢�ꍇ��
SetCameraNearFar( 1.0f, 150.0f ) ;
�@�Ƃ��܂��B

�@���݂ɁA�y�o�b�t�@�̎d�l�̊֌W�� Far �̐ݒ�l���d�v�ł��� Near �̐ݒ�l�͂��d�v�ŁA �Ⴆ�΁u�Ȃ�ׂ��J�����ɋ߂��Ă��`�悵�����v�Ƃ����l������
SetCameraNearFar( 0.00001f, 15000.0f ) ;
�@�Ƃ��Ă��܂����肷��Ƒ�ςł��A���ɂ���Ă̓J�������� 100.0f �ȏ㗣���ƕ`�悳��Ȃ��Ȃ����肵�܂��B
�@�Ȃ̂ŁANear �̒l�͕s�s���������͈͂łȂ�ׂ��傫�Ȓl���AFar �̒l�͕`�悵�����ŉ��̃��m�̂�菭���傫�Ȓl��ݒ肷��悤�ɂ��Ă��������B

�����Ӂ��Ȃ��A���̊֐��̐ݒ��SetDrawScreen�ASetGraphMode�AChangeWindowMode �̂����ꂩ���g�p�����Ƃ��Ƀ��Z�b�g����܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�J�����֐�

%index
ClearDrawScreen
��ʂɕ`���ꂽ���̂���������

%prm
hwnd
hwnd : �E�C���h�E�̃n���h��

%inst
�@�e��`��֐��ŕ`�悵���O���t�B�b�N�����ׂď�����ʂ����������܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���̑���ʑ���n�֐�


%index
SetCameraPositionAndTarget_UpVecY
�J�����̎��_�A�����_��ݒ肷��( ������͂x������Z�o )

%prm
PositionX, PositionY, PositionZ, TargetX, TargetY, TargetZ
float Position �F �J�����̈ʒu
float Target �F �J�����̒����_( ���Ă�����W )

%inst
�@�R�c�\�t�g�ƌ����Ύ��_���O���O���ς��܂��A ���̊֐��͂��̎��_�̈ʒu�ƌ��Ă�����̂��w�肷��֐��ł��B

�@�J�����̎p���́A���_�A�����_�ƁA���ƃJ�����̏����������Ό��܂�܂����A ���̊֐��̓J�����̏�������x���̃v���X��������{�I�ȃJ�����̏�����Ƃ��Ďp�����Z�o���܂��B

�@���݂ɁA�c�w���C�u�����ł͏�����Ԃł͎��_�̈ʒu�� x = 320.0f, y = 240.0f, z = ( ��ʂ̃T�C�Y�ɂ���ĕω� )�A �����_�̈ʒu�� x = 320.0f, y = 240.0f, z = 1.0f�A�J�����̏������ x = 0.0f, y = 1.0f, z = 0.0f�A �܂��ʂ̂w�x���ʏ�̒��S�ɋ��Ăy���̃v���X����������悤�ȃJ�����ɂȂ��Ă��܂��B

�@���� Position �ƈ��� Target �� float x, y, z �̎O�̃����o�ϐ��������Ă���\���� VECTOR �ŁA ���̂R�v�f���g�p���ĂR�����̍��W���w�肵�܂��B
�@�������\���̂��Ǝ��O�Ɉ����ɓn���\���̂�錾���ă����o�ϐ��ɍ��W�l�������āE�E�E�Ə������ʓ|�ł����A ������ x, y, z �̒l��n���Ƃ�������ɂ��� VECTOR �\���̂�߂�l�ŕԂ��Ă����֐� VGet ���g�p���邱�ƂŊȑf�Ɉ�����n�����Ƃ��ł��܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
�J�����֐�

%index
MV1SetPosition
���f���̍��W���Z�b�g����

%prm
MHandle, PositionX, PositionY, PositionZ
int�@�@�@�@MHandle �F ���f���̃n���h��
VECTOR Position �F ���f���ɃZ�b�g������W

%inst
�@MHandle �̃��f���n���h�����������f���̍��W���Z�b�g���܂��B

�@�O���t�B�b�N�n���h���ł͍��W��X�P�[���ƌ��������͉̂摜���̂ɂ͕ێ����� DrawGraph �� DrawExtendGraph �Ȃǂ̕`��֐����Ăԍۂɒ��ڎw�肵�Ă��܂������A �R�c���f���͐ݒ肷�ׂ���񂪑����A���ڂ̐��ɍ��킹�ĕ`��֐��̃o���G�[�V�����𑝂₵����Ƃ�ł��Ȃ����ƂɂȂ��Ă��܂��̂ŁA ���� MV1SetPosition �Ȃǂ̊֐��ŕ`����s���O�ɗ\�ߍ��W���]�l��ݒ肷��悤�ɂȂ��Ă��܂��B

�@���W�� VECTOR �^�̍\����( float x, y, z �������o�ϐ��Ɏ��\���� )�������ɂȂ��Ă��āA �������� VECTOR �\���̂��`���ă����o�ϐ��ɍ��W�l�������āA�Ƃ����菇�𓥂܂Ȃ���΂Ȃ�Ȃ��Ėʓ|�ł����A x, y, z �̒l�������œn���� VECTOR �\���̂�߂�l�Ƃ��ĕԂ��Ă���� VGet �֐����g�p����Ζʓ|�ł͂Ȃ��Ȃ�܂��B

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���f����{����֐�


%index
MV1DrawModel
���f����`�悷��

%prm
MHandle
int MHandle �F�@�`�悷�郂�f���̃n���h��

%inst
�@MHandle �̃��f���n���h�����������f������ʂɕ`�悵�܂��B
�@�p�r�̓��f������ʂɕ\���������ꍇ�ȂǁE�E�E

�@���݂ɁA�y�o�b�t�@���g�p��������̂R�c�����_�����O�ł͔������̕`�敨�͎��_����ł�����Ă�����̂��珇�ɕ`�悵�Ȃ��Ɛ���Ȍ����ڂɂȂ�܂���̂ŁA �����`�悷�郂�f���̒��ɔ������̕������܂܂�Ă���ꍇ�͎��_����̋������l��������� MV1DrawFrame �� MV1DrawMesh �ŕ`�悷��K�v������܂��B ( �c�w���C�u�������̂ɂy�\�[�g�̋@�\�͂���܂��� )

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���f���`��֐�


%index
ScreenFlip
�t���b�v�֐��A��ʂ̗��y�[�W�i���i�͕\������Ă��Ȃ��j�� �\�y�[�W�i���i�\������Ă���j�ɔ��f����

%prm
; �p�����[�^���X�g ���L��

%inst
�@���y�[�W�̓��e��\�y�[�W�ɔ��f���܂��B
(���y�[�W�A�\�y�[�W�ɂ��Ă� �wSetDrawScreen�x�֐��̉�����Q�Ƃ��Ă�������)

���c���̊֐����g�p������̗��y�[�W�̓��e�͊��ɂ���ĕς��܂��̂ŁAScreenFlip �֐����g�p������� ClearDrawScreen �����g�p���ė��y�[�W�����������ĉ�����

%sample
; �T���v���X�N���v�g ���L��

%href
; �֘A���� ���L��

%group
���̑���ʑ���n�֐�

