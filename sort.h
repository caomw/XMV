
#pragma once

#include <stdlib.h>



namespace xmv
{
	template<typename T>
	void BubbleSort(T * arr, int size)
	{
		T * end = arr + size;
		T * p1 = arr;
		T * p2 = p1 + 1;
		T temp;

		bool exchanged = false;
		do
		{
			p1 = arr;
			p2 = p1 + 1;
			exchanged = false;
			for(; p2 < end; ++p1, ++p2)
			{
				if (*p1 > *p2)
				{
					temp = *p1;
					*p1 = *p2;
					*p2 = temp;
					exchanged = true;
				}
			}
		}
		while(exchanged);
	}

	template<typename T>
	void BubbleSort(T * arr, int size, int compare(T & x, T & y))
	{
		T * end = arr + size;
		T * p1 = arr;
		T * p2 = p1 + 1;
		T temp;

		bool exchanged = false;
		do
		{
			p1 = arr;
			p2 = p1 + 1;
			exchanged = false;
			for(; p2 < end; ++p1, ++p2)
			{
				if (compare(*p1, *p2) > 0)
				{
					temp = *p1;
					*p1 = *p2;
					*p2 = temp;
					exchanged = true;
				}
			}
		}
		while(exchanged);
	}

	template <typename T>
	void Quicksort(T * start, T * end)		//����������
	{
		if (start >= end - 1) return;				//���鳤��С�ڵ���1����������

		T key = *start;						//��ȡ��Ԫ
		T temp;								//��������ʱ��Ҫ����ʱ����
		T * left = start + 1;					//leftָ��ָ�������±�1��Ԫ��
		T * right = end - 1;						//rightָ��ָ������Ľ�β

		for(; ; )								//ѭ������
		{
			for( ; left < end; left++)			//left����Ѱ�Ҵ��ڵ���key������
				if (*left >= key) break;			//�ҵ����ڵ���key�����ݣ�����ѭ��
			for( ; right > start; right--)		//right�����ҵ�С��key������
				if (*right < key) break;			//�ҵ�С��key�����ݣ�����ѭ��

			if (left >= right) 					//���left�ƶ�����right���Ҳ�
				break;							//��ֹѭ�������ֽ�������

			temp = *left;						//���������ֽ��н���
			*left = *right;
			*right = temp;

			left++;				//������left����ƶ�һ����׼����һ�β���
			right--;				//������right��ǰ�ƶ�һ����׼����һ�β���
		}

		temp = * start;							//����Ԫ�����������ݵ��м�
		*start = *right;
		*right = temp;

		Quicksort(start, right);				//����Ԫ�����������һ������
		Quicksort(right + 1, end);				//����Ԫ�Ҳ���������һ������
	}

	template <typename T>
	void Quicksort(T * arr, int size)	//�������أ�����ϰ�ߵķ�������������
	{
		Quicksort(arr, arr + size);
	}

	template <typename T>
	void Quicksort(T * start, T * end, int compare(T & x, T & y))		//����������
	{
		if (start >= end - 1) return;				//���鳤��С�ڵ���1����������

		T key = *start;						//��ȡ��Ԫ
		T temp;								//��������ʱ��Ҫ����ʱ����
		T * left = start + 1;					//leftָ��ָ�������±�1��Ԫ��
		T * right = end - 1;						//rightָ��ָ������Ľ�β

		for(; ; )								//ѭ������
		{
			for( ; left < end; left++)			//left����Ѱ�Ҵ��ڵ���key������
				if (compare(*left, key) > 0) break;			//�ҵ����ڵ���key�����ݣ�����ѭ��
			for( ; right > start; right--)		//right�����ҵ�С��key������
				if (compare(*right, key) < 0) break;			//�ҵ�С��key�����ݣ�����ѭ��

			if (left >= right) 					//���left�ƶ�����right���Ҳ�
				break;							//��ֹѭ�������ֽ�������

			temp = *left;						//���������ֽ��н���
			*left = *right;
			*right = temp;

			left++;				//������left����ƶ�һ����׼����һ�β���
			right--;				//������right��ǰ�ƶ�һ����׼����һ�β���
		}

		temp = * start;							//����Ԫ�����������ݵ��м�
		*start = *right;
		*right = temp;

		Quicksort(start, right, compare);				//����Ԫ�����������һ������
		Quicksort(right + 1, end, compare);				//����Ԫ�Ҳ���������һ������
	}

	template <typename T>
	void Quicksort(T * arr, int size, int compare(T & x, T & y))
	{
		Quicksort(arr, arr + size, compare);
	}

	template <typename T>
	int Find(T * arr, int size,  T & value)
	{
		T * p;
		enumArray(T * arr, 0, size, p)
			if (*p == value) return p - arr;
		enumArrayEnd;

		return -1;
	}

	template <typename T>
	int DichotomyFind(T * arr, int start, int end, T & value)
	{
		if (end <= start) return -1;
		if (arr[start] == value) return start;
		if (arr[end - 1] == value) return end;
		if (end - start <= 2) return -1;
		int mid = (start + end - 1) >> 1;
		T & v = arr[mid];
		if (value == v) return mid;
		else if (value < v) return DichotomyFind(arr, start, mid, value);
		else return DichotomyFind(arr, mid + 1, end, value);
	}

	template <typename T>
	int DichotomyFind(T * arr, int size, T & value)
	{
		return DichotomyFind(arr, 0, size, value);
	}
}