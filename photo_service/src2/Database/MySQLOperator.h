
#ifdef DATABASE
#ifdef DB_MYSQL

class CMySQLOperator : public CDBOperator
{
public:
    CMySQLOperator();
    ~CMySQLOperator();

    int Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding);
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
    int m_nPort;
    char m_cUser[DB_DATA_LENGTH];
    char m_cPasswd[DB_DATA_LENGTH];
    char m_cDatabase[DB_DATA_LENGTH];

private:
    MYSQL * m_pMySQL;
    MYSQL_RES * m_pResult;
    MYSQL_ROW m_stRow;
};

#endif
#endif
