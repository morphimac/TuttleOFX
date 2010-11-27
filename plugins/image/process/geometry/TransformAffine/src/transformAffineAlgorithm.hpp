#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_ALGORITHM_HPP_
#define	_TUTTLE_PLUGIN_TRANSFORMAFFINE_ALGORITHM_HPP_

#include "TransformAffinePlugin.hpp"

namespace tuttle {
namespace plugin {
namespace transformAffine {

template <typename F, typename F2>
inline boost::gil::point2<F> transform( const TransformAffineProcessParams<F>& params, const boost::gil::point2<F2>& src )
{
	using namespace boost::numeric::ublas;
	bounded_vector<F,3> pIn;
	pIn[0] = src.x;
	pIn[1] = src.y;
	pIn[2] = 1.0;

	bounded_vector<F,3> pOut = prod( params._matrix, pIn );

	boost::gil::point2<F> res;
	res.x = pOut[0] / pOut[2];
	res.y = pOut[1] / pOut[2];
	
	return res;
}

}
}
}

#endif

