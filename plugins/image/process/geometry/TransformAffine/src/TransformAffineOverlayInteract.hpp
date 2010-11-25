#ifndef _TRANSFORMAFFINEOVERLAYINTERACT_HPP_
#define _TRANSFORMAFFINEOVERLAYINTERACT_HPP_

#include "TransformAffinePlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace transformAffine {

class TransformAffineOverlayInteract : public OFX::OverlayInteract
{
typedef double Scalar;

TransformAffinePlugin* _plugin;

interact::InteractInfos _infos;
interact::InteractScene _interactScene;

public:
        TransformAffineOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
};

class TransformAffineEffectOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
                return new TransformAffineOverlayInteract( handle, effect );
	}

};

}
}
}

#endif
