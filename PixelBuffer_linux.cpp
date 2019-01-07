#include "Main.h"
#include <byteswap.h>
#ifdef ENVISIC_CALIBRATION_UTILITY_VERSION

#endif
#include "PixelBuffer_linux.h"

// PixelBuffer.cpp for EnvisicCalibrationUtility

unsigned char ErosionMask[256] =
{
0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,
0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,0,
0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,
1,1,1,1,0,0,1,1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,0,0,
0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,
1,1,1,1,0,0,1,1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,0,0,0
};

static BOOL bCanDeletePixel[256]=
{
//  0 1 2 3 4 5 6 7 8 9
	0,0,0,1,0,0,1,1,0,0,  // 0
	1,1,1,0,1,1,0,0,0,0,  // 10
	0,0,0,0,1,0,1,1,1,0,  // 20
	1,1,0,0,0,0,0,0,0,0,  // 30
	1,0,1,1,1,1,1,1,1,0,  // 40
	0,0,0,0,0,0,1,0,1,1,  // 50
	1,0,1,1,0,0,0,0,0,0,  // 60
	0,0,0,0,0,1,0,0,0,0,  // 70
	0,0,0,0,0,0,0,0,0,0,  // 80
	1,1,0,0,0,1,1,0,0,0,  // 90
	0,0,0,0,1,1,1,1,1,1,  // 100
	1,1,1,0,0,0,0,0,0,0,  // 110
	1,0,1,1,1,1,1,0,0,1,  // 120
	1,1,0,0,1,1,0,0,1,1,  // 130
	0,0,1,1,0,0,0,0,0,0,  // 140
	1,0,0,0,1,1,0,0,1,1,  // 150
	1,1,1,1,0,1,1,1,1,1,  // 160
	0,0,1,1,0,0,1,1,1,1,  // 170
	1,1,1,1,1,1,0,0,1,1,  // 180
	0,0,1,1,1,1,0,0,1,1,  // 190
	0,0,1,1,0,0,1,1,0,0,  // 200
	1,0,0,0,1,1,0,0,1,1,  // 210
	0,0,1,0,1,1,1,1,0,0,  // 220
	1,1,1,1,0,0,1,1,0,0,  // 230
	1,1,1,1,0,1,1,0,1,1,  // 240
	0,0,1,0,0,0,		  // 250
};


// By definition, a sprig root will have
// more than 2 neighbors.  In such a case, 
// we will delete every point that has more
// than 2 neighbors.
static BOOL bIsSprigRoot[256]=
{
//  0 1 2 3 4 5 6 7 8 9
	0,0,0,0,0,0,0,1,0,0,  // 0
	0,1,0,1,1,1,0,0,0,1,  // 10
	0,1,1,1,0,1,1,1,1,1,  // 20
	1,1,0,0,0,1,0,1,1,1,  // 30
	0,1,1,1,1,1,1,1,0,1,  // 40
	1,1,1,1,1,1,1,1,1,1,  // 50
	1,1,1,1,0,0,0,1,0,1,  // 60
	1,1,0,1,1,1,1,1,1,1,  // 70
	0,1,1,1,1,1,1,1,1,1,  // 80
	1,1,1,1,1,1,0,1,1,1,  // 90
	1,1,1,1,1,1,1,1,1,1,  // 100
	1,1,1,1,1,1,1,1,1,1,  // 110
	1,1,1,1,1,1,1,1,0,0,  // 120
	0,1,0,1,1,1,0,1,1,1,  // 130
	1,1,1,1,0,1,1,1,1,1,  // 140
	1,1,1,1,1,1,1,1,1,1,  // 150
	0,1,1,1,1,1,1,1,1,1,  // 160
	1,1,1,1,1,1,1,1,1,1,  // 170
	1,1,1,1,1,1,1,1,1,1,  // 180
	1,1,0,1,1,1,1,1,1,1,  // 190
	1,1,1,1,1,1,1,1,1,1,  // 200
	1,1,1,1,1,1,1,1,1,1,  // 210
	1,1,1,1,1,1,1,1,1,1,  // 220
	1,1,1,1,1,1,1,1,1,1,  // 230
	1,1,1,1,1,1,1,1,1,1,  // 240
	1,1,1,1,1,1,		  // 250
};


CPixelBuffer::CPixelBuffer(int nStepSize) 
{
	m_pRadiusOffsets = NULL;
	m_pRadiusPoints = NULL ;
	m_nNumRadiusPoints = 0 ;
	m_nStepSize = nStepSize ;
	m_w = m_h = 0 ;
    m_buf 			  = (int *)NULL ;
	m_GaussianMask 	  = NULL ;
	m_GaussianOffsets = NULL ;
	m_bUseGpu 		  = false ;
    m_pPeriphAddress  = 0 ;
    m_pMapPeriphAddr  = NULL ;
    m_nPeriphSize     = 0 ;

	m_GpuData = 0 ;
	m_pArmData = NULL ;

	Reset() ;
}


CPixelBuffer::~CPixelBuffer()
{
	Reset() ;
}

void CPixelBuffer::Posterize ( CPixelBuffer *pbuf
							 , int			nNumGradations
							 , int			nMin
							 , int			nMax )
{
	double dDelta = (double)(nMax-nMin) / (double)nNumGradations ;
	double dVal ;
	double dMin, dMax, dRange ;
	double dNumGradations = (double) nNumGradations ;
	int i ;
	dMin = (double)nMin ;
	dMax = (double)nMax ;

	int maxval = pbuf->GetMax() ;
	if (!Init(pbuf->m_w, pbuf->m_h, pbuf->m_nStepSize))
		return ;

	dRange = (double)(nMax - nMin) ;

	switch (m_nStepSize)
	{
	case 1:
		for (i=0; i<m_size; i++)
		{
			dVal = ((unsigned char *)pbuf->m_buf)[i] ;
			dVal = (dVal - dMin) / dRange ;
			if (dVal<0.0)
				dVal = 0.0 ;
			else
			if (dVal>dNumGradations)
				dVal = dNumGradations ;
			dVal *= dDelta ;
			((unsigned char *)m_buf)[i] = (unsigned char)dVal ;
		}
		break;
	case 2:
		for (i=0; i<m_size; i++)
		{
			dVal = ((unsigned short *)pbuf->m_buf)[i] ;
			dVal = (dVal - dMin) / dRange ;
			if (dVal<0.0)
				dVal = 0.0 ;
			else
			if (dVal>dNumGradations)
				dVal = dNumGradations ;
			dVal *= dDelta ;
			((unsigned short *)m_buf)[i] = (unsigned short)dVal ;
		}
		break; 
	case 4:
		for (i=0; i<m_size; i++)
		{
			dVal = ((unsigned long *)pbuf->m_buf)[i] ;
			dVal = (dVal - dMin) / dRange ;
			if (dVal<0.0)
				dVal = 0.0 ;
			else
			if (dVal>dNumGradations)
				dVal = dNumGradations ;
			dVal *= dDelta ;
			((unsigned long *)m_buf)[i] = (unsigned long)dVal ;
		}
		break;
	}
}


void CPixelBuffer::FillEmptyMax(CPixelBuffer *pbuf)
{
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, maxv, v ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			v = pbuf->GetAtFast(x,y) ;
			if (v==0)
			{
				maxv = pbuf->GetAt(x-1, y-1) ;
				v = pbuf->GetAt(x,y-1) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x+1,y-1) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x+1,y) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x+1,y+1) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x,y+1) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x-1,y+1) ;
				if (maxv<v)
					maxv = v ;
				v = pbuf->GetAt(x-1,y) ;
				if (maxv<v)
					maxv = v ;
				SetAt(x,y,maxv) ;
			}
			else
				SetAt(x,y,v) ;
		}
	}
}

void CPixelBuffer::Reset()
{
    if (m_buf)
        delete [] m_buf ;
	if (m_GaussianMask)
		delete [] m_GaussianMask ;
	if (m_GaussianOffsets)
		delete [] m_GaussianOffsets ;
	if (m_pRadiusOffsets)
		delete m_pRadiusOffsets ;
	if (m_pRadiusPoints)
		delete m_pRadiusPoints ;

	m_pRadiusOffsets = NULL;
	m_pRadiusPoints = NULL ;
	m_nNumRadiusPoints = 0 ;
	m_buf = NULL ;
    m_w = m_h = m_size = 0 ;
	m_GaussianMask = NULL ;
	m_GaussianOffsets = NULL ;
	m_GaussianMaskWidth = 0;
	m_GaussianMaskHeight = 0;
}


int CPixelBuffer::GetMax()
{
	int maxval = 0 ;
    int numelem = m_w*m_h ;

	if (m_nStepSize==1)
	{
		for (int i=0; i<numelem; i++)
		{
			if (((unsigned char *)m_buf)[i] > maxval)
				maxval = m_buf[i] ;
		}
	}
	else
	if (m_nStepSize==2)
        {
		//ASSERT(false) ;
        }
	else
	if (m_nStepSize==4)
	{
		for (int i=0; i<numelem; i++)
		{
			if (m_buf[i] > maxval)
				maxval = m_buf[i] ;
		}
	}
	return maxval ;
}

int CPixelBuffer::GetMin()
{
	int minval = 0x7FFFFFFF ;
    int numelem = m_w*m_h ;

	/*
	int *pbuf = m_buf ;
	int rval ;

	_asm
	{
		mov esi, pbuf
		mov ecx, numelem
		mov edx, 0x7FFFFFFF

GetMin_Loop:
		cmp edx, [esi+ecx*4-4]
		jle GetMin_NotLess
		mov edx, [esi+ecx*4-4]
GetMin_NotLess:
		loop GetMin_Loop
GetMin_Done:
		mov rval, edx
	}
*/
	if (m_nStepSize==1)
	{
		for (int i=0; i<numelem; i++)
		{
			if (((unsigned char *)m_buf)[i] < minval)
				minval = m_buf[i] ;
		}
	}
	else
	if (m_nStepSize==2)
        {
		//ASSERT(false) ;
	}
	else
	if (m_nStepSize==4)
	{
		for (int i=0; i<numelem; i++)
		{
			if (m_buf[i] < minval)
				minval = m_buf[i] ;
		}
	}
	return minval ;
}


void CPixelBuffer::Normalize(CPixelBuffer *pbuf, int minval)
{
    //ASSERT(pbuf);
	int maxval = pbuf->GetMax() ;
	if (!Init(pbuf->m_w, pbuf->m_h))
		return ;

    int numelem = m_w*m_h ;
	if (minval==0)
	{
		for (int i=0; i<numelem; i++)
		{
			//m_buf[i] = MulDiv(pbuf->m_buf[i], 255, maxval) ;
			m_buf[i] = pbuf->m_buf[i] * 255 / maxval ;
		}
	}
	else
	{
		int minval = pbuf->GetMin() ;
		int range = maxval - minval ;
		int val ;

		for (int i=0; i<numelem; i++)
		{
			val = pbuf->m_buf[i] ;
			//m_buf[i] = MulDiv(val-minval, 255, range) ;
			m_buf[i] = (val-minval) * 255 / range ;
		}
	}
}


void CPixelBuffer::Normalize(CPixelBuffer *pbuf, int minval, int maxval)
{
    //ASSERT(pbuf);
	if (!Init(pbuf->m_w, pbuf->m_h))
		return ;

    int numelem = m_w*m_h ;
	for (int i=0; i<numelem; i++)
	{
		//m_buf[i] = MulDiv(pbuf->m_buf[i]-minval, 255, maxval-minval) ;
		m_buf[i] = (pbuf->m_buf[i]-minval)*255/(maxval-minval) ;
		if (m_buf[i]>255)
			m_buf[i]=255 ;
		if (m_buf[i]<0)
			m_buf[i]=0 ;
	}
}


void CPixelBuffer::Flip(CPixelBuffer *pBuf, int FlipMask)
{
    //ASSERT(pBuf);
	int x, y, ix, iy, my ;
	BOOL bFx, bFy ;
	unsigned char *sbuf, *dbuf ;

	if (!FlipMask)
	{







		Copy(*pBuf) ;
		return;
	}

	if (!Init(pBuf->m_w, pBuf->m_h, pBuf->m_nStepSize))
		return ;

	sbuf = (unsigned char *)pBuf->m_buf ;
	dbuf = (unsigned char *)m_buf ;

	bFx = (FlipMask & FLIP_X) ? true : false ;
	bFy = (FlipMask & FLIP_Y) ? true : false ;

	for (y=0; y<m_h; y++)
	{
		if (bFy)
			iy = (m_h - y - 1) * m_w ;
		else
			iy = y * m_w;
		my = y*m_w ;

		for (x=0; x<m_w; x++)
		{
			if (bFx)
				ix = m_w - x - 1 ;
			else
				ix = x ;

			switch (m_nStepSize)
			{
			case 1:
				dbuf[x+my] = sbuf[ix+iy] ;
				break;
			case 4:
				((int*)m_buf)[x+my] = ((int*)pBuf->m_buf)[ix + iy] ;
				break;
			default:
				//ASSERT(false);
				break;
			}
		}
	}
}



long CPixelBuffer::CopyBorder   ( CPixelBuffer &pbuf
							    , int w
							    , int h )
{
    if (  m_w != pbuf.m_w + w*2 
       || m_h != pbuf.m_h + h*2
       || m_nStepSize != pbuf.m_nStepSize )
		return -1;

    if ( m_nStepSize==1 )
    {
		if ( (w%4) || (m_w%4) )
			return -2;
	}

    long i ;
	long nLen = pbuf.m_w ;
	long nIncDst = m_w * m_nStepSize;
    long nIncSrc = pbuf.m_w * m_nStepSize ;

	unsigned char * pSrc = (unsigned char *)pbuf.m_buf;
	unsigned char * pDst = (unsigned char *)m_buf;

	pDst += (nIncDst * h) + (w * m_nStepSize);

    for (i=h; i<m_h-h; i++)
	{
		memcpy ( pDst, pSrc, nLen ) ;
		pDst += nIncDst ;
		pSrc += nIncSrc ;
	}

    return 0 ;
}




long CPixelBuffer::CopyBorderGpu( CPixelBuffer &pbuf
							    , int w
							    , int h )
{
	// This enforces use of the GPU at least on the
	// destination buffer.  It can copy from a
	// regular buffer.
	printf("CopyBorderGpu m_bUseGpu       = 0x%08X\n", m_bUseGpu) ;
	printf("CopyBorderGpu m_pArmData      = 0x%08X\n", m_pArmData) ;
	printf("CopyBorderGpu m_buf           = 0x%08X\n", m_buf) ;
    printf("CopyBorderGpu pbuf.m_bUseGpu  = 0x%08X\n", pbuf.m_bUseGpu) ;
    printf("CopyBorderGpu pbuf.m_pArmData = 0x%08X\n", pbuf.m_pArmData) ;
    printf("CopyBorderGpu pbuf.m_buf      = 0x%08X\n", pbuf.m_buf) ;

    if ( m_bUseGpu == false )
		return -3 ;

    if (  m_w != pbuf.m_w + w*2 
       || m_h != pbuf.m_h + h*2
       || m_nStepSize != pbuf.m_nStepSize )
		return -1;

    if ( m_nStepSize==1 )
    {
		if ( (w%4) || (m_w%4) )
			return -2;
	}

    long i ;
	long nLen = pbuf.m_w ;
	long nIncDst = m_w * m_nStepSize;
    long nIncSrc = pbuf.m_w * m_nStepSize ;
	long rval ;

	unsigned char * pSrc ;

	// We use the normal pointer if the source is non-gpu
	if ( pbuf.m_bUseGpu == false )
	{
		pSrc = (unsigned char *)pbuf.m_buf;
		rval = 1;
	}
	else
	{
		pSrc = (unsigned char *)pbuf.m_pArmData ;
		rval = 2 ;
	}

	// We always use the GPU pointer for the destination
	unsigned char * pDst = (unsigned char *)m_pArmData ;

	pDst += (nIncDst * h) + (w * m_nStepSize);

    for (i=h; i<m_h-h; i++)
	{
		memcpy ( pDst, pSrc, nLen ) ;
		pDst += nIncDst ;
		pSrc += nIncSrc ;
	}

    return rval ;
}



void CPixelBuffer::Copy( CPixelBuffer &pbuf
					   , int l
					   , int t
					   , int r
					   , int b
					   , BOOL bIgnoreCheck)
{
	if (l==-1 && t==-1 && r==-1 && b==-1)
	{
		if (!Init(pbuf.m_w, pbuf.m_h, pbuf.m_nStepSize))
			return ;

		int numelem = m_w*m_h * m_nStepSize ;
		void *psrc = pbuf.m_buf ;
		void *pdst = m_buf ;
		memcpy (m_buf, pbuf.m_buf, numelem) ;
	}
	else
	{
		// This assumes that we copying into a preexisting bitmap buffer
		int sx, sy, dx, dy ;

		if (l<0)
			l = 0 ;
		if (t<0)
			t = 0 ;
		if (r<0)
			r = l+pbuf.m_w ;
		if (b<0)
			b = t+pbuf.m_h ;
		
		if (m_nStepSize==sizeof(int))
		{
			for (sy=0,dy=t; sy<b-t; sy++,dy++)
			{
				for (sx=0,dx=l; sx<r-l; sx++,dx++)
				{
					SetAt(dx, dy, pbuf.GetAt(sx,sy)) ;
				}
			}
		}
	    else
	    if (m_nStepSize==1)
		{
		}
	}
}

void CPixelBuffer::ThresholdMask(CPixelBuffer *pbuf, CPixelBuffer *pmask, int threshval, int replaceval) 
{
    //ASSERT(pbuf);
	//ASSERT(pbuf->m_nStepSize==4) ;

    if (!Init(pbuf->m_w, pbuf->m_h, 4))
        return;

    int numelem = m_w*m_h ;
    int            i ;

	if (replaceval==0)
	{
		for (i=0; i<numelem; i++)
			m_buf[i] = (pbuf->m_buf[i] >= threshval+pmask->m_buf[i]) ? pbuf->m_buf[i] : 0 ;
	}
	else
	{
		for (i=0; i<numelem; i++)
			m_buf[i] = (pbuf->m_buf[i] >= threshval+pmask->m_buf[i]) ? replaceval : 0 ;
	}
}


void CPixelBuffer::Threshold(CPixelBuffer *pbuf, int threshval, int replaceval) 
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    int numelem = m_w*m_h ;
    int            i ;

	if (replaceval==0)
	{
		for (i=0; i<numelem; i++)
			m_buf[i] = (pbuf->m_buf[i] >= threshval) ? pbuf->m_buf[i] : 0 ;
	}
	else
	{
		for (i=0; i<numelem; i++)
			m_buf[i] = (pbuf->m_buf[i] >= threshval) ? replaceval : 0 ;
	}
}

// Init initializes the memory of the buffer.  If the bits
// are not set (i.e. == -1), the buffer is initialized to
// whatever previous bitsize existed.  If the previous bitsize
// was not yet set, it initializes it to 32 bits.

BOOL CPixelBuffer::Init(int width, int height, int nStepSize)
{
    if (width<=0 || height<=0 || nStepSize<=0 || nStepSize>4)
        return false;

    if (width!=m_w || height!=m_h || nStepSize!=m_nStepSize)
    {
		if (m_bUseGpu)
		{
			printf("CPixelBuffer::Init failed.  Tried to initialize while already allocated for GPU use\n") ;
			return false ;
		}

        m_w = width ;
        m_h = height ;
		m_nStepSize = nStepSize ;

        if (m_buf)
		{
	    	delete [] m_buf ;
		}
        m_size = m_w * m_h ;

		{
            long nRealSize = (m_size*m_nStepSize)/4 + (((m_size*m_nStepSize)%4) ? 1 : 0) ;
	   		m_buf = (int*) new long [nRealSize] ;
		}
        Clear() ;
    }

    return true ;
}


BOOL CPixelBuffer::Init(CPixelBuffer &src)
{
	Init ( src.m_w, src.m_h ) ;
    return true ;
}

BOOL CPixelBuffer::Init(CSize newsize, int nStepSize)
{
	return Init(newsize.cx, newsize.cy, nStepSize) ;
}


int CPixelBuffer::GetBufferSize()
{
    return m_size * m_nStepSize;
}


void CPixelBuffer::ConvertBayer(unsigned char *pbuf, int w, int h)
{
    //ASSERT(pbuf);
    int x, y, i, bi, bw ;
	int   r, g, b, rval;

    // This forces is to an integer value!
    if (!Init(w/2, h/2))
        return;

    i = bi = 0 ;
    bw = w ;
    for (y=0; y<m_h; y++)
    {
        for (x=0; x<m_w; x++)
        {
            // Convert bayer pattern to gray
            // pixel!
            r = (int)(pbuf[i+1]) ;
            g = (int)(pbuf[i] + pbuf[bw+i+1]) ;
            b = (int)(pbuf[bw+i]) ;
            rval = (g*30 + r* 30 + b*10) / 100 ;

            if (rval > 255)
                rval=255 ;
            m_buf[bi] = rval ;
            i+=2;
            bi++;
        }
        i+=bw ;
    }
}


void CPixelBuffer::ConvertBayer(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    int x, y, i, bi, bw ;
	int   r, g, b, rval;

    // This forces is to an integer value!
    if (!Init(pbuf->m_w/2, pbuf->m_h/2))
        return;

    i = bi = 0 ;
    bw = pbuf->m_w ;
    for (y=0; y<m_h; y++)
    {
        for (x=0; x<m_w; x++)
        {
            // Convert bayer pattern to gray
            // pixel!
            r = pbuf->m_buf[i+1] ;
            g = pbuf->m_buf[i] + pbuf->m_buf[bw+i+1] ;
            b = pbuf->m_buf[bw+i] ;
            rval = (g*30 + r* 30 + b*10) / 100 ;

            if (rval > 255)
                rval=255 ;
            m_buf[bi] = rval ;
            i+=2;
            bi++;
        }
        i+=bw ;
    }
}

void CPixelBuffer::ConvertBayerColor(CPixelBuffer *pbuf,BOOL HiByte)
{
    //ASSERT(pbuf);
    int x, y, i, bi, bw ;
	int   r, g, b, rval;
	unsigned char *pPixBuf ;

	if (pbuf->m_nStepSize!=1)
		return;

    // This forces it to an integer value!
    if (!Init(pbuf->m_w/2, pbuf->m_h/2, 4))
        return;

    i = bi = 0 ;
    bw = pbuf->m_w ;
	pPixBuf = (unsigned char *)pbuf->m_buf ;

	if (HiByte)
	{
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++)
			{
				// Convert bayer pattern to color
				// pixel!
				b = pPixBuf[i] ;
				g = pPixBuf[i+1] ;
				r = pPixBuf[bw+i+1] ;
				rval = r + (g<<8) + (b<<16) ;

				m_buf[bi] = rval ;
				i+=2;
				bi++;
			}
			i+=bw ;
		}
	}
	else
	{
		i=2;
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++)
			{
				// Convert bayer pattern to color
				// pixel!
				g = pPixBuf[i] ;
				b = pPixBuf[i-1] ;
				r = pPixBuf[bw+i] ;
				rval = r + (g<<8) + (b<<16) ;

				m_buf[bi] = rval ;
				i+=2;
				bi++;
			}
			i+=bw ;
		}
	}
}

void CPixelBuffer::Enlarge(CPixelBuffer *src, int mult)
{
	int i, sz, j, x, y, ci, k ;

	if (!Init(src->m_w*mult, src->m_h*mult))
	    return;
	
	sz = mult*mult*m_w*m_h ;

	i  = 0 ;
	ci = 0 ;
	for (y=0; y<src->m_h; y++)
	{
		for (j=0; j<mult; j++)
		{
			i = y * src->m_w ;

			for (x=0; x<src->m_w; x++)
			{
				for (k=0; k<mult; k++)
				{
					m_buf[ci] = src->m_buf[i] ;
					ci++;
				}
				i++;
			}
		}

	}
}

void CPixelBuffer::Reduce(CPixelBuffer *src, int mult)
{
	int i, x, y, ci ;

	if (!Init(src->m_w/mult, src->m_h/mult, src->m_nStepSize))
	    return;
	
	if (m_nStepSize==1)
	{
		i = 0 ;
		for (y=0; y<src->m_h; y+=mult)
		{
			ci = y * src->m_w ;

			for (x=0; x<src->m_w; x+=mult)
			{
				((char*)m_buf)[i] = ((char*)(src->m_buf))[ci] ;
				ci+=mult;
				i++;
			}
		}
	}
	else
	if (m_nStepSize==4)
	{
		i = 0 ;
		for (y=0; y<src->m_h; y+=mult)
		{
			ci = y * src->m_w ;

			for (x=0; x<src->m_w; x+=mult)
			{
				((int*)(m_buf))[i] = ((int*)(src->m_buf))[ci] ;
				ci+=mult;
				i++;
			}
		}
	}

}


void CPixelBuffer::CopyNewDepth(CPixelBuffer &src, int newdepth)
{
    if (!Init (src.m_w, src.m_h, newdepth))
        return;
	
	int i, sz = src.m_w * src.m_h ;
	DWORD *pdwdest = (DWORD*)m_buf;
	unsigned short *psdest = (unsigned short *)m_buf ;
	unsigned char *pcdest = (unsigned char *)m_buf;

	DWORD *pdwsrc = (DWORD*)src.m_buf;
	unsigned short *pssrc = (unsigned short *)src.m_buf ;
	unsigned char *pcsrc = (unsigned char *)src.m_buf;
	
	if (src.m_bUseGpu)
	{
		pdwsrc = (DWORD*)src.m_pArmData;
		pssrc = (unsigned short *)src.m_pArmData ;
		pcsrc = (unsigned char *)src.m_pArmData ;
	}
	else
	{
		pdwsrc = (DWORD*)src.m_buf;
		pssrc = (unsigned short *)src.m_buf ;
		pcsrc = (unsigned char *)src.m_buf;
	}

	DWORD vd ;
	unsigned short vs ;
	unsigned char vc ;

	switch (m_nStepSize)
	{
		case 1:
			switch (src.m_nStepSize)
			{
				case 1:
					for (i=0; i<sz; i++, vc = *pcsrc, *pcdest = vc, pcsrc++, pcdest++);
					break;
				case 2:
					for (i=0; i<sz; i++, vc = (unsigned char)*pssrc, *pcdest = vc, pssrc++, pcdest++);
					break;
				case 4:
					for (i=0; i<sz; i++, vc = (unsigned char)*pdwsrc, *pcdest = vc, pdwsrc++, pcdest++);
					break;
			}
			break;


		case 2:
			switch (src.m_nStepSize)
			{
				case 1:
					for (i=0; i<sz; i++, vs = *pcsrc, *psdest = vs, pcsrc++, psdest++);
					break;
				case 2:

					for (i=0; i<sz; i++, vs = *pssrc, *psdest = vs, pssrc++, psdest++);
					break;
				case 4:
					for (i=0; i<sz; i++, vs = (unsigned short)*pdwsrc, *psdest = vs, pdwsrc++, psdest++);
					break;
			}
			break;
		case 4:
			switch (src.m_nStepSize)
			{
				case 1:
					for (i=0; i<sz; i++, vd = *pcsrc, *pdwdest = vd, pcsrc++, pdwdest++);
					break;
				case 2:
					for (i=0; i<sz; i++, vd = *pssrc, *pdwdest = vd, pssrc++, pdwdest++);
					break;
				case 4:
					for (i=0; i<sz; i++, vd = *pdwsrc, *pdwdest = vd, pdwsrc++, pdwdest++);
					break;
			}
			break;
	}
}

#ifdef _dib_section_h
BOOL CPixelBuffer::CopyDib(CDibSection &source, int mask)
{
	int  x ;
	int  y ;
	long *pData ;
	unsigned char *pSrc ;

	if (!source.Width() || !source.Height())
		return false;
	if (!Init(source.Width(), source.Height(), 4))
		return false;
	
	pData = (long*)m_buf ;
	pSrc = source.m_bitsBase ;
	for (y=0; y<source.Height(); y++)
	{
		for (x=0; x<source.Width(); x++)
		{
			*pData = (long) pSrc[x*3] ;
			pData++ ;
		}
		pSrc += source.m_widthbytes ;
	}

	return true ;
}
#endif 

void CPixelBuffer::Copy(unsigned char *buf, int width, int height, int nStepSize)
{
    int w, h, i ;
    int numelem ;

    i=0;
    w = width ;
    if (w<=0)
        w = m_w ;
    h = height ;
    if (h<=0)
        h = m_h ;

    if (!w || !h)
        return;
    if (!Init (w, h, nStepSize))
        return;

    numelem = m_w * m_h ;

	memcpy ( m_buf, buf, numelem ) ;
}


void CPixelBuffer::Clear(int cvalue)
{
	int i, j ;
    if (m_size && cvalue==0)
	{
        memset (m_bUseGpu ? (long*)m_pArmData : (long*)m_buf, 0, m_size*m_nStepSize);
	}
	else
	if (m_size)
	{
		switch (m_nStepSize)
		{
		case 1:
			{
				printf ("Enter clear 1 byte per pixel 0x%08X\n", cvalue) ;
			    printf ("   m_bUseGpu  = %d\n", m_bUseGpu ) ;
				printf ("   m_pArmData = 0x%08X\n", m_pArmData) ;
				printf ("   m_buf      = 0x%08X\n", m_buf) ;

				unsigned char *pbuf = (m_bUseGpu ? (unsigned char *)m_pArmData : (unsigned char *)m_buf);

				printf ("   pbuf       = 0x%08X\n", pbuf) ;
				
				j = 0 ;
				for (i=0; i<m_size; i++)
				{
					pbuf[i] = (unsigned char)cvalue ;
					if (i%m_w==0)
					{
						printf(".") ;
						j++ ;
						if (j%32==0)
							printf("\n") ;
					}
				}
				printf("\n\n") ;

				printf ("Exit clear 1 byte per pixel 0x%08X\n", cvalue) ;
			}
			break;

		case 2:
			{
				printf ("Enter clear 2 bytes per pixel 0x%08X\n", cvalue) ;

				unsigned short *pbuf = (m_bUseGpu ? (unsigned short *)m_pArmData : (unsigned short *)m_buf);

				for (i=0; i<m_size; i++)
				{
					pbuf[i] = (unsigned short)cvalue ;
				}
				printf ("Exit clear 2 byte per pixel 0x%08X\n", cvalue) ;
			}
			break;

		case 4:
			{
				printf ("Enter clear 4 bytes per pixel 0x%08X\n", cvalue) ;

				long *pbuf = (m_bUseGpu ? (long *)m_pArmData : (long *)m_buf) ;

				for (i=0; i<m_size; i++)
				{
					pbuf[i] = (long)cvalue ;
				}
				printf ("Exit clear 4 byte per pixel 0x%08X\n", cvalue) ;
			}
			break;
		}
	}
}


void CPixelBuffer::CopyResultToBuf()
{
}    


int * CPixelBuffer::GetBuffer()
{
    return m_buf ;
}


int *CPixelBuffer::GetResult()
{
    return NULL;
}

/*
BOOL CPixelBuffer::FromDisk(LPCTSTR filename)
{
    CStdioFile f ;
    CString    tstr, nstr ;
    int        x, y, i, start, end ;

    if (f.Open(filename, CFile::typeText | CFile::modeReadWrite))
    {
        // figure out how many x and y coordinates there are first
        x = 0 ;
        y = 0 ;
        while (f.ReadString(tstr))
        {
            if (y==0)
            {
                start = 0 ;
                while (start >= 0)
                {
                    start = tstr.Find(",", start);
                    if (start>=0)
                    {
                        x++ ;
                        start++ ;
                    }
                }
            }
            y++ ;
        }

        // Now read in the actual values!
        f.Seek(0, CFile::begin) ;
        if (!Init(x, y))
        {
            f.Close();
            return false;
        }

        i = 0 ;

        for (y=0; y<m_h; y++)
        {
            f.ReadString(tstr) ;
            start = end = 0 ;
            for (x=0; x<m_w; x++)
            {
                end = tstr.Find(",", start) ;
                nstr = tstr.Mid(start, end-start) ;
                start = end + 1 ;

                m_buf[i] = atoi(nstr) ;

                i++;
            }
        }
        f.Close() ;
        return true ;
    }
    else
        return false ;
}
*/

long CPixelBuffer::ToDisk(const char *filename, bool FormatForExcel )
{	
    FILE	   *f ;
    CString    tstr, tempfilename ;
    int        x, y, i, j ;

	// If we didn't get a file name, then choose a directory to save it...
	if (filename==NULL)
		return -3 ;

	f = fopen ( filename, "w+" ) ;
	if ( f==NULL )
		return -2 ;

	fprintf (f, "UseGPU = %d\n\n", m_bUseGpu) ;
	if (m_bUseGpu)
	{
		if (m_pArmData==0)
			return -4 ;
	}
	else
	if (m_buf==0)
		return -5 ;

	for (y=0; y<m_h; y++)
	{
		if (y==0)
		{
			// Print the x header
			if (m_nStepSize==1)
			{
				fprintf (f, "    ") ;
				for (x=0; x<m_w; x+=4)
					fprintf (f, "  %04d   ", x) ;
				fprintf (f, "\n\n" ) ;
			}
			else
			if (m_nStepSize==4)
			{
				fprintf (f, "    ") ;
				for (x=0; x<m_w*4; x+=4)
					fprintf (f, "  %04d   ", x) ;
				fprintf (f, "\n\n" ) ;
			}
		}

        fprintf(f, "%03d ", y) ;
	    
		if (m_nStepSize==1)
		{
	        for ( x=0;   x<(m_w/4);   x++ )
	        { 
				if (m_bUseGpu)
	    	        fprintf(f, "%08X ", __bswap_32(m_pArmData[y*(m_w/4)+x])) ;
				else
					fprintf(f, "%08X ", __bswap_32(m_buf[y*(m_w/4)+x])) ;
		    }
		}
		else
		if (m_nStepSize==4)
		{
			for ( x=0;  x<m_w; x++)
			{
				if (m_bUseGpu)
	    	        fprintf(f, "%08X ", __bswap_32(m_pArmData[y*(m_w)+x])) ;
				else
					fprintf(f, "%08X ", __bswap_32(m_buf[y*(m_w)+x])) ;
			}
		}
		
		fprintf(f, "\n") ;
	}
    
	fclose (f) ;
	return 0 ;
}

// This generates a VRML surface out of a set of points...
/*
void CPixelBuffer::ToDiskVrml(LPCTSTR filename, int left, int top, int right, int bottom)
{
    if (!m_buf)
        return;
    
    CStdioFile f ;
    CString    tstr, tempfilename ;
    int        x, y, i ;

	// If we didn't get a file name, then choose a directory to save it...
	if (filename==NULL)
	{
		static char BASED_CODE szFilter[] = "Envisic Scan Files (*.esf)|*.esf|All Files (*.*)|*.*||";
		CFileDialog fd ( false          // save type dialog
			, "esf"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return;

		tempfilename = fd.GetPathName() ;
		filename = tempfilename ;
	}

	if (left==0 && top==0 && right==0 && bottom==0)
	{
		right = m_w ;
		bottom = m_h ;
	}
	else
	{
		right++ ;
		bottom++ ;
	}

    if (f.Open(filename, CFile::modeCreate | CFile::modeReadWrite))
    {
		f.WriteString("#VRML V1.0 ascii\n");
		f.WriteString("\n");
		f.WriteString("# This VRML file was written by a Caligari (tm) product.\n");
		f.WriteString("\n");
		f.WriteString("Separator \n");
		f.WriteString("{\n");
		f.WriteString("        Background {\n");
		f.WriteString("            fields [ MFColor skyColors ]\n");
		f.WriteString("            skyColors        0.498 0.498 0.498\n");
		f.WriteString("         }\n");
		f.WriteString("        Switch {\n");
		f.WriteString("            whichChild 0\n");
		f.WriteString("            DEF Camera_def PerspectiveCamera {\n");
		f.WriteString("                position 6.59 8.09 -0.777\n");
		f.WriteString("                orientation 0.286 -0.679 -0.33 -1.88\n");
		f.WriteString("                heightAngle 0.761\n");
		f.WriteString("             }\n");
		f.WriteString("         }\n");
		f.WriteString("        Separator {\n");
		f.WriteString("            DEF LocLight PointLight {\n");
		f.WriteString("                color 1 1 1\n");
		f.WriteString("                on true\n");
		f.WriteString("                location -0.103 2.59 1.98\n");
		f.WriteString("                intensity 0.5\n");
		f.WriteString("             }\n");
		f.WriteString("         }\n");
		f.WriteString("        Separator {\n");
		f.WriteString("            DEF LocLight_1 PointLight {\n");
		f.WriteString("                color 1 1 1\n");
		f.WriteString("                on true\n");
		f.WriteString("                location 0.579 2.32 -3.85\n");
		f.WriteString("                intensity 0.5\n");
		f.WriteString("             }\n");
//		f.WriteString("         }\n");
		f.WriteString("    ShapeHints \n");
		f.WriteString("    {\n");
		f.WriteString("        vertexOrdering CLOCKWISE\n");
		f.WriteString("        shapeType   SOLID\n");
		f.WriteString("        faceType    CONVEX\n");
		f.WriteString("    }\n");
		f.WriteString("    Separator \n");
		f.WriteString("    {\n");
		f.WriteString("        Coordinate3 \n");
		f.WriteString("        {\n");
		f.WriteString("		    point   [ \n");

		for (y=top; y<bottom; y++)
		{
			// First, write the peak
			for (x=left; x<right; x++)
			{
				DOUBLE rx, ry, rz ;
				i = m_w * y + x ;

				rx = ((DOUBLE)(x - left) / (DOUBLE)(right - left - 1) - (DOUBLE)0.5)  * (DOUBLE)5.0;
				ry = ((DOUBLE)(y - top) / (DOUBLE)(bottom - top - 1) - (DOUBLE)0.5)  * (DOUBLE)5.0;
				rz = (DOUBLE)m_buf[i] / (DOUBLE)255.0 ;

				tstr.Format("                        %lg %lg %lg,\n", rx, rz, ry) ;
				f.WriteString(tstr) ;
			}
			f.WriteString("\n") ;
		}

		f.WriteString("            ]\n");
		f.WriteString("        }\n");
		f.WriteString("        NormalBinding \n");
		f.WriteString("        {\n");
		f.WriteString("            value   PER_VERTEX_INDEXED\n");
		f.WriteString("        }\n");
		f.WriteString("\n");
		f.WriteString("        Material \n");
		f.WriteString("        {\n");
		f.WriteString("            diffuseColor    1 1 1\n");
		f.WriteString("        }\n");
		f.WriteString("        \n");
		f.WriteString("        MaterialBinding \n");
		f.WriteString("        {\n");
		f.WriteString("            value   OVERALL\n");
		f.WriteString("        }\n");
		f.WriteString("\n");
		f.WriteString("        Group \n");
		f.WriteString("        {\n");
		f.WriteString("            Separator \n");
		f.WriteString("            {\n");
		f.WriteString("                IndexedFaceSet \n");
		f.WriteString("                {\n");
		f.WriteString("                    coordIndex [ \n");
		int w, h ;

		h = bottom - top ;
		w = right - left ;
		for (y=0; y<(h - 1); y++)
		{
			for (x=0; x<(w - 1); x++)
			{
				int idx ;

				idx = x + y * w ;
				tstr.Format("                        %d, %d, %d, %d, -1,\n"
						   , idx, idx+1, idx+w+1, idx+w) ;
				f.WriteString(tstr) ;
			}
		}

		f.WriteString("                     ]\n");
		f.WriteString("                }\n");
		f.WriteString("            }\n");
		f.WriteString("        }\n");
		f.WriteString("    }\n");
		f.WriteString("}\n");



    }
}
*/

