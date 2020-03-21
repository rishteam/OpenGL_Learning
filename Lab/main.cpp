//
//  main.cpp
//  OpenGL_08_CUBE
//
//  Created by apple on 15/1/9.
//  Copyright (c) 2015年 cc. All rights reserved.
//
#include "../Include/Common.h"

//可印出openGL矩陣內容
void printMatrix()
{
	GLfloat model[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, model);
	for (int i = 0; i < 16; i++)
	{
		if (i % 4 == 0 && i != 0)
		{
			printf("\n");
		}
		printf("%.f ", model[i]);
	}
	printf("\n----------------------\n");
}

//畫軸
void drawaxis()
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	// red x axis
	glVertex3f(-4.0, 0.0f, 0.0f);
	glVertex3f(4.0, 0.0f, 0.0f);

	// arrow
	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, 1.0f, 0.0f);

	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, -1.0f, 0.0f);
	glEnd();
	glFlush();

	// green y axis
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -4.0f, 0.0f);
	glVertex3f(0.0, 4.0f, 0.0f);

	// arrow
	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(1.0, 3.0f, 0.0f);

	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(-1.0, 3.0f, 0.0f);
	glEnd();
	glFlush();

	// blue z axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0f, -4.0f);
	glVertex3f(0.0, 0.0f, 4.0f);

	// arrow
	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, 1.0f, 3.0f);

	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, -1.0f, 3.0f);
	glEnd();
}


void display()
{
	//清除color跟depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//視圖矩陣
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//位移
	glTranslatef(3.0, 0.0, 0.0);
	//旋轉
	glRotatef(90.0, 0.0, 1.0, 0.0);

	printMatrix();

	//可直接操作矩陣
	GLfloat shear = 0.0f;
	GLfloat m[16] = {
		1.0f, 0.0f, 0.0f, 3.0f,
		shear, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};
	glMultMatrixf(m);
	printMatrix();
	//畫方塊
	glutWireCube(1.0);
	glPushMatrix();
	drawaxis();
	glPopMatrix();
	// glColor3f(1,0,0);
	//清空buffer，把command送到GPU執行
	glFlush();
}

/**
 *  调整視窗尺寸
 *
 *  @param width  宽度
 *  @param height 高度
 */
void reshape(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	//之后的矩阵操作定义为透视矩阵操作
	glMatrixMode(GL_PROJECTION);
	//等于是将之前矩阵变换导致变化过的栈顶矩阵重新归位，置为单位矩阵！等于是之前的矩阵变换带来的影响到此为止了！
	glLoadIdentity();
	//将当前矩阵与一个透视矩阵相乘，把当前矩阵转换为透视矩阵
	//即定义了一个平截头体(视景体)，在视景体内部的模型将被显示，之外的将被裁减
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	//之后的矩阵操作定义为模型视图矩阵操作
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, const char *argv[])
{
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CUBE");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}