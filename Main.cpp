#include "main.h"          // this header

#include <stdlib.h>          // standard library
#include <math.h>          // fmod
#include <stdio.h>          // cout
#include <iostream>          // cout

#include "HalfEdgeDSRendering.h"     // visualizing the data structure

// ==============
// === BASICS ===
// ==============

int main(int argc, char** argv)
{
     // initialize openGL window
     glutInit(&argc, argv);
     glutInitWindowPosition(300, 200);
     glutInitWindowSize(600, 400);
     glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
     glutCreateWindow("Praktikum GMCAD"); 
     // link functions to certain openGL events
     glutDisplayFunc(renderScene);
     glutReshapeFunc(reshape);  
     glutMouseFunc(mousePressed);
     glutMotionFunc(mouseMoved);
     glutKeyboardFunc(keyPressed);
     // further initializations
     setDefaults();
     initializeGL();
     // data structure stuff
     std::cout << heDS << std::endl << std::endl;
     if (heDS.getHalfEdges().size() > 0) activeHE = heDS.getHalfEdges().front();
     else activeHE = nullptr;
     // activate main loop
     glutMainLoop();
     return 0;
}

void setDefaults()
{
     // scene Information
     transX = 0.0f;
     transY = 0.0f;
     transZ = -8.0f;
     angleX = 0.0f;
     angleY = 0.0f;
     // mouse information
     mouseX = 0;
     mouseY = 0;
     mouseButton = 0;
     mouseSensitivy = 1.0f;
}

void initializeGL()
{
     // black screen
     glClearColor(0, 0, 0, 0);
     // enable depth buffer
     glEnable(GL_DEPTH_TEST);
     // Use Point Smoothing
     glPointSize(5.0f);
     glLineWidth(2.0f);
     // set shading model
     glShadeModel(GL_SMOOTH);
     // set lighting (white light)
     GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };  
     GLfloat ambientLight[] =   { 0.1f, 0.1f, 0.1f, 1.0f };
     GLfloat diffuseLight[] =   { 1.0f, 1.0f, 1.0f, 1.0f };
     GLfloat specularLight[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
     GLfloat shininess = 0.9f * 128.0f;
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
     glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
     glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
     glLightf(GL_LIGHT0, GL_SHININESS, shininess);
     glEnable(GL_LIGHT0);
     GLfloat lp[] = { 0.0f, 0.0f, 0.0f, 1.0f };
     glLightfv(GL_LIGHT0, GL_POSITION, lp);
     // enable lighting by glColor
     glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
     glEnable(GL_COLOR_MATERIAL);
     // key bindings => cout
     coutHelp();
}

void reshape(GLint width, GLint height)
{
     glViewport(0, 0, width, height);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(65.0, (float)width / (float)height, 0.1f, 1000);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

// =================
// === RENDERING ===
// =================

void renderCS()
{
     // x
     glPushMatrix();
     glColor3f(1, 0, 0); 
     renderArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f), 0.04f);
     glPopMatrix();
     // y
     glPushMatrix();
     glColor3f(0, 1, 0);
     renderArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), 0.04f);
     glPopMatrix();
     // z
     glPushMatrix();
     glColor3f(0, 0, 1);
     renderArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f), 0.04f);
     glPopMatrix();
}

