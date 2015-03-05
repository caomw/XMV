#include <list>
#include <cstdlib>
#include "vector.h"
#include "matrix.h"
#pragma once

#include "common.h"
#include "memory.h"



namespace xmv
{
	typedef double real;
	///KSVD�㷨��
	template <typename T>
	class KSVD
	{
	private:
		list<Vector<T> *> samples;		//����

		Matrix<T> * dictionaryPtr;		//�ֵ����n x K��nΪ����ά�ȣ�KΪ�뱾�����������Ҫȷ�����뱾��
		Matrix<T> * samplesPtr;			//��������n x N, nΪ����ά�ȣ�NΪ��������
		Matrix<T> * coefficientPtr;		//ϵ������K x N, KΪ�뱾������NΪ��������
		int codeBookCount;				//K���뱾����
		int sampleCount;				//N����������
		int sampleDimension;			//n, ����ά��

		ClusterAnalyzer<T> ca;
	public:
		Matrix<T> Dictionary()
		{
			return *dictionaryPtr;
		}


	public:
		KSVD()				//���캯��
			: dictionaryPtr(NULL), samplesPtr(NULL), coefficientPtr(NULL),
			codeBookCount(0), sampleCount(0), sampleDimension(0)
		{
		}

		//��������
		~KSVD()
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
			ca.AddSample(sample);
		}

