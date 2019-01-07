// CString.h
#ifndef cstring_h
#define cstring_h

class CString
{
public:
    CString();
    CString(CString &src);
    CString(const char *src);
    ~CString();
    
    int GetLength() ;
    CString& MakeUpper() ;
    CString& MakeLower() ;
    CString Left(int nPos) ;
    CString Mid(int nPos, int nLen=-1) ;
    CString Right(int nPos) ;
    bool operator==(const char *src) ;
    bool operator!=(const char *src) ;
    CString& operator=(const CString& src) ;
    CString& operator=(const char *src) ;
    CString& operator=(char src) ;
    CString& operator+=(const CString& src) ;
    CString& operator+=(const char *src) ;
    CString& operator+=(char src) ;
    CString operator+(const CString& src) ;
    CString operator+(const char *src) ;
    CString operator+(char src) ;
    void Remove(char cRemove) ;
    int ReverseFind(char pSrc);
    operator char *() ;
    void Empty();
    int Compare(char *) ;
    int Delete (int iIndex, int nCount) ;
    int Insert (int iIndex, int nCount) ;
    int Find (const char *pattern, int nStart=0) ;
    int FindOneOf (const char *charset) ;
    int Format ( const char *szFormatStr, ... ) ;
    char* GetBuffer(int nMaxSize=0);
    void ReleaseBuffer();
    
public:
    char *m_pStr ;
};



#endif

