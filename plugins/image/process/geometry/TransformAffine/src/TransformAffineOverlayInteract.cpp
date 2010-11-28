#include "TransformAffineOverlayInteract.hpp"
#include "TransformAffineDefinitions.hpp"
#include "TransformAffinePlugin.hpp"
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
namespace transformAffine {

TransformAffineOverlayInteract::TransformAffineOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = static_cast<TransformAffinePlugin*>( _effect );

	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn0, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn1, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn2, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointIn3, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayIn ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut0, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut1, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut2, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
	_interactScene.push_back( new interact::ParamPoint<interact::FrameClip, eCoordonateSystemXXcn>( _infos, _plugin->_paramPointOut3, _plugin->_clipSrc ), new interact::IsActiveBooleanParamFunctor<>( _plugin->_paramOverlayOut ) );
}

bool TransformAffineOverlayInteract::draw( const OFX::DrawArgs& args )
{
	if( !_plugin->_paramOverlay->getValue() || !_plugin->_clipSrc->isConnected() )
		return false;

	typedef boost::gil::point2<Scalar> Point2;
	static const float lineWidth = 2.0;
	bool displaySomething        = false;

	displaySomething |= _interactScene.draw( args );

	return displaySomething;
}

bool TransformAffineOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool TransformAffineOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool TransformAffineOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}