void renderScene()
{
     // clear and set camera
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glLoadIdentity();
	 // lighting from the viewer perspective
     GLfloat lp[] = { 0.0f, 0.0f, 0.5f, 1.0f };
     glLightfv(GL_LIGHT0, GL_POSITION, lp);
     // translate scene in viewing direction
     glTranslatef(transX, transY, transZ);  
     // rotate scene
     glRotatef(angleX, 0.0f, 1.0f, 0.0f);
     glRotatef(angleY, 1.0f, 0.0f, 0.0f);

	 glEnable(GL_LIGHTING);
     renderDS(heDS);

     renderHEActive(activeHE);
     for(auto loopIterator = activeHE->nextHE;
		 loopIterator != activeHE;
		 loopIterator = loopIterator->nextHE)
	 {
          renderHE(loopIterator, Vec3f(0.004f, 0.141f, 0.337f));
     }
     renderCS();

     // swap Buffers
     glFlush();
     glutSwapBuffers();
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
		// ESC => exit
	case 27:
		exit(0);
		break;
		// help file
	case 'h':
	case 'H':
		coutHelp();
		break;
		// reset view
	case 'r':
	case 'R':
		setDefaults();
		glutPostRedisplay();     // use this whenever 3D data changed to redraw the scene
		break;
		//Highlight Navigation
	case 'w':
	case 'W': //cycle loops on current face
	{
		Loop* currentLoop = activeHE->toLoop;
		Face* currentFace = currentLoop->toFace;
		if (currentLoop == currentFace->outerLoop) { //jump to inner loops
			if (currentFace->innerLoop) {
				activeHE = currentFace->innerLoop->toHE;
			}
			else {
				std::cout << "Current Face has no inner loops!" << std::endl;
			}
		}
		else {
			if (currentLoop->nextLoop == currentFace->innerLoop) { //cycled through all inner, jump to outer
				activeHE = currentFace->outerLoop->toHE;
			}
			else {
				activeHE = currentLoop->nextLoop->toHE;
			}
		}
		break;
	}
	case 's':
	case 'S': //rev cycle loops on current face
	{
		Loop* currentLoop = activeHE->toLoop;
		Face* currentFace = currentLoop->toFace;
		if (currentLoop == currentFace->outerLoop) { //jump to inner loops
			if (currentFace->innerLoop) {
				activeHE = currentFace->innerLoop->prevLoop->toHE;
			}
			else {
				std::cout << "Current Face has no inner loops!" << std::endl;
			}
		}
		else {
			if (currentLoop->prevLoop == currentFace->innerLoop->prevLoop) { //cycled through all inner, jump to outer
				activeHE = currentFace->outerLoop->toHE;
			}
			else {
				activeHE = currentLoop->prevLoop->toHE;
			}
		}
		break;
	}
	case 'd':
	case 'D':
		activeHE = activeHE->nextHE;
		break;
	case 'a':
	case 'A':
		activeHE = activeHE->prevHE;
		break;
	case 'q':
	case 'Q':
		activeHE = activeHE->getEdgeSibling();
		break;
	}
	glutPostRedisplay();
}

void mousePressed(int button, int state, int x, int y)
{
     mouseButton = button;
     mouseX = x; 
     mouseY = y;
}

void mouseMoved(int x, int y)
{
     // rotate (cap angleY within [-85°, +85°])
     if (mouseButton == GLUT_LEFT_BUTTON)
     {
          angleX = fmod(angleX + (x-mouseX) * mouseSensitivy, 360.0f);
          angleY += (y-mouseY) * mouseSensitivy;
          if (angleY > 85) angleY = 85;
          if (angleY < -85) angleY = -85;
          glutPostRedisplay();
     }
     // zoom (here translation in z)
     if (mouseButton == GLUT_RIGHT_BUTTON)
     {
          transZ -= 0.2f * (y-mouseY) * mouseSensitivy;
          glutPostRedisplay();
     }
     // translation in xy
     if (mouseButton == GLUT_MIDDLE_BUTTON) 
     {
          transX += 0.05f * (x-mouseX) * mouseSensitivy;
          transY -= 0.05f * (y-mouseY) * mouseSensitivy;
          glutPostRedisplay();
     }
     // update mouse for next relative movement
     mouseX = x;
     mouseY = y;
}

// ===============
// === VARIOUS ===
// ===============

void coutHelp()
{
     std::cout << std::endl;
     std::cout << "====== KEY BINDINGS ======" << std::endl;
     std::cout << "ESC: exit" << std::endl;
     std::cout << "H: show this (H)elp file" << std::endl;
     std::cout << "R: (R)eset view" << std::endl;
     std::cout << "====== DS NAVIGATION =====" << std::endl;
	 std::cout << "D: Next half edge in current loop" << std::endl;
	 std::cout << "A: Prev half edge in current loop" << std::endl;
	 std::cout << "W: Next loop on current surface" << std::endl;
	 std::cout << "S: Prev loop on current surface" << std::endl;
	 std::cout << "Q: Switch to sibling half edge" << std::endl;
     std::cout << "==========================" << std::endl;
     std::cout << std::endl;
}
