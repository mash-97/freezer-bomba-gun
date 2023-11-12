#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glut.h>


const float GLOBAL_MOVE_RATE = 2.0;
const float XLIM = 500;
const float YLIM = 500;
const float MOV_SPEED = 0.1;

typedef struct Point {
  float x;
  float y;
} Point;

Point *newPoint(float  x, float y) {
  Point *point = (Point *)malloc(sizeof(struct Point));
  point->x = x;
  point->y = y;
  return point;
}

typedef struct SquarePoints {
  Point blp;
  Point brp;
  Point urp;
  Point ulp;
} SquarePoints;

void dSP(SquarePoints sp) {
  printf("%p\n", &sp);
  printf("blp.x: %.3f, blp.y: %.3f\n", sp.blp.x, sp.blp.y);
  printf("brp.x: %.3f, brp.y: %.3f\n", sp.brp.x, sp.brp.y);
  printf("urp.x: %.3f, urp.y: %.3f\n", sp.urp.x, sp.urp.y);
  printf("ulp.x: %.3f, ulp.y: %.3f\n", sp.ulp.x, sp.ulp.y);
}



typedef struct Pod {
  float width;
  float height;
  float x;
  float y;
  char direction;
  void (* nextX) (struct Pod *);
  SquarePoints (* square_points) (struct Pod *); 
} Pod;

SquarePoints square_points(struct Pod *self) {
  Point blp;
  blp.x = self->x - (self->width/2);
  blp.y = self->y - (self->height/2);
  
  Point brp;
  brp.x = self->x + (self->width/2);
  brp.y = blp.y;
  
  Point urp;
  urp.x = brp.x;
  urp.y = self->y + (self->height/2);
  
  Point ulp;
  ulp.x = blp.x;
  ulp.y = urp.y;

  SquarePoints sp = {blp, brp, urp, ulp};
  return sp;
}

void nextX(struct Pod *self) {
  float next_step = 1.0;
  if(self->direction == 1) {
    next_step = self->x + MOV_SPEED * GLOBAL_MOVE_RATE;
    if(next_step <= XLIM) {
      self->x = next_step;
    }
    else self->direction = -1;
  }
  else {
    next_step = self->x - MOV_SPEED * GLOBAL_MOVE_RATE;
    if(next_step >= (float)0.00) 
      self->x = next_step;
    else self->direction = 1;
  }
}

struct Pod * newPod(float width, float height, float x, float y, char direction) {
  struct Pod *pod = (struct Pod *)malloc(sizeof(struct Pod));
  pod->width = width;
  pod->height = height;
  pod->x = x;
  pod->y = y;
  pod->direction = direction;
  pod->nextX = nextX;
  pod->square_points = square_points;
  return pod;
}


void dPod(Pod * pod) {
  printf("\n");
  printf("pod: %p\n", pod);
  printf("width: %.3f, height: %.3f\n", pod->width, pod->height);
  printf("x: %.3f, y: %.3f\n", pod->x, pod->y);
  printf("direction: %d\n", pod->direction);
}

int CURRENT_PODS_SIZE = 0;
const int PODS_SIZE = 2;
struct Pod *PODS[2];

void init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glPointSize(1.0); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, XLIM, 0, YLIM);
}


void display(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  //printf("Paint PODS..\n"); 
  for(int i=0; i<CURRENT_PODS_SIZE; i++) {
    //printf("%d: %p\n", i, PODS[i]);
    SquarePoints sp = PODS[i]->square_points(PODS[i]);
    //dSP(sp);
    glBegin(GL_QUADS);
      glColor3f(0.0f, 1.0f, 0.0f);
      glVertex2f(sp.blp.x, sp.blp.y);
      glVertex2f(sp.brp.x, sp.brp.y);
      glVertex2f(sp.urp.x, sp.urp.y);
      glVertex2f(sp.ulp.x, sp.ulp.y);
    glEnd();
  }
  
  glFlush();
}


void keyboardSpecialFunc(int special_key, int x, int y) {
  printf("special_key: %d, x: %d, y: %d\n", special_key, x, y);
  switch(special_key) {
    case 101:
      PODS[0]->height += 1.0;
      break;
    default:
      printf("# special_key: %d, x: %d, y: %d\n", special_key, x, y);
  }
}


void idleFunc(void) {
  for(int i=0; i<CURRENT_PODS_SIZE; i++)
    PODS[i]->nextX(PODS[i]);
  glutPostRedisplay();
}


int main(int argc, char **argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(XLIM, YLIM);
  glutInitWindowPosition(300, 300);
  glutCreateWindow("Glut Test Again");

  PODS[CURRENT_PODS_SIZE++] = newPod(50.0, 20.0, XLIM/2, 0.00, 1);
  PODS[CURRENT_PODS_SIZE++] = newPod(50.0, 20.0, XLIM/2, YLIM, -1);
  dPod(PODS[0]);
  dPod(PODS[1]); 

  glutDisplayFunc(display);
  init();
  
  glutIdleFunc(idleFunc);
  glutSpecialFunc(keyboardSpecialFunc);
  glutMainLoop();


  return 0;
}


