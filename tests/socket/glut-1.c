#include <stdio.h>
#include <GL/glut.h> // GLUT, inlcude glu.h and gl.h

void display() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);		  // Clear the color buffer (background)
	
	// Draw a Red 1x1 Square centered at origin
	glBegin(GL_QUADS);						// Each set of 4 vertices form a quad
	glColor3f(1.0f, 0.0f, 0.0f); 			// Red
	glVertex2f(-0.5f, -0.5f);			// x, y
	glVertex2f( 0.5f, -0.5f);
	glVertex2f( 0.5f,  0.5f);
	glVertex2f(-0.5f,  0.5f);
	glEnd();
	
	glFlush(); // Render now
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);;					// Initialize GLUT
	glutCreateWindow("OpenGL Setup Test");	// Create a window with the given title
	glutInitWindowSize(720, 720);			// Set the window's initial width & height
	glutInitWindowPosition(550, 550);			// Position window's initial top-left corner
	glutDisplayFunc(display);				// Register display callback handler for window re-paint
	glutMainLoop();							// Enter the event-processing loop
	return 0;
}
	
