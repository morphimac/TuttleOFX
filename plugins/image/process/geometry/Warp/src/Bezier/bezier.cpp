#include "Bezier/bezier.hpp"

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

    void dessinePoint(std::vector< point2<double> > tabPts, int nbBezier,const double r,const double v,const double b)
    {
        //std::cout<<"--------------DessineRecur--------------"<<std::endl;
        for(int i = 0; i < nbBezier; ++i)
        {
            double t = (double(nbBezier)-i)/double(nbBezier);

            point2<double> tab = barycentre(tabPts,t);

            //_tgtPointsBezier.push_back( tab );

            glPointSize(pointWidth);
            glColor3f(r,v,b);
            glBegin(GL_POINTS);
                tracerPoint(tab);
            glEnd();
        }
    }

    point2<double> rempliTabPoint(std::vector< point2<double> > tabPts, double t)
    {
        //std::cout<<"--------------RempliTabPoint--------------"<<std::endl;
        point2<double> pt = barycentre(tabPts,t);
        return pt;
    }
}
}
}
}


