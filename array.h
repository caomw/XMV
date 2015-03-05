#pragma once
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <cstdlib>

#include "memory_.h"
#include "convert.h"
#include "common.h"
#include "io.h"
#include <fstream>
#include "rgb.h"

using namespace std;

#pragma once

extern double epsilon;

namespace xmv
{
#define EnumArr(type, pointer) for(T * pointer = dataPtr; pointer < dataEndPtr; ++pointer)


	template<typename T>
	class Array
	{
	protected:
		int size;								//size count of the Array
		int memorySize;							//memory size of the Array  = size * sizeof(T)
		T * dataPtr;							//the start fo Array array
		T * dataEndPtr;							//the end of Array array

	public:
		Array();
		Array(int size);
		Array(int size, T value);
		Array(Array<T> & other);
		Array(T * arr, int size);
		Array(BGR<T> * arr, int size);
		Array(int size, T* arr, int arrSize);
		template <typename T2> Array(Array<T2> & other);
		template <typename T2> Array(T2 * arr, int size);
		template <typename T2> Array(int size, T2 * arr, int arrSize);

		~Array(void);

		inline int Size();						// ����Ԫ�صĸ���
		inline int MemorySize();				// ����ʵ��ռ���ڴ���ֽ���
		inline void ReSize(int size);			// �����趨��С��������е�����
		inline T * Buffer();					// ��ȡ�ڲ�������׵�ַ
		inline void SetValue(int index, T val);	// ���õ�index�����ݵ�ֵ
		inline T GetValue(int index);			// ��õ�index������
		T SquareOfNorm2();						// 2��ʽ��ƽ��
		T SquareOfDistance(Array<T> & another);	// ������һ������ľ����ƽ��
		T Distance(Array<T> & another);			// ������������ľ���
		Array<T> Cut(int start, int len);		// �������н�ȡһ���ֳ�Ϊ�µ�����
		Array<T> Cut(int start);				// ���������һ���֣������µ����飬��start��ʼ��һֱ������
		T Norm0();								// 0��ʽ�������0Ԫ�ظ���
		T Norm1();								// 1��ʽ����������Ԫ�صĺ�
		T Norm2();								// 2��ʽ����������Ԫ�ص�ƽ���Ϳ�����
		T Norm(int p = 2);						// p��ʽ
		T Sum();								// ����Ԫ�صĺ�
		void SetSmallValueToZero();				// ������ֵС��eps����ֵ����Ϊ0
		T AbsMin();								// ���ؾ���ֵ����Сֵ
		T Min();								// ������Сֵ
		T AbsMax();								// ���ؾ���ֵ�����ֵ
		T Max();								// �������ֵ
		void Offset(T value);					// ��������ƫ��value
		T Average();							// ����������ݵ�ƽ��ֵ
		void AverageTo(T value = 0);			// ������ƽ������ĳ����ֵ
		T Variance();							// ���ط���
		void GaussianWhiten();					// ��˹�׻���ʹ������ƽ��ֵΪ0������Ϊ1
		void AbsValues();						// ���е�Ԫ��ȡ����ֵ
		bool InfinityCheck(T maxValue = 1E50);	// ���ݼ�飬���������Ƿ�������Ϊ�����
		int ClearInfinityValues(T maxValue = 1E50, T clearValue = 0);	// ���ݼ�飬���������Ƿ�������Ϊ���������У������Ϊclearvalue��������յ����ݵĸ���
		bool IsZero();							// ���������Ƿ�ȫ��Ϊ0
		void Clear();							// ������ȫ����0
		void Normalization();					// ���滯��ÿ��Ԫ��ƽ����Ϊ1
		void SumAsOne();						// ��ÿ��Ԫ�صĺ�Ϊ1
		void SetMinTo(T value);					// ��С��value��Ԫ��ȫ������Ϊvalue
		void SetMaxTo(T value);					// ������value��Ԫ��ȫ������Ϊvalue
		bool Contains(T value, int begin = 0, int len = -1); //�����Ƿ����ֵΪvalue��Ԫ��=-0987654321`
		Array<int> histgram(int binnum);

		virtual void WriteToTextFile(string filename, bool append = false);
		virtual void WriteToTextFile(ofstream & o);
		virtual void ReadFromTextFile(string filename);
		virtual void ReadFromTextFile(ifstream & i);

	protected: 
		inline void Init(int size);

	public:
		Array<T> & operator = (Array<T> & v);
		Array<T> & operator = (T value);
		T & operator[] (int index);
		template<typename T2> Array<T> & operator = (Array<T2> & v);

