#include "syscall.h"
main() 
{
	int i;
	int time = 0;
    for(i = 0; i < 5; i++) 
	{
		time = i*200000;
      	Sleep(time);
		// cout << "The " << time << " time" << endl;
        PrintInt(10);
    }
    return 0;
}
