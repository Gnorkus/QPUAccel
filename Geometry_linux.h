#ifndef Geometry_h
#define Geometry_h

#include <math.h>
#include <float.h>

#define SVD_W_T		2
#define SVD_V_T		4

#define MAXPOLYCOORDS 512
#define GEOMETRYLITE
#define USEFLOAT

#ifdef USEFLOAT
#define DOUBLE float
#define DOUBLE_EPSILON FLT_EPSILON
#define DOUBLE_MAX FLT_MAX
#else
#define DOUBLE double
#define DOUBLE_EPSILON DBL_EPSILON
#define DOUBLE_MAX DBL_MAX
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#ifndef INCHES_PER_METER 
#define INCHES_PER_METER 39.3700787
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 180.0/PI
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD PI/180.0
#endif

extern unsigned char exp_2[8] ;
#include "CString_linux.h"


// These are the classes contained in this header file
class CCoord ;
class CIntCoord ;
class CVector ;
class CLine ;
class CQuaternion ;
class CPlane ;
class CTriangle ;
class CMatrix ;
class CCoordArray ;


// The following classes are imported for external use
class CPixelBuffer ;


typedef struct tagVertex
{
    DOUBLE x ;
    DOUBLE y ;
    DOUBLE z ;
} VERTEX, *PVERTEX ;

typedef struct tagFoundStatus
{
    unsigned char CoordinateSet : 1 ;
    unsigned char NormalVectorCalculated : 1 ;
} FOUNDSTATUS, *PFOUNDSTATUS ;

class CIntCoord
{
public:
    CIntCoord() ;
    CIntCoord(CIntCoord &copy) ;
    ~CIntCoord() ;
    CIntCoord &operator=(CIntCoord &copy) ;
    CIntCoord &operator=(CCoord &copy) ;

    void Set(int x, int y, int z) ;

public:
    int m_x, m_y, m_z/*, m_c*/ ;
} ;

typedef struct tagSIZE
{
    int cx ;
    int cy ;
} SIZE, *LPSIZE, *PSIZE ;

typedef struct tagPOINT
{
    int x ;
    int y ;
} POINT, *LPPOINT, *PPOINT ;

typedef struct tagRECT
{
    long left ;
    long top ;
    long right ;
    long bottom ;
} RECT, *LPRECT ;
typedef const RECT * LPCRECT ;

class CPoint : public tagPOINT
{
public:
    CPoint() { x = 0 ; y = 0 ; }
    CPoint(int sx, int sy) { x = sx; y = sy; }
    void operator+=(const CPoint &p) { x+=p.x; y+=p.y; }
    CPoint operator+(const CPoint &p) { CPoint r; r.x=x+p.x; r.y=y+p.y; return r;}
};

class CSize : public tagSIZE
{
public:
    CSize() { cx = 0; cy = 0 ; }
    CSize(int x, int y) {cx = x ; cy = y ;}
    bool operator!=(CSize &sz) const {if (cx!=sz.cx || cy!=sz.cy) return true; else return false;}
};

class CRect : public tagRECT
{
public:
    CRect() ;
    operator LPRECT () ;
    void SetRectEmpty() {left=top=right=bottom=0;}
    void SetRect(int l, int t, int r, int b) {left=l;right=r;top=t;bottom=b;}
} ;

int MulDiv ( int m1, int m2, int d );

class CCoord 
{
public:
    CCoord () ;
    CCoord (const CCoord &copy) ;
    CCoord (DOUBLE x, DOUBLE y, DOUBLE z) ;
    CCoord (const CVector &copy) ;
    CCoord (DOUBLE *components, int numcomp) ;
    CCoord &operator=(const CCoord& copy);
    CCoord &operator=(const CQuaternion& copy);
	CCoord (const CPoint &pt) ;

public:
	bool Epsilon ( CCoord &compval, DOUBLE epsilon=0.000001 ) ;
    void Set(DOUBLE x, DOUBLE Y, DOUBLE z, long nVal=255) ;
    void Set(DOUBLE *components, int numcomp) ;
    void Offset(DOUBLE xOffset, DOUBLE yOffset, DOUBLE zOffset);
    void Offset(CVector &slope, DOUBLE distance=1.0) ;
    void Round(DOUBLE precision) ;
    virtual void Clear() ;
	CPoint MakePoint() ;
	bool IsEmpty() ;
	int  ScreenDistSqr(CPoint &p) ;
    DOUBLE ScalarCrossProduct2D (CVector &a, int nWhichPlane=0);
    DOUBLE ScalarCrossProduct2D (CCoord &a, int nWhichPlane=0);
    
    bool operator==(CCoord &) const;
    bool operator==(CVector &) const;
    bool operator!=(CCoord &) const;

    void operator+=(const CCoord &);
    void operator+=(const CVector &);
    void operator-=(const CCoord &);
    void operator-=(const CVector &);

