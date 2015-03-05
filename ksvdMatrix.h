#pragma once

#include <list>
#include <cstdlib>
#include "vector.h"
#include "matrix.h"
#include "common.h"
#include "memory.h"



namespace xmv
{
	///KSVD�㷨��
	template <typename T>
	class KSVDMatrix
	{
	private:
		list<Vector<T> *> samples;		//����

		Matrix<T> * dictionaryPtr;		//�ֵ����n x K��nΪ����ά�ȣ�KΪ�뱾�����������Ҫȷ�����뱾��
		Matrix<T> * samplesPtr;			//��������n x N, nΪ����ά�ȣ�NΪ��������
		Matrix<T> * coefficientPtr;		//ϵ������K x N, KΪ�뱾������NΪ��������
		int codeBookCount;				//K���뱾����
		int sampleCount;				//N����������
		int sampleDimension;			//n, ����ά��

	public:
		KSVDMatrix()				//���캯��
			: dictionaryPtr(NULL), samplesPtr(NULL), coefficientPtr(NULL), 
			codeBookCount(0), sampleCount(0), sampleDimension(0)
		{
		}

		//��������
		~KSVDMatrix()					
		{
			//DELETE samples
			ClearSamples();

			if (dictionaryPtr)	DELETE dictionaryPtr;
			if (samplesPtr)		DELETE samplesPtr;
			if (coefficientPtr)	DELETE coefficientPtr;
		}

	public:
		//�����뱾����
		int CodebookCount() { return this->codeBookCount; } 

		//ͨ��һ���������һ������
		void AddSample(Vector<T> & sample)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(Vector<T> & sample)") Vector<T>(sample));
		}

		//ͨ��һ�����������һ��������
		void AddSample(T * sampleArray, int arraySize)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(T * sampleArray, int arraySize)") 
				Vector<T>(sampleArray, arraySize));
		}

		//ͨ��һ���������һ������
		template<typename ST>
		void AddSample(Vector<ST> & sample)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(Vector<ST> & sample)") Vector<T>(sample));
		}

		//ͨ��һ�����������һ��������
		template<typename ST>
		void AddSample(ST * sampleArray, int arraySize)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(ST * sampleArray, int arraySize)") 
				Vector<T>(sampleArray, arraySize));
		}

		//ɾ�����е�����
		void ClearSamples()
		{
			list<Vector<T> *>::iterator i = samples.begin();
			list<Vector<T> *>::iterator end = samples.end();

			for(; i != end; ++i)
				DELETE(*i);

			samples.clear();
		}

	private:
		//������������
		bool CreateSamplesMatrix()
		{
			//ɾ��ԭ������
			if (samplesPtr) DELETE samplesPtr;
			samplesPtr = NULL;

			//ȷ������ά��
			if ((this->sampleCount = samples.size()) <= 0) return false;
			this->sampleDimension = (*samples.begin())->Size(); //ȷ������ά��

			//������������
			this->samplesPtr = NEW("ksvdMatrix::CreateSamplesMatrix: Create Samples Matrix")
				Matrix<T>(this->sampleDimension, this->sampleCount);

			if (!samplesPtr) return false;


			//������ѹ����������
			Matrix<T> & samplesMatrix = *samplesPtr;

			auto i = samples.begin();
			auto end = samples.end(); //list<Vector<T> *>::iterator

			//ö�����е�����
			for(int colIndex = 0; i != end; ++i, ++colIndex)
				samplesMatrix.SetCol(colIndex, **i);

			return true;
		}

		//�����뱾����
		bool CreateDictionaryMatrix(int codeBookCount)
		{
			this->codeBookCount = codeBookCount;				//ȷ���뱾����

			//ɾ��ԭ���뱾
			if (this->dictionaryPtr) DELETE this->dictionaryPtr;
			this->dictionaryPtr = NULL;

			//ȷ����������
			if ((this->sampleCount = samples.size()) <= 0) return false;
			this->sampleDimension = (*samples.begin())->Size();

			//�������뱾
			this->dictionaryPtr = NEW("ksvdMatrix::CreateDictionaryMatrix: Create Dictionary Matrix") Matrix<T>(this->sampleDimension, this->codeBookCount);

			if (dictionaryPtr) return true;
			return false;
		}

		bool CreateCoefficientMatrix(int codeBookCount)
		{
			this->codeBookCount = codeBookCount;				//ȷ���뱾����

			//ɾ��ԭ��ϵ������
			if (this->coefficientPtr) DELETE coefficientPtr;
			this->coefficientPtr = NULL;

			//ȷ����������
			if ((this->sampleCount = samples.size()) <= 0) return false;

			//����ϵ������
			this->coefficientPtr = NEW("ksvdMatrix::CreateCoefficientMatrix: Create Coefficient Matrix") Matrix<T>(this->codeBookCount, this->sampleCount); 

			if (this->coefficientPtr) return true;
			return false;
		}

	private:
		//���
		double Check()
		{
			if (this->coefficientPtr && this->samplesPtr && this->dictionaryPtr)
			{
				Matrix<T> checkMatrix = *this->samplesPtr - *this->dictionaryPtr * *this->coefficientPtr;
				return checkMatrix.SquareOfModulo();
			}
			return -1;
		}

	public:
		//ksvd�㷨�����뱾
		//codeBookCount: ����뱾������
		bool KSVD(int codeBookCount, double eps)
		{
			if (!this->CreateSamplesMatrix()) return false;					 //������������
			if (!this->CreateDictionaryMatrix(codeBookCount)) return false;	 //�����뱾����
			if (!this->CreateCoefficientMatrix(codeBookCount)) return false; //����ϵ������

			for(double e = 0;;)
			{
				//�����µ��뱾


				//�����뱾����
				double checkValue = Check();
				if (Abs<T>(checkValue - e) < eps) break;
				e = checkValue;
			}

			return true;
		}

		//����뱾����
		Vector<T> GetCodebook(int codebookIndex)
		{
			if (! dictionaryPtr) return Vector<T>(1);
			return this->dictionaryPtr->GetCol(codebookIndex);


		}
	};
}