/*
static int CrossMask7x7[49] = 
{
-1, -1, -1,  1, -1, -1, -1,
-1, -1, -1,  1, -1, -1, -1,
-1, -1, -1,  1, -1, -1, -1,
 1,  1,  1,  1,  1,  1,  1,      
-1, -1, -1,  1, -1, -1, -1,
-1, -1, -1,  1, -1, -1, -1,
-1, -1, -1,  1, -1, -1, -1
};
static int CrossMask7x7[49] = 
{
 1, -1, -1, -1, -1, -1,  1,
-1,  1, -1, -1, -1,  1, -1,
-1, -1,  1, -1,  1, -1, -1,
-1, -1, -1,  1, -1, -1, -1,      
-1, -1,  1, -1,  1, -1, -1,
-1,  1, -1, -1, -1,  1, -1,
 1, -1, -1, -1, -1, -1,  1
};
*/

// sum = 128
static int GaussianMask7x7[49] = 
{
1,  1,  2,  2,  2,  1,  1,
1,  2,  2,  3,  2,  2,  1,
2,  2,  4,  7,  4,  2,  2,
2,  3,  7,  12, 7,  3,  2,      
2,  2,  4,  7,  4,  2,  2,
1,  2,  2,  3,  2,  2,  1,
1,  1,  2,  2,  2,  1,  1
};

// This is the 3 x 3 mask!
// sum = 132
static int GaussianMask3x3[9] =
{
1,  4,  1,
4,  12, 4,    
1,  4,  1
};

static int GsOfs[49] ;


void CPixelBuffer::Gaussian7x7(CPixelBuffer *psource)
{
/*
    //ASSERT(psource);
    int offsetStart0, offsetStart1, offsetStart2, offsetStart3, offsetStart4, offsetStart5, offsetStart6 ;
    int *srcbuf, *destbuf;
    int w, h, bw ;

    // Verify that the buffers are valid
    if (!psource)
        return ;
    if (!psource->GetBuffer())
        return;
    if (!Init(psource->Width(), psource->Height()))
        return;

    w = m_w ;
    bw = m_w*sizeof(int);
    h = m_h ;

    // This is a fast lookup table...  Same as doing an array, but makes it more visual
    offsetStart0=(-3*w - 3);
    offsetStart1=(-2*w - 3);
    offsetStart2=(-1*w - 3);
    offsetStart3=(     -3);
    offsetStart4=( 1*w - 3);
    offsetStart5=( 2*w - 3);
    offsetStart6=( 3*w - 3);

    // Other startup initialization
    srcbuf = psource->m_buf;
    destbuf = m_buf ;

    h-=6;
    w-=6;

    _asm
    {
        mov edi, destbuf
        mov esi, srcbuf

        // Get our starting point int the pixel buffers...
        add edi, bw
        add edi, bw
        add edi, bw
        add edi, 12

        add esi, bw
        add esi, bw
        add esi, bw
        add esi, 12

    outer_gauss:

            mov edx, w

        inner_gauss:
                mov eax, 0
                // Start the convolution
                // Row 0
                mov ebx, offsetStart0
                add eax, [esi+ebx*4]     // 1
                add eax, [esi+ebx*4+4]   // 1
                add eax, [esi+ebx*4+8]   // 2
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]  // 2
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]  // 2
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 1
                add eax, [esi+ebx*4+24]   // 1

                // Row 1
                mov ebx, offsetStart1

                add eax, [esi+ebx*4]     // 1
                add eax, [esi+ebx*4+4]   // 2
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+8]   // 2
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]   // 3
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]   // 2
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 2
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+24]   // 1

                // Row 2
                mov ebx, offsetStart2

                add eax, [esi+ebx*4]     // 2
                add eax, [esi+ebx*4]     
                add eax, [esi+ebx*4+4]   // 2
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+8]   // 4
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]   // 7
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]   // 4
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 2
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+24]   // 2
                add eax, [esi+ebx*4+24]   

                // Row 3
                mov ebx, offsetStart3

                add eax, [esi+ebx*4]     // 2
                add eax, [esi+ebx*4]     
                add eax, [esi+ebx*4+4]   // 3
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+8]   // 7
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]   // 12
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]   // 7
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 3
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+24]   // 2
                add eax, [esi+ebx*4+24]   

                // Row 4
                mov ebx, offsetStart4

                add eax, [esi+ebx*4]     // 2
                add eax, [esi+ebx*4]     
                add eax, [esi+ebx*4+4]   // 2
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+8]   // 4
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]   // 7
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]   // 4
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 2
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+24]   // 2
                add eax, [esi+ebx*4+24]   

                // Row 5
                mov ebx, offsetStart5

                add eax, [esi+ebx*4]     // 1
                add eax, [esi+ebx*4+4]   // 2
                add eax, [esi+ebx*4+4]   
                add eax, [esi+ebx*4+8]   // 2
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]   // 3
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]   // 2
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 2
                add eax, [esi+ebx*4+20]   
                add eax, [esi+ebx*4+24]   // 1

                // Row 6
                mov ebx, offsetStart6

                add eax, [esi+ebx*4]     // 1
                add eax, [esi+ebx*4+4]   // 1
                add eax, [esi+ebx*4+8]   // 2
                add eax, [esi+ebx*4+8]   
                add eax, [esi+ebx*4+12]  // 2
                add eax, [esi+ebx*4+12]   
                add eax, [esi+ebx*4+16]  // 2
                add eax, [esi+ebx*4+16]   
                add eax, [esi+ebx*4+20]   // 1
                add eax, [esi+ebx*4+24]   // 1

                // Store the result
				shr eax, 7
                mov [edi], eax

                // Go to the next pixel
                add edi, 4
                add esi, 4

                // Are we done with the cols?
                dec edx
                jnz inner_gauss

            // Offset to the beginning of the
            // next row
            add edi, 24 
            add esi, 24

            // Are we done with the rows?
            dec h
            jnz outer_gauss
    }

	// Now, do the outside strips.  This adds a little time to
	// the processing, but certainly not as much as the main section
	// of processing
	int x, y ;
	for (x=0; x<m_w; x++)
	{
		Gaussian7x7Kernel(psource,x,0) ;
		Gaussian7x7Kernel(psource,x,1) ;
		Gaussian7x7Kernel(psource,x,2) ;
		Gaussian7x7Kernel(psource,x,m_h-3) ;
		Gaussian7x7Kernel(psource,x,m_h-2) ;
		Gaussian7x7Kernel(psource,x,m_h-1) ;
	}
	for (y=3; y<m_h-3; y++)
	{
		Gaussian7x7Kernel(psource,0,y) ;
		Gaussian7x7Kernel(psource,1,y) ;
		Gaussian7x7Kernel(psource,2,y) ;
		Gaussian7x7Kernel(psource,m_w-3, y) ;
		Gaussian7x7Kernel(psource,m_w-2, y) ;
		Gaussian7x7Kernel(psource,m_w-1, y) ;
	}
*/
}

void CPixelBuffer::Gaussian7x7Kernel(CPixelBuffer *pbuf, int x, int y) 
{
	int xi, yi, srcind, maskind, sum, divisor, destind, maskval ;

	destind = Index(x,y) ;
	if (destind<0)
		return;

	divisor = 0 ;
	sum = 0 ;
	maskind = 0 ;
	for (yi=-3; yi<=3; yi++)
	{
		for (xi=-3; xi<=3; xi++, maskind++)
		{
			srcind = Index(x+xi, y+yi) ;
			if (srcind>=0)
			{
				maskval = GaussianMask7x7[maskind] ;
				sum += pbuf->m_buf[srcind] * maskval ;
				divisor += maskval ;
			}
		}
	}

	if (divisor>0)
		m_buf[destind] = sum/divisor ;
	else
		m_buf[destind] = 0 ;
}

int *g_pIntensityBuffer ;

int BufferIntensityIndexSort(const void *elem1, const void *elem2)
{
	int p1 = *((int*)elem1) ;
	int p2 = *((int*)elem2) ;

	if (g_pIntensityBuffer[p1]>g_pIntensityBuffer[p2])
		return 1 ;
	else
	if (g_pIntensityBuffer[p1]<g_pIntensityBuffer[p2])
		return -1 ;

	return 0 ;
}

void CPixelBuffer::ThinSort(int nThreshold)
{
/*
	int y, i, v, nNumChanges, nNumIndeces ;
	CPixelBuffer TempCopy ;
	int *pdata ;
	int *pIndeces ;
	int nSize = m_size ;
	int *pSrc = m_buf ;

	if (m_size<=0)
		return ;

	pIndeces = new int [m_size] ;
	if (!pIndeces)
		return ;

	nNumIndeces = 0 ;
	{
		_asm
		{
			mov ecx, nSize			// load the count

			mov esi, pSrc
			mov edi, pIndeces
			mov ebx, nThreshold
			mov eax, 0
			mov edx, 0

another_thin_sort:
			cmp ebx, [esi]			// Is this above the threshold for sorting?
			jle do_thin_sort_set    //     if so, add it to the list to sort
			mov [esi], 0
			add esi, 4
			inc eax
			dec ecx
			jnz another_thin_sort
			jmp done_thin_sort

do_thin_sort_set:
			mov [edi], eax
			add edi, 4
			add esi, 4
			inc eax
			inc edx
			dec ecx
			jnz another_thin_sort

done_thin_sort:
			mov nNumIndeces, edx
		}
//		for (i=0; i<m_size; i++)
//		{
//			if (m_buf[i] < nThreshold)
//				m_buf[i] = 0 ;
//			else
//			{
//				pIndeces[nNumIndeces] = i ;
//				nNumIndeces++;
//			}
//		}
	}

	// The indeces above the threshold will be smaller in number
	// than the indeces exactly at the threshold.  We can do a
	// reverse loop of the indeces and keep pushing index values
	// higher than the threshold to the top.  This will be a 
	// preliminary sort and needs to go through the list only 
	// once to prepare the list.
	// 

	g_pIntensityBuffer = m_buf ;
	{
		qsort(pIndeces,nNumIndeces,sizeof(int),BufferIntensityIndexSort) ;
	}

	{
		do
		{
			nNumChanges = 0 ;
			for (y=0; y<nNumIndeces; y++)
			{
				i = 0 ;

				pdata = m_buf + pIndeces[y] ;
				v = *pdata ;
				if (v)
				{
					*pdata = 255 ;
					i += (pdata[-m_w-1] ? 1   : 0 ) ;
					i += (pdata[-m_w]   ? 2   : 0 ) ;
					i += (pdata[-m_w+1] ? 4   : 0 ) ;
					i += (pdata[	 1]	? 8   : 0 ) ;
					i += (pdata[ m_w+1] ? 16  : 0 ) ;
					i += (pdata[ m_w  ] ? 32  : 0 ) ;
					i += (pdata[ m_w-1] ? 64  : 0 ) ;
					i += (pdata[	-1] ? 128 : 0 ) ;

					if (bCanDeletePixel[i]) //ErMask[i])
					{
						*pdata = 0 ;
						nNumChanges++ ;
					}	
				}
			}
		}
		while (nNumChanges);
	} 

	delete pIndeces ;
*/
}

void CPixelBuffer::NeighborCopy ( CPixelBuffer *pSrcBuf
								, int nMinNeighbors 
								, int nReplaceVal)
{
    int i, nMinInd, nMaxInd ;
    int *pbuf ;

    pbuf = (int *)pSrcBuf->GetBuffer() ;
    if (!pbuf)
        return;

	if (pSrcBuf->m_nStepSize!=4)
		return;

	if (!Init(pSrcBuf->m_w, pSrcBuf->m_h)) 
		return ;

	nMinInd = m_w + 1 ;
	nMaxInd = m_size - m_w - 1 ;
	for (i=0; i<m_size; i++)
	{
		m_buf[i] = 0 ;
		if (pbuf[i] && i>nMinInd && i<nMaxInd)
		{
			// Count the neighbors
			int val1     = (pbuf[i-m_w-1] ? 1 : 0)
						 + (pbuf[i-m_w]   ? 1 : 0)
						 + (pbuf[i-m_w+1] ? 1 : 0)
						 + (pbuf[i+1]     ? 1 : 0)
						 + (pbuf[i+m_w+1] ? 1 : 0)
						 + (pbuf[i+m_w]   ? 1 : 0)
						 + (pbuf[i+m_w-1] ? 1 : 0)
						 + (pbuf[i-1]     ? 1 : 0) ;

			if (val1>=nMinNeighbors)
				m_buf[i] = nReplaceVal;
		}
	}
}

void CPixelBuffer::ExtendPeaksAndThin ( CPixelBuffer *pSrcBuf
									  , int nMaxVal )
{
	static CPixelBuffer IndexBuf ;
	static CPixelBuffer IndexBufCpy ;
    int i, y, v, nNumInd, nNumChanged, nInd, nMinInd, nMaxInd ;
	int *indbuf ;
	int *indbufcpy ;
    int *pbuf ;
	int *pDst ;
	int *pSrc ;
	int *pdata ;
	int nSize ;

    if (!pSrcBuf)
        return ;

    pbuf = (int *)pSrcBuf->GetBuffer() ;
    if (!pbuf)
        return;

	if (pSrcBuf->m_nStepSize!=4)
		return;

	if (!Init(pSrcBuf->m_w, pSrcBuf->m_h, 4)) 
		return ;
	if (!IndexBuf.Init(pSrcBuf->m_w, pSrcBuf->m_h))
		return ;
	if (!IndexBufCpy.Init(pSrcBuf->m_w, pSrcBuf->m_h))
		return ;

	if (m_buf==NULL || m_size<=0)
		return ;

	// First, find all the pixel indeces that satisfy the nMaxVal
	// condition.
	indbuf    = IndexBuf.m_buf ;
	indbufcpy = IndexBufCpy.m_buf ;
	pSrc = pSrcBuf->m_buf ;
	pDst = m_buf ;
	nSize = m_size ;
	nNumInd = 0 ;
	
	for (i=0; i<m_size; i++)
	{
		v = pSrcBuf->m_buf[i];
		if (v>=nMaxVal)
		{
			m_buf[i] = v ;
			indbuf[nNumInd] = i ;
			nNumInd++ ;
		}
		else
		    m_buf[i] = 0 ;
	}
/*
	{
		_asm
		{
			mov esi, pSrc
			mov ecx, pDst
			mov edi, indbuf
			mov ebx, nMaxVal
			mov edx, 0

another_thin_sort2:
			mov eax, [esi]			// m_buf[i] = pSrcBuf->m_buf[i]
			mov [ecx], eax

			cmp ebx, eax			// if (nMaxVal<=m_buf[i])
			jle do_thin_sort_set2   //     
			mov [edi], 0
			add esi, 4
			add ecx, 4
			inc edx
			dec nSize
			jnz another_thin_sort2
			jmp done_thin_sort2

do_thin_sort_set2:
			mov [edi], edx			//     indbuf[nNumInd] = i ;

			inc edx					//     i++
			add esi, 4				//     
			add ecx, 4

			add edi, 4				//	   nNumInd++
			dec nSize	
			jnz another_thin_sort2

done_thin_sort2:
			sub edi, indbuf			// nNumInd = 
			shr edi, 2
			mov nNumInd, edi
		}
	}
*/

	// If we didn't have any at the max, just return,
	// there's no need to thin.
	if (nNumInd<=0)
	{
		return;
	}

	memcpy (indbufcpy, indbuf, sizeof(int)*nNumInd) ;

	nMinInd = m_w + 1 ;
	nMaxInd = m_size - m_w - 1 ;
	{
		do
		{
			nNumChanged = 0 ;

			for (i=0; i<nNumInd; i++)
			{
				nInd = indbuf[i] ;
				if (nInd>nMinInd && nInd<nMaxInd)
				{
					// Now, look at this pixel and it's neighbors.
					// If it doesn't have at least 8 neighbors
					// of nMaxVal, then it should be discarded.
					int val1     = (m_buf[nInd-m_w-1]>=nMaxVal ? 1 : 0)
								 + (m_buf[nInd-m_w]  >=nMaxVal ? 1 : 0)
								 + (m_buf[nInd-m_w+1]>=nMaxVal ? 1 : 0)
								 + (m_buf[nInd+1]    >=nMaxVal ? 1 : 0)
								 + (m_buf[nInd+m_w+1]>=nMaxVal ? 1 : 0)
								 + (m_buf[nInd+m_w]  >=nMaxVal ? 1 : 0)
								 + (m_buf[nInd+m_w-1]>=nMaxVal ? 1 : 0)
								 + (m_buf[nInd-1]    >=nMaxVal ? 1 : 0) ;

					if (val1<=5)
					{
						indbuf[i] = -1 ;
					}
					else
					{
						m_buf[nInd] = nMaxVal + 1 ;
						nNumChanged++;
					}
				}
			}

			if (nNumChanged)
				nMaxVal++ ;
		} 
		while (nNumChanged>0) ;
	}

	// Now, sort the indeces so we can
	// thin the thicker sections off.
	g_pIntensityBuffer = m_buf ;
	{
		qsort(indbufcpy,nNumInd,sizeof(int),BufferIntensityIndexSort) ;
	}

	{
		do
		{
			nNumChanged = 0 ;
			for (y=0; y<nNumInd; y++)
			{
				i = 0 ;

				pdata = m_buf + indbufcpy[y] ;
				v = *pdata ;
				if (v)
				{
					*pdata = 255 ;
					i += (pdata[-m_w-1] ? 1   : 0 ) ;
					i += (pdata[-m_w]   ? 2   : 0 ) ;
					i += (pdata[-m_w+1] ? 4   : 0 ) ;
					i += (pdata[	 1]	? 8   : 0 ) ;
					i += (pdata[ m_w+1] ? 16  : 0 ) ;
					i += (pdata[ m_w  ] ? 32  : 0 ) ;
					i += (pdata[ m_w-1] ? 64  : 0 ) ;
					i += (pdata[	-1] ? 128 : 0 ) ;

					if (bCanDeletePixel[i]) 
					{
						*pdata = 0 ;
						nNumChanged++ ;
					}	
				}
			}
		}
		while (nNumChanged);
	} 
}



void CPixelBuffer::Gaussian3x3(CPixelBuffer *psource)
{
    //ASSERT(psource);
    int i, j, x, y, rval ;
    int *pbuf ;

    if (!psource)
        return ;

    pbuf = (int *)psource->GetBuffer() ;
    if (!pbuf)
        return;

    if (!Init(psource->Width(), psource->Height()))
        return;

    // First, calculate the offsets
    i = 0 ;
    for (y=0; y<3; y++)
    {
        for (x=0; x<3; x++)
        {
            GsOfs[i] = (y-1) * m_w + (x - 1) ;
            i++;
        }
    }


    for (y=1; y<m_h-1; y++)
    {
        for (x=1; x<m_w-1; x++)
        {
            i = y*m_w + x ;
            rval = 0 ;

            for (j=0; j<9; j++)
                rval += (pbuf[i+GsOfs[j]] * GaussianMask3x3[j]);
            m_buf[i] = rval ;



        }
    }

}

// This is the 3 x 3 mask!
static int DimpleMask3x3[9] =
{
1,  1,  1,
1, -8,  1,    
1,  1,  1
};


void CPixelBuffer::Dimple(CPixelBuffer *psource, int nMinRange, int nMinMatch)
{
    //ASSERT(psource);
    int i, j, x, y, rval, min, max, rangecnt ;
    int *pbuf ;

    if (!psource)
        return ;

    pbuf = (int *)psource->GetBuffer() ;
    if (!pbuf)
        return;

    if (!Init(psource->Width(), psource->Height()))
        return;

    // First, calculate the offsets
    i = 0 ;
    for (y=0; y<3; y++)
    {
        for (x=0; x<3; x++)
        {
            GsOfs[i] = (y-1) * m_w + (x - 1) ;
            i++;
        }
    }

    for (y=1; y<m_h-1; y++)
    {
        for (x=1; x<m_w-1; x++)
        {
            i = y*m_w + x ;
            rval = 0 ;

			// First, get local range
			min = 0x7FFFFFFF ;
			max = 0x80000000 ;
            for (j=0; j<9; j++)
			{
                rval = pbuf[i+GsOfs[j]] ;
				if (min > rval)
					min = rval ;
				if (max < rval)
					max = rval ;
			}

			// Are we likely a dimple?
			if (pbuf[i] == min)
			{
				rangecnt = 0 ;

				for (j=0; j<9; j++)
				{
					rval = pbuf[i+GsOfs[j]] ;
					if (rval>=min+nMinRange)
						rangecnt++;
				}

				if (rangecnt >= nMinMatch)
					m_buf[i] = 255 ;
				else
					m_buf[i] = 0 ;
			}
			else
				m_buf[i] = 0 ;
        }
    }
}


BOOL CPixelBuffer::IsZero()
{
	int i ;
	for (i=0; i<m_size; i++)
		if (m_buf[i]!=0)
			return false ;

	return true ;
}


int CPixelBuffer::GetAt(int x, int y)
{
	if (!m_buf || x<0 || y<0 || x>=m_w || y>=m_h)
		return 0 ;
	if (m_nStepSize==sizeof(int))
		return ((int *)m_buf)[m_w*y+x] ;
	else
		return ((unsigned char *)m_buf)[m_nStepSize*(m_w*y+x)] ;
}

int CPixelBuffer::GetAt(DOUBLE *pt)
{
	if (!m_buf || pt[0]<0 || pt[1]<0 || pt[0]>=m_w || pt[1]>=m_h)
		return 0 ;
	if (m_nStepSize==sizeof(int))
		return ((int *)m_buf)[m_w*(int)pt[1]+(int)pt[0]] ;
	else
		return ((unsigned char *)m_buf)[m_nStepSize*(m_w*(int)pt[1]+(int)pt[0])] ;
}

int CPixelBuffer::GetAt(CPoint p)
{
	if (!m_buf || p.x<0 || p.y<0 || p.x>=m_w || p.y>=m_h)
		return 0 ;
    return ((int *)m_buf)[m_w*p.y+p.x] ;
}

int CPixelBuffer::GetAt(CCoord p)
{
	return GetAt ((int)p.m_x, (int)p.m_y) ;
}

// No checks are made in this method

int CPixelBuffer::GetAtFast(int x, int y)
{
    return ((int *)m_buf)[m_w*y+x] ;
}

int CPixelBuffer::GetAtFast(CPoint p)
{
    return ((int *)m_buf)[m_w*p.y+p.x] ;
}

void CPixelBuffer::SetAtFast(CPoint p, int v )
{
    ((int *)m_buf)[m_w*p.y+p.x] = v ;
}


int CPixelBuffer::GetAtFastest(int x, int RowIndex)
{
    return ((int *)m_buf)[RowIndex+x] ;
}


int CPixelBuffer::GetRowIndex(int y)
{
	return m_w*y ;
}

int CPixelBuffer::Index(int x, int y)
{
	if (x<0 || x>=m_w || y<0 || y>m_h)
		return -1 ;
	return x+m_w*y ;
}

int CPixelBuffer::Index(CCoord &c)
{
	int x = (int)c.m_x ;
	int y = (int)c.m_y ;
	return Index(x,y) ;
}

void CPixelBuffer::SetAt(int x, int y, int v)
{
	if (!m_buf || x<0 || y<0 || x>=m_w || y>=m_h)
		return ;
	if (m_nStepSize==4)
		((int *)m_buf)[m_w*y+x] = v;
	else
		((unsigned char *)m_buf)[m_w*y+x] = (unsigned char)v ;
}

void CPixelBuffer::SetAt(CCoord &c, int v)
{
	SetAt((int)c.m_x, (int)c.m_y, v) ;
}

// No checks are made in this method

void CPixelBuffer::SetAtFast(int x, int y, int v)
{
    ((int *)m_buf)[m_w*y+x] = v;
}


void CPixelBuffer::SetAtFastest(int x, int RowIndex, int v)
{
    ((int *)m_buf)[RowIndex+x] = v ;
}


void CPixelBuffer::SetAt(CPixel &pt)
{
    m_buf[m_w*pt.y+pt.x] = pt.v;
}

void CPixelBuffer::Diff(CPixelBuffer *pbuf, BOOL bClip, int nClipLow, int nClipHigh) 
{
    //ASSERT(pbuf);
    int numelem ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    numelem = m_w * m_h ;
    for (int i=0; i<numelem; i++)
	{
        m_buf[i] -= pbuf->m_buf[i];
		if (bClip)
		{
			if (m_buf[i] < nClipLow)
				m_buf[i] = nClipLow ;
			else
			if (m_buf[i] > nClipHigh)
				m_buf[i] = nClipHigh ;
		}
	}
}

void CPixelBuffer::Or(CPixelBuffer *pbuf) 
{
    //ASSERT(pbuf);
    int numelem ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    numelem = m_w * m_h ;
    for (int i=0; i<numelem; i++)
        m_buf[i] |= pbuf->m_buf[i];
}

// Sum adds the values of two buffers together.  It always
// creates a 32 bit buffer.  

void CPixelBuffer::Sum(CPixelBuffer *pbuf) 
{
    //ASSERT(pbuf);
    int numelem, i ;

    // Sums always default to 32 bits!
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    numelem = m_w * m_h ;

    for (i=0; i<numelem; i++)
        m_buf[i] += pbuf->m_buf[i] ;
}

int CPixelBuffer::Sum() 
{
    int numelem, i, rval ;

	rval = 0 ;

    // Sums always default to 32 bits!
    numelem = m_w * m_h ;

    for (i=0; i<numelem; i++)
        rval += m_buf[i] ;

	return rval ;
}

// Div divides all the values by a constant

void CPixelBuffer::Div(int divval) 
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] /= divval ;
}

void CPixelBuffer::Mult(int multval) 
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] *= multval ;
}

void CPixelBuffer::Sub(CPixelBuffer *pbuf)
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] -= pbuf->m_buf[i] ;
}

void CPixelBuffer::SubFloor(CPixelBuffer *pbuf)
{
	//ASSERT(m_nStepSize==pbuf->m_nStepSize) ;
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

	switch (m_nStepSize)
	{
	case 4:
		numelem = m_w * m_h ;
		for (i=0; i<numelem; i++)
		{
			m_buf[i] -= pbuf->m_buf[i] ;
			if (m_buf[i]<0)
				m_buf[i] = 0 ;
		}
		break;
	case 1:
		{
			unsigned char *psrc=(unsigned char *)pbuf->m_buf ;
			unsigned char *pdst=(unsigned char *)m_buf;
			int v ;

			numelem = m_w * m_h ;

			for (i=0; i<numelem; i++)
			{
				v = pdst[i] ;
				v -= (int)psrc[i] ;
				if (v<0)
					v = 0 ;
				pdst[i] = (unsigned char)v ;
			}
		}
		break;
	}
}

void CPixelBuffer::Add(CPixelBuffer *pbuf)
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] += pbuf->m_buf[i] ;
}

void CPixelBuffer::MinThreshold(int threshval, int replaceval)
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
		if (m_buf[i]<threshval)
			m_buf[i] = replaceval ;
}

void CPixelBuffer::Add(int addval) 
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] += addval ;
}

void CPixelBuffer::Sub(int subval) 
{
    int numelem, i ;

    if (m_w<=0 || m_h<=0)
        return ;

    numelem = m_w * m_h ;
    for (i=0; i<numelem; i++)
        m_buf[i] -= subval ;
}

// This is meant only for destination buffers of the specified size.
// The only way this will work is if the dest buffer matches the size
// of the pixel buffer.

void CPixelBuffer::DiffDest(unsigned char *pDest)
{
    int i ;

    for (i=0; i<m_size; i++)
    {
        if (pDest[i] > m_buf[i])
            pDest[i] -= m_buf[i] ;
        else
            pDest[i] = 0 ;
    }
}




/*
BOOL CPixelBuffer::SerializeOut (CFile &file) 
{

    TRY
    {
        file.Write(&m_w, sizeof(int)) ;
        file.Write(&m_h, sizeof(int)) ;
        file.Write(m_buf, GetBufferSize()) ;
    }
    CATCH(CFileException, e)
    {
        return false ;
    }
    END_CATCH
    return true ;
}


BOOL CPixelBuffer::SerializeIn (CFile &file) 
{
    int w, h ;

    TRY
    {
        file.Read(&w, sizeof(int)) ;
        file.Read(&h, sizeof(int)) ;
        if (!Init(w,h))
            return false;
        file.Read(m_buf, GetBufferSize()) ;
    }
    CATCH(CFileException, e)
    {
        return false;
    }
    END_CATCH
    return true ;
}
*/



void CPixelBuffer::CalculateGaussianMask(int maskw, int maskh, DOUBLE sigma, DOUBLE scale)
{
	// We calculate the mask based on the mask width and the 
	// mask height.  The offsets also use the width and height
	// of the buffer.
    int i, x, y;
	DOUBLE  sx, sy, v, minv, maxv ;

	// We must only use odd values for the maskw and maskh
	//ASSERT( (maskw>0) && ((maskw % 2)== 1) && (maskh>0) && ((maskh % 2)== 1) ) ;

	m_GaussianMaskWidth = maskw ;
	m_GaussianMaskHeight = maskh ;

	// Calculate the offsets
	if (m_GaussianOffsets)
		delete [] m_GaussianOffsets ;

	m_GaussianOffsets = new int [maskw * maskh] ;

    i = 0 ;
    for (y=0; y<maskh; y++)
    {
        for (x=0; x<maskw; x++)
        {
            m_GaussianOffsets[i] = (y-maskh/2) * m_w + (x - maskw/2) ;
            i++;
        }
    }

	// Calculate the mask values
	if (m_GaussianMask)
		delete [] m_GaussianMask ;

	m_GaussianMask = new int [maskw * maskh] ;

	// Calculate the maximum value given the input states
	maxv = 0.0 ;
    for (y=0; y<maskh; y++)
    {
	    for (x=0; x<maskw; x++)
        {
            sx = (DOUBLE)(x - maskw/2) ;
            sy = (DOUBLE)(y - maskh/2) ;
            v = sigma * sigma * (DOUBLE) exp ( -(sx*sx+sy*sy) / (2*sigma*sigma) ) ;
			if (v > maxv)
				maxv = v ;
        }
    }

	// Finally, scale the values and insert into the array
	minv = maxv / scale;

	i = 0 ;
    for (y=0; y<maskh; y++)
    {
	    for (x=0; x<maskw; x++)
        {
            sx = (DOUBLE)(x - maskw/2) ;
            sy = (DOUBLE)(y - maskh/2) ;
            v = sigma * sigma * (DOUBLE) exp ( -(sx*sx+sy*sy) / (2*sigma*sigma) ) ;
			m_GaussianMask[i] = (int)(floor(v / minv + 0.5)) ;
			i++ ;
        }
    }
}


void CPixelBuffer::GaussianPrecalculated(CPixelBuffer *source)
{
    //ASSERT(source);
	int x, y, i, j, hw, hh, rval, numindeces ;
	int pixelval, multval ;
	int *pbuf ;
	int *poffset ;

	// ASSERT ( m_GaussianMaskWidth > 0 && m_GaussianMaskHeight > 0 && (m_GaussianMaskWidth % 2) == 1 && (m_GaussianMaskHeight % 2) == 1) ;

    // Verify that the buffers are valid
    if (!source)
        return ;
    if (!source->GetBuffer())
        return;
    if (!Init(source->Width(), source->Height()))
        return;

	hw = m_GaussianMaskWidth / 2 ;
	hh = m_GaussianMaskHeight / 2 ;
	numindeces = m_GaussianMaskWidth * m_GaussianMaskHeight ;

	pbuf = source->GetBuffer() ;

    for (y=hw; y<m_h-hw; y++)
    {
        for (x=hw; x<m_w-hw; x++)
        {
            i = y*m_w + x ;

            rval = 0 ;

			poffset = m_GaussianOffsets ;

            for (j=0; j<numindeces; j++)
			{
				pixelval = pbuf[i + *poffset] ;
				multval  = pixelval * m_GaussianMask[j] ;
                rval += multval ;

				poffset++ ;
			}

            m_buf[i] = rval ;
        }
    }
}


void CPixelBuffer::Zero(int left, int top, int right, int bottom)
{
	int x, y, i ;
	if (m_w==0 || m_h==0)
		return;
	if (left < 0)
		left = 0 ;
	if (top < 0)
		top = 0 ;
	if (right < 0)
		right = m_w ;
	if (bottom < 0)
		bottom = m_h ;


	for (x = left ;  x < right ; x++)
	{
		for (y = top ;  y < bottom ; y++)
		{
			i = x + m_w*y ;
			m_buf[i] = 0 ;
		}
	}
}

// This is a specialized function that can only be called by the
// find peaks method.  It allows us to extract a run of pixels
// in a particular direction.  The xdir and ydir can range from
// -1 to 1.  The length typically is the radius and does not 
// include the center point.  The first point value returned is the
// nearest, the last point value returned is the most distant...

// This will eventually be assembly to speed up the process  
BOOL CPixelBuffer::GetPixelRun(int *rvals, int x, int y, int xdir, int ydir, int length, int *pNonZeroCount)
{
	int lastval = 0;
	int nNonZeroCount = 0 ;

	while (length)
	{
		if (x >= 0 && y >= 0 && x < m_w && y < m_h)
		{
			*rvals = lastval = m_buf[x + y*m_w] ;
		}
		else
			// We fail if we go outside the boundary...
			*rvals = 0 ; //lastval ;

		if (*rvals)
			nNonZeroCount++ ;
		else
		if (pNonZeroCount)
			break;

		rvals++ ;
		length-- ;
		x+=xdir ;
		y+=ydir ;
	}

	if (pNonZeroCount)
		*pNonZeroCount = nNonZeroCount ;
	return true ;
}

// A peak run is valid if it spans the desired minimum range or
// greater.  To do this, the farthes value must be the peakvalue
// - minrange or less.  None of the values may be greater than
// the peak value.  Equal values are valid, but not encouraged...
static int nMinRange = 32;


BOOL CPixelBuffer::ValidPeakRun(int *rvals, int peakval, int length)
{
	// First, do the less or equal test.
	while(length)
	{
		if (*rvals > peakval)
			return false ;
//		if (*rvals < (peakval-nMinRange))
//			return true ;
		if (--length)
			rvals++ ;
	}
	// Now, we're at the last pixel.  It must be less than
	// peakval - nMinRange
	if (*rvals < (peakval-nMinRange*peakval/255))
		return true ;
	else
		return false ;
}

// This is my first attempt at simple peak finding.  Hopefully, we'll
// get it going right...
void CPixelBuffer::FindPeaks ( CPixelBuffer *pbuf
							 , int radius
							 , int threshold
							 , int minrange
							 , int nNumSucesses
							 , int replval ) 
{
	int mindim = radius*2+1;
	int x, y, yind, pixval;
	int pixrun[64], pixantirun[64] ;
	int hminrange ;
	int NumFound ;
	int nSuccessCount ;
	BOOL ValidHorz, ValidVert, ValidDiag1, ValidDiag2;
	BOOL FoundOne ;
	int nNumTested, nNumNotTested ;
	int *pSrc, *pDst ;

	if (!pbuf)
	{
		return ;
	}

	// We're essentially doing a copy...
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	memset (m_buf, 0, m_size*sizeof(int)) ;

	hminrange = minrange/2 ;
	nMinRange = minrange ;

	FoundOne = false ;
	NumFound = 0 ;
	nNumTested = nNumNotTested = 0 ;

	// Make sure we don't do something illegal...
	if (!m_buf)
	{
		return ;
	}
	if (m_w < mindim || m_h < mindim)
	{
		return ;
    }
    
	for (y=2; y<m_h-2; y++)
	{
		yind = y *m_w ;
		pSrc = pbuf->m_buf+yind+2 ;
		pDst = m_buf+yind+2 ;

		for (x=2; x<m_w-2; x++, pSrc++, pDst++)
		{
			// Get the center pixel val
			pixval = *pSrc ;

			if (pixval < threshold + minrange)
            {
				nNumNotTested++;
				// No need to erase since that was already taken care of in the
				// Clear command.
            }
			else
			{
				nNumTested++;
				nSuccessCount = 0 ;
				// Start with horizontal
				if (	pbuf->GetPixelRun(pixrun,x,y,1,0,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,0,radius) )
					ValidHorz = ValidPeakRun(pixrun, pixantirun, pixval, radius);
				else
					ValidHorz = false ;
				if (ValidHorz)
					nSuccessCount++ ;

				// Now the vertical
				if (	pbuf->GetPixelRun(pixrun,x,y,0,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,0,-1,radius) )
					ValidVert = ValidPeakRun(pixrun, pixantirun, pixval, radius);
				else
					ValidVert = false ;
				if (ValidVert)
					nSuccessCount++ ;

				// Now diag 1
				if (	pbuf->GetPixelRun(pixrun,x,y,1,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,-1,radius) )
					ValidDiag1 = ValidPeakRun(pixrun, pixantirun, pixval, radius);
				else
					ValidDiag1 = false ;
				if (ValidDiag1)
					nSuccessCount++ ;

				// Now diag 2
				if (	pbuf->GetPixelRun(pixrun,x,y,1,-1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,1,radius) )
					ValidDiag2 = ValidPeakRun(pixrun, pixantirun, pixval, radius);
				else
					ValidDiag2 = false ;
				if (ValidDiag2)
					nSuccessCount++ ;


				// Get pixels in v direction
				*pDst =   (ValidHorz  ? replval : 0x00) | (ValidVert  ? replval : 0x00) 
					    | (ValidDiag1 ? replval : 0x00) | (ValidDiag2 ? replval : 0x00) ;
				if (nSuccessCount<nNumSucesses)
					*pDst = 0 ;

                if (*pDst)
				{
					FoundOne = true ;
					NumFound++;
				}
			}
		}
	}
	if (FoundOne)
	{
	}
}


