#include "Main.h"
#include "assert.h"
#include <byteswap.h>
#include "PixelBuffer_linux.h"
#include <arm_neon.h>

#define DONORMAL 

unsigned Microseconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

class CGpuBase
{
public:
    CGpuBase(long nReqMemSize);
	CGpuBase(long nCodeUniformAreaSize, long nNumUnifPerThread, CPixelBuffer **pBufArray, long nNumBuffers) ;
    ~CGpuBase();
    int GetBase();
    int DirectExecute ( long *pCode, long nCodeSize, long *pThreadCode, long nThreadCodeSize, long nNumReps, long nNumGpus, CPixelBuffer *pbuf=NULL ) ;
    void *   ToArmMemory(long nDesiredSize, long &nOffsetOutput) ;
    unsigned ToGpuMemory(void *) ;

    int      m_hMailbox ;
    unsigned m_hGpuMemory ;
    unsigned m_pGpuMemory ;
    void *   m_pArmMemory ;
    unsigned m_nSize ;

    unsigned m_nMemFlag ;
    unsigned m_nMemMap ;
    unsigned m_pPeriphAddress ;
    volatile unsigned *m_pMapPeriphAddr ;
    unsigned m_nPeriphSize ;
    bool m_bQpuEnabled ;
};


void * CGpuBase::ToArmMemory(long nDesiredSize, long &nOffsetOutput)
{
    void *rval = (((unsigned char *)m_pArmMemory)+nOffsetOutput) ;
    nOffsetOutput += nDesiredSize ;
    return rval ;
}

unsigned CGpuBase::ToGpuMemory (void *pSrc)
{
    return m_pGpuMemory + ( ((unsigned char *)pSrc)-((unsigned char *)m_pArmMemory)) ;
}

CGpuBase::CGpuBase ( long nMaxCodeUnifSize, long nNumUnifPerThread, CPixelBuffer **pArray, long nNumElem)
{
	// When this is called, the destructor for the pixel buffer is
    // called, and then the GPU pointers are added.  When the pixel
    // buffer goes out of scope, the pointers do NOT get deleted.
    // Rather, the GPU base will do the destruction when it goes
	// out of scope.
    long nTotalSize = 0;
	long i, j ;
    long nOffset = 0 ;

    m_hMailbox = 0 ;
    m_hGpuMemory = 0 ;
    m_pGpuMemory = 0 ;
    m_pArmMemory = 0 ;
    m_nSize = 0 ;
    m_nMemFlag = 0 ;
    m_nMemMap = 0 ;
    m_pPeriphAddress = 0 ;
    m_nPeriphSize = 0 ;
    m_pMapPeriphAddr = 0 ;
    m_bQpuEnabled = false ;


    for (i=0; i<nNumElem; i++)
	{
		printf("CGpuBase requested code size = %d\n", nMaxCodeUnifSize) ;
        printf("    nNumUnifPerThread = %d\n", nNumUnifPerThread) ;
		printf("    requested pixelbuf(%d,%d,%d) size = %d\n", pArray[i]->m_w, pArray[i]->m_h, pArray[i]->m_nStepSize, pArray[i]->m_size*pArray[i]->m_nStepSize) ;
		nTotalSize += (pArray[i]->m_size*pArray[i]->m_nStepSize + nMaxCodeUnifSize + nNumUnifPerThread*12*sizeof(long) ) ;
	}
	m_nSize = nTotalSize ;

	GetBase() ;

    m_hMailbox = mbox_open() ;             		// OPEN VCIO DRIVER HANDLE
    if (qpu_enable(m_hMailbox, 1)) 
        m_bQpuEnabled = false;
    else
        m_bQpuEnabled = true;

    if (m_bQpuEnabled)
    {
        m_hGpuMemory = mem_alloc( m_hMailbox, m_nSize, 4096, GPU_MEM_FLG);
        if (!m_hGpuMemory) 
        {
            qpu_enable(m_hMailbox, 0) ;
            m_bQpuEnabled = false ;
            return;
        }

        m_pGpuMemory = mem_lock( m_hMailbox, m_hGpuMemory );
        m_pArmMemory = mapmem ( BUS_TO_PHYS(m_pGpuMemory+m_nMemMap), m_nSize ) ;
        m_pMapPeriphAddr = (volatile unsigned *)mapmem(m_pPeriphAddress, m_nPeriphSize) ;

		// Now we figure out the pixel buffer memory locations
		// and delete the memory of the pixel buffer.  From this
		// point on, the memory of the pixel buffer belongs to
		// the GPU.  It is IMPORTANT not to allow the objects to
		// go out of scope.
		for (i=0; i<nNumElem; i++)
		{
			if (pArray[i]->m_buf)
				delete pArray[i]->m_buf ;
			pArray[i]->m_buf = NULL ;
			pArray[i]->m_bUseGpu = true ;

		    for (j=0; j<12; j++) // The maximum number of GPUs we're using is 8, although we CAN do 12
		    {
		    	pArray[i]->m_pArmUniforms[j] = (long*)(ToArmMemory(nNumUnifPerThread * sizeof(long), nOffset)) ;
				pArray[i]->m_GpuUniforms[j]  = ToGpuMemory(pArray[i]->m_pArmUniforms[j]) ;
		    }

			pArray[i]->m_pArmCodeMain = pArray[i]->m_pArmCodeThread = (long*) ToArmMemory (nMaxCodeUnifSize, nOffset) ;
			pArray[i]->m_GpuCodeMain  = pArray[i]->m_GpuCodeThread  = ToGpuMemory(pArray[i]->m_pArmCodeMain) ;

		    pArray[i]->m_pArmData = (long*) ToArmMemory ( pArray[i]->m_size*pArray[i]->m_nStepSize, nOffset ) ;
		    pArray[i]->m_GpuData  = ToGpuMemory ( pArray[i]->m_pArmData ) ;

			pArray[i]->m_pPeriphAddress = m_pPeriphAddress ;
			pArray[i]->m_pMapPeriphAddr = m_pMapPeriphAddr ;
			pArray[i]->m_nPeriphSize    = m_nPeriphSize ;
		}
    }
}

