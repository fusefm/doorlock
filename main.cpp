/*
 * FDS - Fuse Door System
 * main.cpp
 *
 * Main routine
 *
 *  Created on: 17-Jun-2009
 *      Author: matthew Leach
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <string.h>

//local includes:
#include "misc.h"
#include "ldap.h"
#include "sql.h"
#include "parallel.h"

using namespace std;

int
main(int argc, char* argv[])
{

  //Get options set in the config file.
  optStruct options;
  options.intTimeout = 0;
  options.strErrorDir = "";
  options.strLDAPuri = "";
  options.strSqlPassword = "";
  options.strSqlServer = "";
  options.strSqlTable = "";
  options.strSqlUser = "";

  if (fnGetOpt("/etc/FDS.conf", &options) == false)
    {
      //error whilst reading the config file.
      std::cout << fnGetCurrentTime()
          << ": ERROR: Could not read in the config file" << " correctly.\n";
      exit(1);
    }

  //Main loop:
  while (1)
    {
      string cardData;
      cin >> cardData;
      if ((int) cardData.length() > 12)
        {
          //remove irrelivant text at the beggning:
          cardData.erase(0, 5);
          cardData.erase(7, cardData.length());
          std::cout << cardData << std::endl;
          int result;
          result = fnCheckDoorAccess(cardData, &options);

          if (result == 0)
            {
              //do not open the door.
              std::cout << "Not opening the door." << std::endl;
              fnSqlLogAccess(cardData, 0, &options);
            }
          else if (result == 1)
            {
              //open the door.
              std::cout << "openning the door." << std::endl;
              fnSetPort(255);
              sleep(5);
              fnSetPort(0);
              fnSqlLogAccess(cardData, 1, &options);
            }
        }
    }
}