    CCoord operator+(const CCoord &) const;
    CCoord operator+(const CVector &) const;
    CCoord operator-(const CCoord &) const;
    CCoord operator-(const CVector &) const;
    CCoord operator-();
    CCoord operator*(DOUBLE Magnitude) ;
    CCoord operator/(DOUBLE Magnitude) ;
    void operator*=(DOUBLE Magnitude) ;
    void operator/=(DOUBLE Magnitude) ;
    CCoord Average(CCoord &point) ;
    operator CVector () ;
	operator CPoint () ;
	DOUBLE Angle (const CCoord &, const CCoord &) ;
	DOUBLE Angle2D (const CCoord &, const CCoord &) ;
	DOUBLE Angle2D (const CCoord &) ;
    DOUBLE DotProduct (CCoord &) ;
    DOUBLE DotProduct (CVector &) ;
    DOUBLE DotProduct2D (CCoord &) ;
    DOUBLE DotProduct2D (CVector &) ;
    DOUBLE PerpDotProduct2D (CCoord &) ;
    DOUBLE PerpDotProduct2D (CVector &) ;
	void Subtract(const CCoord &) ;
	void Subtract(const CVector &) ;
	void Add(const CCoord &) ;
	void Add(const CVector &) ;

    void   ToPoint(LPPOINT) ;

    DOUBLE Distance (const CCoord &) ;
    DOUBLE *GetVal() ;
	void Trace() ;

	CCoord &Rotate(CQuaternion &q, bool Reverse=false) ;
	CCoord &Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse=false) ;

// Make these public since it's a simple class
public:
    DOUBLE m_x ;
    DOUBLE m_y ;
    DOUBLE m_z ;
	int    m_sx ;
	int    m_sy ;
	int    m_nData ;
};

class CColorCoord 
{
public:
    CColorCoord () ;
    CColorCoord (const CColorCoord &copy) ;
    CColorCoord (const CCoord &copy) ;
    CColorCoord &operator=(const CQuaternion& copy);
    CColorCoord (DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE r, DOUBLE g, DOUBLE b) ;
    CColorCoord &operator=(const CColorCoord& copy);
    CColorCoord &operator=(const CCoord& copy);
	void Clear() ;
	CColorCoord &Rotate(CQuaternion &q, bool Reverse=false) ;
	CColorCoord &Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse=false) ;
	CColorCoord &Rotate(DOUBLE *pModelMatrix) ;
    void Offset(DOUBLE xOffset, DOUBLE yOffset, DOUBLE zOffset=0.0);
    void Offset(CVector &slope, DOUBLE distance) ;

public:
	DOUBLE m_x ;
	DOUBLE m_y ;
	DOUBLE m_z ;
	DOUBLE m_r ;
	DOUBLE m_g ;
	DOUBLE m_b ;
	DOUBLE m_d ;
};

// This handles the case of undefined values (i.e. vertical lines)!
class CVector
{
public:
    CVector() ;
	CVector(DOUBLE *p) ;
    CVector(DOUBLE dx, DOUBLE dy, DOUBLE dz=0.0) ;
    CVector(const CVector &copy) ;
    CVector(const CCoord &copy) ;
    CVector(DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2) ;
    CVector(CLine &) ;
    CVector(const CCoord &c1, const CCoord &c2, bool Normalize=true) ;
    CVector &operator=(const CVector& copy);
    CVector &operator=(const CQuaternion& copy);

public:
    void Set(DOUBLE dx, DOUBLE dy, DOUBLE dz=0.0) ;
    void Set(const CVector &copy) ;
    void Set(DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2) ;
    void Set(CLine &) ;
    void Set(const CCoord &c1, const CCoord &c2, bool Normalize=true) ;
    void SetNormal2D(const CCoord &c1, const CCoord &c2, bool Normalize=false) ;
    void Set(DOUBLE *components, int numcomp) ;
    void Invert() ;

    CVector& operator=(CLine& copy);
    CVector& operator=(const CCoord& copy);
    bool operator==(CVector &) const;
    bool SignsSameAs(CVector&) ;
    operator DOUBLE() ;
    CVector &operator+(const CVector &addend) ;
    CVector &operator-(const CVector &diffend) ;
    CVector operator-() ;
    CVector operator*(DOUBLE Magnitude) ;
    CVector operator/(DOUBLE Magnitude) ;
    void operator+=(const CVector &addend) ;
    void operator*=(DOUBLE Magnitude) ;
    void operator-=(const CVector &diffend) ;
    int AboveBelow2D(CCoord &origin, CCoord &point) ;

    bool IsDefined() ;
    bool IsVertical() ;
    bool IsZero() ;

	void Subtract(const CCoord &) ;
	void Subtract(const CVector &) ;
	void Add(const CCoord &) ;
	void Add(const CVector &) ;
    void Normalize() ;
    void Clear() ;
    DOUBLE DotProduct (const CVector &) ;
    DOUBLE DotProduct (CCoord &) ;
    DOUBLE DotProduct2D (CVector &) ;
    DOUBLE DotProduct2D (CCoord &) ;
    DOUBLE PerpDotProduct2D (CCoord &) ;
    DOUBLE PerpDotProduct2D (CVector &) ;
    bool CrossProduct (const CVector &a, const CVector &b, bool Normalize=false);
    DOUBLE ScalarCrossProduct2D (CVector &a, int nWhichPlane=0);
    DOUBLE ScalarCrossProduct2D (CCoord &a, int nWhichPlane=0);
    DOUBLE *GetVal() ;
    DOUBLE Magnitude() ;
	void Trace() ;
	CVector &Rotate(CQuaternion &q, bool Reverse=false) ;
	CVector &Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse=false) ;