CGpuBase::CGpuBase(long nReqMemSize)
{
    m_hMailbox = 0 ;
    m_hGpuMemory = 0 ;
    m_pGpuMemory = 0 ;
    m_pArmMemory = 0 ;
    m_nSize = nReqMemSize ;
    m_nMemFlag = 0 ;
    m_nMemMap = 0 ;
    m_pPeriphAddress = 0 ;
    m_nPeriphSize = 0 ;
    m_pMapPeriphAddr = 0 ;
    GetBase();

    m_hMailbox = mbox_open() ;             		// OPEN VCIO DRIVER HANDLE
    if (qpu_enable(m_hMailbox, 1)) 
        m_bQpuEnabled = false;
    else
        m_bQpuEnabled = true;

    if (m_bQpuEnabled)
    {
        m_hGpuMemory = mem_alloc( m_hMailbox, m_nSize, 4096, GPU_MEM_FLG);
        if (!m_hGpuMemory) 
        {
            qpu_enable(m_hMailbox, 0) ;
            m_bQpuEnabled = false ;
            return;
        }
        m_pGpuMemory = mem_lock( m_hMailbox, m_hGpuMemory );
        m_pArmMemory = mapmem ( BUS_TO_PHYS(m_pGpuMemory+m_nMemMap), m_nSize ) ;
        m_pMapPeriphAddr = (volatile unsigned *)mapmem(m_pPeriphAddress, m_nPeriphSize) ;
    }
}

int CGpuBase::GetBase()
{
    void *handle;
    m_nMemFlag = GPU_FFT_USE_VC4_L2_CACHE? 0xC : 0x4;
    m_nMemMap = GPU_FFT_USE_VC4_L2_CACHE? 0x0 : 0x20000000; // Pi 1 only
    m_pPeriphAddress = 0x20000000;
    m_nPeriphSize = 0x01000000;
    m_pMapPeriphAddr = 0x00000000 ;

    unsigned (*bcm_host_get_sdram_address)     (void);
    unsigned (*bcm_host_get_peripheral_address)(void);
    unsigned (*bcm_host_get_peripheral_size)   (void);

    handle = dlopen("libbcm_host.so", RTLD_LAZY);
    if (!handle) return -9;

    *(void **) (&bcm_host_get_sdram_address)      = dlsym(handle, "bcm_host_get_sdram_address");
    *(void **) (&bcm_host_get_peripheral_address) = dlsym(handle, "bcm_host_get_peripheral_address");
    *(void **) (&bcm_host_get_peripheral_size)    = dlsym(handle, "bcm_host_get_peripheral_size");

    if (bcm_host_get_sdram_address && bcm_host_get_sdram_address()!=0x40000000) 
    {   
        // Pi 2?
        m_nMemFlag = 0x4; // ARM cannot see VC4 L2 on Pi 2
        m_nMemMap  = 0x0;
    }

    if (bcm_host_get_peripheral_address) 
	m_pPeriphAddress = bcm_host_get_peripheral_address();
    if (bcm_host_get_peripheral_size)    
	m_nPeriphSize    = bcm_host_get_peripheral_size();
    dlclose(handle);
}

CGpuBase::~CGpuBase()
{
    if (m_bQpuEnabled)
    {
        unmapmem(m_pArmMemory, m_nSize);
        mem_unlock(m_hMailbox, m_hGpuMemory);
        mem_free(m_hMailbox, m_hGpuMemory);
        qpu_enable(m_hMailbox, 0);
    }
    else
        printf("   qpu not enabled\n") ;
    mbox_close(m_hMailbox) ;
}

long byterev (long input)
{
    long output = __bswap_32(input) ;
    return output ;
}

int CGpuBase::DirectExecute( long *pMainCode
                           , long nMainCodeSize
						   , long *pThreadCode
						   , long nThreadCodeSize
						   , long nNumReps
						   , long nNumGpus 
						   , CPixelBuffer *pbuf )
{
    return 0 ;
}

