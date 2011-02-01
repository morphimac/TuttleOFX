#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
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
	if( r2==0 )
                return 0.0;
        return r2 * log(r2) * 0.217147241;
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
	const std::size_t p = _pIn.size();

	/// @param regularization Amount of "relaxation", 0.0 = exact interpolation
	double regularization = 0.0;

	// Empiric value for avg. distance between points
	const double a = 0.5;

	/*------------ INITIALISATION DES MATRICES -----------*/

	// Remplit k et une partie de l
	for (unsigned i=0; i<p; ++i)
	{
      		const Point2& point_i = _pIn[i];
      		for (unsigned j=0; j<p; ++j)
      		{
        		const Point2& point_j = _pIn[j];
        		Scalar sum = boost::math::pow<2>(point_i.x-point_j.x) + boost::math::pow<2>(point_i.y-point_j.y);
        		mtx_l(i,j) = mtx_orig_k(i,j) = base_func(sum);
      		}
        }

	// Remplit le reste de l
    	for (unsigned i=0; i<p; ++i)
    	{
      		const Point2& point_i = _pIn[i];
      		mtx_l(i, p+0) = 1.0;
      		mtx_l(i, p+1) = point_i.x;
      		mtx_l(i, p+2) = point_i.y;

      		if( i < p )
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
		const Point2& point_i = _pOut[i];
		mtx_v(i,0) = point_i.x;
		mtx_v(i,1) = point_i.y;
	}

	mtx_v(p+0, 0) = mtx_v(p+1, 0) = mtx_v(p+2, 0) = 0.0;
        mtx_v(p+0, 1) = mtx_v(p+1, 1) = mtx_v(p+2, 1) = 0.0;

	// Solve the linear system "inplace"	
	permutation_matrix<Scalar> P(p+3);
        matrix<Scalar> x(p+3, 2);

        lu_factorize(mtx_l, P);
        lu_substitute(mtx_l, P, mtx_v);
}


template<typename SCALAR>
template<typename S2>
typename TPS_Morpher<SCALAR>::Point2 TPS_Morpher<SCALAR>::operator()( const point2<S2>& pt ) const
{
	// Nombre de colonnes de la matrice K
        const unsigned m = mtx_orig_k.size2();

        Scalar x = pt.x, y = pt.y;

        double dx = mtx_v(m+0, 0) + mtx_v(m+1, 0)*x + mtx_v(m+2, 0)*y;
        double dy = mtx_v(m+0, 1) + mtx_v(m+1, 1)*x + mtx_v(m+2, 1)*y;

        //std::cout<<"Point "<<pt.x<<"  "<<pt.y<<std::endl;
        //std::cout<<"DX debut "<<dx<<"  "<<dy<<std::endl;

	std::vector< point2<double> >::const_iterator ite2 = _pOut.begin();
        Const_Matrix_Col cv0(mtx_v, 0), cv1(mtx_v,1);

        typename Const_Matrix_Col::const_iterator cv0_ite(cv0.begin());
        typename Const_Matrix_Col::const_iterator cv1_ite(cv1.begin());

        //std::cout<<"DX fin "<<dx<<"  "<<dy<<std::endl;

        for ( unsigned i=0; i<m; ++i, ++ite2, ++cv0_ite, ++cv1_ite )
	{
		double d = base_func( boost::math::pow<2>(ite2->x - x) + boost::math::pow<2>(ite2->y - y) );
		dx += (*cv0_ite) * d;
		dy += (*cv1_ite) * d;
        }

        //std::cout<<"DX fin "<<dx<<"  "<<dy<<std::endl;

	Point2 res;
        res.x = (pt.x + dx)/2;
        res.y = (pt.y + dy)/2;
        //std::cout<<"res "<<res.x<<"  "<<res.y<<std::endl;

        return res;
}

}
}
}


