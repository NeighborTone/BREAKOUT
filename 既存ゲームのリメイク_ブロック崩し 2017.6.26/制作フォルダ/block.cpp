/*------------- http://akaguro.jp/breakout/breakout_index.shtml ���Q�l�A������������ς����B----
�T���v���̂܂܂��ƃ{�[���̑�����ς���Ƃ��蔲������������̂ŁA�{�[���̂��蔲�������P(�����B�B�B)�����B
-------------- �T���v���ɃR�����g����؂Ȃ��̂łقڂ��ׂĂ̏����ɃR�����g��t�����B ----------------
-------------- ��Փx��t�����B ----------------------------------------------------------------
*/
#define _USE_MATH_DEFINES
#include "DxLib.h"
#include <math.h>

#define BALL_SIZE 4			//�{�[���̑傫��
#define BAR_WIDIH 75		//�o�[�̉���
#define BAR_HEIGHT 10		//�o�[�̍���
#define FIELD_WIDIH 300		//�Q�[����ʂ̉����i�E�[�j
#define FIELD_HEIGHT 480	//�Q�[����ʂ̍����i���[�j
#define X_POSI 170			//�t�B�[���h�̕\���ʒu�𒲐�(�Q�[��������ꏊ(�t�B�[���h)��x���W)
#define BLOCK_WIDTH 50		//�u���b�N�̉���
#define BLOCK_HEIGHT 10		//�u���b�N�̍���


int bar_x;					//�o�[�̈ʒux
int bar_y;					//�o�[�̈ʒuy
double ball_x;				//�{�[���̈ʒux
double ball_y;				//�{�[���̈ʒuy
double ball_x_now;			//�u���b�N�ɓ�����O�̈ʒu���i�[
double ball_y_now;			//�u���b�N�ɓ�����O�̈ʒu���i�[
float degree;
double radian1 = M_PI / 180.0f * degree;	//�{�[���̔��ˊp�x
double speed;				//�{�[���̑���
int bar_speed;				//�o�[�̈ړ����x
int ball_start_flag;		//�{�[���̊J�n�t���O
int x_direction = 0;		//�{�[���̌���1��+����,-1��-����
int y_direction = 0;		//�{�[���̌���1��+����,-1��-����
int game_over_flag;			//�Q�[���I�[�o�[�t���O
int flag_count = 0;			//�Q�[���I�[�o�[�̃t���O1�ŏI��
bool game;					//�Q�[�����n�܂��Ă��邩�̃t���O

int key[256];
int White = GetColor(255, 255, 255);	//�F�̃f�[�^��
int Black = GetColor(0, 0, 0);			//�F�̃f�[�^��
int RED = GetColor(255, 0, 0);			//�F�̃f�[�^��
int Green = GetColor(0, 255, 0);		//�F�̃f�[�^��
int Blue = GetColor(0, 0, 255);			//�F�̃f�[�^��
int yellow = GetColor(255, 255, 0);		//�F�̃f�[�^���F
int skyblue = GetColor(0, 255, 255);	//�F�̃f�[�^��F
float	r = 0,							//�F�̃f�[�^���F�p
		g = 0,
		b = 0;


int i,				//���[�v�p�ϐ�
	j;				//���[�v�p�ϐ�
int count = 0;		//�u���b�N�̕\���p�ϐ�


					//�u���b�N�̔z�u
int block_map[6][6] = {
	{ 1,2,3,4,5,4 },
	{ 5,4,3,2,1,5 },
	{ 3,4,2,3,4,2 },
	{ 1,2,3,1,2,3 },
	{ 2,4,1,2,4,1 },
	{ 4,1,2,3,1,4 }
};

//�u���b�N�̃f�[�^
struct BLOCKS
{
	int x;		//�u���b�N��x���W
	int y;		//�u���b�N��y���W
	int flag;	//�u���b�N�̐����t���O
	int color;	//�u���b�N�̐F
};
BLOCKS block[36];
//-----------------

int gpUpdatekey()
{
	char tmpkey[256];				//���݂̃L�[�̓��͏�Ԃ��i�[
	GetHitKeyStateAll(tmpkey);		//���ׂẴL�[�̓��͏�Ԃ𓾂�
	for (int i = 0; i < 256; ++i)
	{
		if (tmpkey[i] != 0)
		{
			++key[i];
		}
		else
		{
			key[i] = 0;
		}
	}
	return 0;
}

