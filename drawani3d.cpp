#define NDEBUG 1

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>
#include "common.hpp"
#define pi M_PI


#define mod_date 20090216


#define IN_H 0.0
#define BOX_H 2.0

#define FPS 60.0

unsigned fcnt = 0;

double fps_fps = 80.0;


double act_fps = FPS;

using namespace std;

double LastTick;
/*
double GetTickCount()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}
*/


double lastx, lasty, inx, iny;

#define SNAKE_SIZE 22

double snakex[SNAKE_SIZE+1];
double snakey[SNAKE_SIZE+1];

SCoord size;
SCoord end_where;

vector<SCoord> sensorm[SNAKE_SIZE+1];

void drawbrain();

void DrawBox(const SBox& b, const double& hh, const double& ss);

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		SCoord Cf(snakex[0], snakey[0]); 
		SCoord Ch (snakex[SNAKE_SIZE/2], snakey[SNAKE_SIZE/2]);
		SCoord DeltaC = Ch - Cf;
		Cf = Cf - DeltaC;

		// Camera
		gluLookAt(Cf.x-0.001, Cf.y-0.001, IN_H+7.0, 
				Ch.x, Ch.y, IN_H+2.5, 
				0.0, 0.0, 1.0);
		
		// Light - sense of smell.
		SCoord LightDelta = (end_where - Ch)/10.0;
		SCoord LightWhere = Ch + LightDelta;

		GLfloat lightparamxm[4] = {LightWhere.x, LightWhere.y, 10.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightparamxm);

		// Action
		drawbrain();

		// Exit point
		if (absc(end_where - Cf) < 1.0) exit(-1);
	glutSwapBuffers();
	
	glutPostRedisplay();

	
	double fps_measurement = GetTickCount();
	fps_fps = 0.5*fps_fps + 0.5 * (1000.0 / (fps_measurement - LastTick));
	
	if (act_fps > FPS)
		usleep(1000 * (1 + abs(1000.0/FPS - 1000.0/fps_fps)));

	act_fps = 0.8*act_fps + 0.2*1000.0/(GetTickCount() - LastTick);
	++fcnt;
	if (fcnt > FPS*3.0)
	{
		cout << "FPS :" << act_fps << endl;
		fcnt = 0;
	}
	LastTick = GetTickCount();
	

	//usleep(1000);
}


void DrawBox(const SBox& b, const double& hh, const double& ss)
{
	glPushMatrix();
		SCoord mid = (b.sta + b.end) / 2.0;
		SCoord ssize = (b.end - b.sta);
		glTranslatef(mid.x, mid.y, ss);
		glScalef(ssize.x,ssize.y,hh);
		glutSolidCube(1.0);
	glPopMatrix();
}

void DrawSlicedBox(const SBox& box, double hh, double ss, double xslice = size.x / 10.1, double yslice = size.y / 10.1)
{
	//DrawBox(box, hh, ss); return;
	for (double tslx = box.sta.x; tslx <= box.end.x; tslx+=xslice)
			for (double tsly = box.sta.y; tsly <= box.end.y; tsly+=yslice)
						DrawBox(SBox(tslx, tsly, min(box.end.x,tslx+xslice), min(box.end.y,tsly+yslice)),hh,ss);

}