void CPixelBuffer::FindPeaks ( CPixelBuffer *pbuf
							 , int radius
							 , int threshold
							 , int minrange 
							 , CPixelBuffer *pthreshbuf ) 
{

	int mindim = radius*2+1;
	int x, y, yind, xind, pixval;
	int pixrun[64], pixantirun[64] ;
	int hminrange ;
	int NumFound ;
	int nHorzCnt, nVertCnt, nDiag1Cnt, nDiag2Cnt ;
	int cnt1, cnt2 ;
	BOOL ValidHorz, ValidVert, ValidDiag1, ValidDiag2;
	BOOL FoundOne ;

	// We're essentially doing a copy...
    if (!Init(pbuf->m_w, pbuf->m_h, 4))
        return;

    //ASSERT(pbuf);
    Clear() ;

	hminrange = minrange/2 ;
	nMinRange = minrange ;

	FoundOne = false ;
	NumFound = 0 ;

	// Make sure we don't do something illegal...
	if (!m_buf)
		return ;
	if (m_w < mindim || m_h < mindim)
		return ;

	for (y=2; y<m_h-2; y++)
	{
		yind = y *m_w ;

		for (x=2; x<m_w-2; x++)
		{
			// Get the center pixel val
			xind = yind + x ;
            
			pixval = pbuf->m_buf[xind] ;

			if (pixval < threshold + minrange)
            {
				m_buf[xind] = 0 ;
            }
			else
			{
				// Start with horizontal
				if (	pbuf->GetPixelRun(pixrun,x,y,1,0,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,0,radius) )
					ValidHorz =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidHorz = false ;

				// Now the vertical
				if (	pbuf->GetPixelRun(pixrun,x,y,0,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,0,-1,radius) )
					ValidVert =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidVert = false ;

				// Now diag 1
				if (	pbuf->GetPixelRun(pixrun,x,y,1,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,-1,radius) )
					ValidDiag1 =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidDiag1 = false ;

				// Now diag 2
				if (	pbuf->GetPixelRun(pixrun,x,y,1,-1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,1,radius) )
					ValidDiag2 =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidDiag2 = false ;


				// Get pixels in v direction
//				m_buf[xind] = (ValidHorz  ? 0x01 : 0x00) | (ValidVert  ? 0x02 : 0x00) 
//							| (ValidDiag1 ? 0x04 : 0x00) | (ValidDiag2 ? 0x08 : 0x00) ;
				m_buf[xind] = (ValidHorz  ? 0xff : 0x00) | (ValidVert  ? 0xff : 0x00) 
							| (ValidDiag1 ? 0xff : 0x00) | (ValidDiag2 ? 0xff : 0x00) ;

                if (m_buf[xind])
				{
					// If we found a peak, it may be the case that the peak does
					// not actually lie on the proper gradient.  The way to test
					// this is to check the success direction against the local
					// thresholded area.  If the width of the success direction
					// is significantly greater than the width of the failed
					// directions, we really don't have a peak.
					
					if (pthreshbuf)
					{
						// First, what is the length of valid thresholded pixels
						// in the same direction as the successful peak.
						if (ValidHorz)
						{
							pbuf->GetPixelRun(pixrun,x,y,1,0,radius,&cnt1) ;
							pbuf->GetPixelRun(pixantirun,x,y,-1,0,63,&cnt2) ;
							nHorzCnt = cnt1+cnt2 ;
							pbuf->GetPixelRun(pixrun,x,y,0,1,radius,&cnt1) ;
							pbuf->GetPixelRun(pixantirun,x,y,0,-1,63,&cnt2) ;
							nVertCnt = cnt1+cnt2 ;

							if (nVertCnt < nHorzCnt)
								ValidHorz = false ;
						}
						if (ValidVert)
						{
							pbuf->GetPixelRun(pixrun,x,y,1,0,63,&cnt1) ;
							pbuf->GetPixelRun(pixantirun,x,y,-1,0,63,&cnt2) ;
							nHorzCnt = cnt1+cnt2 ;
							pbuf->GetPixelRun(pixrun,x,y,0,1,63,&cnt1) ;
							pbuf->GetPixelRun(pixantirun,x,y,0,-1,63,&cnt2) ;
							nVertCnt = cnt1+cnt2 ;

							if (nHorzCnt < nVertCnt)
								ValidVert = false ;
						}
						if (ValidDiag1)
						{
							pbuf->GetPixelRun(pixrun,x,y,1,-1,63,&cnt1) ;
							pbuf->GetPixelRun(pixrun,x,y,-1,-1,63,&cnt1) ;
							nDiag1Cnt = cnt1+cnt2 ;

							pbuf->GetPixelRun(pixantirun,x,y,1,1,63,&cnt2) ;
							pbuf->GetPixelRun(pixantirun,x,y,-1,1,63,&cnt2) ;
							nDiag2Cnt = cnt1+cnt2 ;

							if (nDiag2Cnt < nDiag1Cnt)
								ValidDiag1 = false ;
						}
						if (ValidDiag2)
						{
							pbuf->GetPixelRun(pixrun,x,y,1,-1,63,&cnt1) ;
							pbuf->GetPixelRun(pixrun,x,y,-1,-1,63,&cnt1) ;
							nDiag1Cnt = cnt1+cnt2 ;

							pbuf->GetPixelRun(pixantirun,x,y,1,1,63,&cnt2) ;
							pbuf->GetPixelRun(pixantirun,x,y,-1,1,63,&cnt2) ;
							nDiag2Cnt = cnt1+cnt2 ;

							if (nDiag1Cnt < nDiag2Cnt)
								ValidDiag2 = false ;
						}

						m_buf[xind] = (ValidHorz  ? 0xff : 0x00) | (ValidVert  ? 0xff : 0x00) 
									| (ValidDiag1 ? 0xff : 0x00) | (ValidDiag2 ? 0xff : 0x00) ;

		                if (m_buf[xind])
						{
							FoundOne = true ;
							NumFound++;
						}
					}
					else
					{
						FoundOne = true ;
						NumFound++;
					}
				}
			}
		}
	}
	if (FoundOne)
	{
	}
}

void CPixelBuffer::FindMaskPeaks ( CPixelBuffer *pbuf
							 , CPixelBuffer *maskbuf
							 , int radius
							 , int threshold
							 , int minrange 
							 , CPixelBuffer *pthreshbuf ) 
{
    //ASSERT(pbuf);
	int mindim = radius*2+1;
	int x, y, yind, xind, pixval;
	int pixrun[64], pixantirun[64] ;
	int hminrange ;
	int NumFound ;
	BOOL ValidHorz, ValidVert, ValidDiag1, ValidDiag2;
	BOOL FoundOne ;

    //ASSERT(pbuf);
	// We're essentially doing a copy...
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    //ASSERT(pbuf);
    Clear() ;
    //ASSERT(pbuf);

	hminrange = minrange/2 ;
	nMinRange = minrange ;

	FoundOne = false ;
	NumFound = 0 ;

	// Make sure we don't do something illegal...
	if (!m_buf)
		return ;
	if (m_w < mindim || m_h < mindim)
		return ;

	for (y=2; y<m_h-2; y++)
	{
		yind = y *m_w ;

		for (x=2; x<m_w-2; x++)
		{
			// Get the center pixel val
			xind = yind + x ;
            
            //ASSERT(pbuf);

			pixval = pbuf->m_buf[xind] ;

			if (!maskbuf->m_buf[xind])
			{
			}
			else
			if (pixval < threshold + minrange)
            {
				m_buf[xind] = 0 ;
                //ASSERT(pbuf);
            }
			else
			{
                //ASSERT(pbuf);
				// Start with horizontal
				if (	pbuf->GetPixelRun(pixrun,x,y,1,0,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,0,radius) )
					ValidHorz =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidHorz = false ;

                //ASSERT(pbuf);
				// Now the vertical
				if (	pbuf->GetPixelRun(pixrun,x,y,0,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,0,-1,radius) )
					ValidVert =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidVert = false ;

                //ASSERT(pbuf);
				// Now diag 1
				if (	pbuf->GetPixelRun(pixrun,x,y,1,1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,-1,radius) )
					ValidDiag1 =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidDiag1 = false ;

                //ASSERT(pbuf);
				// Now diag 2
				if (	pbuf->GetPixelRun(pixrun,x,y,1,-1,radius) 
					&&  pbuf->GetPixelRun(pixantirun,x,y,-1,1,radius) )
					ValidDiag2 =    ValidPeakRun(pixrun, pixval, radius)
								&& ValidPeakRun(pixantirun, pixval, radius) ;
				else
					ValidDiag2 = false ;

                //ASSERT(pbuf);

				// Get pixels in v direction
//				m_buf[xind] = (ValidHorz  ? 0x01 : 0x00) | (ValidVert  ? 0x02 : 0x00) 
//							| (ValidDiag1 ? 0x04 : 0x00) | (ValidDiag2 ? 0x08 : 0x00) ;
				m_buf[xind] = (ValidHorz  ? 0xff : 0x00) | (ValidVert  ? 0xff : 0x00) 
							| (ValidDiag1 ? 0xff : 0x00) | (ValidDiag2 ? 0xff : 0x00) ;

                //ASSERT(pbuf);
                if (m_buf[xind])
				{
					// If we found a peak, it may be the case that the peak does
					// not actually lie on the proper gradient.  The way to test
					// this is to check the success direction against the local
					// thresholded area.  If the width of the success direction
					// is significantly greater than the width of the failed
					// directions, we really don't have a peak.
					FoundOne = true ;
					NumFound++;
				}
			}
		}
	}
	if (FoundOne)
	{
	}
}


int CPixelBuffer::MaxNeighborFast(int index)
{
	int v,r ;

	if (m_nStepSize==4)
	{
	 	v=m_buf[index] ;
	 	r=m_buf[index+1] ;
		if (r>v) v=r ;
		r=m_buf[index-1] ;
		if (r>v) v=r ;
		r=m_buf[index+m_w-1] ;
		if (r>v) v=r ;
		r=m_buf[index+m_w] ;
		if (r>v) v=r ;
		r=m_buf[index+m_w+1] ;
		if (r>v) v=r ;
		r=m_buf[index-m_w-1] ;
		if (r>v) v=r ;
		r=m_buf[index-m_w] ;
		if (r>v) v=r ;
		r=m_buf[index-m_w+1] ;
		if (r>v) v=r ;
	}
	else
	{
		unsigned char *ucbuf = (unsigned char *)m_buf ;
	 	v=ucbuf[index] ;
	 	r=ucbuf[index+1] ;
		if (r>v) v=r ;
		r=ucbuf[index-1] ;
		if (r>v) v=r ;
		r=ucbuf[index+m_w-1] ;
		if (r>v) v=r ;
		r=ucbuf[index+m_w] ;
		if (r>v) v=r ;
		r=ucbuf[index+m_w+1] ;
		if (r>v) v=r ;
		r=ucbuf[index-m_w-1] ;
		if (r>v) v=r ;
		r=ucbuf[index-m_w] ;
		if (r>v) v=r ;
		r=ucbuf[index-m_w+1] ;
		if (r>v) v=r ;
		
	}
	return v ;
}

int CPixelBuffer::MaxNeighbor(int x, int y)
{
	int v,r;

	v=GetAt(x,y) ;
	r=GetAt(x-1,y-1) ;
	if (r>v) v=r ;
	r=GetAt(x,y-1) ;
	if (r>v) v=r ;
	r=GetAt(x+1,y-1) ;
	if (r>v) v=r ;
	r=GetAt(x-1,y) ;
	if (r>v) v=r ;
	r=GetAt(x+1,y) ;
	if (r>v) v=r ;
	r=GetAt(x-1,y+1) ;
	if (r>v) v=r ;
	r=GetAt(x,y+1) ;
	if (r>v) v=r ;
	r=GetAt(x+1,y+1) ;
	if (r>v) v=r ;

	return v ;
}

bool CPixelBuffer::CopyNewDepthGpu(CPixelBuffer &src, long nBorderW, long nBorderH, int newdepth, long nNumGpus)
{
	int    x, y, i, j ;

	if (src.m_bUseGpu==false || m_bUseGpu==false)
		return false ;

	if ( (src.m_w != m_w) || (src.m_h != m_h))
	{
		printf("newdepth gpu failed unequal dimensions\n") ;
		return false;
	}
	if (m_GpuCodeMain == m_GpuCodeThread)
	{
		long MainCode[] = {
							#include "NewDepthGpuMain.c"
						 	};
		long ThrdCode[] = {
							#include "NewDepthGpuThread.c"
							};

		// First, copy the source code for the main thread
		memcpy ( m_pArmCodeMain, MainCode, sizeof(MainCode) ) ;
		m_pArmCodeThread = m_pArmCodeMain + (sizeof(MainCode)/sizeof(long)) ;
		m_GpuCodeThread = m_GpuCodeMain + sizeof(MainCode) ;
		memcpy ( m_pArmCodeThread, ThrdCode, sizeof(ThrdCode) ) ;
	}

	// Copy the parameters into the uniforms
	// The source pointer MUST be a GPU pointer
    for (i=0; i<nNumGpus; i++)
    {
        (m_pArmUniforms[i])[0]  = src.m_GpuData;    // Input data
    	(m_pArmUniforms[i])[1]  = m_GpuData;			// Output data
        (m_pArmUniforms[i])[2]  = nNumGpus ;			// number of gpus
        (m_pArmUniforms[i])[3]  = i;					// index

        (m_pArmUniforms[i])[4]  = nBorderW ;	      // input x start
		(m_pArmUniforms[i])[5]  = nBorderH ;         // input y start
		(m_pArmUniforms[i])[6]  = src.m_w ;  // input stride
		(m_pArmUniforms[i])[7]  = src.m_h ;  // input height

        (m_pArmUniforms[i])[8]  = m_w ;	 // Output stride
		(m_pArmUniforms[i])[9]  = m_h ;  // Output height
		(m_pArmUniforms[i])[10] = nBorderW ;  // Output start x (border width)
		(m_pArmUniforms[i])[11] = nBorderH ;  // Output start y (border height)

        (m_pArmUniforms[i])[12] = src.m_nStepSize ;	// input2
		(m_pArmUniforms[i])[13] = m_nStepSize ;  // Output Stride
		(m_pArmUniforms[i])[14] = 0 ;  // Output Height
		(m_pArmUniforms[i])[15] = 0 ;  // Output Height
    }

	// Finally, execute the GPU code!
	m_pMapPeriphAddr[V3D_DBCFG] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITE] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITC] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = 1<<2 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = (1<<7) | (1<<8) | (1<<16) ; // Reset error bit and counts

	// This will be our loop
	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter
	/////////m_pMapPeriphAddr[V3D_SRQUL] = nNumUnif ; // No uniforms!

	// Start child threads first so they're waiting
    j = 0 ;
    for (i=1; i<nNumGpus; i++)
    {
    	m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[i] ;
	    m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeThread ;
        j++;
  	}
	
	// Start the main thread
    m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[0] ;
	m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeMain ;

	// Busy wait polling
	long n=0 ;
	long nqsz ;

	do
	{
	    // Read V3D register to see if the program finished
	    nqsz = (m_pMapPeriphAddr[V3D_SRQCS]>>16) & 0x000000FF;
  	    n++ ;
	} while (nqsz!=nNumGpus && n<10000000) ;
	if (n==10000000)
		printf("MaxDialateGpu exceeded wait count\n") ;

	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter

	return true ;
}




void CPixelBuffer::SubFloorGpu(CPixelBuffer &src1, CPixelBuffer &src2, int nBorderW, int nBorderH, long nNumGpus) 
{
	int    x, y, i, j ;

	if (m_bUseGpu == false)
	{
		printf("subfloor gpu failed bUseGpu\n") ;
		return;
	}
	if (src1.m_bUseGpu == false)
	{
		printf("subfloor gpu failed src1 bUseGpu\n") ;
		return;
	}
	if (src2.m_bUseGpu == false)
	{
		printf("subfloor gpu failed src2 bUseGpu\n") ;
		return;
	}
	if ( (m_w<64) || (m_w % 4) )
	{
		printf("subfloor gpu failed 64 width or mod4 width\n") ;
		return;
	}
	if ( (src1.m_w != m_w) || (src1.m_h != m_h) || (src2.m_w != m_w) || (src2.m_h != m_h))
	{
		printf("subfloor gpu failed unequal dimensions\n") ;
		return;
	}
	if (m_GpuCodeMain == m_GpuCodeThread)
	{
		long MainCode[] = {
							#include "SubFloorGpuMain.c"
						 	};
		long ThrdCode[] = {
							#include "SubFloorGpuThread.c"
							};

		// First, copy the source code for the main thread
		memcpy ( m_pArmCodeMain, MainCode, sizeof(MainCode) ) ;
		m_pArmCodeThread = m_pArmCodeMain + (sizeof(MainCode)/sizeof(long)) ;
		m_GpuCodeThread = m_GpuCodeMain + sizeof(MainCode) ;
		memcpy ( m_pArmCodeThread, ThrdCode, sizeof(ThrdCode) ) ;
	}

	// Copy the parameters into the uniforms
	// The source pointer MUST be a GPU pointer
    for (i=0; i<nNumGpus; i++)
    {
        (m_pArmUniforms[i])[0]  = src1.m_GpuData;    // Input data
    	(m_pArmUniforms[i])[1]  = m_GpuData;			// Output data
        (m_pArmUniforms[i])[2]  = nNumGpus ;			// number of gpus
        (m_pArmUniforms[i])[3]  = i;					// index

        (m_pArmUniforms[i])[4]  = nBorderW ;	      // input x start
		(m_pArmUniforms[i])[5]  = nBorderH ;         // input y start
		(m_pArmUniforms[i])[6]  = src1.m_w ;  // input stride
		(m_pArmUniforms[i])[7]  = src1.m_h ;  // input height

        (m_pArmUniforms[i])[8]  = m_w ;	 // Output stride
		(m_pArmUniforms[i])[9]  = m_h ;  // Output height
		(m_pArmUniforms[i])[10] = nBorderW ;  // Output start x (border width)
		(m_pArmUniforms[i])[11] = nBorderH ;  // Output start y (border height)

        (m_pArmUniforms[i])[12] = src2.m_GpuData ;	// input2
		(m_pArmUniforms[i])[13] = 0 ;  // Output Stride
		(m_pArmUniforms[i])[14] = 0 ;  // Output Height
		(m_pArmUniforms[i])[15] = 0 ;  // Output Height
    }

	// Finally, execute the GPU code!
	m_pMapPeriphAddr[V3D_DBCFG] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITE] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITC] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = 1<<2 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = (1<<7) | (1<<8) | (1<<16) ; // Reset error bit and counts

	// This will be our loop
	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter
	/////////m_pMapPeriphAddr[V3D_SRQUL] = nNumUnif ; // No uniforms!

	// Start child threads first so they're waiting
    j = 0 ;
    for (i=1; i<nNumGpus; i++)
    {
    	m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[i] ;
	    m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeThread ;
        j++;
  	}
	
	// Start the main thread
    m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[0] ;
	m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeMain ;

	// Busy wait polling
	long n=0 ;
	long nqsz ;

	do
	{
	    // Read V3D register to see if the program finished
	    nqsz = (m_pMapPeriphAddr[V3D_SRQCS]>>16) & 0x000000FF;
  	    n++ ;
	} while (nqsz!=nNumGpus && n<10000000) ;
	if (n==10000000)
		printf("MaxDialateGpu exceeded wait count\n") ;

	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter
}

//
// When we define our own GPU interface, we need to determine
// during design how many QPU's we'll use.
//
void CPixelBuffer::MaxDialateGpu ( CPixelBuffer &pSrc
								 , CPixelBuffer &pSrc2
								 , int nBorderW
								 , int nBorderH
								 , int NumReps/*=1*/
								 , long nNumGpus /*=1*/
								   )
{
	int    x, y, i, j ;

	// This will ONLY work if we're using the GPU and the memory has
	// been allocated.  The m_bUseGpu flag gets set at the point of
	// memory allocation.
	if (m_bUseGpu == false)
		return;

	// The source memory must also be in GPU memory.  
	if (pSrc.m_bUseGpu == false)
		return;
	if (pSrc2.m_bUseGpu == false)
		return;

	// This will ONLY work if the source pixel buffer has a step size
	// of 1.
	if (pSrc.m_nStepSize != 1)
		return;
	if (pSrc2.m_nStepSize != 1)
		return;
  
	// We must be doing at least one repetition
	if (NumReps==0)
		return ;

	// There must be a border available for us to do the optimized GPU code
	if (nBorderH<1)
		return;
	// The border width must at least be 4, and must be divisible by 4 so
	// the pixel loads fall on 32 bit boundaries.
	if (nBorderW<4 || (nBorderW % 4))
		return;
	// The bitmap width must also be divisible by 4, and at least 64 bytes wide
	if ( (m_w<64) || (m_w % 4) )
		return;

	// Make sure the source and destination bitmaps have the same size.
	if ( (pSrc.m_w != m_w) || (pSrc.m_h != m_h) || (pSrc2.m_w != m_w) || (pSrc2.m_h != m_h) )
		return;

	// Is the GPU main code and thread code pointer different?  If not, we
	// know that this is our first attempt.

	if (m_GpuCodeMain == m_GpuCodeThread)
	{
		long MainCode[] = {
							#include "MaxDialateMain.c"
						 	};
		long ThrdCode[] = {
							#include "MaxDialateThread.c"
							};

		// First, copy the source code for the main thread
		memcpy ( m_pArmCodeMain, MainCode, sizeof(MainCode) ) ;
		m_pArmCodeThread = m_pArmCodeMain + (sizeof(MainCode)/sizeof(long)) ;
		m_GpuCodeThread = m_GpuCodeMain + sizeof(MainCode) ;
		memcpy ( m_pArmCodeThread, ThrdCode, sizeof(ThrdCode) ) ;
	}

	// Copy the parameters into the uniforms
	// The source pointer MUST be a GPU pointer
    for (i=0; i<nNumGpus; i++)
    {
        (m_pArmUniforms[i])[0]  = pSrc.m_GpuData;    // Input data
    	(m_pArmUniforms[i])[1]  = m_GpuData;			// Output data
        (m_pArmUniforms[i])[2]  = nNumGpus ;			// number of gpus
        (m_pArmUniforms[i])[3]  = i;					// index

        (m_pArmUniforms[i])[4]  = 4 ;	      // input x start
		(m_pArmUniforms[i])[5]  = 4 ;         // input y start
		(m_pArmUniforms[i])[6]  = pSrc.m_w ;  // input stride
		(m_pArmUniforms[i])[7]  = pSrc.m_h ;  // input height

        (m_pArmUniforms[i])[8]  = m_w ;	 // Output stride
		(m_pArmUniforms[i])[9]  = m_h ;  // Output height
		(m_pArmUniforms[i])[10] = 4 ;  // Output start x (border width)
		(m_pArmUniforms[i])[11] = 4 ;  // Output start y (border height)

        (m_pArmUniforms[i])[12] = pSrc2.m_GpuData ;	// number of reps
		(m_pArmUniforms[i])[13] = 0 ;  // Output Stride
		(m_pArmUniforms[i])[14] = 0 ;  // Output Height
		(m_pArmUniforms[i])[15] = 0 ;  // Output Height
    }

	// Finally, execute the GPU code!
	m_pMapPeriphAddr[V3D_DBCFG] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITE] = 0 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBQITC] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = 1<<2 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = -1 ; // Disallow IRQ
	m_pMapPeriphAddr[V3D_DBCFG] = (1<<7) | (1<<8) | (1<<16) ; // Reset error bit and counts

	// This will be our loop
	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter
	/////////m_pMapPeriphAddr[V3D_SRQUL] = nNumUnif ; // No uniforms!

	// Start child threads first so they're waiting
    j = 0 ;
    for (i=1; i<nNumGpus; i++)
    {
    	m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[i] ;
	    m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeThread ;
        j++;
  	}
	
	// Start the main thread
    m_pMapPeriphAddr[V3D_SRQUA] = m_GpuUniforms[0] ;
	m_pMapPeriphAddr[V3D_SRQPC] = m_GpuCodeMain ;

	// Busy wait polling
	long n=0 ;
	long nqsz ;

	do
	{
	    // Read V3D register to see if the program finished
	    nqsz = (m_pMapPeriphAddr[V3D_SRQCS]>>16) & 0x000000FF;
  	    n++ ;
	} while (nqsz!=nNumGpus && n<10000000) ;
	if (n==10000000)
		printf("MaxDialateGpu exceeded wait count\n") ;

//	printf("  Done MaxDialateGpu\n") ;

	m_pMapPeriphAddr[V3D_SRQCS] = 0x00010180 ; // Reset the counter
}



void CPixelBuffer::MaxDialate(int NumReps/*=1*/)
{
	static CPixelBuffer temp ;
	int    x, y, i, j ;
  
	if (NumReps==0)
		return ;
	if (!temp.Init(m_w, m_h, m_nStepSize))
		return ;

	if (m_nStepSize==4)
	{
		for (j=0; j<NumReps; j++)
		{
			i=0;

			for (y=1; y<m_h-1; y++)
			{
				i=Index(1,y);
				for (x=1; x<m_w-1; x++)
				{
					temp.m_buf[i]= MaxNeighborFast(i) ;
					i++;
				}
			}
			for (y=0; y<m_h; y++)
			{
				temp.SetAt(0,y,MaxNeighbor(0,y)) ;
				temp.SetAt(m_w-1,y,MaxNeighbor(m_w-1,y)) ;
			}
			for (x=0; x<m_w; x++)
			{
				temp.SetAt(x,0,MaxNeighbor(x,0)) ;
				temp.SetAt(x,m_h-1,MaxNeighbor(x,m_h-1)) ;
			}

			Copy(temp) ;
		}
	}
	else
	if (m_nStepSize==1)
	{
		for (j=0; j<NumReps; j++)
		{
			i=0;

			for (y=1; y<m_h-1; y++)
			{
				i=Index(1,y);
				for (x=1; x<m_w-1; x++)
				{
					((unsigned char *)temp.m_buf)[i]= MaxNeighborFast(i) ;
					i++;
				}
			}

			for (y=0; y<m_h; y++)
			{
				temp.SetAt(0,y,MaxNeighbor(0,y)) ;
				temp.SetAt(m_w-1,y,MaxNeighbor(m_w-1,y)) ;
			}
			for (x=0; x<m_w; x++)
			{
				temp.SetAt(x,0,MaxNeighbor(x,0)) ;
				temp.SetAt(x,m_h-1,MaxNeighbor(x,m_h-1)) ;
			}

			Copy(temp) ;
		}
	}
}

void CPixelBuffer::Dialate(int NumReps)
{
	static CPixelBuffer temp ;
	int    x, y, v, i, j ;
	if (!temp.Init(m_w, m_h))
		return ;

	for (j=0; j<NumReps; j++)
	{
		temp.Clear() ;
		i=0;

		for (y=1; y<m_h-1; y++)
		{
			i=Index(1,y);
			for (x=1; x<m_w-1; x++)
			{
				v=m_buf[i] ;

				if (v!=0)
				{
					temp.m_buf[i] = v ;
					temp.m_buf[i+1] = v ;
					temp.m_buf[i-1] = v ;
					temp.m_buf[i+m_w] = v ;
					temp.m_buf[i-m_w] = v ;
					temp.m_buf[i+m_w+1] = v ;
					temp.m_buf[i+m_w-1] = v ;
					temp.m_buf[i-m_w+1] = v ;
					temp.m_buf[i-m_w-1] = v ;
				}
				i++;
			}
		}

		Copy(temp) ;
	}
}

/*
void CPixelBuffer::Dialate(CPixelBuffer *pCopy, int NumReps)
{
	int    x, y, v, i, j ;
	if (!Init(pCopy->Width(), pCopy->Height()))
		return ;

	for (j=0; j<NumReps; j++)
	{
		Clear() ;
		i=0;

		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++)
			{
				v=m_buf[i] ;
				i++;

				if (v!=0)
				{
					if (pCopy->GetAt(x-1, y-1) == 0)
						SetAt(x-1, y-1, v) ;
					if (pCopy->GetAt(x+1, y-1) == 0)
						SetAt(x+1, y-1, v) ;
					if (pCopy->GetAt(x-1, y+1) == 0)
						SetAt(x-1, y+1, v) ;
					if (pCopy->GetAt(x+1, y+1) == 0)
						SetAt(x+1, y+1, v) ;
					if (pCopy->GetAt(x-1, y) == 0)
						SetAt(x-1, y, v) ;
					if (pCopy->GetAt(x+1, y) == 0)
						SetAt(x+1, y, v) ;
					if (pCopy->GetAt(x, y+1) == 0)
						SetAt(x, y+1, v) ;
					if (pCopy->GetAt(x, y-1) == 0)
						SetAt(x, y-1, v) ;
					SetAt(x,y,v) ;
				}
			}
		}
	}
}
*/

void CPixelBuffer::RemoveSmallNoise(CPixelBuffer *pbuf) 
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;
	Clear() ;

	// In this routine, we only copy a pixel if it has
	// more than 0 neighbor
	int n, c ;

	for (int y=0; y<m_h; y++)
	{
		for (int x=0; x<m_w; x++)
		{
			c = pbuf->GetAt(x,y) ;
			if (c)
			{
				n =		pbuf->GetAt(x-1, y-1) ;
				n = n + pbuf->GetAt(x,   y-1) ;
				n = n + pbuf->GetAt(x+1, y-1) ;
				n = n + pbuf->GetAt(x+1, y) ;
				n = n + pbuf->GetAt(x+1, y+1) ;
				n = n + pbuf->GetAt(x,   y+1) ;
				n = n + pbuf->GetAt(x-1, y+1) ;
				n = n + pbuf->GetAt(x-1, y) ;

				if (n)
					SetAt(x, y, c) ;
			}

		}
	}
}

// This method allows us to recursively add a list of
// pixels to a chain, checking all of the neighbors
// in the process...  We must be careful, though,
// not to overrun our stack frame, so we don't want 
// to have too many variables..
void CPixelBuffer::AddToPixelChain( int x, int y, int v, CPixelArray &ChainBuf )
{
	int nv ;
	// First, add the pixel to the point array
	CPixel pix(x,y,v);
	ChainBuf.Add(pix) ;

	// Next erase it from our buffer
	SetAt(x,y,0) ;

	// Now, search the neighbors, recursively
	nv = GetAt(x-1, y-1) ; 
	if (nv != 0)
		AddToPixelChain(x-1, y-1, nv, ChainBuf) ;
	nv = GetAt(x, y-1);
	if (nv != 0)
		AddToPixelChain(x, y-1, nv, ChainBuf) ;
	nv = GetAt(x+1, y-1);
	if (nv != 0)
		AddToPixelChain(x+1, y-1, nv, ChainBuf) ;
	nv = GetAt(x+1, y);
	if (nv != 0)
		AddToPixelChain(x+1, y, nv, ChainBuf) ;
	nv = GetAt(x+1, y+1);
	if (nv != 0)
		AddToPixelChain(x+1, y+1, nv, ChainBuf) ;
	nv = GetAt(x, y+1);
	if (nv != 0)
		AddToPixelChain(x, y+1, nv, ChainBuf) ;
	nv = GetAt(x-1, y+1);
	if (nv != 0)
		AddToPixelChain(x-1, y+1, nv, ChainBuf) ;
	nv = GetAt(x-1, y);
	if (nv != 0)
		AddToPixelChain(x-1, y, nv, ChainBuf) ;
}

// When this method is completed, our buffer will be 
// empty...
int xincs[8] = {-1, 0, 1, 1, 1, 0, -1, -1} ;
int yincs[8] = {-1, -1, -1, 0, 1, 1, 1, 0} ;

void CPixelBuffer::FindPixelChains(CPixelChains &Chains, BOOL bSort, BOOL bReplot, BOOL bContiguousChain)
{
	int	numelem = m_w * m_h;
	int	x, y, v, numchains, i, j, numpixels ;
	int  *XStack ;
	int  *YStack ;
	int  *VStack ;
	char *DirStack; 
	long nCurStackPos ;
	BOOL bZeroReached ;

	Chains.SetSize(0) ;
	numchains = 0 ;

	// We must create a stack so that we don't 
	// cause any stack crashes...
	XStack = new int [numelem] ;
	YStack = new int [numelem] ;
	VStack = new int [numelem] ;
	DirStack   = new char [numelem] ;

	for (i=0; i<numelem; i++)
	{
		v = m_buf[i] ;
		if (v != 0)
		{
//			if ((numchains < 0) || (numchains > 100000))
//				AfxGetMainWnd()->MessageBox("Houston...  We have a problem") ;
			Chains.SetSize(numchains+1) ;
			Chains[numchains].SetSize(0,256) ;
			CPixelArray &ChainBuf = Chains[numchains] ;

			ChainBuf.SetSize(m_w*m_h) ;

			x = i % m_w ;
			y = i / m_w ;

			// This method will go through all of 
			// the pixel neighbors of the current
			// point, make a record of them, and 
			// delete them from the buffer.
			nCurStackPos = 0 ;
			XStack[nCurStackPos] = x ;
			YStack[nCurStackPos] = y ;
			VStack[nCurStackPos] = v ;
			DirStack[nCurStackPos] = 0 ;
			numpixels = 0 ;
			ChainBuf[numpixels++].Set (x,y,v,0) ;
			SetAt(x,y,0) ;
			bZeroReached = false ;

			do
			{
				for (j=DirStack[nCurStackPos]; j<8; j++)
				{
					x = XStack[nCurStackPos] + xincs[j] ;
					y = YStack[nCurStackPos] + yincs[j] ;
					v = GetAt(x, y);
					if (v && x>=0 && y>=0 && x<m_w && y<m_h)
					{
//						TRACE("Add %d, %d, %d\n", x, y, v) ;
						DirStack[nCurStackPos] = j ;
						nCurStackPos++ ;
						XStack[nCurStackPos] = x ;
						YStack[nCurStackPos] = y ;
						VStack[nCurStackPos] = v ;
						DirStack[nCurStackPos] = 0 ;
						ChainBuf[numpixels++].Set(x,y,v,0) ;
						SetAt(x, y, 0) ;
						break;
					}
				}

				if (j==8)
					nCurStackPos-- ;

				// If we started in the middle of a chain
				// and reached the end, we should switch 
				// the order of the pixels here...
				if (nCurStackPos==0 && !bZeroReached)
				{
					bZeroReached = true ;
					if (bContiguousChain)
					{
						ChainBuf.SetSize(numpixels,numelem) ;
						ChainBuf.ReverseDirection() ;
						ChainBuf.SetSize(numelem,numelem) ;
					}
				}
			}
			while (nCurStackPos >= 0) ;
			//numpixels = ChainBuf.GetSize() ;
			ChainBuf.SetSize(numpixels,numpixels) ;
			
			//AddToPixelChain(x, y, v, Chains[numchains]) ;
			if (bSort)
				SortPixelArray(Chains[numchains]) ;
			numchains++;
		}
	}

	delete [] XStack ;
	delete [] YStack ;
	delete [] VStack ;
	delete [] DirStack ;

	if (bReplot)
		PlotPixelChains(Chains) ;
}


// This method converts the pixel runs to chains of pixels.
// In the process, it calculates the length of these chains.
// Any chain less than nMinLength is deleted.  Chains are 
// not ordered, but due to the search process may appear
// ordered.  We will incorporate an ordering routine later
// on.

void CPixelBuffer::FindRegions(CPixelChains &Chains, int nMinAmount, int nMaxAmount)
{
	int	numelem = m_w * m_h;
	int	x, y, v, numchains, i, j, numpixels ;
	int  *XStack ;
	int  *YStack ;
	int  *VStack ;
	char *DirStack; 
	long nCurStackPos ;
	BOOL bZeroReached ;
	CPixel *pPixels ;
	int	 nCnt = 0;

	if (nMinAmount<0)
		nMinAmount = 0 ;
	if (nMaxAmount<0)
		nMaxAmount = numelem ;

	// First, label the regions as unclassified by giving them an
	// index of -1 ;
	for (i=0; i<m_size; i++)
	{
		if (m_buf[i])
		{
			m_buf[i] = -1 ;
			nCnt++ ;
		}
	}

	Chains.SetSize(0) ;
	numchains = 0 ;

	// We must create a stack so that we don't 
	// cause any stack crashes...
	XStack = new int [numelem] ;
	YStack = new int [numelem] ;
	VStack = new int [numelem] ;
	DirStack   = new char [numelem] ;

	pPixels = (CPixel *) new unsigned char [ m_w*m_h * sizeof (CPixel)] ;

	for (i=0; i<numelem; i++)
	{
		v = m_buf[i] ;
		if (v != 0)
		{
			Chains.SetSize(numchains+1) ;
			Chains[numchains].SetSize(0,256) ;
			CPixelArray &ChainBuf = Chains[numchains] ;

//			ChainBuf.SetSize(m_w*m_h) ;
//			pPixels = ChainBuf.GetData() ;

			x = i % m_w ;
			y = i / m_w ;

			// This method will go through all of 
			// the pixel neighbors of the current
			// point, make a record of them, and 
			// delete them from the buffer.
			nCurStackPos = 0 ;
			XStack[nCurStackPos] = x ;
			YStack[nCurStackPos] = y ;
			VStack[nCurStackPos] = v ;
			DirStack[nCurStackPos] = 0 ;
			numpixels = 0 ;
			pPixels[numpixels++].Set (x,y,numchains+1,0) ;
			SetAtFast(x,y,0) ;
			bZeroReached = false ;

			do
			{
				for (j=DirStack[nCurStackPos]; j<8; j++)
				{
					x = XStack[nCurStackPos] + xincs[j] ;
					y = YStack[nCurStackPos] + yincs[j] ;
					v = GetAt(x, y);
					if (v && x>=0 && y>=0 && x<m_w && y<m_h)
					{
						DirStack[nCurStackPos] = j ;
						nCurStackPos++ ;
						XStack[nCurStackPos] = x ;
						YStack[nCurStackPos] = y ;
						VStack[nCurStackPos] = v ;
						DirStack[nCurStackPos] = 0 ;
						pPixels[numpixels++].Set(x,y,numchains+1,0) ;
						SetAtFast(x, y, 0) ;
						break;
					}
				}

				if (j==8)
					nCurStackPos-- ;

				// If we started in the middle of a chain
				// and reached the end, we should switch 
				// the order of the pixels here...
				if (nCurStackPos==0 && !bZeroReached)
					bZeroReached = true ;
			}
			while (nCurStackPos >= 0) ;
			//numpixels = ChainBuf.GetSize() ;

			if (numpixels>=nMinAmount && numpixels<=nMaxAmount)
			{
 				ChainBuf.CopyPtr ( pPixels, numpixels ) ;
				numchains++;
			}
		}
	}
	delete pPixels ;

	// Now, replot the pixels, but assign the array values
	for (i=0; i<numchains; i++)
	{
		CPixelArray &ChainBuf = Chains.ElementAt(i) ;
		CPixel      *pPixels ;

		numpixels = ChainBuf.GetSize() ;
		pPixels = ChainBuf.GetData() ;

		for (j=0; j<numpixels; j++)
		{
			SetAtFast ( pPixels->x, pPixels->y, pPixels->v ) ;
			pPixels++ ;
		}
	}

	delete [] XStack ;
	delete [] YStack ;
	delete [] VStack ;
	delete [] DirStack ;
}


