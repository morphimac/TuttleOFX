#ifndef _TUTTLE_PLUGIN_BEZIER_HPP_
#define _TUTTLE_PLUGIN_BEZIER_HPP_

#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"
#include "WarpOverlayInteract.hpp"

#include <sstream>
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
#include <tuttle/plugin/opengl/gl.h>

#include <vector>

namespace tuttle {
namespace plugin {
namespace warp {
namespace bezier {

using namespace boost::gil;
using namespace boost::numeric::ublas;

    point2<double> barycentre(std::vector< point2<double> > quatrePoints, double t);
    void tracerPoint(const point2<double> p);
    void tracePoint(std::vector< point2<double> > tabPts, int nbPointsTraces, double t);
    void dessinePoint(std::vector< point2<double> > tabPts, int nbBezier,const double r, const double v, const double b);
    point2<double> rempliTabPoint(std::vector< point2<double> > tabPts, double t);


}
}
}
}

#endif
