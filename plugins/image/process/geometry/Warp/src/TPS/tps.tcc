#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/math/special_functions/pow.hpp>

#include <vector>
#include <sstream>

namespace tuttle {
namespace plugin {
namespace warp {

inline double base_func( const double r2 )
{
	// same as r*r * log(r), but for r^2:
	return ( r2==0 )
	? 0.0 // function limit at 0
	: r2 * log(r2) * 0.217147241; // = 1/(2*log(10))
}

template<typename SCALAR>
TPS_Morpher<SCALAR>::TPS_Morpher( const std::vector< Point2 > pIn, const std::vector< Point2 > pOut )
    : _pIn(pIn)
    , _pOut(pOut)
    , mtx_l(pIn.size()+3, pIn.size()+3)
    , mtx_v(pIn.size()+3, 2)
    , mtx_orig_k(pIn.size(), pIn.size())
{
	// Nombre de points d'entrée
	std::size_t p = _pIn.size();

	/// @param regularization Amount of "relaxation", 0.0 = exact interpolation
	double regularization = 0.0;

	// Empiric value for avg. distance between points
    	//
   	// This variable is normally calculated to make regularization
    	// scale independent, but since our shapes in this application are always
    	// normalized to maxspect [-.5,.5]x[-.5,.5], this approximation is pretty
    	// safe and saves us p*p square roots
    	const double a = 0.5;

	/*------------ INITIALISATION DES MATRICES -----------*/
	
	// Remplit k et une partie de l
	for (unsigned i=0; i<p; ++i)
	{
      		const point2<double>& point_i = _pIn[i];
      		for (unsigned j=0; j<p; ++j)
      		{
        		const point2<double>& point_j = _pIn[j];
        		double sum = boost::math::pow<2>(point_i.x-point_j.x) + boost::math::pow<2>(point_i.y-point_j.y);
        		mtx_l(i,j) = mtx_orig_k(i,j) = base_func(sum);
      		}
    	}
	
	// Remplit le reste de l
    	for (unsigned i=0; i<p; ++i)
    	{
      		const point2<double>& point_i = _pIn[i];
      		mtx_l(i, p+0) = 1.0;
      		mtx_l(i, p+1) = point_i.x;
      		mtx_l(i, p+2) = point_i.y;

      		if (i<p)
      		{
        		// diagonal: reqularization parameters (lambda * a^2)
        		mtx_l(i,i) = mtx_orig_k(i,i) = regularization * (a*a);

        		mtx_l(p+0, i) = 1.0;
       			mtx_l(p+1, i) = point_i.x;
        		mtx_l(p+2, i) = point_i.y;
      		}
    	}
	
	for (unsigned i=p; i<p+3; ++i)
	{
      		for (unsigned j=p; j<p+3; ++j)
		{
        		mtx_l(i,j) = 0.0;
		}
	}

	// Remplit une partie de v
	for (unsigned i=0; i<p; ++i)
	{
		const point2<double>& point_i = _pOut[i];
		mtx_v(i,0) = point_i.x;
	      	mtx_v(i,1) = point_i.y;
	}

	mtx_v(p+0, 0) = mtx_v(p+1, 0) = mtx_v(p+2, 0) = 0.0;
	mtx_v(p+0, 1) = mtx_v(p+1, 1) = mtx_v(p+2, 1) = 0.0;
	
	// Solve the linear system "inplace"	
	permutation_matrix<double> P(p+3);
	matrix<double> x(p+3, 2);

	lu_factorize(mtx_l, P);
	x = mtx_v;
	lu_substitute(mtx_l, P, x);

	std::cout<<"L "<<mtx_l.size1()<<" - "<<mtx_l.size2()<<std::endl;
	std::cout<<"V "<<mtx_v.size1()<<" - "<<mtx_v.size2()<<std::endl;
	std::cout<<"K "<<mtx_orig_k.size1()<<" - "<<mtx_orig_k.size2()<<std::endl;
	
}

template<typename SCALAR>
template<typename S2>
Point2 TPS_Morpher<SCALAR>::operator()( const point2<S2>& pt ) const
{
	// Nombre de colonnes de la matrice K
	//const unsigned m = mtx_orig_k.size2();

	double x = pt.x, y = pt.y;
	//std::cout<<"X -> "<<pt.x<<" et Y -> "<<pt.y<<std::endl;
/*
	std::cout<<"L "<<mtx_l.size1()<<" - "<<mtx_l.size2()<<std::endl;
	std::cout<<"V "<<mtx_v.size1()<<" - "<<mtx_v.size2()<<std::endl;
	std::cout<<"K "<<mtx_orig_k.size1()<<" - "<<mtx_orig_k.size2()<<std::endl;
*/

	//std::cout<<"ALLO 1"<<std::endl;
	double test = 0;
	//std::cout<<"B test "<<test<<std::endl;
	//test = mtx_v(m+0, 0); 
	//std::cout<<"M 2 "<<mtx_orig_k.size2()<<std::endl;
      	//double dx = mtx_v(m+0, 0) + mtx_v(m+1, 0)*x + mtx_v(m+2, 0)*y;
      	//double dy = mtx_v(m+0, 1) + mtx_v(m+1, 1)*x + mtx_v(m+2, 1)*y;
/*	
	std::vector< point2<double> >::const_iterator ite2 = _pOut.begin();
      	Matrix_Col cv0(mtx_v,0), cv1(mtx_v,1);
      	Matrix_Col::const_iterator cv0_ite(cv0.begin()), cv1_ite(cv1.begin());
      	for ( unsigned i=0; i<m; ++i, ++ite2, ++cv0_ite, ++cv1_ite )
      	{
        	double d = base_func( boost::math::pow<2>(ite2->x - x) + boost::math::pow<2>(ite2->y - y) );
        	dx += (*cv0_ite) * d;
        	dy += (*cv1_ite) * d;
      	}*/
		/*std::cout<<"ALLO 2"<<std::endl;
	std::cout<<"Dx-> "<<dx<<"et Dy-> "<<dy<<std::endl;
		std::cout<<"ALLO 3"<<std::endl;*//*		
      	pt.x += dx;
      	pt.y += dy;
	std::cout<<"AFTER x-> "<<pt.x<<"et y-> "<<pt.y<<std::endl;*/
        Point2 shift(30.0, 100.0);
        return pt + shift;
}

}
}
}


