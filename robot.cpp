#include "common.hpp"
#include <cmath>

using namespace std;

#define SENS_STEP 0.5
#define BOX_INFL 0.1
#define BOX_EG_INFL 0.1
#define OBS_FACT 1.15

#define ANG_FACT 300


enum Direction { dUp = 90, dDown = 270, dLeft = 180, dRight = 0 };

SCoord decisionx(const SCoord& po, const double obst_factor = OBS_FACT)
{
	double x_factor = po.x / po.y;
	if (abs(x_factor) > obst_factor)
	{
		return SCoord(po.x, - po.x);
	}
	else if (abs(x_factor) < 1/obst_factor)
	{
		return SCoord(po.y, po.y);
	}
	else
	{
		return SCoord(po.x, 0.0);
	}
}

SCoord decisiony(const SCoord& po, const double& obst_factor = OBS_FACT)
{
	
	double y_factor = po.y / po.x;
	if (abs(y_factor) > obst_factor)
	{
		return SCoord(-po.y, po.y);
	}
	else if (abs(y_factor) < 1/obst_factor)
	{
		return SCoord(po.x , po.x);
	}
	else
	{
		return SCoord(0.0  , po.y);
	}
}

SCoord decisionz(const SCoord& c, const double& obst_factor = OBS_FACT)
{
	double x_factor = abs(c.x) / (abs(c.y) + 0.00001);
	if (x_factor > obst_factor)
	{
		return SCoord(c.x, 0.0);
	}
	else if (x_factor < 1 / obst_factor)
	{
		return SCoord(0.0, c.y);
	}
	else
	{
		return c;
	}
}
// space / kolku_ni_treba

SCoord decider3(const SCoord& c1, const SCoord& c2, const SCoord& c3, const SCoord& po)
{
	if (getDirection(po,4) == getDirection(decisionz(c1)) && absc(po) < absc(c1)) return c1;
	if (getDirection(po,4) == getDirection(decisionz(c2)) && absc(po) < absc(c2)) return c2;
	if (getDirection(po,4) == getDirection(decisionz(c3)) && absc(po) < absc(c3)) return c3;
	
	unsigned obsfac = ANG_FACT;
	double ang1 = getDelta(po,c1,36) + obsfac;
	double ang2 = getDelta(po,c2,36) + obsfac;
	double ang3 = getDelta(po,c3,36) + obsfac;

	return (absc(c1)/ang1 > absc(c2)/ang2
				? (absc(c1)/ang1 > absc(c3)/ang3 ? c1 : c3)
				: (absc(c2)/ang2 > absc(c3)/ang3 ? c2 : c3));
				
}



bool overlap(const vector<SCoord>& sens, const SCoord dz, int c = 90)
{
	double k = 0.0;
	for (unsigned i = 0; i < sens.size(); ++i)
	{
		k += getDelta(sens[i], dz);			
	}
	k /= 3.0;
	if (k >= c) 
	{		
		cout << "	SNS OK " << k << endl;
		return false;
	}
	else {
		cout << "	SNS OV " << k << endl;
		return true;
	}
}


void print_sens(const vector<SCoord>& sens)
{
	unsigned i = 0;
	i=1;cout << "S" << i << " " << sens[i] << endl;
	i=0;cout << "S" << i << " " << sens[i] << endl;
	i=2;cout << "S" << i << " " << sens[i] << endl;
}

