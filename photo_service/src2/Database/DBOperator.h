
#ifdef DATABASE
#define DB_DATA_LENGTH      1024

class CDBOperator
{
public:
    virtual ~CDBOperator(){}
    virtual int Init(char * pHost, int nPort, char * pUser, char * pPassward, char * pDatabase, char * pEncoding) = 0;
    virtual int Fini() = 0;

public:
    virtual int CommitSQL(char * pSQL, int nLength) = 0;

    virtual int GetValueStart(char * pSQL) = 0;
    virtual int GetValueCount() = 0;
    virtual int GetValueNext() = 0;
    virtual int GetValueNext(int nIndex) = 0;
    virtual int GetValue(const char ** pValue, int nIndex) = 0;
    virtual int GetValueStop() = 0;
};

#endif