public:
    DOUBLE m_dx ;
    DOUBLE m_dy ;
    DOUBLE m_dz ;
} ;


class CRectangle
{
public:
	CRectangle();
	CRectangle(CTriangle &one) ;
	~CRectangle() ;

	operator LPRECT () ;
	operator LPCRECT () ;

	CCoord m_TopLeft ;
	CCoord m_BottomRight ;
	CRect  m_rcBounds ;
};


class CLine
{
public:
    CLine () ;
    CLine (CVector &slope, CCoord &intercept) ;
    CLine (CCoord &intercept, CVector &slope) ;
    CLine (DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2) ;
    CLine (DOUBLE *p1, DOUBLE *p2) ;
    CLine (CCoord &first, CCoord &second) ;
    CLine (const CLine &);
    CLine &operator=(const CLine& copy);

public:
	bool Joined (CLine &testline) ;
    void Set (CVector &slope, CCoord &intercept) ;
    void Set (CCoord &intercept,CVector &slope) ;
    void Set (DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2 ) ;
    void Set (CCoord &first, CCoord &second) ;
    DOUBLE Length() ;
	void FlipOrder() ;
	CLine  Project(CPlane &ProjectionPlane) ;
	CLine  ReverseVector() ;
    CCoord XPlaneIntersect(DOUBLE x) ;
    CCoord YPlaneIntersect(DOUBLE y) ;
    CCoord ZPlaneIntersect(DOUBLE z) ;
    bool operator==(CLine &) const;
    bool Intersect2 (CLine &, CCoord &rval, bool *IsSkewOrParallel=NULL) ;
    bool Intersect (CLine &, CCoord &rval, bool *IsParallel=NULL) ;
    DOUBLE XYDistance (CCoord &) ;
    DOUBLE Distance (CCoord &, bool Normalize=true) ;
	CCoord Nearest(CCoord &, bool Normalize=true);
	CCoord Nearest(CCoordArray &, bool Normalize=true) ;
    CVector Slope(bool Normalize=true) ;
    CCoord CoordAtX(DOUBLE x, bool *pUndefined=NULL) ;
    CCoord CoordAtY(DOUBLE Y, bool *pUndefined=NULL) ;
    CCoord CoordAtZ(DOUBLE x, bool *pUndefined=NULL) ;
    CCoord MidPoint() ;

    bool IsParallel(CLine &) ;
    bool IsOrthogonal(CLine &) ;
    bool IsVertical() ;
    bool IsAdjoining(CLine &TestLine) ;
    DOUBLE DotProduct (CCoord &) ;
    DOUBLE DotProduct (CVector &) ;
    CVector CrossProduct(CCoord &OuterCoord, bool Normalize=false);

public:
    CCoord  m_first, m_second ;
	CLine	*m_pPrevious ;
} ;

/*
class CRationalCoord : public CCoord
{
public:
    CRationalCoord() ;
    CRationalCoord (const CRationalCoord &copy) ;
    CRationalCoord (DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE h=1.0) ;
    CRationalCoord  (const CCoord &copy, DOUBLE h=1.0);
    CRationalCoord &operator=(const CRationalCoord& copy);
    CRationalCoord &operator=(const CCoord& copy);

public:
    void Set(DOUBLE x, DOUBLE Y, DOUBLE z, DOUBLE h=1.0) ;
    virtual void Clear() ;

    DOUBLE m_h ;
} ;
*/

//template <> void AFXAPI ConstructElements (CCoord*, int);

typedef CArray<int, int>                CIntArray ;
typedef CArray<unsigned int, unsigned int> CUIntArray ;
typedef CArray<unsigned char, unsigned char> CByteArray ;
typedef CArray<DOUBLE, DOUBLE>          CDoubleArray ;
typedef CArray<unsigned char, unsigned char> CCharArray ;
//typedef CArray<LONGLONG, LONGLONG>      CLongLongArray ;
typedef CArray<CIntCoord, CIntCoord&>   CIntCoordArray ;
typedef CArray<CVector, CVector&>       CVectorArray ;
typedef CArray<CCoord, CCoord&>			  CBaseCoordArray ;
typedef CArray<CLine, CLine&>           CBaseLineArray ;
//typedef CArray<CRationalCoord, CRationalCoord&> CRationalCoordArray ;
typedef CArray<CSize, CSize&>			CSizeArray ;
typedef CArray<CColorCoord, CColorCoord&> CBaseColorCoordArray ;
typedef CArray<DWORD,DWORD>             CDWordArray ;
typedef CArray<void*,void*>             CPtrArray ;

