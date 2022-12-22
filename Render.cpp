#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <list>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


//added by Vano
/*void vectProz(double A[], double B[], double C[], int k)
{
	double A1[] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
	double B1[] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };

	list <double> list_ = { k * (A1[1] * B1[2] - A1[2] * B1[1]), k * (A1[2] * B1[0] - A1[0] * B1[2]), k * (A1[0] * B1[1] - A1[1] * B1[0]) };
	return list_;
}*/

#define BALL_SPEED  0.02f       // �������� ��������� �������� ������
#define MAX_STEPS   25.0f       // ����� �����, ������������ �����
static float RADIUS = 0.5f;
bool textureMode = true;
bool lightMode = true;
static GLint dit = 25;
void round12(double step, double pointA[], double pointB[], double pointC[])
{
	glColor3d(0, 0, 0);

	double vect_AB[] = { pointA[0] - pointB[0], pointA[1] - pointB[1] };
	double centre[] = { (pointA[0] + pointB[0]) / 2,  (pointA[1] + pointB[1]) / 2 };

	double length = sqrt(vect_AB[0] * vect_AB[0] + vect_AB[1] * vect_AB[1]);

	double radius = length / 2;

	double Fi = acos(vect_AB[0] / (length + 1));


	glBegin(GL_TRIANGLE_FAN);


	for (double i = -Fi - 0.3; i <= 3.2 - Fi + 0.2; i += step)
	{

		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointA[2] };
		glVertex3dv(point);

	}



	glEnd();


	glBegin(GL_TRIANGLE_FAN);


	for (double i = -Fi - 0.3; i <= 3.2 - Fi + 0.2; i += step)
	{

		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointC[2] };
		glVertex3dv(point);

	}



	glEnd();


	glColor3d(0.2, 0.2, 0.2);

	glBegin(GL_QUADS);


	for (double i = -Fi - 0.3 - step; i <= 3.2 - Fi + 0.2 + step; i += step)
	{
		double point[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointA[2] };
		double point1[] = { radius * cos(i) + centre[0], radius * sin(i) + centre[1], pointC[2] };
		double point2[] = { radius * cos(i + step) + centre[0], radius * sin(i + step) + centre[1], pointA[2] };
		double point3[] = { radius * cos(i + step) + centre[0], radius * sin(i + step) + centre[1], pointC[2] };
		glVertex3dv(point);
		glVertex3dv(point2);
		glVertex3dv(point3);
		glVertex3dv(point1);

	}
	glEnd();
}

struct CVector3
{
public:
	float x, y, z;
};

float g_CurrentTime = 0.0f; // ������� ������� ����� �� ������ (����� 0 � 1)

CVector3 g_vStartPoint = { -7,  0,  0 };   // ��������� ����� ������
CVector3 g_vControlPoint1 = { -2,  3,  2 };   // ������ ����������� �����
CVector3 g_vControlPoint2 = { 2, -3, -2 };   // ������ ����������� �����
CVector3 g_vEndPoint = { 7,  0,  0 };   // �������� ����� ������


//���������� ��������� ����� �� ������
CVector3 PointOnCurve(CVector3 p1, CVector3 p2, CVector3 p3, CVector3 p4, float t)
{
	float var1, var2, var3;
	CVector3 vPoint = { 0.0f, 0.0f, 0.0f };
	// F(t) = P1 * ( 1 - t )^3 + P2 * 3 * t * ( 1 - t )^2 + P3 * 3 * t^2 * ( 1 - t ) + P4 * t^3

	// ������ � ���������� (1-t):
	var1 = 1 - t;

	// ������ � ���������� (1-t)^3
	var2 = var1 * var1 * var1;

	// ������� � ���������� t^3
	var3 = t * t * t;

	vPoint.x = var2 * p1.x + 3 * t * var1 * var1 * p2.x + 3 * t * t * var1 * p3.x + var3 * p4.x;
	vPoint.y = var2 * p1.y + 3 * t * var1 * var1 * p2.y + 3 * t * t * var1 * p3.y + var3 * p4.y;
	vPoint.z = var2 * p1.z + 3 * t * var1 * var1 * p2.z + 3 * t * t * var1 * p3.z + var3 * p4.z;

	return(vPoint);
}

