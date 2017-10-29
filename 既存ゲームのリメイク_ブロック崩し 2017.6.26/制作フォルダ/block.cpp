/*------------- http://akaguro.jp/breakout/breakout_index.shtml を参考、少し書き方を変えた。----
サンプルのままだとボールの速さを変えるとすり抜けが発生するので、ボールのすり抜けを改善(多分。。。)した。
-------------- サンプルにコメントが一切ないのでほぼすべての処理にコメントを付けた。 ----------------
-------------- 難易度を付けた。 ----------------------------------------------------------------
*/
#define _USE_MATH_DEFINES
#include "DxLib.h"
#include <math.h>

#define BALL_SIZE 4			//ボールの大きさ
#define BAR_WIDIH 75		//バーの横幅
#define BAR_HEIGHT 10		//バーの高さ
#define FIELD_WIDIH 300		//ゲーム画面の横幅（右端）
#define FIELD_HEIGHT 480	//ゲーム画面の高さ（下端）
#define X_POSI 170			//フィールドの表示位置を調整(ゲームをする場所(フィールド)のx座標)
#define BLOCK_WIDTH 50		//ブロックの横幅
#define BLOCK_HEIGHT 10		//ブロックの高さ


int bar_x;					//バーの位置x
int bar_y;					//バーの位置y
double ball_x;				//ボールの位置x
double ball_y;				//ボールの位置y
double ball_x_now;			//ブロックに当たる前の位置を格納
double ball_y_now;			//ブロックに当たる前の位置を格納
float degree;
double radian1 = M_PI / 180.0f * degree;	//ボールの反射角度
double speed;				//ボールの速さ
int bar_speed;				//バーの移動速度
int ball_start_flag;		//ボールの開始フラグ
int x_direction = 0;		//ボールの向き1で+方向,-1で-方向
int y_direction = 0;		//ボールの向き1で+方向,-1で-方向
int game_over_flag;			//ゲームオーバーフラグ
int flag_count = 0;			//ゲームオーバーのフラグ1で終了
bool game;					//ゲームが始まっているかのフラグ

int key[256];
int White = GetColor(255, 255, 255);	//色のデータ白
int Black = GetColor(0, 0, 0);			//色のデータ黒
int RED = GetColor(255, 0, 0);			//色のデータ赤
int Green = GetColor(0, 255, 0);		//色のデータ緑
int Blue = GetColor(0, 0, 255);			//色のデータ青
int yellow = GetColor(255, 255, 0);		//色のデータ黄色
int skyblue = GetColor(0, 255, 255);	//色のデータ空色
float	r = 0,							//色のデータ虹色用
		g = 0,
		b = 0;


int i,				//ループ用変数
	j;				//ループ用変数
int count = 0;		//ブロックの表示用変数


					//ブロックの配置
int block_map[6][6] = {
	{ 1,2,3,4,5,4 },
	{ 5,4,3,2,1,5 },
	{ 3,4,2,3,4,2 },
	{ 1,2,3,1,2,3 },
	{ 2,4,1,2,4,1 },
	{ 4,1,2,3,1,4 }
};

//ブロックのデータ
struct BLOCKS
{
	int x;		//ブロックのx座標
	int y;		//ブロックのy座標
	int flag;	//ブロックの生存フラグ
	int color;	//ブロックの色
};
BLOCKS block[36];
//-----------------

