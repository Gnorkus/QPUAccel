#ifndef pixel_buffer_h
#define pixel_buffer_h

#define GPU_MEM_FLG     0xC
#define GPU_MEM_MAP     0x0
#define NUM_QPUS        1
#define MAX_CODE_SIZE   8192

#define V3D_L2CACTL (0xC00020>>2)
#define V3D_SLCACTL (0xC00024>>2)
#define V3D_SRQPC   (0xC00430>>2)
#define V3D_SRQUA   (0xC00434>>2)
#define V3D_SRQCS   (0xC0043c>>2)
#define V3D_DBCFG   (0xC00e00>>2)
#define V3D_DBQITE  (0xC00e2c>>2)
#define V3D_DBQITC  (0xC00e30>>2)
#define V3D_SRQUL   (0xC00438>>2)

#define BUS_TO_PHYS(x) ((x)&~0xC0000000)
#define GPU_FFT_USE_VC4_L2_CACHE 1 // Pi 1 only: cached=1; direct=0


// Pixel Buffer for EnvisicCalibrationUtility
#include "afxtempl.h"

//#include "Geometry_linux.h"
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

//#include "DibSection.h"
//#include "Resource.h"

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 180.0/PI
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD PI/180.0
#endif


#define UNCLASSIFIED 0
#define OUTSIDERUN   1
#define INSIDERUN    2
#define SURFACERUN   4

#define PADDING_SIZE   0
#define MAXPEAKRADIUS  15

typedef  int (*PIXELCOMPAREFUNC)(const void *,const void *) ;

#ifndef FLIP_Y
#define FLIP_Y   0x0001
#define FLIP_X   0x0002
#endif

//extern bool bCanDeletePixel[];

class CPixel
{
public:
    CPixel() {x=y=n=0; v=0;}
    CPixel(int nx, int ny, int nv) {x=nx;y=ny;v=nv;n=0;}
    CPixel(int nx, int ny, int nv, int nn) {x=nx;y=ny;v=nv;n=nn;}
    CPixel(CPixel &copy) ;
    CPixel &operator=(const CPixel &copy) ;
    int DistSqr(CPixel &second) ;
    void Set(int x, int y, int v, int n) ;
    void Add(int x, int y, int v, int n) ;

    int x ;
    int y ;
    int v ;
    int n ;
};

typedef CArray<CPixel, CPixel&> CRootPixelArray  ;

class CPixelArray : public CRootPixelArray
{
public:
    void ReverseDirection() ;
    CPixel &First() ;
    CPixel &Last() ;
    CPixelArray &operator=(const CPixelArray&) ;
	void CopyPtr ( CPixel *pPixel, int nNumPixels ) ;
    void SetValues(int nNewVal) ;
    int Width() ;
    int Height() ;
    CCoord Center() ;
	CRect RegionRect() ;
	DOUBLE Nearest ( CCoord &c ) ;
};

typedef CArray<CPixelArray, CPixelArray&> CPixelChains ;
typedef CArray<CPoint, CPoint&>         CPointArray ;

void LineDDA(int x1, int y1, int x2, int y2, void(*ddaproc)(int x, int y, const void* lpdata), const void*pParam);

typedef struct tagFindRegionStackVal
{
    int x ;
    int y ;
    int rgnnum ;
    int whichneighbor ;
    int stackpos ;
} FINDREGIONSTACKVAL, *PFINDREGIONSTACKVAL ;

class CPixelBuffer
{
public:
    CPixelBuffer(int nStepSize=sizeof(int)) ;
    ~CPixelBuffer() ;

