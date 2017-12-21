#include <GL/glew.h>
#include <vector>
#include <thread>
#include <string>
#include <array>
#include <cmath>
#include <iostream>
#include <chrono>
#include <GL/glu.h>

// STD Headers
#define _USE_MATH_DEFINES

#include <thread>

// freeglut
#include <GL/glew.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <GL/freeglut.h>


#pragma comment(lib,"glew32.lib") 
#pragma comment(lib,"freeglut.lib") 


int main(int argc, char** argv){
// initial glut
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

// create glut window
glutInitWindowSize(640, 640);
glutCreateWindow("MyOpenVR");

// initial glew
glewInit();

glutMainLoop();
}