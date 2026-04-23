
#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>


//関数とか

//1～6確認用
int is_symbol1(char* sym) {
	char* symbol[] = { "1 ","2 ","3 ","4 ","5 ","6 " };
	for (int i = 0; i < sizeof(symbol)/sizeof(symbol[0]); i++) {
		if (strncmp(sym, symbol[i],2) == 0) {
			return 1;
		}
	}
	return 0;
}
//8～9確認用
int is_symbol2(char* sym) {
	char* symbol[] = { "8 ","9 "};
	for (int i = 0; i < sizeof(symbol) / sizeof(symbol[0]); i++) {
		if (strncmp(sym, symbol[i],2) == 0) {
			return 1;
		}
	}
	return 0;
}
//7確認用
int is_symbol3(char* sym) {
	if (strncmp(sym, "7 ",2) == 0) {
		return 1;
	}
	return 0;
}
//☆確認用 いったん仕様無視
int is_symbol4(char* sym) {
		if (strncmp(sym, "☆",3)==0) {
			return 1;
		}
		return 0;
	}
//?確認用
int is_symbol5(char* sym) {
	if (strncmp(sym, "?",2) == 0) {
		return 1;
	}
	return 0;
}


//ANSIエスケープシーケンスを有効にする関数
void enable_ansi_escape() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;
	DWORD dwMode = 0;
	if(!GetConsoleMode(hOut, &dwMode))return;
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

//画面クリアする関数
void clear_screen() {
	printf("\033[2J");
}

//カーソル位置移動する関数
void gotoxy(int x, int y) {
	printf("\033[%d;%dH", y, x);
}

//☆を含め揃ったか確認する関数
int check_line(char* sym1, char* sym2, char* sym3, char** win_sym) {

	// すべてがワイルドカードの場合
	if (strncmp(sym1, "☆", 3) == 0 && strncmp(sym2, "☆", 3) == 0 && strncmp(sym3, "☆", 3) == 0) {
		*win_sym = sym1;
		return 1;
	}

	// 2つがワイルドカードの場合
	if (strncmp(sym1, "☆", 3) == 0 && strncmp(sym2, "☆", 3) == 0 && strncmp(sym3, "☆", 3) != 0) {
		*win_sym = sym3;
		return 1;
	}
	if (strncmp(sym1, "☆", 3) == 0 && strncmp(sym3, "☆", 3) == 0 && strncmp(sym2, "☆", 3) != 0) {
		*win_sym = sym2;
		return 1;
	}
	if (strncmp(sym2, "☆", 3) == 0 && strncmp(sym3, "☆", 3) == 0 && strncmp(sym1, "☆", 3) != 0) {
		*win_sym = sym1;
		return 1;
	}

	// 1つがワイルドカードの場合
	if (strncmp(sym1, "☆", 3) == 0 && strncmp(sym2, sym3, 2) == 0) {
		*win_sym = sym2;
		return 1;
	}
	if (strncmp(sym2, "☆", 3) == 0 && strncmp(sym1, sym3, 2) == 0) {
		*win_sym = sym1;
		return 1;
	}
	if (strncmp(sym3, "☆", 3) == 0 && strncmp(sym1, sym2, 2) == 0) {
		*win_sym = sym1;
		return 1;
	}

	// ワイルドカードがない場合
	if (strncmp(sym1, sym2, 2) == 0 && strncmp(sym2, sym3, 2) == 0) {
		*win_sym = sym1;
		return 1;
	}

	return 0;
}



