//
//Copyright(c) 2016. Huan Xia
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//documentation files(the "Software"), to deal in the Software without restriction, including without limitation
//the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
//and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions
//of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//DEALINGS IN THE SOFTWARE.

#pragma once

#include <map>
#include <windows.h>
#include <unordered_map>
#include <map>
#include <set>
#include <tchar.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// 

template<typename T, int nHashSize, int nStep>
class CSTXDefaultHash
{
public:
	size_t operator()(const T &refValue)
	{
		return ((const __int64)refValue / nStep) % nHashSize;
	}
};

template<int nHashSize, int nStep>
class CSTXDefaultStringHash
{
public:
	size_t operator()(const std::string &refValue)
	{

		return (std::hash<std::string>()(refValue) / nStep) % nHashSize;
	}
};

template<int nHashSize, int nStep>
class CSTXDefaultWStringHash
{
public:
	size_t operator()(const std::wstring &refValue)
	{
		return (std::hash<std::wstring>()(refValue) / nStep) % nHashSize;
	}
};

template<int nHashSize, int nStep>
class CSTXNoCaseWStringHash
{
public:
	size_t operator()(const std::wstring &refValue)
	{
		std::wstring refValueCopy = refValue;
		std::transform(refValueCopy.begin(), refValueCopy.end(), refValueCopy.begin(), tolower);
		return (std::hash<std::wstring>()(refValueCopy) / nStep) % nHashSize;
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashMap

template<typename TKey, typename TValue, int nHashSize = 8, int nStep = 1, typename THashClass = CSTXDefaultHash<TKey, nHashSize, nStep> >
class CSTXHashMap
{
public:
	CSTXHashMap()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashMapValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::map<TKey, TValue> mapContent;
		HashMapValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashMapValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashMapValue(const HashMapValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashMapValue &operator=(const HashMapValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			mapContent = val.mapContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
			lock.EnterLock();
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
			lock.LeaveLock();
		}
	};

private:
	typedef typename std::map<TKey, TValue>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashMapValue> _mapValues;


public:
	TValue& operator[](const TKey& refKey);
	size_t size();
	size_t erase(TKey _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].mapContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = THashClass()(_itval->first);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	// 	_Type_iterator begin()	//No default implements
	_Type_iterator end(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		return value.mapContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashMapValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.mapContent.begin(), (*it).second.mapContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Unlock();
	}
	void lockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Lock();
		}
	}
	void unlockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Unlock();
		}
	}

};


template<typename TKey, typename TValue, int nHashSize /*= 8*/, int nStep /*= 1*/, typename THashClass>
size_t CSTXHashMap<TKey, TValue, nHashSize, nStep, THashClass>::erase(TKey _Keyval)
{
	size_t nHashed = THashClass()(_Keyval);// ((_Keyval / nStep) % nHashSize);
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.mapContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}

template<typename TKey, typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/, typename THashClass>
size_t CSTXHashMap<TKey, TValue, nHashSize, nStep, THashClass>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].mapContent.size();
	}
	return sizeTotal;
}

template<typename TKey, typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/, typename THashClass>
TValue& CSTXHashMap<TKey, TValue, nHashSize, nStep, THashClass>::operator[](const TKey& refKey)
{
	size_t nHashed = THashClass()(refKey);
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	TValue& refFinalValue = value.mapContent[refKey];
	value.Unlock();

	return refFinalValue;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashMap for std::string

template<typename TValue, int nHashSize, int nStep>
class CSTXHashMap<std::string, TValue, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>
{
public:
	CSTXHashMap()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashMapValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::map<std::string, TValue> mapContent;
		HashMapValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashMapValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashMapValue(const HashMapValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashMapValue &operator=(const HashMapValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			mapContent = val.mapContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
			lock.EnterLock();
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
			lock.LeaveLock();
		}

	};

private:
	typedef typename std::map<std::string, TValue>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashMapValue> _mapValues;


public:
	TValue& operator[](const std::string& refKey);
	size_t size();
	size_t erase(std::string _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].mapContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = THashClass()(_itval->first);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	// 	_Type_iterator begin()	//No default implements
	_Type_iterator end(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		return value.mapContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashMapValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.mapContent.begin(), (*it).second.mapContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Unlock();
	}
	void lockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Lock();
		}
	}
	void unlockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Unlock();
		}
	}
};


