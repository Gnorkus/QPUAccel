#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include "CString_linux.h"

CString::CString()
{
    m_pStr = NULL ;
}

CString::CString(CString &src)
{
    m_pStr = NULL ;
    *this = src ;
}

CString::CString(const char *src)
{
    m_pStr = NULL ;
    *this = src ;
}

CString::~CString()
{
    Empty();
}

void CString::Empty()
{
    if (m_pStr)
        delete m_pStr;
    m_pStr = NULL;
}
  
CString& CString::MakeUpper() 
{
    int i, nlen ;
    if (m_pStr==NULL)
        return (*this);
    nlen = strlen(m_pStr) ;
    for (i=0; i<nlen; i++)
        m_pStr[i] = toupper(m_pStr[i]) ;
    return (*this) ;
}

CString& CString::MakeLower()
{
    int i, nlen ;
    if (m_pStr==NULL)
        return (*this);
    nlen = strlen(m_pStr) ;
    for (i=0; i<nlen; i++)
        m_pStr[i] = tolower(m_pStr[i]) ;
    return (*this) ;
}

CString& CString::operator=(const CString& src)
{
    int nlen ;
    if (m_pStr)
       delete m_pStr ;
    m_pStr = NULL ;
    nlen = strlen(src.m_pStr) + 1 ;
    m_pStr = new char [nlen] ;
    strcpy (m_pStr, src.m_pStr) ;
    m_pStr[nlen-1] = 0 ;
    return (*this) ;
}

CString& CString::operator=(const char *src) 
{
    int nlen ;
    if (m_pStr)
       delete m_pStr ;
    m_pStr = NULL ;
    nlen = strlen(src) + 1 ;
    m_pStr = new char [nlen] ;
    strcpy (m_pStr, src) ;
    m_pStr[nlen-1] = 0 ;
    return (*this) ;
}

CString& CString::operator=(char src) 
{
    int nlen ;
    if (m_pStr)
       delete m_pStr ;
    m_pStr = NULL ;
    nlen = 2 ;
    m_pStr = new char [nlen] ;
    m_pStr[0] = src ;
    m_pStr[1] = 0 ;
    return (*this) ;
}

CString& CString::operator+=(const CString& src) 
{
    char *tmp ;
    int nlen1, nlen2, nlen ;
    
    nlen1 = nlen2 = nlen = 0 ;
    
    if (m_pStr)
        nlen1 = strlen(m_pStr) ;
    if (src.m_pStr)
        nlen2 = strlen(src.m_pStr) ;
    nlen = nlen1 + nlen2 ;
    if (nlen==0)
        return (*this);
    tmp = new char [nlen+1] ;
    tmp[0] = 0 ;
    
    // Copy the strings
    if (nlen1)
        strcat(tmp, m_pStr) ;
    if (nlen2)
        strcat(tmp, src.m_pStr) ;
    delete [] m_pStr ;
    m_pStr = tmp ;
    return (*this) ;
}

CString& CString::operator+=(const char *src) 
{
    char *tmp ;
    int nlen1, nlen2, nlen ;

    nlen1=nlen2=nlen=0;

    if (m_pStr)    
        nlen1 = strlen(m_pStr) ;
    if (src)
        nlen2 = strlen(src) ;
    nlen = nlen1 + nlen2 ;
    if (nlen==0)
        return (*this);
    tmp = new char [nlen+1] ;
    tmp[0] = 0 ;
    
    if (nlen1)
        strcat(tmp, m_pStr) ;
    if (nlen2)
        strcat(tmp, src) ;
    delete [] m_pStr ;
    m_pStr = tmp ;
    return (*this) ;
}

CString& CString::operator+=(char src) 
{
    char tmp[2]  ;
    tmp[0] = src ;
    tmp[1] = 0 ;
    *this += tmp;
    return (*this);
}

bool CString::operator==(const char *src)
{
    if (src==NULL && m_pStr==NULL)
        return true ;
    if (m_pStr==NULL)
        return false;
    if (src==NULL)
        return false;
    if (strcmp(src,m_pStr)==0)
        return true ;
    return false;
}

bool CString::operator!=(const char *src)
{
    if (src==NULL && m_pStr==NULL)
        return false ;
    if (m_pStr==NULL)
        return true;
    if (src==NULL)
        return true;
    if (strcmp(src,m_pStr)==0)
        return false ;
    return true;
}

CString CString::operator +(const CString &src)
{
    CString rval ;
    
    rval = *this ;
    rval += src ;
    return rval ;
}

CString CString::operator +(const char *src)
{
    CString rval ;
    
    rval = *this ;
    rval += src ;
    return rval ;
}

CString CString::operator +(char src)
{
    CString rval ;
    
    rval = *this ;
    rval += src ;
    return rval ;
}

