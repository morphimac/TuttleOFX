#ifndef _WARPOVERLAYINTERACT_HPP_
#define _WARPOVERLAYINTERACT_HPP_

#include "WarpPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace warp {

class WarpOverlayInteract : public OFX::OverlayInteract
{
	typedef double Scalar;

	WarpPlugin* _plugin;

	interact::InteractInfos _infos;
	interact::InteractScene _interactScene;

public:
	WarpOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
};

class WarpEffectOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:

	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new WarpOverlayInteract( handle, effect );
	}

};

}
}
}

#endif