		template<typename T> friend ostream & operator << (ostream & out, Array<T> & vector);
		template<typename T> friend istream & operator >> (istream & in, Array<T> & vector);
	};

	template<typename T>
	inline int Array<T>::Size()
	{
		return size; 
	}

	template<typename T>
	inline int Array<T>::MemorySize()
	{ 
		return memorySize;
	}

	template<typename T>
	inline T * xmv::Array<T>::Buffer()
	{
		return dataPtr; 
	}

	template<typename T>
	inline void xmv::Array<T>::SetValue(int index, T val)
	{
		dataPtr[index] = val;
	}

	template<typename T>
	inline T Array<T>::GetValue(int index)
	{
		return dataPtr[index]; 
	}

	//��˹�׻�
	template<typename T> void Array<T>::GaussianWhiten()
	{
		AverageTo(0);
		T val = Sqrt<T>(this->Variance());
		if (val == 0) 
		{
			val = Sqrt<T>(this->Average());
			return;
		}

		EnumArr(T, ptr)
			*ptr /= val;
	}

	template<typename T>
	T Array<T>::SquareOfNorm2()
	{
		T sum = 0;
		EnumArr(T, t)
			sum += *t * *t;
		return sum;
	}

	template<typename T>
	T Array<T>::Norm2()
	{ 
		return Sqrt<T>(SquareOfNorm2()); 
	}

	template<typename T>
	T Array<T>::SquareOfDistance(Array<T> & another)
	{
		T sum = 0;
		T * p1, *p2, t;
		int s = xmv::Min(this->size, another.size);
		enumArray2(this->dataPtr, another.dataPtr, 0, s, p1, p2)
		{
			t = *p1 - *p2;
			sum += t * t;
		} enumArrayEnd;
		return sum;
	}


	template<typename T>
	T Array<T>::Distance(Array<T> & another)
	{
		return Sqrt<T>(SquareOfDistance(another));
	}

	template<typename T>
	Array<T> Array<T>::Cut(int start, int len)
	{
		Array<T> v(len);
		memcpy(v.dataPtr, this->dataPtr + start, len * sizeof(T));
		return v;
	}

	template<typename T>
	Array<T> Array<T>::Cut(int start)
	{
		int len = this->size - start;
		Array<T> v(len);
		memcpy(v.dataPtr, this->dataPtr + start, len * sizeof(T));
		return v;
	}


	template<typename T> T Array<T>::Average()					// ����������ݵ�ƽ��ֵ
	{
		T sum = 0;
		EnumArr(T, ptr)
			sum += *ptr;
		return sum / size;
	}

	template<typename T> T Array<T>::Variance()							// ���ط���
	{
		T avg = Average();
		T sum = 0;
		T temp;
		EnumArr(T, ptr)
		{
			temp = *ptr - avg;
			sum += temp * temp;
		}
		return sum / this->size;
	}

	template<typename T>
	void Array<T>::AverageTo(T value = 0)
	{
		T avg = this->Average();
		EnumArr(T, ptr)
		{
			*ptr -= (avg - value);
		}
	}

	template<typename T>
	T Array<T>::Norm0() // 0��ʽ�������0Ԫ�ظ���
	{
		int count = 0;
		EnumArr(T, ptr)
			if (Abs<T>(*ptr) > epsilon) count++;
		return count;
	}

	template<typename T>
	T Array<T>::Norm1() // 1��ʽ����������Ԫ�صĺ�
	{
		T sum = 0;
		EnumArr(T, ptr)
			sum += *ptr;
		return sum;
	}

	template<typename T>
	T Array<T>::Sum() // ��������Ԫ�صĺ�
	{
		T sum = 0;
		EnumArr(T, ptr)
			sum += *ptr;
		return sum;
	}

	template<typename T>
	inline void Array<T>::Init(int size)
	{
		this->size = size;
		memorySize = size * sizeof(T);

		dataPtr = NEW("XMV.Array<T>.Init: Create Array Buffer") T[size];
		dataEndPtr = dataPtr + size;
	}

	template<typename T>
	Array<T>::Array()
	{ 
		Init(1); 
	}

	template<typename T>
	Array<T>::Array(int size)
	{
		Init(size);
	}

	template<typename T>
	Array<T>::Array(int size, T value)
	{
		Init(size);

		EnumArr(T, ptr)
			*ptr = value;
	}

	template<typename T>
	Array<T>::	Array(T * arr, int size)
	{
		Init(size);
		memcpy(this->dataPtr, arr, this->memorySize);
	}