void CPixelBuffer::FindRegions(CPixelArray &centers, int nMinSize, int nMaxSize)
{
	int	numelem = m_w * m_h;
	int	x, y, v, i, j, numpixels ;
	int  *XStack ;
	int  *YStack ;
	int  *VStack ;
	char *DirStack; 
	long nCurStackPos ;
	BOOL bZeroReached ;
	CPixel *pArray ;
	CPixelArray testcenters ;

	// We must create a stack so that we don't 
	// cause any stack crashes...
	XStack = new int [numelem] ;
	YStack = new int [numelem] ;
	VStack = new int [numelem] ;
	DirStack   = new char [numelem] ;

	testcenters.SetSize(m_w*m_h) ;
	pArray = testcenters.GetData() ;
	numpixels = 0 ;

	for (i=0; i<numelem; i++)
	{
		v = m_buf[i] ;
		if (v != 0)
		{
			x = i % m_w ;
			y = i / m_w ;

			// This method will go through all of 
			// the pixel neighbors of the current
			// point, make a record of them, and 
			// delete them from the buffer.
			nCurStackPos = 0 ;
			XStack[nCurStackPos] = x ;
			YStack[nCurStackPos] = y ;
			VStack[nCurStackPos] = v ;
			DirStack[nCurStackPos] = 0 ;
			
			pArray->Set(x,y,255,1) ;

			SetAtFast(x,y,0) ;
			bZeroReached = false ;

			do
			{
				for (j=DirStack[nCurStackPos]; j<8; j++)
				{
					x = XStack[nCurStackPos] + xincs[j] ;
					y = YStack[nCurStackPos] + yincs[j] ;
					v = GetAt(x, y);
					if (v && x>=0 && y>=0 && x<m_w && y<m_h)
					{
						DirStack[nCurStackPos] = j ;
						nCurStackPos++ ;
						XStack[nCurStackPos] = x ;
						YStack[nCurStackPos] = y ;
						VStack[nCurStackPos] = v ;
						DirStack[nCurStackPos] = 0 ;
						pArray->Add(x,y,0,1) ;
						SetAtFast(x, y, 0) ;
						break;
					}
				}

				if (j==8)
					nCurStackPos-- ;
			}
			while (nCurStackPos >= 0) ;

			// Now, find the center location of the region
			pArray->x /= pArray->n ;
			pArray->y /= pArray->n ;

			pArray++ ;
			numpixels++;
		}
	}

	testcenters.SetSize(numpixels) ;
	centers.SetSize(0) ;

	for (i=0; i<numpixels; i++)
	{
		if (	testcenters[i].n > nMinSize
			 && testcenters[i].n < nMaxSize )
		{
			testcenters[i].v = 255 ;
			SetAt(testcenters[i]) ;
			centers.Add (testcenters[i]) ;
		}
	}

	delete [] XStack ;
	delete [] YStack ;
	delete [] VStack ;
	delete [] DirStack ;
}

void CPixelBuffer::RemoveBigNoise(CPixelBuffer *pBuf, int nMinLength)
{
    //ASSERT(pBuf);
    if (!Init(pBuf->m_w, pBuf->m_h, m_nStepSize))
        return;

	Copy(*pBuf) ;

	// First, add all the non zero points to an array to make our
	// searches smaller
	int i, j ;
	int chainlen ;
	CPixelChains PixelChains ;

	FindPixelChains(PixelChains,false,false,false) ;

	// Now, we have a list of point chains of varying lengths.  
	// We will list through these chains and plot them on the
	// buffer.  However, we will ignore chains that are shorter
	// than the minimum length.
	for (i=0; i<PixelChains.GetSize(); i++)
	{
		CPixelArray &Arr = PixelChains[i] ;

		chainlen = Arr.GetSize();
		if (chainlen >= nMinLength)
		{
			for (j=0; j<chainlen; j++)
				SetAt(Arr[j]) ;
		}
	}
}

// We're just doing a simple 3x3 median filter here...
int compareMedian(const void *elem1, const void *elem2)
{
	if ( *(int*)elem1 < *(int*)elem2)
		return -1;
	else 
	if ( *(int*)elem1 > *(int*)elem2)
		return 1;
	else
		return 0;
}

int CPixelBuffer::IsInBounds (int x, int y)
{
	if (x<0 || x>=m_w || y<0 || y>=m_h)
		return 0 ;
	else
		return 1 ;
}

int CPixelBuffer::OffsetValue (int x, int y)
{
	int rval ;

	rval = x + y*m_w ;
	return rval ;
}

int CPixelBuffer::OffsetValue (CPoint &pt)
{
	int rval ;

	rval = pt.x + pt.y*m_w ;
	return rval ;
}

int CPixelBuffer::InitRadiusSearch(int r)
{
	CPoint RadiusPoints[512] ;

	int i ;

	if (r>64)
		return 0 ;

	m_nNumRadiusPoints = GetCircleCoords ( 0, 0, r, RadiusPoints ) ;
	if (m_nNumRadiusPoints<=0)
	{
		m_nNumRadiusPoints = 0 ;
		return 0;
	}

	if (m_pRadiusOffsets)
		delete m_pRadiusOffsets ;
	if (m_pRadiusPoints)
		delete m_pRadiusPoints ;

	m_pRadiusOffsets = new int [m_nNumRadiusPoints] ;
	m_pRadiusPoints = new CPoint [m_nNumRadiusPoints] ;

	// Copy the values
	memcpy (m_pRadiusPoints, RadiusPoints, m_nNumRadiusPoints*sizeof(CPoint)) ;

	// Calculate the offsets
	for (i=0; i<m_nNumRadiusPoints; i++)
		m_pRadiusOffsets[i] = OffsetValue(m_pRadiusPoints[i]) ;

	return m_nNumRadiusPoints ;
}

void CPixelBuffer::MedianRadius(CPixelBuffer *pbuf, int nRadius)
{
    //ASSERT(pbuf);
	int x, y, i;
	int values[512] ;
	int nNumRadiusVals ;
	int nNumVals, nOffset, nNumValsH ;
	CPoint pt ;

	if (pbuf->m_nStepSize != 4)
		return ;

    if (!Init(pbuf->m_w, pbuf->m_h, 4))
        return;

	nNumRadiusVals = InitRadiusSearch(nRadius) ;
	nNumVals = nNumRadiusVals ;
	nNumValsH = nNumRadiusVals/2 ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (x>nRadius && y>nRadius && x<m_w-nRadius && y<m_h-nRadius)
			{
				nOffset = OffsetValue(x,y) ;

				for (i=0; i<nNumRadiusVals; i++)
					values[i] = pbuf->m_buf[m_pRadiusOffsets[i]+nOffset] ;
				qsort (values, nNumRadiusVals, sizeof(int), compareMedian) ;
				SetAt(x,y,values[nNumValsH]) ;
			}
			else
			{
				nNumVals = 0 ;
				for (i=0; i<nNumRadiusVals; i++)
				{
					pt.x = m_pRadiusPoints[i].x + x ;
					pt.y = m_pRadiusPoints[i].y + y ;
					if (IsInBounds(pt.x, pt.y))
					{
						values[nNumVals] = pbuf->GetAtFast(pt) ;
						nNumVals++ ;
					}
				}
				if (nNumVals)
				{
					qsort (values, nNumVals, sizeof(int), compareMedian) ;
					SetAt(x,y,values[nNumVals / 2]) ;
				}
			}
		}
	}
}

void CPixelBuffer::Median11x11(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	int x, y, i, j, t;
	int values[121] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			t = 0 ;
			for (i=-5; i<=5; i++)
			{
				for (j=-5; j<=5; j++)
				{
					values[t] = pbuf->GetAt(x+i, y+j) ;
					t++;
				}
			}
			
			// Now, sort them...
			qsort (values, 121, sizeof(int), compareMedian) ;

			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			SetAt(x, y, values[60]) ;
		}
	}
}

void CPixelBuffer::Median7x7(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	int x, y, i, j, t;
	int values[49] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			t = 0 ;
			for (i=-3; i<=3; i++)
			{
				for (j=-3; j<=3; j++)
				{
					values[t] = pbuf->GetAt(x+i, y+j) ;
					t++;
				}
			}
			
			// Now, sort them...
			qsort (values, 49, sizeof(int), compareMedian) ;

			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			SetAt(x, y, values[24]) ;
		}
	}
}

void CPixelBuffer::Median5x5(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	int x, y, i, j, t;
	int values[25] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			t = 0 ;
			for (i=-2; i<=2; i++)
			{
				for (j=-2; j<=2; j++)
				{
					values[t] = pbuf->GetAt(x+i, y+j) ;
					t++;
				}
			}
			
			// Now, sort them...
			qsort (values, 25, sizeof(int), compareMedian) ;

			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			SetAt(x, y, values[12]) ;
		}
	}
}

#define ELEM_SWAP(a,b) { register int t=(a);(a)=(b);(b)=t; }

int quick_select_fast(int *pArr, int nNumElem)
{
	return 0 ;
}

int quick_select(int *arr, int nNumElem)
{
	int low, high ;
	int median;
	int middle, ll, hh;
	
	low = 0 ; 
	high = nNumElem-1 ; 
	median = (low + high) / 2;
	
	for (;;) 
	{
		/* One element only */
		if (high <= low) 
			return arr[median] ;

		if (high == low + 1) 
		{ 
			/* Two elements only */
			if (arr[low] > arr[high])
				ELEM_SWAP(arr[low], arr[high]) ;
			return arr[median] ;
		}
		
		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;
		if (arr[middle] > arr[high]) 
			ELEM_SWAP(arr[middle], arr[high]) ;
		if (arr[low] > arr[high]) 
			ELEM_SWAP(arr[low], arr[high]) ;
		if (arr[middle] > arr[low]) 
			ELEM_SWAP(arr[middle], arr[low]) ;
		
		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(arr[middle], arr[low+1]) ;
		
		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;) 
		{
			do 
			{	
				ll++; 
			} while (arr[low] > arr[ll]) ;
			do 
			{
				hh--; 
			} while (arr[hh] > arr[low]) ;
			if (hh < ll)
				break;
			ELEM_SWAP(arr[ll], arr[hh]) ;
		}
		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(arr[low], arr[hh]) ;
		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}
#undef ELEM_SWAP

void CPixelBuffer::FastGet3x3Values(int x, int y, int *values)
{
/*
	int *psrc = m_buf ;
	int i = Index(x-1,y-1) ;
	int w = m_w ;

	_asm
	{
		mov edi, values

		mov esi, psrc
		mov edx, i
		shl edx, 2
		add esi, edx

		mov edx, w
		shl edx, 2
		mov ecx, edx
		shl ecx, 1

		mov eax, dword ptr [esi]
		mov dword ptr [edi+0], eax
		mov eax, dword ptr [esi+4]
		mov dword ptr [edi+4], eax
		mov eax, dword ptr [esi+8]
		mov dword ptr [edi+8], eax

		mov eax, dword ptr [esi+edx]
		mov dword ptr [edi+12], eax
		mov eax, dword ptr [esi+edx+4]
		mov dword ptr [edi+16], eax
		mov eax, dword ptr [esi+edx+8]
		mov dword ptr [edi+20], eax

		mov eax, dword ptr [esi+ecx]
		mov dword ptr [edi+24], eax
		mov eax, dword ptr [esi+ecx+4]
		mov dword ptr [edi+28], eax
		mov eax, dword ptr [esi+ecx+8]
		mov dword ptr [edi+32], eax
	}
*/
}

void CPixelBuffer::Median3x3(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	int x, y, t;
	int values[9] ;
	CPixelBuffer temp ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	for (y=1; y<m_h-1; y++)
	{
		for (x=1; x<m_w-1; x++)
		{
			// First, collect the values...
			pbuf->FastGet3x3Values(x,y,values) ;

			// Now, sort them...
			t = quick_select(values, 9);

			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			SetAtFast(x, y, t) ;
		}
	}
}

void CPixelBuffer::Median3x3(CPixelBuffer *pbuf, long nSearchVal)
{
    //ASSERT(pbuf);
	int x, y, i, j, t, cnt, ind, nVal, done ;
	int values[9] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;
	ind = 0 ;
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++, ind++)
		{
			nVal = pbuf->GetAt(x, y) ;
			if (nVal==nSearchVal)
			{
				cnt = 0 ;
				// First, collect the values...
				nVal = pbuf->GetAt(x-1, y-1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x  , y-1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x+1, y-1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x-1, y) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x  , y) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x+1, y) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x-1, y+1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x  , y+1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;
				nVal = pbuf->GetAt(x+1, y+1) ;
				if (nVal!=nSearchVal)
					values[cnt++] = nVal ;

				// Now, sort them...
				if (cnt>1)
				{
					for (i=0; i<cnt; i++)
					{
						done = 0 ;

						for (j=0; j<cnt-1; j++)
						{
							if (values[j] > values[j+1])
							{
								done++ ;
								t = values[j] ;
								values[j] = values[j+1] ;
								values[j+1] = t ;
							}
						}

						// if no switches were done, that means we're
						// already sorted...
						if (done==0)
							break;
					}
					nVal = values[cnt/2] ;
				}
				else
				if (cnt==1)
					nVal = values[0] ;
			}
			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			SetAt(x, y, nVal) ;
		}
	}
}

void CPixelBuffer::Peak3x3(CPixelBuffer *pbuf, long nSearchVal)
{
    //ASSERT(pbuf);
	int x, y, i, j, maxv ;
	int values[9] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;
	j = 0 ;
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++, j++)
		{
			maxv = pbuf->m_buf[j] ;

			if (maxv==nSearchVal)
			{
				// First, collect the values...
				values[0] = pbuf->GetAt(x-1, y-1) ;
				values[1] = pbuf->GetAt(x  , y-1) ;
				values[2] = pbuf->GetAt(x+1, y-1) ;
				values[3] = pbuf->GetAt(x-1, y) ;
				values[4] = pbuf->GetAt(x  , y) ;
				values[5] = pbuf->GetAt(x+1, y) ;
				values[6] = pbuf->GetAt(x-1, y+1) ;
				values[7] = pbuf->GetAt(x  , y+1) ;
				values[8] = pbuf->GetAt(x+1, y+1) ;

				// Now, sort them...
				maxv = values[0] ;
				for (i=1; i<9; i++)
				{
					if (values[i]>maxv)
						maxv = values[i] ;
				}

				// Finally since we've sorted them, we pick
				// the middle value (at index 4) and stuff it
				// into our current position.
				m_buf[j] = maxv ;
			}

			m_buf[j] = maxv ;
		}
	}
}

void CPixelBuffer::Peak3x3(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	int x, y, i, j, maxv ;
	int values[9] ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;
	j = 0 ;
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++,j++)
		{
			// First, collect the values...
			values[0] = pbuf->GetAt(x-1, y-1) ;
			values[1] = pbuf->GetAt(x  , y-1) ;
			values[2] = pbuf->GetAt(x+1, y-1) ;
			values[3] = pbuf->GetAt(x-1, y) ;
			values[4] = pbuf->GetAt(x  , y) ;
			values[5] = pbuf->GetAt(x+1, y) ;
			values[6] = pbuf->GetAt(x-1, y+1) ;
			values[7] = pbuf->GetAt(x  , y+1) ;
			values[8] = pbuf->GetAt(x+1, y+1) ;

			// Now, sort them...
			maxv = values[0] ;
			for (i=1; i<9; i++)
			{
				if (values[i]>maxv)
					maxv = values[i] ;
			}

			// Finally since we've sorted them, we pick
			// the middle value (at index 4) and stuff it
			// into our current position.
			m_buf[j] = maxv ;
		}
	}
}

static int DiscreteLaplacianArr[9] = { -1,  -1, -1
								  , -1,  8,  -1
								  , -1, -1, -1 } ;
static int DiscreteLaplacianInvArr[9] = { 1, 1, 1
								  , 1, -8, 1
								  , 1, 1, 1 } ;

void CPixelBuffer::DiscreteLaplacian(CPixelBuffer *pbuf, BOOL bInv)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, r ;

	if (bInv)
	{
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++)
			{
				r = 0 ;
				r += pbuf->GetAt (x-1, y-1) * DiscreteLaplacianInvArr[0] ;
				r += pbuf->GetAt (x  , y-1) * DiscreteLaplacianInvArr[1] ;
				r += pbuf->GetAt (x+1, y-1) * DiscreteLaplacianInvArr[2] ;
				r += pbuf->GetAt (x-1, y  ) * DiscreteLaplacianInvArr[3] ;
				r += pbuf->GetAt (x  , y  ) * DiscreteLaplacianInvArr[4] ;
				r += pbuf->GetAt (x+1, y  ) * DiscreteLaplacianInvArr[5] ;
				r += pbuf->GetAt (x-1, y+1) * DiscreteLaplacianInvArr[6] ;
				r += pbuf->GetAt (x  , y+1) * DiscreteLaplacianInvArr[7] ;
				r += pbuf->GetAt (x+1, y+1) * DiscreteLaplacianInvArr[8] ;

				SetAt(x,y,r) ;
			}
		}
	}
	else
	{
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++)

			{
				r = 0 ;
				r += pbuf->GetAt (x-1, y-1) * DiscreteLaplacianArr[0] ;
				r += pbuf->GetAt (x  , y-1) * DiscreteLaplacianArr[1] ;
				r += pbuf->GetAt (x+1, y-1) * DiscreteLaplacianArr[2] ;
				r += pbuf->GetAt (x-1, y  ) * DiscreteLaplacianArr[3] ;
				r += pbuf->GetAt (x  , y  ) * DiscreteLaplacianArr[4] ;
				r += pbuf->GetAt (x+1, y  ) * DiscreteLaplacianArr[5] ;
				r += pbuf->GetAt (x-1, y+1) * DiscreteLaplacianArr[6] ;
				r += pbuf->GetAt (x  , y+1) * DiscreteLaplacianArr[7] ;
				r += pbuf->GetAt (x+1, y+1) * DiscreteLaplacianArr[8] ;

				SetAt(x,y,r) ;
			}
		}
	}
}

static int Kirsh[8][9] = { {  5,  5,  5
				    , -3,  0, -3
				    , -3, -3, -3 },
					{ -3,  5,  5
				    , -3,  0,  5
				    , -3, -3, -3 },
					{ -3, -3,  5
				    , -3,  0,  5
				    , -3, -3,  5 },
					{ -3, -3, -3
				    , -3,  0,  5
				    , -3,  5,  5 },
					{ -3, -3, -3
				    , -3,  0, -3
				    ,  5,  5,  5 },
					{ -3, -3, -3
				    ,  5,  0, -3
				    ,  5,  5, -3 },
					{  5, -3, -3
				    ,  5,  0, -3
				    ,  5, -3, -3 },
					{  5,  5, -3
				    ,  5,  0, -3
				    , -3, -3, -3 }
					};





void CPixelBuffer::Kirsch(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h, 4))
        return;

	int temparr[8], x, y, d, maxr, maxi, r ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			maxr = 0 ;
			maxi = 0 ;
			temparr[0] = pbuf->GetAt (x-1, y-1) ;
			temparr[1] = pbuf->GetAt (x  , y-1) ;
			temparr[2] = pbuf->GetAt (x+1, y-1) ;
			temparr[3] = pbuf->GetAt (x-1, y  ) ;
			temparr[4] = pbuf->GetAt (x  , y  ) ;
			temparr[5] = pbuf->GetAt (x+1, y  ) ;
			temparr[6] = pbuf->GetAt (x-1, y+1) ;
			temparr[7] = pbuf->GetAt (x  , y+1) ;
			temparr[8] = pbuf->GetAt (x+1, y+1) ;
			for (d=0; d<8; d++)
			{
				r = 0 ;
				r += temparr[0] * Kirsh[d][0] ;
				r += temparr[1] * Kirsh[d][1] ;
				r += temparr[2] * Kirsh[d][2] ;
				r += temparr[3] * Kirsh[d][3] ;
				r += temparr[4] * Kirsh[d][4] ;
				r += temparr[5] * Kirsh[d][5] ;
				r += temparr[6] * Kirsh[d][6] ;
				r += temparr[7] * Kirsh[d][7] ;
				r += temparr[8] * Kirsh[d][8] ;
				if (r > maxr)
				{
					maxr = r ;
					maxi = d ;
				}
			}

			SetAt(x,y,maxr) ;
		}
	}
}

static int PrewittArr[8][9] = { {  1,  1,  1
								,  0,  0,  0
								, -1, -1, -1 },
								{  0,  1,  1
								, -1,  0,  1
								, -1, -1,  0 },
								{ -1,  0,  1
								, -1,  0,  1
								, -1,  0,  1 },
								{ -1, -1,  0
								, -1,  0,  1
								,  0,  1,  1 },
								{ -1, -1, -1
								,  0,  0,  0
								,  1,  1,  1 },
								{  0, -1, -1
								,  1,  0, -1
								,  1,  1,  0 },
								{  1,  0, -1
								,  1,  0, -1
								,  1,  0, -1 },
								{  1,  1,  0
								,  1,  0, -1
								,  0, -1, -1 }
								};


static int Sobel5x5Arr4[12*25]  = {    1,  1, -1, -1,  0
									,  1,  1, -1, -1,  0
									,  1,  1, -1, -1,  0
									,  1,  1, -1, -1,  0
									,  0,  0,  0,  0,  0  ,

									  -1, -1,  1,  1,  0
									, -1, -1,  1,  1,  0
									, -1, -1,  1,  1,  0
									, -1, -1,  1,  1,  0
									,  0,  0,  0,  0,  0  ,

									   1,  1,  1,  1,  0
									,  1,  1,  1,  1,  0
									, -1, -1, -1, -1,  0
									, -1, -1, -1, -1,  0
									,  0,  0,  0,  0,  0  ,

									  -1, -1, -1, -1,  0
									, -1, -1, -1, -1,  0
									,  1,  1,  1,  1,  0
									,  1,  1,  1,  1,  0
									,  0,  0,  0,  0,  0  ,

									  -1, -1,  1,  1,  0
									, -1, -1,  1,  1,  0
									,  1,  1, -1, -1,  0
									,  1,  1, -1, -1,  0
									,  0,  0,  0,  0,  0  ,

									   1,  1, -1, -1,  0
									,  1,  1, -1, -1,  0
									, -1, -1,  1,  1,  0
									, -1, -1,  1,  1,  0
									,  0,  0,  0,  0,  0  ,

									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  ,

									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  

									};
static int Sobel5x5Arr3[12*25]  = {    0,  0, -1,  0,  0
									,  0,  0, -1,  0,  0
									,  1,  1,  0,  1,  1
									,  0,  0, -1,  0,  0
									,  0,  0, -1,  0,  0  ,

									   0,  0,  1,  0,  0
									,  0,  0,  1,  0,  0
									, -1, -1,  0, -1, -1
									,  0,  0,  1,  0,  0
									,  0,  0,  1,  0,  0  ,

									   1,  0,  0,  0, -1
									,  0,  1,  0, -1,  0
									,  0,  0,  0,  0,  0
									,  0, -1,  0,  1,  0
									, -1,  0,  0,  0,  1  ,

									  -1,  0,  0,  0,  1
									,  0, -1,  0,  1,  0
									,  0,  0,  0,  0,  0
									,  0,  1,  0, -1,  0
									,  1,  0,  0,  0, -1  ,


									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  ,

									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  ,

									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  ,

									   0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0  
									};
static int Sobel5x5Arr[8*25] =   {    1,  0,  2,  0,  1
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									, -1,  0, -2,  0, -1  ,

									   0,  0,  1,  0,  0
									,  0,  0,  0,  2,  0
									, -1,  0,  0,  0,  1
									,  0, -2,  0,  0,  0
									,  0,  0, -1,  0,  0  ,

									  -1,  0,  0,  0,  1
									,  0,  0,  0,  0,  0
									, -2,  0,  0,  0,  2
									,  0,  0,  0,  0,  0
									, -1,  0,  0,  0,  1  ,

									   0,  0, -1,  0,  0
									,  0, -2,  0,  0,  0
									, -1,  0,  0,  0,  1
									,  0,  0,  0,  2,  0
									,  0,  0,  1,  0,  0  ,

									  -1,  0, -2,  0, -1
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  1,  0,  2,  0,  1  ,

									   0,  0, -1,  0,  0
									,  0,  0,  0, -2,  0
									,  1,  0,  0,  0, -1
									,  0,  2,  0,  0,  0
									,  0,  0,  1,  0,  0  ,

									   1,  0,  0,  0, -1
									,  0,  0,  0,  0,  0
									,  2,  0,  0,  0, -2
									,  0,  0,  0,  0,  0
									,  1,  0,  0,  0, -1  ,

									   0,  0,  1,  0,  0
									,  0,  2,  0,  0,  0
									,  1,  0,  0,  0, -1
									,  0,  0,  0, -2,  0
									,  0,  0, -1,  0,  0  ,
									};
static int Sobel5x5Arr2[8*25] =   {    2,  3,  4,  3,  2
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									, -2, -3, -4, -3, -2  ,

									   0,  0,  2,  3,  0
									,  0,  0,  0,  4,  3
									, -2,  0,  0,  0,  2
									, -3, -4,  0,  0,  0
									,  0, -3, -2,  0,  0  ,

									  -2,  0,  0,  0,  2
									, -3,  0,  0,  0,  3
									, -4,  0,  0,  0,  4
									, -3,  0,  0,  0,  3
									, -2,  0,  0,  0,  2  ,

									   0, -3, -2,  0,  0
									, -3, -4,  0,  0,  0
									, -2,  0,  0,  0,  2
									,  0,  0,  0,  4,  3
									,  0,  0,  2,  3,  0  ,

									  -2, -3, -4, -3, -2
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  0,  0,  0,  0,  0
									,  2,  3,  4,  3,  2  ,

									   0,  0, -2, -3,  0
									,  0,  0,  0, -4, -3
									,  2,  0,  0,  0, -2
									,  3,  4,  0,  0,  0
									,  0,  3,  2,  0,  0  ,

									   2,  0,  0,  0, -2
									,  3,  0,  0,  0, -3
									,  4,  0,  0,  0, -4
									,  3,  0,  0,  0, -3
									,  2,  0,  0,  0, -2  ,

									   0,  3,  2,  0,  0
									,  3,  4,  0,  0,  0
									,  2,  0,  0,  0, -2
									,  0,  0,  0, -4, -3
									,  0,  0, -2, -3,  0  ,
									};

void CPixelBuffer::Sobel5x5(CPixelBuffer *pbuf, CPixelBuffer *pmask, int nType)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, d, maxr, maxi, r, i ;
	int *pArr ;

	switch (nType)
	{
	case 0:
		pArr = Sobel5x5Arr ;
		break;
	case 1:
		pArr = Sobel5x5Arr2 ;
		break;
	case 2:
		pArr = Sobel5x5Arr3 ;
		break;
	case 3:
		pArr = Sobel5x5Arr4 ;
		break;
	}

	for (y=0,i=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++, i++)
		{
			if ( (pmask && pmask->m_buf[i]) || (!pmask) )
			{
				maxr = 0 ;
				maxi = 0 ;
				for (d=0; d<8*25; d+=25)
				{
					r = 0 ;
					r += pbuf->GetAt (x-2, y-2) * pArr[d+0] ;
					r += pbuf->GetAt (x-1, y-2) * pArr[d+1] ;
					r += pbuf->GetAt (x  , y-2) * pArr[d+2] ;
					r += pbuf->GetAt (x+1, y-2) * pArr[d+3] ;
					r += pbuf->GetAt (x+2, y-2) * pArr[d+4] ;
					r += pbuf->GetAt (x-2, y-1) * pArr[d+5] ;
					r += pbuf->GetAt (x-1, y-1) * pArr[d+6] ;
					r += pbuf->GetAt (x  , y-1) * pArr[d+7] ;
					r += pbuf->GetAt (x+1, y-1) * pArr[d+8] ;
					r += pbuf->GetAt (x+2, y-1) * pArr[d+9] ;
					r += pbuf->GetAt (x-2, y  ) * pArr[d+10] ;
					r += pbuf->GetAt (x-1, y  ) * pArr[d+11] ;
					r += pbuf->GetAt (x  , y  ) * pArr[d+12] ;
					r += pbuf->GetAt (x+1, y  ) * pArr[d+13] ;
					r += pbuf->GetAt (x+2, y  ) * pArr[d+14] ;
					r += pbuf->GetAt (x-2, y+1) * pArr[d+15] ;
					r += pbuf->GetAt (x-1, y+1) * pArr[d+16] ;
					r += pbuf->GetAt (x  , y+1) * pArr[d+17] ;
					r += pbuf->GetAt (x+1, y+1) * pArr[d+18] ;
					r += pbuf->GetAt (x+2, y+1) * pArr[d+19] ;
					r += pbuf->GetAt (x-2, y+2) * pArr[d+20] ;
					r += pbuf->GetAt (x-1, y+2) * pArr[d+21] ;
					r += pbuf->GetAt (x  , y+2) * pArr[d+22] ;
					r += pbuf->GetAt (x+1, y+2) * pArr[d+23] ;
					r += pbuf->GetAt (x+2, y+2) * pArr[d+24] ;
					if (r > maxr)
					{
						maxr = r ;
						maxi = d ;
					}
				}
				SetAt(x,y,maxr) ;
			}

		}
	}
}

void CPixelBuffer::Prewitt5x5(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, d, maxr, maxi, r ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			maxr = 0 ;
			maxi = 0 ;
			for (d=0; d<8; d++)
			{
				r = 0 ;
				r += pbuf->GetAt (x-2, y-2) * PrewittArr[d][0] ;
				r += pbuf->GetAt (x-1, y-2) * PrewittArr[d][1] ;
				r += pbuf->GetAt (x  , y-2) * PrewittArr[d][2] ;
				r += pbuf->GetAt (x+1, y-2) * PrewittArr[d][3] ;
				r += pbuf->GetAt (x+2, y-2) * PrewittArr[d][4] ;
				r += pbuf->GetAt (x-2, y-1) * PrewittArr[d][5] ;
				r += pbuf->GetAt (x-1, y-1) * PrewittArr[d][6] ;
				r += pbuf->GetAt (x  , y-1) * PrewittArr[d][7] ;
				r += pbuf->GetAt (x+1, y-1) * PrewittArr[d][8] ;
				r += pbuf->GetAt (x+2, y-1) * PrewittArr[d][9] ;
				r += pbuf->GetAt (x-2, y  ) * PrewittArr[d][10] ;
				r += pbuf->GetAt (x-1, y  ) * PrewittArr[d][11] ;
				r += pbuf->GetAt (x  , y  ) * PrewittArr[d][12] ;
				r += pbuf->GetAt (x+1, y  ) * PrewittArr[d][13] ;
				r += pbuf->GetAt (x+2, y  ) * PrewittArr[d][14] ;
				r += pbuf->GetAt (x-2, y+1) * PrewittArr[d][15] ;
				r += pbuf->GetAt (x-1, y+1) * PrewittArr[d][16] ;
				r += pbuf->GetAt (x  , y+1) * PrewittArr[d][17] ;
				r += pbuf->GetAt (x+1, y+1) * PrewittArr[d][18] ;
				r += pbuf->GetAt (x+2, y+1) * PrewittArr[d][19] ;
				r += pbuf->GetAt (x-2, y+2) * PrewittArr[d][20] ;
				r += pbuf->GetAt (x-1, y+2) * PrewittArr[d][21] ;
				r += pbuf->GetAt (x  , y+2) * PrewittArr[d][22] ;
				r += pbuf->GetAt (x+1, y+2) * PrewittArr[d][23] ;
				r += pbuf->GetAt (x+2, y+2) * PrewittArr[d][24] ;
				if (r > maxr)
				{
					maxr = r ;
					maxi = d ;
				}
			}

			SetAt(x,y,maxr) ;
		}
	}
}




void CPixelBuffer::Prewitt(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, d, maxr, maxi, r ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			maxr = 0 ;
			maxi = 0 ;
			for (d=0; d<8; d++)
			{
				r = 0 ;
				r += pbuf->GetAt (x-1, y-1) * PrewittArr[d][0] ;
				r += pbuf->GetAt (x  , y-1) * PrewittArr[d][1] ;
				r += pbuf->GetAt (x+1, y-1) * PrewittArr[d][2] ;
				r += pbuf->GetAt (x-1, y  ) * PrewittArr[d][3] ;
				r += pbuf->GetAt (x  , y  ) * PrewittArr[d][4] ;
				r += pbuf->GetAt (x+1, y  ) * PrewittArr[d][5] ;
				r += pbuf->GetAt (x-1, y+1) * PrewittArr[d][6] ;
				r += pbuf->GetAt (x  , y+1) * PrewittArr[d][7] ;
				r += pbuf->GetAt (x+1, y+1) * PrewittArr[d][8] ;
				if (r > maxr)
				{
					maxr = r ;
					maxi = d ;
				}
			}

			SetAt(x,y,maxr) ;
		}
	}
}

static int SobelArr[8][9] = {   {  1,  2,  1
								,  0,  0,  0
								, -1, -2, -1 },
								{  0,  1,  2
								, -1,  0,  1
								, -2, -1,  0 },
								{ -1,  0,  1
								, -2,  0,  2
								, -1,  0,  1 },
								{ -2, -1,  0
								, -1,  0,  1
								,  0,  1,  2 },
								{ -1, -2, -1
								,  0,  0,  0
								,  1,  2,  1 },
								{  0, -1, -2
								,  1,  0, -1
								,  2,  1,  0 },
								{  1,  0, -1
								,  2,  0, -2
								,  1,  0, -1 },
								{  2,  1,  0
								,  1,  0, -1
								,  0, -1, -2 }
								};


int CPixelBuffer::FastSobel(int *values)
{
	int rval = 0;
/*
	_asm
	{
		// eax is our sum for each set
		// ebx is our max value
		// esi is our data pointer to the values
		mov ebx, 0x80000000
		mov esi, values
		
		//{  1,  2,  1	   {  0,  4,  8
		//,  0,  0,  0	   , 12, 16, 20
		//, -1, -2, -1 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+0]
		add eax, [esi+4]
		add eax, [esi+4]
		add eax, [esi+8]
		sub eax, [esi+32]
		sub eax, [esi+28]
		sub eax, [esi+28]
		sub eax, [esi+24]
		cmp eax, ebx
		jle fs_done_1
		mov ebx, eax
fs_done_1:

		//{  0,  1,  2	   {  0,  4,  8
		//, -1,  0,  1	   , 12, 16, 20
		//, -2, -1,  0 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+4]
		add eax, [esi+8]
		add eax, [esi+8]
		add eax, [esi+20]
		sub eax, [esi+28]
		sub eax, [esi+24]
		sub eax, [esi+24]
		sub eax, [esi+12]
		cmp eax, ebx
		jle fs_done_2
		mov ebx, eax
fs_done_2:

		//{ -1,  0,  1	   {  0,  4,  8
		//, -2,  0,  2	   , 12, 16, 20
		//, -1,  0,  1 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+8]
		add eax, [esi+20]
		add eax, [esi+20]
		add eax, [esi+32]
		sub eax, [esi+24]
		sub eax, [esi+12]
		sub eax, [esi+12]
		sub eax, [esi+0]
		cmp eax, ebx
		jle fs_done_3
		mov ebx, eax
fs_done_3:

		//{ -2, -1,  0	   {  0,  4,  8
		//, -1,  0,  1	   , 12, 16, 20
		//,  0,  1,  2 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+20]
		add eax, [esi+32]
		add eax, [esi+32]
		add eax, [esi+28]
		sub eax, [esi+12]
		sub eax, [esi+0]
		sub eax, [esi+0]
		sub eax, [esi+4]
		cmp eax, ebx
		jle fs_done_4
		mov ebx, eax
fs_done_4:

		//{ -1, -2, -1	   {  0,  4,  8
		//,  0,  0,  0	   , 12, 16, 20
		//,  1,  2,  1 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+32]
		add eax, [esi+28]
		add eax, [esi+28]
		add eax, [esi+24]
		sub eax, [esi+0]
		sub eax, [esi+4]
		sub eax, [esi+4]
		sub eax, [esi+8]
		cmp eax, ebx
		jle fs_done_5
		mov ebx, eax
fs_done_5:

		//{  0, -1, -2	   {  0,  4,  8
		//,  1,  0, -1	   , 12, 16, 20
		//,  2,  1,  0 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+28]
		add eax, [esi+24]
		add eax, [esi+24]
		add eax, [esi+12]
		sub eax, [esi+4]
		sub eax, [esi+8]
		sub eax, [esi+8]
		sub eax, [esi+20]
		cmp eax, ebx
		jle fs_done_6
		mov ebx, eax
fs_done_6:

		//{  1,  0, -1	   {  0,  4,  8
		//,  2,  0, -2	   , 12, 16, 20
		//,  1,  0, -1 },  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+24]
		add eax, [esi+12]
		add eax, [esi+12]
		add eax, [esi+0]
		sub eax, [esi+8]
		sub eax, [esi+20]
		sub eax, [esi+20]
		sub eax, [esi+32]
		cmp eax, ebx
		jle fs_done_7
		mov ebx, eax
fs_done_7:

		//{  2,  1,  0	   {  0,  4,  8
		//,  1,  0, -1	   , 12, 16, 20
		//,  0, -1, -2 }  , 24, 28, 32 }
		mov eax, 0
		add eax, [esi+12]
		add eax, [esi+0]
		add eax, [esi+0]
		add eax, [esi+4]
		sub eax, [esi+20]
		sub eax, [esi+32]
		sub eax, [esi+32]
		sub eax, [esi+28]
		cmp eax, ebx
		jle fs_done_8
		mov ebx, eax
fs_done_8:

		mov rval, ebx
	}
*/
	return rval ;
}


void CPixelBuffer::Sobel(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, temparr[9] ;

	for (y=1; y<m_h-1; y++)
	{
		for (x=1; x<m_w-1; x++)
		{
			pbuf->FastGet3x3Values(x,y,temparr) ;
			SetAtFast(x,y,FastSobel(temparr)) ;
		}
	}
}

void MinMax (int val, int &min, int &max)
{
	if (val<min)
		min = val ;
	if (val>max)
		max = val ;
}