    int ShowBuffer(int nZoom=1) ;
	void FillEmptyMax(CPixelBuffer *pbuf);
	int MedianFromHistogram() ;
	void Posterize ( CPixelBuffer *pbuf, int nNumGradations, int nMin, int nMax) ;
    void FindInsideOutside (  CPixelBuffer *pbuf
                            , CBaseLongFrameArray &findruns
                            , int nInsideIdentifier
                            , int threshold
                            , int minrange
                            , int radius ) ;
    void AddToPixelChain( int x, int y, int v, CPixelArray &ChainBuf );
    void FindPixelChains(CPixelChains &Chains, bool bSort=false, bool bReplot=false, bool bContiguousChain=true);
    void SortPixelChains(CPixelChains &Chains) ;
    void SetAt(CPixel &) ;
    void GetPixelArray (CPixelArray &Arr, CPixelBuffer *pMask=NULL) ;
    void SortPixelArray (CPixelArray &Arr) ;
    int ClassifyPixel(CPixel &pix);
    void ClassifyPixelChains(CPixelChains &Chains) ;
    void PlotPixelChains(CPixelChains &Chains) ;
    void PlotSprigRoots(CPixelBuffer *PixelBuffer) ;
    void RemoveSprigRoots() ;
	CRect RegionRect(int nRgnVal) ;
	int RegionSum ( CPixelBuffer &src, int nRgnVal ) ;
	int RegionDistanceSq(CCoord &testpt, int nRegion=-1) ;
	int RegionDistanceSq(int x, int y, int nRegion=-1) ;
	int RegionCenters ( CCoordArray &centers, int nTestTotal ) ;
    void AddSprigRootsToChains(CPixelChains &Chains) ;
    void NumNeighborsFilter(CPixelBuffer *, int nFilter, int nReplaceVal=255) ;
    CRect GetBoundingRect(int pixelval) ;
    void FindRegions(CPixelChains &Chains, int nMinAmount=-1, int nMaxAmount=-1);
    void FindRegions(CPixelArray &Chains, int nMinAmount, int nMaxAmount);
    void FindCheckerCorners(CPixelBuffer *pbuf, int radius=4) ;
    int  Lowest(int nIgnore=0x80000000) ;
    bool IsEqual(CPixelBuffer &pbuf) ;

    void FindMaskPeaks(CPixelBuffer *pbuf, CPixelBuffer *pmask, int radius, int threshold, int minrange, CPixelBuffer *pthreshbuf=NULL) ;
    void FindPeaks(CPixelBuffer *pbuf, int radius, int threshold, int minrange, CPixelBuffer *pthreshbuf) ;
	void FindPeaks(CPixelBuffer *pbuf, int radius, int threshold, int minrange, int nNumSucesses, int replval) ;
    bool GetPixelRun(int *rvals, int x, int y, int xdir, int ydir, int length, int *pNonZeroCount=NULL);
    bool ValidPeakRun(int *rvals, int peakval, int length);
	bool ValidPeakRun(int *rvals, int *rantivals, int peakval, int nRadius) ;
    bool ValidPeakRun (int nStartIndex, int nRadius, int nThreshold, int nRange, int *pImage, int *pRunIndeces );

    void Reset() ;
    void RemoveBigNoise(CPixelBuffer *, int nMinLength) ;
    void RemoveSmallNoise(CPixelBuffer *) ;
    void AverageFill(CPixelBuffer *pbuf, int nMaskWidth, int nMaskHeight) ;
    void DetectCrossIntersects(CPixelBuffer *pbuf, int radius=5) ;
    void GreyDialate(CPixelBuffer *pSrc, int nNumReps=1) ;
    void GreyErode(CPixelBuffer *pSrc, int nNumReps=1) ;
    void GreyOpen(CPixelBuffer *pSrc, int nNumReps=1) ;
    void GreyClose(CPixelBuffer *pSrc, int nNumReps=1) ;
    void TopHatPeak(CPixelBuffer *pSrc, bool bClip=false, int nNumReps=1) ;
    void TopHatWell(CPixelBuffer *pSrc, bool bClip=false, int nNumReps=1) ;
    void LaplacianOfGaussian9x9(CPixelBuffer *pSrc) ;

