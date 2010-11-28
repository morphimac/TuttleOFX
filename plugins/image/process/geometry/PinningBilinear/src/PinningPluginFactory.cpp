#include "PinningPluginFactory.hpp"
#include "PinningPlugin.hpp"
#include "PinningDefinitions.hpp"
//#include "PinningOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void PinningPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttlePinningBilinear", "Pinning",
		            "Pinning" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	
	//desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<PinningEffectOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void PinningPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{

	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName ); // clip de source nomme source
	//OFX::ClipDescriptor* toto = desc.defineClip( "toto" );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );	//proprietes de la source
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName ); 	//sortie, laisser comme ca et pi c'est tout
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::GroupParamDescriptor* grpSrc = desc.defineGroupParam( kParamGroupSource );	//groupe source
	grpSrc->setLabel( "Points de départ" );
	grpSrc->setHint( "Selectionnez 4 points de départ" );

	OFX::Double2DParamDescriptor* src0 = desc.defineDouble2DParam( kParamPointSource + "0" );	//kpa... nom utilisé par le plug
	src0->setLabel( "src0" );	//nom que voit l'utilisateur	
	src0->setDefault( 0, 0 );
	src0->setParent(grpSrc);
	OFX::Double2DParamDescriptor* src1 = desc.defineDouble2DParam( kParamPointSource + "1" );
	src1->setLabel( "src1" );
	src1->setDefault( 0, 1 );
	src1->setParent(grpSrc);
	OFX::Double2DParamDescriptor* src2 = desc.defineDouble2DParam( kParamPointSource + "2" );
	src2->setLabel( "src2" );
	src2->setDefault( 1, 1 );
	src2->setParent(grpSrc);
	OFX::Double2DParamDescriptor* src3 = desc.defineDouble2DParam( kParamPointSource + "3" );
	src3->setLabel( "src3" );
	src3->setDefault( 1, 0 );
	src3->setParent(grpSrc);
	
	OFX::GroupParamDescriptor* grpDst = desc.defineGroupParam( kParamGroupDestination );	//groupe destination
	grpDst->setLabel( "Points d'arrivée" );
	grpDst->setHint( "Selectionnez 4 points d'arrivée" );

	OFX::Double2DParamDescriptor* dest0 = desc.defineDouble2DParam( kParamPointDestination + "0" );	//kpa... nom utilisé par le plug
	dest0->setLabel( "dst0" );	//nom que voit l'utilisateur	
	dest0->setDefault( 0, 0 );
	dest0->setParent(grpDst);
	OFX::Double2DParamDescriptor* dest1 = desc.defineDouble2DParam( kParamPointDestination + "1" );
	dest1->setLabel( "dst1" );
	dest1->setDefault( 0, 1 );
	dest1->setParent(grpDst);
	OFX::Double2DParamDescriptor* dest2 = desc.defineDouble2DParam( kParamPointDestination + "2" );
	dest2->setLabel( "dst2" );
	dest2->setDefault( 1, 1 );
	dest2->setParent(grpDst);
	OFX::Double2DParamDescriptor* dest3 = desc.defineDouble2DParam( kParamPointDestination + "3" );
	dest3->setLabel( "dst3" );
	dest3->setDefault( 1, 0 );
	dest3->setParent(grpDst);
	
	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kParamHelpButton );	//cree un bouton help
	//helpButton->setHint( "bla bla" );
	helpButton->setLabel( "Help" );

	OFX::GroupParamDescriptor* displayOptions = desc.defineGroupParam( kParamDisplayOptions );
	displayOptions->setLabel( "Options d'affichage" );
	displayOptions->setHint( "Options d'affichage (Ne change rien sur l'image)" );

	OFX::BooleanParamDescriptor* displayGrid = desc.defineBooleanParam( kParamGridOverlay );
	displayGrid->setLabel( "Afficher la grille" );
	displayGrid->setParent( *displayOptions );
	displayGrid->setDefault( false );
	displayGrid->setEvaluateOnChange( false );
	displayGrid->setHint( "Afficher la grille" );

	OFX::Double2DParamDescriptor* gridCenter = desc.defineDouble2DParam( kParamGridCenter );
	gridCenter->setLabel( "Centre de la grille" );
	gridCenter->setDoubleType( OFX::eDoubleTypePlain );
	gridCenter->setParent( *displayOptions );
	gridCenter->setDefault( 0.0, 0.0 );
	gridCenter->setDisplayRange( -1.0, -1.0, 1.0, 1.0 );
	gridCenter->setEvaluateOnChange( false );
	gridCenter->setHint( "Allows you to shift the center of the  display grid." );

	OFX::Double2DParamDescriptor* center = desc.defineDouble2DParam( kParamCenter );
	center->setLabel( "Center" );
	center->setDoubleType( OFX::eDoubleTypePlain );
	center->setDefault( 0.0, 0.0 );
	center->setDisplayRange( -1.0, -1.0, 1.0, 1.0 );
	center->setHint( "Center parameter allows you to shift the center of distortion." );


	OFX::Double2DParamDescriptor* gridScale = desc.defineDouble2DParam( kParamGridScale );
	gridScale->setLabel( "Echelle de la grille" );
	gridScale->setDoubleType( OFX::eDoubleTypePlain );
	gridScale->setParent( *displayOptions );
	gridScale->setDefault( 1.0, 1.0 );
	gridScale->setDisplayRange( -10.0, -10.0, 10.0, 10.0 );
	gridScale->setEvaluateOnChange( false );
	gridScale->setHint( "Allows you to scale the display grid." );

																				//kParamhelpButton : appelle un pluging
	//OFX::PushButtonParamDescriptor* poussoir = desc.definePushButtonParam( kParamPoussoir );	//cree un bouton help
	//poussoir->setLabel( "poussoir" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PinningPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new PinningPlugin( handle );
}

}
}
}