void CPixelBuffer::ZeroCrossing(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	int x, y, maxi, mini;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			mini = 0x7FFFFFFF ;
			maxi = 0xFFFFFFFF ;

			MinMax(pbuf->GetAt (x-1, y-1), mini, maxi) ;
			MinMax(pbuf->GetAt (x  , y-1), mini, maxi) ;
			MinMax(pbuf->GetAt (x+1, y-1), mini, maxi) ;
			MinMax(pbuf->GetAt (x-1, y  ), mini, maxi) ;
			MinMax(pbuf->GetAt (x  , y  ), mini, maxi) ;
			MinMax(pbuf->GetAt (x+1, y  ), mini, maxi) ;
			MinMax(pbuf->GetAt (x-1, y+1), mini, maxi) ;
			MinMax(pbuf->GetAt (x  , y+1), mini, maxi) ;
			MinMax(pbuf->GetAt (x+1, y+1), mini, maxi) ;
			if (mini<0 && maxi>0)
				SetAt(x,y,maxi) ;
			else
				SetAt(x,y,0) ;
		}
	}
}



// This method uses a modified Zhang-Suen(1984) method of skeletonization.
// This method states a pixel can be deleted if two conditions are met.
// 1.  The pixel must have >=2 and <=6 neighbors.
// 2.  The deletion of the pixel cannot allow a breakage
//     in the continuity of the object...
// 
// For the purposes of this algorithm, a black pixel is a value of zero,
// whereas a white pixel is a value of nonzero.

void CPixelBuffer::Thin(CPixelBuffer *pbuf)
{
    //ASSERT(pbuf);
	CPixelChains	Chains ;
	int				i, j, rv ;
	int				numdeleted ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	// Copy the data...
	Copy(*pbuf) ;

	do
	{
		// First, identify all the pixel runs in the image.  Pixel
		// runs are sets of neighboring pixels.  The runs are sorted
		// according to intensity...
		{
			FindPixelChains(Chains, true, true);
		}
		numdeleted = 0 ;

		// Now, loop through all the chains, and delete
		// points the fulfill the thinning criteria for 
		// deleting.  We don't actually delete the pixel
		// until all the pixels have been tested...
		{

			for (i=0; i<Chains.GetSize(); i++)
			{
				CPixelArray &Arr = Chains[i] ;
				
				for (j=0; j<Arr.GetSize(); j++)
				{
					// Get the neighbor classification
					rv = ClassifyPixel(Arr[j]) ;

					//if (bCanDeletePixel[rv])
					if (ErosionMask[rv])
					{
						Arr[j].v = 0 ;
						// This erases the pixel from the buffer
						SetAt(Arr[j]) ;
						numdeleted++;
					}
				}

				//PlotPixels(Arr) ;
			}
		}

		//if (numdeleted)
		//{
			//PlotPixelChains(Chains) ;
		//}
	} 
	// If any points were deleted, we will go through
	// the process again, just in case we had really thick
	// regions...
	while (numdeleted > 0) ;
}

int CPixelBuffer::ClassifyPixel(CPixel &pix)
{
	/*
	pix.n = (GetAt(pix.x-1, pix.y-1)==0 ? 0 : 1)
		  + (GetAt(pix.x+0, pix.y-1)==0 ? 0 : 2)
		  + (GetAt(pix.x+1, pix.y-1)==0 ? 0 : 4)
		  + (GetAt(pix.x+1, pix.y+0)==0 ? 0 : 8)
		  + (GetAt(pix.x+1, pix.y+1)==0 ? 0 : 16)
		  + (GetAt(pix.x+0, pix.y+1)==0 ? 0 : 32)
		  + (GetAt(pix.x-1, pix.y+1)==0 ? 0 : 64)
		  + (GetAt(pix.x-1, pix.y+0)==0 ? 0 : 128)
		    ;
			*/
	int val1   = GetAt(pix.x-1, pix.y-1);
	int val2   = GetAt(pix.x+0, pix.y-1);
	int val4   = GetAt(pix.x+1, pix.y-1);
	int val8   = GetAt(pix.x+1, pix.y+0);
	int val16  = GetAt(pix.x+1, pix.y+1);
	int val32  = GetAt(pix.x+0, pix.y+1);
	int val64  = GetAt(pix.x-1, pix.y+1);
	int val128 = GetAt(pix.x-1, pix.y+0);
	int val; 

	val   = (val1  ==0 ? 0 : 1)
		  + (val2  ==0 ? 0 : 2)
		  + (val4  ==0 ? 0 : 4)
		  + (val8  ==0 ? 0 : 8)
		  + (val16 ==0 ? 0 : 16)
		  + (val32 ==0 ? 0 : 32)
		  + (val64 ==0 ? 0 : 64)
		  + (val128==0 ? 0 : 128)
		    ;
	pix.n = val ;
	return val ;
}

void CPixelBuffer::ClassifyPixelChains(CPixelChains &Chains)
{
	int i, j ;
	for (i=0; i<Chains.GetSize(); i++)
	{
		CPixelArray &Arr = Chains[i] ;

		for (j=0; j<Arr.GetSize(); j++)
			ClassifyPixel(Arr[j]) ;
	}
}

void CPixelBuffer::PlotPixelChains(CPixelChains &Chains)
{
	int i,j ;

	for (i=0; i<Chains.GetSize(); i++)
	{
		CPixelArray &Arr = Chains[i] ;

		for (j=0; j<Arr.GetSize(); j++)
			SetAt(Arr[j]) ;
	}
}

void CPixelBuffer::Copy(CPixelBuffer &source, CPixelBuffer &mask, BOOL bInvertMask)
{
	if (source.m_w != mask.m_w)
		return;
	if (source.m_h != mask.m_h)
		return;
    if (!Init(source.m_w, source.m_h))
        return;

	int y;

	if (bInvertMask)
	{
		for (y=0; y<m_size; y++)
		{
			if (mask.m_buf[y]==0)
				m_buf[y] = source.m_buf[y] ;
			else
				m_buf[y] = mask.m_buf[y] ;
		}
	}
	else
	{
		for (y=0; y<m_size; y++)
		{
			if (mask.m_buf[y]!=0)
				m_buf[y] = source.m_buf[y] ;
			else
				m_buf[y] = mask.m_buf[y] ;
		}
	}
}


static int PixelCompare ( const void *arg1, const void *arg2)
{
    CPixel *pix1 ;
    CPixel *pix2 ;
    
    pix1 = (CPixel*)arg1 ;
    pix2 = (CPixel*)arg2 ;
    
    if (pix1->v < pix2->v)
        return -1 ;
	if (pix1->v > pix2->v)
        return 1 ;
    return 0 ;
}

void CPixelBuffer::SortPixelArray (CPixelArray &Arr)
{
	int numpix = Arr.GetSize() ;
	if (numpix > 1)
		qsort (Arr.GetData(), numpix, sizeof(CPixel), PixelCompare) ;
}

void CPixelBuffer::PlotSprigRoots(CPixelBuffer *pSourceBuffer) 
{
	int j ;
	CPixel  *pPixels = pSourceBuffer->m_SprigRoots.GetData() ;
	int		nNumPixels = pSourceBuffer->m_SprigRoots.GetSize() ;

	for (j=0; j<nNumPixels; j++)
		SetAt(pPixels[j]) ;
}
// This method looks through all the nonzero pixels
// and determines if they are sprig roots.  A sprig
// root is a pixel that, if removed, would cause a
// loss of connectivity between two pixel regions.
// As we delete these pixels, we also store them
// in a return array so they can be restored at a 
// later time, if desired...

void CPixelBuffer::RemoveSprigRoots() 
{
	CPixel pt ;
	int    i ;

	// Empty our return array
	m_SprigRoots.SetSize(0);

	i = 0 ;
	// Loop through all the m_SprigRoots
	for (pt.y=0; pt.y<m_h; pt.y++)
	{
		for (pt.x=0; pt.x<m_w; pt.x++)
		{
			// Get the current pixel
			pt.v = m_buf[i] ;

			// Make sure this is nonzero
			if (pt.v != 0)
			{
				// Classify it
				ClassifyPixel(pt) ;
				
				// Is it a sprig root?
				if (bIsSprigRoot[pt.n])
				{
					// If so...
					// Store the current pixel in the return array
					m_SprigRoots.Add(pt) ;
					// Don't erase it yet!  We will not be
					// able to get all the sprigs unless
					// we allow the other sprig points to
					// remain...
				}
			}

			i++;
		}
	}

	// Now, loop through the stored points and erase
	for (i=0; i<m_SprigRoots.GetSize(); i++)
	{
		pt = m_SprigRoots[i] ;
		pt.v = 0 ;
		SetAt(pt) ;
	}
}


CSize CPixelBuffer::GetSize()
{
	return CSize(m_w, m_h) ;
}

//static int xstackincs[8] = {-1,0,1,1,1,0,-1,-1};
//static int ystackincs[8] = {-1,-1,-1,0,1,1,1,0};
static int xstackincs[8] = { 0, 1,-1, 0,-1, 1, 1,-1};
static int ystackincs[8] = {-1, 0, 0, 1,-1,-1, 1, 1};


void CPixelBuffer::CheckRegionNeighbor(PFINDREGIONSTACKVAL pstack, int &stackpos, int regionval)
{
	int wn ;
	int val ;
	int ix, iy ;

	wn = pstack[stackpos].whichneighbor; 
//	if (wn < 8)
	if (wn < 4)
	{
		ix = pstack[stackpos].x + xstackincs[wn] ;
		iy = pstack[stackpos].y + ystackincs[wn] ;
		val = GetAt(ix, iy) ; 
		if (val<0)
		{
			SetAt(ix,iy,regionval);
			pstack[stackpos].whichneighbor++ ;
			stackpos++;
			pstack[stackpos].rgnnum = regionval ;
			pstack[stackpos].x = ix ;
			pstack[stackpos].y = iy ;
			pstack[stackpos].whichneighbor = 0 ;
		}
		else

			pstack[stackpos].whichneighbor++ ;
	}
	else
	{
		stackpos--;
	}

	return ;
}


int CPixelBuffer::FindRegions(CPixelBuffer *pRgnBuf, int *pRegionTotals, int maxtotalelems) 
{
	int ix, iy, val ;
	int rgnnum ;
	int stackpos ;
	PFINDREGIONSTACKVAL pstack ;

	if (!pRgnBuf || pRgnBuf->m_w==0 || pRgnBuf->m_h==0)
		return 0 ;
	Copy(*pRgnBuf) ;

	// Need as many entries as the size of the mask.
	pstack = new FINDREGIONSTACKVAL[m_w*m_h] ;
	if (!pstack)
		return 0 ;

	rgnnum = 0 ;

	for (ix=0; ix<m_w; ix++)
	{
		for (iy=0; iy<m_h; iy++)
		{
			val = GetAt(ix, iy) ;

			// Is the value a pixel within the clipping region?
			if (val!=0)
			{
				// Has the value been classified yet?
				if (val<0)
				{
					// No.  This means that all no neighbors will have
					// the current classification value.
					rgnnum++;

					// Now we will recursively search all the neighbors.
					// However, to keep from a stack crash, we will
					// maintain our own stack and simply push and pop
					// till our pop level returns to zero.
					stackpos = 0 ;
					pstack[stackpos].x = ix ;
					pstack[stackpos].y = iy ;
					pstack[stackpos].whichneighbor = 0 ;
					pstack[stackpos].rgnnum = rgnnum ;
					SetAt(ix,iy,rgnnum);

					do
					{
						CheckRegionNeighbor(pstack,stackpos,rgnnum) ;
					}
					while (stackpos >= 0) ;
				}
				else
				{
					// Otherwise skip it
				}
			}
		}
	}

	// Now, loop through the entire image and count the totals for the
	// different regions...
	int total = m_w * m_h ;
	int i, elemind ;

	memset(pRegionTotals, 0, sizeof(int)*maxtotalelems) ;

	for (i=0; i<total; i++)
	{
		if (m_buf[i]>0)
		{
			elemind = m_buf[i] - 1 ;
			pRegionTotals[elemind]++;
		}
	}

	delete [] pstack ;
	return rgnnum ;
}


void CPixelBuffer::ClearRegions(CPixelBuffer *regionbuf, int allowablevalue)
{
	if (!regionbuf)
		return;
	Copy(*regionbuf) ;
	int total = m_w * m_h ;

	if (allowablevalue>0)
	{
		for (int i=0; i<total; i++)
		{
			if (m_buf[i] != allowablevalue)
			{
				if (m_buf[i] != 0)
					m_buf[i] = 0 ;
			}
		}
	}
	else
		memset(m_buf,0,total*sizeof(int)) ;
}


void CPixelBuffer::ReplaceVal(int searchval, int replaceval)
{
	int total = m_w * m_h ;
	int cnt = 0 ;

	for (int i=0; i<total; i++)
	{
		if (m_buf[i] == searchval)
		{
			m_buf[i] = replaceval ;
			cnt++;
		}
	}
}


int CPixelBuffer::CountNonZero()
{
	int total = m_w * m_h ;
	int cnt = 0 ;

	for (int i=0; i<total; i++)
	{
		if (m_buf[i])
			cnt++;
	}

	return cnt ;
}

int CPixelBuffer::CountZero()
{
	int total = m_w * m_h ;
	int cnt = 0 ;

	for (int i=0; i<total; i++)
	{
		if (m_buf[i]==0)
			cnt++;
	}

	return cnt ;
}

void CPixelBuffer::Erode(CPixelBuffer *pBuf, int nNumReps)
{
	int x, y, i, v ;
	CPixelBuffer TempCopy ;

	if (!TempCopy.Init(pBuf->m_w, pBuf->m_h))
		return;
	TempCopy.Clear() ;

	do
	{
		for (y=0; y<TempCopy.m_h; y++)
		{
			for (x=0; x<TempCopy.m_w; x++)
			{
				i = 0 ;

				v = pBuf->GetAt(x, y) ;

				i += (pBuf->GetAt(x-1, y-1)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x+1, y-1)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x-1, y+1)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x+1, y+1)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x-1, y)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x+1, y)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x, y+1)  ? 1 : 0 ) ;
				i += (pBuf->GetAt(x, y-1)  ? 1 : 0 ) ;

				//TempCopy.SetAt(x,y,(i>2 && i<=5) ? 0 : 1) ;
				TempCopy.SetAt(x,y,(i<=5) ? 0 : v) ;
			}
		}
		nNumReps--;
	} while (nNumReps);

	Copy(TempCopy) ;
}

BOOL CPixelBuffer::RegionIsInside(int searchrgn, int boundingrgn) 
{
	// Look for the a pixel inside the searchrgn
	int ix, iy, cx, cy ;

	for (ix=0; ix<m_w; ix++)
	{
		for (iy=0; iy<m_h; iy++)
		{
			if (GetAt(ix,iy)==searchrgn)
			{
				// If we found a searchrgn pixel, go till you get
				// to an edge or you find the boundingrgn.  If you
				// get to an edge, the function fails.
				cx = ix ;
				while(cx>=0)
				{
					if (GetAt(cx,iy)==boundingrgn)
						break;
					cx--;
				}
				if (cx==0)
					return false ;
				cx = ix ;
				while(cx<m_w)
				{
					if (GetAt(cx,iy)==boundingrgn)
						break;
					cx++;
				}
				if (cx==m_w)
					return false ;

				cy = iy ;
				while(cy>=0)
				{
					if (GetAt(ix,cy)==boundingrgn)
						break;
					cy--;
				}
				if (cy==0)
					return false ;
				cy = iy ;
				while(cy<m_h)
				{
					if (GetAt(ix,cy)==boundingrgn)
						break;
					cy++;
				}
				if (cy==m_h)
					return false ;
			}
		}
	}

	return true;
}


int CPixelBuffer::BoundingRegion(int x, int y) 
{
	// Look for the a pixel inside the searchrgn
	int cx, cy, brgn ;

	cx = x ;
	while(cx>=0)
	{
		brgn = GetAt(cx,y);
		if (brgn>0)
			break;
		cx--;
	}
	if (cx>=0)
		return brgn ;
	cx = x ;
	while(cx<m_w)
	{
		brgn = GetAt(cx,y);
		if (brgn>0)
			break;
		cx++;
	}
	if (cx>=m_w)
		return brgn ;


	
	cy = y ;
	while(cy>=0)
	{
		brgn = GetAt(x,cy);
		if (brgn>0)
			break;
		cy--;
	}
	if (cy>=0)
		return brgn;
	cy = y ;
	while(cy<m_h)
	{
		brgn = GetAt(x,cy);
		if (brgn>0)
			break;
		cy++;
	}
	if (cy>=m_h)
		return brgn;

	return -1;
}


void CPixelBuffer::Rotate(CPixelBuffer *pBuf, int RotationAmount)
{
	int sx, sy, sh, dx, dy, sw ;

    //ASSERT(pBuf);

	if (RotationAmount==0)
	{
		Copy(*pBuf) ;
		return;
	}

	switch (RotationAmount)
	{
	case 90:
		{
			sw = pBuf->m_w ;
			sh = pBuf->m_h ;

			// Initialize so width and height are switched
			if (!Init(sh, sw, pBuf->m_nStepSize))
				return ;
			unsigned char *sbuf = (unsigned char *)pBuf->m_buf ;
			unsigned char *dbuf = (unsigned char *)m_buf ;

			// With this, the dx = sh - sy + 1 and dy = sx
			switch (m_nStepSize)
			{
			case 1:
				for (sy=0; sy<sh; sy++)
				{
					dx = sh - sy - 1 ;

					for (sx=0; sx<sw; sx++)
					{
						dy = sx ;
						dbuf[(dx+dy*sh)] = sbuf[(sx+sy*sw)] ;
					}
				}
				break;

			case 4:
				for (sy=0; sy<sh; sy++)
				{
					dx = sh - sy - 1 ;

					for (sx=0; sx<sw; sx++)
					{
						dy = sx ;
						m_buf[(dx+dy*sh)] = pBuf->m_buf[(sx+sy*sw)] ;
					}
				}
				break;
			}
		}
		break;

	case 180:
		{
			sw = pBuf->m_w ;
			sh = pBuf->m_h ;
			if (!Init(sw, sh, pBuf->m_nStepSize))
				return ;

			if (m_nStepSize==1)
			{
				unsigned char *sbuf = (unsigned char *)pBuf->m_buf ;
				unsigned char *dbuf = (unsigned char *)m_buf ;
				for (sy=0; sy<sh; sy++)
				{
					dy = sh - sy - 1 ;

					for (sx=0; sx<sw; sx++)
					{
						dx = sw - sx - 1 ;
						dbuf[dx+dy*sw] = sbuf[sx+sy*sw] ;
					}
				}
			}
			else
			if (m_nStepSize==4)
			{
				int *sbuf = (int *)pBuf->m_buf ;
				int *dbuf = (int *)m_buf ;
				for (sy=0; sy<sh; sy++)
				{
					dy = sh - sy - 1 ;

					for (sx=0; sx<sw; sx++)
					{
						dx = sw - sx - 1 ;
						dbuf[dx+dy*sw] = sbuf[sx+sy*sw] ;
					}
				}
			}
			else
			{
				//ASSERT(false) ;
			}
		}
		break;

	case 270:
		{
			sw = pBuf->m_w ;
			sh = pBuf->m_h ;

			// Initialize so width and height are switched
			if (!Init(sh, sw, pBuf->m_nStepSize))
				return ;

			if (m_nStepSize==1)
			{
				unsigned char *sbuf = (unsigned char *)pBuf->m_buf ;
				unsigned char *dbuf = (unsigned char *)m_buf ;

				// With this, the dx = sh - sy + 1 and dy = sx
				for (sy=0; sy<sh; sy++)
				{
					dx = sy ;

					for (sx=0; sx<sw; sx++)
					{
						dy = sw - sx - 1 ;
						dbuf[dx+dy*sh] = sbuf[sx+sy*sw] ;
					}
				}
			}
			else
			if (m_nStepSize==4)
			{
				int *sbuf = (int *)pBuf->m_buf ;
				int *dbuf = (int *)m_buf ;

				// With this, the dx = sh - sy + 1 and dy = sx
				for (sy=0; sy<sh; sy++)
				{
					dx = sy ;

					for (sx=0; sx<sw; sx++)
					{
						dy = sw - sx - 1 ;
						dbuf[dx+dy*sh] = sbuf[sx+sy*sw] ;
					}
				}
			}
			else
			{
			//	ASSERT(false) ;
			}
		}
		break;

	default:
		Copy(*pBuf) ;
		break;
	}

}

/*
void CPixelBuffer::GetPointLineCoordProc(int x, int y, const void* lpdata)
{
	CPointArray *p = (CPointArray *)lpdata ;
	p->Add(CPoint(x,y)) ;

}
*/

/*
void CPixelBuffer::GetPointLineIndexProc(int x, int y, const void* lpdata)
{
	CPixelBuffer *pbuf = (CPixelBuffer *)lpdata ;

	if (x>=0 || y>=0 || x<pbuf->m_w || y<pbuf->m_h)
		pbuf->m_pIndexArray->Add(x+pbuf->m_w*y) ;
}
*/

/*
int CPixelBuffer::GetPointLine(int x1, int y1, int x2, int y2, CPointArray &p)
{
	int sx, sy ;

	if (x2!=x1 || y2!= y1)
	{
		sx = abs(x2-x1) + 1;
		sy = abs(y2-y1) + 1;
		if (sx>sy)
			p.SetSize(0,sx) ;
		else
			p.SetSize(0,sy) ;

		LineDDA(x1,y1,x2,y2,(LINEDDAPROC)GetPointLineCoordProc,(const void*)&p) ;
	}

	p.Add(CPoint(x2,y2)) ;
	return p.GetSize() ;
}
*/

/*
int CPixelBuffer::GetPointLine(int x1, int y1, int x2, int y2, CDWordArray &p)
{
	int sx, sy ;

	if (x2!=x1 || y2!= y1)
	{
		sx = abs(x2-x1) + 1;
		sy = abs(y2-y1) + 1;
		if (sx>sy)
			p.SetSize(0,sx) ;
		else
			p.SetSize(0,sy) ;

		m_pIndexArray = &p ;
		LineDDA(x1,y1,x2,y2,(LINEDDAPROC)GetPointLineIndexProc,(const void*)this) ;
	}

	if (x2>=0 && y2>=0 && x2<m_w && y2<m_h)
		p.Add(x1+y1*m_w) ;
	return p.GetSize() ;
}
*/

void CPixelBuffer::GetPixelArray(CPixelArray &Arr, CPixelBuffer *pMask)
{
	int index ;

	Arr.SetSize(0,m_h*m_w) ;
	index = 0 ;

	for (int y=0; y<m_h; y++)
	{
		for (int x=0; x<m_w; x++)
		{
			if (pMask)
			{
				if (pMask->m_buf[index])
				{
				    CPixel p(x,y,m_buf[index],index);
					Arr.Add(p) ;
			    }
				index++ ;
			}
			else
			{
				if (m_buf[index])
				{
				    CPixel p(x,y,m_buf[index],index);
					Arr.Add(p) ;
				}
				index++ ;
			}
		}
	}
}

void CPixelBuffer::Invert(CPixelBuffer *pbuf, int maxval)
{
	int i ;
	int nume ;

	nume = pbuf->m_w * pbuf->m_h ;
	if (!nume || !Init(pbuf->m_w, pbuf->m_h))
		return;
	
	for (i=0; i<nume; i++)
	{
		m_buf[i] = maxval - pbuf->m_buf[i] ;
	}
}

/*
void CPixelBuffer::Erode(CPixelBuffer *pbuf, CPixelArray &Arr)
{
	int i ;

	for (i=0; i<Arr.GetSize(); i++)
	{
	}
}
*/

void CPixelBuffer::MaskDistance(CPixelBuffer *pbuf, CPixelBuffer *pmaskbuf, int maxdist)
{
	int x, y, r, d, count ;
	BOOL bNotFound;

	if (!pbuf || !pmaskbuf)
		return;
	if (pbuf->Width()!=pmaskbuf->Width() || pbuf->Height()!=pmaskbuf->Height())
		return;
	if (pbuf->Width()==0 || pbuf->Height()==0)
		return;

	if (!Init(pbuf->m_w, pbuf->m_h))
		return ;
	Clear() ;

	count=0;
	for (y=0; y<pbuf->Height(); y++)
	{
		for (x=0; x<pbuf->Width(); x++)
		{
			if (pbuf->GetAt(x,y))
			{
				bNotFound = true ;
				for (r=1; r<=maxdist && bNotFound; r++)
				{
					for (d=-r; d<=r && bNotFound; d++)
					{
						bNotFound = (pmaskbuf->GetAt(x+r,y+d)) &&
									(pmaskbuf->GetAt(x-r,y+d)) &&
									(pmaskbuf->GetAt(x+d,y+r)) &&
									(pmaskbuf->GetAt(x+d,y-r)) ;
					}
					if (!bNotFound)
						break;
				}

				SetAt(x,y,r) ;
				if (r==maxdist)
					count++;
			}
		}
	}

}

BOOL CPixelBuffer::IsSprigRoot(int x, int y)
{
	int    val ;
	CPixel p(x,y,GetAt(x,y)) ;

	val = ClassifyPixel(p) ;
	return bIsSprigRoot[val] ;
}

CRect CPixelBuffer::GetBoundingRect(int pixelval)
{
	CRect rval ;
	int   x, y, i ;

	rval.left = Width() ;
	rval.right = 0 ;
	rval.top = Height() ;
	rval.bottom = 0 ;

	i=0;
	for (y=0; y<Height(); y++)
	{
		for (x=0; x<Width(); x++)
		{
			if (m_buf[i]==pixelval)
			{
				if (x<rval.left)
					rval.left = x ;
				if (x>=rval.right)
					rval.right = x+1 ;
				if (y<rval.top)
					rval.top = y ;
				if (y>=rval.bottom)
					rval.bottom = y+1 ;
			}

			i++ ;		
		}
	}

	if (rval.left>rval.right || rval.top>rval.bottom)
		rval.SetRectEmpty();
	return rval ;
}

void CPixelBuffer::FindBorders(CPixelBuffer *pbuf, int nFillPixelVal, int nBkgndPixelVal)
{
    //ASSERT(pbuf);
	int x, y ;

    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

    Clear() ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (pbuf->GetAt(x  , y)  != nBkgndPixelVal)
			{
				// First, collect the values...
				if (   pbuf->GetAt(x-1, y-1)==nBkgndPixelVal
					|| pbuf->GetAt(x  , y-1)==nBkgndPixelVal
					|| pbuf->GetAt(x+1, y-1)==nBkgndPixelVal
					|| pbuf->GetAt(x-1, y)==nBkgndPixelVal
					|| pbuf->GetAt(x+1, y)==nBkgndPixelVal
					|| pbuf->GetAt(x-1, y+1)==nBkgndPixelVal
					|| pbuf->GetAt(x  , y+1)==nBkgndPixelVal
					|| pbuf->GetAt(x+1, y+1)==nBkgndPixelVal )
				{
					SetAt(x,y,nFillPixelVal) ;
				}
			}
		}
	}
}

void CPixelBuffer::GreyDialate(CPixelBuffer *pSrc, int nNumReps)
{
	CPixelBuffer temp ;
	int pixelval ;
	int tempval ;
	int x, y, sx, sy ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return ;

	Clear() ;
	Copy(*pSrc) ;

	//for (i=0; i< nNumReps; i++)
	//{
		temp.Copy(*this) ;
		for (y=0; y<pSrc->m_h; y++)
		{
			for (x=0; x<pSrc->m_w; x++)
			{
				pixelval = temp.GetAt(x,y) ;
				for (sy=-nNumReps; sy<=nNumReps; sy++)
				{
					for (sx=-nNumReps; sx<=nNumReps; sx++)
					{
						if (sx*sx+sy*sy<=nNumReps*nNumReps)
						{
							tempval = temp.GetAt(x+sx, y+sy);
							if (tempval>pixelval)
								pixelval = tempval ;
						}
					}
				}
				SetAt(x,y,pixelval) ;
			}
		}
	//}
}

void CPixelBuffer::GreyErode(CPixelBuffer *pSrc, int nNumReps)
{
	CPixelBuffer temp ;
	int pixelval ;
	int tempval ;
	int x, y, sx, sy ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return ;

	Clear() ;
	Copy(*pSrc) ;

	//for (i=0; i< nNumReps; i++)
	//{
		temp.Copy(*this) ;
		for (y=0; y<pSrc->m_h; y++)
		{
			for (x=0; x<pSrc->m_w; x++)
			{
				pixelval = temp.GetAt(x,y) ;
				for (sy=-nNumReps; sy<=nNumReps; sy++)
				{
					for (sx=-nNumReps; sx<=nNumReps; sx++)
					{
						if (sx*sx+sy*sy<=nNumReps*nNumReps)
						{
							tempval = temp.GetAt(x+sx, y+sy);
								if (tempval<pixelval)
									pixelval = tempval ;
						}
					}
				}
				SetAt(x,y,pixelval) ;
			}
		}
	//}
}

void CPixelBuffer::GreyClose(CPixelBuffer *pSrc, int nNumReps)
{
	CPixelBuffer dialate ;
	dialate.GreyDialate(pSrc, nNumReps) ;
	GreyErode(&dialate, nNumReps) ;
}

void CPixelBuffer::GreyOpen(CPixelBuffer *pSrc, int nNumReps)
{
	CPixelBuffer erode ;
	erode.GreyErode(pSrc, nNumReps) ;
	GreyDialate(&erode, nNumReps) ;
}

void CPixelBuffer::TopHatPeak(CPixelBuffer *pSrc, BOOL bClip, int nNumReps) 
{
	CPixelBuffer open ;

	Copy(*pSrc) ;
	open.GreyOpen(pSrc, nNumReps) ;
	Diff(&open, bClip) ;
}

void CPixelBuffer::TopHatWell(CPixelBuffer *pSrc, BOOL bClip, int nNumReps) 
{
	CPixelBuffer close ;
	
	Copy(*pSrc) ;
	close.GreyClose(pSrc, nNumReps) ;
	Diff(&close, bClip) ;
}

void CPixelBuffer::LocalContrastStretch(CPixelBuffer *pSrc, DOUBLE scale, int nRadius)
{
	int x,y,sx,sy,min,max,pval ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return;

	Clear() ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			min = max = pSrc->GetAt(x,y) ;

			for (sy=-nRadius; sy<=nRadius; sy++)
			{
				for (sx=-nRadius; sx<=nRadius; sx++)
				{
					if (sx*sx+sy*sy<=nRadius*nRadius)
					{
						pval = pSrc->GetAt(x+sx, y+sy) ;
						if (pval<min)
							min = pval ;
						if (pval>max)
							max = pval ;
					}
				}
			}

			if (max!=min)
			{
				pval = (int) (scale * (DOUBLE)(pval - min) / (DOUBLE)(max - min)) ;
				SetAt(x,y,pval) ;
			}
		}
	}
}

static int LaplacianOfGaussianMask9x9[81] = 
{
0, 1,  1,  2,  2,  2,  1,  1, 0,
1, 2,  4,  5,  5,  5,  4,  2, 1,
1, 4,  5,  3,  0,  3,  5,  4, 1,
2, 5,  3,-12,-24,-12,  3,  5, 2,
2, 5,  0,-24,-40,-24,  0,  5, 2,   
2, 5,  3,-12,-24,-12,  3,  5, 2,
1, 4,  5,  3,  0,  3,  5,  4, 1,
1, 2,  4,  5,  5,  5,  4,  2, 1,
0, 1,  1,  2,  2,  2,  1,  1, 0,
};


void CPixelBuffer::LaplacianOfGaussian9x9(CPixelBuffer *pSrc)
{
	int x,y,sx,sy,i,ind,pval,sz ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return;

	Clear() ;

	sz = pSrc->m_w - 9 ;
	for (y=4; y<m_h-4; y++)
	{
		for (x=4; x<m_w-4; x++)
		{
			i = 0 ;
			pval = 0 ;

			ind = pSrc->Index(x-4,y-4) ;
			for (sy=-4; sy<=4; sy++)
			{
				for (sx=-4; sx<=4; sx++)
				{
					pval += pSrc->m_buf[ind] * LaplacianOfGaussianMask9x9[i];
					ind++ ;
					i++;
				}
				ind+=sz ;
			}

			SetAtFast(x,y,pval) ;
		}
	}
}

int CPixelBuffer::Lowest(int nIgnore)
{
	if (!m_w || !m_h || !m_buf)
		return 0 ;

	int lowest = 0x7FFFFFF ;
	int *pbuf = m_buf ;
	int x, y ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (*pbuf!=nIgnore)
			{
				if (*pbuf<lowest)
					lowest = *pbuf ;
			}
			pbuf++;
		}
	}

	return lowest ;
}

BOOL CPixelBuffer::Trim(int min, int max, int minreplaceval, int maxreplaceval)
{
	if (!m_w || !m_h || !m_buf)
		return false ;

	int *pbuf = m_buf ;
	int x, y ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (*pbuf<min)
				*pbuf = minreplaceval;
			else
			if (*pbuf>max)
				*pbuf = maxreplaceval ;
			pbuf++;
		}
	}
	return true ;
}

BOOL CPixelBuffer::Crop(int emptyval, int nBorderSize)
{
	CPixelBuffer copy ;
	int minx,miny, maxx,maxy ;
	int x,y ;
	int *pbuf = m_buf ;

	if (!m_w || !m_h || !m_buf)
		return false ;

	copy.Copy(*this) ;
	minx=m_w ;
	miny=m_h ;
	maxx=maxy=0 ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (*pbuf!=emptyval)
			{
				if (x<minx)
					minx = x ;
				if (x>maxx)
					maxx = x ;
				if (y<miny)
					miny = y ;
				if (y>maxy)
					maxy = y ;
			}
			pbuf++;
		}
	}

	Init(maxx-minx+1+nBorderSize*2, maxy-miny+1+nBorderSize*2) ;
	memset (m_buf, 0, m_w*m_h*sizeof(int)) ;

	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			SetAtFast(x+nBorderSize,y+nBorderSize,copy.GetAtFast(x+minx, y+miny)) ;
		}
	}
	return true;
}

BOOL CPixelBuffer::FindOutline(CPixelBuffer *pSrc, int nBlankValue, int nReplaceVal)
{
	int i, j, x, y ;
	CPixelChains chains ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return false;
	memset (m_buf, 0, m_w*m_h*sizeof(int)) ;

	// Get right and left borders
	for (x=0; x<m_w; x++)
	{
		for (y=0; y<m_h; y++)
		{
			if (pSrc->GetAt(x,y)!=nBlankValue)
			{
				SetAt(x,y,nReplaceVal) ;
				break;
			}
		}

		for (y=m_h-1; y>=0; y--)
		{
			if (pSrc->GetAt(x,y)!=nBlankValue)
			{
				SetAt(x,y,nReplaceVal) ;
				break;
			}
		}
	}

	// Get top and bottom borders
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (pSrc->GetAt(x,y)!=nBlankValue)
			{
				SetAt(x,y,nReplaceVal) ;
				break;
			}
		}

		for (x=m_w-1; x>=0; x--)
		{
			if (pSrc->GetAt(x,y)!=nBlankValue)
			{
				SetAt(x,y,nReplaceVal) ;
				break;
			}
		}
	}

	// Remove all sprig roots
	RemoveSprigRoots() ;

	// Now, look for any pixel chains that exist...
	FindPixelChains(chains, false) ;
	AddSprigRootsToChains(chains) ;
	SortPixelChains(chains) ;

	for (i=0; i<chains.GetSize(); i++)
	{
		CPixelArray &Arr = chains[i] ;

		for (j=0; j<Arr.GetSize(); j++)
		{
			Arr[j].v = (i+1)*300+j;
			Arr[j].n = (i+1)*300+j;
			SetAt(Arr[j]) ;
		}
	}

	return true ;
}

int CPixel::DistSqr(CPixel &second)
{
	int dx = second.x-x ;
	int dy = second.y-y ;

	return dx*dx + dy*dy ;
}

// The purpose of this method is to sort the pixel chains so that the tails
// and heads of the chains will line up and give and outline with the longest
// length.
void CPixelBuffer::SortPixelChains(CPixelChains &Chains)
{
	CPixelChains Results ;
	int			 i, j;
	int			 dist, mindist, minindex, numreps ;
	BOOL		 bFirstCloser ;
	CPixel		 CurPixel ;

	numreps = Chains.GetSize()-1 ;
	Results.Add(Chains[0]) ;
	Chains.RemoveAt(0) ;

	for (i=0; i<numreps; i++)
	{
		CurPixel = Results[Results.GetUpperBound()].Last() ;
		mindist = m_w * m_w + m_h * m_h ;
		minindex = 0 ;
		for (j=0; j<Chains.GetSize(); j++)
		{
			dist = Chains[j].First().DistSqr(CurPixel);
			if (dist<mindist)
			{
				mindist = dist ;
				minindex = j ;
				bFirstCloser = true ;
			}

			dist = Chains[j].Last().DistSqr(CurPixel);
			if (dist<mindist)
			{
				mindist = dist ;
				minindex = j ;
				bFirstCloser = false ;
			}
		}

		if (bFirstCloser==false)
			Chains[minindex].ReverseDirection() ;
		Results.Add(Chains[minindex]) ;
		Chains.RemoveAt(minindex) ;
	}

	Chains.SetSize(0) ;
	Chains.Copy(Results) ;
}

void CPixelArray::CopyPtr ( CPixel *pPixel, int nNumPixels )
{
	if (m_pData)
		delete [] m_pData ;
	m_pData = (CPixel *) new unsigned char [sizeof(CPixel)*nNumPixels] ;
	m_nSize = nNumPixels ;
	m_nMaxSize = nNumPixels ;
	m_nGrowBy = 0 ;
	memcpy (m_pData, pPixel, sizeof (CPixel) * nNumPixels ) ;
}

void CPixelArray::ReverseDirection()
{
	int numelem = GetSize() ;
	int i ;
	CPixel temp ;

	for (i=0; i<numelem/2; i++)
	{
		temp = ElementAt(i) ;
		ElementAt(i) = ElementAt(numelem-i-1) ;
		ElementAt(numelem-i-1) = temp ;
	}
}

CCoord CPixelArray::Center()
{
	CCoord  rval ;
	int		i ;

	if (GetSize()==0)
		return rval ;

	for (i=0; i<GetSize(); i++)
	{
		CPixel &c = ElementAt(i) ;
		rval.m_x += c.x ;
		rval.m_y += c.y ;
	}

	rval /= (DOUBLE)GetSize() ;
	return rval ;
}

void CPixelArray::SetValues(int nNewVal)
{
	int i ;
	for (i=0; i<GetSize(); i++)
		ElementAt(i).v = nNewVal ;
}

CPixel &CPixelArray::First()
{
	return ElementAt(0) ;
}

CPixel &CPixelArray::Last()
{
	return ElementAt(GetUpperBound()) ;
}

CPixelArray &CPixelArray::operator =(const CPixelArray &copy)
{
	this->Copy(copy) ;
	return *this;
}