CString::operator char * () 
{
    return m_pStr;
}

CString CString::Left(int nPos)
{
    CString rval ;
    int     nlen ;
    
    nlen = GetLength();
    if (nlen<=0)
       return rval ;
    rval=*this;
    if (nPos>=nlen)
       return rval ;

    rval.m_pStr[nPos]=0;       
    return rval ;
}

CString CString::Right(int nPos)
{
    CString rval ;
    int     nlen ;
    
    nlen = GetLength();
    if (nlen<=0)
       return rval ;
    if (nPos>=nlen)
    {
       rval = *this;
       return rval ;
    }

    rval = m_pStr+(nlen-nPos) ;
    return rval ;
}

CString CString::Mid ( int nPos
                     , int nLen //=-1
                       )
{
    CString rval ;
    int     nlen ;

//    printf("CString::Mid\n");
    nlen = GetLength();
    
    if (nlen==0)
    {
//       printf("   GetLength=0\n");
       return rval ;
    }
       
    if (nPos>=nlen)
    {
//       printf("    nPos>GetLength\n");
       return rval ;
    }
       
    if (nLen==-1)
    {
//       printf("   nLen==-1\n");
       nLen = nlen-nPos;
    }
    
    if (nPos+nLen>nlen)
       nLen = nlen - nPos ;

    if (nLen==0)
       return rval ;
       
//    printf("    allocating new string of length %d\n", nLen+1);
    rval.m_pStr = new char [nLen+1] ;
    
//    printf("    attempting copy\n");
    memcpy ( rval.m_pStr, m_pStr + nPos, nLen ) ;
    
//    printf("    attempting set of null terminator\n");
    rval.m_pStr[nLen] = 0 ;
    return rval ;
}

int CString::Compare(char *src) 
{
    return strcmp(m_pStr, src) ;
}

int CString::GetLength()
{
    if (m_pStr==NULL)
        return 0 ;
    return strlen(m_pStr) ;
}

int CString::Delete (int nIndex, int nCount) 
{
    if (!m_pStr)
        return 0 ;
    int nlen = strlen(m_pStr) ;
    if (nIndex + nCount > nlen)
       nCount = nlen - nIndex ;
       
    strcpy (m_pStr+nIndex, m_pStr+nIndex+nCount) ;
    return 1 ;
}

int CString::Find (const char *pattern, int nStart) 
{
    if (m_pStr==NULL)
        return -1 ;
    if (strlen(m_pStr)==0 || (strlen(pattern)==0))
        return -1 ;
    
    char *pos = strstr(m_pStr, pattern) ;
    
    if (pos==NULL)
        return -1 ;
        
    return (pos - m_pStr) ;
}

int CString::FindOneOf (const char *charset) 
{
    return 0 ;
}

int CString::Format ( const char *szFormatStr, ... ) 
{
    char buffer[1024];
    va_list vl ;
    va_start(vl, szFormatStr) ;
    vsprintf(buffer, szFormatStr, vl) ;
    va_end(vl) ;
    
    *this = buffer;
    return 1 ;    
}

char* CString::GetBuffer(int nMaxLen)
{
    printf("CString::GetBuffer\n");
    char* rval;
    
    printf("    m_pStr=0x%08X  nMaxLen=%d\n", (unsigned int)m_pStr, nMaxLen);
    if (m_pStr==NULL || strlen(m_pStr)<nMaxLen)
    {
       printf("    trying to allocate\n");
       char *tbuf = new char[nMaxLen];
       if (m_pStr)
           memcpy(tbuf,m_pStr,strlen(m_pStr)+1);
       else
           tbuf[0]=0;
       delete [] m_pStr ;
       m_pStr = tbuf ;
    }
    else
       printf("    no reallocation needed\n") ;
    rval = m_pStr;
}

int CString::ReverseFind(char cFind)
{
    int i, n ;
    
    if (m_pStr==NULL)
        return -1;
    n = strlen(m_pStr);
    
    for (i=n; i>=0; i--)
    {
        if (m_pStr[i]==cFind)
            break;
    }
    return i;
}

void CString::Remove(char cRemove)
{
    int i, j, n, nCnt;
    
    if (m_pStr==NULL)
        return;
    n = strlen(m_pStr);
    nCnt = 0 ;
    for (i=0; i<n; i++)
    {
        if (m_pStr[i]==cRemove)
            nCnt++;
    }
    if (nCnt==0)
        return;
    if (nCnt==n)
    { 
        m_pStr[0] = 0 ;
        return;
    }
        
    n++;
    for (i=0,j=0; i<n; i++)
    {
        if (m_pStr[i]!=cRemove)
        {
            m_pStr[j]=m_pStr[i];
            j++;
        }
    }
}

void CString::ReleaseBuffer()
{
}
