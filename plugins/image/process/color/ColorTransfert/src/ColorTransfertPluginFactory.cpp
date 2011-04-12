#include "ColorTransfertPluginFactory.hpp"
#include "ColorTransfertPlugin.hpp"
#include "ColorTransfertDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorTransfertPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "ColorTransfert", "ColorTransfert",
		            "ColorTransfert" );
	desc.setPluginGrouping( "tuttle" );

	desc.setDescription( "ColorTransfert\n"
	                     "Plugin is used to ???." );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorTransfertPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

        OFX::ClipDescriptor* srcRefClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
        srcRefClip->addSupportedComponent( OFX::ePixelComponentRGBA );
        srcRefClip->addSupportedComponent( OFX::ePixelComponentRGB );
        srcRefClip->addSupportedComponent( OFX::ePixelComponentAlpha );
        srcRefClip->setSupportsTiles( kSupportTiles );

        OFX::ClipDescriptor* dstRefClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
        dstRefClip->addSupportedComponent( OFX::ePixelComponentRGBA );
        dstRefClip->addSupportedComponent( OFX::ePixelComponentRGB );
        dstRefClip->addSupportedComponent( OFX::ePixelComponentAlpha );
        dstRefClip->setSupportsTiles( kSupportTiles );


	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kParamHelpButton );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorTransfertPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ColorTransfertPlugin( handle );
}

}
}
}

