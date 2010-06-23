#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include <sys/io.h>

using namespace std;

int
main(int argc, char* argv[])
{
	
	std::cout << "openning the door." << std::endl;
	#define base 0x378           /* printer port base address */
	outb((unsigned char) 255, base);
	sleep(5);
	outb((unsigned char) 0, base);
	
	return 0;
	
}