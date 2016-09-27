
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#include "config.h"
#endif
#ifdef DATABASE
#ifdef DB_MYSQL
#include <mysql.h>

#include "bases/Public.h"
#include "../Global.h"
#include "DBOperator.h"
#include "MySQLOperator.h"


CMySQLOperator::CMySQLOperator()
{
    m_pMySQL = NULL;
    m_pResult = NULL;
}

CMySQLOperator::~CMySQLOperator()
{
}

int CMySQLOperator::Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding)
{
    strncpy(m_cHost, pHost, DB_DATA_LENGTH);
    m_nPort = nPort;
    strncpy(m_cUser, pUser, DB_DATA_LENGTH);
    strncpy(m_cPasswd, pPassward, DB_DATA_LENGTH);
    strncpy(m_cDatabase, pDatabase, DB_DATA_LENGTH);

    m_pMySQL = mysql_init(NULL);
    if(!m_pMySQL)
    {
        ERROR_PRINT("mysql init error%c", '\n');
        return -1;
    }

    if(0 != mysql_options(m_pMySQL, MYSQL_SET_CHARSET_NAME, "utf8"))
    {
        ERROR_PRINT("set mysql option MYSQL_SET_CHARSET_NAME error%c", '\n');
    }

    char flag = 1;
    int rOp = mysql_options(m_pMySQL, MYSQL_OPT_RECONNECT, (char *)&flag);
    if(rOp)
    {
        ERROR_PRINT("set mysql option MYSQL_OPT_RECONNECT error%c", '\n');
    }

    MYSQL * pMySQL = mysql_real_connect(m_pMySQL, m_cHost, m_cUser, m_cPasswd, m_cDatabase, m_nPort, NULL, 0);
    if(!pMySQL)
    {
        ERROR_PRINT("mysql connect error: %s\n", mysql_error(m_pMySQL));
        mysql_close(m_pMySQL);
        m_pMySQL = NULL;
        return -2;
    }

    return 0;
}

int CMySQLOperator::Fini()
{
    if(m_pMySQL)
    {
        mysql_close(m_pMySQL);
        m_pMySQL = NULL;
    }

    m_pResult = NULL;

    return 0;
}

int CMySQLOperator::CommitSQL(char * pSQL, int nLength)
{
    if(mysql_real_query(m_pMySQL, pSQL, nLength))
        ERROR_PRINT("mysql error(%d):%s\n", nLength, mysql_error(m_pMySQL));

    return 0;
}

int CMySQLOperator::GetValueStart(char * pSQL)
{
    if(!pSQL)
    {
        ERROR_PRINT("Mysql SQL error%c", '\n');
        return -1;
    }
    if(!m_pMySQL)
        return -2;

    while(mysql_real_query(m_pMySQL, pSQL, strlen(pSQL)))
    {
        if(mysql_ping(m_pMySQL))
        {
            ERROR_PRINT("reconnect mysql error:%s\n", mysql_error(m_pMySQL));
        }
        else
        {
            break;
        }
    }

    m_pResult = mysql_store_result(m_pMySQL);
    if(!m_pResult)
    {
        ERROR_PRINT("store mysql result error%c", '\n');
        return -3;
    }

    return 0;
}

int CMySQLOperator::GetValueCount()
{
    return 0;
}

int CMySQLOperator::GetValueNext()
{
    m_stRow = mysql_fetch_row(m_pResult);
    if(mysql_errno(m_pMySQL))
    {
        ERROR_PRINT("[CMySQLOperator::GetValueNext]%s\n", mysql_error(m_pMySQL));
        return -1;
    }

    return !m_stRow;
}

int CMySQLOperator::GetValueNext(int nIndex)
{
    m_stRow = mysql_data_seek(m_pResult, nIndex);
    if(mysql_errno(m_pMySQL))
    {
        ERROR_PRINT("[CMySQLOperator::GetValueNext]%s\n", mysql_error(m_pMySQL));
        return -1;
    }

    return !m_stRow;
}

int CMySQLOperator::GetValue(const char ** pValue, int nIndex)
{
    if(pValue == NULL || nIndex < 0)
    {
        ERROR_PRINT("input arguments error%c", '\n');
        return -1;
    }

    if(!m_pResult)
    {
        ERROR_PRINT("store mysql result error%c", '\n');
        return -2;
    }

    if(!m_stRow)
    {
        ERROR_PRINT("line result error%c", '\n');
        return -3;
    }

    *pValue = m_stRow[nIndex];

    return 0;
}

int CMySQLOperator::GetValueStop()
{
    if(m_pResult)
    {
        mysql_free_result(m_pResult);
        m_pResult = NULL;
    }

    return 0;
}

#endif
#endif