int CPixelArray::Width()
{
	int min = 0x7FFFFFFF ;
	int max = 0x80000000 ;
	int i ;
	int v ;

	for (i=0; i<GetSize(); i++)
	{
		v = ElementAt(i).x ;
		if (v<min)
			min = v ;
		if (v>max)
			max = v ;
	}

	return max-min ;
}

int CPixelArray::Height()
{
	int min = 0x7FFFFFFF ;
	int max = 0x80000000 ;
	int i ;
	int v ;

	for (i=0; i<GetSize(); i++)
	{
		v = ElementAt(i).y ;
		if (v<min)
			min = v ;
		if (v>max)
			max = v ;
	}

	return max-min ;
}

DOUBLE CPixelArray::Nearest ( CCoord &c )
{
	int		i, x, y, dist, nearest, dx, dy ;
	DOUBLE  rval = (DOUBLE)-1.0 ;

	if (!GetSize())
		return rval ;

	rval = (DOUBLE)1.0e13 ;
	nearest = 1000000000 ;
	x = (int)c.m_x ;
	y = (int)c.m_y ;
	for (i=0; i<GetSize(); i++)
	{
		dx = m_pData[i].x - x ;
		dy = m_pData[i].y - y ;
		dist = dx*dx + dy*dy ;
		if (dist < nearest)
			nearest = dist ;
	}

	rval = (DOUBLE) sqrt ( (DOUBLE) nearest ) ;
	return rval ;
}

CRect CPixelArray::RegionRect ()
{
	CRect rc ;
	int	  i ;

	rc.SetRect(65536,65536,-1,-1) ;
	for (i=0; i<GetSize(); i++)
	{
		if (m_pData[i].x < rc.left)
			rc.left = m_pData[i].x ;
		if (m_pData[i].x > rc.right)
			rc.right = m_pData[i].x ;

		if (m_pData[i].y < rc.top)
			rc.top = m_pData[i].y ;
		if (m_pData[i].y > rc.bottom)
			rc.bottom = m_pData[i].y ;
	}

	return rc ;
}


void CPixel::Set (int cx, int cy, int cv, int cn)
{
	x = cx ;
	y = cy ;
	v = cv ;
	n = cn ;
}

void CPixel::Add (int cx, int cy, int cv, int cn)
{
	x += cx ;
	y += cy ;
	v += cv ;
	n += cn ;
}

CPixel &CPixel::operator =(const CPixel &copy)
{
	x = copy.x ;
	y = copy.y ;
	v = copy.v ;
	n = copy.n ;
	return *this;
}

CPixel::CPixel(CPixel &copy)
{
	*this = copy ;
}

void CPixelBuffer::AddSprigRootsToChains(CPixelChains &Chains)
{
	int i, j, mindist, minindex, dist ;
	BOOL bFirstCloser ;

	for (i=0; i<m_SprigRoots.GetSize(); i++)
	{
		mindist = m_h*m_h + m_w*m_w ;
		minindex = 0 ;
		for (j=0; j<Chains.GetSize(); j++)
		{
			dist = Chains[j].First().DistSqr(m_SprigRoots[i]) ;
			if (dist<mindist)
			{
				bFirstCloser = true ;
				mindist = dist ;
				minindex = j ;
			}

			dist = Chains[j].Last().DistSqr(m_SprigRoots[i]) ;
			if (dist<mindist)
			{
				bFirstCloser = false ;
				mindist = dist ;
				minindex = j ;
			}
		}

		if (bFirstCloser)
			Chains[minindex].InsertAt(0, m_SprigRoots[i]) ;
		else
			Chains[minindex].Add(m_SprigRoots[i]) ;
	}
}

void CPixelBuffer::AverageFill(CPixelBuffer *pSrc, int nMaskWidth, int nMaskHeight)
{
	int x, y, ix, iy, i, j, k, nInd ;
	int nIndMin, nIndMax, nNumChanged ;
	int nNumFound, nRequiredNum, v ;
	int nMaskSize, total, nNumReps, nDiff, nCnt ;
	int *nMaskDelta=NULL ;
	int nMaskDeltaSize ;
	int deltas[9] ;
	CPixelBuffer Filled ;
	CPixelBuffer Display ;
	CPixelBuffer Noise ;
	CPixelBuffer Threshold ;

	if (!Init(pSrc->m_w, pSrc->m_h, 4))
		return;
	Filled.Init (m_w, m_h, 4) ;

	Noise.RemoveBigNoise ( pSrc, 3 ) ;
	Copy (Noise) ;

	//ShowBuffer(25) ;

	deltas[0] = -m_w - 1 ;
	deltas[1] = -m_w ;
	deltas[2] = -m_w + 1 ;
	deltas[3] = - 1 ;
	deltas[4] =  1 ;
	deltas[5] = m_w - 1 ;
	deltas[6] = m_w ;
	deltas[7] = m_w + 1 ;


	nMaskSize    = (nMaskWidth*2+1) * (nMaskHeight*2+1) ;
	nRequiredNum = nMaskSize / 2 + 1 ;

	i = 0;	

	Threshold.Threshold(this,1,10*16+10) ;

	nIndMin = m_w*nMaskHeight + nMaskWidth ;
	nIndMax = m_size - nIndMin ;

	nMaskDeltaSize = (nMaskWidth*2+1) * (nMaskHeight*2+1) - 1;
	nMaskDelta = new int[nMaskDeltaSize] ;

	if (nMaskDelta)
	{
		for (i=0,iy=-nMaskHeight; iy<=nMaskHeight; iy++)
		{
			for (ix=-nMaskWidth; ix<=nMaskWidth; ix++)
			{
				if (ix || iy)
				{
					nMaskDelta[i] = ix+m_w*iy ;
					i++;
				}
			}
		}
	}

	do
	{
		nNumChanged = 0 ;
		for (nInd=0,y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++,nInd++)
			{
				nNumFound = 0 ;
				total = 0 ;

				// Is this a pixel that's supposed to be averaged?
				// If it is zero on the source, we should try.
				if (Noise.m_buf[nInd]==0)
				{
					if (nInd>=nIndMin && nInd<nIndMax)
					{
						for (i=0; i<nMaskDeltaSize; i++)
						{
							v = m_buf[nInd+nMaskDelta[i]] ;
							if (v)
								nNumFound++ ;
						}
					}
					else
					{
						for (iy=-nMaskHeight; iy<=nMaskHeight; iy++)
						{
							for (ix=-nMaskWidth; ix<=nMaskWidth; ix++)
							{
								if (ix || iy)
								{
									v = GetAt(x+ix, y+iy) ;
									if (v)
										nNumFound++ ;
								}
							}
						}
					}
				}

				if (nNumFound>=nRequiredNum)
				{
					if (m_buf[nInd] == 0)
					{
						nNumChanged++;
						m_buf[nInd] = 255 ;
					}
				}
			}
		}
	} 
	while (nNumChanged!=0) ;

	if (nMaskDelta)
		delete nMaskDelta ;

	// Now that we've filled them, we want to create a map of points that
	// were filled.
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			if (Noise.GetAtFast(x,y)==0 && GetAtFast(x,y))
			{
				Filled.SetAtFast (x,y,5*16+5) ;
				SetAtFast (x,y,128) ;
			}
		}
	}

	// Now, do the averaging of the area
	int *nIndexArray = NULL ;
	int nNumNonZero = Filled.CountNonZero() ;

	if (nNumNonZero>0)
	{
		nIndexArray = new int [nNumNonZero] ;

		// Increase the precision
		Mult(256) ;
		BOOL bShow = false ;

		// Reduce the number of searches needed for each pass by
		// filling a list of only filled indeces.
		for (j=0,i=0; i<Filled.m_size; i++)
		{
			if (Filled.m_buf[i])
			{
				nIndexArray[j] = i ;
				j++ ;
			}
		}
	
		nNumReps = 0 ;
		do
		{
			nNumChanged = 0 ;
			nCnt = 0 ;

			
			for (i=0; i<nNumNonZero; i++)
			{
				total = 0 ;
				nCnt = 0 ;

				for (j=0; j<8; j++)
				{
					nInd = nIndexArray[i] ;

					v = m_buf[nInd + deltas[j]] ;
					if (v)
					{
						nDiff = m_buf[nInd] - v;
						total += nDiff ;
						nCnt++ ;
					}
				}

				if (nCnt)
				{
					total /= nCnt ;
					Filled.m_buf[nInd] = m_buf[nInd] - total ;
					if (total)
						nNumChanged++ ;
				}
			}

			for (i=0; i<nNumNonZero; i++)
			{
				nInd = nIndexArray[i];
				if (Filled.m_buf[nInd])
					m_buf[nInd] = Filled.m_buf[nInd] ;
			}
			

			/*
			// Increase our precision
			for (y=1; y<m_h-1; y++)
			{
				for (x=1; x<m_w-1; x++)
				{
					if (Filled.GetAtFast(x,y))
					{
						i = x + y * m_w ;
						total = 0 ;
						nCnt = 0 ;

						for (j=0; j<8; j++)
						{
							v = m_buf[i+deltas[j]] ;
							if (v)
							{
								nDiff = m_buf[i] - m_buf[i+deltas[j]] ;
								total += nDiff ;
								nCnt++ ;
							}
						}

						if (nCnt)
						{
							total /= nCnt;
							if (Filled.m_buf[i])
							{
								Filled.m_buf[i] = m_buf[i] - total ;
								if (total)
									nNumChanged++ ;
							}
						}
					}
				}
			}

			// Copy the stretched values into the current buffer
			for (i=0; i<m_size; i++)
			{
				if (Filled.m_buf[i])
					m_buf[i] = Filled.m_buf[i] ;
			}
			*/

			nNumReps++ ;
		} 
		while (nNumChanged>0 && nNumReps<1000) ;
		
		Div(256) ;

		if (nIndexArray)
			delete nIndexArray ;
	}
}

BOOL CPixelBuffer::ValidPeakRun(int *rvals, int *rantivals, int peakval, int nRadius)
{
#ifdef USE_ASSEMBLY_OPTIMIZE
/*
	int bal, antibal ;
	_asm
	{
		// eax = bal
		// ebx = peakval
		// esi = rvals 
		// ecx = lencnt

		mov eax, 0
		mov ebx, peakval
		mov ecx, nRadius
		mov esi, rvals

AnotherValidRun1:
		mov edx, [esi]
		cmp edx, ebx
		jg FalseReturn
//		add eax, edx
		add esi, 4
		dec ecx
		jnz AnotherValidRun1
//		mov bal, eax
		sub esi, 4

		mov eax, 0
		mov ecx, nRadius
		mov edi, rantivals

AnotherValidRun2:
		mov edx, [edi]
		cmp edx, ebx
		jg FalseReturn
//		add eax, edx
		add edi, 4
		dec ecx
		jnz AnotherValidRun2
//		mov antibal, eax
		sub edi, 4


		sub ebx, nMinRange
		cmp [esi], ebx
		jge FalseReturn
		cmp [edi], ebx
		jge FalseReturn
	}

	return true ;
FalseReturn:
	return false ;

*/
#else
	int lencnt ;
	int *pvals = rvals ;
	int *pantivals = rantivals ;
	int bal, antibal ;

	bal = 0;
	antibal = 0 ;

	lencnt = nRadius ;
	// First, do the less or equal test.
	while(lencnt)
	{
		if (*pvals > peakval)
			return false ;

		bal += *pvals ;
		if (--lencnt)
			pvals++ ;
	}

	lencnt = nRadius;
	while(lencnt)
	{
		if (*pantivals > peakval)
			return false ;
		
		antibal += *pantivals ;
		if (--lencnt)
			pantivals++ ;
	}

//	largest = bal ;
//	if (antibal > largest)
//		largest = antibal ;

	// Are the values balanced enough???
//	if (largest > 0)
//	{
//		if (  abs (antibal - bal) * 100 / largest > 70 )
//			return false ;
//	}
	
	// Now, we're at the last pixel.  It must be less than
	// peakval - nMinRange
	if ((*pvals < (peakval-nMinRange)) && (*pantivals < (peakval-nMinRange)))
		return true ;
	else
		return false ;
#endif
}

BOOL CPixelBuffer::ValidPeakRun ( int nStartIndex, int nRadius, int nThreshold, int nRange, int *pImage, int *pRunIndeces )
{
	int i ;
	int nInd ;
	int nPeakVal ;
	int nVal ;

	// Check in forward direction first
	nInd = pRunIndeces[nStartIndex] ;
	nPeakVal = pImage[nInd] ;
	if (nPeakVal < nThreshold)
		return false ;

	for (i=0; i<nRadius; i++)
	{
		nInd = pRunIndeces[nStartIndex+i] ;
		if (nInd>=0)
			nVal = pImage[nInd] ;
		else
			nVal = 0 ;
		if (nVal>nPeakVal)
			return false ;
	}
	if (nPeakVal - nVal < nRange)
		return false ;

	for (i=0; i<nRadius; i++)
	{
		nInd = pRunIndeces[nStartIndex-i] ;
		if (nInd>=0)
			nVal = pImage[nInd] ;
		else
			nVal = 0 ;
		if (nVal>nPeakVal)
			return false ;
	}
	if (nPeakVal - nVal < nRange)
		return false ;


	return true ;
}

//#define UNCLASSIFIED 0
//#define OUTSIDERUN   1
//#define INSIDERUN    2
//#define SURFACERUN   3



void CPixelBuffer::FindInsideOutside ( CPixelBuffer *pbuf
									 , CBaseLongFrameArray &findruns
									 , int nInsideIdentifier
									 , int nThreshold
									 , int nMinRange
									 , int nRadius ) 
{
	int    i, j, x, y, nSize, nIndex, nVal, nCnt ;
	BOOL   bNotFound ;
	int    *pImage ;
	int    *pRunIndeces ;

	// First, try to initialize
    if (!Init(pbuf->m_w, pbuf->m_h))
        return;

	// Set all values to UNCLASSIFIED!
	Clear() ;

	pImage = (int *)pbuf->m_buf ;

	// Loop through all of the runs
	int cnt = 0 ;

	for (i=0; i<findruns.GetSize(); i++)
	{
		CDWordArray &run = findruns.ElementAt(i) ;
		pRunIndeces = (int *)run.GetData() ;
		

		bNotFound = true ;
		nSize = run.GetSize() ;

		for (j=nRadius; j<nSize-nRadius; j++)
		{
			nIndex = pRunIndeces[j] ;
			if (bNotFound && ValidPeakRun(j,nRadius,nThreshold,nMinRange,pImage,pRunIndeces))
			{
				// This is a peak classification
				m_buf[nIndex] = SURFACERUN ;
				cnt++ ;
				bNotFound = false ;
			}
			else
			{
				nVal = m_buf[nIndex] ;
				if (nVal != SURFACERUN)
				{
					if (bNotFound)
					{
						m_buf[nIndex] = OUTSIDERUN ;
					}
					else
					if (nVal != OUTSIDERUN)
						m_buf[nIndex] = INSIDERUN ;
				}
				else
				{
				}
			}
		}
	}

	// Finally, find all the runs that have and outside/inside border, 
	// but have no peak classifcation should get a peak classification.
	// Go through all the pixels and check neighbors to see if there are
	// any nonzero bordering pixels.
	int nNumChanged = 0 ;

	do
	{
		nNumChanged = 0 ;
		for (y=1; y<m_h-1; y++)
		{
			for (x=1; x<m_w-1; x++)
			{
				nIndex = (x-1)+(y-1)*m_w ;

				nVal = m_buf[nIndex+m_w+1] ;
				if (nVal==0)
				{
					nCnt = 0 ;

					nCnt += m_buf[nIndex] ? 1 : 0 ;
					nCnt += m_buf[nIndex+1] ? 1 : 0 ;
					nCnt += m_buf[nIndex+2] ? 1 : 0 ;
					nCnt += m_buf[nIndex+m_w] ? 1 : 0 ;
					nCnt += m_buf[nIndex+m_w+2] ? 1 : 0 ;
					nCnt += m_buf[nIndex+m_w+m_w] ? 1 : 0 ;
					nCnt += m_buf[nIndex+m_w+m_w+1] ? 1 : 0 ;
					nCnt += m_buf[nIndex+m_w+m_w+2] ? 1 : 0 ;

					if (nCnt>6)
					{
						nNumChanged++;
						m_buf[nIndex+m_w+1] = 1 ;
					}
				}
			}
		}
	} while (nNumChanged > 0) ;
}

BOOL CPixelBuffer::SetLine(int x1, int y1, int x2, int y2, int v)
{
	m_nSetLineData = v ;
	LineDDA(x1,y1,x2,y2,SetLineProc,(const void*)this) ;
	SetAt(x2,y2,v) ;
	return true ;
}

void CPixelBuffer::SetLineProc(int x, int y, const void* lpdata)
{
	CPixelBuffer *pbuf = (CPixelBuffer *)lpdata ;

	pbuf->SetAt(x,y,pbuf->m_nSetLineData) ;
}

static DOUBLE angle=(DOUBLE)0.0 ;

void CPixelBuffer::FindCheckerCorners(CPixelBuffer *pbuf, int radius)
{
	CPixelBuffer checkrgn ;
	CPixelBuffer *checkpatterns ;
	CPixelBuffer newpat, oldpat ;
	int x, y, i, j ;
	DOUBLE dx, dy, d  ;
	int nNumPatterns ;
	int nGreatestVal ;
	int nMaxVal ;
	int nTotal ;
	int *ptempbuf ;
	int *pcheckbuf ;

	if (!Init(pbuf->Width(), pbuf->Height()))
		return ;

	checkpatterns = new CPixelBuffer[180] ;

	newpat.Init(radius*2, radius*2) ;
	newpat.Clear() ;
	oldpat.Init(radius*2, radius*2) ;
	oldpat.Clear() ;

	nNumPatterns = 0 ;
	for (angle=0.0; angle<180.0; angle+=1.0)
	{
		newpat.Clear() ;
		for (y=0; y<radius*2; y++)
		{
			for (x=0; x<radius*2; x++)
			{
				dx = radius - x - (DOUBLE)0.5;
				dy = radius - y - (DOUBLE)0.5;
				d = (DOUBLE)(atan2(dy, dx) * RAD_TO_DEG) + angle;
				if (d<0.0)
					d+=180.0 ;
				else
				if (d>180.0)
					d-=180.0 ;

				if (d>90.0)
					newpat.SetAt(x,y,255) ;
				else
					newpat.SetAt(x,y,0) ;
			}
		}

		if (newpat.IsEqual(oldpat)==false)
		{
			checkpatterns[nNumPatterns].Copy(newpat) ;
			nNumPatterns++ ;
			oldpat.Copy(newpat) ;
		}
	}

	// Create our checker image patterns.  The number of patterns
	// is proportional to the radius.  Blacks are -1 and Whites are
	// 1.  We go through the number of iterations and take the
	// orientation with the maximum value.
	//checkpatterns = new CPixelBuffer [radius*

	nGreatestVal = 0 ;
	for (y=0; y<pbuf->Height(); y++)
	{
		for (x=0; x<pbuf->Width(); x++)
		{
			// First, copy the region to be checked
			checkrgn.Copy(*pbuf, x-radius, y-radius, x+radius, y+radius) ;
			nMaxVal = 0 ;

			for (i=0; i<nNumPatterns; i++)
			{
				nTotal = 0 ;
				ptempbuf = checkrgn.m_buf ;
				pcheckbuf = checkpatterns[i].m_buf ;
				for (j=0; j<checkrgn.m_size; j++)
				{
					if (*pcheckbuf)
					{
						nTotal += *ptempbuf ;
					}
					pcheckbuf++;
					ptempbuf++;
				}

				if (nTotal > nMaxVal)
					nMaxVal = nTotal ;
			}

			SetAt(x,y,nMaxVal) ;
			if (nMaxVal>nGreatestVal)
				nGreatestVal = nMaxVal ;
		}
	}

	for (i=0; i<m_size; i++)
		m_buf[i] = MulDiv(m_buf[i], 255, nGreatestVal) ;

	delete [] checkpatterns ;
}

BOOL CPixelBuffer::IsEqual(CPixelBuffer &pbuf)
{
	int i ;
	if (pbuf.m_w != m_w || pbuf.m_h != m_h)
		return false ;

	if (pbuf.m_size != m_size)
		return false ;

	for (i=0; i<m_size; i++)
		if (pbuf.m_buf[i] != m_buf[i])
			return false ;

	return true ;
}

int CPixelBuffer::Flood(int x, int y, int compval, int replval) 
{
	if (GetAt(x,y)!=compval)
		return 0;
	SetAt(x,y,replval) ;
	Flood(x+1,y,compval,replval) ;
	Flood(x,y+1,compval,replval) ;
	Flood(x-1,y,compval,replval) ;
	Flood(x,y-1,compval,replval) ;
	return 0 ;
}

int CPixelBuffer::FloodFillCross(int x, int y, int replv)
{
	int v ;		

	v = GetAt(x,y) ;
	Flood(x,y,v,replv) ;
	return 0 ;
}

void CPixelBuffer::DetectCrossIntersects(CPixelBuffer *pbuf, int radius) 
{
	int x, y, i, j, maxval, total ;
	CPixelBuffer *pTestBuffers, *pTest ;
	CPixelBuffer CompareBuffer ;
	int nNumTestBuffers ;
	int *presult ;
	int nCount ;
//	int nNonZeroCount ;

	if (!Init(pbuf->m_w, pbuf->m_h))
		return ;
	presult = m_buf ;

	nNumTestBuffers = radius*2 ;
	pTestBuffers = new CPixelBuffer [nNumTestBuffers] ;
	for (i=0; i<nNumTestBuffers; i++)
	{
		pTestBuffers[i].Init(radius*2+1, radius*2+1) ;
		pTestBuffers[i].Clear() ;
		pTestBuffers[i].ReplaceVal(0,-1) ;
	}
	CompareBuffer.Init(radius*2+1, radius*2+1) ;

	for (x=0; x<radius; x++)
	{
		pTestBuffers[x].SetLine(0, radius+x, radius*2, radius-x, 0) ;
		pTestBuffers[x].SetLine(radius-x, 0, radius+x, radius*2, 0) ;
		pTestBuffers[x].FloodFillCross(0, radius+x+1, 1) ;
		pTestBuffers[x].FloodFillCross(radius*2, radius-x-1, 1) ;

		pTestBuffers[x+radius].SetLine(x, radius*2, radius*2-x, 0, 0) ;
		pTestBuffers[x+radius].SetLine(0, x, radius*2, radius*2-x, 0) ;
		pTestBuffers[x+radius].FloodFillCross(x+1, radius*2, 1) ;
		pTestBuffers[x+radius].FloodFillCross(radius*2-x-1, 0, 1) ;
	}

	// Now, go through all the pixels and sum up the values
	nCount = 0 ;
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			maxval = -1 ;
			// Seed the comparison list
			CompareBuffer.Copy(*pbuf, x-radius, y-radius, x+radius+1, y+radius+1, true) ;
			for (i=0; i<nNumTestBuffers; i++)
			{
				total = 0 ;
				pTest = pTestBuffers+i ;
				for (j=0; j<CompareBuffer.m_size; j++)
				{
					if (CompareBuffer.m_buf[j])
						nCount++;
					total += CompareBuffer.m_buf[j] * pTest->m_buf[j] ;
				}

				if (total>maxval)
					maxval = total ;
			}

			if (maxval>0)
			{
				if (maxval != 1035455)
					*presult = maxval ;
				else
				if (maxval != 1184311)
					*presult = maxval ;
				else
					*presult = maxval ;
			}				
			else
				*presult = 0 ;
			presult++;
		}
	}
}

/*
void CPixelBuffer::Trace()
{
	int x,y,i ;
	CString tstr, nstr ;
	i=0;
	for(y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++)
		{
			nstr.Format("%2d ", m_buf[i]) ;
			i++;
			tstr += nstr ;
		}
		tstr+="\n" ;
	}
	tstr += "\n" ;
	sTraceString += tstr ;
}
*/

void CPixelBuffer::Circle(int xCenter, int yCenter, int radius, int v)
{
	int d ;
	int x=0 ;
	int y=radius ;
	
	d=3-(2*radius);
    while(x<=y)
	{
		SetAt(xCenter+x,yCenter+y,v);
		//SetAt(xCenter+y,yCenter+x,v);
		//SetAt(xCenter-x,yCenter+y,v);
		SetAt(xCenter+y,yCenter-x,v);
		SetAt(xCenter-x,yCenter-y,v);
		//SetAt(xCenter-y,yCenter-x,v);
		//SetAt(xCenter+x,yCenter-y,v);
		SetAt(xCenter-y,yCenter+x,v);
		
		if (d<0)
			d += (4*x)+6;
		else
		{
			d += (4*(x-y))+10;
			y -= 1;
		}
		x++;
		
	}
}



// This is similar to the GetCircleCoords.  The return value is the number of points
// generated.  This will generate 9 sets of points with the same number of points.
// Set number 5 will not be used.  The radius MUST be a number that is generated
// by the following equation (r = 4 + i*3 where i is 0,1,2,...).  The number can
// be checked for validity by checking (r-4)%3 = 0.
/*
int CPixelBuffer::GetMilanoCoords(int xCenter, int yCenter, int radius, CPoint *buffer)
{
	int x=0 ;
	int y=radius ;
	int nNumPts ;
	int nSectionLen ;
	int i, j ;

	// Is the radius valid?
	if ( (radius-4) % 3 )
		return 0 ;

	// Figure out the section length
	nSectionLen = (radius*2+1)/3 ;
	nNumPts = nSectionLen * 9 ;

	// Now, generate the various sections of the search pattern...
	// 1 = NW
	// 2 = N
	// 3 = NE
	// 4 = W
	// 5 = Center
	// 6 = E
	// 7 = SW
	// 8 = S
	// 9 = SE

	j=0;
	// NW
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = -(i+nSectionLen/2+1) ;
		buffer[j].y = -(i+nSectionLen/2+1) ;
	}
	// N
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = 0 ;
		buffer[j].y = -(i+nSectionLen/2+1) ;
	}
	// NE
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = (i+nSectionLen/2+1) ;
		buffer[j].y = -(i+nSectionLen/2+1) ;
	}
	// W
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = -(i+nSectionLen/2+1) ;
		buffer[j].y = 0 ;
	}
	// Center
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = 0 ;
		buffer[j].y = 0 ;
	}
	// E
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = (i+nSectionLen/2+1) ;
		buffer[j].y = 0 ;
	}
	// SW
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = -(i+nSectionLen/2+1) ;
		buffer[j].y = (i+nSectionLen/2+1) ;
	}
	// S
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = 0 ;
		buffer[j].y = (i+nSectionLen/2+1) ;
	}
	// SE
	for (i=0; i<nSectionLen; i++,j++)
	{
		buffer[j].x = (i+nSectionLen/2+1) ;
		buffer[j].y = (i+nSectionLen/2+1) ;
	}

	return nNumPts;
}*/

int CPixelBuffer::GetCircleCoords(int xCenter, int yCenter, int radius, CPoint *buffer)
{
	int d ;
	int x=0 ;
	int y=radius ;
	CPoint *pts ;
	int nCnt ;
	int tCnt ;
	int mCnt ;
	BOOL bXEqualedY ;

	pts = new CPoint [radius*4] ;
	
	d=3-(2*radius);
	nCnt = 0 ;
	tCnt = 0 ;
	bXEqualedY = false ;
    while(x<=y)
	{
		pts[nCnt].x = x ;
		pts[nCnt].y = y ;
		nCnt++;
		if (d<0)
			d += (x<<2)+6;
		else
		{
			d += ((x-y)<<2)+10;
			y -= 1;
		}
		x++;
		if (x==y)
			bXEqualedY = true ;
	}

//	buffer = new CPoint [nCnt*8] ;
	mCnt=0;
	do
	{
		buffer[tCnt].x = xCenter + pts[mCnt].x  ;
		buffer[tCnt].y = yCenter + pts[mCnt].y  ;
		mCnt++ ;
		tCnt++;
	} while (mCnt<nCnt) ;

	if (bXEqualedY)
		mCnt--;
	do
	{
		mCnt--;
		buffer[tCnt].x = xCenter + pts[mCnt].y ;
		buffer[tCnt].y = yCenter + pts[mCnt].x ;
		tCnt++;
	} while (mCnt>1) ;

	mCnt=0;
	do
	{
		buffer[tCnt].x = xCenter + pts[mCnt].y  ;
		buffer[tCnt].y = yCenter - pts[mCnt].x  ;
		mCnt++ ;
		tCnt++;
	} while (mCnt<nCnt) ;

	if (bXEqualedY)
		mCnt--;
	do
	{
		mCnt--;
		buffer[tCnt].x = xCenter + pts[mCnt].x ;
		buffer[tCnt].y = yCenter - pts[mCnt].y ;
		tCnt++;
	} while (mCnt>1) ;

	mCnt=0;
	do
	{
		buffer[tCnt].x = xCenter - pts[mCnt].x  ;
		buffer[tCnt].y = yCenter - pts[mCnt].y  ;
		mCnt++ ;
		tCnt++;
	} while (mCnt<nCnt) ;

	if (bXEqualedY)
		mCnt--;
	do
	{
		mCnt--;
		buffer[tCnt].x = xCenter - pts[mCnt].y ;
		buffer[tCnt].y = yCenter - pts[mCnt].x ;
		tCnt++;
	} while (mCnt>1) ;

	mCnt=0;
	do
	{
		buffer[tCnt].x = xCenter - pts[mCnt].y  ;
		buffer[tCnt].y = yCenter + pts[mCnt].x  ;
		mCnt++ ;
		tCnt++;
	} while (mCnt<nCnt) ;

	if (bXEqualedY)
		mCnt--;
	do
	{
		mCnt--;
		buffer[tCnt].x = xCenter - pts[mCnt].x ;
		buffer[tCnt].y = yCenter + pts[mCnt].y ;
		tCnt++;
	} while (mCnt>1) ;

	// This allows for the light dark transition check
	// to span at least 3 points.
	//buffer[tCnt] = buffer[0] ;
	//tCnt++;
	//buffer[tCnt] = buffer[1] ;
	//tCnt++;

	delete [] pts ;
	return tCnt;
}

BOOL CPixelBuffer::ValidCheckerPos ( int x, int y, int *pOffsets, int nNumPts )
{
	// This routine counts the number of 0 to 1 crossings (not 1 to 0).
	// If the count is 4 it's a candidate for a valid position.
	int index = Index(x,y) ;
	int i ;
	int v ;
	int lv ;
	int count ;
	int counts[64] ;

	lv = m_buf[index + pOffsets[nNumPts-1]] ;
	memset(counts, 0, sizeof(counts)) ;

	count = 0 ;
	for (i=0; i<nNumPts; i++)
	{
		counts[count]++ ;
		v = m_buf[index+pOffsets[i]] ;
		if (lv<v)
			count++ ;
		lv = v ;
	}

	if (count==4)
	{
		counts[0] += counts[4] ;
		for (i=0; i<4; i++)
			if (counts[i] < 6)
				return false ;

		return true ;
	}
	else
		return false ;
}

void CPixelBuffer::FastGetRadiusValues (int x, int y, int w, int r, int *values)
{
/*
	_asm
	{
	}
*/
}

void CPixelBuffer::GetValidCheckerRange ( int *values
										, int *offsets
										, int nNumPts
										, int nIndex
										, int *minrval
										, int *maxrval
										  )
{
/*
	int *pbuf = m_buf ;

	
	
//	for (i=0; i<nNumPts; i++)
//	{
//		v[i] = m_buf[pOffsets[i] + index] ;
//		if (v[i] > max)
//			max = v[i] ;
//		if (v[i] < min)
//			min = v[i] ;
//	}

	// ebx = min
	// edx = max
	// esi = m_buf+index
	// edi = values
	// ecx = offsets
	_asm
	{
		mov esi, pbuf
		shl nIndex, 2
		add esi, nIndex

		mov edi, values
		mov ecx, offsets

		mov ebx, 256
		mov edx, -1

GetValidCheckerRange_Loop:
		mov eax, [ecx]
		mov eax, [esi+eax*4]
		mov [edi], eax			// v[i] = m_buf[pOffsets[i] + index] ;
		cmp eax, edx			// if (v[i] > max)
		cmovg edx, eax			//     max = v[i]
		cmp eax, ebx			// if (v[i] < min)
		cmovl ebx, eax			//    min = v[i]
		add ecx, 4
		add edi, 4
		dec nNumPts
		jnz GetValidCheckerRange_Loop

		mov esi, minrval
		mov [esi], ebx
		mov esi, maxrval
		mov [esi], edx
	}
*/
}

void CPixelBuffer::GetValidCheckerThreshold(int *values, int nThreshold, int nNumPts)
{
/*
	_asm
	{
//	for (i=0; i<nNumPts; i++)
//	{
//		if (v[i]<=nThreshLow)
//			v[i] = -1 ;
//		else
//			v[i] = 1 ;
//	}
//	v[i] = v[0] ;

		mov esi, values
		mov edi, esi
		mov ecx, nNumPts
		mov eax, nThreshold
		mov ebx, 1
		mov edx, -1
GetValidCheckerThresholdLoop:
		sub [esi], eax
		jg GetValidCheckerThresholdPositive
		mov [esi], edx
		add esi, 4
		loop GetValidCheckerThresholdLoop
		jmp GetValidCheckerThresholdDone

GetValidCheckerThresholdPositive:
		mov [esi], ebx
		add esi, 4
		loop GetValidCheckerThresholdLoop

GetValidCheckerThresholdDone:
		mov eax, [edi]
		mov [esi], eax
	}
*/
}


void CPixelBuffer::Range7x7(int *buf, int *lowv, int *highv, int w)
{
	/*
//	submax = -1 ;
//	submin = 256 ;
//
//	for (iy=y-3; iy<=y+3; iy++)
//	{
//		for (ix=x-3; ix<=x+3; ix++)
//		{
//			vp = GetAtFast(ix,iy) ;
//			if (vp < submin)
//				submin = vp ;
//			if (vp > submax)
//				submax = vp ;
//		}
//	}
//
	_asm
	{
		// ebx = min
		// ecx = max
		// esi = buf
		mov ebx, 0x7FFFFFFF
		mov ecx, 0x80000000
		mov esi, buf
		shl w, 2
		sub esi, w
		sub esi, w
		sub esi, w
		sub esi, 12

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]

		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov eax, [esi]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+4]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+8]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+12]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+16]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+20]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		mov eax, [esi+24]
		cmp eax, ebx
		cmovl ebx, eax
		cmp eax, ecx
		cmovg ecx, eax
		add esi, w

		mov esi, lowv
		mov [esi], ebx
		mov esi, highv
		mov [esi], ecx
	}
*/
}

// A valid checker run goes from DL - LD - DL - LD or
// LD - DL - LD - DL.  The transition must be sharp and
// must cover a certain contrast scale.  The transition
// must occur over a distance of at least 3 points.  There
// will be a minimum transition range that must be met, but
// the delta will be adaptive.  We may even do a threshold.
BOOL CPixelBuffer::ValidCheckerRun (  int x
									, int y
									, int *pOffsets
									, int nNumPts
									, int *pSum
									, int nRadius
									, int nMinRange
									, int nMinArcLen
									, int nMinMatchDist
									, BOOL bDoSet
									  )
{
	int index ;
	int i ;
	int max, min, submax, submin ;
	int nTransCnt ;
	int nThreshLow ;
	int v[64] ;
	int c[64] ;

	index = Index(x,y) ;


	if (index<0)
		return false ;

	Range7x7(m_buf+index, &submin, &submax, m_w) ;
	
	if (submax-submin<(nMinRange*0.9))
		return false ;

	// Get min and max range of points in circle
	GetValidCheckerRange(v, pOffsets, nNumPts, index, &min, &max) ;
	if (max-min>nMinRange)
		nThreshLow = min + (max-min)/2 ;
	else
		nThreshLow = 256 ;
	GetValidCheckerThreshold(v, nThreshLow, nNumPts) ;

	nTransCnt = 0 ;
	c[0] = 0 ;
	for (i=0; i<nNumPts; i++)
	{	
	    if (v[i] * v[i+1]==-1)
		{
			nTransCnt++;
			c[nTransCnt] = 0 ;
		}
		c[nTransCnt]++ ;
	}
	c[0] += c[nTransCnt] ;
	c[nTransCnt] = -1 ;

	if (	nTransCnt==4
		 && (abs(c[0] - c[2]) <= nMinMatchDist)
		 && (abs(c[1] - c[3]) <= nMinMatchDist)
		    )
	{
		return true ;
	}
	else
		return false ;
}

int *CPixelBuffer::Histogram(int nMin /*=0*/, int nMax /*=255*/, int *pRange/*=NULL*/)
{
	int *pbuf ;
	int count = m_size ;
	int range = nMax-nMin+1;
	int *pHistogram = NULL ;
	int v ;

	// If nMin>nMax that means that we want to 
	// do an auto range.
	if (range<=0)
	{
		nMin = GetMin() ;
		nMax = GetMax() ;
		range = nMax-nMin+1 ;
		if (pRange)
			*pRange = range ;
	}
	pbuf = m_buf ;

	pHistogram = new int [range] ;
	memset (pHistogram, 0, sizeof(int)*range) ;

	while (count)
	{
		v = *pbuf - nMin ;
		if (v<0)
			v = 0 ;
		if (v>=range)
			v = range-1 ;
		pHistogram[v]++ ;
		count--;
		pbuf++;
	}

	return pHistogram ;
}

static int SortHistogram ( const void *arg1, const void *arg2)
{
    if (*((int*)arg1) < *((int*)arg2))
        return -1 ;
    if (*((int*)arg1) > *((int*)arg2))
        return 1 ;
    return 0 ;
}

int CPixelBuffer::MedianFromHistogram()
{
    int         *pHistogram ;
    int         Histogram[512] ;
    int         nHistogramMedian ;
	int nRange ;
    pHistogram = this->Histogram(0, 255, &nRange) ;
    memcpy (Histogram, pHistogram, 256*sizeof(int)) ;
    delete pHistogram ;

    // Sort the histogram values
	int nsz = m_size / 2 ;
	int ncttl = 0 ;
	int j ;

	// Find the index that lies at exactly half
	// the brightness levels counted
	for (j=0; j<255; j++)
	{
		ncttl += Histogram[j*2] ;
		if (ncttl > nsz)
			break;
	}
    for (j=255; j>=0; j--)
    {
        Histogram[j*2]   = Histogram[j] ;
        Histogram[j*2+1] = j ;
    }
    qsort ( Histogram, 256, sizeof(int)*2, SortHistogram ) ;

    // Now that the histogram has been sorted, we need to pick the right
    // peaks.  The histogram is ordered such that we know the values as
    // well as it's index position.
    for (j=508; j>=0; j-=2)
    {
		// If we get a zero, go back to the previous
		// and break here
		if (Histogram[j]==0)
		{
			j+= 2 ;
			break;
		}
    if (abs(Histogram[511] - Histogram[j+1]) > 30)
        break ;
    }
    nHistogramMedian = (Histogram[511] + Histogram[j+1]) / 2 ;
	return nHistogramMedian ;
}


