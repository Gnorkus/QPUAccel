#include "Main.h"
#include "Geometry_linux.h"
#include "PixelBuffer_linux.h"
// The following are class member definitions for coordinate points.
#define EPSILON 0.0000001


int MulDiv ( int m1, int m2, int d ) { return m1*m2/d; }

CRect::CRect() 
{ 
    top = 0;
    bottom = 0;
    left = 0;
    right = 0; 
}

CRect::operator LPRECT()
{
    return this; 
} 


void CColorCoord::Offset(DOUBLE x, DOUBLE y, DOUBLE z)
{
    m_x += x ;
    m_y += y ;
    m_z += z ;
}

// Move the point a distance in the direction of slope
void CColorCoord::Offset(CVector &slope, DOUBLE distance)
{
    // The slopes are normalized!
    m_x += distance * slope.m_dx ;
    m_y += distance * slope.m_dy ;
    m_z += distance * slope.m_dz ;
}

CColorCoord &CColorCoord::Rotate(CQuaternion &qtemp, bool Reverse)
{
    CQuaternion q = qtemp ;
	if (Reverse)
		q.m_w = -q.m_w ;

	*this = q ^ *this ^ ~q ;

	if (Reverse)
		q.m_w = -q.m_w ;

	return *this;
}

CColorCoord &CColorCoord::Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse)
{
	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	*this = q ^ *this ^ qInv ;

	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	return *this;
}

CColorCoord::CColorCoord()
{
	m_r = m_g = m_b = m_x = m_y = m_z = m_d = 0.0 ;
}

CColorCoord::CColorCoord (DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE r, DOUBLE g, DOUBLE b) 
{
    m_x = x ;
    m_y = y ;
    m_z = z ;
	m_r = r ;
	m_g = g ;
	m_b = b ;
}

CColorCoord::CColorCoord (const CColorCoord &copy) 
{
	m_x = copy.m_x ;
	m_y = copy.m_y ;
	m_z = copy.m_z ;
	m_r = copy.m_r ;
	m_g = copy.m_g ;
	m_b = copy.m_b ;
	m_d = copy.m_d ;
}

CColorCoord::CColorCoord (const CCoord &copy) 
{
	m_x = copy.m_x ;
	m_y = copy.m_y ;
	m_z = copy.m_z ;
	m_r = 0.0 ;
	m_g = 0.0 ;
	m_b = 0.0 ;
	m_d = 0.0 ;
}

void CColorCoord::Clear()
{
	m_x = m_y = m_z = 0.0 ;
	m_r = m_g = m_b = 0.0 ;
	m_d = 0.0 ;
}

CColorCoord& CColorCoord::operator=(const CColorCoord& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	m_r = copy.m_r ;
	m_g = copy.m_g ;
	m_b = copy.m_b ;
	m_d = copy.m_d ;
	return *this;
}

CColorCoord& CColorCoord::operator=(const CQuaternion& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	return *this;
}

CColorCoord& CColorCoord::operator=(const CCoord& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	return *this;
}










CCoord::CCoord()
{
	m_x = m_y = m_z = 0.0 ;
	m_sx = m_sy = 0;
	m_nData = 255 ;
}

CCoord::CCoord(const CPoint &pt)
{
	m_x = (DOUBLE)pt.x ;
	m_y = (DOUBLE)pt.y ;
	m_z = 0.0 ;
	m_sx = m_sy = 0;
	m_nData = 255 ;
}

DOUBLE CCoord::Angle(const CCoord &one, const CCoord &two)
{
	CVector dirone(*this, one) ;
	CVector dirtwo(*this, two) ;
	DOUBLE  angle ;

	dirone.Normalize() ;
	dirtwo.Normalize() ;

	angle = (DOUBLE) (acos ( (DOUBLE) dirone.DotProduct(dirtwo) ) * RAD_TO_DEG) ;

	return angle ;
}

DOUBLE CCoord::Angle2D(const CCoord &one, const CCoord &two)
{
	CVector dirone(*this, one) ;
	CVector dirtwo(*this, two) ;
	DOUBLE  angle ;

	dirone.Normalize() ;
	dirtwo.Normalize() ;

	angle = (DOUBLE) (atan2 ( (DOUBLE) dirone.PerpDotProduct2D(dirtwo), (DOUBLE) dirone.DotProduct2D(dirtwo) ) * RAD_TO_DEG );

	return angle ;
}

DOUBLE CCoord::Angle2D(const CCoord &one)
{
	CVector dirone(*this, one) ;
	CVector dirtwo(1,0,0) ;
	DOUBLE  angle ;

	dirone.Normalize() ;
	dirtwo.Normalize() ;

	angle = (DOUBLE) (atan2 ( (DOUBLE) dirone.PerpDotProduct2D(dirtwo), (DOUBLE) dirone.DotProduct2D(dirtwo) ) * RAD_TO_DEG );

	return angle ;
}

DOUBLE CCoord::DotProduct (CVector &slope)
{
    return slope.m_dx*m_x + slope.m_dy*m_y + slope.m_dz * m_z;
}

DOUBLE CCoord::DotProduct (CCoord &slope)
{
    return slope.m_x*m_x + slope.m_y*m_y + slope.m_z * m_z;
}

DOUBLE CCoord::DotProduct2D (CVector &slope)
{
    return slope.m_dx*m_x + slope.m_dy*m_y ;
}

DOUBLE CCoord::DotProduct2D (CCoord &slope)
{
    return slope.m_x*m_x + slope.m_y*m_y ;
}

DOUBLE CCoord::PerpDotProduct2D (CVector &slope)
{
    return slope.m_dy*m_x - slope.m_dx*m_y ;
}

DOUBLE CCoord::PerpDotProduct2D (CCoord &slope)
{
    return slope.m_y*m_x - slope.m_x*m_y ;
}


int CCoord::ScreenDistSqr(CPoint &p)
{
	int rval ;
	int dx ;
	int dy ;

	dx = p.x - m_sx ;
	dy = p.y - m_sy ;

	rval = dx*dx + dy*dy ;
	return rval ;
}

bool CCoord::IsEmpty()
{
	if (m_x==0.0 && m_y==0.0 && m_z==0.0)
		return true ;
	else
		return false ;
}

CCoord::CCoord (DOUBLE x, DOUBLE y, DOUBLE z) 
{
    m_x = x ;
    m_y = y ;
    m_z = z ;
	m_nData = 255 ;
}

CPoint CCoord::MakePoint()
{
	return CPoint((int)m_x, (int)m_y) ;
}

CCoord::CCoord (DOUBLE *comp, int numcomp)
{
	switch (numcomp)
	{
	case 2:
		m_x = comp[0] ;
		m_y = comp[1] ;
		m_z = 0 ;
		break; 

	case 3:
		m_x = comp[0] ;
		m_y = comp[1] ;
		m_z = comp[2] ;
		break; 

	default:
		m_x = m_y = m_z = 0.0 ;
		break;
	}
}

CCoord::CCoord (const CCoord &copy) 
{
	m_x = copy.m_x ;
	m_y = copy.m_y ;
	m_z = copy.m_z ;
	m_sx = copy.m_sx ;
	m_sy = copy.m_sy ;
	m_nData = copy.m_nData ;
}

void CCoord::Clear()
{
	m_x = m_y = m_z = 0.0 ;
	m_nData = 255 ;
}

CCoord::CCoord(const CVector& copy)
{
    m_x = copy.m_dx ;
    m_y = copy.m_dy ;
    m_z = copy.m_dz ;
	m_nData = 255 ;
}

void CCoord::Round(DOUBLE precision)
{
	m_x = (DOUBLE)floor(m_x / precision + (DOUBLE)0.5) * precision ;
	m_y = (DOUBLE)floor(m_y / precision + (DOUBLE)0.5) * precision ;
	m_z = (DOUBLE)floor(m_z / precision + (DOUBLE)0.5) * precision ;

	if (fabs(m_x) < precision)
		m_x = 0 ;
	if (fabs(m_y) < precision)
		m_y = 0 ;
	if (fabs(m_z) < precision)
		m_z = 0 ;
}

void CCoord::Set(DOUBLE x, DOUBLE y, DOUBLE z, long nVal)
{
    m_x = x ; 
    m_y = y ;
    m_z = z ;
	m_nData = nVal ;
}

void CCoord::Set(DOUBLE *comp, int numcomp)
{
	unsigned char *ucomp = (unsigned char *)(comp + 3);
	switch (numcomp)
	{
	case 2:
		m_x = comp[0] ;
		m_y = comp[1] ;
		m_z = 0 ;
		break; 

	case 3:
		m_x = comp[0] ;
		m_y = comp[1] ;
		m_z = comp[2] ;
		break; 

	case 4:
		m_x = comp[0] ;
		m_y = comp[1] ;
		m_z = comp[2] ;
		m_nData = ucomp[0] ;
		break; 

	default:
		m_x = m_y = m_z = 0.0 ;
		break;
	}
}

//void AFXAPI ConstructElements (CCoord*, int)
//{
//}

void CCoord::ToPoint(LPPOINT lppt)
{
    lppt->x = (int)floor(m_x + 0.5) ;
    lppt->y = (int)floor(m_y + 0.5) ;
}

CCoord& CCoord::operator=(const CCoord& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	m_nData = copy.m_nData ;
	m_sx = copy.m_sx ;
	m_sy = copy.m_sy ;
	return *this;
}

CCoord& CCoord::operator=(const CQuaternion& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	m_nData = 255 ;
	return *this;
}

void CCoord::Offset(DOUBLE x, DOUBLE y, DOUBLE z)
{
    m_x += x ;
    m_y += y ;
    m_z += z ;
}

// Move the point a distance in the direction of slope
void CCoord::Offset(CVector &slope, DOUBLE distance)
{
    // The slopes are normalized!
    m_x += distance * slope.m_dx ;
    m_y += distance * slope.m_dy ;
    m_z += distance * slope.m_dz ;
}

CCoord::operator CVector () 
{
	return CVector(m_x, m_y, m_z);
}

CCoord::operator CPoint () 
{
	return CPoint((int)m_x, (int)m_y);
}

bool CCoord::operator==(CCoord &comp) const
{
    return ((comp.m_x == m_x) && (comp.m_y == m_y) && (comp.m_z == m_z)) ;
}

bool CCoord::operator==(CVector &comp) const
{
    return ((comp.m_dx == m_x) && (comp.m_dy == m_y) && (comp.m_dz == m_z)) ;
}

bool CCoord::operator!=(CCoord &comp) const
{
    return ((comp.m_x != m_x) || (comp.m_y != m_y) || (comp.m_z != m_z)) ;
}

void CCoord::operator+=(const CCoord &add)
{
    m_x += add.m_x ;
    m_y += add.m_y ;
    m_z += add.m_z ;
}

void CCoord::operator+=(const CVector &add)
{
    m_x += add.m_dx ;
    m_y += add.m_dy ;
    m_z += add.m_dz ;
}

void CCoord::operator-=(const CCoord &minus)
{
    m_x -= minus.m_x ;
    m_y -= minus.m_y ;
    m_z -= minus.m_z ;
}

void CCoord::operator-=(const CVector &minus)
{
    m_x -= minus.m_dx ;
    m_y -= minus.m_dy ;
    m_z -= minus.m_dz ;
}

CCoord CCoord::operator+(const CCoord &add) const
{
    return CCoord(m_x+add.m_x, m_y+add.m_y, m_z+add.m_z) ;
}

CCoord CCoord::operator+(const CVector &add) const
{
	return CCoord(m_x + add.m_dx, m_y + add.m_dy, m_z + add.m_dz) ;
}

CCoord CCoord::operator-(const CCoord &minus) const
{
	return CCoord(m_x - minus.m_x, m_y - minus.m_y, m_z - minus.m_z) ;
}

CCoord CCoord::Average(CCoord &pt)
{
	return CCoord((m_x+pt.m_x)/(DOUBLE)2.0, (m_y+pt.m_y)/(DOUBLE)2.0, (m_z+pt.m_z)/(DOUBLE)2.0) ;
}

CCoord CCoord::operator-(const CVector &minus) const
{
	return CCoord(m_x - minus.m_dx, m_y - minus.m_dy, m_z - minus.m_dz) ;
}

CCoord CCoord::operator *(DOUBLE Magnitude) 
{
	return CCoord(m_x * Magnitude, m_y * Magnitude, m_z * Magnitude) ;
}

CCoord CCoord::operator/(DOUBLE Magnitude) 
{
	return CCoord(m_x / Magnitude, m_y / Magnitude, m_z / Magnitude) ;
}

CCoord CCoord::operator-() 
{
	return CCoord(-m_x, -m_y, -m_z);
}

DOUBLE CCoord::Distance (const CCoord &second)
{
    DOUBLE dx, dy, dz ;
    dx = second.m_x - m_x ;
    dy = second.m_y - m_y ;
    dz = second.m_z - m_z ;
    return (DOUBLE)sqrt (dx*dx + dy*dy + dz*dz) ;
}

DOUBLE *CCoord::GetVal()
{
	return &m_x ;
}

void CCoord::Trace()
{
#ifdef _DEBUG 
#ifndef _tracer_dll_h
	//		 ,m_x, m_y, m_z) ;
#endif
#endif
}


bool CQuaternion::Rotate(DOUBLE *pvec, int nNumElem, int nStepSize, bool bReverse)
{
	CQuaternion q ;
	CCoord      c ;
	int			i ;

	for (i=0; i<nNumElem; i++,pvec+=nStepSize)
	{
		if (bReverse)
			m_w = -m_w ;

		c = *this ^ pvec ^ ~(*this) ;

		if (bReverse)
			m_w = -m_w ;

		pvec[0] = c.m_x ;
		pvec[1] = c.m_y ;
		pvec[2] = c.m_z ;
	}

	return true ;
}

CCoord &CCoord::Rotate(CQuaternion &q, bool Reverse)
{
	if (Reverse)
		q.m_w = -q.m_w ;

	*this = q ^ *this ^ ~q ;

	if (Reverse)
		q.m_w = -q.m_w ;

	return *this;
}

CCoord &CCoord::Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse)
{
	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	*this = q ^ *this ^ qInv ;

	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	return *this;
}



// The following are class member definitions for slopes

CVector::CVector() 
{
    m_dx = m_dy = m_dz = 0.0 ;
}

CVector::CVector(DOUBLE dx, DOUBLE dy, DOUBLE dz) 
{
    m_dx = dx ;
    m_dy = dy ;
    m_dz = dz ;
}

CVector::CVector(DOUBLE *deltas) 
{
    m_dx = deltas[0] ;
    m_dy = deltas[1] ;
    m_dz = deltas[2] ;
}

void CVector::Invert()
{
    m_dx = -m_dx ;
    m_dy = -m_dy ;
    m_dz = -m_dz ;
}

CVector::CVector(const CVector &copy) 
{
    m_dx = copy.m_dx ;
    m_dy = copy.m_dy ;
    m_dz = copy.m_dz ;
}

CVector::CVector(const CCoord &copy) 
{
    m_dx = copy.m_x ;
    m_dy = copy.m_y ;
    m_dz = copy.m_z ;
}

CVector::CVector(DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2) 
{
    m_dx = x2 - x1 ;
    m_dy = y2 - y1 ;
    m_dz = z2 - z1 ;
}

CVector::CVector(CLine &copy) 
{
    CVector v = copy.Slope();
    *this = v;
}

CVector::CVector(const CCoord &c1, const CCoord &c2, bool Normalize/*=true*/) 
{
    Set(c1, c2, Normalize) ;
}

CVector & CVector::operator=(const CVector& copy)
{
    m_dx = copy.m_dx ;
    m_dy = copy.m_dy ;
    m_dz = copy.m_dz ;
	return *this;
}

CVector& CVector::operator=(const CQuaternion& copy)
{
    m_dx = copy.m_x ;
    m_dy = copy.m_y ;
    m_dz = copy.m_z ;
	return *this;
}

// This method simply scales the vector components to the
// desired magnitude.  If you want a vector with the same
// length as magnitude, you must first, normalize the
// vector.
CVector CVector::operator *(DOUBLE Magnitude)
{
	return CVector(m_dx * Magnitude, m_dy * Magnitude, m_dz * Magnitude) ;
}

CVector CVector::operator/(DOUBLE Magnitude) 
{
	return CVector(m_dx / Magnitude, m_dy / Magnitude, m_dz / Magnitude) ;
}


DOUBLE CVector::Magnitude()
{
	return (DOUBLE)sqrt(m_dx*m_dx + m_dy*m_dy + m_dz*m_dz) ;
}

// Using the delta version causes normalizatoin to occur
void CVector::Set(DOUBLE dx, DOUBLE dy, DOUBLE dz) 
{
    m_dx = dx ;
    m_dy = dy ;
    m_dz = dz ;
    Normalize() ;
}

void CVector::Set(const CVector &copy) 
{
    *this = copy ;
}
// Using the coordinate version does not cause normalizatoin to occur
void CVector::Set(DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2) 
{
    m_dx = x2 - x1 ;
    m_dy = y2 - y1 ;
    m_dz = z2 - z1 ;
}

void CVector::Set(CLine &copy)
{
    Set(copy.Slope()) ;
}
// Using the coordinate version does not cause normalizatoin to occur
void CVector::Set(const CCoord &c1, const CCoord &c2, bool bNormalize/*=true*/) 
{
    m_dx = c2.m_x - c1.m_x ;
    m_dy = c2.m_y - c1.m_y ;
    m_dz = c2.m_z - c1.m_z ;
	if (bNormalize)
		Normalize() ;
}

void CVector::SetNormal2D(const CCoord &c1, const CCoord &c2, bool bNormalize/*=true*/) 
{
    m_dx = c1.m_y - c2.m_y ;
    m_dy = c2.m_x - c1.m_x ;
	m_dz = 0 ;
	//if (bNormalize)
		Normalize() ;
}

void CVector::Set(DOUBLE *comp, int numcomp)
{
	switch (numcomp)
	{
	case 2:
		m_dx = comp[0] ;
		m_dy = comp[1] ;
		m_dz = 0 ;
		break; 

	case 3:
		m_dx = comp[0] ;
		m_dy = comp[1] ;
		m_dz = comp[2] ;
		break; 

	default:
		m_dx = m_dy = m_dz = 0.0 ;
		break;
	}
}

void CVector::Clear()
{
	m_dx = m_dy = m_dz = 0.0 ;
}

void CVector::Normalize() 
{
    DOUBLE d ;
    d = (DOUBLE)sqrt(m_dx*m_dx + m_dy*m_dy + m_dz*m_dz) ;
	if (d != (DOUBLE)0.0)
	{
		m_dx = m_dx / d ;
		m_dy = m_dy / d ;
		m_dz = m_dz / d ;
	}
}


CVector &CVector::operator=(CLine& copy)
{
    *this = copy.Slope();
	return *this;
}

CVector &CVector::operator=(const CCoord& copy)
{
    m_dx = copy.m_x ;
    m_dy = copy.m_y ;
    m_dz = copy.m_z ;
	return *this;
}

bool CVector::operator==(CVector &comp) const
{
/*
    if (comp.IsVertical() && IsVertical())
        return true ;
    return (m_dy/m_dx == comp.m_dy/comp.m_dx) ;
    */

    // assume we're normalized
    if (   comp.m_dx == m_dx 
        && comp.m_dy == m_dy 
        && comp.m_dz == m_dz 
        )
        return true ;
    else
        return false ;
}


bool CVector::IsDefined() 
{
    return (m_dx != 0.0) ;
}

bool CVector::IsVertical() 
{
    return (m_dx == 0.0) ;
}

CVector::operator DOUBLE()
{
    return m_dy / m_dx ;
}

// Since our slopes are normalized when they are
// set, our dot product will always have a length 
// of 1 (i.e. dx^2 + dy^2 = 1).  The return value
// is cos(x) of the angle between slope 1 and
// slope 2
DOUBLE CVector::DotProduct (const CVector &slope)
{
    return slope.m_dx*m_dx + slope.m_dy*m_dy + slope.m_dz * m_dz;
}

DOUBLE CVector::DotProduct (CCoord &slope)
{
    return slope.m_x*m_dx + slope.m_y*m_dy + slope.m_z * m_dz;
}

DOUBLE CVector::DotProduct2D (CVector &slope)
{
    return slope.m_dx*m_dx + slope.m_dy*m_dy ;
}

DOUBLE CVector::DotProduct2D (CCoord &slope)
{
    return slope.m_x*m_dx + slope.m_y*m_dy ;
}

DOUBLE CVector::PerpDotProduct2D (CVector &slope)
{
    return slope.m_dy*m_dx - slope.m_dx*m_dy ;
}

DOUBLE CVector::PerpDotProduct2D (CCoord &slope)
{
    return slope.m_y*m_dx - slope.m_x*m_dy ;
}

bool CVector::CrossProduct (const CVector &a, const CVector &b, bool Normalize)
{
    DOUBLE length ;

    // First, calculate the vector components
    m_dx = a.m_dy * b.m_dz - a.m_dz * b.m_dy ;
    m_dy = a.m_dz * b.m_dx - a.m_dx * b.m_dz ;
    m_dz = a.m_dx * b.m_dy - a.m_dy * b.m_dx ;

    // Next, normalize the components
	if (Normalize)
	{
		if (m_dx!=0.0 || m_dy!=0.0 || m_dz!=0.0)
		{
			length = (DOUBLE)sqrt(m_dx*m_dx + m_dy*m_dy + m_dz*m_dz);
			m_dx /= length ;
			m_dy /= length ;
			m_dz /= length ;
		}
		else
			return false ;
	}

    return true ;
}

DOUBLE *CVector::GetVal()
{
    return &m_dx;
}

CVector & CVector::operator+(const CVector &addend) 
{
	m_dx += addend.m_dx ;
	m_dy += addend.m_dy ;
	m_dz += addend.m_dz ;
	return *this ;
}

CVector & CVector::operator-(const CVector &diffend) 
{
	m_dx -= diffend.m_dx ;
	m_dy -= diffend.m_dy ;
	m_dz -= diffend.m_dz ;
	return *this ;
}

CVector CVector::operator-() 
{
	CVector rval ;
	rval.m_dx = -m_dx ;
	rval.m_dy = -m_dy ;
	rval.m_dz = -m_dz ;
	return rval ;
}

void CVector::operator+=(const CVector &addend) 
{
	m_dx += addend.m_dx ;
	m_dy += addend.m_dy ;
	m_dz += addend.m_dz ;
}

void CVector::operator*=(DOUBLE Magnitude) 
{
	m_dx *= Magnitude;
	m_dy *= Magnitude;
	m_dz *= Magnitude;
}

void CCoord::operator*=(DOUBLE Magnitude) 
{
	m_x *= Magnitude;
	m_y *= Magnitude;
	m_z *= Magnitude;
}

void CCoord::operator/=(DOUBLE Magnitude) 
{
	m_x /= Magnitude;
	m_y /= Magnitude;
	m_z /= Magnitude;
}

void CVector::operator-=(const CVector &diffend) 
{
	m_dx -= diffend.m_dx ;
	m_dy -= diffend.m_dy ;
	m_dz -= diffend.m_dz ;
}

void CVector::Trace()
{
#ifdef _DEBUG
#ifndef _tracer_dll_h
	//		 ,m_dx, m_dy, m_dz) ;
#endif
#endif
}

CVector &CVector::Rotate(CQuaternion &q, bool Reverse)
{
	if (Reverse)
		q.m_w = -q.m_w ;

	*this = q ^ *this ^ ~q ;

	if (Reverse)
		q.m_w = -q.m_w ;

	return *this;
}

CVector &CVector::Rotate(CQuaternion &q, CQuaternion &qInv, bool Reverse)
{
	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	*this = q ^ *this ^ qInv ;

	if (Reverse)
	{
		q.m_w = -q.m_w ;
		qInv.m_w = -qInv.m_w ;
	}

	return *this;
}




// The following are class member definitions for lines
CLine::CLine () 
{
	m_pPrevious = NULL ;
    Set(0,0,0,0,0,0) ;
}

CLine::CLine (CVector &slope, CCoord &intercept) 
{
	m_pPrevious = NULL ;
    Set(slope,intercept) ;
}

CLine::CLine (CCoord &intercept,CVector &slope) 
{
	m_pPrevious = NULL ;
    Set(slope,intercept) ;
}

CLine::CLine (DOUBLE x1, DOUBLE y1, DOUBLE z1, DOUBLE x2, DOUBLE y2, DOUBLE z2 ) 
{
	m_pPrevious = NULL ;
    Set (x1, y1, z1, x2, y2,z2) ;
}

CLine::CLine (CCoord &first, CCoord &second) 
{
	m_pPrevious = NULL ;
    Set(first.m_x,first.m_y,first.m_z,second.m_x,second.m_y,second.m_z) ;
}

CLine::CLine (const CLine &copy)
{
    *this = copy ;
}

CLine &CLine::operator=(const CLine& copy)
{
    m_first = copy.m_first ;
    m_second = copy.m_second ;
	return *this;
}

bool CLine::operator==(CLine &comp) const
{
    return (
			  ((m_first == comp.m_first) && (m_second == comp.m_second))
		  || ((m_first == comp.m_second) && (m_second == comp.m_first)) 
		    ) ;
}



void CLine::Set (CVector &slope, CCoord &intercept) 
{
    m_first = intercept ;
    m_second = m_first + slope ;
}

void CLine::Set (CCoord &intercept, CVector &slope) 
{
    m_first = intercept ;
    m_second = m_first + slope ;
}

void CLine::Set (DOUBLE x1, DOUBLE y1, DOUBLE z1,DOUBLE x2, DOUBLE y2, DOUBLE z2) 
{
    m_first.Set(x1,y1,z1) ;
    m_second.Set(x2,y2,z2) ;
}

CLine::CLine (DOUBLE *p1, DOUBLE *p2) 
{
	m_first.m_x = p1[0] ;
	m_first.m_y = p1[1] ;
	m_first.m_z = p1[2] ;

	m_second.m_x = p2[0] ;
	m_second.m_y = p2[1] ;
	m_second.m_z = p2[2] ;
}

void CLine::Set (CCoord &first, CCoord &second) 
{
    m_first = first ;
    m_second = second ;
}

DOUBLE CLine::Length()
{
	return m_first.Distance(m_second) ;
}

CVector CLine::CrossProduct(CCoord &OuterCoord, bool Normalize)
{
	CVector rval, coordvec(m_first, OuterCoord) ;
	rval.CrossProduct(Slope(), coordvec, Normalize) ;
	return rval;
}

bool CLine::Intersect2 (CLine &iline, CCoord &rval, bool *pSkewOrParallel)
{
	CVector a(m_first, m_second, false) ;
	CVector b(iline.m_first, iline.m_second, false) ;
	CVector c(m_first, iline.m_first, false) ;
	CVector axb, cxb ;
	
	if (!axb.CrossProduct(a,b))
		return false ;

	if (c.DotProduct(axb)!=0)
		return false ;

	cxb.CrossProduct(c,b) ;
	rval = m_first + a * (cxb.DotProduct(axb) / (DOUBLE)pow(axb.Magnitude(), (DOUBLE)2.0)) ;
	return true ;
}

bool CLine::Intersect (CLine &iline, CCoord &rval, bool *pParallel) 
{
//#ifndef FASTTRANSFORM
//    return false ;
//#endif
	// First, are they coplanar?
	
	CVector SR ;
	CVector N ;
	DOUBLE dist ;
	DOUBLE t ;
	DOUBLE u ;
	CVector D ;
	CVector E ;
	CCoord  R ;
	CCoord  S ;

    S = m_first ;
    R = iline.m_first ;
	D = Slope() ;
	E = iline.Slope() ;
	
	// If the two lines are colinear or parallel, we'll get a false
	if (!N.CrossProduct(D, E))
		return false ;
	
	if (N.Magnitude()==0.0)
	{
		if (pParallel)
			*pParallel = true ;
	    return false ;
	}

	if (pParallel)
		*pParallel = false ;
	SR = R - S ;

	// Now, figure out the shortest distance using dot product
	// of the projected line segment and the vector of the
	// shortest line segment.
	dist = SR.DotProduct(N) ;

	// If the distance is not zero, the lines do not intersect
	if (dist>0.0)
	    return false ;

	// Finally, use cramer's algorithm to determine the parametric
	// unit for intersection.
	
	if (   fabs(N.m_dz) > fabs(N.m_dx) 
		&& fabs(N.m_dz) > fabs(N.m_dy) 
		)
	{
		t =  ( SR.m_dx * E.m_dy - SR.m_dy * E.m_dx ) / N.m_dz ;
		u = -( SR.m_dx * D.m_dy - SR.m_dy * D.m_dx ) / N.m_dz ;
	}
	else
	if ( fabs(N.m_dx) > fabs(N.m_dy ) )
	{
		t =  ( SR.m_dy * E.m_dz - SR.m_dz * E.m_dy ) / N.m_dx ;
		u = -( SR.m_dy * D.m_dz - SR.m_dz * D.m_dy ) / N.m_dx ;
	}
	else
	{
		t =  ( SR.m_dz * E.m_dx - SR.m_dx * E.m_dz ) / N.m_dy ;
		u = -( SR.m_dz * D.m_dx - SR.m_dx * D.m_dz ) / N.m_dy ;
	}

	rval = S + D*t ;

    return true ;
//	return false ;
}

CLine CLine::ReverseVector()
{
	return CLine(m_second, m_first) ;
}

CLine CLine::Project(CPlane &ProjectionPlane)
{
	CCoord f, s ;

	f = ProjectionPlane.NearestPointInPlane(m_first) ;
	s = ProjectionPlane.NearestPointInPlane(m_second) ;
	return CLine(f,s) ;
}

CCoord CLine::XPlaneIntersect(DOUBLE x) 
{
    DOUBLE scale ;
    CCoord rval ;

    rval.m_x = x ;

    // Our line cannot be in the x plane!
    if (m_first.m_x == m_second.m_x)
        return rval ;

    scale = (x - m_first.m_x) / (m_second.m_x - m_first.m_x) ;

    rval.m_y = m_first.m_y + (m_second.m_y - m_first.m_y) * scale ;
    rval.m_z = m_first.m_z + (m_second.m_z - m_first.m_z) * scale ;
    return rval ;
}

CCoord CLine::YPlaneIntersect(DOUBLE y) 
{
    DOUBLE scale ;
    CCoord rval ;

    rval.m_y = y ;

    // Our line cannot be in the x plane!
    if (m_first.m_y == m_second.m_y)
        return rval ;

    scale = (y - m_first.m_y) / (m_second.m_y - m_first.m_y) ;

    rval.m_x = m_first.m_x + (m_second.m_x - m_first.m_x) * scale ;
    rval.m_z = m_first.m_z + (m_second.m_z - m_first.m_z) * scale ;
    return rval ;
}

CCoord CLine::ZPlaneIntersect(DOUBLE z) 
{
    DOUBLE scale ;
    CCoord rval ;

    rval.m_z = z ;

    // Our line cannot be in the x plane!
    if (m_first.m_z == m_second.m_z)
        return rval ;

    scale = (z - m_first.m_z) / (m_second.m_z - m_first.m_z) ;

    rval.m_x = m_first.m_x + (m_second.m_x - m_first.m_x) * scale ;
    rval.m_y = m_first.m_y + (m_second.m_y - m_first.m_y) * scale ;
    return rval ;
}

