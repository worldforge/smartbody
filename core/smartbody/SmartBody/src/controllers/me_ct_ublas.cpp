//#include <boost/numeric/bindings/atlas/cblas.hpp>
#include "controllers/me_ct_ublas.hpp"
#include "sb/SBScene.h"
#include <boost/numeric/bindings/blas/blas.hpp>
#include <boost/numeric/bindings/lapack/lapack.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/traits/ublas_vector.hpp>

#define __SB_LA_NONE 0
#define __SB_LA_UBLAS 1
#define __SB_LA_EIGEN 2

#if defined(EMSCRIPTEN) || defined(__ANDROID__) || defined(SB_IPHONE)
#define __SB_LA __SB_LA_EIGEN
#elif defined(__FLASHPLAYER__)
#define __SB_LA __SB_LA_NONE
#else
#define __SB_LA __SB_LA_UBLAS
#endif

#if __SB_LA == __SB_LA_UBLAS
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/banded.hpp> 
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#endif
#include <boost/numeric/ublas/lu.hpp>


#if __SB_LA == __SB_LA_EIGEN
#include <Eigen/Dense>
#include <Eigen/SVD>
using namespace Eigen;
#endif


#include <time.h>

namespace lapack = boost::numeric::bindings::lapack;
namespace blas   = boost::numeric::bindings::blas;
//namespace ublas  = boost::numeric::ublas;

void MeCtUBLAS::matrixMatMult(const dMatrix& mat1, const dMatrix& mat2, dMatrix& mat3)
{
	if (mat3.size1() != mat1.size1() || mat3.size2() != mat2.size2()){
		mat3.resize(mat1.size1(),mat2.size2());
	}

#if __SB_LA == __SB_LA_UBLAS
	blas::gemm(mat1,mat2,mat3);	
#elif __SB_LA == __SB_LA_EIGEN
	MatrixXd m1(mat1.size1(), mat1.size2()), m2(mat2.size1(),mat2.size2()), m3(mat3.size1(), mat3.size2());
	for(unsigned int i = 0; i < mat1.size1(); ++i)
		for(unsigned int j = 0; j < mat1.size2(); ++j)
			m1(i, j) = mat1(i, j);
	for(unsigned int i = 0; i < mat2.size1(); ++i)
		for(unsigned int j = 0; j < mat2.size2(); ++j)
			m2(i, j) = mat2(i, j);
	m3 = m1 * m2;
	//copy result to mat3
	for(unsigned int i = 0; i < mat3.size1(); ++i)
		for(unsigned int j = 0; j < mat3.size2(); ++j)
			mat3(i, j) = m3(i, j);
#endif
}

void MeCtUBLAS::matrixVecMult(const dMatrix& mat1, const dVector& vin, dVector& vout)
{
	if (vout.size() != mat1.size1())
		vout.resize(mat1.size1());
	if (vin.size() != mat1.size2())
		return;

#if __SB_LA == __SB_LA_UBLAS
	blas::gemv('N',1.0,mat1,vin,0.0,vout);	
#elif __SB_LA == __SB_LA_EIGEN
	MatrixXd m1(mat1.size1(), mat1.size2());
	for(size_t i = 0; i < mat1.size1(); ++i)
		for(size_t j = 0; j < mat1.size2(); ++j)
			m1(i, j) = mat1(i, j);
	VectorXd vIn(vin.size()), vOut;
	for(size_t i = 0; i < vin.size(); ++i)
		vIn(i) = vin(i);
	vOut = m1 * vIn;
	for(size_t i = 0; i < vout.size(); ++i)
		vout(i) = vOut(i);
#endif

}

bool MeCtUBLAS::inverseMatrix( const dMatrix& mat, dMatrix& inv )
{
	using namespace boost::numeric::ublas;
	dMatrix A(mat);
	inv = identity_matrix<double>(mat.size1());
#if !defined(__ANDROID__) && !defined(SB_IPHONE) &&  !defined(__FLASHPLAYER__) && !defined(EMSCRIPTEN)
	lapack::gesv(A,inv);
#elif __SB_LA == __SB_LA_EIGEN
	MatrixXd m(mat.size1(), mat.size2()), mInv;
	for(size_t i = 0; i < mat.size1(); ++i)
		for(size_t j = 0; j < mat.size2(); ++j)
			m(i, j) = mat(i, j);
	mInv = m.inverse();
	for(size_t i = 0; i < inv.size1(); ++i)
		for(size_t j = 0; j < inv.size2(); ++j)
			inv(i, j) = mInv(i, j);
#endif
	return true;
}

bool MeCtUBLAS::linearLeastSquare( const dMatrix& A, const dMatrix& B, dMatrix& sol )
{
	dMatrix AtA, AtB, invAtA;
	MeCtUBLAS::matrixMatMult(ublas::trans(A),A,AtA);
	MeCtUBLAS::matrixMatMult(ublas::trans(A),B,AtB);
	MeCtUBLAS::inverseMatrix(AtA,invAtA);
	MeCtUBLAS::matrixMatMult(invAtA,AtB,sol);
	return true;
}

bool MeCtUBLAS::matrixSVD( const dMatrix& A, dVector& S, dMatrix& U, dMatrix& V )
{
#if __SB_LA == __SB_LA_UBLAS
	dMatrix M(A);
	lapack::gesvd(M,S,U,V);
#elif __SB_LA == __SB_LA_EIGEN
	MatrixXd mA;
	for(size_t i = 0; i < A.size1(); ++i)
		for(size_t j = 0; j < A.size2(); ++j)
			mA(i, j) = A(i, j);
	JacobiSVD<MatrixXd> svd(mA, ComputeThinU | ComputeThinV);
	VectorXd s = svd.singularValues();
	MatrixXd mU = svd.matrixU();
	MatrixXd mV = svd.matrixV();
	for(int i = 0; i < s.size(); ++i)
		S(i) = s(i);

	for(int i = 0; i < mU.innerSize(); ++i)
		for(int j = 0; j < mU.outerSize(); ++j)
			U(i, j) = mU(i, j);
	for(int i = 0; i < mV.innerSize(); ++i)
		for(int j = 0; j < mV.outerSize(); ++j)
			V(i, j) = mV(i, j);
#endif
	return true;
}

double MeCtUBLAS::matrixDeterminant( const dMatrix& mat )
{
	double det = 1.0; 

	dMatrix mLu(mat); 
	ublas::permutation_matrix<std::size_t> pivots(mat.size1() ); 

	int is_singular = lu_factorize(mLu, pivots); 

	if (!is_singular) 
	{ 
		for (std::size_t i=0; i < pivots.size(); ++i) 
		{ 
			if (pivots(i) != i) 
				det *= -1.0; 

			det *= mLu(i,i); 
		} 
	} 
	else 
		det = 0.0; 

	return det;
}
/************************************************************************/
/* MeCtMath Routines                                                    */
/************************************************************************/

float MeCtMath::Random( float r_min, float r_max )
{
	static bool initRand = false;
	if (!initRand)
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		std::string seedStr = scene->getStringAttribute("randomseed");
		if (seedStr == "")
		{
			srand((unsigned int)time(0));
		}
		else
		{
			unsigned int seed = atoi(seedStr.c_str());
			srand(seed);
		}
		
		initRand = true;
	}
	float frand = (float)rand()/(float)RAND_MAX; 
	frand = r_min + frand*(r_max-r_min);
	return frand;
}
