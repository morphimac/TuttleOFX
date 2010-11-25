#include "TransformAffinePluginFactory.hpp"
#include "TransformAffinePlugin.hpp"
#include "TransformAffineDefinitions.hpp"
#include "TransformAffineOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace transformAffine {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TransformAffinePluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TransformAffine", "TransformAffine",
		            "TransformAffine" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TransformAffinePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

        //Groupe des points d'entrée
        OFX::GroupParamDescriptor* groupSource = desc.defineGroupParam(kParamGroupSource);
        groupSource->setLabel("Groupe de points d'entrée");

        // Points d'entrée
        OFX::Double2DParamDescriptor* pointEntree1 = desc.defineDouble2DParam(kParamPointSource + "0");
        pointEntree1->setLabel("Point Entree 1");
        pointEntree1->setDefault(0.0, 0.0);
        pointEntree1->setParent(groupSource);

        OFX::Double2DParamDescriptor* pointEntree2 = desc.defineDouble2DParam(kParamPointSource + "1");
        pointEntree2->setLabel("Point Entree 2");
        pointEntree2->setDefault(0.0, 1.0);
        pointEntree2->setParent(groupSource);

        OFX::Double2DParamDescriptor* pointEntree3 = desc.defineDouble2DParam(kParamPointSource + "2");
        pointEntree3->setLabel("Point Entree 3");
        pointEntree3->setDefault(1.0, 1.0);
        pointEntree3->setParent(groupSource);

        //Groupe des points de sortie
        OFX::GroupParamDescriptor* groupSortie = desc.defineGroupParam(kParamGroupSortie);
        groupSortie->setLabel("Groupe de points de sortie");

        // Points de sortie
        OFX::Double2DParamDescriptor* pointSortie1 = desc.defineDouble2DParam(kParamPointSortie + "0");
        pointSortie1->setLabel("Point Sortie 1");
        pointSortie1->setDefault(0.0, 0.0);
        pointSortie1->setParent(groupSortie);

        OFX::Double2DParamDescriptor* pointSortie2 = desc.defineDouble2DParam(kParamPointSortie + "1");
        pointSortie2->setLabel("Point Sortie 2");
        pointSortie2->setDefault(0.0, 1.0);
        pointSortie2->setParent(groupSortie);

        OFX::Double2DParamDescriptor* pointSortie3 = desc.defineDouble2DParam(kParamPointSortie + "2");
        pointSortie3->setLabel("Point Sortie 3");
        pointSortie3->setDefault(1.0, 1.0);
        pointSortie3->setParent(groupSortie);

        //desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<LensDistortEffectOverlayDescriptor>() );

    }

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TransformAffinePluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new TransformAffinePlugin( handle );
}

}
}
}