CCoord CLine::CoordAtY(DOUBLE y, bool *pUndefined)
{
	// Need to convert to parametric first!
	CCoord rval ;
	CVector slope = Slope(true) ;

	if (slope.m_dy==0.0)
	{
		if (pUndefined)
			*pUndefined = true ;
		return rval ;
	}

	DOUBLE scalar ;

	scalar = (y - m_first.m_y) / slope.m_dy ;
	rval = m_first + slope * scalar;
	if (pUndefined)
		*pUndefined = false ;
    return rval ;
}

CCoord CLine::CoordAtX(DOUBLE x, bool *pUndefined)
{
	// Need to convert to parametric first!
	CCoord rval ;
	CCoord addval ;
	CVector slope = Slope(true) ;

	if (slope.m_dx==0.0)
	{
		if (pUndefined)
			*pUndefined = true ;
		return rval ;
	}

	DOUBLE scalar ;

	slope.Normalize() ;

	scalar = (x - m_first.m_x) / slope.m_dx ;
	addval = (slope * scalar) ;

	rval = m_first + addval;
	if (pUndefined)
		*pUndefined = false ;
    return rval ;
}

CCoord CLine::CoordAtZ(DOUBLE z, bool *pUndefined)
{
	// Need to convert to parametric first!
	CCoord rval ;
	CVector slope = Slope(true) ;

	if (slope.m_dz==0.0)
	{
		if (pUndefined)
			*pUndefined = true ;
		return rval ;
	}

	DOUBLE scalar ;

	slope.Normalize() ;

	scalar = (z - m_first.m_z) / slope.m_dz ;
	rval = m_first + slope * scalar;
	if (pUndefined)
		*pUndefined = false ;
    return rval ;
}

CCoord CLine::MidPoint()
{
	CCoord rval ;

	rval = m_first + m_second ;
	rval = rval / 2.0 ;
	return rval ;
}

bool CLine::IsVertical()
{
    return Slope().IsVertical() ;
}

DOUBLE CLine::XYDistance (CCoord &coord) 
{
	CVector slope = Slope() ;
	CVector OrthVector(slope.m_dy, -slope.m_dx, 0.0) ;
	CLine   OrthLine(coord, OrthVector) ;
	CCoord  IntersectingCoord ;

	bool rval = Intersect(OrthLine, IntersectingCoord);
		
    return coord.Distance(IntersectingCoord);
}

CCoord CLine::Nearest(CCoord &coord, bool Normalize)
{
	CVector slope = Slope() ;
	CCoord  rval ;
	
	// If the slope needs to be normalized before the calculation,
	// do it now.
	if (Normalize)
		slope.Normalize() ;
	
	// Set the line to point vector, but don't normalize it.
	CVector w = coord - m_first ;

	DOUBLE hypotenuse = w.Magnitude() ;
	w.Normalize() ;
	DOUBLE cosofangle = w.DotProduct(slope) ;
	DOUBLE offsetdist = cosofangle * hypotenuse ;

	rval = m_first + slope * offsetdist ;

	return rval ;
}

CCoord CLine::Nearest(CCoordArray &cArray, bool Normalize)
{
    // To find the distance from a line, we use the 
	// the fact that the vectors of the point and
	// the line make a parallelogram.  
	CVector slope = Slope() ;
	int     i ;
	CVector d ;
	DOUBLE  dNearest = (DOUBLE)1e30 ;
	DOUBLE  dDist ;
	CCoord  rNearest ;

	rNearest.Clear() ;
	
	// If the slope needs to be normalized before the calculation,
	// do it now.
	if (Normalize)
		slope.Normalize() ;
	
	CCoord *pArray = cArray.GetData() ;

	for (i=0; i<cArray.GetSize(); i++)
	{
		// Set the line to point vector, but don't normalize it.
		CVector w(m_first, pArray[i], false) ;

		// Now, do our cross product
		d.CrossProduct(slope, w, false) ;

		dDist = d.Magnitude () ;

		if (dDist < dNearest)
		{
			dNearest = dDist ;
			rNearest = pArray[i] ;
		}
	}

    return rNearest ;
}

DOUBLE CLine::Distance (CCoord &coord, bool Normalize/*=true*/) 
{
    // To find the distance from a line, we use the 
	// the fact that the vectors of the point and
	// the line make a parallelogram.  
	CVector slope = Slope() ;
	
	// If the slope needs to be normalized before the calculation,
	// do it now.
	if (Normalize)
		slope.Normalize() ;

	// Set the line to point vector, but don't normalize it.
	CVector w(m_first, coord, false) ;

	// Now, do our cross product
	CVector d ;
	d.CrossProduct(slope, w, false) ;

    return d.Magnitude() ;
}

bool CLine::IsParallel(CLine &comp) 
{
    return (Slope() == comp.Slope()) ;
}

bool CLine::IsOrthogonal(CLine &) 
{
    // Not implemented
    return false ;
}

bool CLine::IsAdjoining(CLine &TestLine) 
{
	if (	TestLine.m_first == m_first
		 || TestLine.m_second == m_first
		 || TestLine.m_first == m_second
		 || TestLine.m_second == m_second )
        return true ;
	else 
		return false ;
}

CVector CLine::Slope(bool Normalize)
{
	return CVector(m_first, m_second, Normalize) ;
}

DOUBLE CLine::DotProduct(CVector &line)
{
    return Slope().DotProduct(line) ;
}

DOUBLE CLine::DotProduct(CCoord &line)
{
    return Slope().DotProduct(line) ;
}


CIntCoord::CIntCoord() 
{
	m_x = m_y = m_z /*= c*/ = 0 ;
}

CIntCoord::CIntCoord(CIntCoord &copy) 
{
	*this = copy ;
}

CIntCoord::~CIntCoord() 
{
}

CIntCoord & CIntCoord::operator=(CIntCoord &copy) 
{
	m_x = copy.m_x ;
	m_y = copy.m_y ;
	m_z = copy.m_z ;
	//c = copy.c ;
	return *this ;
}

CIntCoord & CIntCoord::operator=(CCoord &copy) 
{
	m_x = (int) floor (copy.m_x + 0.5) ;
	m_y = (int) floor (copy.m_y + 0.5) ;
	m_z = (int) floor (copy.m_z + 0.5) ;
	//c = (int) floor (copy.c + 0.5) ;
	return *this ;
}

bool CVector::SignsSameAs(CVector &comp)
{
	// If any of the signs are different, we
	// will get a result that is less than
	// zero when multiplying the components.
	if (m_dx * comp.m_dx < 0 ||
		m_dy * comp.m_dy < 0 ||
		m_dz * comp.m_dz < 0)
		return false ;
	else
		return true ;
}

/*

// Rational Coordinate Section
CRationalCoord::CRationalCoord() 
{
	Clear() ;
}

void CRationalCoord::Clear()
{
	CCoord::Clear() ;
	m_h = 0.0 ;
}

CRationalCoord::CRationalCoord (const CRationalCoord &copy)
{
    *this = copy ;
}

CRationalCoord::CRationalCoord (DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE h)
{
    m_x = x ;
    m_y = y ;
    m_z = z ;
	m_h = h ;
}

CRationalCoord::CRationalCoord  ( const CCoord &copy
                                , DOUBLE h //=1.0
                                  )
{
	m_x = copy.m_x ;
	m_y = copy.m_y ;
	m_z = copy.m_z ;
	m_h = h ;
}

CRationalCoord& CRationalCoord::operator=(const CRationalCoord& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	m_h = copy.m_h ;
	return *this ;
}

CRationalCoord& CRationalCoord::operator=(const CCoord& copy)
{
    m_x = copy.m_x ;
    m_y = copy.m_y ;
    m_z = copy.m_z ;
	return *this ;
}

void CRationalCoord::Set(DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE h)
{
    m_x = x ;
    m_y = y ;
    m_z = z ;
	m_h = h ;
}
*/

void CLine::FlipOrder()
{
	CCoord t = m_first ;
	m_first = m_second ;
	m_second = t ;
}

bool CLineArray::NearestX ( DOUBLE x, CCoord &cval )
{
	bool   rval = false;
	int    i ;
	DOUBLE dNearest = 1e10 ;
	DOUBLE dDist ;

	for (i=0; i<GetSize(); i++)
	{
		CLine &L = ElementAt(i) ;
		dDist = (DOUBLE)fabs(L.m_first.m_x - x) ;
		if (dDist < dNearest)
		{
			cval = L.m_first ;
			dNearest = dDist ;
			rval = true ;
		}
		dDist = (DOUBLE)fabs(L.m_second.m_x - x) ;
		if (dDist < dNearest)
		{
			cval = L.m_second ;
			dNearest = dDist ;
			rval = true ;
		}
	}
	return rval ;
}

void CLineArray::FlipOrder()
{
	int i ;
	CLine t ;
	int   nNumElem = GetSize();
	for (i=0; i<nNumElem/2; i++)
	{
		t = ElementAt(i) ;
		ElementAt(i) = ElementAt(nNumElem-i-1) ;
		ElementAt(nNumElem-i-1) = t ;

		ElementAt(i).FlipOrder() ;
		ElementAt(nNumElem-i-1).FlipOrder() ;
	}
}

DOUBLE CLineArray::Nearest ( CCoord &testcoord)
{
	int i ;
	DOUBLE dNearest = DOUBLE_MAX;
	DOUBLE dDist ;

	for (i=0; i<GetSize(); i++)
	{
		dDist = testcoord.Distance ( ElementAt(i).m_first ) ;
		if (dDist < dNearest)
			dNearest = dDist ;

		dDist = testcoord.Distance ( ElementAt(i).m_second ) ;
		if (dDist < dNearest)
			dNearest = dDist ;
	}

	return dNearest ;
}

void CLineArray::JoinSort()
{
	CLineArray resulttop ;
	CLineArray resultbottom ;
	CByteArray flagged ;
	CLine	   line ;
	CLine	   *pLines ;
	bool	   bFound ;
	int nFlaggedCnt = 0;
	int i,j ;

	if (GetSize()<=1)
		return ;

	// Set our picked flags so we can reduce the number
	// of memory operations
	flagged.SetSize(GetSize()) ;
	memset (flagged.GetData(), 0, flagged.GetSize()) ;

	// This problem is akin to building a doubley linked
	// list but not knowing the pointers....  The trick
	// is that the start and end elements have no 
	// links (i.e. NULL).  In this case they have an
	// unmatched coordinate.  We don't know if we're 
	// starting at the top or the bottom, so let's 
	// travel in the top direction first.
	line = ElementAt(0) ;
	resulttop.Add (line) ;
	flagged[0] = 1 ;
	nFlaggedCnt++ ;

	// Every time we find an element, make sure we flag it.
	for (j=0; j<GetSize(); j++)
	{
		bFound = false ;
		for (i=1; i<GetSize(); i++)
		{
			CLine &compline = ElementAt(i) ;

			if (flagged[i]==0 && line.Joined(compline))
			{
				resulttop.Add (compline) ;
				flagged[i] = 1 ;
				line = compline ;
				nFlaggedCnt++ ;
				bFound = true ;
				break;
			}
		}

		// If we didn't find any, let's break out and start 
		// doing the bottom
		if (!bFound)
			break;
	}

	line = resulttop[0] ;

	for (j=0; j<GetSize(); j++)
	{
		bFound = false ;
		for (i=1; i<GetSize(); i++)
		{
			CLine &compline = ElementAt(i) ;
			if (flagged[i]==0 && line.Joined(compline))
			{
				resultbottom.Add (compline) ;
				flagged[i] = 1 ;
				line = compline ;
				nFlaggedCnt++ ;
				bFound = true ;
				break;
			}
		}

		if (!bFound)
			break;
	}

	pLines = GetData() ;
	if (resultbottom.GetSize())
	{
		for (i=resultbottom.GetSize()-1; i>=0; i--)
		{
			*pLines = resultbottom.ElementAt(i) ;
			//pLines->FlipOrder() ;
			pLines++ ;
		}
	}
	if (resulttop.GetSize())
	{
		for (i=0; i<resulttop.GetSize(); i++)
		{
			*pLines = resulttop.ElementAt(i) ;
			pLines++ ;
		}
	}

	// Now, make sure that all the lines have the
	// correct orientation
	pLines = GetData() ;
	if (pLines[0].m_second != pLines[1].m_first)
	{
		if (pLines[0].m_second.Epsilon(pLines[1].m_second)==false)
			pLines[0].FlipOrder() ;
	}

	for (i=0; i<GetSize()-1; i++)
	{
		if (pLines[i].m_second.Epsilon(pLines[i+1].m_first)==false)
			pLines[i+1].FlipOrder() ;
	}
}

CLineArray &CLineArray::operator=(const CCoordArray &src)
{
	SetSize(src.GetSize()-1) ;

	for (int i=0; i<src.GetSize()-1; i++)
	{
		CLine &L = ElementAt(i) ;
		L.m_first = src[i] ;
		L.m_second = src[i+1] ;
	}

	return *this;
}

CLineArray &CLineArray::operator=(const CLineArray &src)
{
	SetSize(src.GetSize()) ;

	const CLine *psrc = src.GetData() ;
	CLine *pdst = GetData() ;
	
	for (int i=0; i<src.GetSize()-1; i++,psrc++,pdst++)
	{
		pdst->m_first  = psrc->m_first ;
		pdst->m_second = psrc->m_second ;
	}

	return *this;
}





/*
int CoordIntensityCompare ( const CCoordIntensity *arg1, const CCoordIntensity *arg2)
{
    if (arg1->m_intensity < arg2->m_intensity)
        return -1 ;
	if (arg1->m_intensity > arg2->m_intensity)
        return 1 ;
    return 0 ;
}

// This sorts the whole blessed array!
void CCoordIntensityArray::Sort()
{
	int numcoord = GetSize() ;
	if (numcoord > 1)
		qsort (GetData(), numcoord, sizeof(CCoordIntensity), (COORDINTENSITYCOMPAREFUNC)CoordIntensityCompare) ;
}
*/


CPlane::CPlane()
{
	m_origin.Set(0,0,0) ;
	m_normal.Set(1,0,0) ;
//	m_k = 0 ;
}

void CPlane::GenerateUnitVectors()
{
	// Pick an arbitrary reference vector to get the first normal.
	/*
	CVector k, m ;

	k = CVector(1,0,0) ;
	m.CrossProduct(k, m_normal, true) ;

	if (m.Magnitude() < 0.1)
	{
		k = CVector(0,1,0) ;
		m.CrossProduct(k, m_normal, true) ;
		if (m.Magnitude() < 0.1)
		{
			k = CVector(0,0,1) ;
			m.CrossProduct(k, m_normal, true) ;
		}
	}

	m_UnitX = m ;
	m_UnitX.Normalize() ;
	m_UnitY.CrossProduct(m_UnitX, m_normal, true) ;
	*/
}

CCoord CPlane::GridPoint(DOUBLE x, DOUBLE y, DOUBLE z)
{
	CCoord rval ;

//	rval = m_origin + (m_UnitX * x + m_UnitY * y + m_normal * z) ;

	return rval ;
}

CPlane::CPlane(const CCoord &Origin, const CVector &Normal)
{
	Set(Origin, Normal) ;
}

CPlane::CPlane(const CCoord &Origin, const CVector &One, const CVector &Two)
{
	Set(Origin, One, Two) ;
}

CPlane::CPlane(const CCoord &Origin, const CCoord &One, const CCoord &Two)
{
	Set(Origin, One, Two) ;
}

CPlane::CPlane(const CCoord &Origin, const CLine &Line)
{
	Set(Origin, Line.m_first, Line.m_second) ;
}

bool CPlane::IntersectSegments(CCoordArray &segments, CCoordArray &result)
{
	CCoord c ;
	int    i ;
	CCoord *pLine = segments.GetData() ;
	int    nSize = segments.GetSize() ;
	int    nCnt = 0 ;
	CCoord *pCoord ;

	result.SetSize ( nSize ) ;
	pCoord = result.GetData() ;

	// We do not clear the result array, since we may be trying to
	// accumlate segment points...
	for (i=1; i<nSize; i++)
	{
		if (IntersectSegment(pLine[i-1], pLine[i], pCoord[nCnt]))
			nCnt++ ;
	}

	result.SetSize(nCnt) ;
	return (result.GetSize() > 0);
}

CCoord CPlane::IntersectXY(DOUBLE x, DOUBLE y)
{
	CCoord rval ;
	return rval ;
}

CCoord CPlane::IntersectXZ(DOUBLE x, DOUBLE z)
{
	CCoord rval ;
	return rval ;
}

CCoord CPlane::IntersectYZ(DOUBLE y, DOUBLE z)
{
	CCoord rval ;
	return rval ;
}

CCoord spoint[3] ;

#define BUFFERMAXNUMSEGS 1200

bool CPlane::IntersectTriangles ( CDoubleArray &triangles
							 , CLineArray   &la )
{
	DOUBLE *pTriangle ;
	int    sz, oldsz ;
	int    nNumSegs ;
	DOUBLE Segments[BUFFERMAXNUMSEGS*6] ;
	CLine  *pLine ;
	int    i,j ;

	oldsz = la.GetSize() ;
	Normalize() ;
	pTriangle = triangles.GetData() ;
	sz = triangles.GetSize() ;
	nNumSegs = 0 ;
	for (j=0; j<sz; j+=12)
	{
		if (IntersectTriangle (pTriangle+3, Segments+nNumSegs*6))
			nNumSegs++ ;
		if (nNumSegs==BUFFERMAXNUMSEGS)
		{
			la.SetSize(la.GetSize()+nNumSegs) ;
			pLine = la.GetData() + la.GetSize() - nNumSegs ;
			for (i=0; i<nNumSegs; i++)
			{
				pLine->m_first.m_x = Segments[i*6+0] ;
				pLine->m_first.m_y = Segments[i*6+1] ;
				pLine->m_first.m_z = Segments[i*6+2] ;

				pLine->m_second.m_x = Segments[i*6+3] ;
				pLine->m_second.m_y = Segments[i*6+4] ;
				pLine->m_second.m_z = Segments[i*6+5] ;

				pLine++ ;
			}
			nNumSegs = 0 ;
		}
		pTriangle+=12 ;
	}
	if (nNumSegs)
	{
		la.SetSize(la.GetSize()+nNumSegs) ;
		pLine = la.GetData() + la.GetSize() - nNumSegs ;
		for (i=0; i<nNumSegs; i++)
		{
			pLine->m_first.m_x = Segments[i*6+0] ;
			pLine->m_first.m_y = Segments[i*6+1] ;
			pLine->m_first.m_z = Segments[i*6+2] ;

			pLine->m_second.m_x = Segments[i*6+3] ;
			pLine->m_second.m_y = Segments[i*6+4] ;
			pLine->m_second.m_z = Segments[i*6+5] ;

			pLine++ ;
		}
	}
	return true ;
}

bool CPlane::IntersectTriangle ( DOUBLE *pTriangle
							 , DOUBLE *segments
							 , bool bNormalize )
{
	bool firstOnTop, secondOnTop, thirdOnTop ;
	int  nNumPoints ;
	double d1, d2, d3 ;

	// First, are the points on the same side?
	d1 = Distance(pTriangle+0, &firstOnTop, bNormalize) ;
	d2 = Distance(pTriangle+3, &secondOnTop, bNormalize) ;
	d3 = Distance(pTriangle+6, &thirdOnTop, bNormalize) ;

	if (	firstOnTop==secondOnTop
		&& secondOnTop==thirdOnTop )
		return false ;

	nNumPoints = 0 ;
	if (Intersect(pTriangle,pTriangle+3, segments+nNumPoints*3, NULL, true))
		nNumPoints++;
	if (Intersect(pTriangle+3,pTriangle+6, segments+nNumPoints*3, NULL, true))
		nNumPoints++;
	if (Intersect(pTriangle+6,pTriangle, segments+nNumPoints*3, NULL, true))
		nNumPoints++;
	if (nNumPoints==2)
		return true ;
	else
		return false ;
}

bool CPlane::IntersectTriangle(DOUBLE *pTriangle, CLineArray &segments, bool bNormalize)
{
	bool firstOnTop, secondOnTop, thirdOnTop ;
	int  nNumPoints ;
	double d1, d2, d3 ;

	// First, are the points on the same side?
	d1 = Distance(pTriangle+0, &firstOnTop, bNormalize) ;
	d2 = Distance(pTriangle+3, &secondOnTop, bNormalize) ;
	d3 = Distance(pTriangle+6, &thirdOnTop, bNormalize) ;

	if (	firstOnTop==secondOnTop
		&& secondOnTop==thirdOnTop )
		return false ;

	nNumPoints = 0 ;
	if (Intersect(CLine(pTriangle,pTriangle+3), spoint[nNumPoints]))
		nNumPoints++;
	if (Intersect(CLine(pTriangle+3,pTriangle+6), spoint[nNumPoints]))
		nNumPoints++;
	if (Intersect(CLine(pTriangle+6,pTriangle), spoint[nNumPoints]))
		nNumPoints++;
	if (nNumPoints==2)
	{
	    CLine line(spoint[0], spoint[1]);
		segments.Add(line) ;
		return true ;
	}
	else
		return false ;
}

bool CPlane::IntersectTriangle(CCoord &first, CCoord &second, CCoord &third, CLineArray &segments)
{
	bool firstOnTop, secondOnTop, thirdOnTop ;
	static CCoord point[2] ;
	int  nNumPoints ;

	// First, are the points on the same side?
	Distance(first, &firstOnTop, true) ;
	Distance(second, &secondOnTop, true) ;
	Distance(third, &thirdOnTop, true) ;

	if (firstOnTop==secondOnTop==thirdOnTop)
		return false ;

	nNumPoints = 0 ;
	if (Intersect(CLine(first,second), point[nNumPoints]))
		nNumPoints++;
	if (Intersect(CLine(second,third), point[nNumPoints]))
		nNumPoints++;
	if (Intersect(CLine(third,first), point[nNumPoints]))
		nNumPoints++;
	if (nNumPoints==2)
	{

	    CLine line(point[0], point[1]);
		segments.Add(line) ;
		return true ;
	}
	else
		return false ;
}

bool CPlane::IntersectTriangle(CCoord &first, CCoord &second, CCoord &third, CCoordArray &ca)
{
	bool firstOnTop, secondOnTop, thirdOnTop ;
	static CCoord point ;
	int  nNumPoints ;

	// First, are the points on the same side?
	Distance(first, &firstOnTop, false) ;
	Distance(second, &secondOnTop, false) ;
	Distance(third, &thirdOnTop, false) ;

	if (firstOnTop==secondOnTop==thirdOnTop)
		return false ;

	nNumPoints = 0 ;
	CLine linefirst(first, second) ;
	if (Intersect(linefirst, point))
	{
		nNumPoints++;
		ca.Add(point) ;
	}
	
	CLine linesecond(second,third) ;
	if (Intersect(linesecond, point))
	{
		nNumPoints++;
		ca.Add(point) ;
	}
	
	CLine linethird(third,first);
	if (Intersect(linethird, point))
	{
		nNumPoints++;
		ca.Add(point) ;
	}
	if (nNumPoints>0)
		return true ;
	else
		return false ;
}

bool CPlane::IntersectSegment(CCoord &first, CCoord &second, CCoord &point, bool *pParallel)
{
	bool firstOnTop, secondOnTop ;

	// First, are the points on the same side?
	Distance(first, &firstOnTop, false) ;
	Distance(second, &secondOnTop, false) ;

	if (firstOnTop==secondOnTop)
		return false ;

	return Intersect(CLine(first,second), point, pParallel) ;
}

bool CPlane::IntersectSegment(DOUBLE *first, DOUBLE *second, DOUBLE *point, bool *pParallel)
{
	bool firstOnTop, secondOnTop ;

	// First, are the points on the same side?
	Distance(first, &firstOnTop, false) ;
	Distance(second, &secondOnTop, false) ;

	if (firstOnTop==secondOnTop)
		return false ;

	return Intersect(first, second, point, pParallel) ;
}

bool CPlane::IntersectSegments(CLineArray &segments, CCoordArray &result)
{
	CCoord c ;
	int    i ;
	CLine  *pLine = segments.GetData() ;
	int    nSize = segments.GetSize() ;
	int    nCnt = 0 ;
	CCoord *pCoord ;

	result.SetSize ( nSize ) ;
	pCoord = result.GetData() ;

	// We do not clear the result array, since we may be trying to
	// accumlate segment points...
	{
		for (i=0; i<nSize; i++)
		{
			// This should be using AddUnique, but I'm not at the moment
			if (IntersectSegment(pLine[i], pCoord[nCnt]))
				nCnt++ ;
		}
	}

	result.SetSize(nCnt) ;
	return (result.GetSize() > 0);
}

bool CPlane::IntersectSegments(CLineArray &segments, CLineArray &Result)
{
	CCoord c ;
	int    i ;

	// We do not clear the result array, since we may be trying to
	// accumlate segment points...
	Result.SetSize(0) ;

	for (i=0; i<segments.GetSize(); i++)
	{
		if (IntersectSegment(segments[i], c))
			Result.Add(segments[i]) ;
	}

	return (Result.GetSize() > 0);
}

bool CPlane::IntersectSegments(CLineArray &segments, CLine &Result)
{
	CCoord		c ;
	int			i, numfound ;
	CCoordArray	la ;

	// We do not clear the result array, since we may be trying to
	// accumlate segment points...
	numfound = 0 ;
	for (i=0; i<segments.GetSize(); i++)
	{
		if (IntersectSegment(segments[i], c))
		{
			la.AddUnique(c) ;
			numfound++;
		}
	}

	if (la.GetSize()!=2)
	{
		return false ;
	}
	else
	{
		Result.Set(la[0], la[1]) ;
		return true ;
	}
}

bool CPlane::IntersectSegment(CLine &line, CCoord &point, bool *pParallel)
{
	bool firstOnTop, secondOnTop ;
	double d ;

	// First, are the points on the same side?
	// No need to normalize, since we're not really
	// using the scalar distance value, we're only
	// determining top/bottom location.
	d = Distance(line.m_first, &firstOnTop, false) ;
	d = Distance(line.m_second, &secondOnTop, false) ;

	if (firstOnTop==secondOnTop)
		return false ;

	return Intersect(line, point, pParallel) ;
}

void CCoord::Subtract(const CVector &c)
{
	m_x -= c.m_dx ;
	m_y -= c.m_dy ;
	m_z -= c.m_dz ;
}

void CCoord::Subtract(const CCoord &c)
{
	m_x -= c.m_x ;
	m_y -= c.m_y ;
	m_z -= c.m_z ;
}

void CVector::Subtract(const CVector &c)
{
	m_dx -= c.m_dx ;
	m_dy -= c.m_dy ;
	m_dz -= c.m_dz ;
}

void CVector::Subtract(const CCoord &c)
{
	m_dx -= c.m_x ;
	m_dy -= c.m_y ;
	m_dz -= c.m_z ;
}

void CCoord::Add(const CVector &c)
{
	m_x += c.m_dx ;
	m_y += c.m_dy ;
	m_z += c.m_dz ;
}

void CCoord::Add(const CCoord &c)
{
	m_x += c.m_x ;
	m_y += c.m_y ;
	m_z += c.m_z ;
}

void CVector::Add(const CVector &c)
{
	m_dx += c.m_dx ;
	m_dy += c.m_dy ;
	m_dz += c.m_dz ;
}

void CVector::Add(const CCoord &c)
{
	m_dx += c.m_x ;
	m_dy += c.m_y ;
	m_dz += c.m_z ;
}

bool CPlane::Intersect(const CLine &IntersectLine, CCoord &point, bool *pParallel)
{
	// The equation for the line intersection of a
	// plane is
	//
	// C = P1 + newU * Dir
	// 
	// where
	//
	// Dir  = P2 - P1 
	// newU = (k - P1*m_normal) / (Dir*m_normal)
	// 
	// and
	//
	// C    = returned coordinate
	// newU = the new line multiplier
	// P1   = the first coordinate of the line seg
	// P2   = the second coordinate of the ling seg
	// k	= is the distance of the plane to (0,0,0).  This is calculated
	//        at startup....
	// m_normal = the normal of the plane

	// First, figure out if the line is parallel...
/*
	DOUBLE denominator ;
	DOUBLE numerator ;
	DOUBLE newU ;
	CVector Dir (IntersectLine.m_second);

	Dir.Subtract(IntersectLine.m_first) ;
	Dir.Normalize() ;
	denominator = m_normal.DotProduct(Dir) ;

	if (denominator==0.0)
	{
		if (pParallel)
			*pParallel = true ;
		point.Set(0,0,0) ;
		return false ;
	}

	numerator = m_k - m_normal.DotProduct(IntersectLine.m_first) ;
	newU	 = numerator / denominator ;
	
	point = IntersectLine.m_first ;
	Dir *= newU ;
	point.Add(Dir) ;

	if (pParallel)
		*pParallel = false ;
	return true ;
*/

	DOUBLE denominator ;
	DOUBLE numerator ;
	DOUBLE newU ;
	CVector Dir    (IntersectLine.m_second);
	CVector OrgDir (m_origin) ;

	Dir.Subtract(IntersectLine.m_first) ;
	OrgDir.Subtract(IntersectLine.m_first) ;
	denominator = m_normal.DotProduct(Dir) ;

	if (denominator==0.0)
	{
		if (pParallel)
			*pParallel = true ;
		point.Set(0,0,0) ;
		return false ;
	}

	numerator = m_normal.DotProduct(OrgDir) ;
	newU	 = numerator / denominator ;
	
	point = IntersectLine.m_first ;
	Dir *= newU ;
	point.Add(Dir) ;

	if (pParallel)
		*pParallel = false ;
	return true ;

}