//������ ����� ���
void DrawSphere(float x, float y, float z, float radius)
{
	GLUquadricObj* pSphere = gluNewQuadric();
	//glBindTexture(GL_TEXTURE_2D, texId);

	glPushMatrix();

	glTranslatef(x, y, z);
	//��� ����� ���������� � �������� �����,����� ����� ������� ���� � 0.5))))))
	// ��� ������� ���� �����������
	//radius = 0.5;

	// ������ ����� � ���������� �������� � ������������ 25.
	gluSphere(pSphere, radius, 25, dit);

	glPopMatrix();

	gluDeleteQuadric(pSphere);
}

void drawRocket()
{

}

//��������� � ������ ���������� �����
void RenderScene()
{
	
	static float rotateY = 0.0f;    // ���������� ��� ��������

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//������ ����� � �������� ������������ �������.

	gluLookAt(0, 0.5f, 10.0f, 0, 0.5f, 0, 0, 1, 0);

	glDisable(GL_LIGHTING);
	glColor3ub(0, 255, 0);

	CVector3 vPoint = { 0.0f, 0.0f, 0.0f };   // on CVector3 ��� �������� �����

	// ������� ��� ��� ����
	glRotatef(rotateY, 0.0f, 0.5f, 0.0f);

	//rotateY += 0.05f;
	// ����������� �������� �������������
	//� ����� ��������
	if (GetKeyState(VK_UP) < 0)
	{
		rotateY += 0.9f;
	}
	if (GetKeyState(VK_DOWN) < 0)
	{
		rotateY -= 1.8f;
	}
	//�������� ����� �������� � 1.5

	glLineWidth(1.5);

	glBegin(GL_LINE_STRIP);

	for (float t = 0; t <= (1 + (1.0f / MAX_STEPS)); t += 1.0f / MAX_STEPS)
	{
		// ������� 4 ����� �����. ����� ������� t, ������� �����/���������/����� ����� �� ������ �� 0 �� 1.
		// ���� �������� 0 � �������� ��������� ����� �����, ���� 1 � ��������.

		// �������� ������� ����� �� �����
		vPoint = PointOnCurve(g_vStartPoint, g_vControlPoint1, g_vControlPoint2, g_vEndPoint, t);

		// ������ ������� ����� �� ���������� t ����� �� �����.
		glVertex3f(vPoint.x, vPoint.y, vPoint.z);
	}

	glEnd();


	//�����������
	glLineWidth(1.5);

	glBegin(GL_LINE_STRIP);
		glColor3ub(255, 0, 0);
		glVertex3f(g_vStartPoint.x, g_vStartPoint.y, g_vStartPoint.z);
		glVertex3f(g_vControlPoint1.x, g_vControlPoint1.y, g_vControlPoint1.z);
	glEnd();

	glLineWidth(1.5);
	glBegin(GL_LINE_STRIP);
		glColor3ub(255, 0, 0);
		glVertex3f(g_vEndPoint.x, g_vEndPoint.y, g_vEndPoint.z);
		glVertex3f(g_vControlPoint2.x, g_vControlPoint2.y, g_vControlPoint2.z);
	glEnd();
	//

	glEnable(GL_LIGHTING);

	vPoint = PointOnCurve(g_vStartPoint, g_vControlPoint1,
		g_vControlPoint2, g_vEndPoint, g_CurrentTime);

	
	// ������ ����� � �������� 0.2 � ������� ����� ������
	DrawSphere(vPoint.x, vPoint.y, vPoint.z, RADIUS);

	

	// ������ ����������� �����
	DrawSphere(g_vControlPoint1.x, g_vControlPoint1.y, g_vControlPoint1.z, 0.1f);

	// ������
	DrawSphere(g_vControlPoint2.x, g_vControlPoint2.y, g_vControlPoint2.z, 0.1f);
	
	static int ONOFF = 1;

	if (ONOFF == 1)
	{
		if ((g_CurrentTime <= 1) && (GetKeyState(VK_LEFT) > 0))
		{
			g_CurrentTime += 0.001;
			if (g_CurrentTime > 1)
			{
				g_CurrentTime = 0;
			}
		}

		rotateY += 0.3f;
	}
	if (GetKeyState('O') < 0)
	{
		if (ONOFF == 1)
		{
			ONOFF = 0;
		}
		else
		{
			ONOFF = 1;
		}
	}
	/*if ((g_CurrentTime > 0) && (g_CurrentTime < 1))
	{
		g_CurrentTime += BALL_SPEED * -0.1;
	}*/

	if (GetKeyState(VK_LEFT) < 0)
	{
		g_CurrentTime += BALL_SPEED*1.1;
		if (g_CurrentTime > 1)
		{
			g_CurrentTime = 0;
		}
	}
	if (GetKeyState(VK_RIGHT) < 0) {
		g_CurrentTime -= BALL_SPEED*1.5;
		if (g_CurrentTime < 0)
		{
			g_CurrentTime = 1;
		}
	}
	if (GetKeyState('T') < 0)
	{
		dit += 1.1;
	}
	if (GetKeyState('Y') < 0)
	{
		dit -= 0.1;
	}
	if (GetKeyState(VK_OEM_PLUS) < 0)
	{
		RADIUS += 0.01;
	}
	if (GetKeyState(VK_OEM_MINUS) < 0)
	{
		RADIUS -= 0.01;
	}
	if (GetKeyState('W') < 0)
	{
		g_vStartPoint.x += 0.1;
	}
	if (GetKeyState('S') < 0)
	{
		g_vStartPoint.x -= 0.1;
	}
	if (GetKeyState('A') < 0)
	{
		g_vEndPoint.x += 0.1;
	}
	if (GetKeyState('D') < 0)
	{
		g_vEndPoint.x -= 0.1;
	}
	std::cout << (g_CurrentTime);
}