    void FindBorders(CPixelBuffer *pbuf, int nFillPixelVal, int nBkgndPixelVal=0) ;
    void ConvertBayer(CPixelBuffer *pbuf) ;
    void ConvertBayerColor(CPixelBuffer *pbuf,bool bHiByte=true) ;
    void ConvertBayer(unsigned char *pbuf, int w, int h) ;
    void Gaussian7x7Kernel(CPixelBuffer *pbuf, int x, int y) ;
    void Gaussian7x7(CPixelBuffer *pbuf) ;
    void Gaussian3x3(CPixelBuffer *pbuf) ;
    void GaussianPrecalculated(CPixelBuffer *pbuf) ;
    int FloodFillCross(int x, int y, int v) ;
    int Flood(int x, int y, int compval, int replval) ;
    void CalculateGaussianMask(int maskw, int maskh, DOUBLE sigma, DOUBLE scale) ;
    int Index(CCoord &c) ;
    void Dialate(/*CPixelBuffer *pbuf, */int NumReps=1) ;
    void Erode(CPixelBuffer *pbuf, int NumReps=1) ;
    int Index(int x, int y) ;
	int  TrueMedian() ;
	int  TrueMean() ;
	void MedianRadius(CPixelBuffer *pbuf, int nRadius) ;
    void Median3x3(CPixelBuffer *pbuf, long nSearchVal) ;
    void Peak3x3(CPixelBuffer *pbuf, long nSearchVal) ;
    void Peak3x3(CPixelBuffer *pbuf) ;
    void Median3x3(CPixelBuffer *pbuf) ;
    void Median5x5(CPixelBuffer *pbuf) ;
    void Median7x7(CPixelBuffer *pbuf) ;
    void Median11x11(CPixelBuffer *pbuf) ;
    void Sobel(CPixelBuffer *pbuf) ;
    void Sobel5x5(CPixelBuffer *pbuf, CPixelBuffer *pmask=NULL, int nType=0) ;
    void Prewitt(CPixelBuffer *pbuf) ;
    void Prewitt5x5(CPixelBuffer *pbuf) ;
    void Kirsch(CPixelBuffer *pbuf) ;
    void Dimple(CPixelBuffer *pbuf, int nMinRange = 50, int nMinMatch=6) ;
    void DiscreteLaplacian(CPixelBuffer *pbuf, bool bInv=true) ;
    void Threshold(CPixelBuffer *pbuf, int threshval=0, int replaceval=0) ;
    void ThresholdMask(CPixelBuffer *pbuf, CPixelBuffer *pmask, int threshval=0, int replaceval=0) ;
    void Invert(CPixelBuffer *pbuf, int maxval) ;
    void Thin(CPixelBuffer *pbuf) ;
    void ZeroCrossing(CPixelBuffer *pbuf) ;
    void DiffDest(unsigned char *dest) ;
    void Diff(CPixelBuffer *pbuf, bool bClip=false, int nClipLow=0, int nClipHigh=255) ;
    void Sum(CPixelBuffer *pbuf) ;
    int  Sum() ;
    void Div(int divval) ;
    void Or(CPixelBuffer *pbuf) ;
    void Mult(int multval) ;
    void Sub(int subval) ;
	void Sub(CPixelBuffer *pbuf);
    void Add(int addval) ;
	void Add(CPixelBuffer *pbuf);
    void SubFloor(CPixelBuffer *pbuf) ;
	void MinThreshold(int threshval=0, int replaceval=0);
	void MaxDialateGpu(CPixelBuffer &pbuf, CPixelBuffer &pbuf2, int nBorderW, int nBorderH, int nNumReps=1, long nNumGpus=1) ;
	void SubFloorGpu(CPixelBuffer &src1, CPixelBuffer &src2, int nBorderW, int nBorderH, long nNumGpus=1) ;
	void MaxDialate(int nNumReps=1) ;
	int MaxNeighborFast(int index) ;
	int MaxNeighbor(int x, int y) ;
    bool And(CPixelBuffer *pbuf1, CPixelBuffer *pbuf2) ;
    bool Crop(int emptyval, int nBorderSize=0) ;
    int  GetMax() ;  
    int  GetMin() ;  
    CSize GetSize() ;
    void Zero(int left, int top, int right, int bottom) ;
    void Flip(CPixelBuffer *pBuf, int FlipMask) ;
    void Rotate(CPixelBuffer *pBuf, int RotationAmount);
    void Normalize(CPixelBuffer *pbuf, int minrange=0) ;
    void Normalize(CPixelBuffer *pbuf, int minrange, int maxrange) ;
    void MaskDistance(CPixelBuffer *pbuf, CPixelBuffer *pmaskbuf, int maxdist) ;
    void LocalContrastStretch(CPixelBuffer *pbuf, DOUBLE scale, int nRadius) ;

	int InitRadiusSearch (int r) ;
    bool Init(CPixelBuffer &src) ;
    bool Init(int width, int height, int nStepSize=sizeof(int)) ;
    bool Init(CSize sz, int nStepSize=sizeof(int)) ;
    void Enlarge(CPixelBuffer *src, int mult) ;
	void Reduce(CPixelBuffer *src, int mult) ;
	long CopyBorder(CPixelBuffer &pSrc, int nBorderW, int nBorderH) ;
	long CopyBorderGpu(CPixelBuffer &pSrc, int nBorderW, int nBorderH) ;
    void Copy(unsigned char *, int width=-1, int height=-1, int nStepSize=1) ;
    void Copy(CPixelBuffer &pbuf, int l=-1, int t=-1, int r=-1, int b=-1, bool bIgnoreCheck=false) ;
    void Copy(CPixelBuffer &source, CPixelBuffer &mask, bool bInvertMask=false) ;
    void CopyNewDepth(CPixelBuffer &source, int newdepth) ;
	bool CopyNewDepthGpu(CPixelBuffer &source, long nBorderW, long nBorderH, int newdepth, long nNumGpus) ;
	void CopyStep(CPixelBuffer &pbuf, int nStepSize, int nStepStart) ;
#ifdef _dib_section_h
	bool CopyDib(CDibSection &source, int mask) ;
#endif