bool CPlane::Intersect(const CLine &IntersectLine, DOUBLE *point, bool *pParallel)
{
	/*
	DOUBLE denominator ;
	DOUBLE numerator ;
	DOUBLE newU ;
	CVector Dir    (IntersectLine.m_second);
	CVector OrgDir (m_origin) ;

	Dir.Subtract(IntersectLine.m_first) ;
	OrgDir.Subtract(IntersectLine.m_first) ;
	denominator = m_normal.DotProduct(Dir) ;

	if (denominator==0.0)
	{
		if (pParallel)
			*pParallel = true ;
		point[0] = 0 ;
		point[1] = 0 ;
		point[2] = 0 ;
		return false ;
	}

	numerator = m_normal.DotProduct(OrgDir) ;
	newU	 = numerator / denominator ;
	
	point[0] = IntersectLine.m_first.m_x ;
	point[1] = IntersectLine.m_first.m_y ;
	point[2] = IntersectLine.m_first.m_z ;
	Dir *= newU ;
	point[0] += Dir.m_dx;
	point[1] += Dir.m_dy;
	point[2] += Dir.m_dz;

	if (pParallel)
		*pParallel = false ;
	return true ;
*/
	DOUBLE denominator ;
	DOUBLE numerator ;
	DOUBLE newU ;
	DOUBLE DirX = IntersectLine.m_second.m_x - IntersectLine.m_first.m_x ;
	DOUBLE DirY = IntersectLine.m_second.m_y - IntersectLine.m_first.m_y ;
	DOUBLE DirZ = IntersectLine.m_second.m_z - IntersectLine.m_first.m_z ;
	DOUBLE OrgDirX = m_origin.m_x - IntersectLine.m_first.m_x ;
	DOUBLE OrgDirY = m_origin.m_y - IntersectLine.m_first.m_y ;
	DOUBLE OrgDirZ = m_origin.m_z - IntersectLine.m_first.m_z ;

	denominator =   m_normal.m_dx*DirX 
				  + m_normal.m_dy*DirY
				  + m_normal.m_dz*DirZ ;
	if (denominator == 0.0)
	{
		if (pParallel)
			*pParallel = true ;
		point[0] = (DOUBLE)0.0 ;
		point[1] = (DOUBLE)0.0 ;
		point[2] = (DOUBLE)0.0 ;
		return false ;
	}

	numerator   =   m_normal.m_dx*OrgDirX 
				  + m_normal.m_dy*OrgDirY
				  + m_normal.m_dz*OrgDirZ ;
	newU = numerator / denominator ;

	point[0] = IntersectLine.m_first.m_x + DirX * newU ;
	point[1] = IntersectLine.m_first.m_y + DirY * newU ;
	point[2] = IntersectLine.m_first.m_z + DirZ * newU ;

	if (pParallel)
		*pParallel = false ;
	return true ;
}

bool CPlane::Intersect ( DOUBLE *ILine1
					 , DOUBLE *ILine2
					 , DOUBLE *point
					 , bool   *pParallel 
					 , bool   bSegment )
{
	DOUBLE denominator ;
	DOUBLE numerator ;
	DOUBLE newU ;
	DOUBLE DirX, DirY, DirZ ;
	DOUBLE OrgDirX, OrgDirY, OrgDirZ ;

	DirX = ILine2[0] - ILine1[0] ;
	DirY = ILine2[1] - ILine1[1] ;
	DirZ = ILine2[2] - ILine1[2] ;

	OrgDirX = m_origin.m_x - ILine1[0] ;
	OrgDirY = m_origin.m_y - ILine1[1] ;
	OrgDirZ = m_origin.m_z - ILine1[2] ;

	denominator = m_normal.m_dx*DirX 

				+ m_normal.m_dy*DirY 
				+ m_normal.m_dz*DirZ ;
	
	if (denominator==0.0)
	{
		if (pParallel)
			*pParallel = true ;
		point[0] = 0 ;
		point[1] = 0 ;
		point[2] = 0 ;
		return false ;
	}

	numerator =   m_normal.m_dx*OrgDirX 
				+ m_normal.m_dy*OrgDirY 
				+ m_normal.m_dz*OrgDirZ ;
	
	newU	 = numerator / denominator ;
	if (bSegment && (newU<0 || newU>1))
		return false ;
	
	point[0] = ILine1[0] + DirX * newU ;
	point[1] = ILine1[1] + DirY * newU ;
	point[2] = ILine1[2] + DirZ * newU ;

	if (pParallel)
		*pParallel = false ;
	return true ;

}

void CPlane::Normalize()
{
	m_normal.Normalize();
}

bool CPlane::IntersectHalfPlane(bool bTop, CLineArray &segments, CLineArray &result) 
{
	return false;
}

bool CPlane::IntersectHalfPlane(bool bTop, CCoordArray &coords, CCoordArray &result) 
{
	bool bIsTop ;
	int  i ;

	result.SetSize(0,128);
	Normalize();
	for (i=0; i<coords.GetSize(); i++)
	{
		Distance(coords.ElementAt(i), &bIsTop,false);
		if (bIsTop==bTop)
			result.Add(coords.ElementAt(i));
	}
	return true;
}

CCoord CPlane::FindNearest(bool bTop, CLineArray &lines)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = DOUBLE_MAX ;

	for (i=0; i<lines.GetSize(); i++)
	{
		testdist = Distance(lines.ElementAt(i).m_first, &bIsTop,false);
		if (bIsTop==bTop && testdist<dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_first ;
		}
		testdist = Distance(lines.ElementAt(i).m_second, &bIsTop,false);
		if (bIsTop==bTop && testdist<dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_second ;
		}
	}
	return rval ;
}

CCoord CPlane::FindNearest(bool bTop, CCoordArray &coords)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = DOUBLE_MAX ;

	for (i=0; i<coords.GetSize(); i++)
	{
		testdist = Distance(coords.ElementAt(i), &bIsTop,false);
		if (bIsTop==bTop && testdist<dist)
		{
			dist=testdist ;
			rval = coords.ElementAt(i) ;
		}
	}
	return rval ;
}

CCoord CPlane::FindFarthest(bool bTop, CCoordArray &coords)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = (DOUBLE)0.0 ;

	for (i=0; i<coords.GetSize(); i++)
	{
		testdist = Distance(coords.ElementAt(i), &bIsTop,false);
		if (bIsTop==bTop && testdist>dist)
		{
			dist=testdist ;
			rval = coords.ElementAt(i) ;
		}
	}
	return rval ;
}

int CPlane::FindNearestIndex(bool bTop, CCoordArray &coords)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	int rval=-1;

	dist = DOUBLE_MAX ;

	for (i=0; i<coords.GetSize(); i++)
	{
		testdist = Distance(coords.ElementAt(i), &bIsTop,false);
		if (bIsTop==bTop && testdist<dist)
		{
			dist=testdist ;
			rval = i ;
		}
	}
	return rval ;
}

CCoord CPlane::FindNearest(CCoordArray &coords)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = DOUBLE_MAX ;

	for (i=0; i<coords.GetSize(); i++)
	{
		testdist = Distance(coords.ElementAt(i), &bIsTop,false);
		if (testdist<dist)
		{
			dist=testdist ;
			rval = coords.ElementAt(i) ;
		}
	}
	return rval ;
}

CCoord CPlane::FindNearest(CLineArray &lines)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = DOUBLE_MAX ;

	for (i=0; i<lines.GetSize(); i++)
	{
		testdist = Distance(lines.ElementAt(i).m_first, &bIsTop,false);
		if (testdist<dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_first ;
		}

		testdist = Distance(lines.ElementAt(i).m_second, &bIsTop,false);
		if (testdist<dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_second ;
		}

	}
	return rval ;
}

CCoord CPlane::FindFarthest(CLineArray &lines)
{
	bool bIsTop ;
	int  i ;
	DOUBLE dist, testdist ;
	CCoord rval(0,0,0);

	dist = (DOUBLE)0.0 ;

	for (i=0; i<lines.GetSize(); i++)
	{
		testdist = Distance(lines.ElementAt(i).m_first, &bIsTop,false);
		if (testdist>dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_first ;
		}

		testdist = Distance(lines.ElementAt(i).m_second, &bIsTop,false);
		if (testdist>dist)
		{
			dist=testdist ;
			rval = lines.ElementAt(i).m_second ;
		}

	}
	return rval ;
}


DOUBLE CPlane::Distance(const CCoord &point, bool *pTop, bool bNormalize)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane

	CVector PointToOrigin (point) ;
	DOUBLE  dDistance ;
	 
	// We will normalize the vector if necessary
	if (bNormalize)
	{
		m_normal.Normalize() ;
	}

	PointToOrigin.Subtract(m_origin) ;
	dDistance = PointToOrigin.DotProduct(m_normal) ;

	// x*x + y*y + z*z  =  DotProduct
	// dx*n.x + dy*n.y + ?*n.z = 0
	// ? = -(dx*n.x + dy*n.y) / n.z

	if (dDistance >= 0.0)
	{
		if (pTop)
			*pTop = true ;
		return dDistance ;
	}
	else
	{
		if (pTop)
			*pTop = false ;
		return -dDistance ;
	}
}

DOUBLE CPlane::Distance(DOUBLE *point, bool *pTop, bool bNormalize)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane

	DOUBLE dx,dy,dz ;
	DOUBLE dDistance ;
	 
	// We will normalize the vector if necessary
	if (bNormalize)
	{
		m_normal.Normalize() ;
	}

	dx = point[0] - m_origin.m_x ;
	dy = point[1] - m_origin.m_y ;
	dz = point[2] - m_origin.m_z ;

	dDistance = dx*m_normal.m_dx + dy*m_normal.m_dy + dz*m_normal.m_dz ;
	
	if (dDistance >= 0.0)
	{
		if (pTop)
			*pTop = true ;
		return dDistance ;
	}
	else
	{
		if (pTop)
			*pTop = false ;
		return -dDistance ;
	}
}

int CPlane::AboveBelow(CLineArray &lines)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane
	int i ;
	int a1, a2 ;
	int r1, r2 ;

	r1 = 0 ;
	r2 = 0 ;

	for (i=0; i<lines.GetSize(); i++)
	{
		a1 = AboveBelow (lines.ElementAt(i).m_first) ;
		a2 = AboveBelow (lines.ElementAt(i).m_second) ;
		if (a1&&a2)
		{
			if (a1!=a2)
				return 0;
		}

		r1 |= a1 ;
		r2 |= a2 ;
	}
	if (r1 && r2)
	{
		if (r1==3 || r2==3)
			return 0 ;
		else
		if (r1!=r2)
			return 0 ;
		else
			return r1 ;
	}
	return 0 ;
}



int CPlane::AboveBelow(CCoord &point)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane

	CVector PointToOrigin (point) ;
	DOUBLE  dDistance ;
	 
	PointToOrigin.Subtract(m_origin) ;
	dDistance = PointToOrigin.DotProduct(m_normal) ;

	if (fabs(dDistance) < EPSILON)
		return 0 ;
	else
	if (dDistance > 0.0)
		return 2 ;
	else
//	if (dDistance < 0.0)
		return 1 ;
}

int CPlane::AboveBelow(DOUBLE *point)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane

	CVector PointToOrigin (point) ;
	DOUBLE  dDistance ;
	 
	PointToOrigin.Subtract(m_origin) ;
	dDistance = PointToOrigin.DotProduct(m_normal) ;

	if (fabs(dDistance) < EPSILON)
		return 0 ;
	else
	if (dDistance > 0.0)
		return 2 ;
	else
//	if (dDistance < 0.0)
		return 1 ;
}

int CVector::AboveBelow2D(CCoord &origin, CCoord &point)
{
	// The equation for the distance of a point to a
	// plane is
	//
	//    d = N * (P - C) / |N|
	//
	// where d = distance
	//		 N = normal vector
	//		 P = desired point to measure
	//		 C = intersection coord of plane

	CVector PointToOrigin (point) ;
	DOUBLE  dDistance ;
	 
	PointToOrigin.Subtract(origin) ;
	dDistance = DotProduct2D(PointToOrigin) ;

	if (dDistance > 0.0)
		return 2 ;
	else
	if (dDistance < 0.0)
		return 1 ;
	else
		return 0;
}

CPlane::operator CVector& () 
{
    return m_normal ;
}

CPlane::operator CCoord& () 
{
    return m_origin ;
}

void CPlane::operator=(const CPlane& copy)
{
	m_normal = copy.m_normal ;
	m_origin = copy.m_origin ;
//	m_k = copy.m_k ;
}

CPlane CPlane::operator -() 
{
	return CPlane(m_origin, -m_normal) ;
}

bool CPlane::operator==(CPlane &comp) const
{
	return ( (m_origin==comp.m_origin)
		  && (m_normal==comp.m_normal) 
		     ) ;
}

CPlane::CPlane(const CPlane &copy) 
{
    *this = copy ;
}

// This assumes that the 
CPlane::CPlane(DOUBLE *pNormalCoord)
{
	m_normal.m_dx = pNormalCoord[0] ;
	m_normal.m_dy = pNormalCoord[1] ;
	m_normal.m_dz = pNormalCoord[2] ;

	m_origin.m_x = pNormalCoord[3] ;
	m_origin.m_y = pNormalCoord[4] ;
	m_origin.m_z = pNormalCoord[5] ;
}

void CPlane::Set(const CCoord &Origin, CVector Normal)
{
//	bool bIsTop ;
//	Normal.Normalize() ;
	m_normal = Normal ;
	m_origin = Origin ;
//	m_normal.Normalize () ;
//	m_k = Distance(CCoord(0,0,0), &bIsTop) ;
//	if (bIsTop)
//		m_k = -m_k ;
}

void CPlane::Set(const CCoord &Origin, CVector DirOne, CVector DirTwo)
{
//	bool bIsTop ;

	DirOne.Normalize() ;
	DirTwo.Normalize() ;

	m_origin = Origin ; 
	m_normal.CrossProduct(DirOne, DirTwo, true) ;
//	m_k = Distance(CCoord(0,0,0), &bIsTop) ;
//	if (bIsTop)
//		m_k = -m_k ;
}

void CPlane::Set(const CCoord &Origin, const CCoord &One, const CCoord &Two)
{
//	bool bIsTop ;
	CVector DirOne ;
	CVector DirTwo ;

	m_origin = Origin ; 
	DirOne.Set(Origin, One) ;
	DirTwo.Set(Origin, Two) ;
	DirOne.Normalize() ;
	DirTwo.Normalize() ;
	m_normal.CrossProduct(DirOne, DirTwo, true) ;
//	m_k = Distance(CCoord(0,0,0), &bIsTop) ;
//	if (bIsTop)
//		m_k = -m_k ;
}

CPlane::CPlane(DOUBLE *pOrigin, DOUBLE *pPoint1, DOUBLE *pDir, bool Normalize)
{
    DOUBLE length ;
	DOUBLE dir1x = pPoint1[0] - pOrigin[0] ;
	DOUBLE dir1y = pPoint1[1] - pOrigin[1] ;
	DOUBLE dir1z = pPoint1[2] - pOrigin[2] ;

    // First, calculate the vector components
    m_normal.m_dx = dir1y * pDir[2] - dir1z * pDir[1] ;
    m_normal.m_dy = dir1z * pDir[0] - dir1x * pDir[2] ;
    m_normal.m_dz = dir1x * pDir[1] - dir1y * pDir[0] ;

	m_origin.m_x = pOrigin[0] ;
	m_origin.m_y = pOrigin[1] ;
	m_origin.m_z = pOrigin[2] ;

    // Next, normalize the components
	if (Normalize)
	{
		if (m_normal.m_dx!=0.0 || m_normal.m_dx!=0.0 || m_normal.m_dx!=0.0)
		{
			length = (DOUBLE)sqrt( m_normal.m_dx*m_normal.m_dx 
								 + m_normal.m_dy*m_normal.m_dy 
								 + m_normal.m_dz*m_normal.m_dz);
			m_normal.m_dx /= length ;
			m_normal.m_dy /= length ;
			m_normal.m_dz /= length ;
		}
	}
}


// This method assumes that pPoint1 is a coordinate, but that
// pDir is a vector
void CPlane::Set ( DOUBLE *pOrigin
				 , DOUBLE *pPoint1
				 , DOUBLE *pDir
				 , bool Normalize   //=true
				   )
{
    DOUBLE length ;
	DOUBLE dir1x = pPoint1[0] - pOrigin[0] ;
	DOUBLE dir1y = pPoint1[1] - pOrigin[1] ;
	DOUBLE dir1z = pPoint1[2] - pOrigin[2] ;

    // First, calculate the vector components
    m_normal.m_dx = dir1y * pDir[2] - dir1z * pDir[1] ;
    m_normal.m_dy = dir1z * pDir[0] - dir1x * pDir[2] ;
    m_normal.m_dz = dir1x * pDir[1] - dir1y * pDir[0] ;

	m_origin.m_x = pOrigin[0] ;
	m_origin.m_y = pOrigin[1] ;
	m_origin.m_z = pOrigin[2] ;

    // Next, normalize the components
	if (Normalize)
	{
		if (m_normal.m_dx!=0.0 || m_normal.m_dx!=0.0 || m_normal.m_dx!=0.0)
		{
			length = (DOUBLE)sqrt( m_normal.m_dx*m_normal.m_dx 
								 + m_normal.m_dy*m_normal.m_dy 
								 + m_normal.m_dz*m_normal.m_dz);
			m_normal.m_dx /= length ;
			m_normal.m_dy /= length ;
			m_normal.m_dz /= length ;
		}
	}
}

// This method assumes that pPoint1 is a coordinate, but that
// pDir is a vector
void CPlane::Set2( DOUBLE *pOrigin
				 , DOUBLE *pPoint1
				 , DOUBLE *pPoint2
				 , bool Normalize   //=true
				   )
{
    DOUBLE length ;
	DOUBLE dir1x = pPoint1[0] - pOrigin[0] ;
	DOUBLE dir1y = pPoint1[1] - pOrigin[1] ;
	DOUBLE dir1z = pPoint1[2] - pOrigin[2] ;
	DOUBLE dir2x = pPoint2[0] - pOrigin[0] ;
	DOUBLE dir2y = pPoint2[1] - pOrigin[1] ;
	DOUBLE dir2z = pPoint2[2] - pOrigin[2] ;

	/*
	_asm
	{
		movups xmm0, [pOrigin]
		movups xmm1, [pPoint1]
		movups xmm2, [pPoint2]

		// xmm1 becomes dir1
		// xmm2 becomes dir2
		sub xmm1, xmm0
		sub xmm2, xmm0

		// 
	}
	*/


    // First, calculate the vector components
    m_normal.m_dx = dir1y * dir2z - dir1z * dir2y ;
    m_normal.m_dy = dir1z * dir2x - dir1x * dir2z ;
    m_normal.m_dz = dir1x * dir2y - dir1y * dir2x ;

	m_origin.m_x = pOrigin[0] ;
	m_origin.m_y = pOrigin[1] ;
	m_origin.m_z = pOrigin[2] ;

    // Next, normalize the components
	if (Normalize)
	{
		if (m_normal.m_dx!=0.0 || m_normal.m_dy!=0.0 || m_normal.m_dz!=0.0)
		{
			length = (DOUBLE)sqrt( m_normal.m_dx*m_normal.m_dx 
								 + m_normal.m_dy*m_normal.m_dy 
								 + m_normal.m_dz*m_normal.m_dz);
			m_normal.m_dx /= length ;
			m_normal.m_dy /= length ;
			m_normal.m_dz /= length ;
		}
	}
}

CPlane CPlane::Offset(DOUBLE amount)
{
//	bool bIsTop ;
	CPlane rval(*this) ;

	rval.m_origin = rval.m_origin + rval.m_normal * amount ;
//	rval.m_k = rval.Distance(CCoord(0,0,0), &bIsTop) ;
//	if (bIsTop)
//		rval.m_k = -rval.m_k ;
//
	return rval ;
}


void CPlane::Set(CLine &OrgVec, const CVector &DirTwo)
{
	Set(OrgVec.m_first, OrgVec.Slope(true), DirTwo) ;
}

// To find the nearest point, find the distance of the plane from
// the desired point, and then use the distance to scale the unit
// normal vector and add it to the test point.  This will yield
// the point in the plane.  The half plane location will need to
// be determined too....
CCoord CPlane::NearestPointInPlane(CCoord &point, bool *pbOnTop)
{
	CCoord rval ;
	DOUBLE dDistance ;
	bool   bOnTop ;

	dDistance = Distance(point, &bOnTop) ;

	if (bOnTop)
		rval = point - m_normal * dDistance ;
	else
		rval = point + m_normal * dDistance ;

	if (pbOnTop)
		*pbOnTop = bOnTop ;

	return rval ;
}

bool CVector::IsZero()
{
	if (m_dx==0 && m_dy==0 && m_dz==0)
		return true ;
	else
		return false ;
}

// The following returns the intersection of two planes if it indeed
// exists.

bool CPlane::Intersect(CPlane &IntersectPlane, CLine &Result, bool *pParallel)
{
	CVector linedir ;
	
	// The cross product of the two planes yields the vector of 
	// the intersecting line, if the two planes are not parallel!
	if (linedir.CrossProduct(IntersectPlane.m_normal, m_normal, true)==false)
	{
		if (pParallel)
			*pParallel = true ;
		return false ;
	}

	if (pParallel)
		*pParallel = false;

	// Now, we need to figure out a point that lies on both
	// planes.  This will also lie in this line.
	bool   bIsTop ;
	DOUBLE d1 = Distance(CCoord(0,0,0), &bIsTop) ;
	if (bIsTop)
		d1 = -d1 ;
	DOUBLE d2 = IntersectPlane.Distance(CCoord(0,0,0), &bIsTop) ;
	if (bIsTop)
		d2 = -d2 ;
	DOUBLE N1N1 = m_normal.DotProduct(m_normal) ;
	DOUBLE N1N2 = m_normal.DotProduct(IntersectPlane.m_normal) ;
	DOUBLE N2N2 = IntersectPlane.m_normal.DotProduct(IntersectPlane.m_normal) ;
	DOUBLE determinant = N1N1*N2N2 - N1N2*N1N2 ;
	DOUBLE c1 = (d1*N2N2 - d2*N1N2) / determinant;
	DOUBLE c2 = (d2*N1N1 - d1*N1N2) / determinant;

	CCoord IntersectPoint(m_normal*c1 + IntersectPlane.m_normal*c2) ;

	Result.Set(IntersectPoint, linedir) ;

	return true ;
}

// Curves Section

/*
// Base Curve Class
CCurve::CCurve() 
{
	m_bOpen = false ;
}

CCurve::~CCurve() 
{
}

DOUBLE CCurve::Factorial(int n)
{
	DOUBLE rval ;

	rval = (DOUBLE)n ;
	if (n<=0)
		rval = 1 ;
	else
	for (int i=n-1; i>1; i--)
		rval *= (DOUBLE) i ;

	return rval ;
}


// Bezier Curve Section
CBezierCurve::CBezierCurve()
{
	m_bOpen = false ;
}

CBezierCurve::~CBezierCurve()
{
	Clear() ;
}

void CBezierCurve::Clear()
{
	m_BernsteinCombinatorials.SetSize(0) ;
	m_Coords.SetSize(0) ;
}

DOUBLE CBezierCurve::CombinatorialFactor(int n, int i)
{
	long double fn, fi, fni ;

	fn = Factorial(n) ;
	fi = Factorial(i) ;
	fni = Factorial(n-i) ;

	return (DOUBLE)(fn/(fi*fni)) ;
}

// For the Bezier curve, initialization consists of copying the coords,
// and then calculating the combinatorial values (which are factorials).
void CBezierCurve::Initialize(CCoordArray &coords, int order, bool fit, bool open)
{
	int i, n ;

	m_bFit = fit ;
	m_Coords.Copy(coords) ;
	n = m_Coords.GetSize()-1;
	m_BernsteinCombinatorials.SetSize(n+1) ;
	for (i=0; i<=n; i++)
		m_BernsteinCombinatorials[i] = CombinatorialFactor(n, i) ;
}

bool CBezierCurve::Interpolate(CCoord &sum, DOUBLE t) 
{
	int			 n = m_Coords.GetSize() - 1;
	int			 i;

	if (n < 0)
		return false ;

	sum.Set(0,0,0) ;

	for (i=0; i<=n; i++)
		sum += m_Coords[i] * (DOUBLE)(m_BernsteinCombinatorials[i] * pow(t, i) * pow(1.0-t, n-i)) ;

	return true;
}

bool CBezierCurve::Interpolate(CCoordArray &cArr, int numelem) 
{
	int			 n = m_Coords.GetSize() - 1;
	int			 i, j ;
	DOUBLE		 dt ;
	CCoord		 sum ;
	DOUBLE	     t;

	if (numelem < 2 || n < 0)
		return false ;

	dt = (DOUBLE)1.0 / (DOUBLE)(numelem-1) ;

	cArr.SetSize(numelem) ;

	for (j=0; j<numelem; j++)
	{
		t = (DOUBLE)j / (DOUBLE)(numelem-1) ;

		sum.Set(0,0,0) ;

		for (i=0; i<=n; i++)
			sum += m_Coords[i] * (DOUBLE)(m_BernsteinCombinatorials[i] * pow(t, i) * pow(1.0-t, n-i)) ;

		cArr[j] = sum ;
	}

	return true;
}




// BSplineCurve section

CBSplineCurve::CBSplineCurve()
{
	m_bOpen = false ;
	m_dLower = 0.0 ;
	m_dUpper = 1.0 ;
}

CBSplineCurve::~CBSplineCurve()
{
	Clear() ;
}

void CBSplineCurve::Clear()
{
	m_Coords.SetSize(0) ;
}

DOUBLE knots[100];

// For the BSpline curve, initialization consists of copying the coords,
// and then calculating the combinatorial values (which are factorials).
void CBSplineCurve::Initialize(CCoordArray &coords, int order, bool fit, bool open)
{
	int i ;
	int n = coords.GetSize() - 1;
	CCoord cs, ce ;

	if (fit)
		open = true ;

	m_Coords.Copy(coords) ;
	m_nOrder = order ;
	m_bOpen = open ;
	m_bFit = fit ;

	if (m_bOpen)
	{
		m_dLower = 0.0 ;
		m_dUpper = 1.0 ;
		m_Knots.SetSize(n + m_nOrder + 1) ;
		if (m_bFit)
		{
		}
		else
		{
			for (i = 1; i <= n + m_nOrder + 1; i++)
			{
				if (i>=1 && i<=m_nOrder)
					m_Knots[i-1] = 0 ;
				else
				if (i>=m_nOrder+1 && i<=n+1)
					m_Knots[i-1] = (DOUBLE)(i - m_nOrder) / (DOUBLE)(n-m_nOrder+2) ;
				else
					m_Knots[i-1] = 1 ;
			}
		}
	}
	else
	{
		cs = m_Coords[0] ;
		ce = m_Coords[n] ;
		n+= 2 ;
		m_Knots.SetSize(n + m_nOrder + 1) ;
		m_Coords.InsertAt(0, cs) ;
		m_Coords.Add(ce) ;
		m_dLower = (DOUBLE)(m_nOrder-1) / (DOUBLE)(n+m_nOrder) ;
		m_dUpper = (DOUBLE)(n + 1) / (DOUBLE)(n+m_nOrder)  ;

		for (i = 1; i <= n + m_nOrder + 1; i++)
			m_Knots[i-1] = (DOUBLE)(i-1) / (DOUBLE)(n+m_nOrder) ;
	}
}

DOUBLE CBSplineCurve::BasisDivide(DOUBLE numerator, DOUBLE denominator) 
{
	if (numerator==denominator && numerator==0.0)
		return 0.0 ;
	else
		return numerator / denominator ;
}


int maxv = -1 ;
int minv = 200 ;
int ksize = 0 ;

DOUBLE CBSplineCurve::GetKnot(int i)
{
	i-- ;

	if (ksize < m_Knots.GetSize())
		ksize = m_Knots.GetSize();

	if (maxv < i)
		maxv = i ;
	if (minv > i)
		minv = i ;

	return m_Knots[i] ;
}

DOUBLE CBSplineCurve::BSplineBasis(int i, int order, DOUBLE t)
{
	DOUBLE rval ;
	DOUBLE first, second ;
	DOUBLE diff1, N1, diff2, N2, diff3, diff4 ;
	DOUBLE k1, k2, k3, k4;
	int    numknots = m_Knots.GetSize() ;
	DOUBLE *pKnots = m_Knots.GetData() - 1 ;

	if (order > 1)
	{
		k1 = pKnots[i] ;
		k2 = pKnots[i+order-1] ;

		diff1 = (t-k1) ;
		N1    = BSplineBasis(i, order-1, t) ;
		diff2 = k2 - k1 ;
		first  = BasisDivide( diff1 * N1, diff2 ) ;



		k3 = pKnots[i+order] ;
		k4 = pKnots[i+1] ;

		diff3 = (k3 - t) ;
		N2    = BSplineBasis(i+1, order-1, t) ;
		diff4 = k3 - k4 ;
		second = BasisDivide( diff3 * N2, diff4 ) ;

		rval = first + second ;
	}
	else
	{
		DOUBLE k1 ;
		DOUBLE k2 ;

		k1 = pKnots[i] ;
		k2 = pKnots[i+1] ;

		if (i==numknots)
		{
			if (k1 <= t && t <= k2)
				return 1 ;
			else
				return 0 ;
		}
		else
		{
			if (k1 <= t && t < k2)
				return 1 ;
			else
				return 0 ;
		}
	}

	return rval ;
}

bool CBSplineCurve::Interpolate(CCoord &sum, DOUBLE t) 
{
	int			 n = m_Coords.GetSize()-1 ;
	int			 i ;

	if (n<0)
		return false ;

	// If not initialized, or out of range, return origin point...
	if (t>=m_dUpper)
	{
		sum = m_Coords[n] ;
		if (m_bOpen && t==1.0)
			return true ;

		return false ;
	}
	if (t<m_dLower)
		return false ;

	sum.Set(0,0,0) ;
	for (i=1; i<=n+1; i++)
		sum += m_Coords[i-1] * BSplineBasis(i, m_nOrder, t) ;

	// Finally, return the calculated point...
	return true ;
}

bool CBSplineCurve::Interpolate(CCoordArray &sumArr, int numelem) 
{
	int			 n = m_Coords.GetSize()-1 ;
	int			 i, j ;
	DOUBLE		 range, dt, t ;
	CCoord		 sum ;

	if (n<0 || numelem < 2)
		return false ;

	// If not initialized, or out of range, return origin point...
	range = m_dUpper - m_dLower ;
	dt = range / (DOUBLE)(numelem-1) ;

	sumArr.SetSize(numelem) ;

	for (j=0; j<numelem; j++)
	{
		t = m_dLower + range * (DOUBLE) j / (DOUBLE) (numelem - 1) ;

		sum.Set(0,0,0) ;

		for (i=1; i<=n+1; i++)
			sum += m_Coords[i-1] * BSplineBasis(i, m_nOrder, t) ;

		sumArr[j] = sum ;
	}

	// Finally, return the calculated point...
	return true ;
}

CCoord CBSplineCurve::PseudoStart()
{
	CCoord rval ;
	return rval ;
}

CCoord CBSplineCurve::PseudoEnd()
{
	CCoord rval ;
	return rval ;
}
*/







// Rational BSpline Curve Section