void my_init_variable()	//����������
{
	bar_x = 110;							//�o�[��x���W
	bar_y = FIELD_HEIGHT - BAR_HEIGHT;		//�o�[��y���W
	bar_speed = 8;					//�o�[�̑���
	ball_x = 145;					//�{�[���̏����ʒu
	ball_y = (FIELD_HEIGHT - BAR_HEIGHT) - BALL_SIZE;
	speed = 4.0;					//�{�[���̑���
	ball_start_flag = 0;			//�{�[���̏��
	degree = -45;
	radian1 = M_PI / 180.0f * degree;	//�{�[���̔��ˊp�x
	x_direction = 0;				//�{�[���̌���1��+����
	y_direction = 0;				//�{�[���̌���1��+����
	game_over_flag = 0;				//1�ŃQ�[���I�[�o�[
	game = false;					//false�̓Q�[�����n�܂��Ă��Ȃ�
}
void my_init_blocks()	//�u���b�N�̏�����
{
	for (i = 0; i < 6; ++i)		//�Q���z��̒��g����
	{
		for (j = 0; j < 6; ++j)		//2���z��̊O������
		{
			if (block_map[i][j] != 0)	//�u���b�N�������Ă���Ƃ�
			{
				block[count].x = (BLOCK_WIDTH * j);		//�u���b�N��x���W(���ɕ��ׂ�)
				block[count].y = ((BLOCK_HEIGHT + 10)* i);	//�u���b�N��y���W(�c�ɕ��ׂ�)
				block[count].flag = 1;		//���ׂẴu���b�N�Ƀt���O��p��
			}
			switch (block_map[i][j])	//�u���b�N�̐F��count�ŕύX
			{
			case 1:
				block[count].color = RED;		//�z��ԍ�1�Ő�
				break;
			case 2:
				block[count].color = Green;		//�z��ԍ�2�ŗ�
				break;
			case 3:
				block[count].color = Blue;		//�z��ԍ�3�Ő�
				break;
			case 4:
				block[count].color = skyblue;   //�z��ԍ�4�Ő��F	
				break;
			case 5:
				block[count].color = White;   //�z��ԍ�5�Ő��F	
				break;
			default:
				break;
			}
			++count;		//���[�v�ɓ���邱�ƂŔԍ��𑝂₷
		}
	}
}
void Sound()		//SE�Ȃǂ̏���
{
	//���̉���
	int SE1 = LoadSoundMem("�T�E���h/cat1.wav");		//SE�̃��[�h
	int SE2 = LoadSoundMem("�T�E���h/cat2.wav");
	int SE3 = LoadSoundMem("�T�E���h/kan.ogg");
	if (key[KEY_INPUT_T] == 1)	//T�L�[�������ꂽ�u��
	{
		PlaySoundMem(SE1, DX_PLAYTYPE_BACK);	//�L�̐�
	}
	if (ball_y > (500 + BALL_SIZE) && game_over_flag == 0)	//�{�[�������������T�Ԃ̂�
	{
		PlaySoundMem(SE2, DX_PLAYTYPE_BACK);	//�L�̐�
	}
	if (block[count].flag == 0 || ball_x >= (FIELD_WIDIH - 7.f) || ball_x <= 7.0f || ball_x > bar_x && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y || ball_y <= 6.0f)	//�����ɂԂ�������
	{
		PlaySoundMem(SE3, DX_PLAYTYPE_BACK);	//SE
		ChangeVolumeSoundMem(255, SE3);			//SE3�̉���
		if (game_over_flag == 1 && game_over_flag == 3)
		{
			StopSoundMem(SE3);

		}
	}
	
}

void game_start_check()
{
	if (game == false)	//�Q�[�����n�܂��Ă��Ȃ��Ƃ�
	{
		if (key[KEY_INPUT_RETURN] == 1 || key[KEY_INPUT_Z] == 1 || key[KEY_INPUT_X] == 1 || key[KEY_INPUT_E] == 1)
		{
			game = true;
			x_direction = 1;
			y_direction = -1;
		}
	}

}

