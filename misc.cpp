/*
 * FDS - Fuse Door System.
 * misc.cpp
 * contains miscellaneous functions.
 *
 *  Created on: 17-Jun-2009
 *      Author: matthew
 */

#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "misc.h"
using namespace std;

void
trim(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if (pos != string::npos)
    {
      str.erase(pos + 1);
      pos = str.find_first_not_of(' ');
      if (pos != string::npos)
        str.erase(0, pos);
    }
  else
    str.erase(str.begin(), str.end());
}

int
fnConvertStringToInt(string str)
{
  return atoi(str.c_str());
}

//Function: fnCheckForZero: This will prefix a "0" onto an integer-string if it is less than
//2 digits:
string
fnCheckForZero(int intToCheck)
{
  string strToCheck;
  std::stringstream ss;
  ss << intToCheck;
  strToCheck = ss.str();
  if ((int) strToCheck.length() != 2)
    {
      strToCheck = "0" + strToCheck;
    }
  return strToCheck;
}

//Function: fnConvertIntToString. This will convert an integer type to a string:
string
fnConvertIntToString(int number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//Function fnGetDateTime. This will get the current date and time in a human readable format
string
fnGetCurrentTime()
{
  string strYear, strMonth, strDay, strHour, strMinute, strSecond, strDate;
  time_t t = time(0);
  tm time = *localtime(&t);

  strYear = fnConvertIntToString(time.tm_year + 1900);
  strMonth = fnCheckForZero(time.tm_mon + 1);
  strDay = fnCheckForZero(time.tm_mday);
  strHour = fnCheckForZero(time.tm_hour);
  strMinute = fnCheckForZero(time.tm_min);
  strSecond = fnCheckForZero(time.tm_sec);

  strDate = strYear + "-" + strMonth + "-" + strDay + " " + strHour + ":"
      + strMinute + ":" + strSecond;
  return strDate;
}

void
fnLogError(string strFile, string errorMessage)
{
  ofstream errorFile(strFile.c_str(), ios::app);
  errorFile << fnGetCurrentTime();
  errorFile << ": ";
  errorFile << errorMessage;
  errorFile << std::endl;
  errorFile.flush();
  errorFile.close();

}

bool
fnGetOpt(string strFile, optStruct* opts)
{
  //variable Declarations:
  int temp;
  bool gotOpt;
  string strLine, opt, value;

  //create the ifStream:
  std::ifstream optFile("/etc/FDS.conf");

  //make sure that we opened the file successfully:
  if (optFile.is_open())
    {
      //Loop through until the end of the file:
      while (!optFile.eof())
        {
          gotOpt = false;
          //get thpush_back(tempPush);e next line of the file:
          std::getline(optFile, strLine);

          //remove any comments:
          temp = strLine.find("#", 0);
          if (temp >= 0)
            {
              // remove comments on the same line as the options:
              strLine.replace(strLine.find("#", 0), strLine.length(), "");
              opt = "";
              value = "";
            }

          //split the string at the "=" sign:
          temp = strLine.find("=", 0);
          if (temp > 1)
            {
              if (temp != ((int) strLine.length() - 1))
                {
                  opt = strtok((char*) strLine.c_str(), "=");
                  value = strtok(NULL, "\0");
                  trim(opt);
                  trim(value);
                }
            }
          if (strLine == "")
            {
              opt = "";
              value = "";
            }

          //process the options:
          if (opt == "LDAPURI")
            {
              opts->strLDAPuri = value;
              gotOpt = true;
            }
          if (opt == "ErrorFile")
            {
              opts->strErrorDir = value;
              gotOpt = true;
            }
          if (opt == "SQLServer")
            {
              opts->strSqlServer = value;
              gotOpt = true;
            }
          if (opt == "SQLUser")
            {
              opts->strSqlUser = value;
              gotOpt = true;
            }
          if (opt == "timeout")
            {
              opts->intTimeout = fnConvertStringToInt(value);
              gotOpt = true;
            }
          if (opt == "doorOpenTimeout")
            {
              opts->intDoorOpenTime = fnConvertStringToInt(value);
              gotOpt = true;
            }
          if (opt == "SQLPassword")
            {
              opts->strSqlPassword = value;
              gotOpt = true;
            }
          if (opt == "SQLTable")
            {
              opts->strSqlTable = value;
              gotOpt = true;
            }
          if (opt == "basedn")
            {
              opts->basedn = value;
              gotOpt = true;
            }
          if (opt == "SearchGroups")
            {
              //for the All-Access Groups
              unsigned int index;
              string tempPush;
              index = value.find(";");
              while (index != string::npos)
                {
                  char buf[index + 1];
                  value.copy(buf, index, 0);
                  buf[index] = '\0';
                  tempPush.assign(buf);
                  opts->vecStrSearchGroups.push_back(tempPush);
                  value.erase(0, index + 1);
                  index = value.find(";");
                }
              gotOpt = true;
            }
          if (opt == "SearchPresenterGroups")
            {
              //for the Check-Access groups
              unsigned int index;
              string tempPush;
              index = value.find(";");
              while (index != string::npos)
                {
                  char buf[index + 1];
                  value.copy(buf, index, 0);
                  buf[index] = '\0';
                  tempPush.assign(buf);
                  opts->vecStrSearchOnAirGroups.push_back(tempPush);
                  value.erase(0, index + 1);
                  index = value.find(";");
                }
              gotOpt = true;
            }
          if (opt != "" and gotOpt == false)
            {
              cout << "WARNING: Unknown option: " << opt;
            }
        }
    }
  else
    {
      //error opening the file:
      std::cerr << "ERROR, could not open the config file at : " << strFile
          << "\n";
      return false;
    }

  //we got the options:
  return true;
}