void CompileList(vector<pair<SBox, string> >& boxes)
{
	
	glNewList(123, GL_COMPILE);
	
	for (vector<pair<SBox,string> >::iterator it = boxes.begin();
		it != boxes.end(); ++it)
	{
		double hh = BOX_H;
		double where = IN_H;
		if (it->second == "box")
			glColor4f(0.95,0.95,0.95, 1.0);
		if (it->second == "start")
			glColor4f(1.0,0.8,0.05, 0.5);
		if (it->second == "end")
			glColor4f(0.05,1.0,0.8, 0.5);
		if (it->second == "size")
		{
			glColor4f(0.0,0.05,0.15, 1.0);
			where = IN_H - hh;
		}
		//if (it->second == "size")
		//{	
		//	DrawBox(it->first, hh, where);
		//}
		//else {
			DrawSlicedBox(it->first, hh, where); //+uniform_deviate(rand())*0.1, where);
		//}
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

	snakex[SNAKE_SIZE] = xr;
	snakey[SNAKE_SIZE] = yr;
	glColor4f(0.3,0.6,0.9, 0.9);
	glLineWidth(2.0);
	glBegin(GL_LINES);	
		for (unsigned i = 0; i < SNAKE_SIZE; ++i)
		{
			if (i < SNAKE_SIZE/2)
			{
				glVertex3f(snakex[i], snakey[i], IN_H);
				glVertex3f(snakex[i+1], snakey[i+1], IN_H);
			}
			if (i-1 == SNAKE_SIZE/2)
			{
				glColor4f(0.6,0.8,1.0, 1.0);
				glLineWidth(1.0);
				for (unsigned isx = 0; isx < sensorm[i].size(); ++isx)
				{
					glVertex3f(snakex[i],snakey[i], IN_H);
					glVertex3f(sensorm[i][isx].x,sensorm[i][isx].y, IN_H);
				}
				glColor4f(0.3,0.6,0.9,0.9);
				glLineWidth(2.0);
			}
			snakex[i] = (snakex[i+1] + snakex[i]) / 2.0;// + lolx;
			snakey[i] = (snakey[i+1] + snakey[i]) / 2.0;// + loly;
		}
	glEnd();
	glColor4f(0.3,0.5,0.7,1.0);
	double gxr = snakex[SNAKE_SIZE/2];
	double gyr = snakey[SNAKE_SIZE/2];
	DrawBox(SBox(gxr-0.45, gyr-0.45, gxr+0.45, gyr+0.45), 0.9, IN_H);
	lastx = xr;
	lasty = yr;
}


int main(int argc, char* argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(750, 750);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Robot's Animation");
    glutDisplayFunc(display);
    glClearColor (0, 0, 0.0, 0.0);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
	gluPerspective(60, 1, 1.5, 1000.0);;
	
	string data = "data.txt";
	if (argc > 1) data = argv[1];
	vector<string> v;
	cout << "Reading data..." << endl;
	ifstream f(data.c_str());
	vector<pair<SBox, string> > boxes;
	double enverr = 0.2;
	// Data readout.
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
				size.x = xs;
				size.y = ys;
				glMatrixMode(GL_MODELVIEW); glLoadIdentity();
				gluLookAt(xs/2.1, ys/2.1, 50, xs/2.0, ys/2.0, -IN_H, 0.0, 1.0, 0.0);
							
				SBox b(0, 0, xs, ys);
				
				//double tslstep = (xs+ys) / 10.0;
				//for (double tslx = 0.0; tslx <= xs - tslstep; tslx+=tslstep)
				//	for (double tsly = 0.0; tsly <= ys - tslstep; tsly+=tslstep)
				//		boxes.push_back(std::pair<SBox, string>(
				//			SBox(tslx, tsly, min(xs,tslx+tslstep), min(tsly+tslstep,ys)),"size"));
				
				boxes.push_back(std::pair<SBox, string>(b, "size"));
				boxes.push_back(std::pair<SBox, string>(SBox(-0.5,-0.5,xs,0.5), "box"));
				boxes.push_back(std::pair<SBox, string>(SBox(-0.5,-0.5,0.5,ys), "box"));
				boxes.push_back(std::pair<SBox, string>(SBox(xs,-0.5,xs+0.5,ys+0.5), "box"));
				boxes.push_back(std::pair<SBox, string>(SBox(-0.5,ys,xs+0.5,ys+0.5), "box"));
			}
			if (param == "end")
			{
				double xe, ye;
				f >> xe >> ye;
				end_where = SCoord(xe,ye);
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
			if (param == "enverr")
				f >> enverr;
		}
		glMatrixMode(GL_MODELVIEW);
		CompileList(boxes);
		glEnable(GL_NORMALIZE);
		glEnable(GL_POINT_SMOOTH | GL_LINE_SMOOTH);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LIGHTING);
		glEnable (GL_DEPTH_TEST);

		// Light parameters
		glColorMaterial ( GL_FRONT, GL_SPECULAR); //GL_AMBIENT_AND_DIFFUSE ) ;
		glDisable(GL_LIGHT0);
		glEnable ( GL_COLOR_MATERIAL ) ;
		GLfloat lightparam[4] = {1.0, 1.0, 1.0, 1.0};
		GLfloat lightamb[4] = {0.2, 0.25, 0.3, 1.0};
		GLfloat lightspec[4] = {0.9, 0.9, 0.9, 0.5};
		glLightfv(GL_LIGHT0, GL_AMBIENT,  lightamb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightparam);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,  0.005f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,  0.001f);
		
	GLfloat lightparamx[4] = {size.x/2.0, size.y/2.0, 5.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightparamx);
		glEnable(GL_LIGHT0);
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);

		glMaterialfv(GL_FRONT, GL_SPECULAR, lightparam);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lightamb);
	

		glCullFace( GL_BACK );

		glEnable( GL_CULL_FACE );

		// Fog parameters
		//GLuint filter;						// Which Filter To Use
		GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// Storage For Three Types Of Fog
		GLuint fogfilter= 0;					// Which Fog To Use
		GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 0.8f};		// Fog Color
		glFogi(GL_FOG_MODE, fogMode[fogfilter]);		// Fog Mode
		glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
		glFogf(GL_FOG_DENSITY, 0.1*enverr*enverr); // How Dense Will The Fog Be
		glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
		glFogf(GL_FOG_START, 1.0f);				// Fog Start Depth
		glFogf(GL_FOG_END, 100.0f);				// Fog End Depth
		glEnable(GL_FOG);					// Enables GL_FOG

		
	}
	LastTick = GetTickCount();
	cout << "Run animation." << endl;		
    glutMainLoop();
    return 0;
}