void my_move_ball()	//�{�[���̓���
{
	
	/*�g�`�q�c���[�h�A�G���^�[�L�[*/
	if (ball_start_flag == 1)		//�{�[���̃t���O�������Ă����瓮��
	{
		if (x_direction == 1)			//�{�[�����E�[�łȂ��Ȃ�
		{
			ball_x += speed;		//�E�Ɉړ�
		}
		else
		{
			ball_x -= speed;		//���Ɉړ�
		}
		if (y_direction == 1)			//�{�[������[�łȂ��Ȃ�
		{
			ball_y += speed;		//���Ɉړ�
		}
		else
		{
			ball_y -= speed;		//��Ɉړ�
		}
	}
	/*�m�n�q�l�`�k���[�h�AZ�L�[*/
	if (ball_start_flag == 2)		//�{�[���̃t���O�������Ă����瓮��
	{

		if (x_direction == 1)			//�{�[�������[�Ȃ�
		{
			ball_x += speed / 1.4f;		//�E�Ɉړ�
		}
		else
		{
			ball_x -= speed / 1.4f;		//���Ɉړ�
		}
		if (y_direction == 1)			//�{�[������[�Ȃ�
		{
			ball_y += speed / 1.4f;		//���Ɉړ�
		}
		else
		{
			ball_y -= speed / 1.4f;		//��Ɉړ�
		}
	}
	/*�d�`�r�x���[�h�AX�L�[*/
	if (ball_start_flag == 3)		//�{�[���̃t���O�������Ă����瓮��
	{
		if (x_direction == 1)			//�{�[�����E�[�łȂ��Ȃ�
		{
			ball_x += speed / 2.5f;		//�E�Ɉړ�
		}
		else
		{
			ball_x -= speed / 2.5f;		//���Ɉړ�
		}
		if (y_direction == 1)			//�{�[������[�łȂ��Ȃ�
		{
			ball_y += speed / 2.5f;		//���Ɉړ�
		}
		else
		{
			ball_y -= speed / 2.5f;		//��Ɉړ�
		}
	}
	/*�d�w�s�q�d�l�d���[�h�AE�L�[*/
	if (ball_start_flag == 4)		//�{�[���̃t���O�������Ă����瓮��
	{
		if (x_direction == 1)			//�{�[�����E�[�łȂ��Ȃ�
		{
			ball_x += speed + 2;		//�E�Ɉړ�
		}
		else
		{
			ball_x -= speed + 2;		//���Ɉړ�
		}
		if (y_direction == 1)			//�{�[������[�łȂ��Ȃ�
		{
			ball_y += speed + 2;		//���Ɉړ�
		}
		else
		{
			ball_y -= speed + 2;		//��Ɉړ�
		}
	}
	
}

void my_move_bar()	//�o�[�̈ړ�������
{
	
	if (key[KEY_INPUT_RETURN] == 1)	//�G���^�[�L�[�������ꂽ��
	{
		ball_start_flag = 1;		//�{�[���𓮂���
	}
	if (key[KEY_INPUT_Z] == 1)		//Z�L�[�������ꂽ��
	{
		ball_start_flag = 2;		//�{�[���𓮂���
	}
	if (key[KEY_INPUT_X] == 1)		//X�L�[�������ꂽ��
	{
		ball_start_flag = 3;		//�{�[���𓮂���
	}
	if (key[KEY_INPUT_E] == 1)		//E�L�[�������ꂽ��
	{
		ball_start_flag = 4;		//�{�[���𓮂���
	}
	if (ball_start_flag == 1 || ball_start_flag == 2 || ball_start_flag == 3)		//�{�[���̃t���O�������Ă����瓮��
	{
		if (key[KEY_INPUT_RIGHT] > 0 && (bar_x + BAR_WIDIH) < FIELD_WIDIH - 3)	//�E�L�[��������Ă��邩�E�[�łȂ��Ȃ�(���ꂽ��������)
		{
			bar_x += bar_speed;			//�o�[���E�Ɉړ�
		}
		if (key[KEY_INPUT_LEFT] > 0 && bar_x > 0)	//���L�[��������Ă��邩���[�̎�(0�͍��[)
		{
			bar_x -= bar_speed;			//�o�[�����Ɉړ�
		}
	}

	if (ball_start_flag == 4)		//�d�w�s�q�d�l�d���[�h��
	{
		if (key[KEY_INPUT_RIGHT] > 0 && (bar_x + BAR_WIDIH) < FIELD_WIDIH)	//�E�L�[��������Ă��邩�E�[�łȂ��Ȃ�
		{
			bar_x += bar_speed + 4;				//�o�[���E�Ɉړ�			
		}
		if (key[KEY_INPUT_LEFT] > 0 && bar_x > 0)	//���L�[��������Ă��邩���[�̎�(0�͍��[)
		{
			bar_x -= bar_speed + 4;				//�o�[�����Ɉړ�
		}
	}
}
float get_random(int num)
{
	return float(rand() % (num) * -1);
}

