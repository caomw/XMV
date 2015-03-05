#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include "memory.h"
#include "svd.h"
#include "array.h"
#include "io.h"

using namespace std;

namespace xmv
{
	enum MedianFilterSmoothingWindowType
	{
		Square,
		Circle
	};

	enum MedianFilterDataType
	{
		Binary,
		Double,
	};

	extern double epsilon;


	template<typename T> class Vector;
	template<typename T> class VectorH;
	template<typename T> class SVD;
	template<typename T> class Array;

	template<typename T> class Matrix : public Array<T>
	{
	protected:
		int row;				//row count of the matrix
		int col;				//col count of the matrix
	public:
		Matrix();
		Matrix(int rowCount, int colCount);
		Matrix(int rowCount, int colCount, T value);
		Matrix(int rowCount, int colCount, T * valueArr, int valueArrSize);
		Matrix(Matrix<T> & other);
		template<typename T2> Matrix(Matrix<T2> & other);
		template<typename T2> Matrix(int rowCount, int colCount, T2 * valueArr, int valueArrSize);
		~Matrix(void);

		inline int Row();															//���ؾ��������
		inline int Col();															//���ؾ��������	
		inline void ReSize(int rowCount, int colCount);								//���¶������Ĵ�С����������е�����
		inline void SetValue(int rowIndex, int colIndex, T val);					//���þ����е�����
		inline T GetValue(int rowIndex, int colIndex);								//���ؾ����е�����

		T * Buffer() { return dataPtr; }
		int Size() { return size; }
		int MemorySize() { return memorySize; }

		inline Vector<T> ToVector();	
		inline VectorH<T> ToVectorH();

		inline void TransformRowMul(int row, T factor);								//Row[row]  *= factor
		inline void TransformRowDiv(int row, T factor);								//Row[row]  /= factor
		inline void TransformRowSum(int row1, int row2);							//Row[row1] += Row[row2]
		inline void TransformRowSub(int row1, int row2);							//Row[row1] -= Row[row2]
		inline void TransformRow(int row1, T factor1, int row2, T factor2);			//Row[row1] = Row[row1] * factor1 + Row[row2] * factor2
		inline void TransformRowSwap(int row1, int row2);							//Row[row1] <==> Row[row2]
		inline void TransformColSwap(int col1, int col2);							//Col[col1] <==> Col[col2]

		VectorH<T> GetRow(int rowIndex);											//��þ����ĳһ��
		Vector<T> GetCol(int colIndex);												//��þ����ĳһ��
		Matrix<T> SelectColumns(Vector<int> & indexVector);							//�Ӿ���ѡȡindexVector��ָ�����У�����µľ���
		void SetCol(int colIndex, T value);											//�������þ����ĳһ��Ϊͬ����ֵvalue
		void SetCol(int colIndex, Vector<T> & colVector);							//�������þ����ĳһ��
		template<typename T2> void SetCol(int colIndex, Vector<T2> & colVector);	//�������þ����ĳһ��
		void SetRow(int rowIndex, T value);											//�������ĳһ������Ϊͬ��������value
		template<typename T2> void SetRow(int rowIndex, VectorH<T2> & rowVector);	//�������þ����ĳһ��
		void SetRow(int rowIndex, VectorH<T> & rowVector);							//�������þ����ĳһ��
		Matrix<T> Cut(int rowStart, int rowCount, int colStart, int colCount);		//��þ����һ����
		Matrix<T> Cut(int rowStart, int colStart);									//��þ����һ����
		void CopyFrom(Matrix<T> & source, int dstRowStart, int dstColStart);		//����һ����С�ľ����Ƶ���������

		Matrix<T> Transpose(void);													//����ת�þ���
		Matrix<T> ConjugateTranspose(void);											//���ع���ת��
		Matrix<T> Inverse();														//���ظþ������
		Matrix<T> Pseudoinverse();													//���ظþ���Ĺ�����
		Matrix<T> EigenvaluesWithQRMethod(int maxNumOfIterations = 10000);			//�������ֵ����
		Matrix<T> HessenbergUpperMatrix();											//�Ծ������Hessenberg���Ʊ任���γ���׼���Ǿ���
		Matrix<T> Diagonalization2Upper(Matrix<T> & U, Matrix<T> & V);				//���ض��Խǻ����󣬲����U��V ʹ�� U* A V = B
		void SVD(Matrix<T> & U, Matrix<T> & sigma, Matrix<T> & V);					//�Ծ������SVD�ֽ⣬����QR����
		void SVD(Matrix<T> & U, Vector<T> & sigma, Matrix<T> & V);					//�Ծ������SVD�ֽ⣬����QR����
		void GaussianElimination(bool eliminateUpperHalf = true);					//�Ծ�����г��ȱ任��ʹ�þ����ǰrow�г�Ϊ��λ��
		int Rank();																	//��øþ������
		bool IsFullRank();															//���ظþ����Ƿ������ȵ�
		T Determinant();															//��øþ��������ʽ
		template<typename T2> Matrix<T> Convolution(Matrix<T2> & kernel);		//ʹ��Kernel���о��	
		T MaxPosition(int x1, int x2, int y1, int y2, int & x, int & y);			//������	x1 ~ x2, y1 ~ y2���ҵ����ֵ
		T MinPosition(int x1, int x2, int y1, int y2, int & x, int & y);			//������	x1 ~ x2, y1 ~ y2���ҵ���Сֵ
		template<typename T2> T PartialInnerProduct(Matrix<T2> mask, int x1, int x2, int y1, int y2); //�����ڻ�

		virtual void WriteToTextFile(string filename, bool append = false);
		virtual void WriteToTextFile(ofstream & o);
		virtual void ReadFromTextFile(string filename);
		virtual void ReadFromTextFile(ifstream & i);

		//��������������������Ϊ��
		void Not(bool (* notFunction)(int x, int y) );

		//Canny��Ե��⣬edgeThreshold����Ե��ֵ��edgeThreshold2Factor: �ڶ���Ե��ֵϵ����0 ~ 1֮�䣩��gaussianFilterSize����˹�˲������ڴ�С
		Matrix<T> Canny(T edgeThreshold, double edgeThreshold2Factor = 0.4, int gaussianFilterSize = 5,
			double powerOfUpHorizontal = 1,
			double powerOfVehical = 1,
			double powerOfTopLeftToBottomRight = 1,
			double powerOfTopRightToBottomLeft = 1);

		//Canny��Ե��⣬CannyDirectionPower�ص����������ø�����ı�Եǿ�ȣ�0 ~ 1��
		//edgeThreshold����Ե��ֵ��edgeThreshold2Factor: �ڶ���Ե��ֵϵ����0 ~ 1֮�䣩��gaussianFilterSize����˹�˲������ڴ�С
		Matrix<T> Canny(double (*CannyDirectionPower) (double direction, int x, int y), 
			T edgeThreshold, double edgeThreshold2Factor = 0.4, int gaussianFilterSize = 5);		

		//��˹ƽ��
		Matrix<T> GaussianSmoothing(int gaussianFilterSize = 5);
		//��ֵ�˲�ƽ��
		Matrix<T> MedianFilterSmoothing(int windowSize = 5, 
			MedianFilterSmoothingWindowType windowType = MedianFilterSmoothingWindowType::Square,
			MedianFilterDataType dataType = MedianFilterDataType::Double,
			double falseTrueValue = 0.5
			);
		//OTSU�ָ�
		T OTSU();

		Matrix<T> ReduiceSize(int n);

	public:
		Matrix<T> & operator = (Matrix<T> & m);
		Matrix<T> & operator = (T value);

		Matrix<T> & operator += (Matrix<T> & m);

		Matrix<T> & operator -= (Matrix<T> & m);

		Matrix<T> & operator *= (Matrix<T> & m);
		Matrix<T> & operator *= (T value);
		template<typename T2> Matrix<T> & operator *= (T2 value);

		Matrix<T> & operator /= (T value);
		template<typename T2> Matrix<T> & operator /= (T2 value);

		Matrix<T> operator ~ ();
		Matrix<T> operator ! ();
		T * operator [] (int rowIndex);

		template<typename T> friend Matrix<T> operator + (Matrix<T> & m);
		template<typename T> friend Matrix<T> operator - (Matrix<T> & m);

		template<typename T> friend Matrix<T> operator + (Matrix<T> & m1, Matrix<T> & m2);
		template<typename T> friend Matrix<T> operator - (Matrix<T> & m1, Matrix<T> & m2);

		template<typename T> friend Matrix<T> operator * (Matrix<T> & m1, Matrix<T> & m2);
		template<typename T> friend Matrix<T> operator * (T value, Matrix<T> & matrix);
		template<typename T> friend Matrix<T> operator * (Matrix<T> & matrix, T value);
		template<typename T, typename T2> friend Matrix<T> operator * (Matrix<T> & matrix, T2 value);

		template<typename T> friend Matrix<T> operator / (Matrix<T> & matrix, T value);
		template<typename T, typename T2> friend Matrix<T> operator / (Matrix<T> & matrix, T2 value);

		template<typename T> friend ostream & operator << (ostream & out, Matrix<T> & matrix);
		template<typename T> friend istream & operator >> (istream & in, Matrix<T> & matrix);

		friend class Vector<T>;
	};

