
#ifdef DATABASE
#ifdef DB_ODBC


#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#include "config.h"
#endif

#include "../Bases/Public.h"
#include "../Global.h"
#include "DBOperator.h"
#include "../dbresultset/DBResultsetCls.h"
#include "ODBCOperator.h"


CODBCOperator::CODBCOperator()
{
    m_pODBC = NULL;
}

CODBCOperator::~CODBCOperator()
{
}

int CODBCOperator::Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding)
{
    strncpy(m_cHost, pHost, DB_DATA_LENGTH);
    m_nPort = nPort;
    strncpy(m_cUser, pUser, DB_DATA_LENGTH);
    strncpy(m_cPasswd, pPassward, DB_DATA_LENGTH);
    strncpy(m_cDatabase, pDatabase, DB_DATA_LENGTH);

    /*DBResultsetCls RsSet(pDatabase);
    if( RsSet.ExecuteSQL("select tac,termtype,terminal,producer from ts_terminal where tac is not null") != SQL_SUCCESS )
    {
        printf("open ts_terminal error!\n");
    }
    while( RsSet.Next() == SQL_SUCCESS )
    {
        printf("%s\n", RsSet.GetField(1).c_str());
        break;
    }*/

    return 0;
}

int CODBCOperator::Fini()
{
    if(m_pODBC)
    {
        delete m_pODBC;
        m_pODBC = NULL;
    }

    return 0;
}

int CODBCOperator::CommitSQL(char * pSQL, int nLength)
{
    if( m_pODBC->ExecuteSQL(pSQL) != SQL_SUCCESS )
    {
        ERROR_PRINT("ODBC error(%d):%s\n", nLength, pSQL);
        return -1;
    }

    return 0;
}

int CODBCOperator::GetValueStart(char * pSQL)
{
    m_pODBC = new DBResultsetCls(m_cDatabase);
    if(!m_pODBC)
    {
        ERROR_PRINT("ODBC init error%c", '\n');
        return -1;
    }

    if( m_pODBC->ExecuteSQL(pSQL) != SQL_SUCCESS )
    {
        ERROR_PRINT("ODBC error():%s\n", pSQL);
        return -1;
    }

    return 0;
}

int CODBCOperator::GetValueCount()
{
    return m_pODBC->GetRowCount();
}

int CODBCOperator::GetValueNext()
{
    if( m_pODBC->Next() != SQL_SUCCESS )
    {
        printf("[CODBCOperator::GetValueNext]\n");
        return -1;
    }

    return 0;
}

int CODBCOperator::GetValueNext(int nIndex)
{
    m_pODBC->Reset();
    if( m_pODBC->Skip(nIndex) != SQL_SUCCESS )
    {
        printf("[CODBCOperator::GetValueNext]\n");
        return -1;
    }

    return 0;
}

int CODBCOperator::GetValue(const char ** pValue, int nIndex)
{
    *pValue = m_pODBC->GetField(nIndex + 1).c_str();

    return 0;
}

int CODBCOperator::GetValueStop()
{
    if(m_pODBC)
    {
        m_pODBC->Close();
        m_pODBC = NULL;
    }

    return 0;
}

#endif
#endif