class CCoordArray : public CBaseCoordArray
{
public:
	~CCoordArray() ;
	bool CopyToMatrix (CMatrix &dst, bool bUse2D=false) ;
    CCoord Average() ;
	CCoord Average(int nStart, int nEnd=-1) ;
	void RadialSort(CCoord &CCoord) ;
	static int Radial2DSortCompare(const void *elem1, const void *elem2) ;
    void AddSelection(int i) ;
	int AddUnique(CCoord&c) ;
    void ClearSelection() ;
	void Rotate(CQuaternion &Rotation, bool bReverse=false) ;
    void Offset(CVector &slope, DOUBLE distance) ;
    void Offset(CCoord &amount) ;
    void Offset(CVector &amount) ;
    void DeleteSelections() ;
    CCoordArray &operator=(const CCoordArray&);
    CCoordArray &operator=(CMatrix&);
//	operator VARIANT () ;
	void Sort(CPlane &Plane) ;
	void Sort(CPlane &Plane, int start, int len) ;
	void Sort(CVector &Vector) ;
	void Sort(CVector &Vector, int start, int len) ;
	void Trim(CPlane &Plane) ;
	void RadialTrim(CCoord &center, double dLessAngle, double dGreaterAngle) ;
	void SetSizeInit(int nNumElem, CCoord &initval) ;
	static int SortCompare(const void *elem1, const void *elem2) ;
	void TraceDump() ;
	CCoord RangeMin() ;
	CCoord RangeMax() ;
	CCoord Farthest(CPlane &, int *pNearestInd=NULL) ;
	CCoord Farthest(CCoord &, int *pNearestInd=NULL) ;
	CCoord Nearest(CCoord &, int *pNearestInd=NULL) ;
	bool Nearest(CCoord &rval, const CCoord &testpt, const CVector &dir, DOUBLE dAnglePrecision, DOUBLE dDistLow=(DOUBLE)-1.0, DOUBLE dDistHigh=(DOUBLE)1000000, int *pNearestInd=NULL) ;
	bool PopNearest(CCoord &rval, const CCoord &testpt, int *pInd, DOUBLE dDistLow=(DOUBLE)-1.0, DOUBLE dDistHigh=(DOUBLE)1000000.0) ;
	bool PopNearest(CCoord &rval, const CCoord &bottom, CCoord &center, DOUBLE dAngleLow, DOUBLE dAngleHigh) ;
	bool PopNearest(CCoord &rval, const CCoord &testpt, const CVector &dir, DOUBLE dAnglePrecision, DOUBLE dDistLow=(DOUBLE)-1.0, DOUBLE dDistHigh=(DOUBLE)1000000) ;
	CCoord Pop(int nIndex) ;
	bool GetXYPixelBuffer(CPixelBuffer &result, int dx, int dy, CCoord Prec, int nOffsetAdd=0, int Flags=0) ;

	bool Save(LPCTSTR filename=NULL) ;
	bool Load(LPCTSTR filename=NULL) ;
	bool IsEqual (CCoordArray &ca) ;


    CUIntArray m_Selection ;
	static CPlane m_SortPlane ;
	static CCoord m_SortCoord ;
};


class CColorCoordArray : public CBaseColorCoordArray
{
public:
	void Rotate(CQuaternion &Rotation, bool bReverse=false) ;
    void Offset(CVector &slope, DOUBLE distance) ;
    void Offset(CCoord &amount) ;
    void Offset(CVector &amount) ;
    CColorCoordArray &operator=(const CColorCoordArray&);
	void Sort(CPlane &Plane) ;
	void Sort(CPlane &Plane, int start, int len) ;
	void Sort(CVector &Vector) ;
	void Sort(CVector &Vector, int start, int len) ;
	void Trim(CPlane &Plane) ;
	static int SortColorCoordCompare(const void *elem1, const void *elem2) ;

	static CPlane m_SortPlane ;
};


class CLineArray : public CBaseLineArray
{
public:
	CLineArray &operator=(const CCoordArray &);
	CLineArray &operator=(const CLineArray &);
	DOUBLE Nearest (CCoord &testcoord) ;
	bool NearestX (DOUBLE x, CCoord &rval) ;
	CCoord Average() ;
	int AddUnique(CCoord&c1, CCoord &c2) ;
	void JoinSort() ;
	void FlipOrder() ;
	void Rotate (CQuaternion &rAmount, bool bReverse) ;
    void Offset(CVector &slope, DOUBLE distance) ;
    void Offset(CCoord &amount) ;
    void Offset(CVector &amount) ;
	void Trim (CPlane &trimplane) ;
	CLine Extents() ;
};

class CPlane
{
public:
    CPlane() ;
    CPlane(const CPlane &copy) ;
    CPlane(const CCoord &Origin, const CVector &Normal);
    CPlane(const CCoord &Origin, const CVector &DirOne, const CVector &DirTwo);
    CPlane(const CCoord &Origin, const CCoord &DirOne, const CCoord &DirTwo);
	CPlane(DOUBLE *pNormalOrigin) ;
	CPlane(DOUBLE *pOrigin, DOUBLE *pDir1, DOUBLE *pDir2, bool Normalize) ;

	CPlane(const CCoord &Origin, const CLine &Line) ;