int main(int argc, char *argv[]) 
{
    CPixelBuffer pbufgreen, pbufbordered, pbufdialated, pnormbordered, pnormdialated, pnormsub, pbufsub;
	CPixelBuffer newdepth ;

    long i, j, k, nNumGpus ;
    long rval ;
	unsigned dwStart ;
	unsigned dwStop ;

	// Initialize the base experimental image
	pbufgreen.Init(320,240,1) ;

	// Initialize the various stages of the conversion algorithm
	// to use GPU memory.
	pbufbordered.Init(320+8,240+8,1) ;
	pbufdialated.Init(320+8,240+8,1) ;
	pnormbordered.Init(320+8,240+8,1) ;
	pnormsub.Init(320+8,240+8,1) ;
	pbufsub.Init(320+8,240+8,1) ;
	newdepth.Init(320+8,240+8,4) ;



	// Initialize a buffer for testing
    pbufgreen.SetAt(12,3,0xAA) ; pbufgreen.SetAt(13,3,0xAA) ; pbufgreen.SetAt(14,3,0xAA) ;
    pbufgreen.SetAt(12,4,0xAA) ; pbufgreen.SetAt(13,4,0xEE) ; pbufgreen.SetAt(14,4,0xAA) ;
    pbufgreen.SetAt(12,5,0xAA) ; pbufgreen.SetAt(13,5,0xAA) ; pbufgreen.SetAt(14,5,0xAA) ;
    for (i=0, j=0x11; i<15; i++,j+=0x11) 
        pbufgreen.SetAt(14+i,8+i*4,j) ;
    for (i=0; i<64; i++)
        pbufgreen.SetAt(55, i, i+1);


	for (i=0; i<240; i++)
	{
		pbufgreen.SetAt(i,i,i+1) ;
		pbufgreen.SetAt(319-i,i,i+1) ;
	}

	for (j=0; j<240; j++)
	{
		for (i=0; i<256; i++)
		{
			pbufgreen.SetAt(i,j,i) ;
		}
	}



	// Set up the GPU programming interface and memory	
	CPixelBuffer *pArray[20] ;
	pArray[0] = &pbufbordered ;
	pArray[1] = &pbufdialated ;
    pArray[2] = &pbufsub ;
	pArray[3] = &newdepth ;


	CGpuBase Gpu(16384, 16, pArray, 4) ;

	pbufbordered.Clear(0x00000000) ;
	pbufdialated.Clear(0xAAAAAAAA) ;

	// Start the conversion algorithm!  Each algorithm
	// loads gpu code on the first iteration by comparing
	// the thread and main code pointers for initialization.
	// If they are equal, this is the first execution, and
	// code should be copied.
	rval = pbufbordered.CopyBorderGpu (pbufgreen, 4, 4) ;

	printf("CopyBorderGpu rval = %d\n", rval) ;

	nNumGpus = 1 ;
    if (argc==2)
	    nNumGpus = atoi(argv[1]) ;

	printf("\n\nAccelerated dialate\n") ;
	dwStart = Microseconds() ;
	for (i=0; i<426; i++)
	{
		pbufdialated.MaxDialateGpu (pbufbordered, pbufbordered, 4, 4, 1, nNumGpus) ;
		//pbufsub.SubFloorGpu ( pbufdialated, pbufbordered, 4, 4, nNumGpus) ;
	}
	dwStop = Microseconds() ;
	printf("Completed in %d uS\n\n", dwStop-dwStart) ;

	pbufgreen.ToDisk ( "PGreen.txt", false ) ;
	pbufdialated.ToDisk ( "PDialated.txt", false ) ;
	pbufsub.ToDisk ( "PSubGpu.txt", false) ;

	pbufbordered.ToDisk ( "PBordered.txt", false ) ;



	volatile unsigned long pbuf1[19200] ;
	volatile unsigned long pbuf2[76800] ;

	for (i=0; i<76800; i++)
		((unsigned char *)pbuf1)[i] = (i & 255) ;

	rval = pbufbordered.CopyBorderGpu (pbufgreen, 4, 4) ;
	printf("\n\nNew Depth\n") ;
	dwStart = Microseconds() ;

	for (j=0; j<426; j++)
	{
 		for (i=0,k=0; i<19200; i++, k+=4)
 		{
			pbuf2[k] = pbuf1[i] & 255 ;
			pbuf2[k+1] = (pbuf1[i]>>8) & 255 ;
			pbuf2[k+2] = (pbuf1[i]>>16) & 255 ;
			pbuf2[k+3] = (pbuf1[i]>>24) & 255 ;
		}
	}

/*
	for (i=0; i<426; i++)
	{
		if (newdepth.CopyNewDepthGpu (pbufbordered, 4, 4, 4, nNumGpus)==false)
			printf("newdepth failed\n") ;
	}
*/
	dwStop = Microseconds() ;
    for (i=0; i<16; i++)
		printf("0x%08X\n", pbuf1[i]) ;
    for (i=0; i<16; i++)
		printf("0x%08X\n", pbuf2[i]) ;


	printf("Completed in %d uS\n\n", dwStop-dwStart) ;

	newdepth.ToDisk("PNewDepth.txt", false) ;

    return 0 ;
}
	