    void Clear(int nClearVal=0);
    void CopyResultToBuf();
    int *GetBuffer();
    int  GetBufferSize() ;
    int  *GetResult();

	int  IsInBounds(int x, int y) ;
    int  GetRowIndex(int y) ;
    int  GetAt(int x, int y) ;
    int  GetAt(DOUBLE *pt) ;
    int  GetAt(CCoord p) ;
    void SetAt(CCoord &c, int v) ;
    int  GetAt(CPoint p) ;
    int  GetAtFast(int x, int y) ;
    int  GetAtFast(CPoint p) ;
    int  GetAtFastest(int x, int RowIndex) ;
    void SetAt(int x, int y, int v) ;
    void SetAtFast(int x, int y, int v) ;
    void SetAtFast(CPoint p, int v) ;
    void SetAtFastest(int x, int RowIndex, int v) ;

    long ToDisk(const char *filename, bool FormatForExcel=false);
/*
    void ToDiskVrml(LPCTSTR filename, int left=0, int top=0, int right=0, int bottom=0);
    bool FromDisk(LPCTSTR filename);
    bool SerializeOut (CFile &) ;
    bool SerializeIn (CFile &) ;
*/


    int  Width() {return m_w;}
    int  Height() {return m_h;}

    bool IsZero() ;

    void ClearRegions(CPixelBuffer *regionbuf, int allowablevalue=-1) ;
    int  FindRegions(CPixelBuffer *regionbuf, int *pRegionTotals, int maxtotalelems) ;
    void CheckRegionNeighbor(PFINDREGIONSTACKVAL pstack, int &stackpos, int regionval);
    void ReplaceVal(int searchval, int replaceval);
    bool RegionIsInside(int searchrgn, int boundingrgn) ;
    int  BoundingRegion(int x, int y) ;
    int  CountNonZero() ;
    int  CountZero() ;
    bool Trim(int min, int max, int minreplaceval=0, int maxreplaceval=255) ;
    bool SetLine(int x1, int y1, int x2, int y2, int v) ;
    void Circle(int xCenter, int yCenter, int radius, int v);

    static void SetLineProc(int x, int y, const void* lpdata) ;
    int  GetPointLine(int x1, int y1, int x2, int y2, CPointArray &pa) ;
    int  GetPointLine(int x1, int y1, int x2, int y2, CDWordArray &pa) ;
    static void GetPointLineCoordProc(int x, int y, const void* lpdata) ;
    static void GetPointLineIndexProc(int x, int y, const void* lpdata) ;
    bool IsSprigRoot(int x, int y) ;
    void DrawCross(CCoord &c, int r, int v) ;
    void DrawCross(CPixel &p, int r, int v) ;
    bool SetLine(CLine &line, int v) ;
    bool SetLine(CCoord &c1, CCoord &c2, int v);
    bool GetCheckerIntersections(CPixelBuffer *pSrc, CCoordArray &CalibPoints, CCoordArray &ModelPoints, DOUBLE dGridSize, int nWidth, int nHeight, int nRadius=8, bool bShowBuffer=false, bool bPlotOld=false) ;
    bool GetCheckerIntersections (CCoordArray &Guesses, CCoordArray &Results, int  nRadius, int nCheckRadius=-1) ;
    bool GetAllCheckerIntersections ( CCoordArray   &ReturnValues
				, int			nThreshold
				, int			nRadius );
//    bool GetAllMilanoCheckerIntersections ( CCoordArray   &ReturnValues
	//			, CCoordArray	&SquareCenters
		//		, CPixelChains  &SquarePixels 
			//	, CPixelBuffer  &SquareMask
				//, int			nRadius 
				//, bool			bGetBlackSquares = true);
//    bool GetAllMilanoCheckerIntersections ( CCoordArray   &ReturnValues
	//			, int			nThreshold
		//		, int			nRadius );
    bool GetSobelCrossCoords (CCoord &center, DOUBLE dRadius, CCoordArray &rval) ;
    bool FindOutline(CPixelBuffer *src, int nBlankValue, int nReplaceValue) ;
    void Trace() ;
    int GetMilanoCoords(int xCenter, int yCenter, int radius, CPoint *buffer) ;
    int GetCircleCoords(int xCenter, int yCenter, int radius, CPoint *buffer) ;
    bool ValidCheckerRun(int x, int y, int *pOffsets, int nNumPts, int *pSum=NULL, int nRadius=12, int nMinRange=0, int nMinArcLen=6, int nMinMatchDist=2, bool bDoSet=false) ;
    bool ValidCheckerPos(int x, int y, int *pOffsets, int nNumPts) ;
	bool GetCheckerIntersections ( CPixelBuffer *pSrc, int  nThreshold, int  nRadius) ;
    bool GetCheckerIntersection (CPixelBuffer *pSrc, int x, int y, int nRadius, int nMinArclLen=6, int nMinRange=100);
	bool IsMilanoCheckerIntersection (int			*offsets
								 , int			x 
								 , int			y
								 , int			nNumPts
								   ) ;
	bool IsCheckerIntersection ( CPixelBuffer &threhold
								 , int			*offsets
								 , int			x 
								 , int			y
								 , int			nNumPts
								   ) ;
	bool IsCheckerIntersectionArea ( int		nThreshold
								 , int			x 
								 , int			y
								 , int			nRadius
								   ) ;
    int *Histogram (int nMin=0, int nMax=255, int *pRange=NULL) ;
    void FastGet3x3Values(int x, int y, int *values) ;
    int  FastSobel(int *values) ;
    void FastGetRadiusValues(int x, int y, int w, int r, int *values) ;