	bool IntersectTriangles(CDoubleArray &da, CLineArray &la) ;
	bool IntersectTriangle(DOUBLE *pTriangle, DOUBLE *segments, bool bNormalize=true) ;
	bool IntersectTriangle(DOUBLE *pTriangle, CLineArray &segments, bool bNormalize=true) ;
	bool IntersectTriangle(CCoord &first, CCoord &second, CCoord &third, CLineArray &segments) ;
	bool IntersectTriangle(CCoord &first, CCoord &second, CCoord &third, CCoordArray &segments) ;
    bool IntersectSegments(CCoordArray &segments, CCoordArray &result);
    bool IntersectSegments(CLineArray &segments, CCoordArray &result);
    bool IntersectSegments(CLineArray &Coords, CLineArray &Result);
    bool IntersectSegments(CLineArray &Coords, CLine &Result);
    bool Intersect(DOUBLE *ILineCoord1, DOUBLE *ILineCoord2, DOUBLE *point, bool *pParallel=NULL, bool bSegment=false);
    bool Intersect(const CLine &IntersectLine, CCoord &point, bool *pParallel=NULL);
    bool Intersect(const CLine &IntersectLine, DOUBLE *rval, bool *pParallel=NULL);
    bool Intersect(CPlane &IntersectPlane, CLine &Line, bool *pParallel=NULL) ;
    bool IntersectSegment(CCoord &first, CCoord &second, CCoord &point, bool *pParallel=NULL);
    bool IntersectSegment(CLine &line, CCoord &point, bool *pParallel=NULL);
    bool IntersectSegment(DOUBLE *first, DOUBLE *second, DOUBLE *result, bool *pParallel=NULL);

	bool IntersectHalfPlane(bool bTop, CLineArray &segments, CLineArray &result) ;
	bool IntersectHalfPlane(bool bTop, CCoordArray &segments, CCoordArray &result) ;

	CCoord FindFarthest(bool bTop, CCoordArray &coords) ;
	CCoord FindNearest(bool bTop, CCoordArray &coords) ;
	CCoord FindNearest(bool bTop, CLineArray &coords) ;
	int    FindNearestIndex(bool bTop, CCoordArray &coords) ;
	CCoord FindNearest(CCoordArray &coords) ;
	CCoord FindNearest(CLineArray &coords) ;
	CCoord FindFarthest(CLineArray &coords) ;

	CCoord IntersectXY (DOUBLE x, DOUBLE y) ;
	CCoord IntersectYZ (DOUBLE y, DOUBLE z) ;
	CCoord IntersectXZ (DOUBLE x, DOUBLE z) ;

	CCoord GridPoint(DOUBLE x, DOUBLE y, DOUBLE z) ;

    CCoord NearestPointInPlane(CCoord &point, bool *pbOnTop=NULL) ;
    DOUBLE Distance(const CCoord &point, bool *pTop, bool bNormalize=false) ;
    DOUBLE Distance(DOUBLE *point, bool *pTop, bool bNormalize=false) ;
    int AboveBelow(CLineArray &lines) ;
    int AboveBelow(CCoord &point) ;
    int AboveBelow(DOUBLE *point) ;
    operator CVector& () ;
    operator CCoord& () ;
    void operator=(const CPlane& copy);
    bool operator==(CPlane &comp) const;
    CPlane operator-() ;
    void Set(const CCoord &Origin, CVector Normal) ;
    void Set(const CCoord &Origin, CVector DirOne, CVector DirTwo);
    void Set(const CCoord &Origin, const CCoord &DirOne, const CCoord &DirTwo);
    void Set(CLine &OrgVec, const CVector &DirTwo) ;
	void Set(DOUBLE *pOrigin, DOUBLE *pCoord1, DOUBLE *pDir2, bool Normalize=true) ;
	void Set2(DOUBLE *pOrigin, DOUBLE *pCoord1, DOUBLE *pCoord2, bool Normalize=true) ;
    CPlane Offset(DOUBLE amount) ;
	void Normalize() ;
	void GenerateUnitVectors() ;

public:
    CCoord  m_origin ;
    CVector m_normal ;
    //DOUBLE  m_k ;
	//CVector m_UnitX, m_UnitY ;
};


class CTriangle
{
public:
	CTriangle();
	CTriangle(DOUBLE *) ;
	CTriangle(CCoord &one, CCoord &two, CCoord &three) ;
	~CTriangle() ;

	void Set(DOUBLE *) ;
	void Set(DOUBLE *, DOUBLE *, DOUBLE *) ;
	void Set(CCoord &one, CCoord &two, CCoord &three) ;
	CTriangle &Offset (CVector &amount) ;
	DOUBLE Area() ;
	static DOUBLE Area(DOUBLE *) ;
	static DOUBLE Area(CCoord &c1, CCoord &c2, CCoord &c3) ;
	static bool BarycentricTransform  ( DOUBLE *pTriangleSrc
									  , DOUBLE *pTriangleDst
									  , DOUBLE *pSrc);
	static bool BarycentricTransform3D2D  ( DOUBLE *pTriangleSrc
										  , DOUBLE *pTriangleDst
										  , DOUBLE *pSrc);
	bool MinMax ( CCoord &min
				, CCoord &max
				, CCoord *prec ) ;
	static bool MinMax  ( DOUBLE *pTriangle
						, DOUBLE x
						, DOUBLE y 
						, CCoord &min
						, CCoord &max 
						, CCoord *prec ) ;
	static bool MinMax  ( DOUBLE *pTriangleV1
						, DOUBLE *pTriangleV2
						, DOUBLE *pTriangleV3
						, DOUBLE x
						, DOUBLE y 
						, CCoord &min
						, CCoord &max 
						, CCoord *prec ) ;
	CCoord Center() ;
	operator LPRECT () ;
	operator LPCRECT () ;
	void GetFastCoords(DOUBLE *pFastCoords) ;
 	CTriangle &Scale(DOUBLE scalarmultiplier) ;

