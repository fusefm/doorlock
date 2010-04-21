/*
 * FDS - Fuse Door System
 * ldap.cpp
 * LDAP functions for check LDAP stuff.
 *
 *  Created on: 17-Jun-2009
 *      Author: Matthew Leach
 */

#include <ldap.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "misc.h"
#include "sql.h"

using namespace std;



//check the user is in an LDAP thing:
int fnCheckForUserInLDAP(LDAP* svr, string strAccessNumber, optStruct* opts) {
    //ensure we have a valid LDAP object:
    if(svr == NULL) {
        return -1;
    }
    
    //declare variables:
    struct timeval tv;
    LDAPMessage* resultsChain;
    LDAPMessage* result;
    struct berval** val;
    int retval;

    //set up the timeval structure and use for timeout.
    tv.tv_usec = 0;
    tv.tv_sec = opts->intTimeout;

    string strSearchString = "(uid=";
    strSearchString.append(strAccessNumber);
    strSearchString.append(")");
    
    //do the search for the user:
    retval = ldap_search_ext_s(svr, opts->basedn.c_str(), LDAP_SCOPE_SUBTREE,
            strSearchString.c_str(), NULL, 0, NULL, NULL, &tv, 0, &resultsChain);
    
    //error:
    if (retval != LDAP_SUCCESS) {
        return -1;
    }
    //make sure we have a user:
    if (resultsChain != NULL) {
        //get the individual user from the result (we are expecting 1 user only to be found):
        int len = ldap_count_entries(svr, resultsChain);
        if(len == 1) {
            //the user is in LDAP:
            ldap_msgfree(resultsChain);
            return 1;
        }
        ldap_msgfree(resultsChain);
        return 0;
    }
    //there was an error:
    return -1;
}

int fnCheckForUserInLDAPGroup(LDAP* svr, string strAccessNumber,
        string strSearchGroup, optStruct* opts) 
{
    string strSearchString;               //string to be used for searching the LDAP server:
    int retval;
    LDAPMessage *resultsChain = NULL;     //LDAP message structure to store results chain from searches.
    LDAPMessage *result = NULL;           //LDAP message structure to store individual results from the results chain.
    struct timeval tv;                    //time value structure for setting timeouts.
        struct berval ** val;                 //berval struct to store the array of users from the search.
    //set up the timeval structure and use for timeout.
    tv.tv_usec = 0;
    tv.tv_sec = opts->intTimeout;
    
    //build up the search string:
    strSearchString = "(cn=";
    strSearchString.append(strSearchGroup);
    strSearchString.append(")");
    
    //perform the search:
    retval = ldap_search_ext_s(svr, opts->basedn.c_str(), LDAP_SCOPE_SUBTREE,
            strSearchString.c_str(), NULL, 0, NULL, NULL, &tv, 0, &resultsChain);
    //error:Group
    if (retval != LDAP_SUCCESS) 
    {
        //print the error:
        string strError;
        strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
        "CALL: ldap_search_ext_s AccessGroup, MESSAGE: ");
        
        strError.append(ldap_err2string(retval));
        fnLogError(opts->strErrorDir, strError);
        
        ldap_unbind_ext_s(svr, NULL, NULL);
        
        return -1;
    }
    
    //make sure we have a group:
    if (resultsChain != NULL) 
    {
        //get the individual group from the result (we are expecting 1 group only to be found):
        result = ldap_first_entry(svr, resultsChain);
        if (result == NULL) 
        {
            //print the error:
            string strError;
            strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
            "CALL: ldap_first_entry AccessGroup, MESSAGE: ");
            strError.append("result berval struct is NULL.");
            fnLogError(opts->strErrorDir, strError);
            
            ldap_unbind_ext_s(svr, NULL, NULL);
            return 0;
        }
        
        //get the values for the "memberUid" attribute:
        val = ldap_get_values_len(svr, result, "memberUid");
        if (val != NULL) 
        {
            int num; //number of users in the group.
            num = ldap_count_values_len(val);
            for (int j = 0; j < num; j++)
            {
                //loop through each user in the group:
                if (strcmp(strAccessNumber.c_str(), val[j]->bv_val) == 0)
                {
                    ldap_value_free_len(val);
                    
                    //the user is in the access group, allow them entry.
                    ldap_msgfree(resultsChain);
                    
                    //write out a message:
                    std::cout << "Found user in LDAP Group: "
                    << strSearchGroup << std::endl;
                    
                    return 1;
                    
                }
            }
        }
        ldap_value_free_len(val);
    }
    ldap_msgfree(resultsChain);
}//end of All-Access group loop.


/*
 * fnCheckDoorAccess will use a combination of MYSQL and LDAP queries to
 * check if entey is permitted.
 * return value of 1 - allow ernty
 * return value of 0 - deny entry
 * return value of -1 - error.
 *
 */