	template<typename T>
	Array<T>::Array(int size, T* arr, int arrSize)
	{
		Init(size);
		memcpy(this->dataPtr, arr, min(this->memorySize, arrSize * sizeof(T)));
	}

	template<typename T>
	template<typename T2>
	Array<T>::	Array(T2 * arr, int size)
	{
		Init(size);
		T * p1;
		T2 * p2;
		enumArray2(this->dataPtr, arr, 0, size, p1, p2)
		{
			*p1 = *p2;
		}
		enumArrayEnd;
	}

	template<typename T>
	Array<T>::	Array(BGR<T> * arr, int size)
	{
		Init(size);
		T * p1;
		BGR<T> * p2;
		enumArray2(this->dataPtr, arr, 0, size, p1, p2)
		{
			*p1 = p2->Gray();
		}
		enumArrayEnd;
	}

	template<typename T>
	template<typename T2>
	Array<T>::	Array(int size, T2* arr, int arrSize)
	{
		Init(size);
		T * p1;
		T2 * p2;
		enumArray2(this->dataPtr, arr, 0, min(size, arrSize), p1, p2)
		{
			*p1 = *p2;
		}
		enumArrayEnd;
	}

	template<typename T>
	Array<T>::Array(Array<T> & other)
	{
		Init(other.size);
		::memcpy(this->dataPtr, other.dataPtr, memorySize);
	}

	template<typename T>
	template<typename T2>
	Array<T>::Array(Array<T2> & other)
	{
		Init(other.Size());
		T * ptr;
		T2 * ptr2;
		enumArray2(this->dataPtr, ohter.Buffer(), 0, size, ptr, ptr2)
			*ptr = (*ptr2);
		enumArrayEnd;
	}

	template<typename T>
	inline void Array<T>::ReSize(int size) 
	{
		if (this->size != size)
		{
			DELETEARR(dataPtr);
			Init(size);
		}
	}

	template<typename T> Array<T>::~Array(void)
	{
		DELETEARR(dataPtr);
	}

	extern DigtalFormat ____currentDigtalFormat;

	template<typename T> ostream & operator << (ostream & out, Array<T> & data)
	{
		out << ____currentDigtalFormat;

		T * ptr;
		for(T * ptr = data.dataPtr; ptr < data.dataEndPtr; ++ptr)
		{
			out << setw(____currentDigtalFormat.space) << *ptr << ' ';
		}

		return out;
	}

	template<typename T> istream & operator >> (istream & in, Array<T> & data)
	{
		T * ptr = data.dataPtr;
		int size = data.size;

		for(int y = 0; !in.eof() && y < size; ++y)
		{
			in >> *ptr++;
		}

		return in;
	}

	template<typename T> Array<T> & Array<T>::operator = (Array<T> & v)
	{
		this->ReSize(v.size);

		memcpy(this->dataPtr, v.dataPtr, this->memorySize);

		return *this;
	}

	template<typename T> template<typename T2> Array<T> & Array<T>::operator = (Array<T2> & v)
	{
		this->ReSize(v.Size());
		T * p1;
		T2 * p2;
		enumArray2(this->dataPtr, v.Buffer(), 0, this->size, p1, p2)
		{
			*p1 = (T)(*p2);
		}enumArrayEnd

			return *this;
	}

	template<typename T> Array<T> & Array<T>::operator = (T value)
	{
		EnumArr(T, ptr)
			*ptr = value;
		return *this;
	}

	template<typename T> T & Array<T>::operator[] (int index)
	{
		return this->dataPtr[index];
	}

	//��С��epsilon��ֵ����Ϊ0
	template<typename T> void Array<T>::SetSmallValueToZero() 
	{ 
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			if (Abs<T>(*ptr) < epsilon) *ptr = 0;
		return *this;
	}

	template<typename T> T Array<T>::AbsMin()
	{
		T max;
		max = 0;
		EnumArr(T, ptr)
			if (Abs<T>(*ptr) < max) max = Abs<T>(*ptr);
		return max;
	}
	template<typename T> T Array<T>:: Min()
	{
		T max;
		max = 0;
		EnumArr(T, ptr)
			if (*ptr < max) max = *ptr;
		return max;
	}
	template<typename T> T Array<T>::AbsMax()
	{
		T max;
		max = 0;
		EnumArr(T, ptr)
			if (Abs<T>(*ptr) > max) max = Abs<T>(*ptr);
		return max;
	}

	template<typename T> T Array<T>::Max()
	{
		T max;
		max = 0;
		EnumArr(T, ptr)
			if (*ptr > max) max = *ptr;
		return max;
	}

