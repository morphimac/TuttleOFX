#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"
#include "Bezier/bezier.hpp"
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

#include <vector>

namespace tuttle {
namespace plugin {
namespace warp {
namespace bezier {

    point2<double> barycentre(std::vector< point2<double> > quatrePoints, double t)
    {
        //std::cout<<"--------------BARYCENTRE--------------"<<std::endl;
        point2<double> p;

        point2<double> a = quatrePoints[0];
        point2<double> b = quatrePoints[1];
        point2<double> c = quatrePoints[2];
        point2<double> d = quatrePoints[3];

        //B(t) = P0(1-t)^3 + 3 P1 t (1-t)^2 + 3 P2 t^2 (1-t) + P3 t^3;
        p.x= ((a.x)*(1-t)*(1-t)*(1-t))+(3*(b.x)*t*(1-t)*(1-t))+(3*(c.x)*t*t*(1-t))+((d.x)*t*t*t);
        p.y= ((a.y)*(1-t)*(1-t)*(1-t))+(3*(b.y)*t*(1-t)*(1-t))+(3*(c.y)*t*t*(1-t))+((d.y)*t*t*t);

        return p;
    }

    void tracerPoint(const point2<double> p)
    {
            //std::cout<<"--------------TracePoints--------------"<<std::endl;
            glVertex2f(p.x,p.y);
    }

    void dessinePoint(std::vector< point2<double> > tabPts , int nbPointsTraces)
    {
        //std::cout<<"--------------DessineRecur--------------"<<std::endl;
        for(int i = 0; i < 100 ; ++i)
        {
            double t = (100.0-i)/100.0;

            point2<double> tab = barycentre(tabPts,t);

            glColor3ub(0,255,255);
            glBegin(GL_POINTS);
                tracerPoint(tab);
            glEnd();
        }
    }
}
}
}
}


