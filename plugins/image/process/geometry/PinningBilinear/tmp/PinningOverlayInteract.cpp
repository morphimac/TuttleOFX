#include "PinningOverlayInteract.hpp"
#include "PinningDefinitions.hpp"
#include "PinningPlugin.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/interact/ParamPoint.hpp>
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


	/*
    for( unsigned int i = 0; i<4; ++i )
    {
    	_paramSrc[i] = fetchDouble2DParam( kParamPointSource + boost::lexical_cast<std::string>(i) );
    }
    */
    
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramSrc[0], _plugin->_clipSrc ) );
}

bool PinningOverlayInteract::draw( const OFX::DrawArgs& args )	
{
	typedef boost::gil::point2<Scalar> Point2;
	bool displaySomething        = false;

	glLineWidth(5);
	glColor3d( 1.0, 0.0, 0.0 );
	glBegin( GL_LINE_STRIP );
	
	OfxPointI size = _plugin->_clipSrc->getPixelRodSize( args.time );
    for( unsigned int i = 0; i<4; ++i )
    {
    	OfxPointD p = _plugin->_paramSrc[i]->getValue();
		glVertex2d( p.x * size.x , p.y * size.x );
	}
	glEnd();
	
	displaySomething |= _interactScene.draw( args );

	return displaySomething;
}
/*
bool PinningOverlayInteract::penMotion( const OFX::PenArgs& args )	//deplacement de la souris
{
	return _interactScene.penMotion( args );
}

bool PinningOverlayInteract::penDown( const OFX::PenArgs& args )	//clic appuye
{
	return _interactScene.penDown( args );
}

bool PinningOverlayInteract::penUp( const OFX::PenArgs& args )	// clic relache
{
	return _interactScene.penUp( args );
}
*/

}
}
}