	template<typename T> void Array<T>::Offset(T value)
	{
		T max = 0;
		EnumArr(T, ptr)
			* ptr += value;
	}

	template<typename T> T Array<T>::Norm(int p)	//��ʽ
	{
		T sum = 0;
		EnumArr(T, ptr)
			sum += Pow<T>(*ptr, p);
		return Pow<T>(sum, 1.0 / p);
	}		

	template<typename T> void Array<T>::AbsValues()
	{
		EnumArr(T, ptr)
			*ptr = Abs<T>(*ptr);
	}

	// ���ݼ�飬���������Ƿ�������Ϊ�����
	template<typename T> bool Array<T>::InfinityCheck(T maxValue)		// ���ݼ�飬���������Ƿ�������Ϊ�����
	{
		EnumArr(T, ptr)
		{
			if (! (*ptr > -maxValue)) return false;
			if (! (*ptr < maxValue)) return false;
		}
		return true;
	}

	// ���ݼ�飬���������Ƿ�������Ϊ���������У������Ϊclearvalue��������յ����ݵĸ���
	template<typename T> int Array<T>::ClearInfinityValues(T maxValue, T clearValue)
	{
		int count = 0;
		EnumArr(T, ptr)
		{
			if (! (*ptr > -maxValue)) { count++; *ptr = clearValue; }
			if (! (*ptr < maxValue)) { count++; *ptr = clearValue; }
		}
		return count;
	}


	// ���ݼ�飬���������Ƿ�������Ϊ�����
	template<typename T> bool Array<T>::IsZero()		// ���ݼ�飬���������Ƿ�������Ϊ�����
	{
		EnumArr(T, ptr)
		{
			if (*ptr > epsilon) return false;
			if (*ptr < -epsilon) return false;
		}
		return true;
	}

	//��������0
	template<typename T> void Array<T>::Clear()
	{
		memset(dataPtr, 0, memorySize);
	}

	// ���滯��ÿ��Ԫ��ƽ����Ϊ1
	template<typename T> void Array<T>::Normalization()
	{
		T v = this->Norm2();
		if (Abs<T>(v) > epsilon)
			EnumArr(T, ptr)
			*ptr /= v;
	}

	// ʹÿ��Ԫ�صĺ�Ϊ1
	template<typename T> void Array<T>::SumAsOne()
	{
		T v = this->Norm1();
		if (Abs<T>(v) > epsilon)
			EnumArr(T, ptr)
			*ptr /= v;
	}

	// ��С��value��Ԫ��ȫ������Ϊvalue
	template<typename T> void Array<T>::SetMinTo(T value)
	{
		EnumArr(T, ptr)
			if (*ptr < value) *ptr = value;
	}
	// ������value��Ԫ��ȫ������Ϊvalue
	template<typename T> void Array<T>::SetMaxTo(T value)
	{			
		EnumArr(T, ptr)
			if (*ptr > value) *ptr = value;
	}


	template<typename T> void Array<T>::WriteToTextFile(ofstream & o)
	{
		o << this->size << endl;
		o << format(10, 12) << *this;
	}

	template<typename T> void Array<T>::WriteToTextFile(string filename, bool append = false)
	{
		ofstream o;

		if (append)
			o.open(filename, std::ios_base::ate);
		else
			o.open(filename);

		this->WriteToTextFile(o);

		o.close();
	}

	template<typename T> void Array<T>::ReadFromTextFile(string filename)
	{
		ifstream i;
		this->ReadFromTextFile(i);
		i.close();
	}

	template<typename T> void Array<T>::ReadFromTextFile(ifstream & i)
	{
		int s;
		i >> s;
		ReSize(s);

		i >> *this;
	}

	//�����Ƿ����ֵΪvalue��Ԫ��
	template<typename T> bool Array<T>::Contains(T value, int begin = 0, int len = -1)
	{
		if (len < 0) len = size - begin;
		T * p;
		enumArray(this->dataPtr, begin, begin + len, p)
		{
			if (*p == value) 
				return true;
		}
		enumArrayEnd;

		return false;
	}

	// get the histgram of array
	template <typename T> Array<int> Array<T>::histgram(int binnum)
	{
		T step = (Max() - Min()) / binnum;
		Array<int> hist(binnum, 0);

		for (T* ptr = Buffer(); ptr < dataEndPtr; ptr++)
		{
			hist[(int) (*(ptr) / step) % binnum] ++;
		}
		return hist;
	}
}