int main(void) {
	//ANSIエスケープシーケンスを有効にする
	enable_ansi_escape();
	srand((unsigned int)time(NULL));

	//変数とか配列とか
	int data_cnt = 0;//前回のデータを表示する時用
	int game_luck = 0;//未実装luck値によって使用する配列を変える用
	int total_line;
	int st_win_money;

	//配列
	//gotoを使用した際に半角と全角になってしまうため、半角数字には後ろにスペースを追加
	char* symbols_1[] = { "1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","☆","?"};
	char* symbols_2[] = { "1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","☆","?",
		"1 ","2 ","3 ","1 ","2 ","3 ","☆"};
	char* symbols_3[] = { "1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","☆","?",
		"7 ","7 ","7 ","1 ","2 ","3 ","☆"};

	//スロット画面用の配列
	char* reels[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			reels[i][j] = " ";
		}
	}
	
	//所持金付与
	int player_money = 10000;
	int player_sentaku;


	while (1) {

		clear_screen();
		printf("\033[0m");
		gotoxy(1, 1);
		printf("-- スロットゲーム --\n");//1行目

		//初期画面　表示箇所固定
		gotoxy(1, 2);
		printf("現在の所持金は%d円です\n", player_money);//2行目
		gotoxy(1, 3);
		printf("選択してください　(S)回す　(F)終わる\n");//3行目
		gotoxy(1, 4);
		printf("\n");//4行目

		//固定された枠の表示
		//		　　　　　　　 1234567890123456789012345
		gotoxy(1, 5); printf("┌ーーーーーーーーーーー┐\n");
		gotoxy(1, 6); printf("｜　　　｜　　　｜　　　｜\n");//6行目
		gotoxy(1, 7); printf("｜ー　ー　ー　ー　ー　ー｜\n");
		gotoxy(1, 8); printf("｜　　　｜　　　｜　　　｜\n");//8行目
		gotoxy(1, 9); printf("｜ー　ー　ー　ー　ー　ー｜\n");
		gotoxy(1, 10); printf("｜　　　｜　　　｜　　　｜\n");//10行目
		gotoxy(1, 11); printf("└ーーーーーーーーーーー┘\n");

		//前回の結果を表示する
		if (data_cnt > 0) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					gotoxy(5 + k * 8, 6 + j * 2);
					printf("%s", reels[j][k]);
				}
			}
			gotoxy(1, 12);
			if (total_line > 0) {
				if (st_win_money == 0) {
					printf("運気が上がった気がする！\n");
				}
				else {
					printf("%d列揃いました!おめでとう！%d円獲得しました\n", total_line, st_win_money);
				}
			}
		}
		//ここまで

			gotoxy(1, 12);//枠外のカーソル位置に戻す


			do {
				//_getch()でキーを取得する
				player_sentaku = _getch();
				// 取得したキーの確認、違えばループ
			} while (player_sentaku != 's' && player_sentaku != 'S' && player_sentaku != 'f' && player_sentaku != 'F');
			//初期画面のキー入力ここまで

			//キー入力による条件分岐
			switch (player_sentaku) {
			case 's':
			case'S':
				player_money -= 100; //100円減らす
				data_cnt = (data_cnt + 1) % 10; //試行回数

				int is_stop[3] = { 0,0,0 };//各リールの停止状態(0:回転 1:停止)
				int all_stop = 0; //ストップ用のフラグ

				gotoxy(1, 13);
				printf("(1)(2)(3)を押してください");
				printf("\033[?25l");//カーソルの非表示
				fflush(stdout);

				//リールのアニメーション用
				while (!all_stop) {
					for (int j = 0; j < 3; j++) {
						for (int k = 0; k < 3; k++) {
							//game_luck条件をつける
							if (!is_stop[k]) {
								if (game_luck < 3) {
									reels[j][k] = symbols_1[rand() % sizeof(symbols_1) / sizeof(symbols_1[0])];
								}
								else if (game_luck < 6) {
									reels[j][k] = symbols_2[rand() % sizeof(symbols_2) / sizeof(symbols_2[0])];
								}
								else if (6 <= game_luck) {
									reels[j][k] = symbols_3[rand() % sizeof(symbols_3) / sizeof(symbols_3[0])];
								}
							}

							gotoxy(5 + k * 8, 6 + j * 2);
							printf("%s", reels[j][k]);

						}
						fflush(stdout);//即時反映
						Sleep(50);//ミリ秒待機
					}



					if (_kbhit()) {
						int key = _getch();
						if (key == '1') {
							is_stop[0] = 1;
						}
						else if (key == '2') {
							is_stop[1] = 1;
						}
						else if (key == '3') {
							is_stop[2] = 1;
						}
					}
					if (is_stop[0] && is_stop[1] && is_stop[2]) {
						all_stop = 1;
					}
				}//リールアニメーションwhileの最後

				//リール部分のクリア （半角と全角があるため、バッファが残っている場合に違う数字/記号になってしまう）
				for (int j = 0; j < 3; j++) {
					for (int k = 0; k < 3; k++) {
						gotoxy(5 + k * 8, 6 + j * 2);
						printf("   ");
					}
				}
				for (int j = 0; j < 3; j++) {
					for (int k = 0; k < 3; k++) {
						gotoxy(5 + k * 8, 6 + j * 2);
						printf("%s", reels[j][k]);
					}
				}



				int line1 = 0, line2 = 0, line3 = 0, line4 = 0, line5 = 0;//各条件での揃い数
				total_line = 0;//揃ったライン数の合計
				int win_money = 0;
				int win_line[9] = { 0 };//揃ったラインを記録する配列 0～2：横 3～5 左上から右下斜め：6 右上から左下斜め：7

				//横で揃っているか確認
				for (int i = 0; i < 3; i++) {
					char* sym1 = reels[i][0];
					char* sym2 = reels[i][1];
					char* sym3 = reels[i][2];

					//☆も含めて揃っているか確認


						//揃った種類を判定 後で時間があれば関数化する
						char* win_sym = NULL;//戻り値として利用するためchar*変数を利用

						if(check_line(sym1,sym2,sym3,&win_sym)){


							if (is_symbol1(win_sym)) line1++;
							else if (is_symbol2(win_sym)) line2++;
							else if (is_symbol3(win_sym)) line3++;
							else if (strncmp(win_sym, "☆", 3) == 0) line4++;
						else if (is_symbol5(win_sym)) line5++;
						win_line[i] = 1;
					}
				}
				//縦で揃っているか確認
				for (int i = 0; i < 3; i++) {
					char* sym1 = reels[0][i];
					char* sym2 = reels[1][i];
					char* sym3 = reels[2][i];

					/*if (is_match(sym1, sym2) && is_match(sym2, sym3)) {*/

						//揃った種類を判定
						char* win_sym = NULL;

						if (check_line(sym1, sym2, sym3, &win_sym)) {


						if (is_symbol1(win_sym)) line1++;
						else if (is_symbol2(win_sym)) line2++;
						else if (is_symbol3(win_sym)) line3++;
						else if (strncmp(win_sym, "☆", 3) == 0) line4++;
						else if (is_symbol5(win_sym)) line5++;
						win_line[3 + i] = 1;
					}
				}
				//左上から右下斜め
				{
					char* sym1 = reels[0][0];
					char* sym2 = reels[1][1];
					char* sym3 = reels[2][2];

					/*if (is_match(sym1, sym2) && is_match(sym2, sym3)) {*/

						//揃った種類を判定
						char* win_sym = NULL;

						if (check_line(sym1, sym2, sym3, &win_sym)) {

						

						if (is_symbol1(win_sym)) line1++;
						else if (is_symbol2(win_sym))line2++;
						else if (is_symbol3(win_sym))line3++;
						else if (strncmp(win_sym, "☆", 3) == 0) line4++;
						else if (is_symbol5(win_sym))line5++;
						win_line[6] = 1;
					}
				}
				
				//右上から左下斜め
				{
					char* sym1 = reels[0][2];
					char* sym2 = reels[1][1];
					char* sym3 = reels[2][0];


					/*if (is_match(sym1, sym2) && is_match(sym2, sym3)) {*/

						//揃った種類を判定
						char* win_sym = NULL;
						if (check_line(sym1, sym2, sym3, &win_sym)) {

						if (is_symbol1(win_sym)) line1++;
						else if (is_symbol2(win_sym))line2++;
						else if (is_symbol3(win_sym))line3++;
						else if (strncmp(win_sym, "☆", 3) == 0) line4++;
						else if (is_symbol5(win_sym))line5++;
						win_line[7] = 1;
					}
				}
				total_line = line1 + line2 + line3 + line4 + line5;
				//揃った場合と外れた場合の条件式
				if (total_line > 0) {
					//揃ったときの演出

					//リール部分のクリアと表示（半角と全角があるため、バッファが残っている場合に違う数字/記号になってしまう）
					for (int j = 0; j < 3; j++) {
						for (int k = 0; k < 3; k++) {
							gotoxy(5 + k * 8, 6 + j * 2);
							printf("   ");
							gotoxy(5 + k * 8, 6 + j * 2);
							printf("%s",reels[j][k]);
						}
					}
					
					for (int c = 0; c < 3; c++) {
						//反転表示
						printf("\033[7m");
						//	//横が揃っていれば描写
						for (int i = 0; i < 3; i++) {
							if (win_line[i]) {
								for (int j = 0; j < 3; j++) {
									gotoxy(5 + j * 8, 6 + i * 2);
									printf("%s", reels[i][j]);
								}
							}
						}
						//	//縦が揃っていれば描写
						for (int i = 0; i < 3; i++) {
							if (win_line[3 + i]) {
								for (int j = 0; j < 3; j++) {
									gotoxy(5 + i * 8, 6 + j * 2);
									printf("%s", reels[j][i]);
								}
							}
						}
						//	//左上から右下斜めが揃っていれば描写
						if (win_line[6]) {
							for (int i = 0; i < 3; i++) {
								gotoxy(5 + i * 8, 6 + i * 2);
								printf("%s", reels[i][i]);
							}
						}
						//右上から左下斜めが揃っていれば描写
						if (win_line[7]) {
							for (int i = 0; i < 3; i++) {
								gotoxy(5 + (2 - i) * 8, 6 + i * 2);
								printf("%s", reels[i][2 - i]);
							}
						}
						fflush(stdout);
						Sleep(250);

						//色と点滅をリセット
						printf("\033[0m");
						fflush(stdout);
						Sleep(250);

						//通常描写に戻す
						for (int j = 0; j < 3; j++) {
							for (int k = 0; k < 3; k++) {
								gotoxy(5 + k * 8, 6 + j * 2);
								printf("   ");
								gotoxy(5 + k * 8, 6 + j * 2);
								printf("%s", reels[j][k]);
							}
						}
						
						fflush(stdout);
						Sleep(250);
					}

					//所持金や効果の反映
					if (line1 > 0) {
						win_money += 1000 * line1;
					}
					if (line2 > 0) {
						win_money += 3000 * line2;
					}
					if (line3 > 0) {
						win_money += 10000 * line3;
					}
					if (line4 > 0) {
						game_luck += 3;
					}
					if (line5 > 0) {
						game_luck += 3;
					}
					printf("\033[?25h");//カーソルを再表示
					gotoxy(1, 13);//枠外のカーソル位置

					//printf("%d列揃いました!おめでとう！\n", total_line);
					//printf("%d円獲得しました。\n", win_money);
					player_money += win_money;
					st_win_money = win_money;
					game_luck = 0;//当たればゲームラックリセット

					gotoxy(1, 12);
					if (total_line > 0) {
						if (win_money == 0) {
							printf("運気が上がった気がする！\n");
						}
						else {
							printf("%d列揃いました!おめでとう！%d円獲得しました\n", total_line, st_win_money);
						}
					}
				}
				else {
					//printf("はずれ、、、");
					game_luck++; //外れたらゲームラックが++
				}
				////揃った場合と外れた場合の条件式 ここまで

				break;//case S sのブレイク

			case'F':
			case'f':
				gotoxy(1, 13);
				printf("終わり！");
				return 0;
			}
		}
		return 0;
	}