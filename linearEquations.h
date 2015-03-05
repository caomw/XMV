#pragma once

#include "matrix.h"
#include "vector.h"

namespace xmv
{
	//���������Է���
	template <typename T> Vector<T> SolveLinearEquations(Matrix<T> & m, Vector<T> & v)
	{
		Matrix<T> am = xmv::CreateAugmentedMatrix(m, v);

		am.GaussianElimination(true);

		Vector<T> rv = am.GetCol(am.Col() - 1);
		return rv;
	}

	//����С���˷���ù��걸���Է��̵���ѽ��ƽ�
	template <typename T> Vector<T> SolveApproximateLinearEquations(Matrix<T> & m, Vector<T> & v)
	{
		Matrix<T> mT = m.Transpose();
		return SolveLinearEquations(mT * m, mT * v);
	}



}