void my_cd_check()	//�{�[���ƃo�[�ƃu���b�N�̓����蔻��ƃ{�[���̔���
{
	//--- �{�[���̔��ˏ��� ---(�҂����肾�Ƃ��蔲����̂ŁA�������炷)
	if (ball_x >= (FIELD_WIDIH - 7.f))		//�{�[�����E�[�ɗ�����
	{
		x_direction = -1;			//������ς���i���j
		Sound();
		if (ball_y  >= 0 && ball_y <= 180)		//��ʂ̏�̂ق���
		{
			degree = -45;
		}
		if (ball_y >= 181 && ball_y <= 280)		//��ʂ̏c�����ʂ�
		{
			degree = get_random(50);
		}
		if (ball_y >= 281 && ball_y <= 480)	//��ʂ̉�����
		{
			degree = -70;
		}
		
	}
	if (ball_x <= 7.0f && x_direction == -1)				//�{�[�������[�ɗ�����
	{
		x_direction = 1;			//������ς���i�E�j
		Sound();
		if (ball_y >= 0 && ball_y <= 180)		
		{
			degree = -45;
		}
		if (ball_y >= 181 && ball_y <= 280)		//��ʂ̔����ʂ�
		{
			degree = get_random(60);
		}
		if (ball_y >= 281 && ball_y <= 480)	//��ʂ̉�����
		{
			degree = -80;
		}
	}
	if (ball_y <= 6.0f && y_direction == -1)				//�{�[������[�ɗ�����
	{
		y_direction = 1;			//������ς���i���j
		Sound();
	}
	
	//--- �{�[���ƃo�[�̔��� ---
	if (ball_x > bar_x && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y)	//�{�[����x���o�[�̒����̎�
	{
		y_direction = -1;
		Sound();
		degree = -80;		
	}
	if (ball_x > bar_x && ball_x <= (bar_x + 20) && ball_y >= bar_y)			//�o�[�̍����ɓ���������
	{
		degree = -60;
	}
	if (ball_x > bar_x && ball_x >= (bar_x + 55) && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y)		//�E��
	{
		degree = -60;
	}
	
	//--- �u���b�N�ƃ{�[���̂����蔻�� ---
	for (count = 0; count < 36; ++count)
	{

		if (block[count].flag == 1)	//�u���b�N���������Ă���Ȃ�
		{
			//�u���b�N�̂����蔻��
			if (block[count].x <= ball_x && ball_x < block[count].x + BLOCK_WIDTH && block[count].y <= ball_y && ball_y < block[count].y + BLOCK_HEIGHT)	//�_�Ƌ�`�̔���
			{
				//������
				if (y_direction == -1 && ball_x <= block[count].x + 25)	//�{�[���̌�����������Ȃ�
				{
					y_direction = 1;		//�{�[���̌�����ς���(��)
					x_direction = 1;
					degree = get_random(70);
					block[count].flag = 0;			//�u���b�N�̎��S
					Sound();
				}
				//�E����
				else if (y_direction == -1 && ball_x >= block[count].x + 25 && ball_y >= block[count].y)	//�{�[���̌�����������Ȃ�
				{
					y_direction = 1;		//�{�[���̌�����ς���(��)
					x_direction = -1;
					degree = get_random(100);
					block[count].flag = 0;			//�u���b�N�̎��S
					Sound();
				}
				//���
				else if (y_direction == 1)	//�{�[���̌������������Ȃ�
				{
					y_direction = -1;	//�{�[���̌�����ς���(��)
					block[count].flag = 0;		//�u���b�N�̎��S
					Sound();
				}
				//�u���b�N�̍��E�̔���
				//����
				if (x_direction == 1)	//�{�[���̌������E�����Ȃ�
				{
					x_direction = -1;	//�{�[���̌�����ς���(��)
					block[count].flag = 0;		//�u���b�N�̎��S
					Sound();
				}
				//�E��
				else if (x_direction == -1)	//�{�[���̌������������Ȃ�
				{
					x_direction = 1;		//�{�[���̌�����ς���(�E)
					block[count].flag = 0;			//�u���b�N�̎��S
					Sound();
				}

				ball_x_now = ball_x;		//�����钼�O�̃{�[����x���W���i�[
				ball_y_now = ball_y;		//�����钼�O�̃{�[����y���W���i�[
			}
		}
	}	
	ball_x += (double)speed * cos(radian1) * x_direction;		//�{�[���̈ړ����X�V
	ball_y += (double)speed * -sin(radian1) * y_direction;
	radian1 = M_PI / 180.0f * degree;		//�p�x�̍X�V
}