int
fnCheckDoorAccess(string accessNumber, optStruct *opt) {
    LDAP* svr;                            //LDAP server structure.
    int retval;                           //return value var.
    struct timeval tv;                    //time value structure for setting timeouts.
    int LDAPVersion = LDAP_VERSION3;      //value to set the LDAP client version.
   // LDAPMessage *resultsChain = NULL;     //LDAP message structure to store results chain from searches.
   // LDAPMessage *result = NULL;           //LDAP message structure to store individual results from the results chain.
   // string strSearchString;               //Search string to be used to find the user in LDAP.
   // struct berval ** val;                 //berval struct to store the array of users from the search.
    struct berval cred;                   //credentials structure, this is going to be empty
    //as we are using anonymous credentials.
    
    //Initialise the LDAP struct:
    retval = ldap_initialize(&svr, opt->strLDAPuri.c_str());
    
    //check for errors:
    if (retval != LDAP_SUCCESS) {
        //print the error:
        string strError;
        strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
        "CALL: ldap_initialize, MESSAGE: ");
        
        strError.append(ldap_err2string(retval));
        fnLogError(opt->strErrorDir, strError);
        //exit with error code:
        return -1;
    }
    
    //set LDAP version 3 client:
    retval = ldap_set_option(svr, LDAP_OPT_PROTOCOL_VERSION, &LDAPVersion);
    if (retval != LDAP_SUCCESS) {
        //print the error:
        string strError;
        strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
        "CALL: ldap_set_option LDAP_OPT_PROTOCOL_VERSION, MESSAGE: ");
        
        strError.append(ldap_err2string(retval));
        fnLogError(opt->strErrorDir, strError);
        //error:
        return -1;
    }
    
    //set up the timeval structure and use for timeout.
    tv.tv_usec = 0;
    tv.tv_sec = opt->intTimeout;
    retval = ldap_set_option(svr, LDAP_OPT_NETWORK_TIMEOUT, &tv);
    if (retval != LDAP_SUCCESS) {
        //print the error:
        string strError;
        strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
        "CALL: ldap_set_option LDAP_OPT_NETWORK_TIMEOUT, MESSAGE: ");
        
        strError.append(ldap_err2string(retval));
        fnLogError(opt->strErrorDir, strError);
        //error:
        return -1;
    }
    
    //set up the cred structure:
    cred.bv_len = 0;
    cred.bv_val = (char*) "";
    
    //bind anonymously with the LDAP server:
    retval = ldap_sasl_bind_s(svr, NULL, LDAP_SASL_SIMPLE, &cred, NULL, NULL,
            NULL);
    if (retval != LDAP_SUCCESS) {
        //there was an error:
        //print the error:
        string strError;
        strError.assign("ERROR: FUNCTION: fnCheckDoorAccess, "
        "CALL: ldap_sasl_bind_s, MESSAGE: ");
        strError.append(ldap_err2string(retval));
        fnLogError(opt->strErrorDir, strError);
        
        ldap_unbind_ext_s(svr, NULL, NULL);
        
        return -1;
    }
    
    //firstly, check that the user is in LDAP:
    if(fnCheckForUserInLDAP(svr, accessNumber, opt) < 1) 
    {
        //error or we can't find the user:
        ldap_unbind_ext_s(svr, NULL, NULL);
        //the user is not in any of the groups, do not allow them entry:
        std::cout << "User " << accessNumber << " not found." << std::endl;
        return 0;
    }
    //the user is in LDAP, do some group checking:
    
    for (unsigned int i = 0; i < opt->vecStrSearchGroups.size(); i++)
    {
        //loop through all the different search groups:
        retval =fnCheckForUserInLDAPGroup(svr, accessNumber, opt->vecStrSearchGroups[i],
                opt);
        if(retval == 1)
        {
            //we found the user in one of the all access groups:
            ldap_unbind_ext_s(svr, NULL, NULL);
            return 1;
        }
        else if(retval == -1)
        {
            ldap_unbind_ext_s(svr, NULL, NULL);
            return -1;
        }
    }//end of All-Access group loop.
    
    /*
     * if we get here then, the user is not in any of the All-Access groups.
     * we need to search LDAP for the actual user, see if they are in the On-Air groups
     * and check the SQL serve rfor the on-air field as well as the payeduntillaugestof
     * field in LDAP before they are let in.
     */
    
    for (unsigned int i = 0; i < opt->vecStrSearchOnAirGroups.size(); i++) {
        //loop through all the different search groups:
        retval = fnCheckForUserInLDAPGroup(svr, accessNumber, 
                opt->vecStrSearchOnAirGroups[i], opt);
        if(retval == 1)
        {
            //ldap_unbind_ext_s(svr, NULL, NULL);
	    //we now need to make sure that they are in the paied members group:
            retval = fnCheckForUserInLDAPGroup(svr,accessNumber, 
		"PaidMembers", opt);
            if(retval == 1)
            {
               ldap_unbind_ext_s(svr, NULL, NULL);
               return fnSqlCheckOnAir(opt);
            }
            else
            {
               cout << "That'll be £5 please." << endl;
               ldap_unbind_ext_s(svr, NULL, NULL);
               return 0;
            }
        }
        else if(retval == -1)
        {
            ldap_unbind_ext_s(svr, NULL, NULL);
            return -1;
        }
    }//end of on-air access groups check
    
    return 0;
}

