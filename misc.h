/*
 * FDS - Fuse Door System.
 * misc.h
 * contains miscellaneous functions.
 *
 *  Created on: 17-Jun-2009
 *      Author: matthew
 */

#ifndef MISC_H_
#define MISC_H_

#include <string>
#include <vector>
using namespace std;

struct optStruct
{
  string strLDAPuri;
  vector<string> vecStrSearchGroups;
  vector<string> vecStrSearchOnAirGroups;
  int intTimeout;
  int intDoorOpenTime;
  string strErrorDir;
  string strSqlServer;
  string strSqlUser;
  string basedn;
  string strSqlPassword;
  string strSqlTable;
};

//Function fnGetDateTime. This will get the current date and time in a human readable format
void
fnLogError(string strFile, string errorMessage);
string
fnGetCurrentTime();
string
fnConvertIntToString(int number);
bool
fnGetOpt(string strFile, optStruct* opts);
#endif /* MISC_H_ */