	template<typename T> inline Vector<T> Matrix<T>::ToVector()
	{
		return Vector<T>(this->dataPtr, this->size);
	}

	template<typename T> inline VectorH<T> Matrix<T>::ToVectorH()
	{
		return VectorH<T>(this->dataPtr, this->size);
	}

	template<typename T> T * Matrix<T>::operator [] (int rowIndex)
	{
		return dataPtr + rowIndex * col;
	}


	template<typename T> Matrix<T>::Matrix()
	{	
	}

	template<typename T> Matrix<T>::Matrix(int rowCount, int colCount) 
		: row(rowCount), col(colCount), Array(rowCount * colCount)
	{
	}

	template<typename T> Matrix<T>::Matrix(Matrix<T> & other)
		: row(other.row), col(other.col), Array(other.dataPtr, other.row * other.col)
	{
	}

	template<typename T> template<typename T2> Matrix<T>::Matrix(Matrix<T2> & other)
		: row(other.Row()), col(other.Col()), Array(other.Buffer(), other.Row() * other.Col())
	{
	}

	template<typename T> Matrix<T>::Matrix(int rowCount, int colCount, T value) 
		: row(rowCount), col(colCount), Array(rowCount * colCount, value)
	{
	}

	template<typename T> Matrix<T>::Matrix(int rowCount, int colCount, T * valueArr, int valueArrSize)
		: row(rowCount), col(colCount), Array(rowCount * colCount, valueArr, valueArrSize)
	{
	} 

	template<typename T> template<typename T2> Matrix<T>::Matrix(int rowCount, int colCount, T2 * valueArr, int valueArrSize)
		: row(rowCount), col(colCount), Array(rowCount * colCount, valueArr, valueArrSize)
	{
	}

	template<typename T> void Matrix<T>::ReSize(int rowCount, int colCount) 
	{
		row = rowCount;
		col = colCount;
		Array::ReSize(row * col);
	}

	//���ؾ��������
	template<typename T> inline int  Matrix<T>::Row() 
	{
		return row;
	}									

	//���ؾ��������	
	template<typename T> inline int  Matrix<T>::Col() 
	{
		return col; 
	}									

	//���þ����е�����
	template<typename T> inline void  Matrix<T>::SetValue(int rowIndex, int colIndex, T val) 
	{
		dataPtr[rowIndex * col + colIndex] = val;
	}  

	//���ؾ����е�����
	template<typename T> inline T  Matrix<T>::GetValue(int rowIndex, int colIndex) 
	{
		return dataPtr[rowIndex * col + colIndex]; 
	}			

	//���ת�þ���
	template<typename T> Matrix<T> Matrix<T>::Transpose()
	{
		Matrix m(col, row);
		T * ptr = dataPtr;
		T * mColPtr = m.dataPtr, * mPtr;
		int x, y;
		for(y = 0; y < row; ++y, ++mColPtr)
		{
			mPtr = mColPtr;
			for(x = 0; x < col; ++x, ++ptr, mPtr += row)
			{
				*mPtr = *ptr;
			}
		}
		return m;
	}

	//��ù���ת�þ���
	template<typename T> Matrix<T> Matrix<T>::ConjugateTranspose()
	{
		Matrix m(col, row);
		T * ptr = dataPtr;
		T * mColPtr = m.dataPtr, * mPtr;
		int x, y;
		for(y = 0; y < row; ++y, ++mColPtr)
		{
			mPtr = mColPtr;
			for(x = 0; x < col; ++x, ++ptr, mPtr += row)
			{
				*mPtr = Conjugate(*ptr);
			}
		}
		return m;
	}

	template<typename T> Matrix<T>::~Matrix(void)
	{
	}

	extern DigtalFormat ____currentDigtalFormat;

	template<typename T> ostream & operator << (ostream & out, Matrix<T> & matrix)
	{
		T * ptr = matrix.dataPtr;
		int x, y;
		int row = matrix.row;
		int col = matrix.col;

		//cout << ____currentDigtalFormat;

		for(y = 0; y < row; ++y)
		{
			for(x = 0; x < col; ++x)
			{
				out << setw(____currentDigtalFormat.space) << *ptr++ << ' ';
			}
			out << endl;
		}
		return out;
	}