/*
CRationalBSplineCurve::CRationalBSplineCurve()
{
	m_bOpen = false ;
	m_dLower = 0.0 ;
	m_dUpper = 1.0 ;
}

CRationalBSplineCurve::~CRationalBSplineCurve()
{
	Clear() ;
}

void CRationalBSplineCurve::Clear()
{
	m_Coords.SetSize(0) ;
}

// For the BSpline curve, initialization consists of copying the coords,
// and then calculating the combinatorial values (which are factorials).
void CRationalBSplineCurve::Initialize(CRationalCoordArray &coords, int order, bool fit, bool open)
{
	int i;
	int n = coords.GetSize() - 1;
	CRationalCoord cs, ce ;

	if (fit)
		open = true ;

	m_Coords.Copy(coords) ;
	m_nOrder = order ;
	m_bOpen = open ;
	m_bFit = fit ;

	if (m_bOpen)
	{
		m_dLower = 0.0 ;
		m_dUpper = 1.0 ;
		m_Knots.SetSize(n + m_nOrder + 1) ;
		for (i = 1; i <= n + m_nOrder + 1; i++)
		{
			if (i>=1 && i<=m_nOrder)
				m_Knots[i-1] = 0 ;
			else
			if (i>=m_nOrder+1 && i<=n+1)
				m_Knots[i-1] = (DOUBLE)(i - m_nOrder) / (DOUBLE)(n-m_nOrder+2) ;
			else
				m_Knots[i-1] = 1 ;
		}
	}
	else
	{
		cs = m_Coords[0] ;
		ce = m_Coords[n] ;
		n+= 2 ;
		m_Knots.SetSize(n + m_nOrder + 1) ;
		m_Coords.InsertAt(0, cs) ;
		m_Coords.Add(ce) ;
		m_dLower = (DOUBLE)(m_nOrder-1) / (DOUBLE)(n+m_nOrder) ;
		m_dUpper = (DOUBLE)(n + 1) / (DOUBLE)(n+m_nOrder)  ;

		for (i = 1; i <= n + m_nOrder + 1; i++)
			m_Knots[i-1] = (DOUBLE)(i-1) / (DOUBLE)(n+m_nOrder) ;
	}
}

DOUBLE CRationalBSplineCurve::BasisDivide(DOUBLE numerator, DOUBLE denominator) 
{
	if (numerator==denominator && numerator==0.0)
		return 0.0 ;
	else
		return numerator / denominator ;
}


DOUBLE CRationalBSplineCurve::GetKnot(int i)
{
	i-- ;

	if (ksize < m_Knots.GetSize())
		ksize = m_Knots.GetSize();

	if (maxv < i)
		maxv = i ;
	if (minv > i)
		minv = i ;

	return m_Knots[i] ;
}

DOUBLE CRationalBSplineCurve::RationalBSplineBasis(int i, int order, DOUBLE t)
{
	DOUBLE numerator ;
	DOUBLE denominator ;
	DOUBLE rval ;
	int	   j ;
	int	   n = m_Coords.GetSize()-1 ;
	CRationalCoord *pCoords = m_Coords.GetData() - 1;

	numerator = pCoords[i].m_h * BSplineBasis(i, order, t) ;
	denominator = 0.0 ;

	// To optimize, we should precalculate this in the initialize
	// method....
	for (j=1; j<=n+1; j++)
	{
		denominator += pCoords[j].m_h * BSplineBasis(j, order, t) ;
	}

	if (denominator==0.0)
		rval = 0.0 ;

	else
		rval = numerator / denominator ;
	return rval ;
}

DOUBLE CRationalBSplineCurve::BSplineBasis(int i, int order, DOUBLE t)
{
	DOUBLE rval ;
	DOUBLE first, second ;
	DOUBLE diff1, N1, diff2, N2, diff3, diff4 ;
	DOUBLE k1, k2, k3, k4;
	int    numknots = m_Knots.GetSize() ;
	DOUBLE *pKnots = m_Knots.GetData() - 1 ;

	if (order > 1)
	{
		k1 = pKnots[i] ;
		k2 = pKnots[i+order-1] ;

		N1    = BSplineBasis(i, order-1, t) ;
		diff1 = (t-k1) * N1 ;
		diff2 = k2 - k1 ;
		first  = diff2==(DOUBLE)0.0 ? (DOUBLE)0.0 : diff1/diff2 ;

		k3 = pKnots[i+order] ;
		k4 = pKnots[i+1] ;

		N2    = BSplineBasis(i+1, order-1, t) ;
		diff3 = (k3 - t) * N2;
		diff4 = k3 - k4 ;
		second = diff4==(DOUBLE)0.0 ? (DOUBLE)0.0 : diff3/diff4 ;

		rval = first + second ;
	}
	else
	{
		DOUBLE k1 ;
		DOUBLE k2 ;

		k1 = pKnots[i] ;
		k2 = pKnots[i+1] ;

		if (i==numknots)
		{
			if (k1 <= t && t <= k2)
				return 1 ;
			else
				return 0 ;
		}
		else
		{
			if (k1 <= t && t < k2)
				return 1 ;
			else
				return 0 ;
		}
	}

	return rval ;
}

bool CRationalBSplineCurve::Interpolate(CCoord &sum, DOUBLE t) 
{
	int			 n = m_Coords.GetSize()-1 ;
	int			 i ;
	CRationalCoord *pCoords = m_Coords.GetData() ;

	if (n<0)
		return false ;

	// If not initialized, or out of range, return origin point...
	if (t>=m_dUpper)
	{
		sum = pCoords[n] ;
		if (m_bOpen && t==1.0)
			return true ;

		return false ;
	}
	if (t<m_dLower)
		return false ;

	sum.Set(0,0,0) ;
	{
				for (i=1; i<=n+1; i++)
			sum += pCoords[i-1] * RationalBSplineBasis(i, m_nOrder, t) ;
	}

	// Finally, return the calculated point...
	return true ;
}

bool CRationalBSplineCurve::Interpolate(CCoordArray &sumArr, int numelem) 
{
	int			 n = m_Coords.GetSize()-1 ;
	int			 i, j ;
	DOUBLE		 range, dt, t ;
	CCoord		 sum ;
	CRationalCoord *pCoords = m_Coords.GetData() ;

	if (n<0 || numelem < 2)
		return false ;

	// If not initialized, or out of range, return origin point...
	range = m_dUpper - m_dLower ;
	dt = range / (DOUBLE)(numelem-1) ;

	sumArr.SetSize(numelem) ;

	for (j=0; j<numelem; j++)
	{
		t = m_dLower + range * (DOUBLE) j / (DOUBLE) (numelem - 1) ;

		sum.Set(0,0,0) ;

		if (t>=m_dUpper)
		{
			sum = pCoords[n] ;
		}
		if (t<m_dLower)
		{
		}
		else
		{
			for (i=1; i<=n+1; i++)
				sum += pCoords[i-1] * RationalBSplineBasis(i, m_nOrder, t) ;
		}

		sumArr[j] = sum ;
	}

	// Finally, return the calculated point...
	return true ;
}
*/


bool LeftToRightSort(CCoordArray &coords)
{
	CCoordArray rvals ;
	int			i, j ;
	CCoord		c ;
	CCoord		left ;
	CUIntArray  notfound ;
	int			leftind, closest ;
	DOUBLE		dist, lastdist ;

	if (coords.GetSize()<2)
		return false ;

	left = coords[0] ;

	// Average the points to get a right/left
	// starting point...
	c = coords.Average() ;
	notfound.SetSize(coords.GetSize()) ;
	for (i=0; i<notfound.GetSize(); i++)
		notfound[i] = 1 ;

	// Now, look for a point that is to the left
	// of this, but is lowest...
	for (i=0; i<coords.GetSize(); i++)
	{
		if (coords[i].m_y < c.m_y)
		{
			left =coords[i] ;
			leftind = i;
			break;
		}
	}

	for (i=0; i<coords.GetSize(); i++)
	{
		if (coords[i].m_y < c.m_y)
		{
			if (left.m_z > coords[i].m_z)
			{
				left = coords[i] ;
				leftind = i;
			}
		}
	}
	notfound[leftind] = 0 ;

	// Now, find the point closest to this point...
	// We don't want repeats, but if they exist,
	// it's o.k.
	rvals.Add(left) ;
	for (i=0; i<coords.GetSize()-1; i++)
	{
		lastdist =	1000000;
		closest = leftind ;
		for (j=0; j<coords.GetSize(); j++)
		{
			if (notfound[j])
			{
				dist = left.Distance(coords[j]) ;
				if (lastdist > dist)
				{
					lastdist = dist ;
					closest=j ;
				}
			}
		}

		notfound[closest] = 0 ;
		left = coords[closest];
		rvals.Add(left) ;
	}

	coords.SetSize(0);
	coords.Copy(rvals) ;
	return true ;
}

unsigned char CharacterizeXYSlope(CIntCoord &first, CIntCoord &second)
{
	int dx = second.m_x - first.m_x ;
	int dy = second.m_y - first.m_y ;

	if (dx==1)
	{
		if (dy==1)
			return 1 ;
		else
		if (dy==0)
			return 0 ;
		else
		if (dy==-1)
			return 7 ;
		else
			return 8 ;
	}
	else
	if (dx==0)
	{
		if (dy==1)
			return 2 ;
		else
		if (dy==0)
			return 8 ;
		else
		if (dy==-1)
			return 6 ;
		else
			return 8 ;
	}
	else
	if (dx==-1)
	{
		if (dy==1)
			return 3 ;
		else
		if (dy==0)
			return 4 ;
		else
		if (dy==-1)
			return 5 ;
		else
			return 8 ;
	}
	else
		return 8 ;  // uncharacterized
}

unsigned char CharacterizeYZSlope(CIntCoord &first, CIntCoord &second)
{
	int dy = second.m_y - first.m_y ;
	int dz = second.m_z - first.m_z ;

	if (dy==1)
	{
		if (dz==1)
			return 1 ;
		else
		if (dz==0)
			return 0 ;
		else
		if (dz==-1)
			return 7 ;
		else
			return 8 ;
	}
	else
	if (dy==0)
	{
		if (dz==1)
			return 2 ;
		else
		if (dz==0)
			return 8 ;
		else
		if (dz==-1)
			return 6 ;
		else
			return 8 ;
	}
	else
	if (dy==-1)
	{
		if (dz==1)
			return 3 ;
		else
		if (dz==0)
			return 4 ;
		else
		if (dz==-1)
			return 5 ;
		else
			return 8 ;
	}
	else
		return 8 ;  // uncharacterized
}

// Using and ordered list of elements (by the sorting routine
// previously implemented), create a reduced list of control
// points and weights.  The weights are highest on diagonal 
// runs and horizontal runs.
/*
 
bool FindControlRuns ( CCoordArray			&source
					 , CRationalCoordArray  &dest
					 //, CCoordArray			&dest
					 , DOUBLE				dPrecision)
{
	unsigned char   *pdeltas ;
	int				sz = source.GetSize() ;
	int				i, coordelem ;
	CIntCoordArray  intcoord ;
	CUIntArray		countArr ;
	CUIntArray		charArr ;
	unsigned char	lastc ;
	unsigned char	curc ;
	unsigned int	count ;
	CCoord			c ;

	if (sz<2)
		return false;

	pdeltas = new unsigned char[sz-1] ;
	intcoord.SetSize(sz) ;

	for (i=0; i<sz; i++)
	{
		CCoord		&s = source[i] ;
		CIntCoord	&t = intcoord[i] ;

		t.m_x = (int)floor(s.m_x / dPrecision + 0.5) ;
		t.m_y = (int)floor(s.m_y / dPrecision + 0.5) ;
		t.m_z = (int)floor(s.m_z / dPrecision + 0.5) ;
	}

	for (i=0; i<sz-1; i++)
		pdeltas[i] = CharacterizeYZSlope(intcoord[i], intcoord[i+1]) ;
 //		pdeltas[i] = CharacterizeXYSlope(intcoord[i], intcoord[i+1]) ;

	// With the slopes characterized, look for patterns!
	lastc = pdeltas[0] ;
	count = 1 ;
	for (i=1; i<sz-1; i++)
	{
		curc = pdeltas[i] ;
		if (	curc==lastc // Same as last?
			 && curc!=8		// Is this an uncharacterizable distance?
			 )
			count++;
		else
		{
			countArr.Add(count) ;
			charArr.Add(lastc) ;
			count = 1 ;
			lastc = curc ;
		}
	}

	countArr.Add(count) ;
	charArr.Add(lastc) ;


	// First and last coordinates are always used
	dest.Add(CRationalCoord(source[0], 1.0)) ;
	//dest.Add(CCoord(source[0])) ;

	// Now, we loop through the characterized slopes.  
	// The following table describes our actions...
	// 
	// 1,3,5,7 -    If the count is greater than 1, we have
	//				a diagonal and should average the coordinates.
	//				If we have a count of 1, we may have encontered
	//				noise.
	//
	// 0,2,4,6 -    If the count was horz or vert, we average
	//				regardless of the number of segments
	//				
	// 8	   -    These are uncharacterized segments.  They
	//				are always of length 1 and are always 
	//				averaged.
	coordelem = 0 ;
	for (i=0; i<countArr.GetSize(); i++)
	{
		switch(charArr[i])
		{
		case 1:
		case 3:
		case 5:
		case 7:
			if (i>0)
			{
				c = source[coordelem] ;
				coordelem++;
				c = c.Average(source[coordelem]);
				dest.Add(CRationalCoord(c, 1.0)) ;
				coordelem += countArr[i] - 1;
			}
			else
				coordelem += countArr[i] ;
			break;

		case 0:
		case 2:
		case 4:
		case 6:
			if (i>0 && countArr[i-1] > 1)
			{
				c = source[coordelem-1] ;
				coordelem++;
				c = c.Average(source[coordelem-1]);
				dest.Add(CRationalCoord(c, 1.0)) ;
				coordelem += countArr[i] - 1;
			}
			else
				coordelem += countArr[i] ;
			break;

		case 8:
			if (i>0)
			{
				c = source[coordelem] ;
				coordelem++;
				c = c.Average(source[coordelem]);
				dest.Add(CRationalCoord(c, 1.0)) ;
				coordelem += countArr[i] - 1;
			}
			else
				coordelem += countArr[i] ;
			break;
		}
	}
	// First and last coordinates are always used
	dest.Add(CRationalCoord(source[coordelem], 1.0)) ;
	//dest.Add(CCoord(source[coordelem])) ;

	return true ;
}
*/

void CIntCoord::Set(int x, int y, int z) 
{
	m_x = x ;
	m_y = y ;
	m_z = z ;
}

CCoordInt::CCoordInt() 
{
	x=y=z=0;
}

CCoordInt::CCoordInt(int cx, int cy, int cz) 
{
	x = cx ;
	y = cy ;
	z = cz ;
}

CCoordInt::CCoordInt(const CCoordInt &copy) 
{
	*this = copy ;
}

CCoordInt CCoordInt::operator -(CCoordInt &c)
{
	CCoordInt rval ;
	rval.x = x - c.x ;
	rval.y = y - c.y ;
	rval.z = z - c.z ;
	return rval ;
}

CCoordInt CCoordInt::operator +(CCoordInt &c)
{
	CCoordInt rval ;
	rval.x = x + c.x ;
	rval.y = y + c.y ;
	rval.z = z + c.z ;
	return rval ;
}

CCoordInt &CCoordInt::operator =(const CCoordInt &c)
{
	x = c.x ;
	y = c.y ;
	z = c.z ;
	return *this ;
}

void CCoordInt::Clear()
{
	x = y = z = 0 ;
}

void CCoordInt::Add(CCoordInt &src)
{
	x += src.x ;
	y += src.y ;
	z += src.z ;
}

CCoordInt CCoordInt::operator !()
{
	CCoordInt rval ;

	rval.x = abs(x) ;
	rval.y = abs(y) ;
	rval.z = abs(z) ;
	return rval ;
}

bool CCoordInt::operator==(CCoordInt &comp) const
{
    return ((comp.x == x) && (comp.y == y) && (comp.z == z) ) ;
}

bool CCoordInt::operator!=(CCoordInt &comp) const
{
    return ((comp.x != x) || (comp.y != y) || (comp.z != z) ) ;
}

// This method takes two arrays of line segments and, if possible,
// matches endpoints to endpoints to come up with a list of neighboring
// line segments.  If a match is found, an attempt is made to find all
// adjoining segments, until exhausted.  The result is stored in a frame
// array, so the segment runs can be seperated...
bool MatchSegmentArrays(CLineArray &first, CLineArray &second, CLineFrameArray &result)
{
	int			i ;
	int			j ;
	int			k ;
	int			fsize ;
	int			ssize ;
	int			cursize ;
	CLineArray	templist ;
	CLineArray  addlist ;
	bool		found ;

	result.SetSize(0) ;
	templist.SetSize(0) ;
	cursize = 0 ;

	fsize = first.GetSize() ;
	ssize = second.GetSize() ;

	// Find elements from the first list that need to be
	// added.
	for (i=0; i<fsize; i++)
	{
		CLine &f = first.ElementAt(i) ;
		for (j=0; j<ssize; j++)
		{
			CLine &s = second.ElementAt(j) ;

			// If a line in the second list adjoins one in
			// the first list, add the first to the temp
			// list and immediatly start searching for 
			// the next line in the first list.
			if (s.IsAdjoining(f))
			{
				// check to see if this line was already added, i.e.
				// if it's a repeat..

				found = false ;
				for (k=0; k<templist.GetSize() && !found; k++)
					if (f==templist.ElementAt(k))
						found = true ;
				if (!found)
				{
					templist.Add(f) ;
				}
				break; 
			}
		}
	}

	// Find elements from the second list that need to be
	// added.
	for (j=0; j<ssize; j++)
	{
		CLine &s = second.ElementAt(j) ;
		for (i=0; i<fsize; i++)
		{
			CLine &f = first.ElementAt(i) ;

			// If a line in the first list adjoins one in
			// the second list, add the second to the temp
			// list and immediatly start searching for 
			// the next line in the second list.
			if (s.IsAdjoining(f))
			{
				found = false ;
				for (k=0; k<templist.GetSize() && !found; k++)
					if (f==templist.ElementAt(k))
						found = true ;
				if (!found)
				{
					templist.Add(s) ;
				}
			}
		}
	}

	if (templist.GetSize()==0)
		return false ;

	addlist.SetSize(0) ;
	addlist.Add(templist[0]) ;
	templist.RemoveAt(0) ;

	do
	{
		found = false ;
		for (i=0; i<templist.GetSize() && !found; i++)
		{
			for (j=0; j<addlist.GetSize(); j++)
			{
				if (addlist[j].IsAdjoining(templist[i]))
				{
					// If we found an adjoining line, remove it
					// from the temp list and add it to the 
					// addlist.  Break out of both loops...
					addlist.Add(templist[i]) ;
					templist.RemoveAt(i) ;
					found = true ;
					break;
				}
			}
		}

		// If we didn't find any, then we reached the end
		// of the current set.
		if (!found && addlist.GetSize()>0)
		{
			cursize++ ;
			result.SetSize(cursize) ;
			result[cursize-1].Copy(addlist) ;
			addlist.SetSize(0) ;
			if (templist.GetSize() > 0)
				addlist.Add(templist[0]);
		}
	}
	while (addlist.GetSize() != 0) ;

	if (result.GetSize() > 0)
		return true ;
	else
		return false ;
}

CQuaternion::CQuaternion() 
{
	m_x=m_y=m_z=0.0;  m_w=1.0;
}

CQuaternion::CQuaternion(DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE w) 
{
	// Quaternions should always be normalized?
	m_x = x ;
	m_y = y ;
	m_z = z ;
	m_w = w ;
}

CQuaternion::CQuaternion(CVector &v, DOUBLE w) 
{
	m_x = v.m_dx ;
	m_y = v.m_dy ;
	m_z = v.m_dz ;
	m_w = w;
}

CQuaternion::CQuaternion(CCoord &v, DOUBLE w) 
{
	m_x = v.m_x ;
	m_y = v.m_y ;
	m_z = v.m_z ;
	m_w = w;
}

CQuaternion::~CQuaternion() 
{
}


bool CQuaternion::operator==(CQuaternion &c) const
{
	return (   m_x == c.m_x
		    && m_y == c.m_y
			&& m_z == c.m_z
			&& m_w == c.m_w) ;
}

bool CQuaternion::operator!=(CQuaternion &c) const
{
	return (   m_x != c.m_x
		    || m_y != c.m_y
			|| m_z != c.m_z
			|| m_w != c.m_w) ;
}


// Addition and subtraction
void CQuaternion::operator+=(CQuaternion &c)
{
	m_x += c.m_x ;
	m_y += c.m_y ;
	m_z += c.m_z ;
	m_w += c.m_w ;
}

void CQuaternion::operator-=(CQuaternion &c)
{
	m_x -= c.m_x ;
	m_y -= c.m_y ;
	m_z -= c.m_z ;
	m_w -= c.m_w ;
}

CQuaternion CQuaternion::operator+(CQuaternion &c) const
{
	return CQuaternion ( m_x + c.m_x
					 , m_y + c.m_y
					 , m_z + c.m_z
					 , m_w + c.m_w ) ;
}

CQuaternion CQuaternion::operator-(CQuaternion &c) const
{
	return CQuaternion ( m_x - c.m_x
					 , m_y - c.m_y
					 , m_z - c.m_z
					 , m_w - c.m_w ) ;
}


// Same as conjugate
CQuaternion CQuaternion::operator-() const
{
	return CQuaternion ( -m_x, -m_y, -m_z, m_w ) ;
}


// Multiplication
CQuaternion CQuaternion::operator*(DOUBLE Mag) const 
{
	return CQuaternion (m_x * Mag, m_y * Mag, m_z * Mag, m_w * Mag) ;
}

CQuaternion CQuaternion::operator/(DOUBLE Mag) const 
{
	return CQuaternion (m_x / Mag, m_y / Mag, m_z / Mag, m_w / Mag) ;
}

CQuaternion &CQuaternion::Normalize()
{
	DOUBLE tot, sqrtot;

	tot = Norm() ;
	sqrtot = (DOUBLE)sqrt(tot) ;
	m_x = m_x / sqrtot ;
	m_y = m_y / sqrtot ;
	m_z = m_z / sqrtot ;
	m_w = m_w / sqrtot ;
	return *this ;
}

DOUBLE CQuaternion::Norm()
{
	return m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w ;
}

CQuaternion CQuaternion::operator ~()
{
	DOUBLE dNorm = Norm() ;

	return CQuaternion(-m_x/dNorm, -m_y/dNorm, -m_z/dNorm, m_w/dNorm) ;
}

CQuaternion & CQuaternion::operator=(const CQuaternion &c)
{
	m_x = c.m_x ;
	m_y = c.m_y ;
	m_z = c.m_z ;
	m_w = c.m_w ;
	return *this;
}

void CQuaternion::Trace()
{
#ifdef _DEBUG
#ifndef _tracer_dll_h
	//		 ,m_x, m_y, m_z, m_w) ;
#endif
#endif
}

// Dot product
DOUBLE CQuaternion::operator *(const CQuaternion &c) 
{
	DOUBLE r = 0.0 ;

	return (m_x*c.m_x + m_y*c.m_y + m_z*c.m_z + m_w*c.m_w) ;
}

// Cross multiplication
CQuaternion CQuaternion::operator^(const CQuaternion &c) 
{
	/*
	return CQuaternion (
	 	  c.m_w * m_x + c.m_x * m_w + c.m_y * m_z - c.m_z * m_y 
		, c.m_w * m_y - c.m_x * m_z + c.m_y * m_w - c.m_z * m_x 
		, c.m_w * m_z + c.m_x * m_y - c.m_y * m_x - c.m_z * m_w 
		, c.m_w * m_w - c.m_x * m_x - c.m_y * m_y - c.m_z * m_z 
			) ;
			*/

	// Can also be
	//
	// q1 * q2 =
	// (s1s2 - v1.v2, s1v2 + s2v1 + v1 X v2)
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;

	DOUBLE  s2 = c.m_w ;
	CVector v2(c.m_x, c.m_y, c.m_z) ;
	CVector cross ;
	DOUBLE  dotprod ;

	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CQuaternion CQuaternion::operator^(const CVector &v2) 
{
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;
	CVector v2s = v2 ;

	DOUBLE  s2 = 0 ;
	DOUBLE  dotprod ;
	CVector cross ;
	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2s*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CQuaternion CQuaternion::operator^(DOUBLE *pVec) 
{
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;
	CVector v2(pVec) ;

	DOUBLE  s2 = 0 ;
	DOUBLE  dotprod ;
	CVector cross ;
	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CQuaternion CQuaternion::operator^(const CCoord &c) 
{
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;

	DOUBLE  s2 = 0 ;
	CVector v2(c.m_x, c.m_y, c.m_z) ;
	CVector cross ;
	DOUBLE  dotprod ;

	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CQuaternion CQuaternion::operator^(const CColorCoord &c) 
{
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;

	DOUBLE  s2 = 0 ;
	CVector v2(c.m_x, c.m_y, c.m_z) ;
	CVector cross ;
	DOUBLE  dotprod ;

	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CQuaternion CQuaternion::operator^(const CColorCoord *c) 
{
	DOUBLE  s1 = m_w ;
	CVector v1(m_x, m_y, m_z) ;

	DOUBLE  s2 = 0 ;
	CVector v2(c->m_x, c->m_y, c->m_z) ;
	CVector cross ;
	DOUBLE  dotprod ;

	cross.CrossProduct(v1, v2) ;
	dotprod = v1.DotProduct(v2) ;

	return CQuaternion ( v2*s1 + v1*s2 + cross, s1*s2-dotprod) ;
}

CVector CQuaternion::ToEuler()
{
	CVector rval ;
	DOUBLE sqw;
	DOUBLE sqx;
	DOUBLE sqy;
	DOUBLE sqz;
	
	DOUBLE rotxrad;
	DOUBLE rotyrad;
	DOUBLE rotzrad;
	
	sqw = m_w * m_w;
	sqx = m_x * m_x;
	sqy = m_y * m_y;
	sqz = m_z * m_z;
	
	rotxrad = (DOUBLE)atan2l(2.0 * ( m_y * m_z + m_x * m_w ) , ( -sqx - sqy + sqz + sqw ));
	rotyrad = (DOUBLE)asinl(-2.0 * ( m_x * m_z - m_y * m_w ));
	rotzrad = (DOUBLE)atan2l(2.0 * ( m_x * m_y + m_z * m_w ) , (  sqx - sqy - sqz + sqw ));
	
	rval.m_dx = rotxrad * (DOUBLE)180.0 / (DOUBLE)PI ;
	rval.m_dy = rotyrad * (DOUBLE)180.0 / (DOUBLE)PI ;
	rval.m_dz = rotzrad * (DOUBLE)180.0 / (DOUBLE)PI ;
	
	return rval;
}


CQuaternion CQuaternion::ConstructRotation(CVector v, DOUBLE theta) 
{
	// first, convert the theta to radians
	theta = theta * (DOUBLE)(DEG_TO_RAD) / (DOUBLE)2.0;
	DOUBLE sintheta = (DOUBLE)sin(theta) ;
	DOUBLE costheta = (DOUBLE)cos(theta) ;
	v.Normalize() ;
	return CQuaternion(sintheta*v.m_dx, sintheta*v.m_dy, sintheta*v.m_dz, costheta) ;
}

CQuaternion CQuaternion::ConstructRotationRad(CVector v, DOUBLE theta)
{
	DOUBLE sintheta = (DOUBLE)sin(theta) ;
	DOUBLE costheta = (DOUBLE)cos(theta) ;
	v.Normalize() ;
	return CQuaternion(sintheta*v.m_dx, sintheta*v.m_dy, sintheta*v.m_dz, costheta) ;
}

DOUBLE CQuaternion::Scalar()
{
	return m_w ;
}

CVector CQuaternion::Vector()
{
	return CVector(m_x, m_y, m_z) ;
}



CMatrix CQuaternion::Matrix3x3()
{
	CMatrix  mat(3,3) ;


    mat.m_pElem[0]  = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_y * m_y + m_z * m_z );
    mat.m_pElem[1]  =       (DOUBLE)2.0 * ( m_x * m_y + m_z * m_w );
    mat.m_pElem[2]  =       (DOUBLE)2.0 * ( m_x * m_z - m_y * m_w );

    mat.m_pElem[3]  =       (DOUBLE)2.0 * ( m_x * m_y - m_z * m_w );
    mat.m_pElem[4]  = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_x * m_x + m_z * m_z );
    mat.m_pElem[5]  =       (DOUBLE)2.0 * ( m_y * m_z + m_x * m_w );

    mat.m_pElem[6]  =       (DOUBLE)2.0 * ( m_x * m_z + m_y * m_w );
    mat.m_pElem[7]  =       (DOUBLE)2.0 * ( m_y * m_z - m_x * m_w );
    mat.m_pElem[8]  = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_x * m_x + m_y * m_y );

	return mat ;
}


CMatrix CQuaternion::Matrix4x4()
{
	CMatrix  mat(4,4) ;

    mat.m_pElem[0]   = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_y * m_y + m_z * m_z );
    mat.m_pElem[1]   =       (DOUBLE)2.0 * ( m_x * m_y + m_z * m_w );
    mat.m_pElem[2]   =       (DOUBLE)2.0 * ( m_x * m_z - m_y * m_w );

    mat.m_pElem[4]   =       (DOUBLE)2.0 * ( m_x * m_y - m_z * m_w );
    mat.m_pElem[5]   = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_x * m_x + m_z * m_z );
    mat.m_pElem[6]   =       (DOUBLE)2.0 * ( m_y * m_z + m_x * m_w );

    mat.m_pElem[8]   =       (DOUBLE)2.0 * ( m_x * m_z + m_y * m_w );
    mat.m_pElem[9]   =       (DOUBLE)2.0 * ( m_y * m_z - m_x * m_w );
    mat.m_pElem[10]  = (DOUBLE)1.0 - (DOUBLE)2.0 * ( m_x * m_x + m_y * m_y );

    mat.m_pElem[3]  = 
		mat.m_pElem[7] = 
		mat.m_pElem[11] = 
		mat.m_pElem[12] = 
		mat.m_pElem[13] = 
		mat.m_pElem[14] = 0.0;
    mat.m_pElem[15] = 1.0;

	return mat ;
}

/*
CMatrix CQuaternion::Matrix3x3()
{
	CMatrix  mat(3,3) ;


    mat.m_pElem[0]  = 1.0 - 2.0 * ( m_y * m_y + m_z * m_z );
    mat.m_pElem[1]  =       2.0 * ( m_x * m_y - m_z * m_w );
    mat.m_pElem[2]  =       2.0 * ( m_x * m_z + m_y * m_w );

    mat.m_pElem[3]  =       2.0 * ( m_x * m_y + m_z * m_w );
    mat.m_pElem[4]  = 1.0 - 2.0 * ( m_x * m_x + m_z * m_z );
    mat.m_pElem[5]  =       2.0 * ( m_y * m_z - m_x * m_w );

    mat.m_pElem[6]  =       2.0 * ( m_x * m_z - m_y * m_w );
    mat.m_pElem[7]  =       2.0 * ( m_y * m_z + m_x * m_w );
    mat.m_pElem[8]  = 1.0 - 2.0 * ( m_x * m_x + m_y * m_y );

	return mat ;
}


CMatrix CQuaternion::Matrix4x4()
{
	CMatrix  mat(4,4) ;

    mat.m_pElem[0]  = 1.0 - 2.0 * ( m_y * m_y + m_z * m_z );
    mat.m_pElem[1]  =       2.0 * ( m_x * m_y - m_z * m_w );
    mat.m_pElem[2]  =       2.0 * ( m_x * m_z + m_y * m_w );

    mat.m_pElem[4]  =       2.0 * ( m_x * m_y + m_z * m_w );
    mat.m_pElem[5]  = 1.0 - 2.0 * ( m_x * m_x + m_z * m_z );
    mat.m_pElem[6]  =       2.0 * ( m_y * m_z - m_x * m_w );

    mat.m_pElem[8]  =       2.0 * ( m_x * m_z - m_y * m_w );
    mat.m_pElem[9]  =       2.0 * ( m_y * m_z + m_x * m_w );
    mat.m_pElem[10] = 1.0 - 2.0 * ( m_x * m_x + m_y * m_y );

    mat.m_pElem[3]  = 
		mat.m_pElem[7] = 
		mat.m_pElem[11] = 
		mat.m_pElem[12] = 
		mat.m_pElem[13] = 
		mat.m_pElem[14] = 0.0;
    mat.m_pElem[15] = 1.0;

	return mat ;
}
*/

