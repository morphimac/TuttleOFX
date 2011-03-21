#ifndef _TUTTLE_PLUGIN_PARAMTANGENT_HPP_
#define _TUTTLE_PLUGIN_PARAMTANGENT_HPP_

#include "Frame.hpp"
#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "interact.hpp"
#include "ParamPoint.hpp"
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/coordinateSystem.hpp>


#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordinateSystem coord>
class ParamTangent : public ParamPoint<TFrame, coord>
{
public:
    typedef ParamPoint<TFrame, coord> PPoint;
    typedef PPoint Parent;

        ParamTangent( const InteractInfos& infos,
                      OFX::Double2DParam* pCenter,
                      OFX::Double2DParam* pTanA,
                      OFX::Double2DParam* pTanB,
                      const TFrame& frame )
                : Parent( infos, pCenter, frame )
                , _paramTanA( infos, pTanA, frame )
                , _paramTanB( infos, pTanB, frame )
                , _selectType(eSelectTypeNone)
	{}

        virtual ~ParamTangent() {}

protected:
        PPoint _paramTanA;
        PPoint _paramTanB;
        enum ESelectType
        {
                eSelectTypeNone,
                eSelectTypeTanA,
                eSelectTypeTanB,
                eSelectTypeCenter
        };
        ESelectType _selectType;


public:
        bool draw( const OFX::DrawArgs& args ) const;

        MotionType intersect( const OFX::PenArgs& args );

	void setPoint( const Scalar x, const Scalar y )
        {
                Point2 p = this->getPoint();
                Point2 pA = _paramTanA.getPoint();
                Point2 pB = _paramTanB.getPoint();
                //TUTTLE_COUT("Fonction SetPoint");
                //TUTTLE_COUT(_selectType);
                switch( _selectType)
                {
                    case eSelectTypeCenter:
                    {
                        Parent::setPoint(x,y);
                        // _paramTanA.setPoint(bla)
                        // _paramTanB.setPoint(bla)
                        break;
                    }
                    case eSelectTypeTanA:
                    {
                        Point2 p = this->getPoint();
                        _paramTanA.setPoint(x,y);// recuperer la distance entre le point et le point de tangente
                        // symétrique par rapport au centre
                        // B.setPoint(xs,ys);
                        break;
                    }
                    case eSelectTypeTanB:
                    {
                        Point2 p = this->getPoint();
                        _paramTanB.setPoint(x,y);// recuperer la distance entre le point et le point de tangente
                        // symétrique par rapport au centre
                        // A.setPoint(xs,ys);
                        break;
                    }
                    case eSelectTypeNone:
                        break;
                }

	}

};

template<class TFrame, ECoordinateSystem coord>
bool ParamTangent<TFrame, coord>::draw( const OFX::DrawArgs& args ) const
{
        Parent::draw( args );
        _paramTanA.draw( args );
        _paramTanB.draw( args );

        Point2 pA = _paramTanA.getPoint();
        Point2 pB = _paramTanB.getPoint();

        glBegin(GL_LINE);
            glVertex2f(pA.x,pA.y);
            glVertex2f(pB.x,pB.y);
        glEnd();

        return true;
}

template<class TFrame, ECoordinateSystem coord>
MotionType ParamTangent<TFrame, coord>::intersect( const OFX::PenArgs& args )
{
        MotionType m;
        m._mode = eMotionTranslate;
        m._axis = eAxisXY;

        if( _paramTanA.intersect(args)._axis == eAxisXY )
        {
                _selectType = eSelectTypeTanA;
                return m;
        }
        if( _paramTanB.intersect(args)._axis == eAxisXY )
        {
                _selectType = eSelectTypeTanB;
                return m;
        }
        if( Parent::intersect(args)._axis == eAxisXY )
        {
                _selectType = eSelectTypeCenter;
                return m;
        }
        m._mode = eMotionNone;
        m._axis = eAxisNone;
        return m;
}

}
}
}

#endif

