
#ifdef DATABASE

typedef enum DBType_en
{
    MySQL = 1,
    Postgres,
    InfiniDB,
    ODBC,
    Oracle
}DBType_t;

class CDatabaseAgent
{
public:
    int Connect(DBType_t enType, char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding);
    int QueryStart(char * pSQL);
    int QueryCount();
    int Query(UnitInfo_t * pValues, int nUnitCount);
    int Query(UnitInfo_t * pValues, int nUnitCount, int nIndex);
    int QueryStop();
    int Commit(char * pSQL);
    int Disconnect();

protected:
    class CDBOperator * m_pDBOperator;
};

int ConvertCharset(const char * pSrc, int nSrcLen, char * pDst, int nDstLen);

#endif