void my_game_over_check()	//�Q�[���I�[�o�[�ƃN���A��ʂƒ��f
{
	int flag_count = 0;		//�u���b�N�̌����i�[
	if (ball_y > (500 + BALL_SIZE))		//�{�[��������������
	{
		Sound();
		game_over_flag = 1;				//�Q�[���I�[�o�[
		BALL_SIZE - 4;
		ball_x = 200;
		ball_y = 1000;
		game = false;
	}
	for (count = 0; count < 36; ++count)
	{
		flag_count += block[count].flag;	//�u���b�N�̐����i�[
	}
	if (flag_count == 0)		//�u���b�N�̐���0�ɂȂ�����
	{
		game_over_flag = 2;		//�N���A
		if (game_over_flag == 2)
		{
			speed = 0;
		}
	}
	if (key[KEY_INPUT_T] && ball_start_flag != 0 && game_over_flag != 3)	//T�L�[��������
	{
		game_over_flag = 3;	//���f�p�t���O
		if (game_over_flag == 3)
		{
			speed = 0;		//�{�[�����~�߂�
		}
		Sound();			//�L�̐���炷
	}
}

void my_draw_field()	//�o�[�ƃ{�[���ƃQ�[����ʂ̕`��
{

	//�o�[�̕`��(����̓_,�����̓_,�E��̓_,�E���̓_,�F,�h��Ԃ�)
	DrawBox(X_POSI + bar_x, bar_y, X_POSI + bar_x + BAR_WIDIH, bar_y + BAR_HEIGHT, RED, true);
	//�{�[���̕`��(x,y(���S���W),r(���a),�F,�h��Ԃ�)
	DrawCircle(X_POSI + (int)ball_x, (int)ball_y, BALL_SIZE, White, true);
	//�Q�[���t�B�[���h�̕`��(����̓_,�����̓_,�E��̓_,�E���̓_,�F,�h��Ԃ�)
	DrawBox(X_POSI + 0, 0, X_POSI + FIELD_WIDIH, FIELD_HEIGHT, Black, false);
	//�u���b�N�̕`��
	for (count = 0; count < 36; ++count)
	{
		if (block[count].flag == 1)	//�u���b�N�������Ă���Ȃ�
		{
			DrawBox(X_POSI + block[count].x, block[count].y, X_POSI + block[count].x + BLOCK_WIDTH, block[count].y + BLOCK_HEIGHT, block[count].color, true);
		}
	}

}

