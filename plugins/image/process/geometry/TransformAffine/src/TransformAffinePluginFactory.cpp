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

	//////////////////// Options ////////////////////
	OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam( kParamMethod );
	method->setLabel( "Method" );
	method->appendOption( kParamMethodAffine );
	method->appendOption( kParamMethodPerspective );
	method->appendOption( kParamMethodBilinear );
	method->setDefault( 1 );
	method->setHint( "Interpolation method" );

	OFX::ChoiceParamDescriptor* interpolation = desc.defineChoiceParam( kParamInterpolation );
	interpolation->setLabel( "Interpolation" );
	interpolation->appendOption( kParamInterpolationNearest );
	interpolation->appendOption( kParamInterpolationBilinear );
	interpolation->setDefault( 1 );
	interpolation->setHint( "Interpolation method" );

	OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam( kParamOverlay );
	overlay->setLabel( "Overlay" );
	overlay->setDefault( true );

	//////////////////// IN Points ////////////////////
	OFX::GroupParamDescriptor* groupIn = desc.defineGroupParam( kParamGroupIn );
	groupIn->setLabel( "Input points" );

	OFX::Double2DParamDescriptor* pIn0 = desc.defineDouble2DParam( kParamPointIn + "0" );
	pIn0->setLabel( "In 0" );
	pIn0->setHint( "Input point 0" );
	pIn0->setDefault( -0.5, -0.5 );
	pIn0->setParent( groupIn );

	OFX::Double2DParamDescriptor* pIn1 = desc.defineDouble2DParam( kParamPointIn + "1" );
	pIn1->setLabel( "In 1" );
	pIn1->setHint( "Input point 1" );
	pIn1->setDefault( 0.5, -0.5 );
	pIn1->setParent( groupIn );

	OFX::Double2DParamDescriptor* pIn2 = desc.defineDouble2DParam( kParamPointIn + "2" );
	pIn2->setLabel( "In 2" );
	pIn2->setHint( "Input point 2" );
	pIn2->setDefault( 0.5, 0.5 );
	pIn2->setParent( groupIn );

	OFX::Double2DParamDescriptor* pIn3 = desc.defineDouble2DParam( kParamPointIn + "3" );
	pIn3->setLabel( "In 3" );
	pIn3->setHint( "Input point 3" );
	pIn3->setDefault( -0.5, 0.5 );
	pIn3->setParent( groupIn );

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
	pOut0->setLabel( "Out 0" );
	pOut0->setHint( "Output point 0" );
	pOut0->setDefault( -0.5, -0.5 );
	pOut0->setParent( groupOut );

	OFX::Double2DParamDescriptor* pOut1 = desc.defineDouble2DParam( kParamPointOut + "1" );
	pOut1->setLabel( "Out 1" );
	pOut1->setHint( "Output point 1" );
	pOut1->setDefault( 0.5, -0.5 );
	pOut1->setParent( groupOut );

	OFX::Double2DParamDescriptor* pOut2 = desc.defineDouble2DParam( kParamPointOut + "2" );
	pOut2->setLabel( "Out 2" );
	pOut2->setHint( "Output point 2" );
	pOut2->setDefault( 0.5, 0.5 );
	pOut2->setParent( groupOut );

	OFX::Double2DParamDescriptor* pOut3 = desc.defineDouble2DParam( kParamPointOut + "3" );
	pOut3->setLabel( "Out 3" );
	pOut3->setHint( "Output point 3" );
	pOut3->setDefault( -0.5, 0.5 );
	pOut3->setParent( groupOut );

	OFX::BooleanParamDescriptor* overlayOut = desc.defineBooleanParam( kParamOverlayOut );
	overlayOut->setLabel( "Overlay" );
	overlayOut->setDefault( true );
	overlayOut->setParent( groupOut );

	OFX::RGBParamDescriptor* ouverlayOutColor = desc.defineRGBParam( kParamOverlayOutColor );
	ouverlayOutColor->setLabel( "Color" );
	ouverlayOutColor->setHint( "Output point overlay color" );
	ouverlayOutColor->setDefault( 0.0, 0.0, 1.0 );
	ouverlayOutColor->setParent( groupOut );

	//////////////////// Persp Matrix ////////////////////
	OFX::GroupParamDescriptor* groupPerspMatrix = desc.defineGroupParam( kParamGroupPerspMatrix );
	groupPerspMatrix->setLabel( "Perspective matrix" );
	groupPerspMatrix->setHint( "Transformation matrix" );

	OFX::Double3DParamDescriptor* perspMatrixRow0 = desc.defineDouble3DParam( kParamPerspMatrixRow + "0" );
	perspMatrixRow0->setLabel( "row 0" );
	perspMatrixRow0->setDefault( 1.0, 0.0, 0.0 );
	perspMatrixRow0->setParent( groupPerspMatrix );

	OFX::Double3DParamDescriptor* perspMatrixRow1 = desc.defineDouble3DParam( kParamPerspMatrixRow + "1" );
	perspMatrixRow1->setLabel( "row 1" );
	perspMatrixRow1->setDefault( 0.0, 1.0, 0.0 );
	perspMatrixRow1->setParent( groupPerspMatrix );

	OFX::Double3DParamDescriptor* perspMatrixRow2 = desc.defineDouble3DParam( kParamPerspMatrixRow + "2" );
	perspMatrixRow2->setLabel( "row 2" );
	perspMatrixRow2->setDefault( 0.0, 0.0, 1.0 );
	perspMatrixRow2->setParent( groupPerspMatrix );

	////////////////// Bilinear Matrix ////////////////////
	OFX::GroupParamDescriptor* groupBilMatrix = desc.defineGroupParam( kParamGroupBilinearMatrix );
	groupBilMatrix->setLabel( "Bilinear matrix" );
	groupBilMatrix->setHint( "Billinear transformation matrix" );

	OFX::Double2DParamDescriptor* bilMatrixRow0 = desc.defineDouble2DParam( kParamBilinearMatrixRow + "0" );
	bilMatrixRow0->setLabel( "row 0" );
	bilMatrixRow0->setDefault( 1.0, 0.0 );
	bilMatrixRow0->setParent( groupBilMatrix );

	OFX::Double2DParamDescriptor* bilMatrixRow1 = desc.defineDouble2DParam( kParamBilinearMatrixRow + "1" );
	bilMatrixRow1->setLabel( "row 1" );
	bilMatrixRow1->setDefault( 0.0, 1.0 );
	bilMatrixRow1->setParent( groupBilMatrix );

	OFX::Double2DParamDescriptor* bilMatrixRow2 = desc.defineDouble2DParam( kParamBilinearMatrixRow + "2" );
	bilMatrixRow2->setLabel( "row 2" );
	bilMatrixRow2->setDefault( 0.0, 0.0 );
	bilMatrixRow2->setParent( groupBilMatrix );

	OFX::Double2DParamDescriptor* bilMatrixRow3 = desc.defineDouble2DParam( kParamBilinearMatrixRow + "3" );
	bilMatrixRow3->setLabel( "row 3" );
	bilMatrixRow3->setDefault( 0.0, 0.0 );
	bilMatrixRow3->setParent( groupBilMatrix );

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

