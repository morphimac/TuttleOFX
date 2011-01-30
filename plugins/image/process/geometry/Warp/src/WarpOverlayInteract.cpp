#include "WarpOverlayInteract.hpp"
#include "WarpDefinitions.hpp"
#include "WarpPlugin.hpp"
//#include "Bezier/bezier.hpp"
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

        for( std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut )
	{
                interact::AndActiveFunctor<>* activeOut = new interact::AndActiveFunctor<>();
                activeOut->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
                activeOut->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointOut[cptOut] ) );
                _interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_paramPointOut[cptOut], _plugin->_clipSrc ),
                                                                  activeOut,
                                                                  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );

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

        int nbPoints = _plugin->_paramNbPoints->getValue();

        if(nbPoints >1)
            {
                static const float lineWidth = 1.0;
                glLineWidth( lineWidth );
                glColor3f( 0.0f, 1.0f, 0.0f );

                glBegin( GL_LINE_STRIP );
                for(int i=0 ; i < nbPoints ; ++i)
                {
                        for(int j = 0; j < kMaxNbPoints; ++j)
                        {
                                if(nbPoints > j)
                                    glVertex2f( _plugin->_paramPointIn[nbPoints-j-1]->getValue().x, _plugin->_paramPointIn[nbPoints-j-1]->getValue().y);
                        }
                }
                glEnd();
                /*for(int j = 0; j < nbPoints; ++j)
                {
                        bezier::dessinePointRecur(tabPts, nbPoints+1);
                }*/
            }
	displaySomething |= _interactScene.draw( args );

	return displaySomething;       
}

bool WarpOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool WarpOverlayInteract::penDown( const OFX::PenArgs& args )
{
        unsigned int nbPoints = _plugin->_paramNbPoints->getValue();

        if((nbPoints < kMaxNbPoints) && (_plugin->_paramMethod->getValue() == eParamMethodCreation))
        {
                _plugin->_paramPointIn[nbPoints]->setIsSecretAndDisabled(false);
                _plugin->_paramPointIn[nbPoints]->setValue(args.penPosition.x,args.penPosition.y);

                tmp.x = _plugin->_paramPointIn[nbPoints]->getValue().x;
                tmp.y = _plugin->_paramPointIn[nbPoints]->getValue().y;
                tabPts.push_back(tmp);

                _plugin->_paramNbPoints->setValue(nbPoints+1);

                return _interactScene.penDown( args );
        }

        else if(_plugin->_paramMethod->getValue() == eParamMethodMove)
        {
            return _interactScene.penDown( args );
        }
        else if(_plugin->_paramMethod->getValue() == eParamMethodDelete)
        {
                //_plugin->_paramPointIn[2]->setIsSecretAndDisabled(true);
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
	return _interactScene.penUp( args );
}

}
}
}