	bool Inside2D ( int x1, int y1
				  , int x2, int y2
				  , int x3, int y3
				  , int  x, int  y ) ;

	int Intersect(CLine &ray, CCoord &rval) ;
	int Intersect(CLine &ray, DOUBLE *rval) ;
	int IntersectFast ( CLine &ray, DOUBLE *rval);

	static int Intersect ( CLine &ray, DOUBLE *pNormalTriangle, DOUBLE *rval) ;
	static int Intersect ( CLine &ray
						 , CPlane &p
						 , CPlane &p1
						 , CPlane &p2
						 , CPlane &p3
						 , DOUBLE *rval) ;
	static int IntersectFast(DOUBLE *T, DOUBLE *P0, DOUBLE *P1, DOUBLE *rval) ;

	CCoord m_Vertices[3] ;
	CRect  m_rcBounds ;
	CPlane m_LineBoundaries[3] ;
	CPlane m_Plane ;
};


class CCoordIntensity : public CCoord
{
public:
    int m_intensity ;
};

/*
typedef CArray<CCoordIntensity, CCoordIntensity&> CBaseCoordIntensityArray ;

typedef  int (cdecl *COORDINTENSITYCOMPAREFUNC)(const void *,const void *) ;

class CCoordIntensityArray : public CBaseCoordIntensityArray
{
public:
    void Sort() ;
};
*/

typedef CArray<CCoordArray, CCoordArray&> CBaseCoordFrameArray;
typedef CArray<CLineArray, CLineArray&> CBaseLineFrameArray;
typedef CArray<CDWordArray, CDWordArray&> CBaseLongFrameArray;

class CLineFrameArray : public CBaseLineFrameArray
{
public:
	void SetSizeCopy(CLineFrameArray &rootarray) ;
	void Rotate (CQuaternion &rAmount, bool bReverse) ;
    void Offset(CVector &slope, DOUBLE distance) ;
    void Offset(CCoord &amount) ;
    void Offset(CVector &amount) ;
	void JoinSegments ( CLineArray &la ) ;

	int Longest(int *nSize=NULL) ;
    CLineFrameArray &operator=(CLineFrameArray& copy);
};

#define COORD_FRAME_ARRAY_COMP_GREATER 0x00000001
#define COORD_FRAME_ARRAY_COMP_EQUAL   0x00000002
#define COORD_FRAME_ARRAY_COMP_LESS    0x00000004

class CCoordFrameArray : public CBaseCoordFrameArray
{
public:
	bool Save(LPCTSTR filename=NULL) ;
	void SetSizeCopy(CCoordFrameArray &rootarray) ;
	void Init(CCoord &copy) ;
	void Merge() ;
	long TotalSize() ;
	void Rotate(CQuaternion &rAmount, bool bReverse) ;
    void Offset(CVector &slope, DOUBLE distance) ;
    void Offset(CCoord &amount) ;
    void Offset(CVector &amount) ;
	void Trim(CPlane &whichway) ;
	bool GetCoords(CCoordFrameArray &src, int nCompData, int nCompMask) ;
	bool CopyToMatrix (CMatrix &dst, bool bUse2D=false) ;
    CCoordFrameArray &operator=(CCoordFrameArray& copy);
};

class CLongFrameArray : public CBaseLongFrameArray
{
public:
	void SetSizeCopy(CLongFrameArray &rootarray) ;
	void SetSizeCopy(CCoordFrameArray &rootarray) ;
	void Init(DWORD seed) ;
	void Merge() ;
	bool Save(LPCTSTR filename=NULL) ;
	bool Load(LPCTSTR filename=NULL) ;
	int  Compare(CLongFrameArray &cmparray) ;
    CLongFrameArray &operator=(CLongFrameArray& copy);
};

