#include <iostream>
#include "common.hpp"

using namespace std;

#define BOX_RES 14.0

#define BOX_SZ (100.0/BOX_RES)

int main()
{
	srand(time(0));
	cout << "size 100 100\nstart 2 2\nend 98 98" << endl;
	for (double i = BOX_SZ; i <= 100.0; i += 2*BOX_SZ)
	{
		for (double j = BOX_SZ; j <= 100.0; j += 2*BOX_SZ)
		{
			if ((i > 2*BOX_SZ || j > 2*BOX_SZ)
				&& (i < 100.0-BOX_SZ || j < 100.0 - BOX_SZ))
			{
				double m1 = 1.33 * BOX_SZ * uniform_deviate(rand());
				double m2 = 1.33 * BOX_SZ * uniform_deviate(rand());
				double m3 = 1.33 * BOX_SZ * uniform_deviate(rand());
				double m4 = 1.33 * BOX_SZ * uniform_deviate(rand());
				cout << "box " << i - m1 << " " << j - m2 << " " << i + m3 << " " << j + m4 << endl;
			}
		}
	}
}