    void GetValidCheckerRange(int *values, int *offsets, int nNumPts, int nIndex, int *min, int *max) ;
    void GetValidCheckerThreshold(int *values, int nThreshold, int nNumPts) ;
    bool Radius3Count (CPixelBuffer *pSrc) ;
    void Range7x7 (int *buf, int *low, int *high, int w) ;
	double StdDev (int ind, int *offsets, int nNumPts, double dMean ) ;
	double Mean (int ind, int *offsets, int nNumPts) ;
	double m_dHistogramLow ;
	double m_dHistogramHigh ;
	double m_dHistogramRange ;
	void ExtendPeaksAndThin ( CPixelBuffer *pSrcBuf, int nMaxVal ) ;
	void ThinSort(int nThreshold);
	void StretchLevels(CPixelBuffer *pBuf, int nSrcLow, int nSrcHigh, int nDstLow, int nDstHigh) ;
	void NeighborCopy (CPixelBuffer *pSrc, int nMinNeighbors, int nReplaceVal=255) ;
	int  OffsetValue (int x, int y) ;
	int  OffsetValue (CPoint &pt) ;
	void AdaptiveThreshold_SAT (CPixelBuffer *pSrc, int nThreshold, int nRadius) ;
	void SummedAreaTable(CPixelBuffer *pbuf) ;

	void DumpGpuData() ;

public:
    int  m_w ;      // width of the buffer
    int  m_h ;      // height of the buffer
    int  m_size ;   // size in bytes
    int  m_nStepSize ; // pixel width
    int  *m_buf ;

	// Gpu Memory pointers.  A mangled memory mess, I know!
	// This needs to be put into a separate class....
    bool m_bUseGpu ;
    unsigned m_GpuCodeMain ;
	unsigned m_GpuCodeThread ;
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
    int  m_nSetLineData ;

    int  *m_GaussianMask ;
    int  *m_GaussianOffsets ;
    int  *m_GaussianBufferOffsets ;
    int  m_GaussianMaskWidth ;
    int  m_GaussianMaskHeight ;

	int  *m_pRadiusOffsets ;
	CPoint *m_pRadiusPoints ;
	int  m_nNumRadiusPoints ;

    int  *pRegionStack ;
    int  nRegionStackPos ;

    CPixelArray m_SprigRoots ;
    CPointArray *m_pPointArray ;
    CDWordArray *m_pIndexArray ;
};

#ifdef _dib_section_h

class CPixelBufferDlg : public CDialog
{
// Construction
public:
    CPixelBufferDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CPixelBufferDlg)
    enum { IDD = IDD_SHOW_BUFFER };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPixelBufferDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CPixelBufferDlg)
    virtual bool OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    CPixelBuffer *m_pBuffer ;
    CDibSection  m_DibSection ;
	int			 m_nZoom ;
};
#endif

class CCoordPixelBuffer : public CPixelBuffer
{
public:
	CCoordPixelBuffer ( double xMin
					  , double xMax
					  , double xPrec
					  , double yMin
					  , double yMax
					  , double yPrec) ;

public:
	void SetPixel ( double x, double y, int nValue ) ;
	int GetPixel ( double x, double y ) ;
	bool CoordAt ( int x, int y, CCoord &rval ) ;

	double dLeft ;
	double dRight ;
	double dXRange ;
	double dTop ;
	double dBottom ;
	double dYRange ;
};

#endif
