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

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut0, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ), new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut1, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ), new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut2, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ), new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut3, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ), new interact::ColorRGBParam(_plugin->_paramOverlayOutColor) );

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn0, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ), new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn1, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ), new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn2, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ), new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn3, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ), new interact::ColorRGBParam(_plugin->_paramOverlayInColor) );
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
	return _interactScene.penMotion( args );
}

bool PinningOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool PinningOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}