void my_draw_score_box()	//�����̕\������
{
	if (ball_start_flag == 1)
	{
		DrawFormatString(500, 20, White, "��BREAKOUT��");
		DrawFormatString(500, 40, White, "�� �g�`�q�c ��");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		DrawFormatString(0, 20, skyblue, " T�ňꎞ��~");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 2)
	{
		DrawFormatString(500, 20, White, "��BREAKOUT��");
		DrawFormatString(490, 40, White, "���m�n�q�l�`�k��");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		DrawFormatString(0, 20, skyblue, " T�ňꎞ��~");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 3)
	{
		DrawFormatString(500, 20, White, "��BREAKOUT��");
		DrawFormatString(500, 40, White, "���d�`�r�x ��");
		DrawFormatString(500, 60, White, "��YOUR LOSER��");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		DrawFormatString(0, 20, skyblue, " T�ňꎞ��~");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 4)
	{
		DrawFormatString(500, 20, White, "��BREAKOUT��");
		DrawFormatString(480, 40, White, "���d�w�s�q�d�l�d��");
		DrawFormatString(500, 60, White, "��YOUR CRAZY��");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		DrawFormatString(0, 20, skyblue, " T�ňꎞ��~");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
}

void my_draw_op_end()
{
	if (ball_start_flag == 0 && game_over_flag == 0)		//�X�^�[�g�t���O�ƃG���h�t���O�������Ă��Ȃ��Ƃ�
	{
			r += 3.5f;
			g += 2.6f;
			b += 1.7f;
		if (r == 255 || g == 255 || b == 255)
		{
				r -= 1.0f,
				g -= 1.0f,
				b -= 1.0f;
		}
		DrawFormatString(210, 210, GetColor((int)r, (int)g,(int)b), "����FUCKING BREAKOUT����");
		DrawFormatString(175, 230, RED, "�d�w�s�q�d�l�d  press E to start");
		DrawFormatString(175, 250, Green, "�g�`�q�c  press Enter to start");
		DrawFormatString(200, 270, Blue, "�m�n�q�l�`�k  press Z to start");
		DrawFormatString(200, 290, yellow, "�d�`�r�x  press X to start");
	}
	if (game_over_flag == 1)	//�Q�[���I�[�o�[���̕\��
	{
		DrawFormatString(240, 210, skyblue, " m9(^�D^)�߷ެ�");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		ball_start_flag = 0;
	}
	if (game_over_flag == 2)	//�Q�[���N���A���̕\��
	{
		DrawFormatString(240, 210, yellow, "Congratulation");
		DrawFormatString(0, 0, skyblue, " Esc�ŏI��");
		ball_start_flag = 0;
	}
	if (game_over_flag == 3)	//�ꎞ��~���
	{
		DrawFormatString(280, 210, skyblue, "pause");
		DrawFormatString(230, 230, skyblue, "press R to restart");
		ball_start_flag = 0;
		if (key[KEY_INPUT_R] == 1)		//R�L�[�Ńm�[�}�����[�h�ɕ��A
		{
			ball_start_flag = 2;
			if (ball_start_flag != 0)
			{
				speed = 4;
			}
			game_over_flag = 0;
		}
	}
}


/*------------- �G���g���[�|�C���g ------------*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	ChangeWindowMode(0), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);	//�E�B���h�E���[�h�ύX�Ə������Ɨ���ʐݒ�

	int back = LoadGraph("�摜/space.png");	//�w�i�摜�̃��[�h
	int BGM = LoadSoundMem("�T�E���h/BREAKOUT.ogg");	//BGM�̃��[�h
	PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);				//BGM�̍Đ�
	ChangeVolumeSoundMem(200, BGM);
	my_init_variable();	//����������
	my_init_blocks();	//�u���b�N�̏���������

						// while(����ʂ�\��ʂɔ��f�A���b�Z�[�W�����A��ʃN���A�A�L�[�̍X�V)
	while (ProcessMessage() == 0 && ClearDrawScreen() == 0 && gpUpdatekey() == 0)
	{
		//�����ɃQ�[���̏�����`��------------------------------------------------		
		game_start_check();				//�Q�[�����n�܂��Ă��邩�`�F�b�N
		my_move_ball();					//�{�[���̓���
		my_move_bar();					//�o�[�̓���
		my_cd_check();					//�����蔻��
		my_game_over_check();			//�Q�[���I�[�o�[���̏���
		DrawGraph(0, 0, back, FALSE);	//�w�i�̕`��
		my_draw_field();				//�`�揈��
		my_draw_score_box();			//�����̕\��
		my_draw_op_end();				//�X�^�[�g�ƃG���hz
		if (key[KEY_INPUT_ESCAPE])		//Esc�L�[�ŋ����I���B
		{
			break;
		}
		if (game_over_flag == 1 || game_over_flag == 2)		//�Q�[���I�[�o�[���N���A��BGM���~�߂�
		{
			StopSoundMem(BGM);
		}

		ScreenFlip();	//����ʂ�\��ʂɔ��f

		//----------------------------------------------------------------------
	}
	DxLib_End();							// DX���C�u�����I������
	return 0;
}