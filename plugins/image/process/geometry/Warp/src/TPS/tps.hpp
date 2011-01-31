#ifndef _TUTTLE_PLUGIN_TPS_HPP_
#define _TUTTLE_PLUGIN_TPS_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace warp {

using namespace boost::gil;
using namespace boost::numeric::ublas;

template<typename SCALAR>
class TPS_Morpher
{
public:
	typedef SCALAR Scalar;
	typedef point2<Scalar> Point2;

        typedef	matrix<Scalar> Matrix;
        typedef matrix_row<const Matrix> Const_Matrix_Row;
        typedef matrix_column<const Matrix> Const_Matrix_Col;

public:
	TPS_Morpher( const std::vector< Point2 > pIn, const std::vector< Point2 > pOut );

	template<typename S2>
	Point2 operator()( const point2<S2>& pt ) const;

private:
	const std::vector<Point2> _pIn;
	const std::vector<Point2> _pOut;

        Matrix mtx_l, mtx_v, mtx_orig_k;
};

}
}
}

#include "tps.tcc"

#endif
