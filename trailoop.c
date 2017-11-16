#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define B_COUNT 20 //ブロック数
#define P_B_COUNT 26 //進捗バー数
#define B_WIDTH 0.25 //B_COUNTと一緒に変更する
#define REFRESH_RATE 20 //msec
#define DEFAULT_SPEED 30 //ブロックスピード
#define DEFAULT_CREATE_BLOCKS 6 //生成ブロック数

typedef struct {   
    double x;
    double y;
    double color[3];
    double dx;
    double dy;
    int active;
} block;  //ブロック系部品の構造体

typedef struct{
  int x;
  int y;
} player;


//グローバル関数
int bar_speed = DEFAULT_SPEED;
int refresh_count = 0;
int flag = 0;
block blocks[B_COUNT][B_COUNT];
block progress_bar[P_B_COUNT];
player player1;

/* 座標変換関数
mode
  0:ブロック座標
  1:px座標(X)=>ブロック座標
  2:px座標(Y)=>ブロック座標
  3:px座標=>gl2D座標 */
double convertV(int x, int block_count, double zTH, int mode)
{
  int bcTH, pxTH, pxMG;
  double a;

  bcTH = block_count/2; // 20/2=10
  pxTH = WINDOW_HEIGHT/2; // 800/2=400
  pxMG = WINDOW_HEIGHT/(2*(zTH+1)); // 800/8=100

  switch(mode){
    case 0:  //ブロック座標
      if(x < bcTH){
        a = (zTH/bcTH *x) - zTH; // 3.0/5*1 -3.0
      }else{
        x -= bcTH; // 20-10=10
        a = (zTH/bcTH *x); // 3.0/10*1
      }
      //printf("%f\n", a);
      break;
    case 1:  //px座標(X)=>ブロック座標
      if((x>100)&&(x<700)){
        a = (int)((x-pxMG)/((WINDOW_HEIGHT-(2*pxMG))/block_count));
        break;
      }
      a = -1.0;
      break;
    case 2:  //px座標(Y)=>ブロック座標
      if((x>100)&&(x<700)){
        a = (int)((block_count-1)-((x-pxMG)/((WINDOW_HEIGHT-(2*pxMG))/block_count)));
        break;
      }
      a = -1.0;
      break;
    case 3:  //px座標=>gl2D座標
      if(x < pxTH){ //x < 400px
        a = (zTH/pxTH *x) - zTH; // 3.0/400*1 -3.0
      }else{
        x -= pxTH; // 800-400=400
        a = (zTH/pxTH *x); // 3.0/400*1
      }
      break;
    default:  //例外
      a = -1.0;
      break;
  }

  return a;
}

/* HSVtoRGB変換(カラーグラデーション用) */
void hsv2rgb(double h, double s, double v, double *r, double *g, double *b){
  int i;
  double max, min;

  max = v;
  min = max - ((s/255)*max);
  *r = *g = *b = max;

  i = h/60.0;
  switch (i) {
      case 0:
          *g = (h/60.0)*(max-min)+min;
          *b = min;
          break;
      case 1:
          *r = ((120.0-h)/60.0)*(max-min)+min;
          *b = min;
          break;
      case 2:
          *r = min;
          *b = ((h-120.0)/60.0)*(max-min)+min;
          break;
      case 3:
          *r = min;
          *g = ((240-h)/60.0)*(max-min)+min;
          break;
      case 4:
          *r = ((h-240)/60.0)*(max-min)+min;
          *g = min;
          break;
      case 5:
      default:
          *g = min;
          *b = ((360.0-h)/60.0)*(max-min)+min;
          break;
  }
}

