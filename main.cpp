//#include "stdafx.h"

#include <string>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "GL/freeglut.h"
#include <GL/glut.h>
#pragma comment(lib, "OpenGL32.lib")

// keycodes
#define VK_W 0x57
#define VK_S 0x53

// window width, height, and fps
int width = 300;
int height = 300;
int interval = 1000 / 60;

// the ball positions
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 4;
int ball_speed = 2;

// rackets
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;

// left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;

//right racket position
float racket_right_x = width - racket_width - 10;
float racket_right_y = 50;

// the score
int score_left = 0;
int score_right = 0;


void keyboard() {
    // left racket
    if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;

    // right racket
    if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}

std::string int2str(int x) {
    // converts int to string
    std::stringstream ss;
    ss << x;
    return ss.str( );
}

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

void drawCircle(float x_in, float y_in, float radius) {

    glBegin(GL_LINE_LOOP);
    // draws an x sided shape, to resemble a circle
    for (double i = 0; i < 12; i++) {
        float theta = 2.0f * M_PI * float(i) / 12.0f;
        float x = radius * cosf(theta); // x component
        float y = radius * sinf(theta); // y component
        glVertex2f(x + x_in, y + y_in);
    }

    glEnd();

}

void drawRect(float x, float y, float width, float height) {
    // tells openGL to begin drawing a quad and then tells it
    // the four points of the quad.
    // we then call glEnd when we are finished.
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}

void draw() {
    // clear (has to be done at the beginning)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw rackets
    drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
    drawRect(racket_right_x, racket_right_y, racket_width, racket_height);

    // draw ball
    drawCircle(ball_pos_x, ball_pos_y, ball_size);

    // draw score
    int heightOffset = 15;
    int widthOffset = 10;
    drawText(width / 2 - widthOffset, height - heightOffset,
             int2str(score_left) + ":" + int2str(score_right));

    // swap buffers (has to be done at the end)
    glutSwapBuffers();
}

void vec2_norm(float& x, float &y) {
    // sets a vectors length to y
    float length = sqrt((x * x) + (y * y));
    if (length != 0.0f) {
        length = 1.0f / length;
        x *= length;
        y *= length;
    }
}

void updateBall() {
    // increases the balls position by it's direction multiplied
    // by it's speed.
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;

    // checks a left racket collision
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
            float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
            ball_dir_x = fabs(ball_dir_x);
            ball_dir_y = t;
        }

    // hit by right racket?
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
    }

    // hit left wall?
    if (ball_pos_x < 0) {
        ++score_right;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
    }

    // hit right wall?
    if (ball_pos_x > width) {
        ++score_left;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
    }

    // hit top wall?
    if (ball_pos_y > height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }

    // hit bottom wall?
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }

    // make sure that length of dir stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);

}

void update(int value) {
   //input handling
   keyboard();

   // update ball
   updateBall();

   // Call update() again in 'interval' milliseconds
   glutTimerFunc(interval, update, 0);

   // Redisplay frame
   glutPostRedisplay();
}

void enable2D(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}


// program entry point
int main(int argc, char** argv) {

    // initialize opengl (via glut)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pong");

    // Register callback functions
    glutDisplayFunc(draw);
    glutTimerFunc(interval, update, 0);

    // setup scene to 2d and set draw colour to white
    enable2D(width, height);
    glColor3f(1.0f, 1.0f, 1.0f);

    // start the whole thing
    glutMainLoop();
    return 0;
}