int gpUpdatekey()
{
	char tmpkey[256];				//現在のキーの入力状態を格納
	GetHitKeyStateAll(tmpkey);		//すべてのキーの入力状態を得る
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

void my_init_variable()	//初期化処理
{
	bar_x = 110;							//バーのx座標
	bar_y = FIELD_HEIGHT - BAR_HEIGHT;		//バーのy座標
	bar_speed = 8;					//バーの速さ
	ball_x = 145;					//ボールの初期位置
	ball_y = (FIELD_HEIGHT - BAR_HEIGHT) - BALL_SIZE;
	speed = 4.0;					//ボールの速さ
	ball_start_flag = 0;			//ボールの状態
	degree = -45;
	radian1 = M_PI / 180.0f * degree;	//ボールの反射角度
	x_direction = 0;				//ボールの向き1で+方向
	y_direction = 0;				//ボールの向き1で+方向
	game_over_flag = 0;				//1でゲームオーバー
	game = false;					//falseはゲームが始まっていない
}
void my_init_blocks()	//ブロックの初期化
{
	for (i = 0; i < 6; ++i)		//２次配列の中身を回す
	{
		for (j = 0; j < 6; ++j)		//2次配列の外側を回す
		{
			if (block_map[i][j] != 0)	//ブロックが生きているとき
			{
				block[count].x = (BLOCK_WIDTH * j);		//ブロックのx座標(横に並べる)
				block[count].y = ((BLOCK_HEIGHT + 10)* i);	//ブロックのy座標(縦に並べる)
				block[count].flag = 1;		//すべてのブロックにフラグを用意
			}
			switch (block_map[i][j])	//ブロックの色をcountで変更
			{
			case 1:
				block[count].color = RED;		//配列番号1で赤
				break;
			case 2:
				block[count].color = Green;		//配列番号2で緑
				break;
			case 3:
				block[count].color = Blue;		//配列番号3で青
				break;
			case 4:
				block[count].color = skyblue;   //配列番号4で水色	
				break;
			case 5:
				block[count].color = White;   //配列番号5で水色	
				break;
			default:
				break;
			}
			++count;		//ループに入れることで番号を増やす
		}
	}
}
void Sound()		//SEなどの処理
{
	//仮の音源
	int SE1 = LoadSoundMem("サウンド/cat1.wav");		//SEのロード
	int SE2 = LoadSoundMem("サウンド/cat2.wav");
	int SE3 = LoadSoundMem("サウンド/kan.ogg");
	if (key[KEY_INPUT_T] == 1)	//Tキーが押された瞬間
	{
		PlaySoundMem(SE1, DX_PLAYTYPE_BACK);	//猫の声
	}
	if (ball_y > (500 + BALL_SIZE) && game_over_flag == 0)	//ボールが落下した週間のみ
	{
		PlaySoundMem(SE2, DX_PLAYTYPE_BACK);	//猫の声
	}
	if (block[count].flag == 0 || ball_x >= (FIELD_WIDIH - 7.f) || ball_x <= 7.0f || ball_x > bar_x && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y || ball_y <= 6.0f)	//何かにぶつかったら
	{
		PlaySoundMem(SE3, DX_PLAYTYPE_BACK);	//SE
		ChangeVolumeSoundMem(255, SE3);			//SE3の音量
		if (game_over_flag == 1 && game_over_flag == 3)
		{
			StopSoundMem(SE3);

		}
	}
	
}

void game_start_check()
{
	if (game == false)	//ゲームが始まっていないとき
	{
		if (key[KEY_INPUT_RETURN] == 1 || key[KEY_INPUT_Z] == 1 || key[KEY_INPUT_X] == 1 || key[KEY_INPUT_E] == 1)
		{
			game = true;
			x_direction = 1;
			y_direction = -1;
		}
	}

}

void my_move_ball()	//ボールの動き
{
	
	/*ＨＡＲＤモード、エンターキー*/
	if (ball_start_flag == 1)		//ボールのフラグが立っていたら動く
	{
		if (x_direction == 1)			//ボールが右端でないなら
		{
			ball_x += speed;		//右に移動
		}
		else
		{
			ball_x -= speed;		//左に移動
		}
		if (y_direction == 1)			//ボールが上端でないなら
		{
			ball_y += speed;		//下に移動
		}
		else
		{
			ball_y -= speed;		//上に移動
		}
	}
	/*ＮＯＲＭＡＬモード、Zキー*/
	if (ball_start_flag == 2)		//ボールのフラグが立っていたら動く
	{

		if (x_direction == 1)			//ボールが左端なら
		{
			ball_x += speed / 1.4f;		//右に移動
		}
		else
		{
			ball_x -= speed / 1.4f;		//左に移動
		}
		if (y_direction == 1)			//ボールが上端なら
		{
			ball_y += speed / 1.4f;		//下に移動
		}
		else
		{
			ball_y -= speed / 1.4f;		//上に移動
		}
	}
	/*ＥＡＳＹモード、Xキー*/
	if (ball_start_flag == 3)		//ボールのフラグが立っていたら動く
	{
		if (x_direction == 1)			//ボールが右端でないなら
		{
			ball_x += speed / 2.5f;		//右に移動
		}
		else
		{
			ball_x -= speed / 2.5f;		//左に移動
		}
		if (y_direction == 1)			//ボールが上端でないなら
		{
			ball_y += speed / 2.5f;		//下に移動
		}
		else
		{
			ball_y -= speed / 2.5f;		//上に移動
		}
	}
	/*ＥＸＴＲＥＭＥモード、Eキー*/
	if (ball_start_flag == 4)		//ボールのフラグが立っていたら動く
	{
		if (x_direction == 1)			//ボールが右端でないなら
		{
			ball_x += speed + 2;		//右に移動
		}
		else
		{
			ball_x -= speed + 2;		//左に移動
		}
		if (y_direction == 1)			//ボールが上端でないなら
		{
			ball_y += speed + 2;		//下に移動
		}
		else
		{
			ball_y -= speed + 2;		//上に移動
		}
	}
	
}

void my_move_bar()	//バーの移動処理等
{
	
	if (key[KEY_INPUT_RETURN] == 1)	//エンターキーが押されたら
	{
		ball_start_flag = 1;		//ボールを動かす
	}
	if (key[KEY_INPUT_Z] == 1)		//Zキーが押されたら
	{
		ball_start_flag = 2;		//ボールを動かす
	}
	if (key[KEY_INPUT_X] == 1)		//Xキーが押されたら
	{
		ball_start_flag = 3;		//ボールを動かす
	}
	if (key[KEY_INPUT_E] == 1)		//Eキーが押されたら
	{
		ball_start_flag = 4;		//ボールを動かす
	}
	if (ball_start_flag == 1 || ball_start_flag == 2 || ball_start_flag == 3)		//ボールのフラグが立っていたら動く
	{
		if (key[KEY_INPUT_RIGHT] > 0 && (bar_x + BAR_WIDIH) < FIELD_WIDIH - 3)	//右キーが押されているかつ右端でないなら(ずれた分を引く)
		{
			bar_x += bar_speed;			//バーを右に移動
		}
		if (key[KEY_INPUT_LEFT] > 0 && bar_x > 0)	//左キーが押されているかつ左端の時(0は左端)
		{
			bar_x -= bar_speed;			//バーを左に移動
		}
	}

	if (ball_start_flag == 4)		//ＥＸＴＲＥＭＥモード時
	{
		if (key[KEY_INPUT_RIGHT] > 0 && (bar_x + BAR_WIDIH) < FIELD_WIDIH)	//右キーが押されているかつ右端でないなら
		{
			bar_x += bar_speed + 4;				//バーを右に移動			
		}
		if (key[KEY_INPUT_LEFT] > 0 && bar_x > 0)	//左キーが押されているかつ左端の時(0は左端)
		{
			bar_x -= bar_speed + 4;				//バーを左に移動
		}
	}
}
float get_random(int num)
{
	return float(rand() % (num) * -1);
}

void my_cd_check()	//ボールとバーとブロックの当たり判定とボールの反射
{
	//--- ボールの反射処理 ---(ぴったりだとすり抜けるので、少しずらす)
	if (ball_x >= (FIELD_WIDIH - 7.f))		//ボールが右端に来たら
	{
		x_direction = -1;			//向きを変える（左）
		Sound();
		if (ball_y  >= 0 && ball_y <= 180)		//画面の上のほうで
		{
			degree = -45;
		}
		if (ball_y >= 181 && ball_y <= 280)		//画面の縦半分位で
		{
			degree = get_random(50);
		}
		if (ball_y >= 281 && ball_y <= 480)	//画面の下半分
		{
			degree = -70;
		}
		
	}
	if (ball_x <= 7.0f && x_direction == -1)				//ボールが左端に来たら
	{
		x_direction = 1;			//向きを変える（右）
		Sound();
		if (ball_y >= 0 && ball_y <= 180)		
		{
			degree = -45;
		}
		if (ball_y >= 181 && ball_y <= 280)		//画面の半分位で
		{
			degree = get_random(60);
		}
		if (ball_y >= 281 && ball_y <= 480)	//画面の下半分
		{
			degree = -80;
		}
	}
	if (ball_y <= 6.0f && y_direction == -1)				//ボールが上端に来たら
	{
		y_direction = 1;			//向きを変える（下）
		Sound();
	}
	
	//--- ボールとバーの判定 ---
	if (ball_x > bar_x && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y)	//ボールのxがバーの長さの時
	{
		y_direction = -1;
		Sound();
		degree = -80;		
	}
	if (ball_x > bar_x && ball_x <= (bar_x + 20) && ball_y >= bar_y)			//バーの左側に当たった時
	{
		degree = -60;
	}
	if (ball_x > bar_x && ball_x >= (bar_x + 55) && ball_x <= (bar_x + BAR_WIDIH) && ball_y >= bar_y)		//右側
	{
		degree = -60;
	}
	
	//--- ブロックとボールのあたり判定 ---
	for (count = 0; count < 36; ++count)
	{

		if (block[count].flag == 1)	//ブロックが生存しているなら
		{
			//ブロックのあたり判定
			if (block[count].x <= ball_x && ball_x < block[count].x + BLOCK_WIDTH && block[count].y <= ball_y && ball_y < block[count].y + BLOCK_HEIGHT)	//点と矩形の判定
			{
				//左下面
				if (y_direction == -1 && ball_x <= block[count].x + 25)	//ボールの向きが上方向なら
				{
					y_direction = 1;		//ボールの向きを変える(下)
					x_direction = 1;
					degree = get_random(70);
					block[count].flag = 0;			//ブロックの死亡
					Sound();
				}
				//右下面
				else if (y_direction == -1 && ball_x >= block[count].x + 25 && ball_y >= block[count].y)	//ボールの向きが上方向なら
				{
					y_direction = 1;		//ボールの向きを変える(下)
					x_direction = -1;
					degree = get_random(100);
					block[count].flag = 0;			//ブロックの死亡
					Sound();
				}
				//上面
				else if (y_direction == 1)	//ボールの向きが下方向なら
				{
					y_direction = -1;	//ボールの向きを変える(上)
					block[count].flag = 0;		//ブロックの死亡
					Sound();
				}
				//ブロックの左右の判定
				//左面
				if (x_direction == 1)	//ボールの向きが右方向なら
				{
					x_direction = -1;	//ボールの向きを変える(左)
					block[count].flag = 0;		//ブロックの死亡
					Sound();
				}
				//右面
				else if (x_direction == -1)	//ボールの向きが左方向なら
				{
					x_direction = 1;		//ボールの向きを変える(右)
					block[count].flag = 0;			//ブロックの死亡
					Sound();
				}

				ball_x_now = ball_x;		//あたる直前のボールのx座標を格納
				ball_y_now = ball_y;		//あたる直前のボールのy座標を格納
			}
		}
	}	
	ball_x += (double)speed * cos(radian1) * x_direction;		//ボールの移動を更新
	ball_y += (double)speed * -sin(radian1) * y_direction;
	radian1 = M_PI / 180.0f * degree;		//角度の更新
}


void my_game_over_check()	//ゲームオーバーとクリア画面と中断
{
	int flag_count = 0;		//ブロックの個数を格納
	if (ball_y > (500 + BALL_SIZE))		//ボールが落下したら
	{
		Sound();
		game_over_flag = 1;				//ゲームオーバー
		BALL_SIZE - 4;
		ball_x = 200;
		ball_y = 1000;
		game = false;
	}
	for (count = 0; count < 36; ++count)
	{
		flag_count += block[count].flag;	//ブロックの数を格納
	}
	if (flag_count == 0)		//ブロックの数が0になったら
	{
		game_over_flag = 2;		//クリア
		if (game_over_flag == 2)
		{
			speed = 0;
		}
	}
	if (key[KEY_INPUT_T] && ball_start_flag != 0 && game_over_flag != 3)	//Tキーをしたら
	{
		game_over_flag = 3;	//中断用フラグ
		if (game_over_flag == 3)
		{
			speed = 0;		//ボールを止める
		}
		Sound();			//猫の声を鳴らす
	}
}

void my_draw_field()	//バーとボールとゲーム画面の描画
{

	//バーの描画(左上の点,左下の点,右上の点,右下の点,色,塗りつぶし)
	DrawBox(X_POSI + bar_x, bar_y, X_POSI + bar_x + BAR_WIDIH, bar_y + BAR_HEIGHT, RED, true);
	//ボールの描画(x,y(中心座標),r(半径),色,塗りつぶし)
	DrawCircle(X_POSI + (int)ball_x, (int)ball_y, BALL_SIZE, White, true);
	//ゲームフィールドの描画(左上の点,左下の点,右上の点,右下の点,色,塗りつぶし)
	DrawBox(X_POSI + 0, 0, X_POSI + FIELD_WIDIH, FIELD_HEIGHT, Black, false);
	//ブロックの描画
	for (count = 0; count < 36; ++count)
	{
		if (block[count].flag == 1)	//ブロックが生きているなら
		{
			DrawBox(X_POSI + block[count].x, block[count].y, X_POSI + block[count].x + BLOCK_WIDTH, block[count].y + BLOCK_HEIGHT, block[count].color, true);
		}
	}

}

void my_draw_score_box()	//文字の表示処理
{
	if (ball_start_flag == 1)
	{
		DrawFormatString(500, 20, White, "◆BREAKOUT◆");
		DrawFormatString(500, 40, White, "◆ ＨＡＲＤ ◆");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		DrawFormatString(0, 20, skyblue, " Tで一時停止");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 2)
	{
		DrawFormatString(500, 20, White, "◆BREAKOUT◆");
		DrawFormatString(490, 40, White, "◆ＮＯＲＭＡＬ◆");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		DrawFormatString(0, 20, skyblue, " Tで一時停止");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 3)
	{
		DrawFormatString(500, 20, White, "◆BREAKOUT◆");
		DrawFormatString(500, 40, White, "◆ＥＡＳＹ ◆");
		DrawFormatString(500, 60, White, "◆YOUR LOSER◆");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		DrawFormatString(0, 20, skyblue, " Tで一時停止");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
	if (ball_start_flag == 4)
	{
		DrawFormatString(500, 20, White, "◆BREAKOUT◆");
		DrawFormatString(480, 40, White, "◆ＥＸＴＲＥＭＥ◆");
		DrawFormatString(500, 60, White, "◆YOUR CRAZY◆");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		DrawFormatString(0, 20, skyblue, " Tで一時停止");
		DrawFormatString(0, 40, skyblue, "X %.1f", ball_x);
		DrawFormatString(0, 60, skyblue, "Y %.1f", ball_y);
		DrawFormatString(0, 80, skyblue, "D %.1f", degree * -1);
	}
}

void my_draw_op_end()
{
	if (ball_start_flag == 0 && game_over_flag == 0)		//スタートフラグとエンドフラグが立っていないとき
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
		DrawFormatString(210, 210, GetColor((int)r, (int)g,(int)b), "■◆FUCKING BREAKOUT◆■");
		DrawFormatString(175, 230, RED, "ＥＸＴＲＥＭＥ  press E to start");
		DrawFormatString(175, 250, Green, "ＨＡＲＤ  press Enter to start");
		DrawFormatString(200, 270, Blue, "ＮＯＲＭＡＬ  press Z to start");
		DrawFormatString(200, 290, yellow, "ＥＡＳＹ  press X to start");
	}
	if (game_over_flag == 1)	//ゲームオーバー時の表示
	{
		DrawFormatString(240, 210, skyblue, " m9(^Д^)ﾌﾟｷﾞｬｰ");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		ball_start_flag = 0;
	}
	if (game_over_flag == 2)	//ゲームクリア時の表示
	{
		DrawFormatString(240, 210, yellow, "Congratulation");
		DrawFormatString(0, 0, skyblue, " Escで終了");
		ball_start_flag = 0;
	}
	if (game_over_flag == 3)	//一時停止画面
	{
		DrawFormatString(280, 210, skyblue, "pause");
		DrawFormatString(230, 230, skyblue, "press R to restart");
		ball_start_flag = 0;
		if (key[KEY_INPUT_R] == 1)		//Rキーでノーマルモードに復帰
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


/*------------- エントリーポイント ------------*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	ChangeWindowMode(0), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);	//ウィンドウモード変更と初期化と裏画面設定

	int back = LoadGraph("画像/space.png");	//背景画像のロード
	int BGM = LoadSoundMem("サウンド/BREAKOUT.ogg");	//BGMのロード
	PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);				//BGMの再生
	ChangeVolumeSoundMem(200, BGM);
	my_init_variable();	//初期化処理
	my_init_blocks();	//ブロックの初期化処理

						// while(裏画面を表画面に反映、メッセージ処理、画面クリア、キーの更新)
	while (ProcessMessage() == 0 && ClearDrawScreen() == 0 && gpUpdatekey() == 0)
	{
		//ここにゲームの処理を描く------------------------------------------------		
		game_start_check();				//ゲームが始まっているかチェック
		my_move_ball();					//ボールの動き
		my_move_bar();					//バーの動き
		my_cd_check();					//あたり判定
		my_game_over_check();			//ゲームオーバー時の処理
		DrawGraph(0, 0, back, FALSE);	//背景の描画
		my_draw_field();				//描画処理
		my_draw_score_box();			//文字の表示
		my_draw_op_end();				//スタートとエンドz
		if (key[KEY_INPUT_ESCAPE])		//Escキーで強制終了。
		{
			break;
		}
		if (game_over_flag == 1 || game_over_flag == 2)		//ゲームオーバーかクリアでBGMを止める
		{
			StopSoundMem(BGM);
		}

		ScreenFlip();	//裏画面を表画面に反映

		//----------------------------------------------------------------------
	}
	DxLib_End();							// DXライブラリ終了処理
	return 0;
}