#ifndef _PINNINGOVERLAYINTERACT_HPP_
#define _PINNINGOVERLAYINTERACT_HPP_

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

        bool _beginSelection;
        OfxRectD _multiSelectionRec;

        bool _keyPressed_ctrl;
        bool _keyPressed_shift;

public:
	PinningOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );

	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );

        bool keyDown( const KeyArgs& args );
        bool keyUp( const KeyArgs& args );
        bool keyRepeat( const KeyArgs& args );
};


class PinningEffectOverlayDescriptor : public OFX::EffectOverlayDescriptor
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
