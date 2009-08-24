#include "common.hpp"
#include <cmath>

using namespace std;

#define BOX_INFL 0.0

#define BOUNCE_FAC 5.0

// This is x2
#define MAX_VIEWANGLE 45.0
// This is "forgiveness"/focal strength of angle.
//#define FOCUS_ANGLE 78
#define FOCUS_ANGLE 105
#define MAX_TURN_ANG 180

//#define FORGIVE_ANGLE 0.1


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
	SCoord size(100.0,100.0);
	string data_f = "data.txt";
	if (argc > 1) data_f = argv[1];
	ifstream f(data_f.c_str());
	string cmd;
	double gpserr = 0.5, enverr = 0.3, steperr = 0.4, steps = 0.35, angres = 5.0, angview = MAX_VIEWANGLE, angfoc = FOCUS_ANGLE, 
			viewfoc = FOCUS_ANGLE, maxturnang = MAX_TURN_ANG, bouncefac = BOUNCE_FAC;

	ifstream fs("sonar.conf");
	while (fs >> cmd)
	{
		if (cmd == "gpserr")
			fs >> gpserr;
		if (cmd == "enverr")
			fs >> enverr;
		if (cmd == "steperr")
			fs >> steperr;
		if (cmd == "step")
			fs >> steps;
		if (cmd == "angres")
			fs >> angres;
		if (cmd == "angview")
			fs >> angview;
		if (cmd == "angfoc")
			fs >> angfoc;
		if (cmd == "viewfoc")
			fs >> viewfoc;
		if (cmd == "maxturnang")
			fs >> maxturnang;
		if (cmd == "bouncefac")
			fs >> bouncefac;
	}
	while (f >> cmd)
	{
		if (cmd == "size") { 
			f >> size.x >> size.y; 
			boxes.push_back(SBox(0,0,size.x,0));
			boxes.push_back(SBox(0,0,0,size.y));
			boxes.push_back(SBox(0,size.y,size.x,size.y));
			boxes.push_back(SBox(size.x,0,size.x,size.y));
		}
		if (cmd == "start") { f >> start.x >> start.y; }
		if (cmd == "end") { f >> end.x >> end.y; }
		if (cmd == "box")
		{
			SBox box;
			f >> box.sta.x >> box.sta.y >> box.end.x >> box.end.y;
			box.sta.x -= BOX_INFL;
			box.sta.y -= BOX_INFL;
			box.end.x -= BOX_INFL;
			box.end.y -= BOX_INFL;
			boxes.push_back(box);
		}
		
		// environment conditions override
		if (cmd == "enverr")
		{	
			f >> enverr;
		}
		if (cmd == "gpserr")
		{
			f >> gpserr;
		}
		if (cmd == "steperr")
		{
			f >> steperr;
		}
	}
	cout << "ERRDATA ENVE(" << enverr << ") GPSE(" << gpserr << ") STPE (" << steperr << ")" << endl;

	where = start;// + SCoord(SENS_STEP,SENS_STEP);
	SCoord delta(1.0,1.0);// = end - where; //(1.0,1.0);

	vector<SCoord> sensors;
	
	while (! (end == where))
	{
		SCoord last_delta = delta;
		SCoord target = end - SCoord(where.x + gpserr*gaussian_deviate(), 
							  where.y + gpserr*gaussian_deviate());
		
		double current_max = 0.0;
		SCoord max_go(0.0,0.0);
		double max_angle = 0.0;

		double alphax = getDirection(last_delta,720);
		double alpha1 = alphax;
		double alpha2 = alphax;
		double targetPath = absc(target);

		double omega = 0.0;
		while (abs(alpha1 - alphax) < angview) // MAX_VIEWANGLE
		{
			vector<SCoord> sensor;
			sensor.push_back(SCoord(0,0));
			sensor.push_back(SCoord(0,0));
			for (unsigned j = 0; j < sensor.size(); ++j)
			{
				double alpha;
				if (j == 0)  
				{
					alpha = alpha1;
				}
				else 
				{
					alpha = alpha2;
					if (alpha1 == alpha2) break; // don't duplicate middle sensor.
				}

				double focus_ahead = exp(0.0 - (alpha-alphax)*(alpha-alphax) / (viewfoc*viewfoc));
				double w = steps * gaussian_error(steperr); // percentage error in step 
				SCoord deltaS(cos(alpha*M_PI/180.0) * w, sin(alpha*M_PI/180.0) * w);
				while (!InBox(where + sensor[j], boxes))
				{
					sensor[j] = sensor[j] + deltaS;
					if (
						(!((where + sensor[j]) < size)) ||
						(!((where + sensor[j]) > SCoord(0,0)))
						)  break; 
				}
				//sensor[j] = sensor[j] - deltaS;
				double serr = gaussian_error(enverr);
				sensor[j].x *= serr;
				sensor[j].y *= serr;
				cout << "SN " << (where + sensor[j]) << endl;
				double sensorPath = absc(sensor[j]);
				if (sensorPath > targetPath) sensorPath = targetPath;
				double deltaAngle = getDelta(sensor[j], target,360);
				double decider = focus_ahead * sensorPath * exp(0.0 - deltaAngle*deltaAngle / (angfoc*angfoc)); 
				if (decider > current_max)
				{
					current_max = decider;
					max_go = deltaS * 1.5;
					max_angle = omega;
					// New code added to limit turn angles.
					if (j != 0) max_angle = 0.0 - max_angle;
					if (abs(max_angle) > maxturnang)
					{
						max_angle = maxturnang * sgn(max_angle);
						double max_go_ang = alphax + max_angle;
						max_go = SCoord(cos(max_go_ang*M_PI/180.0) * w, sin(max_go_ang*M_PI/180.0) * w) * 1.5;
					}
					//end of new code.
				}
			}
			omega += angres; // turning "wheels"
			alpha1 += angres;
			alpha2 -= angres;
		
		} // end of sensor process

		//delta = sgncf(delta, min_delta);
		delta = max_go;
		cout << " +TR " << target << endl;
		cout << " +TA " << getDirection(target,180) << endl;
		cout << " +MX " << current_max << endl;
		cout << " +DA " << max_angle << endl;
		cout << " +MA " << getDirection(max_go,180) << endl;
		cout << " +DT " << delta << endl << endl;	
		//double dist = absc(target);
		//delta.x += (sgn(target.x)*15.0 / (dist*dist));
		//delta.y += (sgn(target.y)*15.0 / (dist*dist));
		where = where + delta;
		//boxes.push_back(SBox(where - SCoord(2 * delta.x, 2 * delta.y),where - delta));
	
		if (InBox(where, boxes))
		{
			// try backtracking on X
			where = where - SCoord(delta.x,0.0);
			if (InBox(where, boxes))
			{
				// If still in-box, try back-tracking on Y
				where = where - SCoord(0.0-delta.x,delta.y);
				if  (InBox(where, boxes))
				{
					//If still in box, try back-tracking on both
					where = where - delta;
					delta = SCoord(0,0) - delta;
				}
				else 
				{ 
					// Success backtracking on y 
					delta = SCoord(delta.x, 0.0 - delta.y / bouncefac);
				}
			}
			else 
			{	
				// Success backtracking on X
				delta = SCoord(0.0 - delta.x / bouncefac, delta.y);
			}
			/*
			while (InBox(where, boxes)) { 
				cout << "EE !!!" << endl; 
				where = where - delta;
			}
			*/
		}
		/*if ((!(where + sensors[i] < size)) ||
			(!(where + sensors[i] > SCoord(0,0)))
			)*/
		/*
		while (!SBox(0,0,size.x,size.y).InBox(where)) { 
			cout << "EE !" << endl; 
			where = where - delta;
		}
		delta = SCoord(0,0) - delta;
		*/
		cout << "AT " << where << endl;
	}

	return 0;
}
