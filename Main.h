#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <dlfcn.h>
#include "mailbox.h"


#define FALSE false
#define TRUE true
typedef unsigned long DWORD ;
typedef unsigned short WORD ;
typedef char * LPSTR ;
typedef const char * LPCSTR ;
typedef const char * LPCTSTR ;
typedef unsigned char UCHAR ;
typedef unsigned char BYTE ;
typedef unsigned short USHORT ;
typedef bool BOOL ;

#include "afxtempl.h"
#include "Geometry_linux.h"

void Sleep(int nMilliseconds) ;

#define TIMEOUT_MS  5000 


