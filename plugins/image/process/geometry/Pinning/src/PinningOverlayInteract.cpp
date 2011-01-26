#include "PinningOverlayInteract.hpp"
#include "PinningDefinitions.hpp"
#include "PinningPlugin.hpp"
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
namespace pinning {

PinningOverlayInteract::PinningOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = static_cast<PinningPlugin*>( _effect );

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut0, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut1, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut2, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	interact::AndActiveFunctor<>* activeOut3 = new interact::AndActiveFunctor<>();
	activeOut3->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	activeOut3->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointOut3 ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut3, _plugin->_clipSrc ),
							  activeOut3,
							  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn0, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn1, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn2, _plugin->_clipSrc ),
							  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	interact::AndActiveFunctor<>* activeIn3 = new interact::AndActiveFunctor<>();
	activeIn3->push_back( new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	activeIn3->push_back( new interact::IsNotSecretParamFunctor<>( _plugin->_paramPointIn3 ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn3, _plugin->_clipSrc ),
							  activeIn3,
							  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
}

bool PinningOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	static const float lineWidth = 2.0;
	bool displaySomething        = false;

	displaySomething |= _interactScene.draw( args );

	return displaySomething;
}

bool PinningOverlayInteract::penMotion( const OFX::PenArgs& args )
{

	//std::cout<<"pen move"<<std::endl;
	
        return _interactScene.penMotion( args );
}

bool PinningOverlayInteract::penDown( const OFX::PenArgs& args )
{
	//std::cout<<"pen down"<<std::endl;
	OfxPointD pos = args.penPosition;
	std::cout<<pos.x<<";"<<pos.y<<std::endl;
	double pres = args.penPressure; // lorsque l'on utilise une tablette graphique, on peut récupérer la pression du stylo
	OfxTime time = args.time;
	
        int width = _plugin->_clipSrc->getCanonicalRodSize(args.time, args.renderScale).x;
        _plugin->_paramPointIn0->setValue( args.penPosition.x / width, args.penPosition.y / width );
	

        /*
	double seuil = 10;
	
	for( int i; i < 50; ++i )
	{
		if( (abs(pos.x - pIn[i][0]) < seuil) &&  (abs(pos.y - pIn[i][1]) < seuil) )
		{
			pSelect[i] = pIn[i];
		}
		if( (abs(pos.x - pOut[i][0]) < seuil) &&  (abs(pos.y - pOut[i][1]) < seuil) )
		{
			pSelect[i] = pOut[i];
		}
	}
        */
	
	
        bool selObj = _interactScene.penDown( args );
        if( selObj )
        {
            return true;
        }
        else
        {
            _beginSelection = true;
            _multiSelectionRec.x1 = args.penPosition.x;
            _multiSelectionRec.y1 = args.penPosition.y;

            if(_keyPressed_ctrl)
            {
                _multiSelection = true;
            }
        }
}

bool PinningOverlayInteract::penUp( const OFX::PenArgs& args )
{
    if( _multiSelection )
    {
        _multiSelectionRec.x2 = args.penPosition.x;
        _multiSelectionRec.y2 = args.penPosition.y;
        _beginSelection = false;

        // parcours Points
        //??

        _multiSelection = false;
        BOOST_FOREACH( const interact::InteractObject& p, _interactScene.getObjects() )
        {
            bool b = p.selectIfIsIn( _multiSelectionRec );
            _multiSelection = _multiSelection || b;
        }
    }

    // x , y
    _interactScene.moveXYSelected( x, y );

	//std::cout<<"pen up"<<std::endl;
    return _interactScene.penUp( args );
}


bool PinningOverlayInteract::keyDown( const KeyArgs& args )
{
   if( (args.keySymbol == kOfxKey_Control_L) || (args.keySymbol == kOfxKey_Control_R) )
   {
         _keyPressed_ctrl = true;
   }
}

bool keyUp( const KeyArgs& args )
{
   if( (args.keySymbol == kOfxKey_Control_L) || (args.keySymbol == kOfxKey_Control_R) )
   {
         _keyPressed_ctrl = false;
   }
}

bool keyRepeat( const KeyArgs& args )
{

}

/*
void movePts(pSelect)
{
	for(int i=0 ; i<pSelect.size() ; ++i)
	{
		pIn[i][...] += n;
	}
}
*/
/*
void rotatePts(pSelect, double angle)
{
	OfxPointD centreRotation;
	
	for(int i=0 ; i<pSelect.size() ; ++i)
	{
		centreRotation.x += pSelect[i][0];
		centreRotation.y += pSelect[i][1];
	}
	centreRotation.x /= pSelect.size();
	centreRotation.y /= pSelect.size();
	
	for(int i=0 ; i<pSelect.size() ; ++i)
	{
		pSelect[i][0] = pSelect[i][0]*cos(angle) - pSelect[i][1]*sin(angle);
		pSelect[i][1] = pSelect[i][0]*sin(angle) - pSelect[i][1]*cos(angle);
	}
}
*/

}
}
}