bool CCoordFrameArray::GetCoords ( CCoordFrameArray &src
								 , int nCompData
								 , int nCompMask )
{
	int i ;
	int j ;

	SetSize (src.GetSize()) ;
	for (i=0; i<src.GetSize(); i++)
	{
		CCoordArray &dsta = ElementAt(i) ;
		CCoordArray &srca = src.ElementAt(i) ;

		dsta.SetSize(0,128) ;
		for (j=0; j<srca.GetSize(); j++)
		{
			CCoord &c = srca.ElementAt(j) ;
			switch (nCompMask)
			{
			case COORD_FRAME_ARRAY_COMP_EQUAL:
				if (c.m_nData == nCompData)
					dsta.Add(c) ;
				break;
			case COORD_FRAME_ARRAY_COMP_EQUAL | COORD_FRAME_ARRAY_COMP_LESS:
				if (c.m_nData <= nCompData)
					dsta.Add(c) ;
				break;
			case COORD_FRAME_ARRAY_COMP_LESS:
				if (c.m_nData < nCompData)
					dsta.Add(c) ;
				break;
			case COORD_FRAME_ARRAY_COMP_EQUAL | COORD_FRAME_ARRAY_COMP_GREATER:
				if (c.m_nData >= nCompData)
					dsta.Add(c) ;
				break;
			case COORD_FRAME_ARRAY_COMP_GREATER:
				if (c.m_nData > nCompData)
					dsta.Add(c) ;
				break;
			}
		}
	}
	return true ;
}

bool CCoordFrameArray::CopyToMatrix(CMatrix &dst, bool bUse2D)
{
	int i, j, k, n ;

	// Right now, only supporting 1D array where elements
	// are ordered in n rows by 3 columns

	// Get the total number of elements in the frames
	n = TotalSize() ;

	// Resize to n rows and 2 or 3 columns
	dst.Resize ( n, bUse2D ? 2 : 3 ) ;

	// loop through all the data
	for (i=0, k=0; i<GetSize(); i++)
	{
		CCoordArray &arr = ElementAt(i) ;
		CCoord		*c = arr.GetData() ;
		for (j=0; j<arr.GetSize(); j++)
		{
			dst[k][0] = c->m_x ;
			dst[k][1] = c->m_y ;
			if (!bUse2D)
				dst[k][2] = c->m_z ;
			k++;
			c++;
		}
	}

	return true ;
}

long CCoordFrameArray::TotalSize()
{
	long rval ;

	rval = 0 ;
	for (int i=0; i<GetSize(); i++)
	{
		rval += ElementAt(i).GetSize();
	}

	return rval ;
}

void CCoordFrameArray::SetSizeCopy(CCoordFrameArray &rootarray)
{
	CBaseCoordFrameArray::SetSize(rootarray.GetSize()) ;
	for (int i=0; i<rootarray.GetSize(); i++)
	{
		CCoordArray &c = ElementAt(i) ;
		CCoordArray &root = rootarray.ElementAt(i) ;

		c.SetSize(root.GetSize()) ;
	}
}

void CCoordFrameArray::Init(CCoord &copy)
{
	for (int i=0; i<GetSize(); i++)
	{
		CCoordArray &c = ElementAt(i) ;
		CCoord      *pCoords = c.GetData() ;
		for (int j=0; j<c.GetSize(); j++)
		{
			pCoords[j] = copy ;
		}
	}
}


void CCoordFrameArray::Merge()
{
	int i;

	for (i=0; i<GetSize(); i++)
	{
	}
}

CCoordFrameArray & CCoordFrameArray::operator=(CCoordFrameArray& copy)
{
	SetSizeCopy (copy) ;
	for (int i=0; i<GetSize(); i++)
	{
		CCoordArray &c = ElementAt(i) ;
		CCoordArray &root = copy.ElementAt(i) ;

		c = root ;
	}
	return *this;
}

bool CCoord::Epsilon ( CCoord &compval, DOUBLE epsilon)
{
	if ((DOUBLE)fabs(m_x-compval.m_x) > epsilon)
		return false ;
	if ((DOUBLE)fabs(m_y-compval.m_y) > epsilon)
		return false ;
	if ((DOUBLE)fabs(m_z-compval.m_z) > epsilon)
		return false ;
	return true ;
}

bool CLine::Joined ( CLine &testline )
{
	if (   m_first.Epsilon(testline.m_first) 
		|| m_second.Epsilon(testline.m_first) 
		|| m_first.Epsilon(testline.m_second) 
		|| m_second.Epsilon(testline.m_second) )
		return true ;
	else
		return false ;
}

int CLineFrameArray::Longest(int *nSize)
{
	int i ;
	int nBiggest ;
	int nBiggestInd; 
	if (GetSize()<=0)
		return -1 ;

	nBiggest = -1 ;
	nBiggestInd = -1 ;

	for (i=0; i<GetSize(); i++)
	{
		if (ElementAt(i).GetSize() > nBiggest)
		{
			nBiggest = ElementAt(i).GetSize() ;
			nBiggestInd = i;
		}
	}

	if (nBiggestInd<0)
		nBiggest =-1;

	if (nSize)
		*nSize=nBiggest ;
	return nBiggestInd ;
}

// The purpose of this method is to join the segments in an array
// so that one or more contiguous polylines exist.  This works
// well if the dataset (la) is made up of segments that have
// been obtained from a plane slicing through a surface array.
// That way, many of the segments will have equivalent endpoints.
void CLineFrameArray::JoinSegments (CLineArray &la)
{
	CLineArray  temp ;
	CLineArray  src, dst ;
	CLine		*psrc, *pdst ;
	CDWordArray removeflags ;
	int			i, j, k, nRemoveCount ;
	CLine		tempstore[2048] ;
	int			*flagged ;
	int			*flaggedpos ;
	int			nFlaggedCnt ;
	int			nFlaggedInd ;
	int			nFlaggedPos ;
	int			nSize ;
	int			nTempFlagged ;
	int			nFoundCount ;

	// Clear the frame array.  We're starting from scratch.
	SetSize(0) ;
	nSize = la.GetSize() ;
	flagged = new int [nSize] ;
	flaggedpos = new int [nSize] ;
	nFlaggedCnt = 0 ;
	nFlaggedInd = 0 ;
	nRemoveCount = 0 ;

	memset(flagged, 0, nSize*sizeof(int)) ;
	psrc = la.GetData() ;

	do
	{
		// Find the first empty bin, and flag it.
		nFlaggedPos = 0 ;
		for (j=0; j<nSize; j++)
		{
			if (flagged[j]==0)
			{
				psrc[j].m_pPrevious = NULL ;
				flagged[j] = nFlaggedInd+1 ;
				flaggedpos[j] = nFlaggedPos ;
				break;
			}
		}

		nTempFlagged = nFlaggedInd+1 ;

		do
		{
			nFoundCount = 0 ;
			nFlaggedPos++ ;

			// First, loop through all of the bins
			// that have the current search flag
			for (i=0; i<nSize; i++)
			{
				if (flagged[i]==nTempFlagged)
				{
					// If this bin has the current
					// search flag, search through
					// all the non-flagged bins
					// to see if any are connected
					// to it.  If they are, flag
					// them also, and update the
					// flagged count.
					for (j=0; j<nSize; j++)
					{
						if (flagged[j]==0 && psrc[i].Joined(psrc[j]))
						{
							// Save a pointer to the previous
							// join, so we know which direction
							// to grow each of the arrays
							psrc[j].m_pPrevious = psrc+i ;

							// Saving which position we've
							// just added will help in speeding
							// up the sort a little later on...
							flagged[j] = nTempFlagged ;
							flaggedpos[j] = nFlaggedPos ;
							nFoundCount++ ;
						}
					}

					// Make sure that what we just found, we
					// don't search again.
					flagged[i] = -nTempFlagged ;
				}
			}
		}
		while (nFoundCount) ;

		nRemoveCount = 0 ;
		for (i=0; i<nSize; i++)
			if (flagged[i])
				nRemoveCount++ ;

		nFlaggedInd++ ;
	} 
	while (nRemoveCount<nSize) ;

	// We now know how many segments we have, so let's allocate them.
	SetSize(nFlaggedInd) ;
	for (i=0; i<nFlaggedInd; i++)
	{
		nFlaggedCnt = 0 ;

		// How many lines do we have with this index?
		for (j=0; j<nSize; j++)
		{
			if (flagged[j]==-(i+1))
				nFlaggedCnt++ ;
		}

		// Allocate the space
		ElementAt(i).SetSize(nFlaggedCnt) ;
		pdst = ElementAt(i).GetData() ;

		// Loop through all the flags and
		// add the correct elements to the list
		for (j=0, k=0; j<nSize; j++)
		{
			if (flagged[j]==-(i+1))
			{
				pdst[k] = psrc[j] ;
				k++ ;
			}
		}

		// Finally, sort all the elements
		ElementAt(i).JoinSort() ;
	}

	delete flagged ;
	delete flaggedpos ;

	/*
	// Now make sure that all the points are in the
	// right order
	for (i=0; i<GetSize(); i++)
		ElementAt(i).JoinSort() ;
	*/
}

void CLineFrameArray::SetSizeCopy(CLineFrameArray &rootarray)
{
	CBaseLineFrameArray::SetSize(rootarray.GetSize()) ;
	for (int i=0; i<rootarray.GetSize(); i++)
	{
		CLineArray &c = ElementAt(i) ;
		CLineArray &root = rootarray.ElementAt(i) ;

		c.SetSize(root.GetSize()) ;
	}
}

CLineFrameArray & CLineFrameArray::operator=(CLineFrameArray& copy)
{
	SetSizeCopy (copy) ;
	for (int i=0; i<GetSize(); i++)
	{
		CLineArray &c = ElementAt(i) ;
		CLineArray &root = copy.ElementAt(i) ;

		c.Copy(root) ;
	}
	return *this;
}

DOUBLE CCoord::ScalarCrossProduct2D (CVector &a, int nWhichPlane)
{
	return m_x * a.m_dy - a.m_dx * m_y ;
}

DOUBLE CCoord::ScalarCrossProduct2D (CCoord &a, int nWhichPlane)
{
	return m_x * a.m_y - a.m_x * m_y ;
}

DOUBLE CVector::ScalarCrossProduct2D (CVector &a, int nWhichPlane)
{
	return m_dx * a.m_dy - a.m_dx * m_dy ;
}

DOUBLE CVector::ScalarCrossProduct2D (CCoord &a, int nWhichPlane)
{
	return m_dx * a.m_y - a.m_x * m_dy ;
}


CPlane CCoordArray::m_SortPlane ;

//template <> void AFXAPI ConstructElements <CCoord> ( CCoord* pNewPersons, int nCount )
//{
//}

bool CCoordArray::Save(LPCTSTR filename) 
{
/*
	CString TempFileName ;
	CFile   file ;

	if (!filename)
	{
		static char BASED_CODE szFilter[] = "Coord File (*.crd)|*.crd|All Files (*.*)|*.*||";
		CFileDialog fd ( false          // save type dialog
			, "crd"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return false;

		TempFileName = fd.GetPathName() ;
		filename = TempFileName ;
	}

	if (!file.Open(filename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive))
        return false;

	int nSize = GetSize() ;

	file.Write ( &nSize, sizeof (int) ) ;
	for (int i=0; i<GetSize(); i++)
	{
		file.Write ( &ElementAt(i).m_x, sizeof(DOUBLE) ) ;
		file.Write ( &ElementAt(i).m_y, sizeof(DOUBLE) ) ;
		file.Write ( &ElementAt(i).m_z, sizeof(DOUBLE) ) ;
	}

	file.Close() ;

	return true ;
	*/
	return false;
}

bool CCoordFrameArray::Save(LPCTSTR filename) 
{
/*
	CString TempFileName ;
	CFile   file ;

	if (!filename)
	{
		static char BASED_CODE szFilter[] = "Coord File (*.crd)|*.crd|All Files (*.*)|*.*||";
		CFileDialog fd ( false          // save type dialog
			, "crd"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return false;

		TempFileName = fd.GetPathName() ;
		filename = TempFileName ;
	}

	if (!file.Open(filename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive))
        return false;

	int nSize = GetSize() ;

	file.Write ( &nSize, sizeof (int) ) ;
	for (int i=0; i<GetSize(); i++)
	{
		CCoordArray &ca = ElementAt(i) ;
		nSize = ca.GetSize() ;
		file.Write ( &nSize, sizeof(int) ) ;
		for (int j=0; j<nSize; j++)
		{
			file.Write ( &ca.ElementAt(j).m_x, sizeof(DOUBLE) ) ;
			file.Write ( &ca.ElementAt(j).m_y, sizeof(DOUBLE) ) ;
			file.Write ( &ca.ElementAt(j).m_z, sizeof(DOUBLE) ) ;
		}
	}

	file.Close() ;

	return true ;
	*/
	return false ;
}



bool CCoordArray::Load(LPCTSTR filename) 
{
/*
	CString TempFileName ;
	CFile   file ;

	if (!filename)
	{
		static char BASED_CODE szFilter[] = "Coord File (*.crd)|*.crd|All Files (*.*)|*.*||";
		CFileDialog fd ( true          // open type dialog
			, "crd"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return false;

		TempFileName = fd.GetPathName() ;
		filename = TempFileName ;
	}

	if (!file.Open(filename, CFile::modeRead | CFile::shareExclusive))
        return false;

	int nSize ;

	file.Read ( &nSize, sizeof (int) ) ;
	SetSize(nSize) ;
	for (int i=0; i<GetSize(); i++)
	{
		file.Read ( &ElementAt(i).m_x, sizeof(DOUBLE) ) ;
		file.Read ( &ElementAt(i).m_y, sizeof(DOUBLE) ) ;
		file.Read ( &ElementAt(i).m_z, sizeof(DOUBLE) ) ;
	}

	file.Close() ;

	return true ;
	*/
	return false;
}

bool CCoordArray::IsEqual (CCoordArray &ca) 
{
	// Are they the same size?
	if (ca.GetSize() != GetSize())
		return false ;

	// They're the same size, now look at the elements...
	for (int i=0; i<GetSize(); i++)
	{
		if (ca.ElementAt(i).m_x != ElementAt(i).m_x)
			return false ;
		if (ca.ElementAt(i).m_y != ElementAt(i).m_y)
			return false ;
		if (ca.ElementAt(i).m_z != ElementAt(i).m_z)
			return false ;
	}

	return true ;
}

CCoordArray::~CCoordArray()
{
}

void CCoordArray::SetSizeInit(int nNumElem, CCoord &initval)
{
	CBaseCoordArray::SetSize(nNumElem) ;
	CCoord *pc = GetData() ;
	for (int i=0; i<nNumElem; i++,pc++)
		*pc = initval ;
}

bool CCoordArray::CopyToMatrix(CMatrix &dst, bool bUse2D)
{
	int j, n ;

	// Right now, only supporting 1D array where elements
	// are ordered in n rows by 3 columns

	// Get the total number of elements in the frames
	n = GetSize() ;

	// Resize to n rows and 2 or 3 columns
	dst.Resize ( n, bUse2D ? 2 : 3 ) ;

	// loop through all the data
	CCoord	*c = GetData() ;
	for (j=0; j<n; j++)
	{
		dst[j][0] = c->m_x ;
		dst[j][1] = c->m_y ;
		if (!bUse2D)
			dst[j][2] = c->m_z ;
		c++;
	}

	return true ;
}


bool CCoordArray::GetXYPixelBuffer ( CPixelBuffer	&result
								   , int			dx
								   , int			dy
								   , CCoord	        Prec
								   , int			nOffsetAdd
								   , int			nFlags )
{
	int  x,y,z,i,v ;
	CPixelBuffer temp ;

	if (!result.Init (dx, dy))
		return false;
	if (!temp.Init(dx,dy))
		return false ;

	CCoord Min = RangeMin() ;
	CCoord Max = RangeMax() ;

	if (nFlags==0x00000001) 
	{
		temp.Clear() ;
		result.Clear() ;
		temp.ReplaceVal(0,32767) ;
		// Copy highest points
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x + 0.5) ;
			y = (int) (c.m_y / Prec.m_y + 0.5) ;

			if (temp.GetAt(x,y)>z)
			{
				temp.SetAt(x,y,z) ;
				result.SetAt(x,y,z) ;
			}
		}
	}
	else
	if (nFlags==0x00000002) 
	{
		// Copy highest points
		temp.Clear() ;
		result.Clear() ;
		temp.ReplaceVal(0,32767) ;
//		result.ReplaceVal(0,32767) ;
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x + 0.5) ;
			y = (int) (c.m_y / Prec.m_y + 0.5) ;

			if (temp.GetAt(x,y)>z)
			{
				temp.SetAt(x,y,z) ;
				result.SetAt(x,y,255-c.m_nData) ;
			}
		}


	}
	else
	if (nFlags==0x00000003) 
	{
		// Copy highest points
		temp.Clear() ;
		result.Clear() ;
		temp.ReplaceVal(0,32767) ;
//		result.ReplaceVal(0,32767) ;
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x + 0.5) ;
			y = (int) (c.m_y / Prec.m_y + 0.5) ;

			if (temp.GetAt(x,y)>z)
			{
				temp.SetAt(x,y,z) ;
				result.SetAt(x,y,c.m_nData) ;
			}
		}


	}
	else
	if (nFlags==0x00000004) 
	{
		int nCnt = 0 ;
		// Copy highest points
		temp.Clear() ;
		result.Clear() ;
		temp.ReplaceVal(0,32767) ;
		result.ReplaceVal(0,32767) ;
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x + 0.5) ;
			y = (int) (c.m_y / Prec.m_y + 0.5) ;

			if (temp.GetAt(x,y)>z)
			{
				temp.SetAt(x,y,z) ;
				result.SetAt(x,y,c.m_nData) ;
				nCnt++ ;
			}
		}
	}
	else
	{	
		// Copy lowest points
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x + 0.5) ;
			y = (int) (c.m_y / Prec.m_y + 0.5) ;

			v = result.GetAt(x,y) ;
			if (v==0 || v>z)
				result.SetAt(x,y,z) ;
		}
	}
	return true ;
}

/*
bool CCoordArray::GetXYPixelBuffer(CPixelBuffer &result, int dx, int dy, CCoord Prec, int nOffsetAdd, int nFlags)
{
	int  x,y,z,i,v ;

	if (!result.Init (dx, dy))
		return false;

	CCoord Min = RangeMin() ;
	CCoord Max = RangeMax() ;

	if (nFlags&0x00000001 || nFlags&0x00000002) 
	{
		// Copy highest points
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x) ;
			y = (int) (c.m_y / Prec.m_y) ;

			if (result.GetAt(x,y)<z)
				result.SetAt(x,y,z) ;
		}
	}
	else
	{	
		// Copy lowest points
		for (i=0; i<GetSize(); i++)
		{
			CCoord &c = ElementAt(i) ;
			c -= Min ;

			z = (int) (c.m_z / Prec.m_z) + nOffsetAdd ;
			x = (int) (c.m_x / Prec.m_x) ;
			y = (int) (c.m_y / Prec.m_y) ;

			v = result.GetAt(x,y) ;
			if (v==0 || v>z)
				result.SetAt(x,y,z) ;
		}
	}
	return true ;
}*/



CCoord CCoordArray::Farthest(CCoord &testpt, int *pFarthestInd)
{
	DOUBLE dist ;
	DOUBLE farthestdist = (DOUBLE)0.0 ;
	int    farthestind  = -1 ;
	int    i ;

	for (i=0; i<GetSize(); i++)
	{
		dist = ElementAt(i).Distance(testpt) ;
		if (dist > farthestdist)
		{
			farthestdist = dist ;
			farthestind = i ;
		}
	}

	if (pFarthestInd)
		*pFarthestInd = farthestind ;

	if (farthestind>=0)
		return ElementAt(farthestind) ;
	else
		return testpt ;
}


CCoord CCoordArray::Nearest(CCoord &testpt, int *pNearestInd)
{
	DOUBLE dist ;
	DOUBLE nearestdist = (DOUBLE)10000000000000.0 ;
	int    nearestind  = -1 ;
	int    i ;

	for (i=0; i<GetSize(); i++)
	{
		dist = ElementAt(i).Distance(testpt) ;
		if (dist < nearestdist)
		{
			nearestdist = dist ;
			nearestind = i ;
		}
	}

	if (pNearestInd)
		*pNearestInd = nearestind ;

	if (nearestind>=0)
		return ElementAt(nearestind) ;
	else
		return testpt ;
}

bool CCoordArray::Nearest    ( CCoord &rval
							 , const CCoord &testpt
							 , const CVector &searchdirtmp
							 , DOUBLE dAnglePrecision
							 , DOUBLE dDistLow/*=(DOUBLE)-1.0*/
							 , DOUBLE dDistHigh/*=(DOUBLE)-1.0*/
							 , int *pNearestInd
							   )
{
	DOUBLE dist ;
	DOUBLE nearestdist = (DOUBLE)10000000000000.0 ;
	int    nearestind  = -1 ;
	int    i ;
	DOUBLE dotprod ;
	CVector dir ;
	DOUBLE angle ;
	CVector searchdir = searchdirtmp ;

	searchdir.Normalize() ;

	for (i=0; i<GetSize(); i++)
	{
		if (testpt==ElementAt(i))
		{
			rval = testpt ;
			if (pNearestInd)
				*pNearestInd = i ;
			return true ;
		}
		dir.Set(testpt, ElementAt(i)) ;

		dotprod = searchdir.DotProduct (dir) ;
		if (dotprod>(DOUBLE)1.0)
			dotprod = (DOUBLE)1.0 ;
		else
		if (dotprod<(DOUBLE)-1.0)
			dotprod = (DOUBLE)-1.0 ;
		angle = (DOUBLE) (acos ( dotprod ) * RAD_TO_DEG) ;
		dist = ElementAt(i).Distance(testpt) ;

		if (dist==0.0)
		{
			nearestdist = dist ;
			nearestind = i ;
		}
		else
		if (fabs(angle)<dAnglePrecision)
		{
			if (   dist > dDistLow
				&& dist < dDistHigh
				&& dist < nearestdist)
			{
				nearestdist = dist ;
				nearestind = i ;
			}
		}
	}

	if (nearestind>=0)
	{
		if (pNearestInd)
			*pNearestInd = nearestind ;
		rval = ElementAt(nearestind) ;
		return true ;
	}
	else
		return false ;
}

CCoord CCoordArray::Pop (int nIndex)
{
	CCoord rval ;

	if (nIndex<0 || nIndex>=GetSize())
		return rval ;

	rval = ElementAt(nIndex) ;
	RemoveAt(nIndex,1) ;
	return rval ;
}

bool CCoordArray::PopNearest ( CCoord       &rval
                             , const CCoord &testpt
                             , int          *pInd
                             , DOUBLE       dDistLow
                             , DOUBLE       dDistHigh
                               )
{
	DOUBLE dist ;
	DOUBLE nearestdist = (DOUBLE)10000000000000.0 ;
	int    nearestind  = -1 ;
	int    i ;

	for (i=0; i<GetSize(); i++)
	{
		dist = ElementAt(i).Distance(testpt) ;
		if (dist < nearestdist)
		{
			nearestdist = dist ;
			nearestind = i ;
		}
	}

	if (	nearestind>=0
		 && nearestdist>dDistLow
		 && nearestdist<dDistHigh )
	{
		rval = ElementAt(nearestind) ;
		RemoveAt(nearestind) ;
		if (pInd)
			*pInd = nearestind ;
		return true ;
	}
	else
	{
		if (pInd)
			*pInd = -1 ;
		return false ;
	}
}

bool CCoordArray::PopNearest ( CCoord       &rval
                             , const CCoord &testpt
                             , CCoord       &center
                             , DOUBLE       dAngleLow
                             , DOUBLE       dAngleHigh)
{
	DOUBLE dist ;
	DOUBLE nearestdist = (DOUBLE)10000000000000.0 ;
	int    nearestind  = -1 ;
	int    i ;
	double angle ;

	for (i=0; i<GetSize(); i++)
	{
	    CCoord &c = ElementAt(i);
		angle = center.Angle2D(testpt, c) ;
		if (dAngleLow<angle && angle<dAngleHigh)
		{
			dist = ElementAt(i).Distance(testpt) ;
			if (dist < nearestdist)
			{
				nearestdist = dist ;
				nearestind = i ;
			}
		}
	}

	if (nearestind>=0)
	{
		rval = ElementAt(nearestind) ;
		RemoveAt(nearestind) ;
		return true ;
	}
	else
		return false ;
}

bool CCoordArray::PopNearest ( CCoord &rval
							 , const CCoord &testpt
							 , const CVector &searchdirtmp
							 , DOUBLE dAnglePrecision
							 , DOUBLE dDistLow//=(DOUBLE)-1.0
							 , DOUBLE dDistHigh//=(DOUBLE)-1.0
							   )
{
	DOUBLE dist ;
	DOUBLE nearestdist = (DOUBLE)10000000000000.0 ;
	int    nearestind  = -1 ;
	int    i ;
	DOUBLE dotprod ;
	CVector dir ;
	DOUBLE angle ;
	DOUBLE nearestangle ;
	CVector searchdir(searchdirtmp);

	searchdir.Normalize() ;
	nearestangle = 0 ;

	for (i=0; i<GetSize(); i++)
	{
		if (testpt==ElementAt(i))
		{
			rval=testpt ;
			RemoveAt(i) ;
			return true ;
		}
		dir.Set(testpt, ElementAt(i)) ;

		dotprod = searchdir.DotProduct (dir) ;
		if (dotprod>(DOUBLE)1.0)
			dotprod = (DOUBLE)1.0 ;
		else
		if (dotprod<(DOUBLE)-1.0)
			dotprod = (DOUBLE)-1.0 ;
		angle = (DOUBLE) (acos ( dotprod ) * RAD_TO_DEG) ;
		dist = ElementAt(i).Distance(testpt) ;

		if (dist==0.0)
		{
			nearestangle = angle ;
			nearestdist = dist ;
			nearestind = i ;
		}
		else
		if (fabs(angle)<dAnglePrecision)
		{
			if (   dist > dDistLow
				&& dist < dDistHigh
				&& dist < nearestdist)
			{
				nearestangle = angle ;
				nearestdist = dist ;
				nearestind = i ;
			}
		}
	}

	if (nearestind>=0)
	{
		rval = ElementAt(nearestind) ;
		RemoveAt(nearestind) ;
		return true ;
	}
	else
		return false ;
}

/*
CCoordArray::operator VARIANT () 
{
	DWORD			Dims[1] ;
	COleSafeArray	sa ;
	DOUBLE			*pdata ;

	Dims[0] = GetSize() * 3 ;

	if (Dims[0] > 0)
	{
		sa.Create(VT_R8, 1, Dims) ;
		sa.AccessData((void**)&pdata) ;
		memcpy(pdata, GetData(), Dims[0]*sizeof(DOUBLE)) ;
		sa.UnaccessData() ;
	}

	return sa.Detach() ;
}*/

int CCoordArray::AddUnique(CCoord &c)
{
	int i ;
	for (i=0; i<GetSize(); i++)
		if (ElementAt(i)==c)
			return i;

	Add(c) ;
	return m_nSize-1 ;
}

CCoordArray &CCoordArray::operator=(const CCoordArray &src)
{
	Copy(src) ;
	return *this;
}

CCoordArray &CCoordArray::operator=(CMatrix &src)
{
	int n = src.m_nRows ;
	int i ;
	CCoord *pc ;

	SetSize(n) ;
	pc = GetData() ;
	for (i=0; i<n; i++,pc++)
	{
		pc->m_x = src[i][0] ;
		pc->m_y = src[i][1] ;
		pc->m_z = src[i][2] ;
	}

	return *this;
}

void CCoordArray::Rotate(CQuaternion &Rotation, bool bReverse)
{
	CQuaternion RotationInv = ~Rotation ;
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Rotate(Rotation, RotationInv, bReverse) ;
}

void CCoordArray::Offset(CVector &slope, DOUBLE distance)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(slope,distance) ;
}

void CCoordArray::Offset(CVector &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount.m_dx, amount.m_dy, amount.m_dz) ;
}

void CCoordArray::Offset(CCoord &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount.m_x, amount.m_y, amount.m_z) ;
}

CCoord CCoordArray::Average()
{
	CCoord  rval ;
	int		i ;

	for (i=0; i<GetSize(); i++)
	{
		rval += ElementAt(i) ;
	}

	if (GetSize() > 0)
		rval = rval / (DOUBLE)GetSize() ;

	return rval ;
}

CCoord CCoordArray::RangeMin()
{
	CCoord  rval ;
	CCoord  *pCoords = (CCoord*)GetData() ;
	int		i ;

	rval = CCoord(DOUBLE_MAX, DOUBLE_MAX, DOUBLE_MAX) ;

	for (i=0; i<GetSize(); i++)
	{
		if (pCoords[i].m_x < rval.m_x)
			rval.m_x = pCoords[i].m_x ;
		if (pCoords[i].m_y < rval.m_y)
			rval.m_y = pCoords[i].m_y ;
		if (pCoords[i].m_z < rval.m_z)
			rval.m_z = pCoords[i].m_z ;
	}

	return rval ;
}

CCoord CCoordArray::RangeMax()
{
	CCoord  rval ;
	CCoord  *pCoords = (CCoord*)GetData() ;
	int		i ;

	rval = CCoord(-DOUBLE_MAX, -DOUBLE_MAX, -DOUBLE_MAX) ;

	for (i=0; i<GetSize(); i++)
	{
		if (pCoords[i].m_x > rval.m_x)
			rval.m_x = pCoords[i].m_x ;
		if (pCoords[i].m_y > rval.m_y)
			rval.m_y = pCoords[i].m_y ;
		if (pCoords[i].m_z > rval.m_z)
			rval.m_z = pCoords[i].m_z ;
	}

	return rval ;
}

CCoord CCoordArray::Average(int nStart, int nEnd)
{
	CCoord  rval ;
	int		i ;

	if (nEnd==-1)
		nEnd = GetSize()-1 ;

	if (nEnd<nStart)
	{
		int sw = nEnd ;
		nEnd = nStart ;
		nStart = sw ;
	}

	for (i=nStart; i<=nEnd; i++)
	{
		rval += ElementAt(i) ;
	}

	rval = rval / (DOUBLE)(nEnd-nStart+1) ;

	return rval ;
}