int main(int argc, char** argv)
{
	srand(time(0));
	vector<SBox> boxes;
	SCoord start;
	SCoord where;
	SCoord end;
	SCoord size;
	string data_f = "data.txt";
	if (argc > 1) data_f = argv[1];
	ifstream f(data_f.c_str());
	string cmd;
	double steps = 2.0;
	while (f >> cmd)
	{
		if (cmd == "size") { f >> size.x >> size.y; }
		if (cmd == "start") { f >> start.x >> start.y; }
		if (cmd == "end") { f >> end.x >> end.y; }
		if (cmd == "box")
		{
			SBox box;
			f >> box.sta.x >> box.sta.y >> box.end.x >> box.end.y;
			box.sta.x -=BOX_INFL;
			box.sta.y -=BOX_INFL;
			box.end.x +=BOX_INFL;
			box.end.y +=BOX_INFL;
			boxes.push_back(box);
			
			//boxes.push_back(minibox(box.sta,BOX_EG_INFL));
			//boxes.push_back(minibox(box.end,BOX_EG_INFL));
			//boxes.push_back(minibox(SCoord(box.sta.x, box.end.y),BOX_EG_INFL));
			//boxes.push_back(minibox(SCoord(box.end.x, box.sta.y),BOX_EG_INFL));
			
		}
		if (cmd == "step")
		{
			f >> steps;
		}
	}
	steps = 0.35;
	where = start;// + SCoord(SENS_STEP,SENS_STEP);
	//end = end + SCoord(SENS_STEP,SENS_STEP);
	SCoord olddelta(1.0, 0.0);
	SCoord delta;

	vector<SCoord> sensors;
	delta = end - where;
	sensors.push_back(sgnc(olddelta,1.0)); // sensor z
	sensors.push_back(sgnc(decisionx(olddelta),1.0)); // sensor y
	sensors.push_back(sgnc(decisiony(olddelta),1.0)); 
	olddelta = delta;	
	//sensors.push_back(SCoord(1.0, 1.0));
	//sensors.push_back(SCoord(1.0, -1.0));

	while (! (where == end))
	{
		
		const double min_delta = steps * (0.9 + (0.2 * uniform_deviate(rand())));
		
		sensors[1] = sgnc(decisionz(olddelta), min_delta);			
		sensors[0] = sgnc(decisionx(sensors[1],OBS_FACT),min_delta);
		sensors[2] = sgnc(decisiony(sensors[1],OBS_FACT),min_delta);
		
		delta = end - where;
		SCoord po = delta;
		cout << "OD " << olddelta << endl;
		
		SCoord decision_z = sgnc(decisionz(delta), min_delta);
		SCoord decision_x = sgnc(decisionx(decision_z),min_delta);
		SCoord decision_y = sgnc(decisiony(decision_z),min_delta);

		delta = sgnc(delta, min_delta);
	
		print_sens(sensors);
		
		if (!overlap(sensors, po, 165))
		{
			sensors[1] = /*sgnc(decisionz(sensors[2]),min_delta); */ decision_z;
			sensors[0] = /*sgnc(decisionx(sensors[2]),min_delta); */ decision_x;
			sensors[2] = /*sgnc(decisiony(sensors[2]),min_delta); */ decision_y;// = sgnc(decisiony(delta, 1.5), min_delta);
		}
		
		cout << " DZ " << decision_z << endl;
		cout << " DX " << decision_x << endl;
		cout << " DY " << decision_y << endl;

		SCoord maxx(0.0,0.0);
		SCoord maxy(0.0,0.0);
		SCoord maxz(0.0,0.0);
		for (unsigned i = 0; i < sensors.size(); ++i)
		{
			SCoord deltaS = sgnc(sensors[i],SENS_STEP);
			sensors[i] = deltaS;
				//sensors[i].x = sgnc(ensors[i].y, min_delta / SENS_STEP);
			//deltaS = sensors[i];
			while (!InBox(where + sensors[i], boxes))
			{
				debug("BeamLoop");
				sensors[i] = sensors[i] + deltaS;
				if (
					(!((where + sensors[i]) < size)) ||
					(!((where + sensors[i]) > SCoord(0,0)))
					) { debug("BreakBeamLoop"); break; }
				//cout << sensors[i].x << " " << sensors[i].y << endl;
			}
			sensors[i] = sensors[i] - deltaS;		
			debug("SensorLoop");
			switch (i)
			{
				case 0: maxx = sensors[i]; break;
				case 1: maxz = sensors[i]; break;
				case 2: maxy = sensors[i]; break;
			}
			sensors[i] = deltaS;
		}
		//Maximums!
		cout << "SN " << where + maxz << endl;
		cout << "SN " << where + maxx << endl;
		cout << "SN " << where + maxy << endl;

		delta = sgnc(decider3(maxx, maxy, maxz, po),min_delta);
	
		cout << "D1 " << delta << endl;	

		
		if (absc(maxx) + absc(maxy) + absc(maxz) < 3*min_delta)
		{
			double coin = uniform_deviate(rand());
			if (coin >= 0.5)
			{
				delta = SCoord(0,0) - decision_y;
			}
			else 
			{
				delta = SCoord(0,0) - decision_x;
			}
			//delta = SCoord(uniform_deviate(rand())*1.0 - 0.5,uniform_deviate(rand())*1.0 - 0.5);
			/*
			if (getDirection(delta) == getDirection(maxx))
			{
				if (absc(maxx) < absc(delta))
				{
					delta = delta - maxx;
				}
			}
			if (getDirection(delta) == getDirection(maxy))
			{
				if (absc(maxy) < absc(delta))
				{
					delta = delta - maxy;
				}
			}
			*/
			
		}
	
		delta = sgncf(delta, min_delta);
		cout << " +DT " << delta << endl;	
		where = where + delta;
		if (pow(sqrt(absc(maxx)) + sqrt(absc(maxy)) + sqrt(absc(maxz)),2.0) > 30.0) {
			if (uniform_deviate(rand()) < 0.01)
			delta = decision_z; 
		}
		//boxes.push_back(SBox(where - SCoord(2 * delta.x, 2 * delta.y),where - delta));

		if (InBox(where, boxes)) { 
			cout << "EE !!!" << endl; 
			where = where - delta;
		}
		/*if ((!(where + sensors[i] < size)) ||
			(!(where + sensors[i] > SCoord(0,0)))
			)*/
		if (!SBox(0,0,size.x,size.y).InBox(where)) { 
			cout << "EE !" << endl; 
			where = where - delta;
		}

		debug("MoveLoop");
		olddelta = delta;	
		cout << "AT " << where << endl;

		cout << endl;
	}

	return 0;
}