BOOL CPixelBuffer::GetSobelCrossCoords  ( CCoord &center
										, DOUBLE dRadius
										, CCoordArray &rval)
{
	int nNumPts ;
	int lv ;
	int v ;
	CPoint pts[1024] ;
	int nCounts[1024] ;
	int nCount ;
	int i ;
    CCoord tp(0,0,0);
    
	rval.SetSize(0) ;
	rval.Add(tp) ;

	nNumPts = GetCircleCoords ( (int)center.m_x
							  , (int)center.m_y
							  , (int)dRadius
							  , pts ) ;

	// Bin up sobel directions
	nCount = 0 ;
	nCounts[0] = 0 ;
	v = 0 ;
	for (i=0; i<nNumPts; i++)
	{
		lv = v ;
		v = GetAt(pts[i]) ;
		if (v)
		{
			nCounts[nCount]++ ;
			rval[nCount] += CCoord ((DOUBLE)pts[i].x, (DOUBLE)pts[i].y, 0) ;
		}
		else
		if (lv)
		{
		    CCoord p(0,0,0);
			nCount++ ;
			nCounts[nCount] = 0 ;
			rval.Add(p) ;
		}
	}
	// If the last bin and the first bin
	// actually touch, be sure to transfer
	// the counts.
	if (v && GetAt(pts[0]))
	{
		nCounts[0] += nCounts[nCount] ;
		nCounts[nCount] = 0 ;
		rval[0] += rval[nCount] ;
		rval.SetSize(nCount-1) ;
		nCount-- ;
	}

	// Perform averaging
	for (i=0; i<nCount; i++)
		rval[i] /= (DOUBLE)nCounts[i] ;

	return true ;
}


void DumpTracePix ( CString &tstr, CPoint *pts, int *deltas, int nNumPts, int nMin, int nMax)
{
	CString numstr ;
	int		i ;

	numstr.Format ("Circle Size = %d Pts   MinVal=%d   MaxVal=%d\n", nNumPts, nMin, nMax) ;
	tstr += numstr ;
	for (i=0; i<nNumPts; i++)
	{
		numstr.Format("Pt[%03d]=%5d,%5d,%4d\n", i, pts[i].x, pts[i].y, deltas[i]) ;
		tstr += numstr ;
	}
	tstr += "\n" ;
}


// We know we have checker intersections when we have a DL - LD - DL - LD
// pattern of light to dark transitions.

BOOL CPixelBuffer::GetCheckerIntersections ( CPixelBuffer	*pSrc
										   , CCoordArray    &rCalibPoints
										   , CCoordArray    &rModelPoints
										   , DOUBLE			dGridSize
										   , int			nNumColumns
										   , int			nNumRows
										   , int			nRadius
										   , BOOL			bShowBuffer
										   , BOOL			bPlotOld
										    )
{
	CPoint      pts[1024] ;
	CString		tstr ;
	int			offsets[1024] ;
	int			deltas[1024] ;
	int			nNumPts;
	int			x, y, i, j, k, ind, lastv, v, nNumDeltas ;
//	int			nHistMax ;
//	int			nHistMin ;
	CCoord		FarthestOne ;
	CCoord		FarthestTwo ;
	CCoord		FarthestThree ;
	CCoord		FarthestFour ;
	CCoord		LeftMostMajor ;
	CCoord		LeftMostMinor ;
	CCoord		RightMostMajor ;
	CCoord		RightMostMinor ;
	CCoord		TopLeft ;
	CCoord		TopRight ;
	CCoord		BottomLeft ;
	CCoord		BottomRight ;
	CCoord		rval ;
	CCoord		ScreenCenter((DOUBLE)m_w/2, (DOUBLE)m_h/2, (DOUBLE)0) ;
	CCoordArray TestSort ;
	CCoordArray ResultSort ;
	CCoordArray CalibPoints ;
	CCoordArray ModelPoints ;
	CPlane		SortPlane ;
	static	CPixelBuffer sobel ;
	static	CPixelBuffer gaussian ;
	static  CPixelBuffer median ;
	static	CPixelBuffer threshold ;
	static	CPixelBuffer radiuscount ;
	static	CPixelBuffer radiusthreshold ;
	static	CPixelBuffer normalize ;
	static  CPixelArray chains ;
	static	CPixelArray Centers ;
	static  CPixelBuffer mask ;
	CLine		TopLine ;
	CLine		BottomLine ;
	CLine		LeftLine ;
	CLine		RightLine ;
	CLine		LineOne ;
	CLine		LineTwo ;
	CCoord		TestLeft ;
	int			nLinesFound ;
	int			sz ;
	int			nMin ;
	int			nMax ;
	int			nCurRows ;
	int			nCurCols ;
	int			r, c ;
	int			nMinRange ;


	if (!Init(pSrc->m_w, pSrc->m_h))
		return false ;

	sobel.Sobel(pSrc) ;
	normalize.Normalize(&sobel) ;
	threshold.Threshold(&normalize,60,255) ;
	threshold.Dialate() ;
	Copy(threshold) ;
	nNumPts = GetCircleCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + m_w*pts[i].y ;
	Clear() ;
	for (y=nRadius;  y<m_h-nRadius;  y++)
	{
		ind = Index(nRadius,y);
		for (x=nRadius;  x<m_w-nRadius;  x++)
		{
			if (threshold.GetAtFast(x,y))
			{
				nNumDeltas = 0 ;
				// First, step through the circle points till you
				// get to the first zero pixel in the threshold
				// buffer.
				lastv = threshold.m_buf[ind+offsets[0]] ;
				for (i=0; i<nNumPts; i++)
				{
					v = threshold.m_buf[ind+offsets[i]] ;
					if (v && !lastv)
						break;
					lastv = v ;
				}
				j = i ;

				// Next, go through all the points, starting at the
				// zero pixel that you found.  Count the deltas
				// between the pixels.
				lastv = v ;
				deltas[0] = 0 ;
				for (i=0; i<nNumPts+1; i++,j++)
				{
					k = j % nNumPts;
					v = threshold.m_buf[ind+offsets[k]] ;
					if (v && !lastv)
					{
						nNumDeltas++ ;
						deltas[nNumDeltas] = 0 ;
					}
					else
						deltas[nNumDeltas]++ ;
					lastv = v ;
				}

				// Finally, count deltas and delta spacing
				if (nNumDeltas==4)
				{
					// We may have the right number of deltas, but is 
					// the spacing consistent?
					if ( abs(deltas[0]-deltas[2]) < 5
						&& abs(deltas[1]-deltas[3]) < 5 
						)
						SetAt(x,y,255) ;
				}
			}
			ind++ ;
		}
	}


	// By now, we should have the intersections we desire.  The object now is
	// to count the number of intersection blobs we find and plot 
	if (bShowBuffer)
		ShowBuffer() ;
	Dialate() ;
	mask.Copy(*this) ;
	Copy(*pSrc, mask, true) ;

	return true ;

	if (bShowBuffer)
		ShowBuffer() ;
	FindRegions(Centers, 10, 8*8) ;
	Copy(*pSrc) ;
//	Div(2) ;
	CalibPoints.SetSize(0) ;
	ResultSort.SetSize(0) ;
	for (i=0; i<Centers.GetSize(); i++)
	{
	    CCoord p((DOUBLE)Centers[i].x, (DOUBLE)Centers[i].y, (DOUBLE)0.0);
		ResultSort.Add(p) ;
    }
    
	BOOL bSubPixFailure = false ;
	// Now that we have the centers of the points, we can get the 
	// sub pixel cross location
	for (i=0; i<ResultSort.GetSize(); i++)
	{
		CPoint centerpt = ResultSort[i] ;
		// Get the circle gray values
		nMin = 255 ;
		nMax = 0 ;
		for (j=0; j<nNumPts; j++)
		{
			deltas[j] = pSrc->GetAtFast ( pts[j] + centerpt ) ;
			if (deltas[j] < nMin)
				nMin = deltas[j] ;
			if (deltas[j] > nMax)
				nMax = deltas[j] ;
//			SetAtFast ( pts[j] + centerpt, 255 ) ;
			if (j<10)
			{
				deltas[j+nNumPts] = deltas[j] ;
				pts[j+nNumPts] = pts[j] ;
			}
		}

		nMinRange = (nMax-nMin)*8/10 ;
		int nMidRange = (nMax+nMin)/2 ;
		DOUBLE dSubRange ;
		DOUBLE dSubDelta ;
		BOOL bWhiteSearch = deltas[0]>nMidRange ? false : true ;
		BOOL bSearchResult ;

		nLinesFound = 0 ;

//		if ((nMax-nMin)>50)
		{

			// First, look for black to white transitions
			for (j=1; j<nNumPts+1; j++) 
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
					
					LineOne.m_first = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineOne.m_first, 2, 255) ;
					nLinesFound |= 0x00000001 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;
			// Now, look for white to black transitions
			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
					
					LineTwo.m_first = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineTwo.m_first, 2, 255) ;
					nLinesFound |= 0x00000004 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;
			// Now, look for white to black transitions
			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				//if (deltas[j+6] - deltas[j] > nMinRange)
				{
					CCoord cf ( pts[j] + centerpt ) ;
					CCoord cs ( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
					
					LineOne.m_second = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineOne.m_second, 2, 255) ;
					nLinesFound |= 0x00000002 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;

			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
					
					LineTwo.m_second = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineTwo.m_second, 2, 255) ;
					nLinesFound |= 0x00000008 ;
					break;
				}
			}
		}

		if (nLinesFound & 0x00000003 == 0x00000003)
		{
		}

		if (nLinesFound & 0x0000000C == 0x0000000C)
		{
		}

		if (nLinesFound == 0x0000000F)
		{
			CCoord c ;
			LineTwo.Intersect(LineOne, c);
			CalibPoints.Add(c) ;
		}
		else
		{
			bSubPixFailure = true;
		}
	}

	if (CalibPoints.GetSize()!=nNumRows * nNumColumns)
	{
		for (i=0; i<CalibPoints.GetSize(); i++)
		{
			//if (GetAt(CalibPoints[i])==255)
			//	MessageBeep(0) ;
			DrawCross(CalibPoints[i], 3, 255) ;
		}
		if (bShowBuffer)
			ShowBuffer() ;
		return false ;
	}

	// Sort the centers into rows and columns

	// The farthest point from the center will be a corner
	FarthestOne = CalibPoints.Farthest(ScreenCenter) ;
	// The farthest point from the found corner will be another corner
	FarthestTwo = CalibPoints.Farthest(FarthestOne) ;

	// Use those two points to create a plane
	SortPlane.Set (FarthestOne, FarthestTwo, FarthestTwo+CCoord(0,0,1)) ;
	TestSort = CalibPoints ;
	FarthestThree = SortPlane.FindFarthest(true,  TestSort) ;
	FarthestFour  = SortPlane.FindFarthest(false, TestSort) ;

	if (FarthestOne==FarthestTwo)
	{
		return false ;
	}

	// Now we have all the corner points.  Which one is left-top most?
	LeftMostMajor = (FarthestOne.m_x < FarthestTwo.m_x)    ? FarthestOne   : FarthestTwo ;
	LeftMostMinor = (FarthestThree.m_x < FarthestFour.m_x) ? FarthestThree : FarthestFour ;
	TopLeft = (LeftMostMajor.m_y < LeftMostMinor.m_y) ? LeftMostMajor : LeftMostMinor ;
	BottomLeft = (LeftMostMajor.m_y > LeftMostMinor.m_y) ? LeftMostMajor : LeftMostMinor ;

	RightMostMajor = (FarthestOne.m_x > FarthestTwo.m_x)    ? FarthestOne   : FarthestTwo ;
	RightMostMinor = (FarthestThree.m_x > FarthestFour.m_x) ? FarthestThree : FarthestFour ;
	TopRight = (RightMostMajor.m_y < RightMostMinor.m_y) ? RightMostMajor : RightMostMinor ;
	BottomRight = (RightMostMajor.m_y > RightMostMinor.m_y) ? RightMostMajor : RightMostMinor ;

	TopLine.Set(TopLeft, TopRight) ;
	BottomLine.Set(BottomLeft, BottomRight) ;
	LeftLine.Set (TopLeft, BottomLeft) ;
	RightLine.Set (TopRight, BottomRight) ;

	nCurRows = 0 ;
	nCurCols = 0 ;

	ResultSort.SetSize(0) ;
	
	while (TestSort.PopNearest(rval, TopLeft, LeftLine.Slope(), (DOUBLE)10.0, -1, 10000) )
	{
		ResultSort.Add(rval) ;
		TestLeft = TopLeft ;

		while (TestSort.PopNearest(rval, TestLeft, TopLine.Slope(), (DOUBLE)10.0, -1, 10000))
		{
			TestLeft = rval ;
			ResultSort.Add(rval) ;
		}
		if (nCurCols==0)
			nCurCols = ResultSort.GetSize() ;

//		if (nCurCols != nNumColumns)
//		{
//			for (i=0; i<ResultSort.GetSize()-2; i++)
//				DrawCross(ResultSort[i], 3, 255) ;
//			ShowBuffer() ;
//			return false ;
//		}

		nCurRows++ ;

		if (TestSort.Nearest(rval, TopLeft, LeftLine.Slope(), (DOUBLE)10.0, -1, 10000))
			TopLeft = rval;
		if (TestSort.Nearest(rval, TopRight, RightLine.Slope(), (DOUBLE)10.0, -1, 10000) )
			TopRight = rval ;
		TopLine.Set(TopLeft, TopRight) ;
	}

	if (ResultSort.GetSize() != nNumColumns * nNumRows)
		return false ;


	if (nCurRows * nCurCols != nNumColumns * nNumRows)
	{
		for (i=0; i<CalibPoints.GetSize(); i++)
			DrawCross(CalibPoints[i], 3, 255) ;
		if (bShowBuffer)
			ShowBuffer() ;
		return false ;
	}

	// At this point, we're somewhat ordered.  The arrangement goes from topleft
	// to topright and then topleft to bottomleft.  This may not be the desired
	// arrangement.  If the obtained number of rows does not equal the desired
	// number of rows, we will need to reorganize them by transposing the 
	// arrangement (rotating 90 degrees).  We should ALWAYS return the proper
	// number of columns and rows.
	//
	// The final ordering
	if (nNumRows != nCurRows)
	{
		TestSort = ResultSort ;
		for (r=0; r<nNumRows; r++)
		{
			for (c=0; c<nNumColumns; c++)
			{
				ResultSort[(nNumColumns-c-1)+r*nNumColumns] = TestSort[r+c*nCurCols] ;
			}
		}
	}

	// Now, set up the model array too.  Use the dGridSize to create the
	// proper element sizes.  The model array will use the x's for the 
	// cols and the y's for the rows.
	ModelPoints.SetSize(0) ;
	for (r=0; r<nNumRows; r++)
	{
		for (c=0; c<nNumColumns; c++)
		{
		    CCoord p((DOUBLE)c*dGridSize,(DOUBLE)r*dGridSize,(DOUBLE)0.0);
			ModelPoints.Add(p) ;
		}
	}

	CalibPoints = ResultSort ;

	if (bSubPixFailure)
		return false ;

	sz = CalibPoints.GetSize() ;
	for (i=0; i<sz; i++)
	{
		DrawCross(CalibPoints[i], 5, MulDiv(i,155,sz)+100) ;
		if (i)
			SetLine(CalibPoints[i], CalibPoints[i-1], 255) ;
	}
	DrawCross(FarthestOne, 15, 176) ;
	DrawCross(FarthestTwo, 15, 176) ;
	DrawCross(FarthestThree, 15, 176) ;
	DrawCross(FarthestFour, 15, 176) ;
	DrawCross(CalibPoints[0], 5, 255) ;
	if (bShowBuffer)
		ShowBuffer() ;

	// Only modify the input parameters if we have successfully calibrated a frame
	rCalibPoints = CalibPoints ;
	rModelPoints = ModelPoints ;

	return true ;
}

double CPixelBuffer::StdDev ( int ind, int *offsets, int nNumPts, double dMean )
{
	int    i ;
	double dSum ;
	int    nSum ;
	int    nVal ;

	nSum = 0 ;
	for (i=0; i<nNumPts; i++)
	{
		nVal = m_buf[ind+offsets[i]] ;
		nSum += nVal*nVal ;
	}
	dSum = sqrt (((double)nSum - (double)nNumPts*dMean*dMean) / (double)nNumPts) ;
	return dSum ;
}

double CPixelBuffer::Mean ( int ind, int *offsets, int nNumPts )
{
	double dSum = 0.0 ;
	int    i ;

	for (i=0; i<nNumPts; i++)
		dSum += m_buf[ind+offsets[i]] ;
	dSum /= (double)nNumPts ;
	return dSum ;
}

// This specifically checks Milano style intersections!
// The m_dHistogramHigh and m_dHistogramLow values should
// have been set before executing this...
BOOL CPixelBuffer::IsMilanoCheckerIntersection (int			*offsets
										 , int			x 
										 , int			y
										 , int			nNumPts
										   )
{
	int ind, i, nNumSectionPts ;
	double alpha = 0.1 * 255.0 ;
	double beta = 0.9 ;
	double dMean[9] ;
	double dStdDev[9] ;
	int    *pSectionOffsets ;

	nNumSectionPts = nNumPts / 9 ;
	ind = Index ( x, y ) ;

	for (i=0; i<9; i++)
	{
		pSectionOffsets = offsets + i*nNumSectionPts ;
		dMean[i] = Mean ( ind, pSectionOffsets, nNumSectionPts ) ;
		dStdDev[i] = StdDev ( ind, pSectionOffsets, nNumSectionPts, dMean[i] ) ;
	}

	if (   dStdDev[0] < m_dHistogramLow
		&& dStdDev[2] < m_dHistogramLow
		&& dStdDev[6] < m_dHistogramLow
		&& dStdDev[8] < m_dHistogramLow
		&& fabs(dMean[0] - dMean[8]) < m_dHistogramLow
		&& fabs(dMean[2] - dMean[6]) < m_dHistogramLow
		&& fabs(dMean[0] - dMean[2]) > m_dHistogramHigh
		&& fabs(dMean[2] - dMean[8]) > m_dHistogramHigh
		&& fabs(dMean[6] - dMean[8]) > m_dHistogramHigh
		&& fabs(dMean[6] - dMean[0]) > m_dHistogramHigh )
		return true ;

	if (   dStdDev[1] < m_dHistogramLow
		&& dStdDev[3] < m_dHistogramLow
		&& dStdDev[5] < m_dHistogramLow
		&& dStdDev[7] < m_dHistogramLow
		&& fabs(dMean[1] - dMean[7]) < m_dHistogramLow
		&& fabs(dMean[3] - dMean[5]) < m_dHistogramLow
		&& fabs(dMean[1] - dMean[3]) > m_dHistogramHigh
		&& fabs(dMean[1] - dMean[5]) > m_dHistogramHigh
		&& fabs(dMean[5] - dMean[7]) > m_dHistogramHigh
		&& fabs(dMean[7] - dMean[3]) > m_dHistogramHigh )
		return true ;

	return false ;
}

BOOL CPixelBuffer::IsCheckerIntersection ( CPixelBuffer &threshold
										 , int			*offsets
										 , int			x 
										 , int			y
										 , int			nNumPts
										   )
{
	int ind ;
	int lastv ;
	int	nNumDeltas ;
	int deltas[1024] ;
	int i ;
	int j ;
	int k ;
	int v ;

	ind = threshold.Index(x,y);
	if (threshold.GetAtFast(x,y))
	{
		nNumDeltas = 0 ;
		// First, step through the circle points till you
		// get to the first zero pixel in the threshold
		// buffer.
		lastv = threshold.m_buf[ind+offsets[0]] ;
		for (i=0; i<nNumPts; i++)
		{
			v = threshold.m_buf[ind+offsets[i]] ;
			if (v && !lastv)
				break;
			lastv = v ;
		}
		j = i ;

		// Next, go through all the points, starting at the
		// zero pixel that you found.  Count the deltas
		// between the pixels.
		lastv = v ;
		deltas[0] = 0 ;
		for (i=0; i<nNumPts+1; i++,j++)
		{
			k = j % nNumPts;
			v = threshold.m_buf[ind+offsets[k]] ;
			if (v && !lastv)
			{
				nNumDeltas++ ;
				deltas[nNumDeltas] = 0 ;
			}
			else
				deltas[nNumDeltas]++ ;
			lastv = v ;
		}

		// Finally, count deltas and delta spacing
		if (nNumDeltas==4)
		{
			// We may have the right number of deltas, but is 
			// the spacing consistent?
			if ( abs(deltas[0]-deltas[2]) < 5
				&& abs(deltas[1]-deltas[3]) < 5
				)
				return true ;
		}
	}
	return false ;
}

BOOL CPixelBuffer::IsCheckerIntersectionArea ( int			nThreshold
											  , int			x 
											 , int			y
											 , int			nRadius )
{
	int i, j, v, w, h, nNumVals, nSum, nMax, nMin ;

	w = m_w ;
	h = m_h ;
	nSum = 0 ;
	nNumVals = 0 ;
	nMax = 0 ;
	nMin = 255 ;

	for (i=x-nRadius; i<=x+nRadius; i++)
	{
		if (i>=0 && i<w)
		{
			for (j=y-nRadius; j<y+nRadius; j++)
			{
				if (j>=0 && j<h)
				{
					v = GetAtFast ( i, j ) ;
					if (v > nMax)
						nMax = v ;
					if (v < nMin)
						nMin = v ;
					nSum += v ;
					nNumVals++ ;
				}
			}
		}
	}

	nSum /= nNumVals ;
	if (   (nMax - nMin >= nThreshold)
		&& ((double)(nSum-nMin)/(double)(nMax-nMin) > 0.35)
		&& ((double)(nSum-nMin)/(double)(nMax-nMin) < 0.75)
		)
		return true ;
	else
		return false ;
}
										   

BOOL CPixelBuffer::GetCheckerIntersections ( CPixelBuffer	*pSrc
										   , int			nThreshold
										   , int			nRadius
										    )
{
	CPoint      pts[1024] ;
	CString		tstr ;
	int			offsets[1024] ;
	int			nNumPts;
	int			x, y, i;
//	int			nHistMax ;
//	int			nHistMin ;
	CCoord		FarthestOne ;
	CCoord		FarthestTwo ;
	CCoord		FarthestThree ;
	CCoord		FarthestFour ;
	CCoord		LeftMostMajor ;
	CCoord		LeftMostMinor ;
	CCoord		RightMostMajor ;
	CCoord		RightMostMinor ;
	CCoord		TopLeft ;
	CCoord		TopRight ;
	CCoord		BottomLeft ;
	CCoord		BottomRight ;
	CCoord		rval ;
	CCoord		ScreenCenter((DOUBLE)m_w/2, (DOUBLE)m_h/2, (DOUBLE)0) ;
	CCoordArray TestSort ;
	CCoordArray ResultSort ;
	CCoordArray CalibPoints ;
	CCoordArray ModelPoints ;
	CPlane		SortPlane ;
	static	CPixelBuffer sobel ;
	static	CPixelBuffer gaussian ;
	static  CPixelBuffer median ;
	static	CPixelBuffer threshold ;
	static	CPixelBuffer radiuscount ;
	static	CPixelBuffer radiusthreshold ;
	static	CPixelBuffer normalize ;
	static  CPixelArray chains ;
	static	CPixelArray Centers ;
	static  CPixelBuffer mask ;
	CLine		TopLine ;
	CLine		BottomLine ;
	CLine		LeftLine ;
	CLine		RightLine ;
	CLine		LineOne ;
	CLine		LineTwo ;
	CCoord		TestLeft ;


	if (!Init(pSrc->m_w, pSrc->m_h))
		return false ;

	sobel.Sobel(pSrc) ;
	normalize.Normalize(&sobel) ;
	threshold.Threshold(&normalize,nThreshold,255) ;
	threshold.Dialate() ;
	Copy(threshold) ;

	nNumPts = GetCircleCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + m_w*pts[i].y ;
	Clear() ;
	for (y=nRadius;  y<m_h-nRadius;  y++)
	{
		for (x=nRadius;  x<m_w-nRadius;  x++)
		{
			if (IsCheckerIntersection(threshold, offsets, x, y, nNumPts))
			{
			}
		}
	}


	// By now, we should have the intersections we desire.  The object now is
	// to count the number of intersection blobs we find and plot 
	Dialate() ;
	mask.Copy(*this) ;
	Copy(*pSrc, mask, true) ;

	return true ;

	/*
	FindRegions(Centers, 10, 8*8) ;
	Copy(*pSrc) ;
//	Div(2) ;
	CalibPoints.SetSize(0) ;
	ResultSort.SetSize(0) ;
	for (i=0; i<Centers.GetSize(); i++)
		ResultSort.Add(CCoord(Centers[i].x, Centers[i].y, 0)) ;

	BOOL bSubPixFailure = false ;
	// Now that we have the centers of the points, we can get the 
	// sub pixel cross location
	for (i=0; i<ResultSort.GetSize(); i++)
	{
		CPoint centerpt = ResultSort[i] ;
		// Get the circle gray values
		nMin = 255 ;
		nMax = 0 ;
		for (j=0; j<nNumPts; j++)
		{
			deltas[j] = pSrc->GetAtFast ( pts[j] + centerpt ) ;
			if (deltas[j] < nMin)
				nMin = deltas[j] ;
			if (deltas[j] > nMax)
				nMax = deltas[j] ;
//			SetAtFast ( pts[j] + centerpt, 255 ) ;
			if (j<10)
			{
				deltas[j+nNumPts] = deltas[j] ;
				pts[j+nNumPts] = pts[j] ;
			}
		}

		nMinRange = (nMax-nMin)*8/10 ;
		int nMidRange = (nMax+nMin)/2 ;
		DOUBLE dSubRange ;
		DOUBLE dSubDelta ;
		BOOL bWhiteSearch = deltas[0]>nMidRange ? false : true ;
		BOOL bSearchResult ;

		nLinesFound = 0 ;

//		if ((nMax-nMin)>50)
		{

			// First, look for black to white transitions
			for (j=1; j<nNumPts+1; j++) 
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs(deltas[j] - deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs(nMidRange - deltas[j]) ;
					
					LineOne.m_first = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineOne.m_first, 2, 255) ;
					nLinesFound |= 0x00000001 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;
			// Now, look for white to black transitions
			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs(deltas[j] - deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs(nMidRange - deltas[j]) ;
					
					LineTwo.m_first = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineTwo.m_first, 2, 255) ;
					nLinesFound |= 0x00000004 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;
			// Now, look for white to black transitions
			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				//if (deltas[j+6] - deltas[j] > nMinRange)
				{
					CCoord cf ( pts[j] + centerpt ) ;
					CCoord cs ( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs(deltas[j] - deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs(nMidRange - deltas[j]) ;
					
					LineOne.m_second = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineOne.m_second, 2, 255) ;
					nLinesFound |= 0x00000002 ;
					break;
				}
			}
			bWhiteSearch = !bWhiteSearch ;

			for (;j<nNumPts+1; j++)
			{
				bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
				if (bSearchResult)
				{
					CCoord cf( pts[j] + centerpt ) ;
					CCoord cs( pts[j-1] + centerpt ) ;
					CVector v(cf,cs) ;

					dSubRange = (DOUBLE)fabs(deltas[j] - deltas[j-1]) ;
					dSubDelta = (DOUBLE)fabs(nMidRange - deltas[j]) ;
					
					LineTwo.m_second = cf + (v * (dSubDelta/dSubRange)) ;
					//DrawCross(LineTwo.m_second, 2, 255) ;
					nLinesFound |= 0x00000008 ;
					break;
				}
			}
		}

		if (nLinesFound & 0x00000003 == 0x00000003)
		{
		}

		if (nLinesFound & 0x0000000C == 0x0000000C)
		{
		}

		if (nLinesFound == 0x0000000F)
		{
			CCoord c ;
			LineTwo.Intersect(LineOne, c);
			CalibPoints.Add(c) ;
		}
		else
		{
			bSubPixFailure = true;
		}
	}
	
	for (i=0; i<CalibPoints.GetSize(); i++)
		DrawCross(CalibPoints[i], 3, 255) ;
	
	return true ;
	*/
}

BOOL CPixelBuffer::GetCheckerIntersections (CCoordArray &Guesses, CCoordArray &Results, int  nRadius, int nCheckRadius)
{
	BOOL    bSubPixFailure = false ;
	CPoint  pts[1024] ;
	CString	tstr ;
	int		offsets[1024] ;
	int		deltas[1024] ;
	int		nNumPts;
	int		i ;
	int     j ;
	int		nMin ;
	int		nMax ;
	int		nLinesFound ;
	CLine   LineOne ;
	CLine   LineTwo ;

	Results.SetSize(0) ;
	// Now that we have the centers of the points, we can get the 
	// sub pixel cross location
	nNumPts = GetCircleCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + m_w*pts[i].y ;

	for (i=0; i<Guesses.GetSize(); i++)
	{
		CPoint centerpt = Guesses[i] ;

		if (   (centerpt.x >= nRadius) 
			&& (centerpt.x<(m_w-nRadius))
			&& (centerpt.y >= nRadius) 
			&& (centerpt.y<(m_h-nRadius))
			   )
		{
			// Get the circle gray values
			nMin = 255 ;
			nMax = 0 ;
			for (j=0; j<nNumPts; j++)
			{
				deltas[j] = GetAtFast ( pts[j] + centerpt ) ;
				if (deltas[j] < nMin)
					nMin = deltas[j] ;
				if (deltas[j] > nMax)
					nMax = deltas[j] ;
	//			SetAtFast ( pts[j] + centerpt, 255 ) ;
				if (j<10)
				{
					deltas[j+nNumPts] = deltas[j] ;
					pts[j+nNumPts] = pts[j] ;
				}
			}

			nMinRange = (nMax-nMin)*8/10 ;
			int nMidRange = (nMax+nMin)/2 ;
			DOUBLE dSubRange ;
			DOUBLE dSubDelta ;
			BOOL bWhiteSearch = deltas[0]>nMidRange ? false : true ;
			BOOL bSearchResult ;

			nLinesFound = 0 ;

	//		if ((nMax-nMin)>50)
			{

				// First, look for black to white transitions
				for (j=1; j<nNumPts+1; j++) 
				{
					bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
					if (bSearchResult)
					{
						CCoord cf( pts[j] + centerpt ) ;
						CCoord cs( pts[j-1] + centerpt ) ;
						CVector v(cf,cs) ;

						dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
						dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
						
						LineOne.m_first = cf + (v * (dSubDelta/dSubRange)) ;
						//DrawCross(LineOne.m_first, 2, 255) ;
						nLinesFound |= 0x00000001 ;
						break;
					}
				}
				bWhiteSearch = !bWhiteSearch ;
				// Now, look for white to black transitions
				for (;j<nNumPts+1; j++)
				{
					bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
					if (bSearchResult)
					{
						CCoord cf( pts[j] + centerpt ) ;
						CCoord cs( pts[j-1] + centerpt ) ;
						CVector v(cf,cs) ;

						dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
						dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
						
						LineTwo.m_first = cf + (v * (dSubDelta/dSubRange)) ;
						//DrawCross(LineTwo.m_first, 2, 255) ;
						nLinesFound |= 0x00000004 ;
						break;
					}
				}
				bWhiteSearch = !bWhiteSearch ;
				// Now, look for white to black transitions
				for (;j<nNumPts+1; j++)
				{
					bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
					if (bSearchResult)
					//if (deltas[j+6] - deltas[j] > nMinRange)
					{
						CCoord cf ( pts[j] + centerpt ) ;
						CCoord cs ( pts[j-1] + centerpt ) ;
						CVector v(cf,cs) ;

						dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
						dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
						
						LineOne.m_second = cf + (v * (dSubDelta/dSubRange)) ;
						//DrawCross(LineOne.m_second, 2, 255) ;
						nLinesFound |= 0x00000002 ;
						break;
					}
				}
				bWhiteSearch = !bWhiteSearch ;

				for (;j<nNumPts+1; j++)
				{
					bSearchResult = bWhiteSearch ? (deltas[j] > nMidRange) : (deltas[j] < nMidRange) ;
					if (bSearchResult)
					{
						CCoord cf( pts[j] + centerpt ) ;
						CCoord cs( pts[j-1] + centerpt ) ;
						CVector v(cf,cs) ;

						dSubRange = (DOUBLE)fabs((DOUBLE)deltas[j] - (DOUBLE)deltas[j-1]) ;
						dSubDelta = (DOUBLE)fabs((DOUBLE)nMidRange - (DOUBLE)deltas[j]) ;
						
						LineTwo.m_second = cf + (v * (dSubDelta/dSubRange)) ;
						//DrawCross(LineTwo.m_second, 2, 255) ;
						nLinesFound |= 0x00000008 ;
						break;
					}
				}
			}

			if (nLinesFound & 0x00000003 == 0x00000003)
			{
			}

			if (nLinesFound & 0x0000000C == 0x0000000C)
			{
			}

			if (nLinesFound == 0x0000000F)
			{
				CCoord c ;
				LineTwo.Intersect(LineOne, c);
				if ( nCheckRadius<0 
					|| nCheckRadius>=Guesses[i].Distance (c))
				{
					c.m_sx = Guesses[i].m_sx ;
					c.m_sy = Guesses[i].m_sy ;
					c.m_nData = Guesses[i].m_nData ;
					Results.Add(c) ;
				}
			}
			else
			{
			    CCoord p(-999,-999,-999);
				Results.Add(p) ;
				bSubPixFailure = true;
			}
		}
//		else
//			Results.Add (Guesses[i]) ;
	}
	
	return true ;
}

// Guess[0] = Origin
// Guess[1] = Z Extent
// Guess[2] = Y Extent
// Guess[3] = ZY Extent
BOOL CPixelBuffer::GetAllCheckerIntersections 
	( 
        CCoordArray  &ReturnValues
	  , int			 nThreshold
	  , int			 nRadius
	 )
{
	CPoint				 pts[1024] ;
	int					 offsets[1024] ;
	int					 nNumPts ;
	static	CPixelBuffer sobel ;
	static	CPixelBuffer threshold ;
	static	CPixelBuffer normalize ;
	static  CPixelBuffer findcheckers ;
	CPixelArray			 Centers ;
	CCoordArray			 RegionCenters ;
	CCoordArray			 RefineIntersections ;
	int					 i ;
	int					 x ;
	int					 y ;

	sobel.Sobel(this) ;
	normalize.Normalize(&sobel) ;
	threshold.Threshold(&normalize,nThreshold,255) ;
	findcheckers.Init(m_w, m_h) ;
	nNumPts = GetCircleCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + m_w*pts[i].y ;
	for (y=nRadius;  y<m_h-nRadius;  y++)
	{
		for (x=nRadius;  x<m_w-nRadius;  x++)
		{
			if (IsCheckerIntersection(threshold, offsets, x, y, nNumPts))
			{
				findcheckers.SetAt(x,y,255) ;
			}
		}
	}

//	findcheckers.ShowBuffer() ;

	findcheckers.Dialate() ;
	findcheckers.FindRegions(Centers, 19, 8*8) ;
	findcheckers.Clear() ;
	for (i=0; i<Centers.GetSize(); i++)
	{
	    CCoord p((DOUBLE)Centers[i].x, (DOUBLE)Centers[i].y, (DOUBLE)0);
		RegionCenters.Add(p) ;
    }
    
	GetCheckerIntersections(RegionCenters, ReturnValues, nRadius) ;
/*
	ReturnValues.SetSize (0) ;

	// Now refine our checks by trying a range of radii
	for (j=0; j<RefineIntersections.GetSize(); j++)
	{
		CCoord &c = RefineIntersections.ElementAt (j) ;

		if (IsCheckerIntersectionArea (nThreshold, (int)c.m_x, (int)c.m_y, nRadius))
			ReturnValues.Add (c)  ;
	}
/*/
	return true ;
}