void CCoordArray::AddSelection(int sel)
{
	int i ;
	int sz ;

	// Check if this is a repeat
	sz = m_Selection.GetSize() ;
	for (i=0; i<sz; i++)
	{
		if (m_Selection[i]==(unsigned int)sel)
			break;
	}

	if (i<sz)
		return;
	m_Selection.Add(sel) ;
}

void CCoordArray::ClearSelection()
{
	m_Selection.SetSize(0) ;
}

void CCoordArray::DeleteSelections()
{
	CCoordArray base ;
	int			i, j ;
	int			selsz, basesz ;

	selsz = m_Selection.GetSize() ;
	basesz = GetSize() ;

	for (i=0; i<basesz; i++)
	{
		for (j=0; j<selsz; j++)
			if (m_Selection[j]==(unsigned int)i)
				break;

		if (j==selsz)
			base.Add(ElementAt(i)) ;
	}

	SetSize(0) ;
	Copy(base) ;
}

CCoord CCoordArray::m_SortCoord ;

int CCoordArray::Radial2DSortCompare(const void *elem1, const void *elem2)
{
	DOUBLE  d1, d2 ;

	CCoord *p1 = (CCoord*)elem1 ;
	CCoord *p2 = (CCoord*)elem2 ;

	d1 = (DOUBLE)atan2( p1->m_x - m_SortCoord.m_x, p1->m_y - m_SortCoord.m_y ) + (DOUBLE)PI/(DOUBLE)2;
	d2 = (DOUBLE)atan2( p2->m_x - m_SortCoord.m_x, p2->m_y - m_SortCoord.m_y ) + (DOUBLE)PI/(DOUBLE)2;

	if (d1>d2)
		return 1 ;
	else
	if (d1<d2)
		return -1 ;

	return 0 ;
}

void CCoordArray::RadialSort(CCoord &Coord)
{
	m_SortCoord = Coord ;
	if (GetSize())
		qsort(m_pData, GetSize(), sizeof(CCoord), Radial2DSortCompare) ;	
}

int CCoordArray::SortCompare(const void *elem1, const void *elem2)
{
	DOUBLE  d1, d2 ;
	bool	bTop ;

	CCoord *p1 = (CCoord*)elem1 ;
	CCoord *p2 = (CCoord*)elem2 ;

	d1 = m_SortPlane.Distance(*p1, &bTop) ;
	if (bTop)
		d1 *= -1.0;

	d2 = m_SortPlane.Distance(*p2, &bTop) ;
	if (bTop) 
		d2 *= -1.0 ;

	if (d1>d2)
		return 1 ;
	else
	if (d1<d2)
		return -1 ;

	return 0 ;
}

void CCoordArray::Sort(CVector &Vector)
{
	m_SortPlane = CPlane(CCoord(0,0,0), Vector) ;

	if (GetSize())
		qsort(m_pData, GetSize(), sizeof(CCoord), SortCompare) ;	
}

void CCoordArray::Sort(CPlane &Plane)
{
	m_SortPlane = Plane ;

	if (GetSize())
		qsort(m_pData, GetSize(), sizeof(CCoord), SortCompare) ;	
}


void CCoordArray::Sort(CVector &Vector, int start, int len)
{
	m_SortPlane = CPlane(CCoord(0,0,0), Vector) ;

	if (GetSize() && len>=2)
		qsort(m_pData+start, len, sizeof(CCoord), SortCompare) ;	
}

void CCoordArray::Sort(CPlane &Plane, int start, int len)
{
	m_SortPlane = Plane ;

	if (GetSize() && len>=2)
		qsort(m_pData+start, len, sizeof(CCoord), SortCompare) ;	
}

void CCoordArray::TraceDump()
{
#ifdef _DEBUG
	for (int i=0; i<GetSize(); i++)
	{
		CCoord &c=ElementAt(i) ;
	}		
#endif
}


// This trims away all coordinates in the array that are on the same 
// side of the plane as the normal vector.
void CCoordFrameArray::Trim(CPlane &TrimPlane)
{
	for (int i=0; i<GetSize(); i++)
		ElementAt(i).Trim(TrimPlane) ;
}

void CCoordArray::RadialTrim (CCoord &center, double dLessAngle, double dGreaterAngle)
{
	int			i, nSize ;
	CCoordArray rval ;
	CCoord		*pSrc, *pDest ;
	DOUBLE      dAngle ;

	if (GetSize()==0)
		return;

	rval.SetSize(GetSize()) ;
	pSrc = GetData() ;
	pDest = rval.GetData() ;
	nSize = 0 ;

	for (i=0; i<GetSize(); i++)
	{
		dAngle = center.Angle2D ( *pSrc ) ;
		if (dAngle>=dLessAngle && dAngle<=dGreaterAngle)
		{
			*pDest = *pSrc ;
			pDest++;
			nSize++;
		}
		pSrc++;
	}

	rval.SetSize(nSize) ;
	Copy(rval) ;
}

void CCoordArray::Trim(CPlane &TrimPlane)
{
	int			i, nSize ;
	bool		bTop ;
	CCoordArray rval ;
	CCoord		*pSrc, *pDest ;

	if (GetSize()==0)
		return;

	rval.SetSize(GetSize()) ;
	pSrc = GetData() ;
	pDest = rval.GetData() ;

	nSize = 0 ;
	for (i=0; i<GetSize(); i++)
	{
		TrimPlane.Distance(*pSrc, &bTop) ;
		if (!bTop)
		{
			*pDest = *pSrc ;
			pDest++;
			nSize++;
		}
		pSrc++;
	}

	rval.SetSize(nSize) ;
	Copy(rval) ;
}


bool CLongFrameArray::Save(LPCTSTR filename)
{
/*
	CString TempFileName ;
	CFile   file ;

	if (!filename)
	{
		static char BASED_CODE szFilter[] = "Coord File (*.dwd)|*.dwd|All Files (*.*)|*.*||";
		CFileDialog fd ( false          // save type dialog
			, "dwd"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return false;

		TempFileName = fd.GetPathName() ;
		filename = TempFileName ;
	}

	if (!file.Open(filename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive))
        return false;

	int nSize = GetSize() ;

	file.Write ( &nSize, sizeof (int) ) ;
	for (int i=0; i<GetSize(); i++)
	{
		CDWordArray &arr = ElementAt(i) ;
		int			nSubSize = arr.GetSize() ;
		DWORD       *pdata = arr.GetData() ;

		file.Write ( &nSubSize, sizeof (int) ) ;
		file.Write ( pdata, nSubSize*sizeof(DWORD) ) ;
	}

	file.Close() ;

	return true ;*/
	return false;
}

bool CLongFrameArray::Load(LPCTSTR filename)
{
/*
	CString TempFileName ;
	CFile   file ;

	if (!filename)
	{
		static char BASED_CODE szFilter[] = "Coord File (*.dwd)|*.dwd|All Files (*.*)|*.*||";
		CFileDialog fd ( true          // open type dialog
			, "dwd"          // default filter
			, NULL           // initial file path
			, OFN_HIDEREADONLY
			, szFilter       // filter string
			, AfxGetMainWnd() // parent
			) ;
		int rval = fd.DoModal () ;

		if (rval != IDOK)
			return false;

		TempFileName = fd.GetPathName() ;
		filename = TempFileName ;
	}

	if (!file.Open(filename, CFile::modeRead | CFile::shareExclusive))
        return false;

	int nSize = GetSize() ;

	file.Read ( &nSize, sizeof (int) ) ;
	SetSize(nSize) ;

	for (int i=0; i<GetSize(); i++)
	{
		CDWordArray &arr = ElementAt(i) ;
		int			nSubSize ;
		DWORD       *pdata ;

		file.Read ( &nSubSize, sizeof (int) ) ;
		arr.SetSize ( nSubSize ) ;
		pdata = arr.GetData() ;
		file.Read ( pdata, nSubSize*sizeof(DWORD) ) ;
	}

	file.Close() ;

	return true ;*/
	return false;
}

int CLongFrameArray::Compare(CLongFrameArray &cmpfrmarr)
{
	int i, j;

	if (GetSize() != cmpfrmarr.GetSize())
		return 2 ;
	if (GetSize()==0)
		return 5 ;

	for (i=0; i<GetSize(); i++)
	{
		if (ElementAt(i).GetSize() != cmpfrmarr.ElementAt(i).GetSize())
			return 3 ;
	}

	for (i=0; i<GetSize(); i++)
	{
		CDWordArray &arr = ElementAt(i) ;
		CDWordArray &cmparr = cmpfrmarr.ElementAt(i) ;
		DWORD		*pArrData = arr.GetData() ;
		DWORD		*pCmpArrData = cmparr.GetData() ;
		long		nSize = arr.GetSize() ;

		for (j=0; j<nSize; j++)
		{
			if (pArrData[j] != pCmpArrData[j])
				return 4 ;
		}
	}

	return 1 ;

}

void CLongFrameArray::SetSizeCopy(CCoordFrameArray &rootarray)
{
	CBaseLongFrameArray::SetSize(rootarray.GetSize()) ;
	for (int i=0; i<rootarray.GetSize(); i++)
	{
		CDWordArray &c = ElementAt(i) ;
		CCoordArray &root = rootarray.ElementAt(i) ;

		c.SetSize(root.GetSize()) ;
	}
}

void CLongFrameArray::SetSizeCopy(CLongFrameArray &rootarray)
{
	CBaseLongFrameArray::SetSize(rootarray.GetSize()) ;
	for (int i=0; i<rootarray.GetSize(); i++)
	{
		CDWordArray &c = ElementAt(i) ;
		CDWordArray &root = rootarray.ElementAt(i) ;

		c.SetSize(root.GetSize()) ;
	}
}

void CLongFrameArray::Init(DWORD copy)
{
	for (int i=0; i<GetSize(); i++)
	{
		CDWordArray &c = ElementAt(i) ;
		DWORD       *pCoords = c.GetData() ;
		for (int j=0; j<c.GetSize(); j++)
		{
			pCoords[j] = copy ;
		}
	}
}


void CLongFrameArray::Merge()
{
	int i;

	for (i=0; i<GetSize(); i++)
	{
	}
}

CLongFrameArray & CLongFrameArray::operator=(CLongFrameArray& copy)
{
	SetSizeCopy (copy) ;
	for (int i=0; i<GetSize(); i++)
	{
		CDWordArray &c = ElementAt(i) ;
		CDWordArray &root = copy.ElementAt(i) ;

		c.Copy(root) ;
	}
	return *this;
}






CPlane CColorCoordArray::m_SortPlane ;

CColorCoordArray &CColorCoordArray::operator=(const CColorCoordArray &src)
{
	Copy(src) ;
	return *this;
}

void CColorCoordArray::Rotate(CQuaternion &Rotation, bool bReverse)
{
	CQuaternion RotationInv = ~Rotation ;
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Rotate(Rotation, RotationInv, bReverse) ;
}

void CColorCoordArray::Offset(CVector &slope, DOUBLE distance)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(slope,distance) ;
}

void CColorCoordArray::Offset(CVector &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount.m_dx, amount.m_dy, amount.m_dz) ;
}

void CColorCoordArray::Offset(CCoord &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount.m_x, amount.m_y, amount.m_z) ;
}

int CColorCoordArray::SortColorCoordCompare(const void *elem1, const void *elem2)
{
	CColorCoord *p1 = (CColorCoord*)elem1 ;
	CColorCoord *p2 = (CColorCoord*)elem2 ;

	if (p1->m_d>p2->m_d)
		return 1 ;
	else
	if (p1->m_d<p2->m_d)
		return -1 ;

	return 0 ;
}

void CColorCoordArray::Sort(CVector &Vector)
{
	bool bIsTop ;

	m_SortPlane = CPlane(CCoord(0,0,0), Vector) ;
	for (int i=0; i<GetSize(); i++)
	{
		CColorCoord &c = ElementAt(i) ;
		c.m_d = m_SortPlane.Distance(CCoord(c.m_x, c.m_y, c.m_z), &bIsTop) ;
		if (bIsTop)
			c.m_d *= -1.0 ;
	}

	if (GetSize())
		qsort(m_pData, GetSize(), sizeof(CColorCoord), SortColorCoordCompare) ;	
}

void CColorCoordArray::Sort(CPlane &Plane)
{
	bool bIsTop ;

	m_SortPlane = Plane ;
	for (int i=0; i<GetSize(); i++)
	{
		CColorCoord &c = ElementAt(i) ;
		c.m_d = m_SortPlane.Distance(CCoord(c.m_x, c.m_y, c.m_z), &bIsTop) ;
		if (bIsTop)
			c.m_d *= -1.0 ;
	}


	if (GetSize())
		qsort(m_pData, GetSize(), sizeof(CColorCoord), SortColorCoordCompare) ;	
}


void CColorCoordArray::Sort(CVector &Vector, int start, int len)
{
	bool bIsTop ;

	m_SortPlane = CPlane(CCoord(0,0,0), Vector) ;
	for (int i=0; i<GetSize(); i++)
	{
		CColorCoord &c = ElementAt(i) ;
		c.m_d = m_SortPlane.Distance(CCoord(c.m_x, c.m_y, c.m_z), &bIsTop) ;
		if (bIsTop)
			c.m_d *= -1.0 ;
	}


	if (GetSize() && len>=2)
		qsort(m_pData+start, len, sizeof(CColorCoord), SortColorCoordCompare) ;	
}

void CColorCoordArray::Sort(CPlane &Plane, int start, int len)
{
	bool bIsTop ;

	m_SortPlane = Plane ;
	for (int i=0; i<GetSize(); i++)
	{
		CColorCoord &c = ElementAt(i) ;
		c.m_d = m_SortPlane.Distance(CCoord(c.m_x, c.m_y, c.m_z), &bIsTop) ;
		if (bIsTop)
			c.m_d *= -1.0 ;
	}


	if (GetSize() && len>=2)
		qsort(m_pData+start, len, sizeof(CColorCoord), SortColorCoordCompare) ;	
}

// This trims away all coordinates in the array that are on the same 
// side of the plane as the normal vector.
void CColorCoordArray::Trim(CPlane &TrimPlane)
{
	int			i ;
	bool		bTop ;
	CColorCoordArray rval ;

	if (GetSize()==0)
		return;

	for (i=0; i<GetSize(); i++)
	{
		CColorCoord &c = ElementAt(i) ;
		TrimPlane.Distance(CCoord(c.m_x, c.m_y, c.m_z), &bTop) ;
		if (!bTop)
			rval.Add(ElementAt(i)) ;
	}

	SetSize(0) ;
	Copy(rval) ;
}


void CCoordFrameArray::Rotate(CQuaternion &rAmount, bool bReverse)
{
	int i ;
	for (i=0; i<GetSize(); i++)
		ElementAt(i).Rotate(rAmount, bReverse) ;
}

void CCoordFrameArray::Offset(CVector &slope, DOUBLE distance)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(slope,distance) ;
}

void CCoordFrameArray::Offset(CVector &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount) ;
}

void CCoordFrameArray::Offset(CCoord &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount) ;
}

int CLineArray::AddUnique(CCoord &c1, CCoord &c2)
{
	int i ;
	for (i=0; i<GetSize(); i++)
	{
		CLine &Line = ElementAt(i) ;
		if (   (Line.m_first==c1 && Line.m_second==c2)
			|| (Line.m_first==c2 && Line.m_second==c1)
				)
			return i;
	}

    CLine line(c1,c2) ;
	Add(line) ;
	return m_nSize-1 ;
}

CCoord CLineArray::Average()
{
	CCoord  rval ;
	CLine   line ;
	int		i ;

	for (i=0; i<GetSize(); i++)
	{
		line = ElementAt(i) ;
		rval += line.m_first ;
		rval += line.m_second ;
	}

	if (GetSize() > 0)
		rval = rval / (DOUBLE)GetSize() / (DOUBLE)2.0 ;

	return rval ;
}


CLine CLineArray::Extents()
{
	CLine rval ;
	CCoord maxc(-1e10,-1e10,-1e10) ;
	CCoord minc(1e10,1e10,1e10) ;
	if (GetSize()>1)
	{
		int i ;
		for (i=0; i<GetSize(); i++)
		{
			CLine &lval = ElementAt(i) ;
			if (lval.m_first.m_x<minc.m_x)
				minc.m_x = lval.m_first.m_x ;
			if (lval.m_first.m_y<minc.m_y)
				minc.m_y = lval.m_first.m_y ;
			if (lval.m_first.m_z<minc.m_z)
				minc.m_z = lval.m_first.m_z ;

			if (lval.m_first.m_x>maxc.m_x)
				maxc.m_x = lval.m_first.m_x ;
			if (lval.m_first.m_y>maxc.m_y)
				maxc.m_y = lval.m_first.m_y ;
			if (lval.m_first.m_z>maxc.m_z)
				maxc.m_z = lval.m_first.m_z ;

			if (lval.m_second.m_x<minc.m_x)
				minc.m_x = lval.m_second.m_x ;
			if (lval.m_second.m_y<minc.m_y)
				minc.m_y = lval.m_second.m_y ;
			if (lval.m_second.m_z<minc.m_z)
				minc.m_z = lval.m_second.m_z ;

			if (lval.m_second.m_x>maxc.m_x)
				maxc.m_x = lval.m_second.m_x ;
			if (lval.m_second.m_y>maxc.m_y)
				maxc.m_y = lval.m_second.m_y ;
			if (lval.m_second.m_z>maxc.m_z)
				maxc.m_z = lval.m_second.m_z ;
		}

		rval.Set(minc,maxc) ;
	}
	return rval ;
}

void CLineArray::Trim(CPlane &TrimPlane)
{
	int			i, nSize ;
	bool		bTop1, bTop2 ;
	CLineArray  rval ;
	CLine		*pSrc, *pDest ;

	if (GetSize()==0)
		return;

	rval.SetSize(GetSize()) ;
	pSrc = GetData() ;
	pDest = rval.GetData() ;

	nSize = 0 ;
	for (i=0; i<GetSize(); i++)
	{
		TrimPlane.Distance(pSrc->m_first,  &bTop1) ;
		TrimPlane.Distance(pSrc->m_second, &bTop2) ;

		if (!bTop1 && !bTop2)
		{
			*pDest = *pSrc ;
			pDest++;
			nSize++;
		}
		pSrc++;
	}

	rval.SetSize(nSize) ;
	Copy(rval) ;
}

void CLineArray::Rotate(CQuaternion &rAmount, bool bReverse)
{
	int i ;
	for (i=0; i<GetSize(); i++)
	{
		CLine &line = ElementAt(i) ;
		line.m_first.Rotate(rAmount, bReverse) ;
		line.m_second.Rotate(rAmount, bReverse) ;
	}
}

void CLineArray::Offset(CVector &slope, DOUBLE distance)
{
	int i ;
	for (i=0; i<GetSize(); i++)
	{
		CLine &line = ElementAt(i) ;
		line.m_first.Offset(slope, distance) ;
		line.m_second.Offset(slope, distance) ;
	}
}

void CLineArray::Offset(CVector &amount)
{
	int i ;
	for (i=0; i<GetSize(); i++)
	{
		CLine &line = ElementAt(i) ;
		line.m_first += amount ;
		line.m_second += amount ;
	}
}

void CLineArray::Offset(CCoord &amount)
{
	int i ;
	for (i=0; i<GetSize(); i++)
	{
		CLine &line = ElementAt(i) ;
		line.m_first += amount ;
		line.m_second += amount ;
	}
}

void CLineFrameArray::Rotate(CQuaternion &rAmount, bool bReverse)
{
	int i ;
	for (i=0; i<GetSize(); i++)
		ElementAt(i).Rotate(rAmount, bReverse) ;
}

void CLineFrameArray::Offset(CVector &slope, DOUBLE distance)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(slope,distance) ;
}

void CLineFrameArray::Offset(CVector &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount) ;
}

void CLineFrameArray::Offset(CCoord &amount)
{
	int			i ;

	for (i=0; i<GetSize(); i++)
		ElementAt(i).Offset(amount) ;
}









CMatrix::CMatrix() 
{
	m_nCols = m_nRows = m_nStep = 0 ;
	m_bReference = false ;
	m_pElem = NULL ;
}

CMatrix::CMatrix(const CMatrix &c) 
{
	m_nCols = m_nRows = m_nStep = 0 ;
	m_bReference = false ;
	m_pElem = NULL ;
	*this = c ;
}

CMatrix::CMatrix(const char * flag, int dimension)
{ 
	DOUBLE init=0.0;
	int i,j;
	
	m_nCols = m_nRows = m_nStep = 0 ;
	m_bReference = false ;
	Resize(dimension,dimension) ;
		
	switch (flag[0])
	{
	case 'I':
		for (i=0; i< dimension; i++)
		{
			for (j = 0; j < dimension; j++) 
				(*this)[i][j] = (i == j ? (DOUBLE)1.0 : 0);
		}
		break;
	case 'D':
		for (i=0; i< dimension; i++)
		{
			for (j = 0; j < dimension; j++) 
				(*this)[i][j] = (i == j ? init : 0);
		}
		break;
	case 'S':
		for (i=0; i< dimension; i++)
		{
			for (j = 0; j < dimension; j++) 
				(*this)[i][j] = (i == j ? 0 : init);
		}
		
		break;
	}
} 

int CMatrix::Index ( int nRow
				 , int nCol )
{
	return nRow*m_nStep + nCol ;
}

bool CMatrix::GetSubMatrix ( CMatrix &src
						 , int nRowS
						 , int nColS
						 , int nRowF
						 , int nColF
						 , bool bReference // = false
						     )
{
	if (m_pElem && !m_bReference)
	{
		delete m_pElem ;
		m_pElem = NULL ;
	}

	if (bReference)
	{
		m_nRows = nRowF ;
		m_nCols = nColF ;
		m_nStep = src.m_nStep ;
		m_pElem = src.m_pElem + src.Index(nRowS, nColS) ;
		m_bReference = true ;
	}
	else
	{
		int sr, sc, r, c ;

		Resize(nRowF, nColF) ;
		for (sr=nRowS, r=0; sr<nRowS+nRowF; sr++, r++)
		{
			for (sc=nColS, c=0; sc<nColS+nColF; sc++, c++)
			{
				(*this)[r][c] = src[sr][sc] ;
			}
		}
	}
	return true ;
}

void CMatrix::Resize(int nRows, int nCols, DOUBLE *pData, bool bReference/*=false*/)
{
	m_nCols = m_nRows = 0 ;
	if (m_bReference==false)
	{
		if (m_pElem)
			delete m_pElem ;
	}
	m_pElem = NULL ;

	if (bReference)
	{
		m_pElem = pData ;
		m_nStep = nCols ;
		m_nCols = nCols ;
		m_nRows = nRows ;
		m_bReference = bReference ;
	}
	else
	{
		m_pElem = NULL ;
		Resize(nRows, nCols) ;
	}
}

void CMatrix::Resize(int nRows, int nCols)
{
	if (m_bReference==false)
	{
		if (m_pElem)
			delete m_pElem ;
	}
	m_pElem = NULL ;

	m_nRows = nRows ;
	m_nCols = nCols ;
	m_nStep = nCols ;
	m_pElem = new DOUBLE[m_nRows*m_nCols] ;
	m_bReference = false ;
}

void CMatrix::Resize(CMatrix &src)
{
	Resize(src.m_nRows, src.m_nCols) ;
}

CMatrix &CMatrix::operator=(const CMatrix &cpy)
{
	// If we're equal, or if this is a reference
	if (   m_nRows*m_nCols==cpy.m_nRows*cpy.m_nCols
		&& m_nRows * m_nCols>=1
		   )
	{
		m_nRows = cpy.m_nRows ;
		m_nCols = cpy.m_nCols ;
		for (int r=0; r<m_nRows; r++)
			for (int c=0; c<m_nCols; c++)
				(*this)[r][c] = cpy[r][c];

		return *this;
	}
	
	Resize (cpy.m_nRows, cpy.m_nCols) ;
	memcpy (m_pElem, cpy.m_pElem, m_nRows*m_nCols*sizeof(DOUBLE)) ;
	return *this;
}

CMatrix::CMatrix ( int		nRows
			 , int		nCols	   // = 1
			 , DOUBLE	*pInitData // = NULL
			 , bool		bReference // = false
				   )
{
	m_nCols = m_nRows = 0 ;
	if (bReference)
	{
		m_pElem = pInitData ;
		m_nStep = nCols ;
		m_nCols = nCols ;
		m_nRows = nRows ;
		m_bReference = bReference ;
	}
	else
	{
		m_pElem = NULL ;
		Resize(nRows, nCols) ;
	}
}

CMatrix::~CMatrix() 
{
	if (m_pElem && !m_bReference)
		delete m_pElem ;
	m_pElem = NULL ;
}

CVector CMatrix::Mult(CVector &mulv) 
{
	if (m_nCols==3 && m_nRows==3)
	{
		return 
		CVector(mulv.m_dx * m_pElem[0] + mulv.m_dy * m_pElem[3] + mulv.m_dz * m_pElem[6]
			 , mulv.m_dx * m_pElem[1] + mulv.m_dy * m_pElem[4] + mulv.m_dz * m_pElem[7]
			 , mulv.m_dx * m_pElem[2] + mulv.m_dy * m_pElem[5] + mulv.m_dz * m_pElem[8] 
			    ) ;
	}
	else
	if (m_nCols==4 && m_nRows==4)
	{
		return
		CVector(mulv.m_dx * m_pElem[0] + mulv.m_dy * m_pElem[4] + mulv.m_dz * m_pElem[8]  + m_pElem[12]
			 , mulv.m_dx * m_pElem[1] + mulv.m_dy * m_pElem[5] + mulv.m_dz * m_pElem[9]  + m_pElem[13]
			 , mulv.m_dx * m_pElem[2] + mulv.m_dy * m_pElem[6] + mulv.m_dz * m_pElem[10] + m_pElem[14] 
			    ) ;
	}

	CVector v ;

	return v ;
}

CCoord CMatrix::Mult(CCoord &mulv) 
{
	if (m_nCols==3 && m_nRows==3)
	{
		return CCoord ( mulv.m_x * m_pElem[0] + mulv.m_y * m_pElem[3] + mulv.m_z * m_pElem[6]
					 , mulv.m_x * m_pElem[1] + mulv.m_y * m_pElem[4] + mulv.m_z * m_pElem[7]
					 , mulv.m_x * m_pElem[2] + mulv.m_y * m_pElem[5] + mulv.m_z * m_pElem[8] 
						) ;
	}
	else
	if (m_nCols==4 && m_nRows==4)
	{
		return CCoord ( mulv.m_x * m_pElem[0] + mulv.m_y * m_pElem[4] + mulv.m_z * m_pElem[8]  + m_pElem[12]
					 , mulv.m_x * m_pElem[1] + mulv.m_y * m_pElem[5] + mulv.m_z * m_pElem[9]  + m_pElem[13]					 , mulv.m_x * m_pElem[2] + mulv.m_y * m_pElem[6] + mulv.m_z * m_pElem[10] + m_pElem[14] 
						) ;
	}

	CCoord v;
	return v ;
}

CString CMatrix::m_dumpstr ;

DOUBLE *CMatrix::operator [] (int row) const
{
	return (DOUBLE*)m_pElem + row*m_nStep;
}

DOUBLE &CMatrix::operator () (int row)
{
	return *((DOUBLE*)m_pElem + row);
}

bool CMatrix::MulTransposed(CMatrix &src, CMatrix *pDelta)
{
	CMatrix transpose ;

	transpose.Transpose(src) ;
	
	if (pDelta)
		*this = transpose * (src - *pDelta);
	else
		*this = transpose * src ;

	return true ;
}

bool CMatrix::MulTransposed(CMatrix &src, int nOrder)
{
	CMatrix transpose ;

	transpose.Transpose(src) ;

	if (nOrder==0)
		*this = transpose * src ;
	else
		*this = src * transpose;

	return true ;
}

CMatrix CMatrix::operator *(const CMatrix &op)
{
	int nRow, nCol, m ;

	// cols must equal op.rows
	// our result size will be rows x op.cols
	CMatrix rval (m_nRows, op.m_nCols) ;

	for (nRow=0; nRow<rval.m_nRows; nRow++)
	{
		for (nCol=0; nCol<rval.m_nCols; nCol++)
		{
			DOUBLE sum ;

			sum = 0 ;
			for (m=0; m<op.m_nRows; m++)
			{
				sum += (*this)[nRow][m] * op[m][nCol] ;
			}
			rval[nRow][nCol] = sum ;
		}
	}

	return rval;
}

int CMatrix::NumElements()
{
	return m_nRows * m_nCols ;
}

CMatrix CMatrix::operator *(DOUBLE scale)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix rval (m_nRows, m_nCols) ;

	for (nRow=0; nRow<m_nRows; nRow++)
	{
		for (nCol=0; nCol<m_nCols; nCol++)
		{
			rval[nRow][nCol] = (*this)[nRow][nCol] * scale ;
		}
	}

	return rval;
}

CMatrix CMatrix::operator +(DOUBLE delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix rval (m_nRows, m_nCols) ;

	for (nRow=0; nRow<m_nRows; nRow++)
	{
		for (nCol=0; nCol<m_nCols; nCol++)
		{
			rval[nRow][nCol] = (*this)[nRow][nCol] + delta ;
		}
	}

	return rval;
}

CMatrix CMatrix::operator -(DOUBLE delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix rval (m_nRows, m_nCols) ;

	for (nRow=0; nRow<m_nRows; nRow++)
	{
		for (nCol=0; nCol<m_nCols; nCol++)
		{
			rval[nRow][nCol] = (*this)[nRow][nCol] - delta ;
		}
	}

	return rval;
}

CMatrix CMatrix::operator +(const CMatrix &delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix rval = *this ;

	if (delta.m_nCols!=1 && delta.m_nRows==1)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] += delta[0][nCol] ;
			}
		}
	}
	else
	if (delta.m_nCols==m_nCols && delta.m_nRows==m_nRows)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] += delta[nRow][nCol] ;
			}
		}
	}

	return rval;
}

CMatrix& CMatrix::operator +=(CMatrix &delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix& rval = *this ;

	if (delta.m_nCols!=1 && delta.m_nRows==1)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] += delta[0][nCol] ;
			}
		}
	}
	else
	if (delta.m_nCols==m_nCols && delta.m_nRows==m_nRows)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] += delta[nRow][nCol] ;
			}
		}
	}

	return rval;
}

CMatrix& CMatrix::operator -=(CMatrix &delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix& rval = *this ;

	if (delta.m_nCols!=1 && delta.m_nRows==1)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] -= delta[0][nCol] ;
			}
		}
	}
	else
	if (delta.m_nCols==m_nCols && delta.m_nRows==m_nRows)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] -= delta[nRow][nCol] ;
			}
		}
	}

	return rval;
}

CMatrix CMatrix::operator -(CMatrix &delta)
{
	int nRow, nCol ;

	// our result size will be rows x op.cols
	CMatrix rval (m_nRows, m_nCols) ;

	if (m_nRows != delta.m_nRows)
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] = (*this)[nRow][nCol] - delta[0][nCol] ;
			}
		}
	}
	else
	{
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				rval[nRow][nCol] = (*this)[nRow][nCol] - delta[nRow][nCol] ;
			}
		}
	}

	return rval;
}

