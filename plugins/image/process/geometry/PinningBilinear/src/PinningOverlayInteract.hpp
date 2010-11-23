#ifndef _LENSDISTORTOVERLAYINTERACT_HPP_
#define _LENSDISTORTOVERLAYINTERACT_HPP_

#include "PinningPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace pinning {

class PinningOverlayInteract : public OFX::OverlayInteract
{
typedef double Scalar;

PinningPlugin* _plugin;

interact::InteractInfos _infos;
interact::InteractScene _interactScene;

public:
	PinningOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );

	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );

};

class PinningOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new PinningOverlayInteract( handle, effect );
	}

};

}
}
}

#endif

