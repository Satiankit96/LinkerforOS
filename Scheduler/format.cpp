
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <getopt.h>

// see also http://www.cplusplus.com/reference/iomanip/setw/

using namespace std;

int use_c = 0;

void printStuff(int arg1, int arg2)
{
	if (use_c) {
		printf("%3d: %4d\n", arg1, arg2);
		printf("%03d: %04d\n\n", arg1, arg2);
	} else {
		cout << setw(3) << arg1 << ": " << setw(4) << arg2 << endl;
		cout << setw(3) << setfill('0') << arg1 << ": " << setw(4) << arg2 << endl << endl;
		cout << setfill(' '); // have to reset the fill to nothing
	}
}


int main(int argc, char* argv[])
{
        int c;
        int schedtype = 0;

        // proper way to parse arguments
        while ((c = getopt(argc,argv,"c")) != -1 )
        {
                switch(c) {
                case 'c':
			printf("Using printf style instead of cout\n");
                        use_c = 1;
                        break;
                }
        }


	printStuff(1,2);
	printStuff(1000,2);
	printStuff(1,2000);
	printStuff(5,20);
	printStuff(20,5);
}

