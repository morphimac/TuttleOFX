#include "TransformAffinePluginFactory.hpp"
#include "TransformAffinePlugin.hpp"
#include "TransformAffineDefinitions.hpp"
#include "TransformAffineOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
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
	desc.setLabels( "TuttleTransformAffine", "TransformAffine",
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

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<TransformAffineEffectOverlayDescriptor > ( ) );
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

	OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam( kParamOverlay );
	overlay->setLabel( "Overlay" );
	overlay->setDefault( true );

	//////////////////// IN Points ////////////////////
	OFX::GroupParamDescriptor* groupIn = desc.defineGroupParam( kParamGroupIn );
	groupIn->setLabel( "Input points" );

	OFX::Double2DParamDescriptor* pIn0 = desc.defineDouble2DParam( kParamPointIn + "0" );
	pIn0->setLabel( "Input point 0" );
	pIn0->setDefault( -0.5, -0.5 );
	pIn0->setParent( groupIn );

	OFX::Double2DParamDescriptor* pIn1 = desc.defineDouble2DParam( kParamPointIn + "1" );
	pIn1->setLabel( "Input point 1" );
	pIn1->setDefault( 0.5, -0.5 );
	pIn1->setParent( groupIn );

	OFX::Double2DParamDescriptor* pIn2 = desc.defineDouble2DParam( kParamPointIn + "2" );
	pIn2->setLabel( "Input point 2" );
	pIn2->setDefault( 0.5, 0.5 );
	pIn2->setParent( groupIn );

	OFX::BooleanParamDescriptor* overlayIn = desc.defineBooleanParam( kParamOverlayIn );
	overlayIn->setLabel( "Overlay" );
	overlayIn->setDefault( true );
	overlayIn->setParent( groupIn );

	OFX::RGBParamDescriptor* ouverlayInColor = desc.defineRGBParam( kParamOverlayInColor );
	ouverlayInColor->setLabel( "Color" );
	ouverlayInColor->setHint( "Input point overlay color" );
	ouverlayInColor->setDefault( 1.0, 0.0, 0.0 );
	ouverlayInColor->setParent( groupIn );

	//////////////////// OUT Points ////////////////////
	OFX::GroupParamDescriptor* groupOut = desc.defineGroupParam( kParamGroupOut );
	groupOut->setLabel( "Output points" );

	OFX::Double2DParamDescriptor* pOut0 = desc.defineDouble2DParam( kParamPointOut + "0" );
	pOut0->setLabel( "Output point 0" );
	pOut0->setDefault( -0.5, -0.5 );
	pOut0->setParent( groupOut );

	OFX::Double2DParamDescriptor* pOut1 = desc.defineDouble2DParam( kParamPointOut + "1" );
	pOut1->setLabel( "Output point 1" );
	pOut1->setDefault( 0.5, -0.5 );
	pOut1->setParent( groupOut );

	OFX::Double2DParamDescriptor* pOut2 = desc.defineDouble2DParam( kParamPointOut + "2" );
	pOut2->setLabel( "Output point 2" );
	pOut2->setDefault( 0.5, 0.5 );
	pOut2->setParent( groupOut );

	OFX::BooleanParamDescriptor* overlayOut = desc.defineBooleanParam( kParamOverlayOut );
	overlayOut->setLabel( "Overlay" );
	overlayOut->setDefault( true );
	overlayOut->setParent( groupOut );

	OFX::RGBParamDescriptor* ouverlayOutColor = desc.defineRGBParam( kParamOverlayOutColor );
	ouverlayOutColor->setLabel( "Color" );
	ouverlayOutColor->setHint( "Output point overlay color" );
	ouverlayOutColor->setDefault( 0.0, 0.0, 1.0 );
	ouverlayOutColor->setParent( groupOut );

	//////////////////// Matrix ////////////////////
	OFX::GroupParamDescriptor* groupMatrix = desc.defineGroupParam( kParamGroupMatrix );
	groupMatrix->setLabel( "Matrix" );
	groupMatrix->setHint( "Transformation matrix" );

	OFX::Double3DParamDescriptor* matrixRow0 = desc.defineDouble3DParam( kParamMatrixRow + "0" );
	matrixRow0->setLabel( "row 0" );
	matrixRow0->setDefault( 1.0, 0.0, 0.0 );
	matrixRow0->setParent( groupMatrix );

	OFX::Double3DParamDescriptor* matrixRow1 = desc.defineDouble3DParam( kParamMatrixRow + "1" );
	matrixRow1->setLabel( "row 1" );
	matrixRow1->setDefault( 0.0, 1.0, 0.0 );
	matrixRow1->setParent( groupMatrix );

	OFX::Double3DParamDescriptor* matrixRow2 = desc.defineDouble3DParam( kParamMatrixRow + "2" );
	matrixRow2->setLabel( "rox 2" );
	matrixRow2->setDefault( 0.0, 0.0, 1.0 );
	matrixRow2->setParent( groupMatrix );

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

