#include<stdio.h>
#include<GL/glut.h>
#include<math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define B_COUNT 20
#define B_MARGIN 0.14 //B_COUNT/100 *3

typedef struct {   
    double x;
    double y;
    double color[3];
    double width;
    double height;

} block;



double rotAng = 0.0;
int keyIn = 0;
block blocks[B_COUNT][B_COUNT];


double convertV(int x, int mode)
{
  int bcTH, pxTH;
  double zTH=2.0;
  double a;

  switch(mode){
    case 0:
      bcTH = B_COUNT/2; // 10/2=5
      pxTH = WINDOW_WIDTH/2; // 800/2=400
      if(x < bcTH){
        a = (zTH/bcTH *x) - zTH; // 3.0/5*1 -3.0
      }else{
        x -= bcTH; // 10-5=5
        a = (zTH/bcTH *x); // 3.0/5*1
      }
      //printf("%f\n", a);
    break;
    default: break;
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
    gluOrtho2D(-3.0*w/h, 3.0*w/h, -3.0, 3.0);
  }else{
    gluOrtho2D(-3.0, 3.0, -3.0*h/w, 3.0*h/w);
  }
}

void init(void){
  int i,j;
  double x,y;
  double r, g, b;
  
  glClearColor(0.0, 0.0, 0.0, 1.0);

  //ブロック初期化
  for(i=0; i<B_COUNT; i++){
    for(j=0; j<B_COUNT; j++){
      x = convertV(i,0);
      y = convertV(j,0);
      hsv2rgb((360.0/B_COUNT)*i, (128.0/B_COUNT)*j+127.0, 255, &r, &g, &b);

      blocks[i][j].x = x;
      blocks[i][j].y = y;
      blocks[i][j].color[0] = r/255.0;
      blocks[i][j].color[1] = g/255.0;
      blocks[i][j].color[2] = b/255.0;
      //printf("%f, %f, %f\n", blocks[i][j].color[0], blocks[i][j].color[1], blocks[i][j].color[2]);
      //printf("%f\n", (360.0/COLOR_COUNT)*i);
    }
    //printf("\n");
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
  glutTimerFunc(5000, timer, 0);
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
   }

}

void display(void){
  int i,j;
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
  

  //ベース
  //glRectf(-3, -1.8, 3, -2);

  

  //ブロック描画
  for(i=0; i<B_COUNT; i++){
    for(j=0; j<B_COUNT; j++){
      glColor3d(blocks[i][j].color[0], blocks[i][j].color[1], blocks[i][j].color[2]);
      glRectf(blocks[i][j].x, blocks[i][j].y, blocks[i][j].x+B_MARGIN, blocks[i][j].y+B_MARGIN);
    }
    //printf("\n");
  }

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