float dd = 0;
void Render()
{    //���
	//glRotatef(0, 0, 0, dd);
	//glRotated(dd, 0, 1, 0);


	//RenderScene();




	






	// glBindTexture(GL_TEXTURE_2D, texId);
	//glRotatef(0.5f,1.0f, 0.0f, 0.0f);
	glColor3d(0.1, 0.6, 0.1);
	//glNormal3d(0, 0, 1);
	
	
		//////////////////////////////////////////////////////
	//�������
	float normals[] = { 0,0,1, 0,0,1,    0,0,1,	  0,0,1,    0,0,1,     0,0,1,     0,0,1,     0,0,1, };
	//float normals[] = { -9,-3,2,    -6,-6,2,    -9,-10,2,   6,13,2,   8,17,2,   13,15,2,   11,10,2,   13,7,2 };
	float bn[] = { -9,-3,2,  -9,-3,0,  -6,-6,0,  -6,-6,2, };
	float bn1[] = { -6,6,2,  -6,6,0, -9,10,0, -9,10,2, };
	float bn2[] = { -9,10,2, -9,10,0,  6,-13,0,  6,-13,2, };
	float bn3[] = { -6,13,2,  -6,13,0, -8,17,0, -8,17,2, };
	float bn4[] = { 8,17,2, 8,17,0, -13,15,0, -13,15,2, };
	float bn5[] = { 13,-15,2, 13,-15,0, 11,10,0, 11,10,2, };
	float bn6[] = { 11,10,2, 11,10,0, -13,7,0, -13,7,2 };
	float bn7[] = { 13,-7,2, 13,-7,0, 9,-3,0,9,-3,2, };


	float normals1[] = { 9,3,-1, 6,6,-1,  9,10,-1, };
	float normals2[] = { 9,10,0,   6,13,0,   8,17,0, };
	float normals3[] = { 8,17,3,   13,15,3,   11,10,3, };
	float normals4[] = { 11,10,3,   13,7,3 , 9,3,3, };
	float normals5[] = { 9,3,3,    9,10,3,    11,10,3, };
	float normals6[] = { 11,10,3,   8,17,3,  9,10,3 };
		//��� ������
		float top[] = { 9,3,2,    6,6,2,    9,10,2,   6,13,2,   8,17,2,   13,15,2,   11,10,2,   13,7,2 };
		float top1[] = { 9,10,2,   6,13,2,   8,17,2, };
		float top2[] = { 8,17,2,   13,15,2,   11,10,2, };
		float top3[] = { 11,10,2,   13,7,2 , 9,3,2, };
		float top4[] = { 9,3,2,    9,10,2,    11,10,2, };
		float top5[] = { 11,10,2,   8,17,2,  9,10,2 };
		//��� ���
		float bottom[] = { 9,3,0,    6,6,0,    9,10,0,   6,13,0,   8,17,0,   13,15,0,   11,10,0,   13,7,0 };
		float bottom1[] = { 9,10,0,   6,13,0,   8,17,0, };
		float bottom2[] = { 8,17,0,   13,15,0,   11,10,0, };
		float bottom3[] = { 11,10,0,   13,7,0 , 9,3,0, };
		float bottom4[] = { 9,3,0,    9,10,0,    11,10,0, };
		float bottom5[] = { 9,10,0,   8,17,0,   11,10,0, };
		//��� ������
		float border[] = {9,3,2,  9,3,0,  6,6,0,  6,6,2, };
		float border1[] = {6,6,2,  6,6,0, 9,10,0, 9,10,2, };
		float border2[] = {9,10,2, 9,10,0,  6,13,0,  6,13,2, };
		float border4[] = {6,13,2,  6,13,0, 8,17,0, 8,17,2, };
		float border5[] = {8,17,2, 8,17,0, 13,15,0, 13,15,2, };
		float border6[] = {13,15,2, 13,15,0, 11,10,0, 11,10,2, };
		float border7[] = {11,10,2, 11,10,0, 13,7,0, 13,7,2 };
		float border8[] = {13,7,2, 13,7,0, 9,3,0,9,3,2, };

		//������ ������
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom1);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom2);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom3);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom4);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &bottom5);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
		glPopMatrix();


		//������ ���
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top1);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top2);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

		glPopMatrix();
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top3);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top4);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &top5);
		glNormalPointer(GL_FLOAT, 0, &normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glPopMatrix();

		//������ ����
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border);
		glNormalPointer(GL_FLOAT, 0, &bn);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();

		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border1);
		glNormalPointer(GL_FLOAT, 0, &bn1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border2);
		glNormalPointer(GL_FLOAT, 0, &bn2);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border4);
		glNormalPointer(GL_FLOAT, 0, &bn3);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border5);
		glNormalPointer(GL_FLOAT, 0, &bn4);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border6);
		glNormalPointer(GL_FLOAT, 0, &bn5);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border7);
		glNormalPointer(GL_FLOAT, 0, &bn6);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
		glEnableClientState(GL_NORMAL_ARRAY);
		glPushMatrix();
		//glNormal3f(0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, &border8);
		glNormalPointer(GL_FLOAT, 0, &bn7);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
		glPushMatrix();
		

	////////////////////////////
	double A[] = { 8, 3, 2 };
	double B[] = { 8, 7, 2 };
	double C[] = { 6,7, 2 };

	double B1[] = { 8, 3, 2 };
	double A1[] = { 8, 5, 2 };
	double C1[] = { 13, 5, 2 };

	double A2[] = { 6, 7, 2 };
	double C2[] = { 9, 7, 2 };
	double B2[] = { 9, 10, 2 };

	double A3[] = { 13, 5, 2 };
	double C3[] = { 11, 5, 2 };
	double B3[] = { 11, 10, 2 };

	double A4[] = { 11, 10, 2 };
	double C4[] = { 11, 13, 2 };
	double B4[] = { 14, 13, 2 };

	double A5[] = { 14, 13, 2 };
	double C5[] = { 9, 13, 2 };
	double B5[] = { 9, 17, 2 };

	double A6[] = { 9, 17, 2 };
	double B6[] = { 6, 14, 2 };
	double C6[] = { 9, 14, 2 };

	double A7[] = { 9, 14, 2 };
	double B7[] = { 6, 14, 2 };
	double C7[] = { 9, 10, 2 };
	/*glEnableClientState(GL_VERTEX_ARRAY);//��������� �������� �� ������� ������
	glEnableClientState(GL_NORMAL_ARRAY);
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top);
	glNormalPointer(GL_FLOAT,0,&normals);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
	glPopMatrix();
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top1);
	glNormalPointer(GL_FLOAT, 0, &normals1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
	
	glPopMatrix();
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top2);
	glNormalPointer(GL_FLOAT, 0, &normals2);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glPopMatrix();
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top3);
	glNormalPointer(GL_FLOAT, 0, &normals2);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top4);
	glNormalPointer(GL_FLOAT, 0, &normals);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
	glPushMatrix();
	//glNormal3f(0, 0, 1);
	glVertexPointer(3, GL_FLOAT, 0, &top5);
	glNormalPointer(GL_FLOAT, 0, &normals1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glPopMatrix();
	
	//glDrawArrays(GL_TRIANGLES,0, 8);
	glPopMatrix();
	/*

	glBegin(GL_TRIANGLES);
	//glColor3d(0, 1, 0);
	//glNormal3b(4, 2, 2);
	
	glVertex3dv(A);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C);
	//glNormal3f(0, 0, 1);
	glVertex3dv(A1);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B1);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C1);
	//glNormal3f(0, 0, 1);
	glVertex3dv(A2);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B2);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C2);
	//glNormal3f(0, 0, 1);

	glVertex3dv(A3);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B3);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C3);
	//glNormal3f(0, 0, 1);

	glVertex3dv(A4);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B4);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C4);
	//glNormal3f(0, 0, 1);

	glVertex3dv(A5);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B5);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C5);
	//glNormal3f(0, 0, 1);

	glVertex3dv(A6);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B6);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C6);
	//glNormal3f(0, 0, 1);

	glVertex3dv(A7);
	//glNormal3f(0, 0, 1);
	glVertex3dv(B7);
	//glNormal3f(0, 0, 1);
	glVertex3dv(C7);
	//glNormal3f(0, 0, 1);

	glEnd();
	double A8[] = { 9, 14, 2 };
	double B8[] = { 11, 14, 2 };
	double C8[] = { 11, 5, 2 };
	double D8[] = { 9, 5, 2 };

	double A9[] = { 8, 5, 2 };
	double B9[] = { 9, 5, 2 };
	double C9[] = { 9, 7, 2 };
	double D9[] = { 8, 7, 2 };
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3dv(A8);
	glVertex3dv(B8);
	glVertex3dv(C8);
	glVertex3dv(D8);


	glVertex3dv(A9);
	glVertex3dv(B9);
	glVertex3dv(C9);
	glVertex3dv(D9);
	glEnd();

	/////////////////

	double A_[] = { 8, 3, 0 };
	double B_[] = { 8, 7, 0 };
	double C_[] = { 6,7,  0 };

	double B1_[] = { 8, 3, 0 };
	double A1_[] = { 8, 5, 0 };
	double C1_[] = { 13, 5, 0 };

	double A2_[] = { 6, 7, 0 };
	double C2_[] = { 9, 7, 0 };
	double B2_[] = { 9, 10, 0 };

	double A3_[] = { 13, 5, 0 };
	double C3_[] = { 11, 5, 0 };
	double B3_[] = { 11, 10, 0 };

	double A4_[] = { 11, 10, 0 };
	double C4_[] = { 11, 13, 0 };
	double B4_[] = { 14, 13, 0 };

	double A5_[] = { 14, 13, 0 };
	double C5_[] = { 9, 13, 0 };
	double B5_[] = { 9, 17, 0 };

	double A6_[] = { 9, 17, 0 };
	double B6_[] = { 6, 14, 0 };
	double C6_[] = { 9, 14, 0 };

	double A7_[] = { 9, 14, 0 };
	double B7_[] = { 6, 14, 0 };
	double C7_[] = { 9, 10, 0 };



	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glNormal3f(0, 0, 1);
	glVertex3dv(A_);
	glVertex3dv(B_);
	glVertex3dv(C_);

	glVertex3dv(A1_);
	glVertex3dv(B1_);
	glVertex3dv(C1_);

	glVertex3dv(A2_);
	glVertex3dv(B2_);
	glVertex3dv(C2_);

	glVertex3dv(A3_);
	glVertex3dv(B3_);
	glVertex3dv(C3_);

	glVertex3dv(A4_);
	glVertex3dv(B4_);
	glVertex3dv(C4_);

	glVertex3dv(A5_);
	glVertex3dv(B5_);
	glVertex3dv(C5_);

	glVertex3dv(A6_);
	glVertex3dv(B6_);
	glVertex3dv(C6_);

	glVertex3dv(A7_);
	glVertex3dv(B7_);
	glVertex3dv(C7_);

	glEnd();
	double A8_[] = { 9, 14, 0 };
	double B8_[] = { 11, 14, 0 };
	double C8_[] = { 11, 5, 0 };
	double D8_[] = { 9, 5, 0 };

	double A9_[] = { 8, 5, 0 };
	double B9_[] = { 9, 5, 0 };
	double C9_[] = { 9, 7, 0 };
	double D9_[] = { 8, 7, 0 };
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glNormal3f(0, 0, 1);
	glVertex3dv(A8_);
	glNormal3f(0, 0, 1);
	glVertex3dv(B8_);
	glNormal3f(0, 0, 1);
	glVertex3dv(C8_);
	glNormal3f(0, 0, 1);
	glVertex3dv(D8_);

	glNormal3f(0, 0, 1);
	glVertex3dv(A9_);
	glNormal3f(0, 0, 1);
	glVertex3dv(B9_);
	glNormal3f(0, 0, 1);
	glVertex3dv(C9_);
	glNormal3f(0, 0, 1);
	glVertex3dv(D9_);
	glEnd();

	double A1v[] = { 8, 3, 0 };
	double B1v[] = { 8, 3, 2 };
	double C1v[] = { 6, 7, 2 };
	double D1v[] = { 6, 7, 0 };

	double A2v[] = { 9, 10, 0 };
	double B2v[] = { 9, 10, 2 };
	double C2v[] = { 6, 7, 2 };
	double D2v[] = { 6, 7, 0 };

	double A3v[] = { 9, 10, 0 };
	double B3v[] = { 9, 10, 2 };
	double C3v[] = { 6, 14, 2 };
	double D3v[] = { 6, 14, 0 };

	double A4v[] = { 9, 17, 0 };
	double B4v[] = { 9, 17, 2 };
	double C4v[] = { 6, 14, 2 };
	double D4v[] = { 6, 14, 0 };

	double A5v[] = { 8, 3, 0 };
	double B5v[] = { 8, 3, 2 };
	double C5v[] = { 13, 5, 2 };
	double D5v[] = { 13, 5, 0 };

	double A6v[] = { 11, 10, 0 };
	double B6v[] = { 11, 10, 2 };
	double C6v[] = { 13, 5, 2 };
	double D6v[] = { 13, 5, 0 };

	double A7v[] = { 11, 10, 0 };
	double B7v[] = { 11, 10, 2 };
	double C7v[] = { 14, 13, 2 };
	double D7v[] = { 14, 13, 0 };

	double C8v[] = { 9, 17, 0 };
	double C18v[] = { 13, 17, 2 };
	double C81v[] = { 9, 17, 2 };
	double C181v[] = { 13, 17, 2 };

	glBegin(GL_QUADS);
	glColor3d(0, 0, 1);
	glNormal3f(0, 0, 1);
	glVertex3dv(A1v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B1v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C1v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D1v);
	glNormal3f(0, 0, 1);
	glVertex3dv(A2v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B2v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C2v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D2v);

	glNormal3f(0, 0, 1);
	glVertex3dv(A3v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B3v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C3v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D3v);
	glNormal3f(0, 0, 1);
	glNormal3f(0, 0, 1);
	glVertex3dv(A4v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B4v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C4v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D4v);
	glNormal3f(0, 0, 1);
	glVertex3dv(A5v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B5v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C5v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D5v);
	glNormal3f(0, 0, 1);
	glVertex3dv(A6v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B6v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C6v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D6v);
	glNormal3f(0, 0, 1);
	glVertex3dv(A7v);
	glNormal3f(0, 0, 1);
	glVertex3dv(B7v);
	glNormal3f(0, 0, 1);
	glVertex3dv(C7v);
	glNormal3f(0, 0, 1);
	glVertex3dv(D7v);



	glEnd();

	//glBegin(GL_TRIANGLES);
	round12(0.2, D7v, C8v, C18v);
	//round(0.2, C7v, C81v, C181v);*/
dd += 0.5;
}



//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 1);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	


	

	
	RenderScene();
	//Render();

	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	//ss << "T - ���/���� �������" << std::endl;
	//ss << "L - ���/���� ���������" << std::endl;
	//ss << "F - ���� �� ������" << std::endl;
	// << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "VOVAS -> on / off �������� - \n� ����������� TY" << std::endl;
	ss << "VOVAS -> WS ��� ��������� ��������� ����� AD ��������" << std::endl;
	ss << "VOVAS -> g_CurrentTime ��� � ��� ���" << g_CurrentTime << std::endl;
	ss << "VOVAS -> c�������� ��� �������� ������\n � �������� ����� + � - ��� ��� ��������) " << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}