bool CMatrix::Transpose(CMatrix &src)
{
	if (src.m_nRows==0 || src.m_nCols==0)
		return false ;

	Resize(src.m_nCols, src.m_nRows) ;
	for (int row = 0; row < m_nRows; row++)
	{
		for(int col = 0; col < m_nCols; col++)
		{
			(*this)[row][col] = src[col][row];
		}
	}

	return true ;
}

CMatrix &CMatrix::TransposeSelf()
{
	// if(m_nRows != m_nCols)
	//   error("matrix must be square to Transpose!\n");
	CMatrix trans(m_nCols,m_nRows);

	for (int row = 0; row < m_nRows; row++)
	{
		for(int col = 0; col < m_nCols; col++)
		{
			trans[col][row] = (*this)[row][col];
		}
	}

	memcpy (m_pElem, trans.m_pElem, sizeof(DOUBLE)*m_nCols*m_nRows) ;
	m_nCols = trans.m_nCols ;
	m_nRows = trans.m_nRows ;
	m_nStep = trans.m_nStep ;
	m_bReference = trans.m_bReference ;

	return *this;
}

CMatrix CMatrix::Transpose()
{
	// if(m_nRows != m_nCols)
	//   error("matrix must be square to Transpose!\n");
	CMatrix trans(m_nCols,m_nRows);

	for (int row = 0; row < m_nRows; row++)
	{

		for(int col = 0; col < m_nCols; col++)
		{
			trans[col][row] = (*this)[row][col];
		}
	}

	return trans;
}

DOUBLE CMatrix::Determinant()
{
	CMatrix indx(m_nCols); // create the "index vector"
	int d;
	// perform the decomposition once:
	CMatrix decomp ;
	decomp.lu_decompose(*this, indx,d);

	double determinant = d;
	for(int i=0; i < m_nCols ; i++)
		determinant *= decomp[i][i];
	return (DOUBLE)determinant;
}

void CMatrix::SetIdentity(DOUBLE v)
{
	int nRow, nNumVals ;

	nNumVals = m_nRows<m_nCols ? m_nRows : m_nCols ;
	Zero() ;
	for (nRow = 0;  nRow < nNumVals; nRow++)
		(*this)[nRow][nRow] = v ;
}

CMatrix CMatrix::Inverse()
{
	CMatrix Y("I",m_nRows); // create an identity matrix  
	CMatrix indx(m_nCols); // create the "index vector"  
	CMatrix B(m_nCols); // see Press & Flannery 
	int d; 
	// perform the decomposition once: 
	CMatrix decomp ;
	decomp.lu_decompose(*this,indx,d); 
	for(int col = 0; col < m_nCols; col++){ 
		B.copy_column(Y,col,0); 
		decomp.lu_back_subst(indx,B); 
		Y.copy_column(B,0,col); 
	} 
	return Y.Transpose(); 
} 

void CMatrix::copy_column(CMatrix& mm, int from_col, int to_col)
{
	for(int row=0; row < m_nRows; row++) 
		(*this)[row][to_col] = mm[row][from_col]; 
} 

void CMatrix::switch_columns(int col1, int col2)
{
	int row;
	CMatrix temp(m_nRows,1); 
	for(row = 0; row < m_nRows; row++) 
		// temporarily store col 1: 
		temp[row][0] = (*this)[row][col1];  
	for(row = 0; row < m_nRows; row++) 
		(*this)[row][col1] = (*this)[row][col2]; // move col2 to col1  
	for(row = 0; row < m_nRows; row++) 
		(*this)[row][col2] = temp[row][0]; // move temp to col2  
} 

// make an image of a matrix (used in L-U decomposition) 
void CMatrix::deepcopy(CMatrix& from, CMatrix& to)
{ 
	for(int row = 0; row < from.m_nRows; row++) 
	{
		for(int col = 0; col < from.m_nCols; col++) 
			to[row][col] = from[row][col]; 
	}
} 

// scale a matrix (used in L-U decomposition)  
CMatrix CMatrix::scale()
{ 
	DOUBLE temp; 
	CMatrix scale_vector(m_nRows); 
	for (int col = 0; col < m_nCols; col++)
	{ 
		DOUBLE maximum = 0; 
		for(int row = 0; row < m_nRows; row++) 
			if ((temp = (DOUBLE)fabs((*this)[row][col])) > maximum) 
				maximum = temp;  // find max column magnitude in this row
			scale_vector[col][0] = 1/maximum; // save the scaling  
	} 
	return scale_vector; 
} 

bool CMatrix::lu_decompose(CMatrix &src, CMatrix& indx, int& d )
{ 
/* 
Returns the L-U decomposition of a matrix. indx is an output 
vector which records the row permutation effected by the  
partial pivoting, d is output as +-1 depending on whether the 
number of row interchanges was even or odd, respectively.   
This routine is used in combination with lu_back_subst to  
solve linear equations or invert a matrix. 
	*/ 
	if (src.m_nRows != src.m_nCols) 
		return false ;

	indx.Resize(src.m_nRows, 1) ;
	d = 1; // parity check  
	int row,col,k,col_max; // counters  
	DOUBLE dum; // from the book -- I don't know significance  
	DOUBLE sum; 
	DOUBLE maximum; 
	// make a direct copy of the original matrix: 
	*this = src ;

	CMatrix scale_vector = (*this).scale(); // scale the matrix  
	// The loop over columns of Crout's method:  
	for(row = 0; row < m_nRows; row++)
	{  
		if (row > 0) 
		{ 
			// eqn 2.3.12 except for row=col:  
			for (col = 0; col <= row-1; col++) 
			{  
				sum = (*this)[row][col]; 
				if(col > 0) 
				{ 
					for(k = 0; k <= col-1; k++) 
						sum -= (*this)[row][k]*(*this)[k][col]; 
					(*this)[row][col] = sum; 
				} 
			} 
		} 
		// Initialize for the search for the largest pivot element: 
		maximum = 0;  
		// i=j of eq 2.3.12 & i=j+1..N of 2.3.13: 
		for(col=row; col <= m_nCols-1; col++)
		{  
			sum = (*this)[row][col]; 
			if(row > 0)
			{ 
				for(k=0; k <= row-1; k++) 
					sum -=  (*this)[k][col] * (*this)[row][k]; 
				(*this)[row][col] = sum; 
			} 
			// figure of merit for pivot: 
			dum = scale_vector[col][0] * (DOUBLE)fabs(sum); 
			if (dum >= maximum)
			{ // is it better than the best so far?
				col_max = col; 
				maximum = dum; 
			} 
		} 
		// Do we need to interchange rows?  
		if(row != col_max) 
		{ 
			(*this).switch_columns(col_max,row); // Yes, do so...
			d *= -1;  // ... and change the parity of d  
			// also interchange the scale factor: 
			dum = scale_vector[col_max][0];  
			scale_vector[col_max][0] = scale_vector[row][0]; 
			scale_vector[row][0] = dum;  
		} 
		indx[row][0] = (DOUBLE)col_max; 
		// Now, finally, divide by the pivot element: 
		if(row != m_nRows -1)
		{   
			if((*this)[row][row] == 0)  
				(*this)[row][row] = (DOUBLE)1e-20;
			// If the pivot element is zero the matrix is  
			// singular (at least to the precision of the  
			// algorithm).  For some applications on singular  
			// matrices, it is desirable to substitute tiny for zero 
			dum = (DOUBLE)1.0/(*this)[row][row]; 
			for(col=row+1; col <= m_nCols-1; col++)
				(*this)[row][col] *= dum; 
		} 
	} 
	if((*this)[m_nRows-1][m_nCols-1] == 0)  
		(*this)[m_nRows-1][m_nCols-1] = (DOUBLE)1e-20;
	return true ;
} 

void CMatrix::lu_back_subst(CMatrix& indx, CMatrix& b)
{ 
/*  
Solves the set of N linear equations A*X = B.  Here "this"  
is the LU-decomposition of the matrix A, determined by the 
routine lu_decompose(). Indx is input as the permutation  
vector returned  by lu_decompose().  B is input as the  
right-hand side vector B,  and returns with the solution  
vector X.  This routine takes into  account the possibility  
that B will begin with many zero elements,  so it is efficient 
for use in matrix inversion.   See pp 36-37 in  
Press & Flannery. 
	*/  
//		error ("non-square lu_decomp matrix in lu_back_subst()"); 
//		error("wrong size B vector passed to lu_back_subst()"); 
//		error("wrong size indx vector passed to lu_back_subst()"); 
	int row,col,ll; 
	int ii = 0; 
	DOUBLE sum; 
	for(col=0;col < m_nCols; col++)
	{ 
		ll= (int)indx[col][0]; 
		sum = b[ll][0]; 
		b[ll][0] = b[col][0]; 
		if (ii >= 0) 
			for(row = ii; row <= col-1; row++) 
				sum -= (*this)[row][col] * b[row][0]; 
			else if(sum != 0) 
				ii = col; 
			b[col][0] = sum; 
	} 
	for(col = m_nCols -1; col >= 0; col--)
	{ 
		sum = b[col][0]; 
		if (col < m_nCols -1) 
			for (row = col + 1; row <= m_nRows-1; row++) 
				sum -= (*this)[row][col] * b[row][0]; 
			// store a component of the soln vector X: 
			b[col][0] = sum/(*this)[col][col];  
	} 
}


bool CMatrix::GetColumns(CMatrix &src, int nStart, int nFinish, bool bReference)
{
	int nRow, nCol, i ;

	if (nFinish<=nStart)
		return false ;
	if (nFinish>src.m_nCols)
		return false ;
	if (nStart>src.m_nCols)
		return false ;
	if (nFinish<1 || nStart<0)
		return false ;
	if (src.m_nCols<=0 || src.m_nRows<=0)
		return false ;

	if (bReference)
	{
		if (m_bReference==false && m_pElem)
			delete m_pElem ;
		m_pElem = src.m_pElem + nStart;
		m_nStep = src.m_nStep ;
		m_nCols = nFinish - nStart ;
		m_nRows = src.m_nRows ;
		m_bReference = true ;
	}
	else
	{
		Resize(src.m_nRows, nFinish-nStart) ;
		for (nRow=0; nRow<src.m_nRows; nRow++)
		{
			for (nCol=nStart, i=0; nCol<nFinish; nCol++,i++)
			{
				(*this)[nRow][i] = src[nRow][nCol] ;
			}
		}
	}
	return true ;
}

bool CMatrix::GetRows(CMatrix &src, int nStart, int nFinish, bool bReference)
{
	int nRow, nCol, i ;

	if (nFinish<=nStart)
		return false ;
	if (nFinish>src.m_nRows)
		return false ;
	if (nStart>src.m_nRows)
		return false ;
	if (nFinish<1 || nStart<0)
		return false ;
	if (src.m_nCols<=0 || src.m_nRows<=0)
		return false ;

	if (bReference)
	{
		if (m_bReference==false && m_pElem)
			delete m_pElem ;
		m_pElem = src.m_pElem + nStart*src.m_nCols ;
		m_nStep = src.m_nStep ;
		m_nRows = nFinish - nStart ;
		m_nCols = src.m_nCols ;
		m_bReference = true ;
	}
	else
	{
		Resize(nFinish-nStart, src.m_nCols) ;
		for (nRow=nStart,i=0; nRow<nFinish; nRow++,i++)
		{
			for (nCol=0; nCol<m_nCols; nCol++)
			{
				(*this)[i][nCol] = src[nRow][nCol] ;
			}
		}
	}
	return true ;
}

bool CMatrix::AvgCols (CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;

	Resize(1, src.m_nCols) ;

	for (i=0; i<src.m_nCols; i++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (j=0; j<src.m_nRows; j++)
		{
			val = src[i][j];
			N++ ;	
			sum += val ;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[0][j] = sum / (DOUBLE)N ;
		else
			(*this)[0][j] = 0.0 ;
	}

	return true ;
}

bool CMatrix::AvgRows(CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;

	Resize(src.m_nRows, 1) ;

	for (j=0; j<m_nRows; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nCols; i++)
		{
			val = src[j][i];
			N++ ;	
			sum += val ;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[i][0] = sum / (DOUBLE)N ;
		else
			(*this)[0][j] = 0.0 ;
	}

	return true ;
}

bool CMatrix::StandardDeviationCols (CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;
	DOUBLE  diff ;

	this->AvgCols(src) ;

	for (j=0; j<src.m_nCols; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nRows; i++)
		{
			val = src[i][j];
			N++ ;	
			diff = val - (*this)[0][j] ;
			sum += diff*diff ;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[0][j] = (DOUBLE) sqrt(sum / (DOUBLE)N) ;
		else
			(*this)[0][j] = (DOUBLE)0.0 ;
	}

	return true ;
}

bool CMatrix::StandardDeviationRows(CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;
	DOUBLE  diff ;

	this->AvgRows(src) ;

	for (j=0; j<m_nRows; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nCols; i++)
		{
			val = src[j][i];
			N++ ;
			diff = val - (*this)[i][0] ;
			sum += diff * diff;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[i][0] = (DOUBLE) sqrt(sum / (DOUBLE)N) ;
		else
			(*this)[i][0] = (DOUBLE)0.0 ;
	}

	return true ;
}

bool CMatrix::AvgStandardDeviationCols (CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;
	DOUBLE  diff ;

	Resize(2, src.m_nCols) ;

	for (j=0; j<src.m_nCols; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nRows; i++)
		{
			val = src[i][j];
			sum += val ;
			N++ ;	
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[0][j] = sum / (DOUBLE)N ;
		else
			(*this)[0][j] = 0.0 ;
	}


	for (j=0; j<src.m_nCols; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nRows; i++)
		{
			val = src[i][j];
			diff = val - (*this)[0][j] ;
			sum += diff*diff ;
			N++ ;	
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N>1)
			(*this)[1][j] = (DOUBLE) sqrt(sum / (DOUBLE)(N)) ;
		else
			(*this)[1][j] = (DOUBLE)0.0 ;
	}

	return true ;
}

bool CMatrix::AvgStandardDeviationRows(CMatrix &src)
{
	int     N ;
	int		i ;
	int		j ;
	DOUBLE  sum ;
	DOUBLE  val ;
	DOUBLE  diff ;

	Resize(src.m_nRows, 2) ;

	for (j=0; j<m_nRows; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nCols; i++)
		{
			val = src[j][i];
			N++ ;	
			sum += val ;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[i][0] = sum / (DOUBLE)N ;
		else
			(*this)[0][j] = 0.0 ;
	}


	for (j=0; j<m_nRows; j++)
	{
		N = 0 ;
		sum = 0.0 ;
		for (i=0; i<src.m_nCols; i++)
		{
			val = src[j][i];
			N++ ;
			diff = val - (*this)[i][0] ;
			sum += diff * diff;
		}

		// Put the average into the proper channel
		// (i.e. column).
		if (N)
			(*this)[i][1] = (DOUBLE) sqrt(sum / (DOUBLE)N) ;
		else
			(*this)[i][1] = (DOUBLE)0.0 ;
	}

	return true ;
}

bool CMatrix::Zero()
{
	if (   m_nCols<=0
		|| m_nRows<=0
		|| m_pElem==NULL)
		return false ;
	for (int r=0; r<m_nRows; r++)
	{
		for (int c=0; c<m_nCols; c++)
		{
			(*this)[r][c] = 0.0 ;
		}
	}
	return true ;
}

bool CMatrix::Scale ( CMatrix &src
					, DOUBLE dScale
					, DOUBLE dShift // = 0.0
					  )
{
	int nRow, nCol ;

	if (src.m_nCols!=m_nCols || src.m_nRows!=m_nRows)
		return false ;

	for (nRow=0; nRow<m_nRows; nRow++)
	{
		for (nCol=0; nCol<m_nCols; nCol++)
		{
			(*this)[nRow][nCol] = src[nRow][nCol] * dScale + dShift ;
		}
	}

	return true ;
}

bool CMatrix::CheckSize (int r, int c)
{
	if (m_nRows==r && m_nCols==c)
		return true ;
	else
		return false ;
}

int CMatrix::MaxDim()
{
	if (m_nCols>m_nRows)
		return m_nCols ;
	else
		return m_nRows ;
}

int CMatrix::MinDim()
{
	if (m_nCols>m_nRows)
		return m_nRows ;
	else
		return m_nCols ;
}

void CMatrix::Trace(LPCTSTR szLabel, int width, int prec)
{
	int i, j ;
	CString numstr, formatstr ;

	formatstr.Format("%%%d.%dlf", width, prec) ;

	if (szLabel)
	{
		m_dumpstr += szLabel ;
		m_dumpstr += "\n";
		numstr.Format("Rows=%d, Cols=%d, Step=%d, Reference=%s", m_nRows, m_nCols, m_nStep, m_bReference?"true":"false") ;
		m_dumpstr += numstr ;
		m_dumpstr += "\n" ;
	}

	for (i=0; i<m_nRows; i++)
	{
		for (j=0; j<m_nCols; j++)
		{
			numstr.Format(formatstr, (*this)[i][j]) ;
			m_dumpstr += numstr ;
			if (j<m_nCols-1)
				m_dumpstr += "," ;
			else
				m_dumpstr += "\n" ;
		}
	}

	m_dumpstr += "\n\n";
}

CString CMatrix::Print(int width, int prec)
{
	int i, j ;
	CString numstr, formatstr, rstr ;

	rstr.Empty() ;
	formatstr.Format("%%%d.%dlf", width, prec) ;

	for (i=0; i<m_nRows; i++)
	{
		for (j=0; j<m_nCols; j++)
		{
			numstr.Format(formatstr, (*this)[i][j]) ;
			rstr += numstr ;
			if (j<m_nCols-1)
				rstr += "," ;
			else
				rstr += "\n" ;
		}
	}

	return rstr ;
}

bool CMatrix::SaveTraceFile(LPCTSTR filename)
{
/*
	CStdioFile file ;

	if (file.Open (filename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		file.WriteString(m_dumpstr) ;
		file.Close() ;
		return true ;
	}*/
	return false ;
}

void CMatrix::ClearTrace()
{
	m_dumpstr.Empty() ;
}



#define SIGN(a,b) ((b) >= (DOUBLE)0.0 ? (DOUBLE)fabs(a) : (DOUBLE)-fabs(a))
static DOUBLE dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static DOUBLE dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

DOUBLE my_dpythag(DOUBLE a, DOUBLE b)
{
	DOUBLE absa,absb;
	absa=(DOUBLE)fabs(a);
	absb=(DOUBLE)fabs(b);
	if (absa > absb) 
		return absa*(DOUBLE)sqrt((DOUBLE)1.0+DSQR(absb/absa));
	else 
		return (absb == (DOUBLE)0.0 ? (DOUBLE)0.0 : absb*(DOUBLE)sqrt((DOUBLE)1.0+DSQR(absa/absb)));
}

// The output of this method is stored in the calling matrix.
//
bool CMatrix::dsvdcmp( CMatrix &w   // output [1..m][1..n]
					 , CMatrix &v   // output [1..m][1..1] if not SVD_W_T [1..1][1..m] otherwise
//					 , CMatrix &v   // output [1..n][1..n] if SVD_V_T specified, transpose it
//					 , int nFlags
					   )
{
	int flag,i,its,j,jj,k,L,nm;
	DOUBLE c,f,h,s,x,y,z;
	DOUBLE anorm=0.0,g=0.0,scale=0.0;
	CMatrix rv1;
	CMatrix a ;
	int m = m_nRows ;
	int n = m_nCols ;

	a = *this ;
	
	rv1.Resize(n,1);
	w.Resize(n,1) ;
	v.Resize(n,n) ;
	rv1.Zero() ;
	w.Zero() ;
	v.Zero() ;


	for (i=0;i< n;i++)
	{
		L=i+1;
		rv1[i][0]=scale*g;
		g=s=scale=0.0;
		if (i <  m)
		{
			for (k=i;k< m;k++) 
				scale += (DOUBLE)fabs(a[k][i]);

			if (scale)
			{
				for (k=i;k< m;k++)
				{
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][i]=f-g;

				
					for (j=L;j< n;j++)
					{
						for (s=0.0,k=i;k< m;k++) 
							s += a[k][i]*a[k][j];

						f=s/h;
						for (k=i;k< m;k++) 
							a[k][j] += f*a[k][i];
					}

				for (k=i;k< m;k++) 
					a[k][i] *= scale;
			}
		}
		w[i][0]=scale *g;
		g=s=scale=(DOUBLE)0.0;

		if (i <  m && i != n)
		{
			for (k=L;k< n;k++) 
				scale += (DOUBLE)fabs(a[i][k]);

			if (scale)
			{
				for (k=L;k< n;k++)
				{
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][L];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][L]=f-g;

				for (k=L;k< n;k++) 
					rv1[k][0]=a[i][k]/h;

				
					for (j=L;j< m;j++)
					{
						for (s=(DOUBLE)0.0,k=L;k< n;k++) 
							s += a[j][k]*a[i][k];

						for (k=L;k< n;k++) 
							a[j][k] += s*rv1[k][0];
					}
					
				for (k=L;k< n;k++) 
					a[i][k] *= scale;
			}
		}
		anorm=DMAX(anorm,(DOUBLE)(fabs(w[i][0])+fabs(rv1[i][0])));
	}

	for (i=n-1;i>=0;i--)
	{
		if (i < n)
		{
			if (g)
			{
				for (j=L;j< n;j++) 
					v[j][i]=(a[i][j]/a[i][L])/g;
				for (j=L;j< n;j++)
				{
					for (s=0.0,k=L;k< n;k++) 
						s += a[i][k]*v[k][j];

					for (k=L;k< n;k++) 
						v[k][j] += s*v[k][i];
				}
			}
			for (j=L;j< n;j++) 
				v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=(DOUBLE)1.0;
		g=rv1[i][0];
		L=i;
	}

	for (i=IMIN(m,n)-1;i>=0;i--)
	{
		L=i+1; 
		g=w[i][0];

			for (j=L;j< n;j++) 
				a[i][j]=(DOUBLE)0.0;

			if (g)
			{
				g=(DOUBLE)1.0/g;

				
					for (j=L;j< n;j++)
					{
						for (s=(DOUBLE)0.0,k=L;k< m;k++) 
							s += a[k][i]*a[k][j];

						f=(s/a[i][i])*g;

						for (k=i;k< m;k++) 
							a[k][j] += f*a[k][i];
					}

				for (j=i;j< m;j++) 
					a[j][i] *= g;
			} 
			else 
			{
				for (j=i;j< m;j++) 
					a[j][i]=(DOUBLE)0.0;
			}
		++(a[i][i]);
	}

		

	// This is the start of the problem area
	for (k=n-1;k>=0;k--)
	{
		for (its=1;its<=30;its++)
		{
			flag=1;
			for (L=k;L>=0;L--)
			{
				nm=L-1;
				if ((DOUBLE)(fabs(rv1[L][0])+anorm) == anorm)
				{
					flag=0;
					break;
				}
				if (nm==-1)
					return false;
				if ((DOUBLE)(fabs(w[nm][0])+anorm) == anorm) 
					break;
			}
			if (flag)
			{
				c=(DOUBLE)0.0;
				s=(DOUBLE)1.0;
				for (i=L;i<=k;i++)
				{
					f=s*rv1[i][0];
					rv1[i][0]=c*rv1[i][0];
					if ((DOUBLE)(fabs(f)+anorm) == anorm) 
						break;
					g=w[i][0];
					h=my_dpythag(f,g);
					w[i][0]=h;
					h=(DOUBLE)1.0/h;
					c=g*h;
					s = -f*h;
					for (j=0;j< m;j++)
					{
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}

				}
			}
			z=w[k][0];
			if (L == k)
			{
				if (z < 0.0)
				{
					w[k][0] = -z;
					for (j=0;j< n;j++) 
						v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its == 30) 
				//nrerror("no convergence in 30 dsvdcmp iterations");
				return false ;
			x=w[L][0];
			nm=k-1;
			y=w[nm][0];
			g=rv1[nm][0];
			h=rv1[k][0];
			f=((y-z)*(y+z)+(g-h)*(g+h))/((DOUBLE)2.0*h*y);
			g=my_dpythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=(DOUBLE)1.0;
			for (j=L;j<=nm;j++)
			{
				i=j+1;
				g=rv1[i][0];
				y=w[i][0];
				h=s*g;
				g=c*g;
				z=my_dpythag(f,h);
				rv1[j][0]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=0;jj< n;jj++)
				{
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=my_dpythag(f,h);
				w[j][0]=z;
				if (z)
				{
					z=(DOUBLE)1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=0;jj< m;jj++)
				{
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[L][0]=(DOUBLE)0.0;
			rv1[k][0]=f;
			w[k][0]=x;
		}
	}

	*this = a ;

	return   true   ;
}

bool CMatrix::svdbksub ( CMatrix &u   // u[1..m][1..n]
					 , CMatrix &w   // w[1..n][1..1]
					 , CMatrix &v   // v[1..n][1..n]
					 , CMatrix &b   // input  b[1..m]
					     ) // return value is this[1..n]
{
	int		jj, j, i, m, n ;
	CMatrix tmp ;
	DOUBLE  s ;

	m = u.m_nRows ;
	n = u.m_nCols ;
	Resize(n,1) ;

	if (	w.m_nRows != n
		 || w.m_nCols != 1
		 || v.m_nRows != n
		 || v.m_nCols != n
		 || b.m_nRows != m
		 || b.m_nCols != 1 )
		 return false ;

	tmp.Resize (n, 1) ;
	for (j=0; j<n; j++)
	{
		s = (DOUBLE)0.0 ;
		if (w[j][0] != (DOUBLE)0.0)
		{
			for (i=0; i<m; i++)
			{
				s += u[i][j] * b[i][0] ;
			}
			s /= w[j][0] ;
		}
		tmp[j][0] = s ;
	}

	for (j=0; j<n; j++)
	{
		s = (DOUBLE)0.0 ;
		for (jj=0; jj<n; jj++)
		{
			s += v[j][jj] * tmp[jj][0] ;
		}
		*this[j][0] = s ;
	}

	return false ;
}

bool CMatrix::CrossProduct(CMatrix &v1, CMatrix &v2)
{
	if (  m_nRows * m_nCols != 3
	   || v1.m_nRows * v1.m_nCols != 3
	   || v2.m_nRows * v2.m_nCols != 3 )
	   return false ;

    m_pElem[2*m_nStep] = v1.m_pElem[0*v1.m_nStep] * v2.m_pElem[1*v2.m_nStep] - v1.m_pElem[1*v1.m_nStep] * v2.m_pElem[0*v2.m_nStep];
    m_pElem[0*m_nStep] = v1.m_pElem[1*v1.m_nStep] * v2.m_pElem[2*v2.m_nStep] - v1.m_pElem[2*v1.m_nStep] * v2.m_pElem[1*v2.m_nStep];
    m_pElem[1*m_nStep] = v1.m_pElem[2*v1.m_nStep] * v2.m_pElem[0*v2.m_nStep] - v1.m_pElem[0*v1.m_nStep] * v2.m_pElem[2*v2.m_nStep];
	return true ;
}

CMatrix &CMatrix::Reshape(int nRows, int nCols)
{
	// Simply flip the rows and columns values
	if (nRows==-1 && nCols==-1)
	{
		m_nStep = 1 ;
		m_nRows *= m_nCols ;
		m_nCols = 1 ;
	}
	else
	if (nRows * nCols == m_nRows * m_nCols)
	{
		m_nStep = (m_nRows * m_nCols) / nRows ;
		m_nRows = nRows ;
		m_nCols = nCols ;
	}
	return *this; 
}

CMatrix &CMatrix::Reshape(CMatrix& srcshape)
{
	// Simply flip the rows and columns values
	m_nStep = srcshape.m_nStep ;
	m_nRows = srcshape.m_nRows ;
	m_nCols = srcshape.m_nCols ;
	return *this; 
}

DOUBLE CMatrix::Norm(int NormType)
{
	DOUBLE rval=(DOUBLE)1.0 ;
	int i ;
	DOUBLE sum ;
	DOUBLE v ;

	sum = 0 ;

	switch (NormType)
	{
	case NORM_C:
		{
			rval = (DOUBLE)0.0 ;
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum = (DOUBLE)fabs(v) ;
				if (sum > rval)
					rval = sum ;
			}
		}
		break;

	case NORM_L1:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += (DOUBLE)fabs(v) ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;

	case NORM_L2:
	default:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += v*v ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;
	}

	return rval ;
}


DOUBLE CMatrix::Norm(CMatrix &arr1, int NormType)
{
	DOUBLE rval=(DOUBLE)1.0 ;
	int i ;
	DOUBLE sum ;
	DOUBLE v ;

	sum = 0 ;

	switch (NormType)
	{
	case NORM_C:
		{
			rval = (DOUBLE)0.0 ;
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum = (DOUBLE)fabs(v) ;
				if (sum > rval)
					rval = sum ;
			}
		}
		break;

	case NORM_L1:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += (DOUBLE)fabs(v) ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;

	case NORM_L2:
	default:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += v*v ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;
	}

	return rval ;
}

DOUBLE CMatrix::Norm(CMatrix &arr1, CMatrix &mask, int NormType)
{
	DOUBLE rval=(DOUBLE)1.0 ;
	int i ;
	DOUBLE sum ;
	DOUBLE v ;

	sum = 0 ;

	switch (NormType)
	{
	case NORM_C:
		{
			rval = (DOUBLE)0.0 ;
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum = (DOUBLE)fabs(v) ;
				if (sum > rval)
					rval = sum ;
			}
		}
		break;

	case NORM_L1:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += (DOUBLE)fabs(v) ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;

	case NORM_L2:
	default:
		{
			for (i=0; i<m_nRows*m_nCols; i++)
			{
				v = m_pElem[i] ;
				sum += v*v ;
			}

			rval = (DOUBLE)sqrt(sum) ;
		}
		break;
	}

	return rval ;
}

CMatrix& CMatrix::CopyToSel ( CMatrix &src
						 , int rStart
						 , int cStart
						 , int nHeight
						 , int nWidth )
{
	if (   rStart==0
		&& cStart==0
		&& nHeight==-1
		&& nWidth==-1 )
	{
		*this = src ;
		return *this;
	}

	if (nHeight<0)
		nHeight = src.m_nRows ;
	if (nWidth<0)
		nWidth = src.m_nCols ;

	int r, c ;

	for (r=0; r<nHeight; r++)
	{
		for (c=0; c<nWidth; c++)
		{
			(*this)[r+rStart][c+cStart] = src[r][c] ;
		}
	}

	return *this ;
}

CRectangle::CRectangle()
{
}

CRectangle::CRectangle(CTriangle &triangle)
{
}

CRectangle::~CRectangle()
{
}


