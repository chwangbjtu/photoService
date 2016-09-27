
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

#ifdef DATABASE
#ifdef DB_MYSQL
#include <mysql.h>
#endif
#ifdef DB_POSTGRESQL
#include <libpq-fe.h>
#endif
#ifdef DB_ODBC
#include "dbresultset/DBResultsetCls.h"
#endif

#include "../Public.h"
#include "../StrExt.h"
#include "../Log/Log.h"
#include "DBOperator.h"
#include "MySQLOperator.h"
#include "PostgreSQLOperator.h"
#include "ODBCOperator.h"
#include "DatabaseAgent.h"


int CDatabaseAgent::Connect(DBType_t enType, char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding)
{
    int nRC = 0;
    switch(enType)
    {
#ifdef DB_MYSQL
    case MySQL:
        m_pDBOperator = new CMySQLOperator;
        break;
#endif
#ifdef DB_POSTGRESQL
    case Postgres:
        m_pDBOperator = new CPostgreSQLOperator;
        break;
#endif
#ifdef DB_ODBC
    case ODBC:
        m_pDBOperator = new CODBCOperator;
        break;
#endif
    default:
        ERROR_CONFIG_PRINT("The database type was not supported yet!%c", '\n');
        return -1;
        break;
    }

    nRC = m_pDBOperator->Init(pHost, nPort, pUser, pPassward, pDatabase, pEncoding);
    if(0 != nRC)
    {
        ERROR_OUTSIDE_PRINT("database init failed!%c", '\n');
        return -1;
    }

    return 0;
}

int CDatabaseAgent::Disconnect()
{
    m_pDBOperator->Fini();
    delete m_pDBOperator;
    return 0;
}

int CDatabaseAgent::QueryStart(char * pSQL)
{
    m_pDBOperator->GetValueStart(pSQL);
    return 0;
}

int CDatabaseAgent::QueryCount()
{
    return m_pDBOperator->GetValueCount();
}

int CDatabaseAgent::Query(UnitInfo_t * pUnitInfo, int nUnitCount)
{
    const char * pValue = NULL;
    int iRC = 0;
    int iIndex = 0;

    iRC = m_pDBOperator->GetValueNext();
    if(iRC)
        return 1;

    //setlocale()
    for(int i = 0; i < nUnitCount; i++)
    {
        iRC = m_pDBOperator->GetValue(&pValue, iIndex++);
        if((0 != iRC) || (NULL == pValue))
            continue;
        switch(pUnitInfo[i].enType)
        {
        case UNIT_TYPE_INT:
            *((int *)pUnitInfo[i].pUnit) = atoi(pValue);
            break;
        case UNIT_TYPE_I64:
            *((long long *)pUnitInfo[i].pUnit) = strtoull(pValue, NULL, 10);
            break;
        case UNIT_TYPE_PTR:
            *((void **)pUnitInfo[i].pUnit) = (void *)pValue;
            break;
        case UNIT_TYPE_STR:
#ifdef WIN32
            ConvertCharset(pValue, strlen(pValue), (char *)pUnitInfo[i].pUnit, pUnitInfo[i].nAttribute);
#else
            strncpy((char *)pUnitInfo[i].pUnit, pValue, pUnitInfo[i].nAttribute);
#endif
            break;
        default:
            break;
        }
    }
    return 0;
}

int CDatabaseAgent::Query(UnitInfo_t * pUnitInfo, int nUnitCount, int nIndex)
{
    const char * pValue = NULL;
    int iRC = 0;
    int iIndex = 0;

    iRC = m_pDBOperator->GetValueNext(nIndex);
    if(iRC)
        return 1;

    //setlocale()
    for(int i = 0; i < nUnitCount; i++)
    {
        iRC = m_pDBOperator->GetValue(&pValue, iIndex++);
        if((0 != iRC) || (NULL == pValue))
            continue;
        switch(pUnitInfo[i].enType)
        {
        case UNIT_TYPE_INT:
            *((int *)pUnitInfo[i].pUnit) = atoi(pValue);
            break;
        case UNIT_TYPE_I64:
            *((long long *)pUnitInfo[i].pUnit) = strtoull(pValue, NULL, 10);
            break;
        case UNIT_TYPE_STR:
#ifdef WIN32
            ConvertCharset(pValue, strlen(pValue), (char *)pUnitInfo[i].pUnit, pUnitInfo[i].nAttribute);
#else
            strncpy((char *)pUnitInfo[i].pUnit, pValue, pUnitInfo[i].nAttribute);
#endif
            break;
        default:
            break;
        }
    }
    return 0;
}

int CDatabaseAgent::QueryStop()
{
    return m_pDBOperator->GetValueStop();
}

int CDatabaseAgent::Commit(char * pSQL)
{
    m_pDBOperator->CommitSQL(pSQL, strlen(pSQL));
    return 0;
}

//////////////////////////////////////////////////////////////////////////

void UTF_8ToUnicode(unsigned short * pOut,const char *pText)  
{
    char* uchar = (char *)pOut;
    uchar[1] = ((pText[0]&0x0F)<<4)+((pText[1]>>2)&0x0F);
    uchar[0] = ((pText[1]&0x03)<<6)+(pText[2]&0x3F);
}

void UnicodeToGB18030(char* pOut,unsigned short* uData)
{
#ifdef WIN32
    ::WideCharToMultiByte(CP_ACP,NULL,LPCWSTR(uData),1,pOut,sizeof(WCHAR),NULL,NULL);
#else
#endif
}

int ConvertCharset(const char * pSrc, int nSrcLen, char * pDst, int nDstLen)
{
    char Ctemp[4];

    memset(Ctemp, 0, 4);
    memset(pDst, 0, nDstLen);

    int i =0, j = 0;
    while((i < nSrcLen) && (j < nDstLen))
    {
        if(pSrc[i] > 0)
        {
            pDst[j++] = pSrc[i++];
        }
        else
        {
            unsigned short Wtemp;
            UTF_8ToUnicode(&Wtemp,pSrc+i);
            UnicodeToGB18030(Ctemp,(unsigned short *)&Wtemp);

            pDst[j] = Ctemp[0];
            pDst[j+1] = Ctemp[1];

            i+= 3;  
            j+= 2;  
        }  
    }
    return 0;
}

#endif
