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
typedef	matrix<double> Matrix;
typedef matrix_row<Matrix> Matrix_Row;
typedef matrix_column<Matrix> Matrix_Col;

class TPS_Morpher
{
public:

	const std::vector< point2<double> > _pIn;
	const std::vector< point2<double> > _pOut;
	std::vector< point2<double> > _pToBuild;

	Matrix mtx_l, mtx_v, mtx_orig_k;

	TPS_Morpher( const std::vector< point2<double> > pIn, const std::vector< point2<double> > pOut, std::vector< point2<double> > pToBuild);
	void morphTPS(point2<double> pt);	

private:

};

}
}
}

#endif