		//ͨ��һ�����������һ��������
		void AddSample(T * sampleArray, int arraySize)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(T * sampleArray, int arraySize)")
				Vector<T>(sampleArray, arraySize));
			ca.AddSample(sampleArray, arraySize);
		}

		//ͨ��һ���������һ������
		template<typename ST>
		void AddSample(Vector<ST> & sample)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(Vector<ST> & sample)") Vector<T>(sample));
			ca.AddSample(sample);
		}

		//ͨ��һ�����������һ��������
		template<typename ST>
		void AddSample(ST * sampleArray, int arraySize)
		{
			this->samples.push_back(NEW("ClusterArrayer.AddSample(ST * sampleArray, int arraySize)")
				Vector<T>(sampleArray, arraySize));
			ca.AddSample(sampleArray, arraySize);
		}

		//ɾ�����е�����
		void ClearSamples()
		{
			list<Vector<T> *>::iterator i = samples.begin();
			list<Vector<T> *>::iterator end = samples.end();

			for (; i != end; ++i)
				DELETE(*i);

			samples.clear();
			ca.ClearSamples();
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
			for (int colIndex = 0; i != end; ++i, ++colIndex)
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
			this->dictionaryPtr = NEW("ksvdMatrix::CreateDictionaryMatrix: Create Dictionary Matrix")
				Matrix<T>(this->sampleDimension, this->codeBookCount);

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
			this->coefficientPtr = NEW("ksvdMatrix::CreateCoefficientMatrix: Create Coefficient Matrix")
				Matrix<T>(this->codeBookCount, this->sampleCount);

			if (this->coefficientPtr) return true;
			return false;
		}

	public:
		//ksvd�㷨�����뱾
		//codeBookCount: ����뱾������
		bool Run(int codeBookCount, int Sparsity, double eps, double Lemda,
			string saveFolderPath, int imgWidth, int imgHeight)
		{
			int beginI = 0;
			cout << "EPS: " << eps << endl;
			if (!this->CreateSamplesMatrix()) return false;					 //������������
			if (!this->CreateDictionaryMatrix(codeBookCount)) return false;	 //�����뱾����
			if (!this->CreateCoefficientMatrix(codeBookCount)) return false; //����ϵ������

			int n = this->sampleDimension;
			int N = this->sampleCount;
			int K = this->codeBookCount;
			cout << "Sample Count: " << sampleCount << endl;

			Matrix<T> & D = *this->dictionaryPtr;
			Matrix<T> & Y = *this->samplesPtr;
			Matrix<T> & X = *this->coefficientPtr;

			//��D��ʼ��Ϊ������������
			ifstream in(saveFolderPath + "\\Dictionary.txt");
			if (in.bad() || in.fail())
			{
				if (false)
				{
					D = RandomMatrix<T>(n, K, 0, 1);
					for (int i = 0; i < K; ++i)
					{
						Vector<T> v = D.GetCol(i);
						v.Normalization();
						D.SetCol(i, v);
					}
				}
				//D = RandomMatrix<T>(n, K, 0, 1);
				else
				{

					cout << "KMeas..." << endl;
					ca.kMeans(codeBookCount, nullptr, eps / 1E8, saveFolderPath, imgWidth, imgHeight);
					cout << "init Dictionary" << endl;
					for (int i = 0; i < K; ++i)
					{
						Vector<T> v = ca.Centers()[i];
						v.Normalization();
						D.SetCol(i, v);
					}
				}
			}
			else
			{
				D.ReadFromTextFile(in);
				in.close();
			}

			SaveDictionary(saveFolderPath, imgWidth, imgHeight);

			//D = RandomMatrix<T>(n, K, -1, 1);
			////cout << D;

			//�����еĳ�ʼ�뱾��һ��
			for (int i = 0; i < K; ++i)
			{
				Vector<T> column = D.GetCol(i);
				D.SetCol(i, column / column.Norm2());
			}
			//cout << "Dictionary: \n" << D << endl;
			Matrix<T> minD = D;
			double e = 0;
			int minId = 0;
			double minE = 0;
			eps = eps * D.AbsMax();
			for (int id = 0;; ++id)
			{

				//�����µ��뱾������ϵ������X
				//cout << "OMP..." << endl;
				OMP<T>(D, Y, Sparsity, X, eps);
				////cout << "D\n" << D << endl;
				////cout << "Y\n" << Y << endl;
				////cout << "X\n" << X << endl;

				//������
				Matrix<T> E = Y - D * X;
				////cout << "Err\n" << E << endl;

				double v1 = E.Norm2() / E.Col();
				double v2 = X.Norm0() * 1.0 / sampleCount;
				double checkValue = v1 + v2 * Lemda;
				cout << "EValue " << checkValue << " = " <<
					v1 << " + " << v2 << " * " << Lemda;
				if (id == 0)
				{
					minD = D;
					minE = checkValue;
					minId = 0;
					e = checkValue;
					cout << " min " << endl;
				}
				else
				{
					if (checkValue < minE)
					{
						minD = D;
						minId = id;
						minE = checkValue;
						cout << " min " << endl;
						if (checkValue < e && Abs<T>(e - checkValue) < eps / 1E2) break;
					}
					else cout << endl;

					e = checkValue;
				}

				//�����ֵ�
				//int beginI = rand(K);
				//int IEnd = 1 + beginI;
				//for (int ii = beginI; ii < IEnd; ++ii)
				for (int i = 0; i < K; ++i)
				{
					//beginI++;
					//beginI = beginI % K;
					//int i = ii % K;

					//����ֵ�ĵ�i���еķ�0Ԫ�ĸ������Լ�����
					Vector<T> xti = X.GetRow(i).Transpose();
					int wc = xti.Norm0(); //��0Ԫ����
					bool empty = false;
					if (wc <= 0)
					{
						wc = 1;
						empty = true;
					}

					Vector<int> w(wc);
					int jj = 0;
					Matrix<real> Yj(n, wc);
					Matrix<real> Xj(K, wc);
					for (int j = 0; j < xti.Size(); ++j)
					{
						if (Abs(xti[j]) > epsilon || empty)
						{
							Yj.SetCol(jj, Y.GetCol(j));
							Xj.SetCol(jj, X.GetCol(j));
							w[jj++] = j;
							if (empty) break;
						}
					}



					D.SetCol(i, 0);				//����ֵ�ĵ�i��
					X.SetRow(i, 0);				//���ϵ���ĵ�i��
					Matrix<T> Ei = Y - D * X;
					Matrix<real> Eij(n, wc);
					for (int jj = 0; jj < wc; ++jj)
					{
						Eij.SetCol(jj, Ei.GetCol(w[jj]));
					}



					////cout << "Ei\n" << Ei << endl;

					//��Ei����SVD�ֽ�
					Matrix<T> U, S, V;
					cout << "SVD ..." << i << '\r';
					Eij.SVD(U, S, V);
					T s0 = S[0][0];
					////cout << "S\n" << S << endl;
					D.SetCol(i, U.GetCol(0));
					Vector<T> nx = V.GetCol(0) * s0;
					VectorH<T> nix(N, 0);
					for (int jj = 0; jj < wc; ++jj)
					{
						nix[w[jj]] = nx[jj];
					}
					X.SetRow(i, nix);


					//SaveDictionary(saveFolderPath, imgWidth, imgHeight, i);

					//OMP<T>(D, Y, Sparsity, X, eps);
				}
				cout << endl;
				//cout << "D\n" << D << endl;

				SaveDictionary(saveFolderPath, imgWidth, imgHeight);
			}

			return true;
		}

		//����뱾����
		Vector<T> GetCodebook(int codebookIndex)
		{
			if (!dictionaryPtr) return Vector<T>(1);
			return this->dictionaryPtr->GetCol(codebookIndex);
		}

	public:
		void SaveDictionary(string codePath, int imgWidth, int imgHeight, int index = -1)
		{
			Matrix<T> & DF = this->Dictionary();
			//�뱾д���ļ�
			if (index < 0)
			{
				string fn = codePath + "\\Dictionary.txt";
				ofstream sw(fn);
				sw << DF.Row() << endl;
				sw << DF.Col() << endl;
				sw << DF;
				sw.close();


				real min = DF.Min();
				real max = DF.Max();

				real k = Min(fabs(min), fabs(max));
				min = -k;
				max = k;

				for (int i = 0; i < this->codeBookCount; ++i)
				{
					Vector<real>  & centerVector = DF.GetCol(i);

					string fn = codePath + "\\" + i + ".bmp";
					xmv::Bitmap<real> bmp(centerVector.Buffer(), imgWidth, imgHeight);

					real min = bmp.Min();
					real max = bmp.Max();
					real k = Max(fabs(min), fabs(max));
					min = -k;
					max = k;

					bmp.Save(fn, min, max);

					//cout << "\rWriting ... " << i << "/" << centerCount;
				}
			}
			else
			{
				Vector<real>  & centerVector = DF.GetCol(index);
				real min = centerVector.Min();
				real max = centerVector.Max();
				real k = Max(fabs(min), fabs(max));
				min = -k;
				max = k;
				string fn = codePath + "\\" + index + ".bmp";
				xmv::Bitmap<real> bmp(centerVector.Buffer(), imgWidth, imgHeight);
				bmp.Save(fn, min, max);
			}
		}
	};
}