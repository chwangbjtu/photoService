
#include <string.h>
#include <stdio.h>

#ifdef DATABASE
#ifdef DB_POSTGRESQL
#include "libpq-fe.h"

#include "../Public.h"
#include "../Log/Log.h"
#include "DBOperator.h"
#include "PostgreSQLOperator.h"

CPostgreSQLOperator::CPostgreSQLOperator()
{
    m_pPostgreSQL = NULL;
    m_pResult = NULL;
    m_nRowCount = 0;
}

CPostgreSQLOperator::~CPostgreSQLOperator()
{
}

int CPostgreSQLOperator::Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding)
{
    strncpy(m_cHost, pHost, DB_DATA_LENGTH);
    sprintf(m_cPort, "%d", nPort);
    strncpy(m_cUser, pUser, DB_DATA_LENGTH);
    strncpy(m_cPasswd, pPassward, DB_DATA_LENGTH);
    strncpy(m_cDatabase, pDatabase, DB_DATA_LENGTH);

//     m_pPostgreSQL = mysql_init(NULL);
//     if(!m_pPostgreSQL)
//     {
//         ERROR_PRINT("mysql init error%c", '\n');
//         return -1;
//     }
// 
//     char flag = 1;
//     int rOp = mysql_options(m_pPostgreSQL, MYSQL_OPT_RECONNECT, (char *)&flag);
//     if(rOp)
//     {
//         ERROR_PRINT("set mysql option MYSQL_OPT_RECONNECT error%c", '\n');
//     }

    m_pPostgreSQL = PQsetdbLogin(m_cHost, m_cPort, NULL, NULL, m_cDatabase, m_cUser, m_cPasswd);
    if(!m_pPostgreSQL)
    {
        ERROR_OUTSIDE_PRINT("PostgreSQL connect error: %s\n", PQerrorMessage(m_pPostgreSQL));
        PQfinish(m_pPostgreSQL);
        m_pPostgreSQL = NULL;
        return -2;
    }

    if(NULL != pEncoding)
    {
        if(0 != PQsetClientEncoding(m_pPostgreSQL, pEncoding))
        {
            ERROR_OUTSIDE_PRINT("set PostgreSQL ClientEncoding %s error!\n", pEncoding);
        }
    }

    return 0;
}

int CPostgreSQLOperator::Fini()
{
    if(m_pPostgreSQL)
    {
        PQfinish(m_pPostgreSQL);
        m_pPostgreSQL = NULL;
    }

    m_pResult = NULL;

    return 0;
}

int CPostgreSQLOperator::CommitSQL(char * pSQL, int nLength)
{
    if(PQexec(m_pPostgreSQL, pSQL))
    {
        ERROR_OUTSIDE_PRINT("mysql error(%d):%s\n", nLength, PQerrorMessage(m_pPostgreSQL));
    }

    return 0;
}

int CPostgreSQLOperator::GetValueStart(char * pSQL)
{
    if(!pSQL)
    {
        ERROR_OUTSIDE_PRINT("Mysql SQL error%c", '\n');
        return -1;
    }
    if(!m_pPostgreSQL)
        return -2;

    m_pResult = PQexec(m_pPostgreSQL, pSQL);
//     while(PQexec(m_pPostgreSQL, pSQL, strlen(pSQL)))
//     {
//         if(mysql_ping(m_pPostgreSQL))
//         {
//             ERROR_PRINT("reconnect mysql error:%s\n", mysql_error(m_pPostgreSQL));
//         }
//         else
//         {
//             break;
//         }
//     }

//     m_pResult = mysql_store_result(m_pPostgreSQL);
    if(!m_pResult)
    {
        ERROR_OUTSIDE_PRINT("store mysql result error:%s\n", PQerrorMessage(m_pPostgreSQL));
        return -3;
    }

    m_nRowQuantity = PQntuples(m_pResult);
    m_nRowCount = -1;

    return 0;
}

int CPostgreSQLOperator::GetValueCount()
{
    return m_nRowQuantity;
}

int CPostgreSQLOperator::GetValueNext()
{
    m_nRowCount++;
    if(m_nRowCount >= m_nRowQuantity)
        return -1;

    return 0;
}

int CPostgreSQLOperator::GetValueNext(int nIndex)
{
    m_nRowCount = nIndex;
    if((m_nRowCount < 0) || (m_nRowQuantity <= m_nRowCount))
        return -1;

    return 0;
}


int CPostgreSQLOperator::GetValue(const char ** pValue, int nIndex)
{
    if(pValue == NULL || nIndex < 0)
    {
        ERROR_OUTSIDE_PRINT("input arguments error%c", '\n');
        return -1;
    }

    if(!m_pResult)
    {
        ERROR_OUTSIDE_PRINT("store mysql result error%c", '\n');
        return -2;
    }

    *pValue = PQgetvalue(m_pResult, m_nRowCount, nIndex);

    if(!*pValue)
    {
        ERROR_OUTSIDE_PRINT("line result error:%s\n", PQerrorMessage(m_pPostgreSQL));
        return -3;
    }

    return 0;
}

int CPostgreSQLOperator::GetValueStop()
{
    if(m_pResult)
    {
        PQclear(m_pResult);
        m_pResult = NULL;
    }

    return 0;
}

#endif
#endif
