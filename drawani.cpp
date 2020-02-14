#define NDEBUG 1

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>
#include <unistd.h>
#include "common.hpp"

#define pi M_PI


#define mod_date 20090226


using namespace std;

//int sgn(double x) { return x < 0?-1:1; }

double lastx, lasty, inx, iny;

#define SNAKE_SIZE 18

#define SNAKE_SIZE_2 13

double snakex[SNAKE_SIZE+1];
double snakey[SNAKE_SIZE+1];


vector<SCoord> sensorm[SNAKE_SIZE+5];

void drawbrain();

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawbrain();
    glutSwapBuffers();
	usleep(15000);
	glutPostRedisplay();
}



void CompileList(vector<pair<SBox, string> >& boxes)
{
	
	glNewList(123, GL_COMPILE);
	
	for (vector<pair<SBox,string> >::iterator it = boxes.begin();
		it != boxes.end(); ++it)
	{
		if (it->second == "box")
			glColor3f(1.0,1.0,1.0);
		if (it->second == "start")
			glColor3f(1.0,0.5,0.0);
		if (it->second == "end")
			glColor3f(0.0,1.0,0.5);
		if (it->second == "size")
			glColor3f(0.0,0.1,0.2);
		glBegin(GL_POLYGON);
		//else
		//glBegin(GL_LINES);
			SBox& b = it->first;
			glVertex2f(b.sta.x, b.sta.y);
			glVertex2f(b.sta.x, b.end.y);
			glVertex2f(b.sta.x, b.end.y);
			glVertex2f(b.end.x, b.end.y);
			glVertex2f(b.end.x, b.end.y);
			glVertex2f(b.end.x, b.sta.y);
			glVertex2f(b.end.x, b.sta.y);
			glVertex2f(b.sta.x, b.sta.y);
		glEnd();
	}
	glEndList();

}

void drawbrain()
{
	glCallList(123);
	double xr = lastx, yr = lasty;
	string crap;
	while (cin >> crap)
	{
		//cout << crap << endl;
		if (crap == "SN") {
			sensorm[SNAKE_SIZE].clear();
			double xsl, ysl;
			unsigned w_t_f = 0;
			while (cin >> xsl >> ysl >> crap)
			{
				++w_t_f;
				sensorm[SNAKE_SIZE].push_back(SCoord(xsl,ysl));
				if (crap != "SN") 
				{
					for (unsigned is = 0; is < SNAKE_SIZE; ++is)
					{
						//shift sensors.
						if (sensorm[is].size() != sensorm[is+1].size())
						{
							sensorm[is].resize(sensorm[is+1].size());
							for (unsigned isk = 0; isk < sensorm[is+1].size(); ++isk)
								sensorm[is][isk] = sensorm[is+1][isk];
						}
						else
						{
							for (unsigned isk = 0; isk < sensorm[is+1].size(); ++isk)
								sensorm[is][isk] = (sensorm[is][isk] + sensorm[is+1][isk]) / 2.0;
							
						}
					}
					break;
				}
			}
		}
		if (crap == "AT") break;
	}
		
		if (cin >> xr >> yr) { }
		else { xr = lastx; yr = lasty; }

	//cout << xr << yr << endl;
		//double inx = (xr + lastx) / 2.0; 
	//double iny = (yr + lasty) / 2.0;//(xr + lasty)/2.0;
	snakex[SNAKE_SIZE] = xr;
	snakey[SNAKE_SIZE] = yr;
	glColor3f(0.8,0.9,1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		for (unsigned i = 0; i < SNAKE_SIZE; ++i)
		{
			if (i < SNAKE_SIZE_2)
			{
				glVertex2f(snakex[i], snakey[i]);
				glVertex2f(snakex[i+1], snakey[i+1]);
			}
			if (i == SNAKE_SIZE_2)
			{
				glColor3f(0.2,0.5,0.6);
				glLineWidth(1.0);
				for (unsigned isx = 0; isx < sensorm[i].size(); ++isx)
				{
					glVertex2f(snakex[i],snakey[i]);
					glVertex2f(sensorm[i][isx].x,sensorm[i][isx].y);
				}
				glColor3f(0.8,0.9,1.0);
				glLineWidth(2.0);
			}
			snakex[i] = (snakex[i+1] + snakex[i]) / 2.0;// + lolx;
			snakey[i] = (snakey[i+1] + snakey[i]) / 2.0;// + loly;
		}
		/*
		glVertex2f(xr -0.5, yr +0.5);
		glVertex2f(xr +0.5, yr -0.5);
		glVertex2f(xr -0.5, yr -0.5);
		glVertex2f(xr +0.5, yr +0.5);*/
	glEnd();
	glColor3f(0.5,0.9,1.0);
	glPointSize(7.0);
	glBegin(GL_POLYGON);
		double gxr = snakex[SNAKE_SIZE_2];
		double gyr = snakey[SNAKE_SIZE_2];
		glVertex2f(gxr-0.5, gyr-0.5);
		glVertex2f(gxr-0.5, gyr+0.5);
		glVertex2f(gxr+0.5, gyr+0.5);
		glVertex2f(gxr+0.5, gyr-0.5);
	glEnd();
	inx = lastx;
	iny = lasty;
	lastx = xr;
	lasty = yr;
	//if (snakex[0] == xr && snakey[0] == xr) { exit(0); }
	//srand(time(0));
}


int main(int argc, char* argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(750, 750);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Robot's Animation");
    glutDisplayFunc(display);
    glClearColor (0, 0, 0.1, 0.1);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
   	//glOrtho (0, XSize, YSize, 0, 0, 1)
   	//glMatrixMode (GL_MODELVIEW)
	
	string data = "data.txt";
	if (argc > 1) data = argv[1];
	vector<string> v;
	cout << "Reading data..." << endl;
	ifstream f(data.c_str());
	vector<pair<SBox, string> > boxes;
	{
		while (f)
		{
			string param;
			f >> param;
			if (param == "box")
			{
				double x1, y1, x2, y2;
				f >> x1 >> y1 >> x2 >> y2;
				SBox b(x1, y1, x2, y2);
				boxes.push_back(std::pair<SBox, string>(b, "box"));
			}
			if (param == "size")
			{
				double xs, ys;
				f >> xs >> ys;
				gluOrtho2D(-1, xs+1, -1, ys+1);
				SBox b(0, 0, xs, ys);
				boxes.push_back(std::pair<SBox, string>(b,"size"));
			}
			if (param == "end")
			{
				double xe, ye;
				f >> xe >> ye;
				SBox b(xe-0.7, ye-0.7, xe+0.7, ye+0.7);
				boxes.push_back(std::pair<SBox, string>(b,"end"));
			}
			if (param == "start")
			{
				f >> lastx >> lasty;
				for (unsigned i = 0; i <= SNAKE_SIZE; ++i)
				{
					snakex[i] = lastx;
					snakey[i] = lasty;
				}
				boxes.push_back(std::pair<SBox, string>(SBox(lastx-0.4,lasty-0.4,lastx+0.4,lasty+0.4),"start"));
			}
		}
		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_POINT_SMOOTH | GL_LINE_SMOOTH);
		glEnable(GL_MULTISAMPLE);
		cout << "Compiling GL list..." << endl;
		CompileList(boxes);
	}
	cout << "Run animation." << endl;

		
    glutMainLoop();
    return 0;
}