/*
class CCurve
{
public:
    CCurve() ;
    ~CCurve() ;
    virtual void Clear() = 0;
    virtual bool Interpolate(CCoord &c, DOUBLE t)=0 ;
    virtual bool Interpolate(CCoordArray &c, int numelem)=0 ;
    DOUBLE Factorial(int n);

protected:
    bool m_bOpen ;
    bool m_bFit ;
};

// We will need a piecewise bezier curve...
// So later, we will use the order to cause the 
// generation of that curve
class CBezierCurve : public CCurve
{
public:
    CBezierCurve() ;
    ~CBezierCurve() ;

    virtual void Clear() ;
    virtual void Initialize(CCoordArray &coords, int order=0, bool Fit=true, bool open=false) ;
    virtual bool Interpolate(CCoord &c, DOUBLE t) ;
    virtual bool Interpolate(CCoordArray &c, int numelem) ;

private:
    DOUBLE CombinatorialFactor(int n, int i);

    CDoubleArray m_BernsteinCombinatorials ;
    CCoordArray  m_Coords ;
};
class CCubicSplineCurve : public CCurve
{
public:
    CCubicSplineCurve() ;
    ~CCubicSplineCurve() ;

    virtual void Clear() ;
    virtual void Initialize(CCoordArray &coords, int order=0, bool Fit=true, bool open=false) ;
    virtual bool Interpolate(CCoord &c, DOUBLE t) ;
    virtual bool Interpolate(CCoordArray &c, int numelem) ;

private:
    CDoubleArray m_t ;
    CCoordArray  m_Coords ;
};
class CBSplineCurve : public CCurve
{
public:
    CBSplineCurve() ;
    ~CBSplineCurve() ;

    virtual void Clear() ;
    virtual void Initialize(CCoordArray &coords, int order=0, bool Fit=true, bool open=false) ;
    virtual bool Interpolate(CCoord &c, DOUBLE t) ;
    virtual bool Interpolate(CCoordArray &c, int numelem) ;

private:
    DOUBLE BSplineBasis(int index, int order, DOUBLE t) ;
    DOUBLE GetKnot(int index) ;
    DOUBLE BasisDivide(DOUBLE numerator, DOUBLE denominator) ;

    CCoord PseudoStart() ;
    CCoord PseudoEnd() ;

    CCoordArray  m_Coords ;
    int          m_nOrder ;
    CDoubleArray m_Knots ;

    DOUBLE       m_dLower;
    DOUBLE       m_dUpper;
};

class CRationalBSplineCurve : public CCurve
{
public:
    CRationalBSplineCurve() ;
    ~CRationalBSplineCurve() ;

    virtual void Clear() ;
    virtual void Initialize(CRationalCoordArray &coords, int order=0, bool Fit=true, bool open=false) ;
    virtual bool Interpolate(CCoord &c, DOUBLE t) ;
    virtual bool Interpolate(CCoordArray &c, int numelem) ;

private:
    DOUBLE RationalBSplineBasis(int index, int order, DOUBLE t) ;
    DOUBLE BSplineBasis(int index, int order, DOUBLE t) ;
    DOUBLE GetKnot(int index) ;
    DOUBLE BasisDivide(DOUBLE numerator, DOUBLE denominator) ;

    CRationalCoordArray  m_Coords ;
    int          m_nOrder ;
    CDoubleArray m_Knots ;

    DOUBLE       m_dLower;
    DOUBLE       m_dUpper;
};*/

class CCoordInt
{
public:
    CCoordInt() ;
    CCoordInt(int cx, int cy, int cz) ;
    CCoordInt(const CCoordInt &copy) ;
    CCoordInt &operator=(const CCoordInt& copy);

    void Copy(CCoordInt &src);
    void Clear() ;
    void Add(CCoordInt &src);
    CCoordInt operator!() ;

    CCoordInt operator-(CCoordInt &c) ;
    CCoordInt operator+(CCoordInt &c) ;
    bool operator==(CCoordInt &) const;
    bool operator!=(CCoordInt &) const;

    int x ;
    int y ;
    int z ;
} ;

typedef CArray<CCoordInt, CCoordInt&> CCoordIntArray ;

class CMatrix ;

class CQuaternion
{
public:
	CQuaternion() ;
	CQuaternion(DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE w) ;
	CQuaternion(CVector &v, DOUBLE w) ;
	CQuaternion(CCoord &v, DOUBLE w) ;
	~CQuaternion() ;

	static CQuaternion ConstructRotation(CVector v, DOUBLE theta) ;
	static CQuaternion ConstructRotationRad(CVector v, DOUBLE theta) ;

	CVector ToEuler() ;

    bool operator==(CQuaternion &) const;
    bool operator!=(CQuaternion &) const;

	// Addition and subtraction
    void operator+=(CQuaternion &);
    void operator-=(CQuaternion &);
    CQuaternion operator+(CQuaternion &) const;
    CQuaternion operator-(CQuaternion &) const;

	// Same as conjugate
    CQuaternion operator-() const;

	// Multiplication
    CQuaternion operator*(DOUBLE Magnitude) const ;
    CQuaternion operator/(DOUBLE Magnitude) const ;

	// Dot Product
    DOUBLE operator *(const CQuaternion &c) ;
	// Cross Product
    CQuaternion operator^(const CQuaternion &c) ;
    CQuaternion operator^(const CVector &c) ;
    CQuaternion operator^(DOUBLE *vec) ;
    CQuaternion operator^(const CCoord &c) ;
    CQuaternion operator^(const CColorCoord &c) ;
    CQuaternion operator^(const CColorCoord *c) ;

	CVector Vector() ;
	DOUBLE  Scalar() ;

	CQuaternion operator ~() ;
	CQuaternion &Normalize() ;
	DOUBLE Norm() ;

    CQuaternion &operator=(const CQuaternion &copy) ;

	CMatrix Matrix3x3() ;
	CMatrix Matrix4x4() ;

	void Trace() ;

	bool Rotate(DOUBLE *pVec, int nNumElem=1, int nStepSize=3, bool bReverse=true) ;

	DOUBLE m_x, m_y, m_z, m_w ;
};