void printString(float x, float y, char* str, int length){
	float z = -1.0f;
	glRasterPos3f(x, y, z);
	
	for (int i = 0; i < length; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}

/* 画面リサイズ（ウインドウサイズ変更に対しての対応） */
void resize(int w, int h){
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(w-h > 0){
    gluOrtho2D(-4.0*w/h, 4.0*w/h, -4.0, 4.0);
  }else{
    gluOrtho2D(-4.0, 4.0, -4.0*h/w, 4.0*h/w);
  }
}

/* 初期化関数(画面, ブロックetc.) */
void init(void){
  int i,j;
  double x,y;
  double r, g, b;
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
  srand(time(NULL));

  //プレイヤー初期化
  player1.x = B_COUNT/2;
  player1.y = 0;

  //ブロック初期化
  for(i=0; i<B_COUNT; i++){
    for(j=0; j<B_COUNT; j++){
      x = convertV(i, B_COUNT, 3.0, 0); //ブロック座標変換
      y = convertV(j, B_COUNT, 3.0, 0);

      //レインボー表示するためhsv表色系から生成する
      hsv2rgb((360.0/B_COUNT)*i, (128.0/B_COUNT)*j+127.0, 255, &r, &g, &b);

      blocks[i][j].x = x; //ブロック座標
      blocks[i][j].y = y;
      blocks[i][j].color[0] = r/255.0; //255段階を0.0~1.0に変換
      blocks[i][j].color[1] = g/255.0;
      blocks[i][j].color[2] = b/255.0;
      blocks[i][j].active = 0; //初期値は非表示
      //printf("%f, %f, %f\n", blocks[i][j].color[0], blocks[i][j].color[1], blocks[i][j].color[2]);
      //printf("%f\n", (360.0/COLOR_COUNT)*i);
    }
    //printf("\n");
  }
  
  //進捗バー初期化
  for(i=0; i<P_B_COUNT; i++){
    x = convertV(i, P_B_COUNT, 4.0, 0); //ブロック座標変換
    y = -3.6;

    progress_bar[i].x = x; //ブロック座標
    progress_bar[i].y = y;
    progress_bar[i].dy = 0.0;
    progress_bar[i].color[0] = 0.8;  //灰色指定
    progress_bar[i].color[1] = 0.8;
    progress_bar[i].color[2] = 0.8;
    progress_bar[i].active = 1; //初期値は表示（使ってない）
  }
}

void idle(void){
  glutPostRedisplay();
}

/* 画面描画間隔タイマー */
static void timer(int dummy){

  glutPostRedisplay();
  glutTimerFunc(REFRESH_RATE, timer, 0);
}

/* キーボード入力系 */
void keyin(unsigned char key, int x, int y){
  switch(key){
    case 'w':
    case 'W':
      blocks[player1.x][player1.y].active = 0;
      if((player1.y+1)<B_COUNT)  player1.y += 1;
      break;
    case 'a':
    case 'A':
      blocks[player1.x][player1.y].active = 0;
      if((player1.x-1)>-1)  player1.x -= 1;
      break;
    case 's':
    case 'S':
      blocks[player1.x][player1.y].active = 0;
      if((player1.y-1)>-1)  player1.y -= 1;
      break;
    case 'd':
    case 'D':
      blocks[player1.x][player1.y].active = 0;
      if((player1.x+1)<B_COUNT)  player1.x += 1;
      break;
    case 'n':
    case 'N':
      flag = 0;
      refresh_count = 0;
      blocks[player1.x][player1.y].active = 0;
      //プレイヤー初期化
      player1.x = B_COUNT/2;
      player1.y = 0;
      break;
    case '\033':
    case 'c':
    case 'q': exit(0);  break;
    default: break;
  }
}

/* マウス入力系 */
void mouse(int button, int state, int x, int y) //マウスコールバック関数の定義
{
   if(button==GLUT_LEFT_BUTTON && state == GLUT_DOWN)  //左クリック
  {
    printf("Pushed at (%d, %d)\n",x,y); 
    x = convertV(x, B_COUNT, 3.0, 1);  //px座標からブロック指定する座標への変換
    y = convertV(y, B_COUNT, 3.0, 2);

    //表示・非表示の切り替え
    if((x==-1)||(y==-1)||blocks[x][y].active==1){
      blocks[x][y].active = 0;
    }else{
      blocks[x][y].active = 1;
    }
    
    printf("Convert to (%d, %d)\n",x,y); 
  }

}

/* 画面描画関数 */
void display(void){
  int i,j;
  static int speed_count = 0;
  static int active_prog_bar = 0;
  static int all_refresh_count = 0;
  double r, g, b;
  char buf[6] = {0};
  
  /* 初期設定 */
  glClearColor(0, 0, 0, 0);    /* 黒背景 */
  glClear(GL_COLOR_BUFFER_BIT);  /* カラーバッファ初期化 */
  glColor3d(1.0,1.0,1.0);  /* 色 */
  
  
  //プレイヤー当たり判定
  if((blocks[player1.x][player1.y].active == 1)||(flag == 1)){
    printString(0, 0, "GAME OVER", 10);
    printString(0, -0.6, "PRESS 'Q', 'C' KEY TO EXIT", 26);
    printString(0, -0.9, "'N' KEY TO NEW GAME!!", 21);
    printString(0, -1.3, "Score", 6);
    itoa(refresh_count,buf,10);
    printString(0, -1.6, buf, 6);
    

    glutSwapBuffers();
    
    flag = 1;
    return;
  }
  blocks[player1.x][player1.y].active = 2;

  //ブロック描画
  for(i=0; i<B_COUNT; i++){
    for(j=0; j<B_COUNT; j++){
      //block構造体中の色情報より、ブロックの色の設定
      if(blocks[i][j].active == 0){
        glColor3d(0.2, 0.2, 0.2);
      }else if(blocks[i][j].active == 1){
        glColor3d(blocks[i][j].color[0], blocks[i][j].color[1], blocks[i][j].color[2]);
      }else{
        glColor3d(1.0, 1.0, 1.0);
      }
      //ブロック描画
      glRectf(blocks[i][j].x, blocks[i][j].y, blocks[i][j].x+B_WIDTH, blocks[i][j].y+B_WIDTH);
    }
    //printf("\n");
  }

  //進捗バー描画
  for(i=0; i<P_B_COUNT; i++){
    //色設定
    glColor3d(progress_bar[i].color[0],progress_bar[i].color[1],progress_bar[i].color[2]);
    //進捗バー描画
    glRectf(progress_bar[i].x,
            progress_bar[i].y+progress_bar[i].dy,
            progress_bar[i].x+0.5,
            progress_bar[i].y);
  }

  //ベース
  glColor3d(1.0,1.0,1.0);
  glRectf(-4, -3.5, 4, -4);

  //スコア等文字表示
  printString(-3.8, -1.4, "Speed", 5);
  itoa(bar_speed,buf,10);
  printString(-3.8, -1.7, buf, 6);  //スピード

  printString(-3.8, -2.0, "AllScore", 8);
  itoa(all_refresh_count,buf,10);
  printString(-3.8, -2.3, buf, 6);  //全回数

  printString(-3.8, -2.6, "Score", 5);
  itoa(refresh_count,buf,10);
  printString(-3.8, -2.9, buf, 6);  //回数

  

  //遷移時処理
  if(speed_count == bar_speed){
    progress_bar[active_prog_bar].dy = 0.5;  //アクティブのバーを最大値に設定
    for(i=0; i<P_B_COUNT; i++)  progress_bar[i].dy -= 0.04;  //すべてのバーを減少させて階段状に
    
    //アクティブバーの変更
    active_prog_bar++;

    //遷移時処理
    if(active_prog_bar == P_B_COUNT){
      //ブロックの移動
      for(i=0; i<B_COUNT; i++){
        blocks[i][0].active = 0;
        for(j=1; j<B_COUNT; j++){
          if(blocks[i][j].active == 1){
            blocks[i][j].active = 0;
            blocks[i][j-1].active = 1;
          }
        }
      }
      //上部にブロックの新規作成
      for(i=0; i<DEFAULT_CREATE_BLOCKS; i++)  blocks[rand()%B_COUNT][B_COUNT-1].active = 1;
      
      //スピード変更
      bar_speed = DEFAULT_SPEED - 2*(refresh_count/10);
      

      refresh_count++;
      all_refresh_count++;
      active_prog_bar = 0;
    }
        
    //次の遷移時処理を行うための再初期化
    speed_count = 0;
  }else{
    //遷移時処理を行わない（スキップ）
    speed_count++;
  }

  //glRectf(blocks[0][0].x, blocks[0][0].y, blocks[0][0].x-0.05, blocks[0][0].y-0.05);

  glutSwapBuffers();
}

/* メイン関数 */
int main(int argc, char** argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyin);
  glutMouseFunc(mouse);
  glutTimerFunc(100, timer, 0);
  init();
  glutMainLoop();
  return 0;
}
