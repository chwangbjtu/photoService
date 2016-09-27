
#ifdef DATABASE
#ifdef DB_POSTGRESQL

class CPostgreSQLOperator : public CDBOperator
{
public:
    CPostgreSQLOperator();
    ~CPostgreSQLOperator();

    int Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * Encoding = NULL);
    int Fini();

    int CommitSQL(char * pSQL, int nLength);

    int GetValueStart(char * pSQL);
    int GetValueCount();
    int GetValueNext();
    int GetValueNext(int nIndex);
    int GetValue(const char ** pValue, int nIndex);
    int GetValueStop();

protected:
    char m_cHost[DB_DATA_LENGTH];
    char m_cPort[DB_DATA_LENGTH];
    char m_cUser[DB_DATA_LENGTH];
    char m_cPasswd[DB_DATA_LENGTH];
    char m_cDatabase[DB_DATA_LENGTH];

private:
    PGconn * m_pPostgreSQL;
    PGresult * m_pResult;
    int m_nRowCount;
    int m_nRowQuantity;
};

#endif
#endif