bool MatchSegmentArrays(CLineArray &first, CLineArray &second, CLineFrameArray &result) ;
bool LeftToRightSort(CCoordArray &coords);
unsigned char CharacterizeXYSlope(CIntCoord &first, CIntCoord &second) ;
unsigned char CharacterizeYZSlope(CIntCoord &first, CIntCoord &second) ;

/*
bool FindControlRuns ( CCoordArray          &source
                     , CRationalCoordArray  &dest
                     //, CCoordArray            &dest
                     , DOUBLE               dPrecision);
                     */

#define NORM_C  0
#define NORM_L1 1
#define NORM_L2 2

class CMatrix
{
public:
	CMatrix() ;
	CMatrix(int nRows, int nCols=1, DOUBLE *pData=NULL, bool bReference=false) ;
	CMatrix(const char * flag, int dimension);
	~CMatrix() ;
	CMatrix(const CMatrix &m) ;

    CMatrix		&operator=(const CMatrix &copy) ;

	CVector		Mult(CVector &) ;
	CCoord		Mult(CCoord &) ;
	bool		MulTransposed(CMatrix &src, CMatrix *pDelta=NULL) ;
	bool		MulTransposed(CMatrix &src, int nOrder) ;
	CMatrix		operator *(const CMatrix &op) ;
	CMatrix		operator +(const CMatrix &op) ;
	CMatrix&	operator +=(CMatrix &op) ;
	CMatrix&	operator -=(CMatrix &op) ;
	CMatrix		operator -(CMatrix &op) ;
	CMatrix		operator *(DOUBLE op) ;
	CMatrix		operator +(DOUBLE op) ;
	CMatrix		operator -(DOUBLE op) ;
	DOUBLE *	operator[] (int index) const ;
	DOUBLE &    operator()(int index) ;
	void		Resize(int nRows, int nCols, DOUBLE *pData, bool bReference=false) ;
	void		Resize(int nRows, int nCols) ;
	void		Resize(CMatrix &src) ;
	void		SetIdentity(DOUBLE v=(DOUBLE)1.0) ;
	bool		CheckSize(int nRows, int nCols) ;
	void		Trace(LPCTSTR szLabel=NULL, int width=23, int prec=19) ;
	int			NumElements() ;
	int			Index(int nRow, int nCol) ;

	bool		Transpose(CMatrix &src) ;
	CMatrix		Transpose();					// transpose a square matrix 
	CMatrix&	TransposeSelf();				// transpose a square matrix 
	//CMatrix     t(){ return Transpose();};		// Transpose shell for newmat
	DOUBLE		Determinant();					// Determinant
	CMatrix		Inverse();						// Inverse
	//CMatrix		i(){ return Inverse();}			// Inverse shell for newmat
	bool		CrossProduct(CMatrix &v1, CMatrix &v2) ;

	CMatrix&	CopyToSel(CMatrix &src, int rStart=0, int cStart=0, int nHeight=-1, int nWidth=-1) ;
	bool		GetColumns(CMatrix &src, int nStart, int nFinish, bool bReference=false) ;
	bool		GetRows(CMatrix &src, int nStart, int nFinish, bool bReference=false) ;
	bool		GetSubMatrix(CMatrix &src, int nRowS, int nColS, int nRowF, int nColF, bool bReference = false) ;
	bool		AvgCols(CMatrix &src) ;
	bool		AvgRows(CMatrix &src) ;
	bool		StandardDeviationCols(CMatrix &src) ;
	bool		StandardDeviationRows(CMatrix &src) ;
	bool		AvgStandardDeviationCols(CMatrix &src) ;
	bool		AvgStandardDeviationRows(CMatrix &src) ;
	bool        Zero() ;
	int			MaxDim() ;
	int			MinDim() ;
	CString		Print(int width=17, int prec=12) ;
	static bool SaveTraceFile(LPCTSTR filename) ;
	static void	ClearTrace() ;
	bool		Scale (CMatrix &src, DOUBLE dScale, DOUBLE dShift=(DOUBLE)0.0) ;
	bool		dsvdcmp ( CMatrix &w
						, CMatrix &v
//						, CMatrix &v
//						, int nFlags=0
						  ) ;
	bool		svdbksub(CMatrix &u, CMatrix &w, CMatrix &v, CMatrix &b) ;
	CMatrix&	Reshape(int nRows=-1, int nCols=-1) ;
	CMatrix&	Reshape(CMatrix& srcshape) ;
	DOUBLE		Norm(int nType=NORM_L2) ;
	DOUBLE		Norm(CMatrix &arr2, int nType=NORM_L2) ;
	DOUBLE		Norm(CMatrix &arr2, CMatrix &mask, int nType=NORM_L2) ;
	
private:
	DOUBLE &    val(int row, int col);
	CMatrix		scale(void);
	void		copy_column(CMatrix& mm, int from_col, int to_col);
	void		deepcopy(CMatrix& from, CMatrix& to);
	bool		lu_decompose(CMatrix &src, CMatrix& indx, int& d );
	void		switch_columns(int col1, int col2);
	void		lu_back_subst(CMatrix& indx, CMatrix& b);

public:
	int				m_nCols ;
	int				m_nRows ;
	int				m_nStep ;
	bool			m_bReference ;
	DOUBLE			*m_pElem ;
	static CString  m_dumpstr ;
};



#endif