/*
BOOL CPixelBuffer::GetAllMilanoCheckerIntersections 
	( 
        CCoordArray  &ReturnValues
	  , CCoordArray  &SquareCenters
	  , CPixelChains &SquarePixels
	  , CPixelBuffer &SquareMask
	  , int			 nRadius
	  ,	BOOL		 bGetBlackSquares
	 )
{
	CPoint				 pts[1024] ;
	int					 offsets[1024] ;
	int					 totals[2048] ;
	int					 sums[2048] ;
	int					 nNumPts ;
	int				     nNumTotals ;
	CPixelBuffer		 sobel ;
	CPixelBuffer		 threshold ;
	CPixelBuffer		 normalize ;
	CPixelBuffer		 findcheckers ;
	CPixelBuffer		 thresholdrgn ;
	CPixelBuffer		 squareoutlines ;
	CPixelBuffer		 squareoutlinesnoctr ;
	CPixelBuffer		 medianbuf ;
	CPixelBuffer		 findregions ;
	CPixelBuffer		 histogram ;
	CPixelBuffer		 squarergns ;
	CPixelArray			 Centers ;
	CPixelBuffer		 image ;
	CCoordArray			 RegionCenters ;
	CCoordArray			 guesses ;
	CCoordArray			 centers ;
//	int					 nRadiusLarge = nRadius * 3 / 2 ;
	int					 i ;
	int					 j ;
	int					 x ;
	int					 y ;
	int					 v ;
	int					 vl ;
	int					 nSum ;
	int					 nCount ;
	int					 nLow ;
	int					 nHigh ;
	int					 nMean ;
	
	// Sort the histogram values
	ReturnValues.SetSize (0) ;
	image.Copy(*this) ;
   	sobel.Sobel ( this ) ;

	sobel.ShowBuffer() ;


	// Find the outlines of the squares.  To really be robust,
	// we need to figure out what would be the best value here.
	// We could use a histogram function of some sort to 
	// accomplish this.
	nMean = sobel.TrueMean() ;
	//threshold.Threshold (&sobel, nMean, -1) ;
	threshold.AdaptiveThreshold_SAT ( &sobel, 20, 50 ) ;

	threshold.ShowBuffer() ;

	// Enarge just a little to close gaps if any exist
	//threshold.Dialate (1) ;

	// Save the outlines
	squareoutlines.Copy ( threshold ) ;
	

	// Find out which outlines are too small to be square outlines
	// Anything smaller than the prescribed limit gets deleted (set
	// to zero)
	nNumTotals = thresholdrgn.FindRegions ( &threshold, totals, 2048 ) ;
	for (j=0; j<thresholdrgn.m_size; j++)
	{
		v = thresholdrgn.m_buf[j] ;
		if (v)
		{
			if (totals[v-1] < 6000 )
				thresholdrgn.m_buf[j] = 0 ;
			else
				thresholdrgn.m_buf[j] = 255 ;
		}
	}

	// Save the outlines without the center patterns
	squareoutlinesnoctr.Copy (thresholdrgn) ;
	squareoutlinesnoctr.ShowBuffer() ;
	SquareMask.Copy(squareoutlinesnoctr) ;

	// Now, replace the 0's with -1's and the 255's with 0's.
	thresholdrgn.ReplaceVal (0, -1) ;
	thresholdrgn.ReplaceVal (255, 0) ;
	thresholdrgn.ShowBuffer() ;


	// O.K.  Now classify the exisiting regions so we can isolate
	// the squares (somewhat).
	nNumTotals = threshold.FindRegions ( &thresholdrgn, totals, 2048 ) ;
	for (j=0; j<threshold.m_size; j++)
	{
		v = threshold.m_buf[j] ;
		if (v)
		{
			if (totals[v-1] > 10000)
				threshold.m_buf[j] = 0 ;
			if (totals[v-1] < 1500 )
				threshold.m_buf[j] = 0 ;
		}
	}

	for (j=0; j<nNumTotals; j++)
	{
		if (totals[j] > 10000)
			totals[j] = 0 ;
		if (totals[j] < 1500 )
			totals[j] = 0 ;
	}

//	Threshold (&threshold,1, 255) ;
//	return true;

	// Alright.  Anywhere there is nonzero, use these values to come up
	// with a histogram!  Essentially, we'll sum the values from the 
	// original image to come up with a real average for the block.
	memset (sums, 0, sizeof(sums)) ;
	
	SquareCenters.SetSize (nNumTotals) ;
	SquarePixels.SetSize (nNumTotals) ;
	for (j=0; j<nNumTotals; j++)
	{
		SquareCenters[j].Clear() ;
		SquarePixels[j].SetSize(0,1024) ;
	}

	j=0; 
	for (y=0; y<threshold.m_h; y++)
	{
		for (x=0; x<threshold.m_w; x++,j++)
		{
			v = threshold.m_buf[j] ;
			if (v && (squareoutlines.m_buf[j]==0))
			{
				sums[v-1] += this->m_buf[j] ;
				SquareCenters[v-1].Add (x,y,1,this->m_buf[j]) ;
				SquarePixels[v-1].Add ( CPixel(x,y,v-1) ) ;
			}
		}
	}
	// Now, calculate the average for the blocks
	for (j=0; j<nNumTotals; j++)
	{
		if (totals[j])
			sums[j] /= totals[j] ;
	}

	for (j=0; j<SquareCenters.GetSize(); j++)
	{
		if (SquareCenters.ElementAt(j).m_z > 0)
		{
			SquareCenters.ElementAt(j).m_x /= SquareCenters.ElementAt(j).m_z ;
			SquareCenters.ElementAt(j).m_y /= SquareCenters.ElementAt(j).m_z ;
			SquareCenters.ElementAt(j).m_nData /= (int)SquareCenters.ElementAt(j).m_z ;
		}
	}

	// Find highs and lows
	nLow = 255 ;
	nHigh = 0 ;
	for (j=0; j<nNumTotals; j++)
	{
		if (sums[j] > nHigh)
			nHigh = sums[j] ;
		if (sums[j] < nLow)
			nLow = sums[j] ;
	}

	// Finally, replace the values in the threshold image with the
	// block averages
	for (j=0; j<threshold.m_size; j++)
	{
		v = threshold.m_buf[j] ;
		if (v)
			threshold.m_buf[j] = sums[v-1] ;
	}

	// Finally, sum the values obtained to come up
	// with the perfect threshold value...
	nSum = 0 ;
	nCount = 0 ;
	for (j=0; j<nNumTotals; j++)
	{
		if (totals[j])
		{
			nSum += sums[j] ;
			nCount++ ;
		}
	}

	if (nCount)
		nSum /= nCount ;

	// Now, delete all the square centers except the black squares
	for (j=nNumTotals-1; j>=0; j--)
	{
		// Were the totals zero, i.e. a bigger object than what
		// we'd expect?
		if (SquareCenters[j].m_z<1)
		{
			SquareCenters.RemoveAt(j) ;
			SquarePixels.RemoveAt(j) ;
		}
		else
		{
			if (bGetBlackSquares)
			{
				// Is the average gray value above the mean?
				if (SquareCenters[j].m_nData > nSum)
				{
					SquareCenters.RemoveAt(j) ;
					SquarePixels.RemoveAt(j) ;
				}
			}
			else
			{
				// Is the average gray value below the mean?
				if (SquareCenters[j].m_nData < nSum)
				{
					SquareCenters.RemoveAt(j) ;
					SquarePixels.RemoveAt(j) ;
				}
			}
		}
	}

	// Now we have the perfect threshold value.  Do a proper
	// threhoshold
	squarergns.Copy(threshold) ;
	//threshold.Threshold ( this, nSum, 255 ) ;
	threshold.AdaptiveThreshold_SAT ( this, 8, 70 ) ;
	medianbuf.Median3x3 ( &threshold ) ;
	//medianbuf.Copy (threshold) ;
	// Clear the median buf's interior points
	for (j=0; j<medianbuf.m_size; j++)
	{
		v = squarergns.m_buf[j] ;
		vl = medianbuf.m_buf[j] ;
		if (bGetBlackSquares)
		{
			if (v>0 && v<nSum && vl>v)
			{
				medianbuf.m_buf[j] = v ;
			}
			if (v>0 && v>nSum)
				squarergns.m_buf[j] = 0 ;
		}
		else
		{
			if (v>0 && v>nSum && vl<v)
			{
				medianbuf.m_buf[j] = v ;
			}
			if (v>0 && v<nSum)
				squarergns.m_buf[j] = 0 ;
		}
	}

	//SquareMask.Copy (squarergns) ;

	//medianbuf.Dialate ( 2 ) ;
	thresholdrgn.Init ( medianbuf  ) ;

	nNumPts = medianbuf.GetMilanoCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + medianbuf.m_w*pts[i].y ;

	medianbuf.m_dHistogramLow = 25 ;
	medianbuf.m_dHistogramHigh = 255 - medianbuf.m_dHistogramLow ;
	//medianbuf.m_dHistogramHigh = nHigh ;
	//medianbuf.m_dHistogramLow = nLow ;

	findregions.Init (medianbuf) ;
	findregions.Clear () ;
	i = nRadius + medianbuf.m_w * nRadius ;
	for (y=nRadius; y<medianbuf.m_h-nRadius; y++)
	{
		for (x=nRadius; x<medianbuf.m_w-nRadius; x++)
		{
			if (squareoutlinesnoctr.m_buf[i])
			{
				if (medianbuf.IsMilanoCheckerIntersection ( offsets, x, y, nNumPts ) )
					findregions.SetAtFast (x,y, 255) ;
			}
			i++ ;
		}

		i+=nRadius + nRadius ;
	}

	findregions.Dialate(2) ;

	Copy (findregions) ;
	//Copy (medianbuf) ;
	findregions.FindRegions(Centers, 1, 1000) ;
	guesses.SetSize (Centers.GetSize()) ;

	// Now, add the centers as guesses for accurate locations
	for (i=0; i<Centers.GetSize(); i++)
		guesses[i].Set (Centers[i].x, Centers[i].y, 0 ) ;

	ReturnValues.Copy (guesses) ;
	image.GetCheckerIntersections ( guesses, ReturnValues, nRadius*2/3 ) ;
	//medianbuf.GetCheckerIntersections ( guesses, ReturnValues, nRadius ) ;
	//sobel.GetSobelCheckerIntersections ( guesses, ReturnValues, nRadius ) ;


	// As a test, we need to go through the return values and see 
	// how close they are to the guesses.  If none of the return values
	// are within range of the guesse
	//SquareCenters.SetSize(0) ;
					
	return true ;
}
*/

/*
BOOL CPixelBuffer::GetAllMilanoCheckerIntersections 
	( 
        CCoordArray  &ReturnValues
	  , int			 nThreshold
	  , int			 nRadius
	 )
{
	CPoint				 pts[1024] ;
	int					 offsets[1024] ;
	int					 totals[2048] ;
	int					 sums[2048] ;
	int					 nNumPts ;
	int				     nNumTotals ;
	CPixelBuffer		 sobel ;
	CPixelBuffer		 threshold ;
	CPixelBuffer		 normalize ;
	CPixelBuffer		 findcheckers ;
	CPixelBuffer		 thresholdrgn ;
	CPixelBuffer		 squareoutlines ;
	CPixelBuffer		 medianbuf ;
	CPixelBuffer		 findregions ;
	CPixelArray			 Centers ;
	CCoordArray			 RegionCenters ;
	CCoordArray			 RefineIntersections ;
	CCoordArray			 guesses ;
	CDibSection			 ds ;
	int					 i ;
	int					 j ;
	int					 x ;
	int					 y ;
	int					 v ;
	int					 nSum ;
	int					 nCount ;
	int					 *pHistogram ;
	int					 nHistogram[512] ;
	
	pHistogram = Histogram() ;
	memcpy (nHistogram, pHistogram, 256*sizeof(int)) ;
	delete pHistogram ;

	ds.Setup (NULL) ;
	ds.Init (*this) ;

	// Sort the histogram values
   	sobel.Sobel ( this ) ;
	threshold.Threshold (&sobel, 96, -1) ;
	threshold.Dialate (1) ;

	nNumTotals = thresholdrgn.FindRegions ( &threshold, totals, 2048 ) ;
	for (j=0; j<thresholdrgn.m_size; j++)
	{
		v = thresholdrgn.m_buf[j] ;
		if (v)
		{
			if (totals[v-1] < 2000)
				thresholdrgn.m_buf[j] = 0 ;
			else
				thresholdrgn.m_buf[j] = 255 ;
		}
	}

	// Now, replace the 0's with -1's and the 255's with 0's.
	thresholdrgn.ReplaceVal (0, -1) ;
	thresholdrgn.ReplaceVal (255, 0) ;

	// O.K.  Now classify the exisiting regions so we can isolate
	// the squares (somewhat).
	nNumTotals = threshold.FindRegions ( &thresholdrgn, totals, 2048 ) ;
	for (j=0; j<threshold.m_size; j++)
	{
		v = threshold.m_buf[j] ;
		if (v)
		{
			if (totals[v-1] > 3000)
				threshold.m_buf[j] = 0 ;
		}
	}

	// Alright.  Anywhere there is nonzero, use these values to come up
	// with a histogram!  Essentially, we'll sum the values from the 
	// original image to come up with a real average for the block.
	memset (sums, 0, sizeof(sums)) ;
	for (j=0; j<threshold.m_size; j++)
	{
		v = threshold.m_buf[j] ;
		if (v && (squareoutlines.m_buf[j]==0))
			sums[v-1] += this->m_buf[j] ;
	}
	// Now, calculate the average for the blocks
	for (j=0; j<nNumTotals; j++)
	{
		if (totals[j])
			sums[j] /= totals[j] ;
	}
	// Finally, replace the values in the threshold image with the
	// block averages
	for (j=0; j<threshold.m_size; j++)
	{
		v = threshold.m_buf[j] ;
		if (v)
			threshold.m_buf[j] = sums[v-1] ;
	}

	// Finally, sum the values obtained to come up
	// with the perfect threshold value...
	nSum = 0 ;
	nCount = 0 ;
	for (j=0; j<nNumTotals; j++)
	{
		if (totals[j])
		{
			nSum += sums[j] ;
			nCount++ ;
		}
	}

	if (nCount)
		nSum /= nCount ;

	// Now we have the perfect threshold value.  Do a proper
	// threhoshold
	threshold.Threshold ( this, nSum, 255 ) ;
	medianbuf.Median3x3 ( &threshold ) ;
	//medianbuf.Enlarge ( &medianbuf, 2 ) ;
	thresholdrgn.Init ( medianbuf  ) ;

	nRadius = 7 ;
	nNumPts = medianbuf.GetMilanoCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + medianbuf.m_w*pts[i].y ;

	medianbuf.m_dHistogramLow = 25 ;
	medianbuf.m_dHistogramHigh = 255 - medianbuf.m_dHistogramLow ;
	findregions.Init (medianbuf) ;
	findregions.Clear () ;
	i = nRadius + medianbuf.m_w * nRadius ;
	for (y=nRadius; y<medianbuf.m_h-nRadius; y++)
	{
		for (x=nRadius; x<medianbuf.m_w-nRadius; x++)
		{
			if (squareoutlines.m_buf[i])
			{
				if (medianbuf.IsMilanoCheckerIntersection ( offsets, x, y, nNumPts ) )
					findregions.SetAtFast (x,y, 255) ;
			}
			i++ ;
		}

		i+=nRadius + nRadius ;
	}

	findregions.FindRegions(Centers, 1, 100) ;
	guesses.SetSize (Centers.GetSize()) ;

	// Now, add the centers as guesses for accurate locations
	for (i=0; i<Centers.GetSize(); i++)
		guesses[i].Set (Centers[i].x, Centers[i].y, 0 ) ;

	medianbuf.GetCheckerIntersections ( guesses, ReturnValues, nRadius ) ;
	return true ;
}
*/


void LineDDA ( int x1,  int y1, int x2, int y2
             , void(*ddaproc)(int x, int y, const void *pParam)
             , const void *pParam )
{
	int incx = 0 ;
	int incy = 0 ;
	int dx = abs(x2-x1) ;
	int dy = abs(y2-y1) ;
    int bigy = (dy>dx) ? 1 : 0 ;
	int count = (dy>dx) ? dy : dx ;
	int err = (dy>dx) ? -dy/2 : -dx/2 ;
	int addval = (dy<dx) ? dy/2 : dx/2 ;
	int subval = count ;
	   
	if (dx)
		incx = dx>0 ? 1 : -1 ;
	if (dy)
		incy = dy>0 ? 1 : -1 ;
		
	while(count)
	{
	    ddaproc(x1,y1,pParam);
		err += addval ;
		if (bigy)
		   y1 += incy ;
		else
		   x1 += incx ;
		if (err>=0)
		{
		    err-=subval;
		    if (bigy)
		        x1 += incx ;
		    else
		        y1 += incy ;
		}
		count-- ;
	}
    ddaproc(x1,y1,pParam);
}	



BOOL CPixelBuffer::SetLine(CLine &Line, int v)
{
	m_nSetLineData = v ;
	LineDDA( (int)Line.m_first.m_x
		    ,(int)Line.m_first.m_y
			,(int)Line.m_second.m_x
			,(int)Line.m_second.m_y
			,SetLineProc
			,(const void*)this ) ;
	SetAt( (int)Line.m_second.m_x
		 , (int)Line.m_second.m_y
		 , v ) ;
	return true ;
}

BOOL CPixelBuffer::SetLine(CCoord &c1, CCoord &c2, int v)
{
	m_nSetLineData = v ;
	LineDDA ( (int)c1.m_x
		    , (int)c1.m_y
			, (int)c2.m_x
			, (int)c2.m_y
			, SetLineProc
			, (const void*)this ) ;
	SetAt( (int)c2.m_x
		 , (int)c2.m_y
		 , v ) ;
	return true ;
}



void CPixelBuffer::DrawCross(CCoord &c, int r, int v)
{
	int x, y ;

	x = (int)c.m_x ;
	y = (int)c.m_y ;

	SetLine (x-r, y, x+r, y, v) ;
	SetLine (x, y-r, x, y+r, v) ;
}

void CPixelBuffer::DrawCross(CPixel &c, int r, int v)
{
	int x, y ;

	x = c.x ;
	y = c.y ;

	SetLine (x-r, y, x+r, y, v) ;
	SetLine (x, y-r, x, y+r, v) ;
}


// We know we have checker intersections when we have a DL - LD - DL - LD
// pattern of light to dark transitions.
BOOL CPixelBuffer::GetCheckerIntersection ( CPixelBuffer *pSrc
										  , int x
										  , int y
										  , int nRadius
										  , int	nMinArcLen
										  , int	nMinRange
										    )
{
	CPoint      pts[1024] ;
	int			offsets[1024] ;
	int			nNumPts;
	int			i, sum ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return false;

	nNumPts = GetCircleCoords (0, 0, nRadius, pts) ;
	for (i=0; i<nNumPts; i++)
		offsets[i] = pts[i].x + m_w*pts[i].y ;

//	Clear();
	if (pSrc->ValidCheckerRun(x,y, offsets,nNumPts, &sum, nRadius, nMinRange, nMinArcLen, 2, true))
	{
		return true ;
	}
	else
		return false ;
}

BOOL CPixelBuffer::Radius3Count(CPixelBuffer *pSrc)
{
/*
	int wmr ;
	int *pbufsrc ;
	int *pbufdst ;
	int w4, h ;

	if (!Init(pSrc->m_w, pSrc->m_h))
		return false;

	pbufsrc = pSrc->m_buf ;
	pbufdst = m_buf + Index(3,3) ;
	wmr = m_w - 6;
	w4 = m_w * 4 ;
	h = m_h - 6 ;

	_asm
	{
		mov ecx, wmr
		mov ebx, w4
		mov edx, h
		mov esi, pbufsrc
		mov edi, pbufdst

Radius3Count_InnerLoop:
		xor eax, eax
		add eax, [esi]
		add eax, [esi+4]
		add eax, [esi+8]
		add eax, [esi+12]
		add eax, [esi+16]
		add eax, [esi+20]

		add esi, 24
		add eax, [esi]
		add esi, ebx
		add eax, [esi]
		add esi, ebx
		add eax, [esi]
		add esi, ebx
		add eax, [esi]
		add esi, ebx
		add eax, [esi]
		add esi, ebx
		add eax, [esi]
		add esi, ebx

		add eax, [esi]
		add eax, [esi-4]
		add eax, [esi-8]
		add eax, [esi-12]
		add eax, [esi-16]
		add eax, [esi-20]
		sub esi, 24

		add eax, [esi]
		sub esi, ebx
		add eax, [esi]
		sub esi, ebx
		add eax, [esi]
		sub esi, ebx
		add eax, [esi]
		sub esi, ebx
		add eax, [esi]
		sub esi, ebx
		add eax, [esi]
		sub esi, ebx

		mov [edi], eax
		add esi, 4
		add edi, 4
		loop Radius3Count_InnerLoop

		add esi, 24
		add edi, 24
		mov ecx, wmr
		dec edx
		jnz Radius3Count_InnerLoop
	}
*/
	return true ;
}

void CPixelBuffer::NumNeighborsFilter(CPixelBuffer *src, int nFilter, int nReplaceVal)
{
	int x, y, nIndex, nVal, nCnt ;

	if (!Init(src->m_w, src->m_h))
		return;
	Clear() ;

	for (y=1; y<m_h-1; y++)
	{
		for (x=1; x<m_w-1; x++)
		{
			nIndex = (x-1)+(y-1)*m_w ;

			nVal = src->m_buf[nIndex+m_w+1] ;
			if (nVal==0)
			{
				nCnt = 0 ;

				nCnt += src->m_buf[nIndex] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+1] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+2] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+m_w] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+m_w+2] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+m_w+m_w] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+m_w+m_w+1] ? 1 : 0 ;
				nCnt += src->m_buf[nIndex+m_w+m_w+2] ? 1 : 0 ;

				if (nCnt<8 && nCnt>=nFilter)
					m_buf[nIndex+m_w+2] = nReplaceVal ;
			}			
		}
	}
}

BOOL CPixelBuffer::And(CPixelBuffer *pbuf1, CPixelBuffer *pbuf2) 
{
	int i;
	unsigned char *ucbuf1, *ucbuf2, *ucbufr ;
	unsigned short *usbuf1, *usbuf2, *usbufr ;
	int *ibuf1, *ibuf2, *ibufr ;

	// Pointers must be valid
	if (pbuf1 && pbuf2 == false)
		return false ;

	// Step sizes must be equal
	if (pbuf1->m_nStepSize != pbuf2->m_nStepSize)
		return false ;

	if (!Init(pbuf1->m_w, pbuf1->m_h, pbuf1->m_nStepSize))
		return false ;

	switch (m_nStepSize)
	{
	case 1:
		{
			ucbuf1 = (unsigned char *)pbuf1->m_buf ;
			ucbuf2 = (unsigned char *)pbuf2->m_buf ;
			ucbufr = (unsigned char *)m_buf ;
			for (i=0; i<m_size; i++)
			{
				*ucbufr = *ucbuf1 & *ucbuf2 ;
				ucbuf1++ ;
				ucbuf2++ ;
				ucbufr++ ;
			}
		}
		break;

	case 2:
		{
			usbuf1 = (unsigned short *)pbuf1->m_buf ;
			usbuf2 = (unsigned short *)pbuf2->m_buf ;
			usbufr = (unsigned short *)m_buf ;
			for (i=0; i<m_size; i++)
			{
				*usbufr = *usbuf1 & *usbuf2 ;
				usbuf1++ ;
				usbuf2++ ;
				usbufr++ ;
			}
		}
		break;

	case 4:
		{
			ibuf1 = (int *)pbuf1->m_buf ;
			ibuf2 = (int *)pbuf2->m_buf ;
			ibufr = (int *)m_buf ;
			for (i=0; i<m_size; i++)
			{
				*ibufr = *ibuf1 & *ibuf2 ;
				ibuf1++ ;
				ibuf2++ ;
				ibufr++ ;
			}
		}
		break;
	default:
		// Unsupported step size
		return false ;
	}
	return true ;
}

int CPixelBuffer::ShowBuffer (int nZoom)
{
/*
	CPixelBufferDlg ShowDlg ;

	ShowDlg.m_nZoom = nZoom ;
	ShowDlg.m_pBuffer = this ;
	return ShowDlg.DoModal() ;*/
	return 0;
}

CRect CPixelBuffer::RegionRect ( int nRgnVal )
{
	CRect rval ;
	int	  i, x, y ;

	rval.top = 65536 ;
	rval.bottom = -65536 ;
	rval.left = 65536 ;
	rval.right = -65536 ;

	i = 0 ;
	for (y=0; y<m_h; y++)
	{
		for (x=0; x<m_w; x++, i++)
		{
			if (m_buf[i] == nRgnVal)
			{
				if (rval.top > y)
					rval.top = y ;
				if (rval.bottom < y)
					rval.bottom = y ;
				if (rval.left > x)
					rval.left = x ;
				if (rval.right < x)
					rval.right = x ;
			}
		}
	}

	return rval ;
}

int CPixelBuffer::RegionDistanceSq ( CCoord &testpt, int nRegion)
{
	int x = (int)testpt.m_x ;
	int y = (int)testpt.m_y ;

	return RegionDistanceSq ( x, y, nRegion ) ;
}

int CPixelBuffer::RegionDistanceSq ( int sx, int sy, int nRegion)
{
	int	  i, x, y, nNearest, nDistSq, dx, dy ;

	nNearest = 1000000000 ;
	if (nRegion==-1)
	{
		i = 0 ;
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++, i++)
			{
				if (m_buf[i])
				{
					dx = sx-x ;
					dy = sy-y ;
					nDistSq = dx*dx + dy*dy ;
					if (nNearest > nDistSq)
						nNearest = nDistSq ;
				}
			}
		}
	}
	else
	{
		i = 0 ;
		for (y=0; y<m_h; y++)
		{
			for (x=0; x<m_w; x++, i++)
			{
				if (m_buf[i]==nRegion)
				{
					dx = sx-x ;
					dy = sy-y ;
					nDistSq = dx*dx + dy*dy ;
					if (nNearest > nDistSq)
						nNearest = nDistSq ;
				}
			}
		}
	}

	return nNearest ;
}

int CPixelBuffer::RegionCenters ( CCoordArray &centers, int nTestTotal )
{
	int *pCounts ;
	CPoint *pPixels ;
	int i, v ;

	centers.SetSize (0) ;
	if (nTestTotal<=0 || nTestTotal>2048)
		return 0 ;

	pPixels = new CPoint [nTestTotal] ;
	pCounts = new int [nTestTotal] ;

	memset (pPixels, 0, sizeof(CPoint)*nTestTotal) ;
	memset (pCounts, 0, sizeof(int)*nTestTotal) ;

	for (i=0; i<m_size; i++)
	{

		v = m_buf[i] ;
		if (v)
		{
			pPixels[v-1] += CPoint(i%m_w, i/m_w) ;
			pCounts[v-1] ++ ;
		}
	}

	for (i=0; i<nTestTotal; i++)
	{
		if (pCounts[i])
		{
		    CCoord c ( (DOUBLE)pPixels[i].x / (DOUBLE)pCounts[i]
				     , (DOUBLE)pPixels[i].y / (DOUBLE)pCounts[i]
					 , (DOUBLE)0.0 ) ;
			centers.Add ( c ) ;
		}
	}

	delete pCounts ;
	delete pPixels ;

	return centers.GetSize() ;
}















#ifdef _dib_section_h
CPixelBufferDlg::CPixelBufferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPixelBufferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPixelBufferDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pBuffer = NULL ;
	m_nZoom = 1 ;
}


void CPixelBufferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPixelBufferDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPixelBufferDlg, CDialog)
	//{{AFX_MSG_MAP(CPixelBufferDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPixelBufferDlg message handlers

BOOL CPixelBufferDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_DibSection.Init(*m_pBuffer) ;
	m_DibSection.CopyToReds(m_pBuffer) ;
	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CPixelBufferDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	if (m_pBuffer)
	{
		m_DibSection.PaintTo(dc.GetSafeHdc(), 0, 0, m_nZoom) ;
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CPixelBufferDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

int CPixelBufferDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}
#endif

void CPixelBuffer::CopyStep(CPixelBuffer &pbuf, int nStepDist, int nStepStart)
{
	int nSize ;
	int numelem ;

	//ASSERT(pbuf.m_nStepSize==1);
	if (pbuf.m_w % nStepDist)
		return;
	if (!Init(pbuf.m_w/nStepDist, pbuf.m_h, 4))
		return;

	if (pbuf.m_nStepSize==1)
	{
		unsigned char *psrc = ((unsigned char *)pbuf.m_buf)+nStepStart ;
		unsigned long *pdst = (unsigned long *)m_buf ;
		numelem = m_w * m_h ;
		while (numelem)
		{
			// Make sure the element is zero'd out
			*pdst = 0 ;

			// Copy the low byte
			*pdst = *psrc ;
			psrc+=nStepDist ;
			pdst++ ;
			numelem-- ;
		}
	}
	else
	if (pbuf.m_nStepSize==4)
	{
		unsigned long *psrc = ((unsigned long *)pbuf.m_buf)+nStepStart ;
		unsigned long *pdst = (unsigned long *)m_buf ;
		numelem = m_w * m_h ;
		while (numelem)
		{
			// Make sure the element is zero'd out
			*pdst = 0 ;

			// Copy the low byte
			*pdst = *psrc ;
			psrc+=nStepDist ;
			pdst++ ;
			numelem-- ;
		}
	}

	/*
	nSize = m_size ;

	switch (pbuf.m_nStepSize)
	{
	case 1:
		{


			_asm
			{
				mov eax, 0 
				mov esi, psrc
				mov edi, pdst
				mov ecx, nSize
				mov ebx, nStepStart

anothercopystep1:
				mov al, [esi+ebx]
				mov [edi], eax
				
				add edi, 4
				add ebx, nStepDist

				loop anothercopystep1
			}
		}
		break;
	case 2:
		{
			unsigned short *psrc = (unsigned short *)pbuf.m_buf ;
			unsigned long *pdst = (unsigned long *)m_buf ;
			_asm
			{
				mov eax, 0 
				mov esi, psrc
				mov edi, pdst
				mov ecx, nSize
				mov ebx, nStepStart

anothercopystep2:
				mov ax, [esi+ebx*2]
				mov [edi], eax
				
				add edi, 4
				add ebx, nStepDist

				loop anothercopystep2
			}
		}
		break;
	case 4:
		{
			unsigned long *psrc = (unsigned long *)pbuf.m_buf ;
			unsigned long *pdst = (unsigned long *)m_buf ;
			_asm
			{
				mov eax, 0 
				mov esi, psrc
				mov edi, pdst
				mov ecx, nSize
				mov ebx, nStepStart

anothercopystep4:
				mov eax, [esi+ebx*4]
				mov [edi], eax
				
				add edi, 4
				add ebx, nStepDist

				loop anothercopystep4
			}
		}
		break;
	}
	*/
}

int CPixelBuffer::RegionSum ( CPixelBuffer &src, int nRgnVal )
{
	int nSum = 0 ;
	int i ;

	if (src.m_w != m_w || src.m_h != m_h)
		return -1 ;

	for (i=0; i<m_size; i++)
	{
		if (m_buf[i]==nRgnVal)
			nSum+=src.m_buf[i] ;
	}

	return nSum ;
}

void CPixelBuffer::StretchLevels( CPixelBuffer *pSrc, int nSrcLow, int nSrcHigh, int nDstLow, int nDstHigh)
{
    //ASSERT(pSrc);
    int i ;

    if (!pSrc)
        return ;

    if (!Init(pSrc->Width(), pSrc->Height(), pSrc->m_nStepSize))
        return;

	switch (m_nStepSize)
	{
	case 4:
		{
			int *pS, *pD ;
			int n ;

			pS = (int*)pSrc->GetBuffer() ;
			pD = (int*)GetBuffer() ;
			for (i=0; i<m_size; i++)
			{
				n = *pS ;
				if (n<nSrcLow)
					*pD = nDstLow ;
				else
				if (n>nSrcHigh)
					*pD = nDstHigh ;
				else
					*pD = (*pS - nSrcLow) * (nDstHigh - nDstLow) / (nSrcHigh - nSrcLow) ;
				pD++ ;
				pS++ ;
			}
		}
		break;

	case 2:
		{
			WORD *pS, *pD ;
			WORD n ;

			pS = (WORD*)pSrc->GetBuffer() ;
			pD = (WORD*)GetBuffer() ;
			for (i=0; i<m_size; i++)
			{
				n = *pS ;
				if (n<nSrcLow)
					*pD = nDstLow ;
				else
				if (n>nSrcHigh)
					*pD = nDstHigh ;
				else
					*pD = (*pS - nSrcLow) * (nDstHigh - nDstLow) / (nSrcHigh - nSrcLow) ;
				pD++ ;
				pS++ ;
			}
		}
		break;

	case 1:
	default:
		{
			unsigned char *pS, *pD ;
			unsigned char n ;

			pS = (unsigned char *)pSrc->GetBuffer() ;
			pD = (unsigned char *)GetBuffer() ;
			for (i=0; i<m_size; i++)
			{
				n = *pS ;
				if (n<nSrcLow)
					*pD = nDstLow ;
				else
				if (n>nSrcHigh)
					*pD = nDstHigh ;
				else
					*pD = (*pS - nSrcLow) * (nDstHigh - nDstLow) / (nSrcHigh - nSrcLow) ;
				pD++ ;
				pS++ ;
			}
		}
		break;
	}
}

/*
 static double[,] SumMatrix(double[,] matrix)
 {
  int n = matrix.GetLength(0);
  int m = matrix.GetLength(1);
  double[,] s = new double[n,m];
  double[,] ii = new double[n,m];

  s[0,0] = matrix[0,0];
  ii[0,0] = s[0,0];
  for (int x = 1; x < n; x++) 
  {
   s[x,0] = matrix[x,0];
   ii[x,0] = ii[x-1,0] + s[x,0]; 
  }
  for (int y = 1; y < m; y++) {
   ii[0,y] = s[0,y] = s[0,y-1] + matrix[0, y];
   
   for (int x = 1; x < n; x++) {
    s[x,y] = s[x,y-1] + matrix[x,y];
    ii[x,y] = ii[x-1,y] + s[x,y];
   }
  }
  return ii;
 }*/

void CPixelBuffer::SummedAreaTable(CPixelBuffer *matrix)
{
	// the *this is the same as ii
	int			 x, y, v ;
	CPixelBuffer s ;

	if (matrix->m_nStepSize != 4)
		return ;

	if (!s.Init(matrix->m_w, matrix->m_h, matrix->m_nStepSize))
		return ;

	if (!Init(matrix->m_w, matrix->m_h, matrix->m_nStepSize))
		return ;

	s.SetAtFast(0,0, matrix->GetAtFast(0,0)) ;
	SetAtFast(0,0, s.GetAtFast(0,0)) ;
	for (x=1; x<m_w; x++)
	{
		s.SetAtFast(x,0, matrix->GetAtFast(x,0)) ;
		SetAtFast(x, 0, GetAtFast(x-1,0) + s.GetAtFast(x,0)) ;
	}

	for (y=1; y<m_h; y++)
	{
		v = s.GetAtFast(0,y-1) + matrix->GetAtFast(0,y) ;
		SetAt(0,y,v) ;
		s.SetAt(0,y,v) ;

		for (x=1; x<m_w; x++)
		{
			s.SetAt(x,y, s.GetAt(x,y-1) + matrix->GetAt(x,y)) ;
			SetAt(x,y, GetAt(x-1,y) + s.GetAt(x,y)) ;
		}
	}
}

void CPixelBuffer::AdaptiveThreshold_SAT(CPixelBuffer *pSrc, int nThreshold, int s)
{
	CPixelBuffer intImg ;
	int i, j ;
	int x1, y1, x2, y2 ;
	int s2 = s/2 ;
	int count ;
	int sum ;
	static CPixelBuffer borderImg ;

	if (!Init(pSrc->m_w, pSrc->m_h, pSrc->m_nStepSize))
		return ;
	borderImg.Init (pSrc->m_w+s, pSrc->m_h+s, pSrc->m_nStepSize) ;
	borderImg.Copy (*pSrc, s2, s2) ;
	intImg.SummedAreaTable(&borderImg) ;

	for (i=0; i<m_w+s; i++)
	{
		for (j=0; j<m_h+s; j++)
		{
			x1 = i ;
			x2 = i+s ;

			y1 = j ;
			y2 = j+s ;

			count = (x2-x1)*(y2-y1) ;

			sum =    intImg.GetAt(x2,y2) 
				   - intImg.GetAt(x2,y1-1)
				   - intImg.GetAt(x1-1,y2)
				   + intImg.GetAt(x1-1,y1-1) ;

			if (pSrc->GetAt(i,j)*count <= (sum*(255-nThreshold)/255) )
				SetAt(i,j, 0) ;
			else
				SetAt(i,j, 255) ;
		}
	}
}

int CPixelBuffer::TrueMean()
{
	int nSum, i ;

	if (m_nStepSize!=4 || m_size<0)
		return 0 ;

	nSum = 0 ;
	for (i=0; i<m_size; i++)
	{
		nSum += m_buf[i] ;
	}

	return nSum / m_size ;
}

int CPixelBuffer::TrueMedian()
{
	int *medianbuf ;

	if (m_nStepSize!=4 || m_size<=0)
		return 0 ;

	medianbuf = new int [m_size] ;
	memcpy (medianbuf, m_buf, m_size*sizeof(int)) ;
	qsort (medianbuf, m_size, sizeof(int), compareMedian) ;

	return medianbuf[m_size/2] ;
}

CCoordPixelBuffer::CCoordPixelBuffer ( double xMin
									 , double xMax
									 , double xPrec
									 , double yMin
									 , double yMax
									 , double yPrec )
{
	dLeft  = xMin ;
	dRight  = xMax ;
	dTop    = yMin ;
	dBottom = yMax ;

	dXRange = xMax-xMin ;
	dYRange = yMax-yMin ;

	Init ( floor (fabs(dXRange)/xPrec + 1)
		 , floor (fabs(dYRange)/yPrec + 1)
		 , 4 ) ;
}

void CCoordPixelBuffer::SetPixel ( double x, double y, int nVal )
{
	int sx, sy ;

	sx = (int)((x - dLeft) * (double)(m_w-1) / dXRange + 0.5) ;
	sy = (int)((y - dTop)  * (double)(m_h-1) / dYRange + 0.5) ;

	SetAt(sx,sy, nVal) ;
}

int CCoordPixelBuffer::GetPixel ( double x, double y )
{
	int sx, sy ;

	sx = (int)((x - dLeft) * (double)(m_w-1) / dXRange + 0.5) ;
	sy = (int)((y - dTop)  * (double)(m_h-1) / dYRange + 0.5) ;

	return (int)GetAt(sx,sy) ;
}

BOOL CCoordPixelBuffer::CoordAt ( int sx, int sy, CCoord &rval )
{
	rval.m_y = (double)sx * dXRange / (double)(m_w-1) + dLeft ;
	rval.m_z = (double)sy * dYRange / (double)(m_h-1) + dTop ;
	rval.m_nData = GetAt(sx,sy) ;
	return true ;
}

void CPixelBuffer::DumpGpuData()
{
	// This dumps out all of the basic information
	// that was allocated prior to the call....
/*
    printf("m_bUseGpu       = %02X\n", m_bUseGpu) ;
    printf("m_GpuCodeMain   = %08X\n", m_GpuCodeMain) ;
    printf("m_GpuCodeThread = %08X\n", m_GpuCodeThread) ;
	printf("m_buf			= %08X\n", m_buf) ;

    for (j=0; j<12; j++) // The maximum number of GPUs we're using is 8, although we CAN do 12
    {
    	pArray[i]->m_pArmUniforms[j] = (long*)(ToArmMemory(nNumUnifPerThread * sizeof(long), nOffset)) ;
		pArray[i]->m_GpuUniforms[j]  = ToGpuMemory(pArray[i]->m_pArmUniforms[j]) ;
    }


	unsigned m_GpuUniforms[12] ;
	unsigned m_GpuData ;
	long	 *m_pArmCodeMain ;
	long	 *m_pArmCodeThread ;
	long	 *m_pArmUniforms[12] ;
	long 	 *m_pArmData ;

    unsigned m_pPeriphAddress ;
    volatile unsigned *m_pMapPeriphAddr ;
    unsigned m_nPeriphSize ;

    unsigned m_nNumGpus ;
    unsigned m_nNumUniforms ;	

			pArray[i]->m_pArmCodeMain = pArray[i]->m_pArmCodeThread = (long*) ToArmMemory (nMaxCodeUnifSize, nOffset) ;
			pArray[i]->m_GpuCodeMain  = pArray[i]->m_GpuCodeThread  = ToGpuMemory(pArray[i]->m_pArmCodeMain) ;

		    pArray[i]->m_pArmData = (long*) ToArmMemory ( pArray[i]->m_size*pArray[i]->m_nStepSize, nOffset ) ;
		    pArray[i]->m_GpuData  = ToGpuMemory ( pArray[i]->m_pArmData ) ;

			pArray[i]->m_pPeriphAddress = m_pPeriphAddress ;
			pArray[i]->m_pMapPeriphAddr = m_pMapPeriphAddr ;
			pArray[i]->m_nPeriphSize    = m_nPeriphSize ;
*/
}