	template<typename T> istream & operator >> (istream & in, Matrix<T> & matrix)
	{
		T * ptr = matrix.dataPtr;
		int x, y;

		for(y = 0; y < matrix.Row(); ++y)
		{
			for(x = 0; x < matrix.Col(); ++x)
			{
				in >> *ptr++;
			}
		}
		return in;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator = (Matrix<T> & m)
	{
		this->ReSize(m.Row(), m.Col());
		T * sourcePtr = m.Buffer();
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr, ++sourcePtr)
			*ptr = *sourcePtr;
		return *this;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator = (T value)
	{
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			*ptr = value;
		return *this;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator += (Matrix<T> & m)
	{
		T * sourcePtr = m.Buffer();
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr, ++sourcePtr)
			*ptr += *sourcePtr;
		return *this;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator *= (Matrix<T> & m)
	{
		return *this = *this * m;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator -= (Matrix<T> & m)
	{
		T * sourcePtr = m.Buffer();
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr, ++sourcePtr)
			*ptr -= *sourcePtr;
		return *this;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator *= (T value)
	{
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			*ptr *= value;
		return *this;
	}

	template<typename T> template<typename T2> Matrix<T> & Matrix<T>::operator *= (T2 value)
	{
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			*ptr *= value;
		return *this;
	}

	template<typename T> Matrix<T> & Matrix<T>::operator /= (T value)
	{
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			*ptr /= value;
		return *this;
	}

	template<typename T> template<typename T2> Matrix<T> & Matrix<T>::operator /= (T2 value)
	{
		for(T * ptr = this->dataPtr; ptr < this->dataEndPtr; ++ptr)
			*ptr /= value;
		return *this;
	}

	template<typename T> Matrix<T> Matrix<T>::operator ~ ()
	{
		return this->ConjugateTranspose();
	}

	template<typename T> Matrix<T> operator + (Matrix<T> & m1, Matrix<T> & m2)
	{
		Matrix<T> m = m1;
		m += m2;
		return m;
	}

	template<typename T> Matrix<T> operator - (Matrix<T> & m1, Matrix<T> & m2)
	{
		Matrix<T> m = m1;
		m -= m2;
		return m;
	}

	template<typename T> Matrix<T> operator + (Matrix<T> & matrix)
	{
		Matrix<T> m = matrix;
		return m;
	}

	template<typename T> Matrix<T> operator - (Matrix<T> & matrix)
	{
		Matrix<T> m(matrix.row, matrix.col);
		T * sourcePtr = matrix.dataPtr;
		for(T * ptr = m.dataPtr; ptr < m.dataEndPtr; ++ptr, ++sourcePtr)
			*ptr = -*sourcePtr;
		return m;
	}

	template<typename T> Matrix<T> operator * (Matrix<T> & m1, Matrix<T> & m2)
	{
		int c1 = m1.col;
		int c2 = m2.Col();
		int r1 = m1.row;
		int r2 = m2.Row();

		if (c1 != r2) throw;

		Matrix<T> m(r1, c2);

		int x, y, z;
		T * m1RowPtr = m1.dataPtr, *ptr1;
		T * m2Ptr = m2.Buffer(), *m2ColPtr, *ptr2;
		T * ptr = m.dataPtr;
		T sum;
		for(y = 0; y < r1; ++y, m1RowPtr += c1) //ѭ��m����
		{
			m2ColPtr = m2Ptr;
			for(x = 0; x < c2; ++x, ++m2ColPtr, ++ptr) //ѭ��m����
			{
				ptr1 = m1RowPtr;
				ptr2 = m2ColPtr;
				sum = 0;
				for(z = 0; z < c1; ++z, ++ptr1, ptr2 += c2) //����mÿ����Ԫ���ֵ
				{
					sum += *ptr1 * *ptr2;
				}
				*ptr = sum;
			}
		}

		return m;
	}

	template<typename T> Matrix<T> operator * (T val, Matrix<T> & matrix)
	{
		Matrix<T> m = matrix;
		m *= val;

		return m;
	}

	template<typename T> Matrix<T> operator * (Matrix<T> & matrix, T val)
	{
		Matrix<T> m = matrix;
		m *= val;

		return m;
	}


	template<typename T, typename T2> Matrix<T> operator * (Matrix<T> & matrix, T2 val)
	{
		Matrix<T> m = matrix;
		m *= val;

		return m;
	}

	template<typename T> Matrix<T> operator / (Matrix<T> & matrix, T val)
	{
		Matrix<T> m = matrix;
		m /= val;

		return m;
	}

	template<typename T, typename T2> Matrix<T> operator / (Matrix<T> & matrix, T2 val)
	{
		Matrix<T> m = matrix;
		m /= val;

		return m;
	}

	//ͨ������m������v�����������
	template<typename T> Matrix<T> CreateAugmentedMatrix(Matrix<T> & m, Vector<T> & v)
	{
		if (m.Col() != m.Row()) throw;
		if (m.Row() != v.Size()) throw;

		Matrix<T> matrix(m.Row(), m.Col() + 1);
		T * ptr1 = m.Buffer();
		T * ptrV = v.Buffer();
		T * ptr2 = matrix.Buffer();
		int x, y;
		for(y = 0; y < m.Row(); ++y, ++ptr2, ++ptrV)
		{
			for(x = 0; x < m.Col(); ++x, ++ptr1, ++ptr2)
			{
				*ptr2 = *ptr1;
			}
			*ptr2 = *ptrV;
		}

		return matrix;
	}

	//��õ�λ����
	template <typename T> Matrix<T> UnitMatrix(int row, int col)
	{
		Matrix<T> m(row, col, 0);
		int sz = Min(row, col);
		int id = 0;
		for(T * ptr = m.Buffer(); id < sz; ++id, ptr += col + 1)
			*ptr = 1;

		return m;
	}

	//��õ�λ����
	template <typename T> Matrix<T> UnitMatrix(int matrixOrder)
	{
		Matrix<T> m(matrixOrder, matrixOrder, 0);
		int id = 0;
		for(T * ptr = m.Buffer(); id < matrixOrder; ++id, ptr += matrixOrder + 1)
			*ptr = 1;

		return m;
	}

	//���Givenת�����󣬸�����ֵ,x, y������������cos(theta) = c, sin(theta) = s��ʹ��
	// [(c, -s)T, (s, c)T]T [x, y]T = [r, 0]T
	// ����Ҫ������x, y��r�����ض��׷���[(c, -s)T, (s, c)T]
	// ���㷨��ͬʱ�����r
	template <typename T> void GivenTransformMatrix(T x, T y, T & s, T & c, T & r)
	{
		if (y == 0) { c = 1; s = 0; r = x; }
		else
		{
			if (Abs<T>(y) > Abs<T>(x))
			{
				T t = -x / y; 
				s = Sqrt(1 + t * t);
				r = -y * s;
				s = 1 / s;
				c = s * t;
			}
			else
			{
				T t = -y / x;
				c = Sqrt(1 + t * t);
				r = x * c;
				c = 1 / c;
				s = c * t;
			}

		}
	}

	//replace n-vectors v1 and v2 by v1 = c * v1 - s * v2, v2 = s * v1 + c * v2
	template <typename T> void GivenTransform(T s, T c, Vector<T> & v1, Vector<T> & v2) 
	{
		Vector<T> t = v1;
		v1 = c * t - s * v2;
		v2 = s * t + c * v2;
	}

	//���Givenת������matrixOrderΪ����Ľף��Խ�ԪΪ1��
	//a_ii = cos(theta),  a_ij = sin(theta)
	//a_ji = -sin(theta), a_jj = cos(theta)
	//��������������Ϊ0
	template <typename T> Matrix<T> GivenTransformMatrix(int matrixOrder, int i, int j, T theta)
	{
		Matrix<T> m = UnitMatrix(matrixOrder);
		m[i][i] = m[j][j] = cos(theta);
		m[j][i] = -(m[i][j] = sin(theta));
		return m;
	}

	//���Givenת�����󣬸þ���Ϊn�׷��󣬶Խ�ԪΪ1��
	//a_ii = cos(theta),  a_ij = sin(theta)
	//a_ji = -sin(theta), a_jj = cos(theta)
	//��������������Ϊ0
	template <typename T> Matrix<T> GivenTransformMatrix(int matrixOrder, int i, int j, T sinTheta, T cosTheta)
	{
		Matrix<T> m = UnitMatrix<T>(matrixOrder);
		m[i][i] = m[j][j] = (T)cosTheta;
		m[j][i] = -(m[i][j] = (T)sinTheta);
		return m;
	}

	//��÷��������ֵ������QR����
	template<typename T> Matrix<T> Matrix<T>::EigenvaluesWithQRMethod(int maxNumOfIterations)
	{
		if (this->col != this->row) throw;

		int n = this->col;
		Matrix<T> & A = *this;	//��ǰ������ΪA
		Matrix<T> B = A.HessenbergUpperMatrix();	//������Ƶ�Hessenber����
		//B = B * B.ConjugateTranspose();

		for(int ix = 0; ix < maxNumOfIterations; ++ ix)
		{
			////cout << "B:\n" << B << endl;

			//������Bת��Ϊ�����Ǿ���
			Matrix Q = UnitMatrix<T>(n);
			Matrix R = B;
			for(int i = 0; i < n - 1; ++i)
			{
				T r1 = R[i][i];
				T r2 = R[i + 1][i];
				T r = (T)Sqrt(r1 * r1 + r2 * r2);
				if (Abs<T>(r) < epsilon) continue;
				Matrix<T> Ri = GivenTransformMatrix(n, i, i + 1, r2 / r, r1 / r);	//���Given�任��
				R = Ri * R;
				R[i + 1][i] = 0;
				Q *= Ri.ConjugateTranspose();
			}
			//////cout << "Q:\n" << Q << endl;
			//////cout << "R:\n" << R << endl;
			//////cout << "Q * R:\n" << Q * R << endl;

			B = R * Q;

			//ǿ����0������������
			int count = 0;
			bool allIsZero = true;
			for(int y = 1; y < n; ++y)
			{
				T * ptr = B.Buffer() + y * n;
				for(int x = 0; x < y; ++x, ++ptr)
					if (Abs<T>(*ptr) < epsilon) *ptr = 0;
					else allIsZero = false;
			}

			if (allIsZero)
			{
				//cout << ix << endl;
				break;
			}

			//ǿ����0������������
			for(int y = 0; y < n; ++y)
			{
				T * ptr = B.Buffer() + y * n;
				for(int x = 0; x < n; ++x, ++ptr)
					if (Abs<T>(*ptr) < epsilon) *ptr = 0;
			}

			////cout << "B:\n" << B << endl;
		}

		return B;
	}

	//���������Ϻ�ɭ������󣬼�����׼��������
	template<typename T> Matrix<T> Matrix<T>::HessenbergUpperMatrix()
	{
		int n = this->col;
		Matrix<T> m(n, n);

		if (this->col != this->row) throw;

		Matrix<T> A = *this;
		Matrix<T> H(n, n, 0);

		for(int i = 0; i < n - 2; ++i)
		{
			//��þ���A��i������n-1-i��Ԫ�أ��������ai
			Vector<T> ai = A.GetCol(i).Cut(i + 1, n - 1 - i); 

			if (Abs<T>(ai.Norm2()) < epsilon) continue;

			//����Householder����ʹ�� H_ * ai = ||ai|| e1������e1 = {1, 0, 0, ... 0}
			Matrix<T> H_ = HouseholderTransformationToUnitVector(ai);

			//ƴ�Ӿ���H									//	H��	|1, #, #, #, ... #|
			H = 0;										//		|0, 1, #, #, ... #|
			for(int j = 0; j <= i; ++j) H[j][j] = 1;	//		|0, 0,			  |
			H.CopyFrom(H_, i + 1, i + 1);				//		|0, 0,     H_     |
			//											//		| ...             |
			//���¾���A									//		|0, 0,            |
			A = H * A * H;

			//�������
			////cout << "-------- i=" << i << " ----------------------------" << endl;
			////cout << H_ << endl;
			////cout << ai.Modulo() << endl;
			////cout << H_ * ai << endl;
			////cout << H << endl;
			////cout << A << endl;
		}

		//ǿ����0������������
		for(int y = 2; y < n; ++y)
		{
			T * ptr = A.Buffer() + y * n;
			for(int x = 0; x < y - 1; ++x, ++ptr)
				*ptr = 0;
		}

		return A;
	}

	//Householder�任�����Householder����
	//v�����б任��������
	//���أ�I - (2 / <v, v>) * v * v*
	template<typename T> Matrix<T> HouseholderTransformation(Vector<T> & v)
	{
		return UnitMatrix(vector.Size()) - (2 / (v * v)) * v * v.ConjugateTranspose();
	}

	//Householder�任�����Householder����H��ʹ��Hx = y
	//x��y�����б任����������||x|| = ||y||
	//���أ�I - (2 / <v, v>) * v * v*
	template<typename T> Matrix<T> HouseholderTransformation(Vector<T> & x, Vector<T> & y)
	{
		Vector<T> v = x - y;
		return UnitMatrix<T>(v.Size()) - ((T)2 / (v * v)) * v * v.ConjugateTranspose();
	}

	//Householder�任�����Householder����H��ʹ��Hx = ||x||e1
	//v = x + sign(x1)||x||e1
	//���أ�I - (2 / <v, v>) * v * v*
	template<typename T> Matrix<T> HouseholderTransformationToUnitVector(Vector<T> & x)
	{
		Vector<T> y(x.Size(), 0);
		y[0] = -sign(x[0]) * x.Norm2();
		Vector<T> v = x - y;
		return UnitMatrix<T>(v.Size()) - ((T)2 / (v * v)) * v * v.ConjugateTranspose();
	}

	//Householder�任�����Householder����H��ʹ��Hx = ||x||e1
	//v = x + sign(x1)||x||e1������delta = |x1|
	//���أ�I - (2 / <v, v>) * v * v*
	template<typename T> Matrix<T> HouseholderTransformationToUnitVector(Vector<T> & x, T & delta)
	{
		Vector<T> y(x.Size(), 0);
		y[0] = delta = -sign(x[0]) * x.Norm2();
		Vector<T> v = x - y;
		return UnitMatrix<T>(v.Size()) - ((T)2 / (v * v)) * v * v.ConjugateTranspose();
	}

	//���һ���������
	template<typename T> Matrix<T> RandomMatrix(int row, int col, T minValue, T maxValue)
	{
		Matrix<T> m(row, col);
		T * ptr;
		enumArray(m.Buffer(), 0, m.Size(), ptr) 
			*ptr = rand(minValue, maxValue);
		enumArrayEnd;

		return m;
	}

	////����һ��Svd�ֽ⣬�����Խ���B�ֽ�Ϊ P* * B * Q ����ʽ�����һ��SVD��������iMin���㵽iMax
	////n��ʾ����B�Ľ�
	////deltaΪ�Խ�Ԫ���飬��0��n-1��gammaΪ�ζԽ�Ԫ���飬��1��n-1
	//template<typename T> void SVDIteration(T * delta, T * gamma, int n, int iMin, int iMax,
	//									   Matrix<T> & P, Matrix<T> & Q)
	//{
	//	T d = ((delta[iMax - 1] * delta[iMax - 1] + gamma[iMax - 1] * gamma[iMax - 1]) - 
	//		(delta[iMax] * delta[iMax] + gamma[iMax] * gamma[iMax])) / 2;
	//	T miu = (delta[iMax] * delta[iMax] + gamma[iMax] * gamma[iMax]) + 
	//		d - sign(d) * Sqrt(d * d + delta[iMax - 1] * delta[iMax - 1] * gamma[iMax] * gamma[iMax]);
	//	T x = delta[iMin] * delta[iMin] - miu;
	//	T y = delta[iMin] * gamma[iMin + 1];
	//	int k = iMin;
	//
	//	Q = UnitMatrix<T>(n);
	//	P = UnitMatrix<T>(n);
	//
	//	for(;; ++k)
	//	{
	//		T s, c, r;
	//		GivenTransformMatrix(x, y, s, c, r);
	//		//����	|x gamma_k+1|	|delta_k gamma_k+1|| c s|
	//		//		|y delta_k+1| = |0       delta_k+1||-s c|
	//		Matrix<T> csMatrix = GivenTransformMatrix(2, 0, 1, s, c);
	//		Matrix<T> tempDeltaGamma(2, 2);
	//		tempDeltaGamma[0][0] = delta[k];
	//		tempDeltaGamma[0][1] = gamma[k + 1];
	//		tempDeltaGamma[1][0] = 0;
	//		tempDeltaGamma[1][1] = delta[k + 1];
	//		Matrix<T> tempDeltaGamma2 = tempDeltaGamma * csMatrix;
	//		x = tempDeltaGamma2[0][0];
	//		y = tempDeltaGamma2[1][0];
	//		gamma[k + 1] = tempDeltaGamma2[0][1];
	//		delta[k + 1] = tempDeltaGamma2[1][1];
	//
	//		//����Q����
	//		Vector<T> qv1 = Q.GetCol(k);
	//		Vector<T> qv2 = Q.GetCol(k + 1);
	//		GivenTransform(s, c, qv1, qv2);
	//		Q.SetCol(k, qv1);
	//		Q.SetCol(k + 1, qv2);
	//
	//		if (k > iMin)
	//			gamma[k] = r;
	//		else
	//		{
	//			T s, c, r;
	//			GivenTransformMatrix(x, y, s, c, r);
	//			delta[k] = r;
	//
	//			//���¾���P
	//			Vector<T> qv1 = P.GetCol(k);
	//			Vector<T> qv2 = P.GetCol(k + 1);
	//			GivenTransform(s, c, qv1, qv2);
	//			P.SetCol(k, qv1);
	//			P.SetCol(k + 1, qv2);
	//		}
	//
	//		if (k < iMax - 1)
	//		{
	//			Matrix<T> csMatrix = GivenTransformMatrix(2, 0, 1, s, c);
	//			Matrix<T> tempDeltaGamma(2, 2);
	//			tempDeltaGamma[0][0] = gamma[k + 1];
	//			tempDeltaGamma[0][1] = 0;
	//			tempDeltaGamma[1][0] = delta[k + 1];
	//			tempDeltaGamma[1][1] = delta[k + 2];
	//			Matrix<T> tempDeltaGamma2 = csMatrix.ConjugateTranspose() * tempDeltaGamma;
	//			x = tempDeltaGamma2[0][0];
	//			y = tempDeltaGamma2[0][1];
	//			delta[k + 1] = tempDeltaGamma2[1][0];
	//			gamma[k + 2] = tempDeltaGamma2[1][1];
	//		}
	//		else
	//		{
	//			Matrix<T> csMatrix = GivenTransformMatrix(2, 0, 1, s, c);
	//			Vector<T> gdTemp(2);
	//			gdTemp[0] = gamma[iMax];
	//			gdTemp[1] = delta[iMax];
	//			Vector<T> vLast = csMatrix.ConjugateTranspose() * gdTemp;
	//			gamma[iMax] = vLast[0];
	//			delta[iMax] = vLast[1];
	//			break;
	//		}
	//	}
	//
	//
	//}

	//template<typename T> void Matrix<T>::SVD_QRMethod(Matrix<T> & U, Matrix<T> & sigma, Matrix<T> & V, int maxNumOfIterations)
	//{
	//	//���Խǻ����
	//	Matrix U1, V1;
	//	Matrix<T> BEx = -Diagonalization2Upper(U1, V1);
	//	//cout << U1 * BEx * V1.ConjugateTranspose() << endl;
	//	int n = Min(BEx.col, BEx.row);
	//
	//	Matrix<T> U2 = UnitMatrix<T>(n);
	//	Matrix<T> V2 = UnitMatrix<T>(n);
	//
	//	Matrix<T> B = BEx.Cut(0, n, 0, n);
	//
	//
	//	//cout << "B:\n" << B << endl;
	//
	//	//ȡ��B�ĶԽ�Ԫ�ʹζԽ�Ԫ������delta��gamma
	//	T * delta = new T[n];
	//	T * gamma = new T[n];
	//	for(int i = 0; i < n; ++i) { delta[i] = B[i][i]; if (i > 0) gamma[i] = B[i - 1][i]; }
	//
	//	//��ʼ����SVD����
	//	int iMin = 0, iMax = 0;
	//	for(;;)
	//	{
	//STEP_i:
	//		//��������֤��
	//		bool allGammaIsZero = true;
	//		for(int i = 1; i < n; ++i) 
	//			if (Abs<T>(gamma[i]) < epsilon) 
	//				gamma[i] = 0; 
	//			else 
	//				allGammaIsZero = false;
	//		if (allGammaIsZero) break;
	//
	//		//Ѱ�Ҵ�iMin��iMax��ʹ��gamma[i]��0
	//		iMin = 0;
	//		iMax = 0;
	//		for(int i = 1; i < n; ++i) { if (gamma[i] != 0) { iMin = i - 1; break; } }
	//		for(int i = iMin + 1; i < n; ++i) { if (gamma[i] != 0) iMax = i; else break; }
	//
	//		//��iMin��iMax - 1�����delat[i]�Ƿ�Ϊ0
	//STEP_iii:
	//		for(int i = iMin; i < iMax; ++i)
	//		{
	//			if (Abs<T>(delta[i]) < epsilon) 
	//			{
	//				delta[i] = 0;
	//				T x = gamma[i + 1];
	//				T y = delta[i + 1];
	//				gamma[i + 1] = 0;
	//				int l = 1;
	//STEP_iv:
	//				T s, c, r;
	//				GivenTransformMatrix(x, y, s, c, r);
	//				delta[i + 1] = r;
	//				U2 = U2 * GivenTransformMatrix(n, i, i + l, s, c).Transpose();
	//
	//STREP_v:
	//				if (l < iMax - i)
	//				{
	//					x = s * gamma[i + l + 1];
	//					gamma[i + l + 1] = c * gamma[i + l + 1];
	//					y = delta[i + l + 1];
	//					l = l + 1;
	//					goto STEP_iv;
	//				}
	//				else goto STEP_i;
	//			}
	//		}
	//
	//
	//		Matrix<T> P, Q;
	//		for(int i = 0; i < n; ++i) { delta[i] = B[i][i]; if (i > 0) gamma[i] = B[i - 1][i]; }
	//		//cout << "B:\n" << B << endl;
	//		SVDIteration(delta, gamma, n, iMin, iMax, P, Q);
	//		//cout << "P:\n" << P << endl;
	//		//cout << "Q:\n" << Q << endl;
	//		U2 = U2 * P;
	//		V2 = V2 * Q;
	//		for(int i = 0; i < n; ++i) {B[i][i] =  delta[i]; if (i > 0) B[i - 1][i] = gamma[i]; }
	//		//cout << "PBQ*:\n" << (P.ConjugateTranspose() * B * Q).SetSmallValueToZero() << endl;
	//		//cout << "B:\n" << B << endl;
	//		system("pause");
	//	}
	//
	//	Matrix<T> BT(n, n, 0);
	//	for(int i = 0; i < n; ++i) {B[i][i] =  delta[i]; BT[i][i] = 1 / delta[i]; if (i > 0) B[i - 1][i] = gamma[i]; }
	//	//V2 = BT * U2.ConjugateTranspose() * B;
	//
	//	
	//	
	//	//BB = U2 B V2
	//	//U2 * BB = B V2
	//	//
	//
	//	//cout << "B:\n" << B << endl;
	//	//cout << "U2 B V2:\n" << (U2.ConjugateTranspose() * B * V2).SetSmallValueToZero() << endl;
	//	//cout << "U2 B V2:\n" << U2.ConjugateTranspose().SetSmallValueToZero() * U2 << endl;
	//	//cout << "U2 B V2:\n" << V2.ConjugateTranspose().SetSmallValueToZero() * V2 << endl;
	//
	//	delete delta;
	//	delete gamma;
	//}

	//���ض��Խǻ����󣬲����U��V ʹ�� U* A V = B
	template<typename T> Matrix<T> Matrix<T>::Diagonalization2Upper(Matrix<T> & U, Matrix<T> & V)
	{
		//���ȣ���������Խǻ��������U��V
		Matrix<T> A = *this;

		//cout << "A:\n" << A << endl;

		int m = A.Row();
		int n = A.Col();

		U = UnitMatrix<T>(m);
		V = UnitMatrix<T>(n);

		int sz = Min(m, n);

		for(int i = 0; i < sz; ++i)
		{
			////cout << "------------" << i << "------------" << endl;
			//����0���·���0
			Vector<T> v1 = A.GetCol(i).Cut(i);
			Matrix<T> A1 = A.Cut(i, m - i, i + 1, n - i - 1);

			T delta1 = v1.Norm2();
			Matrix<T> P = UnitMatrix<T>(m);
			if (Abs<T>(delta1) >= epsilon)
			{
				Matrix<T> p1 = HouseholderTransformationToUnitVector(v1, A[i][i]);
				P.CopyFrom(p1, i, i);
				A1 = p1 * A1;
				//A[i][i] = y[0];
				for(int k = i + 1; k < m; ++k) A[k][i] = 0;
				A.CopyFrom(A1, i, i + 1);
				A.SetSmallValueToZero(epsilon);
			}
			else
			{
				for(int k = i; k < m; ++k) A[k][i] = 0;
			}

			U = U * P;

			//���ڵ�0�н��б任
			if (i < n - 2)
			{
				Matrix<T> H = UnitMatrix<T>(n);
				VectorH<T> u1 = A1.GetRow(0);
				Matrix<T> A1_ = A1.Cut(1, A1.Row() - 1, 0, A1.Col());
				T gamma2 = u1.Norm2();
				if (Abs<T>(gamma2) >= epsilon)
				{
					Matrix<T> H1 = HouseholderTransformationToUnitVector(u1.Transpose());
					H.CopyFrom(H1, i + 1, i + 1);
					Matrix<T> A1H1 = A1 * H1;
					A1H1.SetSmallValueToZero(epsilon);
					//	//cout << "A1 * H1\n" << A1H1;
					A.CopyFrom(A1H1, i, i + 1);
					A.SetSmallValueToZero(epsilon);
				}
				else
					for(int k = i + 1; k < n; ++k) A[i][k] = 0;

				V = V * H;
			}
		}

		return A;

	}

	template<typename T> void Matrix<T>::SVD(Matrix<T> & U, Matrix<T> & sigma, Matrix<T> & V)	//�Ծ������SVD�ֽ⣬����QR����
	{
		xmv::SVD<T> svd;
		svd.eps = epsilon;
		svd.dec(*this);
		U = svd.getU();
		sigma = svd.getSM();
		V = svd.getV();
	}

	template<typename T> void Matrix<T>::SVD(Matrix<T> & U, Vector<T> & sigma, Matrix<T> & V) //�Ծ������SVD�ֽ⣬����QR����
	{
		xmv::SVD<T> svd;
		svd.eps = epsilon;
		svd.dec(*this);
		U = svd.getU();
		sigma = svd.getSV();
		V = svd.getV();
	}

	template<typename T> Matrix<T> Matrix<T>::Cut(int rowStart, int rowCount, int colStart, int colCount)
	{
		Matrix<T> m(rowCount, colCount);
		T * ptrD = m.Buffer();
		T * ptrS = this->dataPtr + col * rowStart + colStart;
		int copyLen = colCount * sizeof(T);
		for(int i = 0; i < rowCount; ++i, ptrD += colCount, ptrS += this->col)
			memcpy(ptrD, ptrS, copyLen);

		return m;
	}	 //��þ����һ����

	template<typename T> Matrix<T> Matrix<T>::Cut(int rowStart, int colStart)
	{
		int rowCount = this->row - rowStart;
		int colCount = this->col - colStart;
		Matrix<T> m(rowCount, colCount);
		T * ptrD = m.Buffer();
		T * ptrS = this->dataPtr + col * rowStart + colStart;
		int copyLen = colCount * sizeof(T);
		for(int i = 0; i < rowCount; ++i, ptrD += colCount, ptrS += this->col)
			memcpy(ptrD, ptrS, copyLen);

		return m;
	}	 //��þ����һ����

	template<typename T> void Matrix<T>::SetCol(int colIndex, T value)
	{
		T * dataPtr = this->dataPtr + colIndex;
		for(int rowIndex = 0; rowIndex < row; ++rowIndex, dataPtr += col)
			*dataPtr = value;
	}

	template<typename T> void Matrix<T>::SetCol(int colIndex, Vector<T> & colVector)
	{
		T * vectorPtr = colVector.Buffer();
		T * vectorPtrEnd = vectorPtr + colVector.Size();
		T * dataPtr = this->dataPtr + colIndex;
		for(; vectorPtr < vectorPtrEnd; ++vectorPtr, dataPtr += col)
			*dataPtr = *vectorPtr;
	}

	template<typename T> template<typename T2> void Matrix<T>::SetCol(int colIndex, Vector<T2> & colVector)
	{
		T * vectorPtr = colVector.Buffer();
		T * vectorPtrEnd = vectorPtr + colVector.Size();
		T * dataPtr = this->dataPtr + colIndex;
		for(; vectorPtr < vectorPtrEnd; ++vectorPtr, dataPtr += col)
			*dataPtr= (T)*vectorPtr ;
	}

	template<typename T> void Matrix<T>::SetRow(int rowIndex, T value)
	{
		T * dataPtr = this->dataPtr + this->col * rowIndex;
		for(int colIndex = 0; colIndex < col; ++colIndex, ++dataPtr)
			*dataPtr = value;
	}

	template<typename T> template<typename T2> void Matrix<T>::SetRow(int rowIndex, VectorH<T2> & rowVector)
	{
		T * vectorPtr = rowVector.Buffer();
		T * vectorPtrEnd = vectorPtr + colVector.Size();
		T * dataPtr = this->dataPtr + this->col * rowIndex;
		for(; vectorPtr < vectorPtrEnd; ++dataPtr, ++vectorPtr)
			*dataPtr = (T)*vectorPtr;
	}

	template<typename T> void Matrix<T>::SetRow(int rowIndex, VectorH<T> & rowVector)
	{
		T * vectorPtr = rowVector.Buffer();
		T * dataPtr = this->dataPtr + this->col * rowIndex;
		memcpy(dataPtr, vectorPtr, rowVector.MemorySize());
	}

	template<typename T> Matrix<T> Matrix<T>::SelectColumns(Vector<int> & indexVector)
	{
		int xSize = indexVector.Size();
		int ySize = row;
		Matrix<T> m(ySize, xSize, 0);
		T * mPtr;

		for(int y = 0; y < ySize; ++y)
		{
			T * mRow = m[y];
			T * MRow = dataPtr + y * col;
			int * ixPtr = indexVector.Buffer();
			for(int x = 0; x < xSize; ++x, ++ixPtr)
			{
				mRow[x] = MRow[*ixPtr];
			}
		}

		return m;
	}

	template<typename T> void Matrix<T>::CopyFrom(Matrix<T> & source, int dstRowStart, int dstColStart)
	{
		T * ptrS = source.Buffer();
		int colCount = source.Col();
		int rowCount = source.Row();
		T * ptrD = this->dataPtr + col * dstRowStart + dstColStart;
		int copyLen = colCount * sizeof(T);
		for(int i = 0; i < rowCount; ++i, ptrS += colCount, ptrD += this->col)
			memcpy(ptrD, ptrS, copyLen);
	}	//����һ����С�ľ����Ƶ���������

	template<typename T> VectorH<T> Matrix<T>::GetRow(int rowIndex)
	{
		return VectorH<T>(dataPtr + rowIndex * col, col); 
	}

	template<typename T> Vector<T> Matrix<T>::GetCol(int colIndex) 
	{
		Vector<T> v(row);
		T * ptr = v.Buffer();
		T * ptrEnd = ptr + v.Size();
		T * ptr2 = this->dataPtr + colIndex;
		for(; ptr < ptrEnd; ++ptr, ptr2 += col)
			*ptr = *ptr2;

		return v;
	}

	//Row[row] *= factor
	template<typename T> inline void Matrix<T>::TransformRowMul(int row, T factor)
	{
		T * ptr = dataPtr + row * col; 
		T* ptrEnd = ptr + col; 
		for(; ptr < ptrEnd; ++ptr) 
			*ptr *= factor; 
	}

	//Row[row] /= factor
	template<typename T> inline void Matrix<T>::TransformRowDiv(int row, T factor)
	{
		T * ptr = dataPtr + row * col;
		T * ptrEnd = ptr + col; 
		for(; ptr < ptrEnd; ++ptr) 
			*ptr /= factor; 
	}

	//Row[row1] += Row[row2]
	template<typename T> inline void Matrix<T>::TransformRowSum(int row1, int row2)
	{
		T * p1 = dataPtr + row1 * col;
		T * p2 = dataPtr + row2 * col;
		T * pe = p1 + col;
		for(; p1 < pe; ++p1, ++p2)
			*p1 += *p2; 
	}

	//Row[row1] -= Row[row2]
	template<typename T> inline void Matrix<T>::TransformRowSub(int row1, int row2)
	{
		T * p1 = dataPtr + row1 * col; 
		T * p2 = dataPtr + row2 * col; 
		T * pe = p1 + col;
		for(; p1 < pe; ++p1, ++p2)
			*p1 -= *p2; 
	}

	//Row[row1] = Row[row1] * factor1 + Row[row2] * factor2
	template<typename T> inline void Matrix<T>::TransformRow(int row1, T factor1, int row2, T factor2) 
	{
		T * p1 = dataPtr + row1 * col;
		T * p2 = dataPtr + row2 * col;
		T * pe = p1 + col;
		for(; p1 < pe; ++p1, ++p2)
			*p1 = *p1 * factor1 + *p2 * factor2;
	}

	//Row[row1] <==> Row[row2]
	template<typename T> inline void Matrix<T>::TransformRowSwap(int row1, int row2)
	{
		T t;
		T * p1 = dataPtr + row1 * col;
		T * p2 = dataPtr + row2 * col;
		T * pe = p1 + col; for(; p1 < pe; ++p1, ++p2)
		{
			t = *p1; 
			*p1 = *p2;
			*p2 = t;
		} 
	}

	//Col[col1] <==> Col[col2]
	template<typename T> inline void Matrix<T>::TransformColSwap(int col1, int col2)
	{ 
		T t; 
		T * p1 = dataPtr + col1; 
		T * p2 = dataPtr + col2;
		T * pe = this->dataEndPtr;
		for(; p1 < pe; p1 += col, p2 += col)
		{
			t = *p1;
			*p1 = *p2;
			*p2 = t;
		}
	}

	//��˹��ȥ�������һ���Խ���
	template<typename T> void Matrix<T>::GaussianElimination(bool eliminateUpperHalf)
	{
		Matrix<T> & am = *this;
		T * rowPtr = am.Buffer();
		T * ptr;
		int row = am.Row();
		int col = am.Col();

		int r = min(row, col);

		int x, y;

		for(x = 0; x < r; ++x, rowPtr += col + 1)
		{
			//Ѱ�ҵ�һ����0�У����������
			ptr = rowPtr;
			for (y = x; y < row && Abs<T>(*ptr) < epsilon; ++y, ptr += col); 
			if (y >= row) 
				continue;
			if (y != x)
				am.TransformRowSum(x, y);

			////cout << am << endl;

			//���й�һ��
			ptr = rowPtr;
			for(y = x; y < row; ++y, ptr += col)
			{
				if (Abs<T>(*ptr) > epsilon)
					am.TransformRowDiv(y, *ptr);
			}

			////cout << am << endl;

			//�����·��趨Ϊ0
			ptr = rowPtr + col;
			for(y = x + 1; y < row; ++y, ptr += col)
			{
				if (Abs<T>(*ptr) > epsilon)
					am.TransformRowSub(y, x);
			}

			////cout << am << endl;

			//�����Ϸ��趨Ϊ0
			if (eliminateUpperHalf && Abs<T>(*rowPtr) > epsilon)
			{
				ptr = rowPtr - col;
				for(y = x - 1; y >= 0; --y, ptr -= col)
				{
					am.TransformRow(y, 1, x, -(*ptr));
				}
			}

			////cout << am << endl;
		}
	}

	//��������������ϲ�Ϊһ������
	template<typename T> Matrix<T> MergeMatrixVertical(Matrix<T> & m1, Matrix<T> & m2)
	{
		int c1 = m1.Col();
		int c2 = m2.Col();
		int r1 = m1.Row();
		int r2 = m2.Row();
		int r = r1 + r2;
		int c = c1;

		if (c1 != c2) throw;

		Matrix<T> m(r, c);
		CopyImage(m.Buffer(), c, r, 0, c, 0, r1, m1.Buffer());
		CopyImage(m.Buffer(), c, r, 0, c, r1, r, m2.Buffer());

		return m;
	}

	//�������������ϲ�Ϊһ������
	template<typename T> Matrix<T> MergeMatrixHorizontal(Matrix<T> & m1, Matrix<T> & m2)
	{
		int c1 = m1.Col();
		int c2 = m2.Col();
		int c = c1 + c2;
		int r = m1.Row();

		if (r != m2.Row()) throw;

		Matrix<T> m(r, c);
		CopyImage(m.Buffer(), c, r, 0, c1, 0, r, m1.Buffer());
		CopyImage(m.Buffer(), c, r, c1, c, 0, r, m2.Buffer());

		return m;
	}



	//���ؾ������
	template<typename T> Matrix<T> Matrix<T>::Inverse()
	{
		if (this->row != this->col) throw;
		int r = this->row;
		Matrix<T> mx = MergeMatrixHorizontal(*this, UnitMatrix<T>(r));
		mx.GaussianElimination();
		return mx.Cut(0, r);
	}

	//���ؾ������
	template<typename T> int Matrix<T>::Rank()
	{
		Matrix<T> m = *this;
		m.GaussianElimination(false);

		int r = min(m.col, m.row);
		for(int i = 0; i < r; ++i)
		{
			if (Abs<T>(m[i][i]) < epsilon)
				--r;
		}
		return r;
	}

	//�þ����Ƿ�Ϊ���Ⱦ���
	template<typename T> bool Matrix<T>::IsFullRank()
	{
		return this->Rank() == Min(this->row, this->col);
	}

	//���ؾ���Ĺ�����
	template<typename T> Matrix<T> Matrix<T>::Pseudoinverse()
	{
		Matrix<T> & A = *this;
		int r = A.Rank();

		//����þ���Ϊ���ȷ���
		if (r == A.row && r == A.col)
			return this->Inverse();

		//��������������ȵ�
		if (r == A.row)
		{
			Matrix<T> AH = ~A;
			Matrix<T> AAH = A * AH;
			return AH * AAH.Inverse();
		}

		//����þ����������ȵ�
		if (r == A.col)
		{
			Matrix<T> AH = ~A;
			Matrix<T> AHA = AH * A;
			return AHA.Inverse() * AH;
		}

		//SVD�ֽ�
		Matrix<T> U, S, V;
		A.SVD(U, S, V);
		for(T *ptr = S.dataPtr; ptr < S.dataEndPtr; ptr += S.col + 1)
			if (Abs<T>(*ptr) > epsilon)
				*ptr = 1 / *ptr;
		return V * S * ~U;
	}

	//���ؾ��������߹�����
	template<typename T> Matrix<T> Matrix<T>::operator ! ()
	{
		return this->Pseudoinverse();
	}



	//���ؾ��������ʽ
	template<typename T> T Matrix<T>::Determinant()
	{
		Matrix<T> am = *this;
		T * rowPtr = am.Buffer();
		T * ptr;
		int row = am.Row();
		int col = am.Col();

		if (row != col) throw;

		int x, y;

		T dev = 1;
		for(x = 0; x < row; ++x, rowPtr += col + 1)
		{
			//Ѱ�ҵ�һ����0�У����������
			ptr = rowPtr;
			for (y = x; y < row && Abs<T>(*ptr) < epsilon; ++y, ptr += col) ; 
			if (y >= row) return 0;	//�ҵ���ȫ0�����ʽΪ0
			if (y != x)
				am.TransformRowSum(x, y);

			////cout << am << endl;

			//���й�һ��
			ptr = rowPtr;
			for(y = x; y < row; ++y, ptr += col)
			{
				if (Abs<T>(*ptr) > epsilon)
				{
					dev *= *ptr;
					am.TransformRowDiv(y, *ptr);
				}
			}

			////cout << am << endl;

			//�����·��趨Ϊ0
			ptr = rowPtr + col;
			for(y = x + 1; y < row; ++y, ptr += col)
			{
				if (Abs<T>(*ptr) > epsilon)
					am.TransformRowSub(y, x);
			}


		}

		//cout << am;
		//cout << dev << endl;

		return dev;
	}

	//ʹ��Kernel���о��	
	template<typename T> template<typename T2> Matrix<T> Matrix<T>::Convolution(Matrix<T2> & kernel)	
	{
		Matrix<T> m(this->row, this->col, T(0));
		Matrix<T> r(this->row, this->col, T(0));

		int bx = kernel.Col() / 2;
		int by = kernel.Row() / 2;

		T2 * kPtr;
		T2 lastK = 0;
		T2 k;
		enumImage(kernel.Buffer(), kernel.Col(), kernel.Row(), 0, kernel.Col(), 0, kernel.Row(), kPtr)
		{
			k = *kPtr;
			if (k == 0) continue;
			if (xmv::Abs<T2>(k) < epsilon) continue;
			if (lastK != k)
			{
				if (k == 1)
					memcpy(r.Buffer(), this->Buffer(), this->MemorySize());
				else
				{
					T * rPtr, * thisPtr;
					enumArray2(r.Buffer(), this->Buffer(), 0, r.Size(), rPtr, thisPtr)
						*rPtr = *thisPtr * k;
					enumArrayEnd;
				}
			}

			CopyAndAddImage(m.Buffer(), this->col, this->row, 
				bx - x_____, by - y_____, 
				r.Buffer(), this->col, this->row);

			lastK = k;
		}
		enumImageEnd;

		return m;
	}

	template<typename T>
	T Matrix<T>::MaxPosition(int x1, int x2, int y1, int y2, int & x, int & y)					//������	x1 ~ x2, y1 ~ y2���ҵ����ֵ
	{
		T m = (*this)[y1][x1];
		x = x1;
		y = y1;
		T * ptr;
		enumImage(dataPtr, col, row, x1, x2, y1, y2, ptr)
		{
			if (*ptr > m)
			{
				m = *ptr;
				x = x_____;
				y = y_____;
			}
		}
		enumImageEnd;
		return m;
	}

	template<typename T>
	T Matrix<T>::MinPosition(int x1, int x2, int y1, int y2, int & x, int & y)					//������	x1 ~ x2, y1 ~ y2���ҵ���Сֵ
	{
		T m = (*this)[y1][x1];
		x = x1;
		y = y1;
		T * ptr;
		enumImage(dataPtr, col, row, x1, x2, y1, y2, ptr)
		{
			if (*ptr < m)
			{
				m = *ptr;
				x = x_____;
				y = y_____;
			}	
		}
		enumImageEnd;
		return m;
	}

	//�����ڻ���mask�͵�ǰ�����С��ͬ����x1 ~ x2, y1 ~ y2���ֽ����ڻ�
	template<typename T> template<typename T2> T Matrix<T>::PartialInnerProduct(Matrix<T2> mask, int x1, int x2, int y1, int y2) //�����ڻ�
	{
		T * p1;
		T2 * p2;
		T sum = 0;
		int c = 0;
		enumImage2(dataPtr, mask.Buffer(), col, row, x1, x2, y1, y2, p1, p2)
		{
			if (*p2 > epsilon || *p2 < -epsilon) ++c;
			sum += sign(*p1 * *p2);
		}
		enumImageEnd;

		if (c > 0)
			return -sum / c;
		else
			return 0;
	}

	template<typename T> void Matrix<T>::WriteToTextFile(ofstream & o)
	{
		o << this->row << " " << this->col << endl;
		o << format(10, 12) << *this;
	}

	template<typename T> void Matrix<T>::WriteToTextFile(string filename, bool append = false)
	{
		ofstream o;

		if (append)
			o.open(filename, std::ios_base::ate);
		else
			o.open(filename);

		this->WriteToTextFile(o);

		o.close();
	}

	template<typename T> void Matrix<T>::ReadFromTextFile(string filename)
	{
		ifstream i(filename);
		ReadFromTextFile(i);
		i.close();
	}

	template<typename T> void Matrix<T>::ReadFromTextFile(ifstream & i)
	{
		int r, c;
		i >> r >> c;
		ReSize(r, c);

		i >> *this;
	}

	template <typename T> void Matrix<T>::Not(bool (* notFunction)(int x, int y))
	{
		T * ptr;
		enumImage(dataPtr, col, row, 0, col, 0, row, ptr)
			if (notFunction(x_____, y_____))
				*ptr = ~ *ptr;
		enumImageEnd;
	}

	template<typename T> Matrix<T> Matrix<T>::GaussianSmoothing(int gaussianFilterSize = 5)
	{
		Matrix<double> kernel = CreateGaussianFilterKernel<double>(gaussianFilterSize);
		//cout << kernel;
		return this->Convolution(kernel);
	}

	template<typename T> Matrix<T> Matrix<T>::MedianFilterSmoothing(int windowSize = 5, 
			MedianFilterSmoothingWindowType windowType = MedianFilterSmoothingWindowType::Square,
			MedianFilterDataType dataType = MedianFilterDataType::Double,
			double falseTrueValue = 0.5)
	{
		T * p;
		T * pm;
		T * arr = NEW("Matrix::MedianFilterSmooth") T [windowSize * windowSize];
		T * ptr;

		int cx = windowSize / 2;
		int cy = windowSize / 2;
		Matrix<T> m(row, col);
		Matrix<T> window;

		if (windowType == MedianFilterSmoothingWindowType::Circle)
			window = CreateAverageCircularKernel<T>(windowSize);
		else
			window = CreateAverageSquareKernel<T>(windowSize);

		bool binary = dataType == MedianFilterDataType::Binary;

		enumImage2(dataPtr, m.Buffer(), col, row, 0, col, 0, row, p, pm)
		{
			int x1 = x_____ - cx;
			int y1 = y_____ - cy;
			int count = 0;
			T * windowPtr = window.Buffer();
			int bcount = 0;
			enumImage(dataPtr, col, row, x1, x1 + windowSize, y1, y1 + windowSize, ptr)
			{
				if (Abs(*windowPtr++) <= epsilon) continue;
				if (x_____ < 0) continue;
				if (y_____ < 0) continue;
				if (x_____ >= col) continue;
				if (y_____ >= row) continue;

				if (binary)
					if (*ptr >= falseTrueValue)
						++bcount;
				else
					arr[count] = *ptr;

				++count;
			}
			enumImageEnd;

			if (binary)
				*pm = bcount >= count / 2;
			else
			{
				Quicksort(arr, count);
				*pm = arr[count / 2];
			}
		}
		enumImageEnd;

		DELETEARR arr;

		return m;
	}

	template <typename T> Matrix<T> Max(Matrix<T> & m1, Matrix<T> & m2)
	{
		Matrix<T> m(m1.Row(), m1.Col());
		T * p1, *p2, *p;
		enumArray3(m1.Buffer(), m2.Buffer(), m.Buffer(), 0,  m.Size(), p1, p2, p)
		{
			*p = Max(*p1, *p2);
		}
		enumArrayEnd;

		return m;
	}

	template <typename T> Matrix<T> Abs(Matrix<T> & m)
	{
		Matrix<T> mm(m.Row(), m.Col());
		T * pp, *p;
		enumArray2(mm.Buffer(), m.Buffer(), 0, m.Size(), pp, p)
		{
			*pp = xmv::Abs(*p);
		}
		enumArrayEnd;
		return mm;
	}

	template <typename T> T Matrix<T>::OTSU()
	{
		return Otsu<T>(this->dataPtr, col, row, 0, col, 0, row);
	}

	//Canny��Ե��⣬CannyDirectionPower�ص����������ø�����ı�Եǿ�ȣ�0 ~ 1��
	//edgeThreshold����Ե��ֵ��edgeThreshold2Factor: �ڶ���Ե��ֵϵ����0 ~ 1֮�䣩��gaussianFilterSize����˹�˲������ڴ�С
	template <typename T>
	Matrix<T> Matrix<T>::Canny(double (*CannyDirectionPower) (double direction, int x, int y), 
		T edgeThreshold, double edgeThreshold2Factor, int gaussianFilterSize)
	{
		//��˹ƽ��
		Matrix<T> m = this->GaussianSmoothing(gaussianFilterSize);

		T v1 = edgeThreshold;
		T v2 = edgeThreshold * edgeThreshold2Factor;

		double ky[4] = {1, 1, -1, -1};
		Matrix<double> kmy = Matrix<double>(2, 2, ky, 4);
		Matrix<T> my = Abs(m.Convolution(kmy));

		double kx[4] = {1, -1, 1, -1};
		Matrix<double> kmx = Matrix<double>(2, 2, kx, 4);
		Matrix<T> mx = Abs(m.Convolution(kmx));

		Matrix<double> d(row, col, 0);
		Matrix<T> m_(row, col, 0);

		T * px, *py, *pm_;
		double * pd;
		enumImage4(m_.Buffer(), d.Buffer(), my.Buffer(), mx.Buffer(), this->col, this->row, 
			0, this->col, 0, this->row, pm_, pd, px, py)
		{
			*pm_ = Abs(*px) + Abs(*py);
			*pd = atan2(*py, *px);
			*pm_ *= (CannyDirectionPower(*pd, x_____, y_____) * 2.25);
		}
		enumImageEnd;


		T * pm;
		int * pe_;
		Matrix<int> e_(row, col, 0);
		Matrix<T> e(row, col, 0);

		double pi = 3.141592454;

		enumImage3(m_.Buffer(), e_.Buffer(), d.Buffer(), col, row, 1, col - 1, 1, row - 1, pm, pe_, pd)
		{
			T v = Abs(*pm);
			if (Abs(Abs(*pd) - pi / 2) <= pi / 8 && v >= Abs(pm[-1]) && v > Abs(pm[1])) 
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;
			}
			if (Abs(Abs(*pd)) <= pi / 8 && v >= Abs(pm[-col]) && v > Abs(pm[col]))
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;		
			}
			if (Abs(Abs(*pd) - pi * 0.75) <= pi / 8 && v >= Abs(pm[-1 - col]) && v > Abs(pm[1 + col])) 
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;
			}
			if (Abs(Abs(*pd) - pi * 0.25) <= pi / 8 && v >= Abs(pm[1 - col]) && v > Abs(pm[col - 1]))
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;		
			}
		}
		enumImageEnd;

