#include<stdio.h>
#include<GL/glut.h>
#include<math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define B_COUNT 20 //ブロック数
#define P_B_COUNT 26 //進捗バー数
#define B_WIDTH 0.25 //B_COUNTと一緒に変更する
#define REFRESH_RATE 20 //msec

typedef struct {   
    double x;
    double y;
    double color[3];
    double width;
    double height;
    int active;
} block;



double rotAng = 0.0;
int keyIn = 0;
int bar_speed = 5;
block blocks[B_COUNT][B_COUNT];
block progress_bar[P_B_COUNT];

/* いろいろ座標変換してくれる関数
mode
  0:ブロック座標
  1:px座標(X)=>ブロック座標
  2:px座標(Y)=>ブロック座標
  3:px座標=>gl2D座標) */
double convertV(int x, int block_count, double zTH, int mode)
{
  int bcTH, pxTH, pxMG;
  double a;

  bcTH = block_count/2; // 20/2=10
  pxTH = WINDOW_HEIGHT/2; // 800/2=400
  pxMG = WINDOW_HEIGHT/(2*(zTH+1)); // 800/8=100

  switch(mode){
    case 0:
      if(x < bcTH){
        a = (zTH/bcTH *x) - zTH; // 3.0/5*1 -3.0
      }else{
        x -= bcTH; // 20-10=10
        a = (zTH/bcTH *x); // 3.0/10*1
      }
      //printf("%f\n", a);
      break;
    case 1:
      if((x>100)&&(x<700)){
        a = (int)((x-pxMG)/((WINDOW_HEIGHT-(2*pxMG))/block_count));
        break;
      }
      a = -1.0;
      break;
    case 2:
      if((x>100)&&(x<700)){
        a = (int)((block_count-1)-((x-pxMG)/((WINDOW_HEIGHT-(2*pxMG))/block_count)));
        break;
      }
      a = -1.0;
      break;
    case 3:
      if(x < pxTH){ //x < 400px
        a = (zTH/pxTH *x) - zTH; // 3.0/400*1 -3.0
      }else{
        x -= pxTH; // 800-400=400
        a = (zTH/pxTH *x); // 3.0/400*1
      }
      break;
    default: 
      a = -1.0;
      break;
  }

  return a;
}

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
    x = convertV(i, P_B_COUNT, 4.0, 0);
    y = -3.6;

    progress_bar[i].x = x; //ブロック座標
    progress_bar[i].y = y;
    progress_bar[i].color[0] = 0.8;
    progress_bar[i].color[1] = 0.8;
    progress_bar[i].color[2] = 0.8;
    progress_bar[i].active = 1; //初期値は表示
  }
}

void idle(void){
  glutPostRedisplay();
}

static void timer(int dummy){

  if(keyIn==1){
    rotAng += 3.0*M_PI/180.0;
  }else if(keyIn==2){
    
  }else if(keyIn==3){
    
  }
  keyIn=0;
  glutPostRedisplay();
  glutTimerFunc(REFRESH_RATE, timer, 0);
}

void keyin(unsigned char key, int x, int y){
  switch(key){
    case ' ':  keyIn=1;  break;
    case 'y':  keyIn=2;  break;
    case 'z':  keyIn=3;  break;
    case '\033':
    case 'c':
    case 'q': exit(0);  break;
    default: break;
  }
}

void mouse(int button, int state, int x, int y) //マウスコールバック関数の定義
{
   if(button==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    printf("Pushed at (%d, %d)\n",x,y); 
    x = convertV(x, B_COUNT, 3.0, 1);
    y = convertV(y, B_COUNT, 3.0, 2);
    if((x==-1)||(y==-1)||blocks[x][y].active==1){
      blocks[x][y].active = 0;
    }else{
      blocks[x][y].active = 1;
    }
    
    printf("Convert to (%d, %d)\n",x,y); 
  }

}

void display(void){
  int i,j;
  static int speed_count = 0;
  double r, g, b;
  double theta, dt, x, y;
  

  glClearColor(0, 0, 0, 0);    /* 黒背景 */
  glClear(GL_COLOR_BUFFER_BIT);  /* カラーバッファ初期化 */
  glColor3d(1.0,1.0,1.0);  /* 色 */
  
  
  /* 頂点間に線を引く．始点と終点も結ぶ */
  /*
  glBegin(GL_LINE_LOOP);
  dt = 4.0*M_PI/5.0;  // 描画する頂点の角度差
  theta = rotAng;  // 初期角度
  for(i=0; i<5; i++){    // 座標指定と描画
    x = cos(theta);
    y = sin(theta);
    glVertex2d(x,y);
    theta += dt;
  }
  glEnd();*/

  //ブロック描画
  for(i=0; i<B_COUNT; i++){
    for(j=0; j<B_COUNT; j++){
      if(blocks[i][j].active == 0){
        glColor3d(0.2, 0.2, 0.2);
      }else{
        glColor3d(blocks[i][j].color[0], blocks[i][j].color[1], blocks[i][j].color[2]);
      }
      
      glRectf(blocks[i][j].x, blocks[i][j].y, blocks[i][j].x+B_WIDTH, blocks[i][j].y+B_WIDTH);
    }
    //printf("\n");
  }

  //進捗バー描画
  for(i=0; i<P_B_COUNT; i++){
    glColor3d(progress_bar[i].color[0],progress_bar[i].color[1],progress_bar[i].color[2]);
    glRectf(progress_bar[i].x, progress_bar[i].y+0.2, progress_bar[i].x+0.5, progress_bar[i].y);
  }

  //ベース
  glColor3d(1.0,1.0,1.0);
  glRectf(-4, -3.5, 4, -4);

  //glRectf(blocks[0][0].x, blocks[0][0].y, blocks[0][0].x-0.05, blocks[0][0].y-0.05);

  glutSwapBuffers();
}

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
