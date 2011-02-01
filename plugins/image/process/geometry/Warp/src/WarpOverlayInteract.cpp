#include "WarpOverlayInteract.hpp"
#include "WarpDefinitions.hpp"
#include "WarpPlugin.hpp"
#include "Bezier/bezier.hpp"
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
#include <tuttle/plugin/interact/ParamPointRelativePoint.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace warp {

WarpOverlayInteract::WarpOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
        _plugin = static_cast<WarpPlugin*>( _effect );

        for( std::size_t i = 0; i < kMaxNbPoints; ++i )
        {
            // main point
            interact::AndActiveFunctor<>* activeOut = new interact::AndActiveFunctor<>();
            activeOut->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
            activeOut->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn[i] ) );
            _interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc ),
                                      activeOut,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayOutColor));

            interact::PointInteract* point = new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointOut[i], _plugin->_clipSrc );

            // tangente A
            interact::AndActiveFunctor<>* activeTgtA = new interact::AndActiveFunctor<>();
            activeTgtA->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgt ) );
            activeTgtA->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgt[i*2] ) );
            _interactScene.push_back(new interact::ParamPointRelativePoint<interact::FrameClip,
                                       eCoordonateSystemXXcn>( _infos, _plugin->_paramPointTgt[i*2], _plugin->_clipSrc, point ),
                                       activeTgtA,
                                       new interact::ColorRGBParam(_plugin->_paramOverlayTgtColor ));

            // tangente B
            interact::AndActiveFunctor<>* activeTgtB = new interact::AndActiveFunctor<>();
            activeTgtB->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayTgt ) );
            activeTgtB->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointTgt[i*2+1] ) );
            _interactScene.push_back(new interact::ParamPointRelativePoint<interact::FrameClip,
                                      eCoordonateSystemXXcn>( _infos, _plugin->_paramPointTgt[i*2+1], _plugin->_clipSrc, point ),
                                      activeTgtB,
                                      new interact::ColorRGBParam(_plugin->_paramOverlayTgtColor ));

        }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        for(unsigned int cptIn = 0; cptIn < kMaxNbPoints; ++cptIn)
	{
                interact::AndActiveFunctor<>* activeIn = new interact::AndActiveFunctor<>();
                activeIn->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
                activeIn->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn[cptIn] ) );
                _interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointIn[cptIn], _plugin->_clipSrc ),
                                                                  activeIn,
                                                                  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
        }
}

bool WarpOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	bool displaySomething        = false;

	const std::size_t nbPoints = _plugin->_paramNbPoints->getValue();

        if(nbPoints >2)
        {
                for( std::size_t i=0 ; i < 2*nbPoints ; i+=2 )
                {
                    glColor3ub(255,60,140);

                    glBegin(GL_POINTS);
                        glVertex2f(tabTgt[i].x,tabTgt[i].y);
                        glVertex2f(tabTgt[i+1].x,tabTgt[i+1].y);
                    glEnd();

                    glBegin(GL_LINES);
                        glVertex2f(tabTgt[i].x,tabTgt[i].y);
                        glVertex2f(tabTgt[i+1].x,tabTgt[i+1].y);
                    glEnd();
                }

                for( std::size_t c = 0 ; c < nbPoints ; ++c )
		{
                        std::vector< point2<double> > tabPts;
                        OfxPointD p1 = _plugin->_paramPointIn[c]->getValue();
                        OfxPointD p2 = _plugin->_paramPointIn[c+1]->getValue();

                        //Recherche du point de la tangent le plus proche du point a relier

                        tabPts.push_back( point2<double>( p1.x, p1.y ) );
                        tabPts.push_back( point2<double>( tabTgt[c*2+1].x, tabTgt[c*2+1].y ) );

                        tabPts.push_back( point2<double>( tabTgt[c*2+2].x, tabTgt[c*2+2].y ) );
                        tabPts.push_back( point2<double>( p2.x, p2.y ) );

			bezier::dessinePoint( tabPts, 4 );
                }
        }

	displaySomething |= _interactScene.draw( args );

        return displaySomething;
}

bool WarpOverlayInteract::penMotion( const OFX::PenArgs& args )
{
        if(_plugin->_paramMethod->getValue() == eParamMethodMove)
        {
            return _interactScene.penMotion( args );
        }
}

bool WarpOverlayInteract::penDown( const OFX::PenArgs& args )
{
	unsigned int nbPoints = _plugin->_paramNbPoints->getValue();

	if( (nbPoints < kMaxNbPoints) && (_plugin->_paramMethod->getValue() == eParamMethodCreation) )
	{
		_plugin->_paramPointIn[nbPoints]->setIsSecretAndDisabled(false);
		_plugin->_paramPointIn[nbPoints]->setValue(args.penPosition.x,args.penPosition.y);
		_plugin->_paramNbPoints->setValue(nbPoints+1);

		return _interactScene.penDown( args );
	}
	else if(_plugin->_paramMethod->getValue() == eParamMethodMove)
	{
		return _interactScene.penDown( args );
	}
	else if(_plugin->_paramMethod->getValue() == eParamMethodDelete)
        {
	}
	return false;
}

bool WarpOverlayInteract::keyDown( const OFX::KeyArgs& args )
{
	if(args.keySymbol == kOfxKey_space)
		TUTTLE_COUT("test");
	else
		TUTTLE_COUT("test raté");
	return false;
}

//bool WarpOverlayInteract::keyUp( const KeyArgs& args );

//bool WarpOverlayInteract::keyRepeat( const KeyArgs& args );

//if(PinningOverlayInteract::keyDown(args))
  //  TUTTLE_COUT("control est pressé comme une orange");

bool WarpOverlayInteract::penUp( const OFX::PenArgs& args )
{
    if(_plugin->_paramMethod->getValue() == eParamMethodCreation)
    {
        unsigned int numPt = _plugin->_paramNbPoints->getValue();
        TUTTLE_COUT(numPt);

        point2<double> ptCur;
        ptCur.x = _plugin->_paramPointIn[numPt-1]->getValue().x;
        ptCur.y = _plugin->_paramPointIn[numPt-1]->getValue().y;

        point2<double> ptTgt1;
        ptTgt1.x = args.penPosition.x;
        ptTgt1.y = args.penPosition.y;

        point2<double> ptTgt2;
        ptTgt2.x = (ptCur.x)+(ptCur.x - args.penPosition.x);
        ptTgt2.y = (ptCur.y)+(ptCur.y - args.penPosition.y);

        tabTgt.push_back( point2<double>(ptTgt1.x , ptTgt1.y) );
        tabTgt.push_back( point2<double>(ptTgt2.x , ptTgt2.y) );
        return _interactScene.penUp( args );
    }
    else
        return _interactScene.penUp( args );
}

}
}
}