bool CTriangle::Inside2D  ( int x1, int y1
						  , int x2, int y2
						  , int x3, int y3
						  , int  x, int  y ) 
{
	int s1, s2, s3 ;
	int dxt, dyt ;
	int dxp, dyp ;

	// if the sign of all 3 dot products is the same,
	// the point is inside the triangle.
	dxt = x2 - x1 ;
	dyt = y2 - y1 ;
	dxp = x - x1 ;
	dyp = y - y1 ;
	s1 = (dxt*dyp - dyt*dxp) > 0 ? 1 : -1 ;


	dxt = x3 - x2 ;
	dyt = y3 - y2 ;
	dxp = x - x2 ;
	dyp = y - y2 ;
	s2 = (dxt*dyp - dyt*dxp) > 0 ? 1 : -1 ;


	dxt = x1 - x3 ;
	dyt = y1 - y3 ;
	dxp = x - x3 ;
	dyp = y - y3 ;
	s3 = (dxt*dyp - dyt*dxp) > 0 ? 1 : -1 ;


	if (s1==s2 && s2==s3)
		return true ;
	else
		return false ;
}

CTriangle::CTriangle()
{
	m_Vertices[0].Clear() ;
	m_Vertices[1].Clear() ;
	m_Vertices[2].Clear() ;
}

CTriangle::CTriangle(DOUBLE *pInitData)
{
	m_Vertices[0].Set(pInitData,3) ;
	m_Vertices[1].Set(pInitData+3,3) ;
	m_Vertices[2].Set(pInitData+6,3) ;
}

CTriangle::CTriangle(CCoord &one, CCoord &two, CCoord &three)
{
	m_Vertices[0] = one ;
	m_Vertices[1] = two ;
	m_Vertices[2] = three ;
}

CTriangle::~CTriangle()
{
}

bool CTriangle::MinMax ( DOUBLE *pTriangleV1
					 , DOUBLE *pTriangleV2
					 , DOUBLE *pTriangleV3
					 , DOUBLE sx
					 , DOUBLE sy
					 , CCoord &min
					 , CCoord &max
					 , CCoord *prec )
{
	min.m_x = max.m_x = pTriangleV1[0] ;
	min.m_y = max.m_y = pTriangleV2[1] ;
	min.m_z = max.m_z = pTriangleV3[2] ;
	
	if (pTriangleV1[0] < min.m_x)
		min.m_x = pTriangleV1[0] ;
	if (pTriangleV1[1] < min.m_y)
		min.m_y = pTriangleV1[1] ;
	if (pTriangleV1[2] < min.m_z)
		min.m_z = pTriangleV1[2] ;

	if (pTriangleV1[0] > max.m_x)
		max.m_x = pTriangleV1[0] ;
	if (pTriangleV1[1] > max.m_y)
		max.m_y = pTriangleV1[1] ;
	if (pTriangleV1[2] > max.m_z)
		max.m_z = pTriangleV1[2] ;


	if (pTriangleV2[0] < min.m_x)
		min.m_x = pTriangleV2[0] ;
	if (pTriangleV2[1] < min.m_y)
		min.m_y = pTriangleV2[1] ;
	if (pTriangleV2[2] < min.m_z)
		min.m_z = pTriangleV2[2] ;

	if (pTriangleV2[0] > max.m_x)
		max.m_x = pTriangleV2[0] ;
	if (pTriangleV2[1] > max.m_y)
		max.m_y = pTriangleV2[1] ;
	if (pTriangleV2[2] > max.m_z)
		max.m_z = pTriangleV2[2] ;


	if (pTriangleV3[0] < min.m_x)
		min.m_x = pTriangleV3[0] ;
	if (pTriangleV3[1] < min.m_y)
		min.m_y = pTriangleV3[1] ;
	if (pTriangleV3[2] < min.m_z)
		min.m_z = pTriangleV3[2] ;

	if (pTriangleV3[0] > max.m_x)
		max.m_x = pTriangleV3[0] ;
	if (pTriangleV3[1] > max.m_y)
		max.m_y = pTriangleV3[1] ;
	if (pTriangleV3[2] > max.m_z)
		max.m_z = pTriangleV3[2] ;

	if (prec)
	{
		min.m_x = (DOUBLE)(floor((min.m_x - sx) / prec->m_x) * prec->m_x  + sx) ;
		min.m_y = (DOUBLE)(floor((min.m_y - sy) / prec->m_y) * prec->m_y + sy) ;
		max.m_x = (DOUBLE)(ceil((max.m_x - sx) / prec->m_x) * prec->m_x + sx) ;
		max.m_y = (DOUBLE)(ceil((max.m_y - sy) / prec->m_y) * prec->m_y + sy) ;
		max.m_x += prec->m_x ;
		min.m_x -= prec->m_x ;
		max.m_y += prec->m_y ;
		min.m_y -= prec->m_y ;
		//min.m_z = min.m_z ;
		//max.m_z = max.m_z ;
	}
	return true ;
}

bool CTriangle::MinMax ( DOUBLE *pTriangle
					 , DOUBLE sx
					 , DOUBLE sy
					 , CCoord &min
					 , CCoord &max
					 , CCoord *prec )
{
	int i ;
	min.m_x = max.m_x = pTriangle[0] ;
	min.m_y = max.m_y = pTriangle[1] ;
	min.m_z = max.m_z = pTriangle[2] ;
	
	for (i=3; i<9; i+=3)
	{
		if (pTriangle[i+0] < min.m_x)
			min.m_x = pTriangle[i+0] ;
		if (pTriangle[i+1] < min.m_y)
			min.m_y = pTriangle[i+1] ;
		if (pTriangle[i+2] < min.m_z)
			min.m_z = pTriangle[i+2] ;

		if (pTriangle[i+0] > max.m_x)
			max.m_x = pTriangle[i+0] ;
		if (pTriangle[i+1] > max.m_y)
			max.m_y = pTriangle[i+1] ;
		if (pTriangle[i+2] > max.m_z)
			max.m_z = pTriangle[i+2] ;

	}
	if (prec)
	{
		min.m_x = (DOUBLE)(floor((min.m_x - sx) / prec->m_x) * prec->m_x  + sx) ;
		min.m_y = (DOUBLE)(floor((min.m_y - sy) / prec->m_y) * prec->m_y + sy) ;
		max.m_x = (DOUBLE)(ceil((max.m_x - sx) / prec->m_x) * prec->m_x + sx) ;
		max.m_y = (DOUBLE)(ceil((max.m_y - sy) / prec->m_y) * prec->m_y + sy) ;
		max.m_x += prec->m_x ;
		min.m_x -= prec->m_x ;
		max.m_y += prec->m_y ;
		min.m_y -= prec->m_y ;
		//min.m_z = min.m_z ;
		//max.m_z = max.m_z ;
	}
	return true ;
}

bool CTriangle::MinMax ( CCoord &min
					 , CCoord &max
					 , CCoord *prec )
{
	int i ;
	min = max = m_Vertices[0] ;

	for (i=1; i<3; i++)
	{
		if (m_Vertices[i].m_x < min.m_x)
			min.m_x = m_Vertices[i].m_x ;
		if (m_Vertices[i].m_y < min.m_y)
			min.m_y = m_Vertices[i].m_y ;
		if (m_Vertices[i].m_z < min.m_z)
			min.m_z = m_Vertices[i].m_z ;

		if (m_Vertices[i].m_x > max.m_x)
			max.m_x = m_Vertices[i].m_x ;
		if (m_Vertices[i].m_y > max.m_y)
			max.m_y = m_Vertices[i].m_y ;
		if (m_Vertices[i].m_z > max.m_z)
			max.m_z = m_Vertices[i].m_z ;

	}
	if (prec)
	{
		min.m_x = (DOUBLE)(floor((min.m_x) / prec->m_x) * prec->m_x) ;
		min.m_y = (DOUBLE)(floor((min.m_y) / prec->m_y) * prec->m_y) ;
		max.m_x = (DOUBLE)(ceil((max.m_x) / prec->m_x) * prec->m_x) ;
		max.m_y = (DOUBLE)(ceil((max.m_y) / prec->m_y) * prec->m_y) ;
		//min.m_z = min.m_z ;
		//max.m_z = max.m_z ;
	}
	return true ;
}

void CTriangle::GetFastCoords(DOUBLE *pFastCoords)
{
	pFastCoords[0] = m_Vertices[0].m_x ;
	pFastCoords[1] = m_Vertices[0].m_y ;
	pFastCoords[2] = m_Vertices[0].m_z ;
	pFastCoords[3] = m_Vertices[1].m_x ;
	pFastCoords[4] = m_Vertices[1].m_y ;
	pFastCoords[5] = m_Vertices[1].m_z ;
	pFastCoords[6] = m_Vertices[2].m_x ;
	pFastCoords[7] = m_Vertices[2].m_y ;
	pFastCoords[8] = m_Vertices[2].m_z ;
}

DOUBLE CTriangle::Area()
{
	DOUBLE a = m_Vertices[0].Distance (m_Vertices[1]) ;
	DOUBLE b = m_Vertices[1].Distance (m_Vertices[2]) ;
	DOUBLE c = m_Vertices[2].Distance (m_Vertices[0]) ;

	return (DOUBLE) sqrt ( (a+b+c) * (b+c-a) * (c+a-b) * (a+b-c) ) ;
}

DOUBLE CTriangle::Area(CCoord &c1, CCoord &c2, CCoord &c3)
{
	DOUBLE a = c1.Distance (c2) ;
	DOUBLE b = c2.Distance (c3) ;
	DOUBLE c = c3.Distance (c1) ;

	return (DOUBLE) sqrt ( (a+b+c) * (b+c-a) * (c+a-b) * (a+b-c) ) ;
}

DOUBLE CTriangle::Area(DOUBLE *p)
{
	DOUBLE a = (DOUBLE) sqrt ( (p[3]-p[0]) * (p[3]-p[0]) 
		            + (p[4]-p[1]) * (p[4]-p[1]) 
					+ (p[5]-p[2]) * (p[5]-p[2]) ) ;
	DOUBLE b = (DOUBLE) sqrt ( (p[6]-p[3]) * (p[6]-p[3]) 
		            + (p[7]-p[4]) * (p[7]-p[4]) 
					+ (p[8]-p[5]) * (p[8]-p[5]) ) ;
	DOUBLE c = (DOUBLE) sqrt ( (p[0]-p[6]) * (p[0]-p[0]) 
		            + (p[1]-p[7]) * (p[1]-p[1]) 
					+ (p[2]-p[8]) * (p[2]-p[2]) ) ;

	return (DOUBLE) sqrt ( (a+b+c) * (b+c-a) * (c+a-b) * (a+b-c) ) ;
}

bool CTriangle::BarycentricTransform ( DOUBLE *pSrc
									 , DOUBLE *pDst
									 , DOUBLE *pPoint )
{
	DOUBLE AB, BC, CA ;
	DOUBLE AP, BP, CP ;
	DOUBLE a1, a2, a3 ;
	DOUBLE aWhole ;

	AB = (DOUBLE) sqrt ( (pSrc[0]-pSrc[3])*(pSrc[0]-pSrc[3])
					  + (pSrc[1]-pSrc[4])*(pSrc[1]-pSrc[4])
					  + (pSrc[2]-pSrc[5])*(pSrc[2]-pSrc[5])
						) ;
	BC = (DOUBLE) sqrt ( (pSrc[3]-pSrc[6])*(pSrc[3]-pSrc[6])
					  + (pSrc[4]-pSrc[7])*(pSrc[4]-pSrc[7])
					  + (pSrc[5]-pSrc[8])*(pSrc[5]-pSrc[8])
						) ;
	CA = (DOUBLE) sqrt ( (pSrc[6]-pSrc[0])*(pSrc[6]-pSrc[0])
					  + (pSrc[7]-pSrc[1])*(pSrc[7]-pSrc[1])
					  + (pSrc[8]-pSrc[2])*(pSrc[8]-pSrc[2])
						) ;

	AP = (DOUBLE) sqrt ( (pSrc[0]-pPoint[0])*(pSrc[0]-pPoint[0])
					  + (pSrc[1]-pPoint[1])*(pSrc[1]-pPoint[1])
					  + (pSrc[2]-pPoint[2])*(pSrc[2]-pPoint[2])
						) ;
	BP = (DOUBLE) sqrt ( (pSrc[3]-pPoint[0])*(pSrc[3]-pPoint[0])
					  + (pSrc[4]-pPoint[1])*(pSrc[4]-pPoint[1])
					  + (pSrc[5]-pPoint[2])*(pSrc[5]-pPoint[2])
						) ;
	CP = (DOUBLE) sqrt ( (pSrc[6]-pPoint[0])*(pSrc[6]-pPoint[0])
					  + (pSrc[7]-pPoint[1])*(pSrc[7]-pPoint[1])
					  + (pSrc[8]-pPoint[2])*(pSrc[8]-pPoint[2])
						) ;

	// BP, CP, BC
	a1 =  (DOUBLE) sqrt ( (BP+CP+BC) * (CP+BC-BP) * (BC+BP-CP) * (BP+CP-BC) ) ;

	// CP, AP, CA
	a2 =  (DOUBLE) sqrt ( (CP+AP+CA) * (AP+CA-CP) * (CA+CP-AP) * (CP+AP-CA) ) ;

	// AP, BP, AB
	a3 =  (DOUBLE) sqrt ( (AP+BP+AB) * (BP+AB-AP) * (AB+AP-BP) * (AP+BP-AB) ) ;

	aWhole = (DOUBLE) sqrt ( (BC+CA+AB) * (CA+AB-BC) * (AB+BC-CA) * (BC+CA-AB) ) ;

	a1 /= aWhole ;
	a2 /= aWhole ;
	a3 /= aWhole ;

	pPoint[0] = pDst[0] * a1 + pDst[3] * a2 + pDst[6] * a3 ;
	pPoint[1] = pDst[1] * a1 + pDst[4] * a2 + pDst[7] * a3 ;
	pPoint[2] = pDst[2] * a1 + pDst[5] * a2 + pDst[8] * a3 ;

	return true ;
}

bool CTriangle::BarycentricTransform3D2D ( DOUBLE *pSrc
									 , DOUBLE *pDst
									 , DOUBLE *pPoint )
{
	DOUBLE AB, BC, CA ;
	DOUBLE AP, BP, CP ;
	DOUBLE a1, a2, a3 ;
	DOUBLE aWhole ;

	AB = (DOUBLE) sqrt ( (pSrc[0]-pSrc[3])*(pSrc[0]-pSrc[3])
 					  + (pSrc[1]-pSrc[4])*(pSrc[1]-pSrc[4])
					  + (pSrc[2]-pSrc[5])*(pSrc[2]-pSrc[5])
						) ;
	BC = (DOUBLE) sqrt ( (pSrc[3]-pSrc[6])*(pSrc[3]-pSrc[6])
					  + (pSrc[4]-pSrc[7])*(pSrc[4]-pSrc[7])
					  + (pSrc[5]-pSrc[8])*(pSrc[5]-pSrc[8])
						) ;
	CA = (DOUBLE) sqrt ( (pSrc[6]-pSrc[0])*(pSrc[6]-pSrc[0])
					  + (pSrc[7]-pSrc[1])*(pSrc[7]-pSrc[1])
					  + (pSrc[8]-pSrc[2])*(pSrc[8]-pSrc[2])
						) ;

	AP = (DOUBLE) sqrt ( (pSrc[0]-pPoint[0])*(pSrc[0]-pPoint[0])
					  + (pSrc[1]-pPoint[1])*(pSrc[1]-pPoint[1])
					  + (pSrc[2]-pPoint[2])*(pSrc[2]-pPoint[2])
						) ;
	BP = (DOUBLE) sqrt ( (pSrc[3]-pPoint[0])*(pSrc[3]-pPoint[0])
					  + (pSrc[4]-pPoint[1])*(pSrc[4]-pPoint[1])
					  + (pSrc[5]-pPoint[2])*(pSrc[5]-pPoint[2])
						) ;
	CP = (DOUBLE) sqrt ( (pSrc[6]-pPoint[0])*(pSrc[6]-pPoint[0])
					  + (pSrc[7]-pPoint[1])*(pSrc[7]-pPoint[1])
					  + (pSrc[8]-pPoint[2])*(pSrc[8]-pPoint[2])
						) ;

	// BP, CP, BC
	a1 =  (DOUBLE) sqrt ( (BP+CP+BC) * (CP+BC-BP) * (BC+BP-CP) * (BP+CP-BC) ) ;

	// CP, AP, CA
	a2 =  (DOUBLE) sqrt ( (CP+AP+CA) * (AP+CA-CP) * (CA+CP-AP) * (CP+AP-CA) ) ;

	// AP, BP, AB
	a3 =  (DOUBLE) sqrt ( (AP+BP+AB) * (BP+AB-AP) * (AB+AP-BP) * (AP+BP-AB) ) ;

	aWhole = (DOUBLE) sqrt ( (BC+CA+AB) * (CA+AB-BC) * (AB+BC-CA) * (BC+CA-AB) ) ;

	a1 /= aWhole ;
	a2 /= aWhole ;
	a3 /= aWhole ;

	pPoint[0] = pDst[0] * a1 + pDst[2] * a2 + pDst[4] * a3 ;
	pPoint[1] = pDst[1] * a1 + pDst[3] * a2 + pDst[5] * a3 ;
	pPoint[2] = 0 ;

	return true ;
}


void CTriangle::Set(DOUBLE *pInitData)
{
	m_Vertices[0].Set(pInitData,3) ;
	m_Vertices[1].Set(pInitData+3,3) ;
	m_Vertices[2].Set(pInitData+6,3) ;
}

CTriangle& CTriangle::Offset(CVector &amount)
{
	m_Vertices[0].Offset (amount) ;
	m_Vertices[1].Offset (amount) ;
	m_Vertices[2].Offset (amount) ;
	return *this ;
}

void CTriangle::Set(CCoord &one, CCoord &two, CCoord &three)
{
	m_Vertices[0] = one ;
	m_Vertices[1] = two ;
	m_Vertices[2] = three ;
}

CTriangle::operator LPRECT ()
{
	CPoint one, two, three ;
	DOUBLE left, top, right, bottom ;

	one   = m_Vertices[0].MakePoint() ;
	two   = m_Vertices[1].MakePoint() ;
	three = m_Vertices[2].MakePoint() ;

	left = right  = m_Vertices[0].m_x ;
	top  = bottom = m_Vertices[0].m_y ;

	if (m_Vertices[1].m_x < left)   left  = m_Vertices[1].m_x ;
	if (m_Vertices[2].m_x < left)   left  = m_Vertices[2].m_x ;
	if (m_Vertices[1].m_x > right)  right = m_Vertices[1].m_x ;
	if (m_Vertices[2].m_x > right)  right = m_Vertices[2].m_x ;

	if (m_Vertices[1].m_y < top)	top    = m_Vertices[1].m_y ;
	if (m_Vertices[2].m_y < top)	top    = m_Vertices[2].m_y ;
	if (m_Vertices[1].m_y > bottom) bottom = m_Vertices[1].m_y ;
	if (m_Vertices[2].m_y > bottom) bottom = m_Vertices[2].m_y ;

	m_rcBounds.left   = (int)floor(left) ;
	m_rcBounds.top    = (int)floor(top) ;
	m_rcBounds.right  = (int)ceil(right) ;
	m_rcBounds.bottom = (int)ceil(bottom) ;

	return (LPRECT)m_rcBounds ;
}

CTriangle::operator LPCRECT ()
{
	CPoint one, two, three ;
	DOUBLE left, top, right, bottom ;

	one   = m_Vertices[0].MakePoint() ;
	two   = m_Vertices[1].MakePoint() ;
	three = m_Vertices[2].MakePoint() ;

	left = right  = m_Vertices[0].m_x ;
	top  = bottom = m_Vertices[0].m_y ;

	if (m_Vertices[1].m_x < left)     left  = m_Vertices[1].m_x ;
	if (m_Vertices[2].m_x < left)   left  = m_Vertices[2].m_x ;
	if (m_Vertices[1].m_x > right)    right = m_Vertices[1].m_x ;
	if (m_Vertices[2].m_x > right)  right = m_Vertices[2].m_x ;

	if (m_Vertices[1].m_x > top)		 top    = m_Vertices[1].m_y ;
	if (m_Vertices[2].m_x > top)	 top    = m_Vertices[2].m_y ;
	if (m_Vertices[1].m_x < bottom)   bottom = m_Vertices[1].m_y ;
	if (m_Vertices[2].m_x < bottom) bottom = m_Vertices[2].m_y ;

	m_rcBounds.left   = (int)floor(left) ;
	m_rcBounds.top    = (int)floor(top) ;
	m_rcBounds.right  = (int)ceil(right) ;
	m_rcBounds.bottom = (int)ceil(bottom) ;

	return (LPRECT)m_rcBounds ;
}

CTriangle &CTriangle::Scale(DOUBLE scalarmultiplier)
{
	m_Vertices[0] *= scalarmultiplier ;
	m_Vertices[1] *= scalarmultiplier ;
	m_Vertices[2] *= scalarmultiplier ;
	return *this;
}

CCoord CTriangle::Center ()
{
	CCoord rval ;
	rval = m_Vertices[0] ;
	rval += m_Vertices[1] ;
	rval += m_Vertices[2] ;
	rval /= 3.0 ;
	return rval ;
}

#define SMALL_NUM  0.00000001 // anything that avoids division overflow


// int CTriangle::Intersect(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
int CTriangle::Intersect ( CLine &ray
						 , CCoord &rval)
{
	CPlane  p (m_Vertices[0], m_Vertices[1], m_Vertices[2]) ;
	DOUBLE  a1, a2, a3, a ;
//	long nOldVal = _controlfp(RC_CHOP, _MCW_RC) ;

	// Find the plane intersection
	if (p.Intersect(ray, rval)==false)
	{
//		_controlfp(nOldVal, _MCW_RC) ;
		return -1 ;
	}

	// Does it lie within the triangle?
	// If the sum of barycentric coordinates are greater than 
	// the area of the entire triangle.
	a = Area() ;
	a1 = Area(rval, m_Vertices[0], m_Vertices[1]) ;
	a2 = Area(rval, m_Vertices[1], m_Vertices[2]) ;
	a3 = Area(rval, m_Vertices[2], m_Vertices[0]) ;

	if (a1+a2+a3 - a > SMALL_NUM)
	{
//		_controlfp(nOldVal, _MCW_RC) ;
		return 0 ;
	}

//	_controlfp(nOldVal, _MCW_RC) ;
	return 1 ;


    CVector    u, v, n;             // triangle vectors
    CVector    dir, w0, w;          // ray vectors
    DOUBLE     r, b;             // params to calc ray-plane intersect


    // get triangle edge vectors and plane normal
    u = m_Vertices[1] - m_Vertices[0];
    v = m_Vertices[2] - m_Vertices[1];
    if (!n.CrossProduct(u,v))      // cross product
					               // triangle is degenerate
	{
//		_controlfp(nOldVal, _MCW_RC) ;
        return -1;                 // do not deal with this case
	}

    dir = ray.m_second - ray.m_first;             // ray direction vector
    w0 = ray.m_first - m_Vertices[0] ;
    a = -n.DotProduct(w0);
    b = n.DotProduct(dir);
    if (fabs(b) < SMALL_NUM) 
	{							    // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
		{
//			_controlfp(nOldVal, _MCW_RC) ;
            return 2;
		}
        else 
		{
//			_controlfp(nOldVal, _MCW_RC) ;
			return 0;             // ray disjoint from plane
		}
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < (DOUBLE)0.0)           // ray goes away from triangle
	{
//		_controlfp(nOldVal, _MCW_RC) ;
        return 0;                  // => no intersect
	}
    // for a segment, also test if (r > 1.0) => no intersect

    rval = ray.m_first + dir * r;           // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = u.DotProduct(u);
    uv = u.DotProduct(v);
    vv = v.DotProduct(v);

    w = rval - m_Vertices[0];
    wu = w.DotProduct(u);
    wv = w.DotProduct(v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
	{
//		_controlfp(nOldVal, _MCW_RC) ;
        return 0;
	}
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
	{
//		_controlfp(nOldVal, _MCW_RC) ;
        return 0;
	}

//	_controlfp(nOldVal, _MCW_RC) ;
    return 1;                      // I is in T
}

int
CTriangle::Intersect(CLine &ray, DOUBLE *rval)
{
    CVector    u, v, n;             // triangle vectors
    CVector    dir, w0, w;          // ray vectors
    DOUBLE     r, a, b;             // params to calc ray-plane intersect
	CCoord     rv ;

    // get triangle edge vectors and plane normal
    u = m_Vertices[1] - m_Vertices[0];
    v = m_Vertices[2] - m_Vertices[1];
    if (!n.CrossProduct(u,v))      // cross product
					               // triangle is degenerate
        return -1;                 // do not deal with this case

    dir = ray.m_second - ray.m_first;             // ray direction vector
    w0 = ray.m_first - m_Vertices[0] ;
    a = -n.DotProduct(w0);
    b = n.DotProduct(dir);
    if (fabs(b) < SMALL_NUM) 
	{							    // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < (DOUBLE)0.0)           // ray goes away from triangle
        return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

    rv = ray.m_first + dir * r;           // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = u.DotProduct(u);
    uv = u.DotProduct(v);
    vv = v.DotProduct(v);
    w = rv - m_Vertices[0];
    wu = w.DotProduct(u);
    wv = w.DotProduct(v);
    D = uv * uv - uu * vv;

	rval[0] = rv.m_x ;
	rval[1] = rv.m_y ;
	rval[2] = rv.m_z ;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                      // I is in T
}


void CTriangle::Set(DOUBLE *pInitData1, DOUBLE *pInitData2, DOUBLE *pInitData3)
{
	m_Vertices[0].Set(pInitData1,3) ;
	m_Vertices[1].Set(pInitData2,3) ;
	m_Vertices[2].Set(pInitData3,3) ;
	m_Plane.Set2(pInitData1, pInitData2, pInitData3) ;
	m_LineBoundaries[0].Set (pInitData1, pInitData2, &m_Plane.m_normal.m_dx) ;
	m_LineBoundaries[1].Set (pInitData2, pInitData3, &m_Plane.m_normal.m_dx) ;
	m_LineBoundaries[2].Set (pInitData3, pInitData1, &m_Plane.m_normal.m_dx) ;
}


// This assumes that the triangle has been set using the 
// CTriangle::Set method using 3 double pointers for the set.
int CTriangle::IntersectFast ( CLine &ray
							 , DOUBLE *rval)
{
	if (m_Plane.Intersect (ray, rval)==false)
		return -1 ;

	int n1 = m_LineBoundaries[0].AboveBelow ( rval ) ;
	int n2 = m_LineBoundaries[1].AboveBelow ( rval ) ;
	int n3 = m_LineBoundaries[2].AboveBelow ( rval ) ;

	if ((n1 | n2 | n3) != 3)
		return 1 ;
	else
		return 0 ;
}

int CTriangle::Intersect ( CLine &ray
						 , DOUBLE *pNormalTriangle
						 , DOUBLE *rval)
{
	CPlane  p (pNormalTriangle) ;

	// Find the plane intersection
	if (p.Intersect(ray, rval)==false)
	{
		return -1 ;
	}

	// Does it lie within the triangle?
	// If the sum of barycentric coordinates are greater than 
	// the area of the entire triangle.

	// First coordinate in pNormalTriangle is the precalculated
	// normal of the triangle.  Use this to create the three 
	// planes.
	CPlane  p1 (pNormalTriangle+3, pNormalTriangle+6, pNormalTriangle, true) ;
	CPlane  p2 (pNormalTriangle+6, pNormalTriangle+9, pNormalTriangle, true) ;
	CPlane  p3 (pNormalTriangle+9, pNormalTriangle+3, pNormalTriangle, true) ;

	// Are all 3 simultaneously above/below
	int n1 = p1.AboveBelow ( rval ) ;
	int n2 = p2.AboveBelow ( rval ) ;
	int n3 = p3.AboveBelow ( rval ) ;

	if ((n1 | n2 | n3) != 3)
		return 1 ;
	else
		return 0 ;
}

int CTriangle::Intersect ( CLine &ray
						 , CPlane &p
						 , CPlane &p1
						 , CPlane &p2
						 , CPlane &p3
						 , DOUBLE *rval)
{
	// Find the plane intersection
	if (p.Intersect(ray, rval)==false)
	{
		return -1 ;
	}

	// Does it lie within the triangle?
	// If the sum of barycentric coordinates are greater than 
	// the area of the entire triangle.

	// First coordinate in pNormalTriangle is the precalculated
	// normal of the triangle.  Use this to create the three 
	// planes.

	// Are all 3 simultaneously above/below
	int n1 = p1.AboveBelow ( rval ) ;
	int n2 = p2.AboveBelow ( rval ) ;
	int n3 = p3.AboveBelow ( rval ) ;

	if ((n1 | n2 | n3) != 3)
		return 1 ;
	else
		return 0 ;
}




// int CTriangle::Intersect(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
int CTriangle::IntersectFast ( DOUBLE *T
							 , DOUBLE *P0
							 , DOUBLE *P1
							 , DOUBLE *rval )
{
    DOUBLE     ux,uy,uz, vx,vy,vz,    nx,ny,nz; // triangle vectors
    DOUBLE     dx,dy,dz, w0x,w0y,w0z, wx,wy,wz; // ray vectors
    DOUBLE     r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
	ux = T[3] - T[0] ;
	uy = T[4] - T[1] ;
	uz = T[5] - T[2] ;
	
	vx = T[6] - T[0] ;
	vy = T[7] - T[1] ;
	vz = T[8] - T[2] ;
	
    nx = uy * vz - uz * vy ;
    ny = uz * vx - ux * vz ;
    nz = ux * vy - uy * vx ;
    if (   nx==(DOUBLE)0.0
		&& ny==(DOUBLE)0.0
		&& nz==(DOUBLE)0.0 )       // cross product
					               // triangle is degenerate
        return -1;                 // do not deal with this case

	dx = P1[0] - P0[0] ;
	dy = P1[1] - P0[1] ;
	dz = P1[2] - P0[2] ;

    w0x = P0[0] - T[0];
    w0y = P0[1] - T[1];
    w0z = P0[2] - T[2];

	a = -(nx*w0x + ny*w0y + nz*w0z) ;
    b = nx*dx + ny*dy + nz*dz ;
    
	if (fabs(b) < SMALL_NUM) 
	{							   // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else 
			return 0;              // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < (DOUBLE)0.0)           // ray goes away from triangle
        return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

	if (rval)
	{
		P0[0] += (dx * r) ;
		P0[1] += (dy * r) ;
		P0[2] += (dz * r) ;

		rval[0] = P0[0];
		rval[1] = P0[1];
		rval[2] = P0[2];
	}

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = ux*ux + uy*uy + uz*uz ;
	uv = ux*vx + uy*vy + uz*vz ;
	vv = vx*vx + vy*vy + vz*vz ;
	
	wx = P0[0] - T[0] ;
	wy = P0[1] - T[1] ;
	wz = P0[2] - T[2] ;

    wu = wx*ux + wy*uy + wz*uz ;
	wv = wx*vx + wy*vy + wz*vz ;
	D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                      // I is in T
}