		//1��ֵ��ɢ
		T * pe;
		bool proliferation = false;
		do
		{
			proliferation = false;

			enumImage2(e.Buffer(), e_.Buffer(), col, row, 1, col - 1, 1, row - 1, pe, pe_)
			{
				if (*pe_ == 1) *pe = 1;
				else if (*pe_ == -1)
				{
					if (pe_[-1] == 1 || pe_[1] == 1 || pe_[-col] == 1 || pe_[col] == 1)
					{
						*pe_ = 1;
						*pe = 1;
						proliferation = true;
					}
				}
			}
			enumImageEnd;

		}while(proliferation);


		return e;
	}

	template <typename T> Matrix<T> Matrix<T>::Canny(T edgeThreshold, 
		double edgeThreshold2Factor, 
		int gaussianFilterSize,
		double powerOfUpHorizontal,
		double powerOfVehical,
		double powerOfTopLeftToBottomRight,
		double powerOfTopRightToBottomLeft
		)
	{
		//��˹ƽ��
		Matrix<T> m = this->GaussianSmoothing(gaussianFilterSize);

		T v1 = edgeThreshold;
		T v2 = edgeThreshold * edgeThreshold2Factor;

		double k1[9] = {1, 1, 1, 0, 0, 0, -1, -1, -1};
		Matrix<double> km1 = Matrix<int>(3, 3, k1, 9) * powerOfUpHorizontal;
		Matrix<T> m1 = Abs(m.Convolution(km1));

		double k2[9] = {1, 0, -1, 1, 0, -1, 1, 0, -1, };
		Matrix<double> km2 = Matrix<int>(3, 3, k2, 9) * powerOfVehical;
		Matrix<T> m2 = Abs(m.Convolution(km2));

		double k3[9] = {1, 1, 0, 1, 0, -1, 0, -1, -1};
		Matrix<double> km3 = Matrix<int>(3, 3, k3, 9) * powerOfTopLeftToBottomRight;
		Matrix<T> m3 = Abs(m.Convolution(km3));

		double k4[9] = {0, 1, 1, -1, 0, 1, -1, -1, 0};
		Matrix<double> km4 = Matrix<int>(3, 3, k4, 9) * powerOfTopRightToBottomLeft;
		Matrix<T> m4 = Abs(m.Convolution(km4));

		Matrix<int> d(row, col, 0);


		Matrix<T> m_(row, col, 0);

		T * p1, *p2, *p3, *p4, *pm_;
		int * pd;
		enumArray6(m_.Buffer(), d.Buffer(), m1.Buffer(), m2.Buffer(), m3.Buffer(), m4.Buffer(), 
			0, size, pm_, pd, p1, p2, p3, p4)
		{
			if (*p1 >= *p2 && *p1 >= *p3 && *p1 >= *p4)
			{
				*pd = 0;
				*pm_ = *p1;
			}
			else if (*p2 >= *p1 && *p2 >= *p3 && *p2 >= *p4)
			{
				*pd = 1;
				*pm_ = *p2;
			}
			else if (*p3 >= *p1 && *p3 >= *p2 && *p3 >= *p4)
			{
				*pd = 2;
				*pm_ = *p3;
			}
			else if (*p4 >= *p1 && *p4 >= *p2 && *p4 >= *p3)
			{
				*pd = 3;
				*pm_ = *p4;
			}
		}
		enumArrayEnd;


		T * pm;
		int * pe_;
		Matrix<int> e_(row, col, 0);
		Matrix<T> e(row, col, 0);

		enumImage3(m_.Buffer(), e_.Buffer(), d.Buffer(), col, row, 1, col - 1, 1, row - 1, pm, pe_, pd)
		{
			T v = Abs(*pm);
			if (*pd == 1 && v >= Abs(pm[-1]) && v > Abs(pm[1])) 
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;
			}
			if (*pd == 0 && v >= Abs(pm[-col]) && v > Abs(pm[col]))
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;		
			}
			if (*pd == 2 && v >= Abs(pm[-1 - col]) && v > Abs(pm[1 + col])) 
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;
			}
			if (*pd == 3 && v >= Abs(pm[1 - col]) && v > Abs(pm[col - 1]))
			{
				if (v >= v1) *pe_ = 1;
				else if (v >= v2) *pe_ = -1;		
			}
		}
		enumImageEnd;

		//1��ֵ��ɢ
		T * pe;
		bool proliferation = false;
		do
		{
			proliferation = false;

			enumImage2(e.Buffer(), e_.Buffer(), col, row, 1, col - 1, 1, row - 1, pe, pe_)
			{
				if (*pe_ == 1) *pe = 1;
				else if (*pe_ == -1)
				{
					if (pe_[-1] == 1 || pe_[1] == 1 || pe_[-col] == 1 || pe_[col] == 1)
					{
						*pe_ = 1;
						*pe = 1;
						proliferation = true;
					}
				}
			}
			enumImageEnd;

		}while(proliferation);


		return e;
	}

	template<typename T>
	Matrix<T> Shrink(Matrix<T> & bmp)
	{
		Matrix<T> m(bmp.Row(), bmp.Col());
		Shrink(bmp.Buffer(), m.Buffer(), bmp.Col(), bmp.Row());
		return m;
	}

	template<typename T>
	Matrix<T> Expand(Matrix<T> & bmp)
	{
		Matrix<T> m(bmp.Row(), bmp.Col());
		Expand(bmp.Buffer(), m.Buffer(), bmp.Col(), bmp.Row());
		return m;
	}

	template<typename T> Matrix<int> MarkRegion(Matrix<T> & bmp, Vector<int> & regionArea)
	{
		Matrix<int> region(bmp.Row(), bmp.Col());
		Vector<int> area(region.Size() + 1);

		int rc = MarkRegion(bmp.Buffer(), region.Buffer(), area.Buffer(), bmp.Col(), bmp.Row());
		regionArea = area.Cut(0, rc);

		return region;
	}

	template<typename T> void DeleteRegion(Matrix<T> & bmp, Matrix<int> & region, int regionIxToDelete)
	{
		DeleteRegion(bmp.Buffer(), region.Buffer(), regionIxToDelete, bmp.Col(), bmp.Row());
	}

	template <typename T> Matrix<T> Matrix<T>::ReduiceSize(int n)
	{
		int c = col / n;
		int r = row / n;

		Matrix<T> m(r, c);
		ReSample_ReduceImageSize(this->dataPtr, col, row, m.dataPtr, n, m.size, c, r);
		return m;
	}
}