template<typename TValue, int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashMap<std::string, TValue, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>::erase(std::string _Keyval)
{
	size_t nHashed = THashClass()(_Keyval);
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.mapContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}

template<typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/>
size_t CSTXHashMap<std::string, TValue, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].mapContent.size();
	}
	return sizeTotal;
}

template<typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/>
TValue& CSTXHashMap<std::string, TValue, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>::operator[](const std::string& refKey)
{
	size_t nHashed = THashClass()(_Keyval);
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	TValue& refFinalValue = value.mapContent[refKey];
	value.Unlock();

	return refFinalValue;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashMap for std::wstring

template<typename TValue, int nHashSize, int nStep>
class CSTXHashMap<std::wstring, TValue, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>
{
public:
	CSTXHashMap()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashMapValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::map<std::wstring, TValue> mapContent;
		HashMapValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashMapValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashMapValue(const HashMapValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashMapValue &operator=(const HashMapValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			mapContent = val.mapContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
			lock.EnterLock();
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
			lock.LeaveLock();
		}

	};

private:
	typedef typename std::map<std::wstring, TValue>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashMapValue> _mapValues;


public:
	TValue& operator[](const std::wstring& refKey);
	size_t size();
	size_t erase(std::wstring _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].mapContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_itval->first);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(std::wstring _Keyval)
	{
		size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.mapContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	// 	_Type_iterator begin()	//No default implements
	_Type_iterator end(std::wstring _Keyval)
	{
		size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		return value.mapContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashMapValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.mapContent.begin(), (*it).second.mapContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(std::wstring _Keyval)
	{
		size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(std::wstring _Keyval)
	{
		size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_Keyval);
		HashMapValue &value = _mapValues[nHashed];
		value.Unlock();
	}
	void lockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Lock();
		}
	}
	void unlockall()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			HashMapValue &value = _mapValues[i];
			value.Unlock();
		}
	}
};


template<typename TValue, int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashMap<std::wstring, TValue, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>::erase(std::wstring _Keyval)
{
	size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(_Keyval) % nHashSize;
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.mapContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}

template<typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/>
size_t CSTXHashMap<std::wstring, TValue, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].mapContent.size();
	}
	return sizeTotal;
}

template<typename TValue, int nHashSize /*= 4*/, int nStep /*= 1*/>
TValue& CSTXHashMap<std::wstring, TValue, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>::operator[](const std::wstring& refKey)
{
	size_t nHashed = CSTXDefaultWStringHash<nHashSize, nStep>()(refKey);
	HashMapValue &value = _mapValues[nHashed];
	value.Lock();
	TValue& refFinalValue = value.mapContent[refKey];
	value.Unlock();

	return refFinalValue;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashSet

template<typename TKey, int nHashSize = 8, int nStep = 1, typename THashClass = CSTXDefaultHash<TKey, nHashSize, nStep>>
class CSTXHashSet
{
public:
	CSTXHashSet()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashSetValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::set<TKey> setContent;
		HashSetValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashSetValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashSetValue(const HashSetValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashSetValue &operator=(const HashSetValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			setContent = val.setContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
		}

	};

private:
	typedef typename std::set<TKey>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashSetValue> _mapValues;


public:
	//TValue& operator[](const TKey& refKey);
	size_t size();
	size_t erase(TKey _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].setContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = THashClass()(*_itval); // (((DWORD_PTR)(*_itval) / nStep) % nHashSize);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	void insert(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		value.setContent.insert(_Keyval);
		value.Unlock();
	}
	_Type_iterator end(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		return value.setContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashSetValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.setContent.begin(), (*it).second.setContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(TKey _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Unlock();
	}
};

template<typename TKey, int nHashSize /*= 8*/, int nStep /*= 1*/, typename THashClass>
size_t CSTXHashSet<TKey, nHashSize, nStep, THashClass>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].setContent.size();
	}
	return sizeTotal;
}


