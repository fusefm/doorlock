/*
 * FDS - Fuse Door System
 * sql.cpp
 * Contains SQL routines.
 *
 *  Created on: 17-Jun-2009
 *      Author: matthew
 */
#include <iostream>
#include <stdio.h>
#include "misc.h"
#include <string.h>
#include <mysql/mysql.h>

using namespace std;

void fnSqlLogAccess(string accessNumber, int result, optStruct* opts)
{
	MYSQL* objSql;
	string strQuery;

	objSql = mysql_init(NULL);

	//connect to the server
	if(!mysql_real_connect(objSql, opts->strSqlServer.c_str(),
			opts->strSqlUser.c_str(),
			opts->strSqlPassword.c_str(),
			opts->strSqlTable.c_str(),
			0, NULL, 0))
	{
		//we didn't connect
		string strTemp;
		strTemp.assign(mysql_error(objSql));
		strTemp.append(" fnSqlLogAccess");
		fnLogError(opts->strErrorDir,strTemp );
		return;
	}

	//set up the query string:
	strQuery = "insert into tbl_log (ID, AllowedEntry) values ('";
	strQuery.append(accessNumber);
	strQuery.append("', '");
	strQuery.append(fnConvertIntToString(result));
	strQuery.append("');");

	//do the query
	mysql_query(objSql, strQuery.c_str());

	//close the connection to the server:
	mysql_close(objSql);
}

int fnSqlCheckOnAir(optStruct* opts)
{
	MYSQL* objSql;						//mysql data object
	MYSQL_RES* result;					//the results set
	MYSQL_ROW resultRow;				//the actual restult from the results set.

	//initalise the library:
	objSql = mysql_init(NULL);

	//connect to the server
	if(!mysql_real_connect(objSql, opts->strSqlServer.c_str(),
			opts->strSqlUser.c_str(),
			opts->strSqlPassword.c_str(),
			opts->strSqlTable.c_str(),
			0, NULL, 0))
	{
		string strTemp;
		strTemp.assign(mysql_error(objSql));
		strTemp.append(" fnSqlCheckOnAir");
		fnLogError(opts->strErrorDir,strTemp );
		return -1;
	}

	//perform the query:
	if(mysql_query(objSql, "SELECT * FROM `tbl_Settings` LIMIT 1;") != 0)
	{
		string strTemp;
		strTemp.assign(mysql_error(objSql));
		strTemp.append(" fnSqlCheckOnAir");
		fnLogError(opts->strErrorDir,strTemp );
		return -1;
	}

	//we got some data (hopefully).
	result = mysql_store_result(objSql);
	if(!result)
	{
		//we didn't get any info, exit.
		fnLogError(opts->strErrorDir, "No Result.");
		return -1;
	}

	//get the row:
	resultRow = mysql_fetch_row(result);

	//free the results set as we don't need it:
	mysql_free_result(result);

	//close the connection to the database:
	mysql_close(objSql);

	//check to see if the station is on-air.
	if(strcmp(resultRow[0], "0") == 0)
	{
		return 0;
	}
	else if (strcmp(resultRow[0], "1") == 0)
	{
		return 1;
	}
	else
	{
		return -1;
		//something went wrong.
	}
}

