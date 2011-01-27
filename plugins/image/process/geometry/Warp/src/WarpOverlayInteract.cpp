#include "WarpOverlayInteract.hpp"
#include "WarpDefinitions.hpp"
#include "WarpPlugin.hpp"
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
#include <tuttle/plugin/interact/ParamPointRelativePoint.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

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

bool WarpOverlayInteract::bezier( const std::vector< point2<double> > p1, std::vector< point2<double> > p2)
{
    /*
    std::vector< point2<double> > pBezier;

    for(double t= 0.0 ; t < 1.0 ; t+=0.02)
    {
        pBezier.x = ( (1-t)*(p1.x) ) + t*(p2.x);
        pBezier.y = ( (1-t)*(p1.y) ) + t*(p2.x);
    }
    */
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

                glColor3f( 1.0f, 0.25f, 0.6f );

                glBegin( GL_POINTS );
                for(int i=0 ; i < nbPoints ; ++i)
                {
                        for(int j = 0; j < kMaxNbPoints; ++j)
                        {
                                if(nbPoints > j)
                                {
                                    double ptx1 = _plugin->_paramPointIn[nbPoints-j-1]->getValue().x;
                                    double pty1 = _plugin->_paramPointIn[nbPoints-j-1]->getValue().y;
                                    glVertex2f( ptx1 + 50.0,pty1 + 50.0);
                                }
                        }
                }
                glEnd();
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
		for(unsigned int i = 0; i<nbPoints; ++i)
		{/*
			/*boost::mt19937 rng;                 
			boost::uniform_int<> randomX(0,640);     
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(rng, randomX);             
			int x = die();
			std::cout<<"Aleatoire "<<x<<std::endl;*/
                	_plugin->_paramPointIn[nbPoints]->setIsSecretAndDisabled(false);
                	_plugin->_paramPointIn[nbPoints]->setValue(args.penPosition.x,args.penPosition.y);

                	_plugin->_paramNbPoints->setValue(nbPoints+1);
		}

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
}

bool WarpOverlayInteract::keyDown( const OFX::KeyArgs& args )
{
        if(args.keySymbol == kOfxKey_space)
            TUTTLE_COUT("test");
        else
            TUTTLE_COUT("test raté");
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