template<typename TKey, int nHashSize /*= 8*/, int nStep /*= 1*/, typename THashClass>
size_t CSTXHashSet<TKey, nHashSize, nStep, THashClass>::erase(TKey _Keyval)
{
	size_t nHashed = THashClass()(_Keyval);
	HashSetValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.setContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashSet for std::string

template<int nHashSize, int nStep>
class CSTXHashSet<std::string, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>
{
public:
	CSTXHashSet()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashSetValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::set<std::string> setContent;
		HashSetValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashSetValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashSetValue(const HashSetValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashSetValue &operator=(const HashSetValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			setContent = val.setContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
			lock.EnterLock();
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
			lock.LeaveLock();
		}

	};

private:
	typedef typename std::set<std::string>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashSetValue> _mapValues;


public:
	//TValue& operator[](const std::string& refKey);
	size_t size();
	size_t erase(std::string _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].setContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = THashClass()(*_itval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	void insert(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		value.setContent.insert(_Keyval);
		value.Unlock();
	}
	_Type_iterator end(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		return value.setContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashSetValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.setContent.begin(), (*it).second.setContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(std::string _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Unlock();
	}
};

template<int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashSet<std::string, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].setContent.size();
	}
	return sizeTotal;
}


template<int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashSet<std::string, nHashSize, nStep, CSTXDefaultStringHash<nHashSize, nStep>>::erase(std::string _Keyval)
{
	size_t nHashed = THashClass()(_Keyval);
	HashSetValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.setContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CSTXHashSet for std::wstring

template<int nHashSize, int nStep>
class CSTXHashSet<std::wstring, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>
{
public:
	CSTXHashSet()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i];	//Create Objects;
		}
	}
protected:
	struct HashSetValue
	{
		bool bCSInitialized;
		CRITICAL_SECTION cs;
		std::set<std::wstring> setContent;
		HashSetValue()
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
		}
		~HashSetValue()
		{
			DeleteCriticalSection(&cs);
		}
		HashSetValue(const HashSetValue &val)
		{
			InitializeCriticalSection(&cs);
			bCSInitialized = true;
			*this = val;
		}
		HashSetValue &operator=(const HashSetValue &val)
		{
			if (!bCSInitialized)
				InitializeCriticalSection(&cs);
			setContent = val.setContent;
			return *this;
		}
		void Lock()
		{
			EnterCriticalSection(&cs);
			lock.EnterLock();
		}
		void Unlock()
		{
			LeaveCriticalSection(&cs);
			lock.LeaveLock();
		}

	};

private:
	typedef typename std::set<std::wstring>::iterator _Type_iterator;


protected:
	std::unordered_map<size_t, HashSetValue> _mapValues;


public:
	//TValue& operator[](const std::wstring& refKey);
	size_t size();
	size_t erase(std::wstring _Keyval);
	void clear()
	{
		for (size_t i = 0; i < nHashSize; i++)
		{
			_mapValues[i].setContent.clear();
		}
	}

public:
	_Type_iterator erase(_Type_iterator _itval)
	{
		size_t nHashed = THashClass()(*_itval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.erase(_itval);
		value.Unlock();

		return refResult;
	}
	_Type_iterator find(std::wstring _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		_Type_iterator refResult = value.setContent.find(_Keyval);
		value.Unlock();

		return refResult;
	}
	void insert(std::wstring _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
		value.setContent.insert(_Keyval);
		value.Unlock();
	}
	_Type_iterator end(std::wstring _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		return value.setContent.end();
	}
	template<typename TFunc>
	void foreach(TFunc pfnFunc)
	{
		std::unordered_map<size_t, HashSetValue>::iterator it = _mapValues.begin();
		for (; it != _mapValues.end(); it++)
		{
			(*it).second.Lock();
			std::for_each((*it).second.setContent.begin(), (*it).second.setContent.end(), pfnFunc);
			(*it).second.Unlock();
		}
	}
	void lock(std::wstring _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Lock();
	}
	void unlock(std::wstring _Keyval)
	{
		size_t nHashed = THashClass()(_Keyval);
		HashSetValue &value = _mapValues[nHashed];
		value.Unlock();
	}
};

template<int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashSet<std::wstring, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>::size()
{
	size_t sizeTotal = 0;
	for (size_t i = 0; i < nHashSize; i++)
	{
		sizeTotal += _mapValues[i].setContent.size();
	}
	return sizeTotal;
}


template<int nHashSize /*= 8*/, int nStep /*= 1*/>
size_t CSTXHashSet<std::wstring, nHashSize, nStep, CSTXDefaultWStringHash<nHashSize, nStep>>::erase(std::wstring _Keyval)
{
	size_t nHashed = THashClass()(_Keyval);
	HashSetValue &value = _mapValues[nHashed];
	value.Lock();
	size_t refResult = value.setContent.erase(_Keyval);
	value.Unlock();

	return refResult;
}

//////////////////////////////////////////////////////////////////////////
//

