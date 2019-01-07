#ifndef __AFXTEMPL_H__
#define __AFXTEMPL_H__

typedef unsigned char BYTE;

// the two functions below are deprecated.  Use a constructor/destructor instead.
#pragma deprecated( DestructElements )
#pragma deprecated( ConstructElements )

template<class TYPE>
inline void CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	if ((nCount == 0 || pDest != 0 && pSrc != 0)==false)
	    throw;
	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE, class ARG_TYPE>
bool CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	if ((pElement1 != NULL && pElement2 != NULL)==false)
	    throw;
	return *pElement1 == *pElement2;
}

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE>

template<class TYPE, class ARG_TYPE = const TYPE&>
class CArray 
{
public:
// Construction
	CArray();

// Attributes
	int GetSize() const;
	int GetCount() const;
	bool IsEmpty() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	const TYPE& GetAt(int nIndex) const;
	TYPE& GetAt(int nIndex);
	void SetAt(int nIndex, ARG_TYPE newElement);
	const TYPE& ElementAt(int nIndex) const;
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(const CArray& src);
	void Copy(const CArray& src);

	// overloaded operator helpers
	const TYPE& operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CArray* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	~CArray();
};

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE> inline int CArray<TYPE, ARG_TYPE>::GetSize() const { return m_nSize; }
template<class TYPE, class ARG_TYPE> inline int CArray<TYPE, ARG_TYPE>::GetCount() const { return m_nSize; }
template<class TYPE, class ARG_TYPE> inline bool CArray<TYPE, ARG_TYPE>::IsEmpty() const { return m_nSize == 0; }
template<class TYPE, class ARG_TYPE> inline int CArray<TYPE, ARG_TYPE>::GetUpperBound() const{ return m_nSize-1; }
template<class TYPE, class ARG_TYPE> inline void CArray<TYPE, ARG_TYPE>::RemoveAll() { SetSize(0, -1); }
template<class TYPE, class ARG_TYPE> inline TYPE& CArray<TYPE, ARG_TYPE>::GetAt(int nIndex)
{ 
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
    // Throw exception
}
template<class TYPE, class ARG_TYPE> inline const TYPE& CArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const
{
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	// Throw exception
}
template<class TYPE, class ARG_TYPE> inline void CArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
{ 
	if(nIndex >= 0 && nIndex < m_nSize)
		m_pData[nIndex] = newElement; 
	else
	{ throw;
	}
}
template<class TYPE, class ARG_TYPE> inline const TYPE& CArray<TYPE, ARG_TYPE>::ElementAt(int nIndex) const
{ 
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	throw;		
}
template<class TYPE, class ARG_TYPE> inline TYPE& CArray<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{ 
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	throw;		
}
template<class TYPE, class ARG_TYPE> inline const TYPE* CArray<TYPE, ARG_TYPE>::GetData() const { return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE> inline TYPE* CArray<TYPE, ARG_TYPE>::GetData() { return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE> inline int CArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
template<class TYPE, class ARG_TYPE> inline const TYPE& CArray<TYPE, ARG_TYPE>::operator[](int nIndex) const { return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE> inline TYPE& CArray<TYPE, ARG_TYPE>::operator[](int nIndex) { return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::CArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::~CArray()
{
	if (m_pData != NULL)
	{
		for( int i = 0; i < m_nSize; i++ )
			(m_pData + i)->~TYPE();
		delete[] (BYTE*)m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	if (nGrowBy >= 0)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			for( int i = 0; i < m_nSize; i++ )
				(m_pData + i)->~TYPE();
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create buffer big enough to hold number of requested elements or
		// m_nGrowBy elements, whichever is larger.
		size_t nAllocSize ;
		if (nNewSize>m_nGrowBy)
		    nAllocSize = nNewSize ;
		else
		    nAllocSize = m_nGrowBy ;
		m_pData = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
		memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(TYPE));
//		for( int i = 0; i < nNewSize; i++ )
//			::new( (void*)( m_pData + i ) ) TYPE;
//			m_pData+i = new TYPE [1];
		m_nSize = nNewSize;
		m_nMaxSize = nAllocSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
//			for( int i = 0; i < nNewSize-m_nSize; i++ )
//				::new( (void*)( m_pData + m_nSize + i ) ) TYPE;
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			for( int i = 0; i < m_nSize-nNewSize; i++ )
				(m_pData + nNewSize + i)->~TYPE();
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		if(nNewMax  < m_nMaxSize)
			throw;

		TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];

		// copy new data from old
		memcpy(pNewData, m_pData, (size_t)m_nSize * sizeof(TYPE));

		// construct remaining elements
		memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
//		for( int i = 0; i < nNewSize-m_nSize; i++ )
//			::new( (void*)( pNewData + m_nSize + i ) ) TYPE;

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Append(const CArray& src)
{
	if(this == &src)
		throw;

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	CopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Copy(const CArray& src)
{
	if(this != &src)
	{
		SetSize(src.m_nSize);
		CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::FreeExtra()
{
	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	if(nIndex < 0)
		throw;

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
	if(nIndex < 0 || nCount <= 0)
		throw;

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		for( int i = 0; i < nCount; i++ )
			(m_pData + nOldSize + i)->~TYPE();
		// shift old data up to fill gap
		memmove(m_pData + nIndex + nCount, m_pData + nIndex, (nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(TYPE));
//		for( int i = 0; i < nCount; i++ )
//			::new( (void*)( m_pData + nIndex + i ) ) TYPE;
	}

	// insert new value in the gap
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	int nUpperBound = nIndex + nCount;

	if(nIndex < 0 || nCount < 0 || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
		throw;

	// just remove a range
	int nMoveCount = m_nSize - (nUpperBound);
	for( int i = 0; i < nCount; i++ )
		(m_pData + nIndex + i)->~TYPE();
	if (nMoveCount)
	{
		memmove(m_pData + nIndex, m_pData + nUpperBound, (size_t)nMoveCount * sizeof(TYPE));
	}
	m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, CArray* pNewArray)
{
	if(pNewArray == NULL || nStartIndex < 0)
	{
	    // Throw exception
	}

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

//template<class TYPE, class ARG_TYPE>

#endif //__AFXTEMPL_H__

