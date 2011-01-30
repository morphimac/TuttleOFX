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


    int Nb_controle;
    // declaration pts de controle
    std::vector< point2<double> > tabPts;

    point2<double> barycentre(const point2<double> a, const point2<double> b, double t)
    {
        //std::cout<<"--------------BARYCENTRE--------------"<<std::endl;
        point2<double> p;

        p.x= ( (1-t)*(a.x) ) + t*(b.x);
        p.y= ( (1-t)*(a.y) ) + t*(b.y);

        return p;
    }

    void tracerPoint(const point2<double> p)
    {
            //std::cout<<"--------------TracePoints--------------"<<std::endl;
            glVertex2f(p.x,p.y);
    }
/*
    void dessinePointsControle()
    {
        //unsigned int nbPoints = _plugin->_paramNbPoints->getValue();
        glColor3ub(90,0,0);
        glBegin(GL_LINES);
        unsigned int i = 0;
        //for(; i< Nb_controle;i++)
        for(; i< Nb_controle;i++)
        {
            tracerPoint(tabPts[i]);
            tracerPoint(tabPts[i+1]);
        }
        glEnd();
    }
*/

    void tracePointRecur(std::vector< point2<double> > tabPts, int taille, double t)
    {
        //std::cout<<"--------------TracePointRecur--------------"<<std::endl;
        if(taille == 1)
        {
            glColor3ub(255,0,255);
            glBegin(GL_POINTS);
            tracerPoint(tabPts[0]);
            glEnd();
        }
        else
        {
            std::vector< point2<double> > tab;
            for(int i=0; i<taille-1; i++)
               tab[i] = barycentre(tabPts[i],tabPts[i+1],t);
            tracePointRecur(tab, taille-1,t);
        }
    }

    void dessinePointRecur(std::vector< point2<double> > tabPts, int taille)
    {
        //std::cout<<"--------------DessineRecur--------------"<<std::endl;
        double t;
        for(t=0.00;t<1.00;t+=kPasBezier)
        {
            tracePointRecur(tabPts, Nb_controle, t);
        }
    }
/*
    void display(void){

        glClear(GL_COLOR_BUFFER_BIT);
        if(Nb_controle != 0)
        {
                dessinePointsControle();
                dessinePointRecur(tabPts, Nb_controle);
        }
        glutSwapBuffers();
    }

    void mouse(int key, int state, int x, int y){
      if(key==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
      {
            tabPts = realloc(tabPts, sizeof(point*)*(Nb_controle+1));
            tabPts[Nb_controle] = allocpt(x/2,300-(y/2));
            Nb_controle++;
      }
      display();
    }
*/
}
}
}
}


