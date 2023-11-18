#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include <GL/glut.h>


const float XLIM = 500.00f;
const float YLIM = 550.00f;

const float BC[3] = {0.0f, 0.0f, 0.0f};
const float MC[3] = {0.33f, 0.33f, 0.33f};
const float UC[3] = {0.00f, 1.00f, 0.00f};

const float SS = 10.00f;
const float BW = 20.0f * SS;
const float BH = 10.0f * SS;

const float MW = 5.0f * SS;
const float MH = 20.0f * SS;

const float UW = 10.0f * SS;
const float UH = 10.0f * SS;


void bottom(float cx, float cy) {
    const float hbw = BW/2;
    const float hbh = BH/2;
    
    glBegin(GL_QUADS);
        glColor3f(BC[0], BC[1], BC[2]);
        // bottom left
        glVertex2f(cx-hbw, cy-hbh);
        // bottom right
        glVertex2f(cx+hbw, cy-hbh);
        // upper right
        glVertex2f(cx+hbw, cy+hbh);
        // upper left
        glVertex2f(cx-hbw, cy+hbh);
    glEnd();
}

void middle(float cx, float cy) {
    const float hmw = MW/2;
    const float hmh = MH/2;
    
    glBegin(GL_POLYGON);
        glColor3f(MC[0], MC[1], MC[2]);
        // bottom left
        glVertex2f(cx-hmw, cy-hmh);
        // bottom right
        glVertex2f(cx+hmw, cy-hmh);
        // upper right
        glVertex2f(cx+hmw, cy+hmh);
        // upper left
        glVertex2f(cx-hmw, cy+hmh);
    glEnd();
}


void upper(float cx, float cy) {
    const float huw = UW/2;
    const float huh = UH/2;
    glColor3f(UC[0], UC[1], UC[2]);
    glBegin(GL_QUADS);
        // bottom left
        glVertex2f(cx-huw, cy-huh);
        // bottom right
        glVertex2f(cx+huw, cy-huh);
        // upper right
        glVertex2f(cx+huw, cy+huh);
        // upper left
        glVertex2f(cx-huw, cy+huh);
    glEnd();

    const float htuw = (UW/3)/2;
    const float htuh = (UH/3)/2;
    
    // upper triangles
    // middle triangle
    glBegin(GL_TRIANGLES);
      glVertex2f(cx-htuw, cy+huh); // bl
      glVertex2f(cx+htuw, cy+huh); // br
      glVertex2f(cx, cy+htuh*2);   // um
    glEnd();
}


/* display routine */
void labDisp(void) {
	// printf("inside podgun disp..\n");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

  const float CX = 250.00f;
  const float CY = 150.00f;
  bottom(CX, CY);
  middle(CX, CY + (MH/2));
  upper(CX, CY + (BH/2) + (MH/2) + (UH/2));


	glFlush();
}


/* initialize local glut */
void initGlut(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, XLIM, 0.0f, YLIM);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE |  GLUT_RGB);
	glutInitWindowSize(XLIM, YLIM);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("Lab Eval(181-15-955)");
	initGlut();

	glutDisplayFunc(labDisp);
	glutMainLoop();

	return 0;
}
