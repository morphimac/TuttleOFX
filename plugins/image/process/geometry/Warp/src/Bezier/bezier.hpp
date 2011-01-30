#ifndef _TUTTLE_PLUGIN_BEZIER_HPP_
#define _TUTTLE_PLUGIN_BEZIER_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace warp {
namespace bezier {

using namespace boost::gil;
using namespace boost::numeric::ublas;

    point2<double> barycentre(const point2<double> a, const point2<double> b, double t);
    void tracerPoint(const point2<double> p);
    //void dessinePointsControle();
    void tracePointRecur(std::vector< point2<double> > tabPts, int taille, double t);
    void dessinePointRecur(std::vector< point2<double> > tabPts, int taille);

}
}
}
}

#endif
