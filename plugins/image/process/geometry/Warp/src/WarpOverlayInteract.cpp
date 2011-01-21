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

	for(int cptOut = 0; cptOut < nbPoints; ++cptOut)
	{
		_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut[cptOut], _plugin->_clipSrc ),
					  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ),
					  new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	for(int cptIn = 0; cptIn < nbPoints; ++cptIn)
	{
		_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn[cptIn], _plugin->_clipSrc ),
					  new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ),
					  new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	}
}

bool WarpOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	bool displaySomething        = false;

	displaySomething |= _interactScene.draw( args );

	return displaySomething;
}

bool WarpOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool WarpOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool WarpOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}
