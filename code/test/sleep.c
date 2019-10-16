#include "syscall.h"
//#include "main.h"

main() 
{
    int i;
    for(i = 0; i < 5; i++) 
	{
//		cout << "The " << i << "sleep times................." <<endl;
        Sleep(1500000);
        PrintInt(i);
    }
    return 0;
}
