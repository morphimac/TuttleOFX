#include "WarpPluginFactory.hpp"
#include "WarpPlugin.hpp"
#include "WarpDefinitions.hpp"
#include "WarpOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <limits>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void WarpPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleWarp", "Warp",
				 "Warp" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( false );
	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<WarpEffectOverlayDescriptor > ( ) );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void WarpPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
										   OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( false );

	OFX::ClipDescriptor* srcBClip = desc.defineClip( kClipSourceB );
	srcBClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcBClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcBClip->setOptional( true );
	srcBClip->setSupportsTiles( false );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );

	//////////////////// Options ////////////////////

	OFX::PushButtonParamDescriptor* reset = desc.definePushButtonParam( kParamReset );
	reset->setLabel( "Reset" );

	OFX::PushButtonParamDescriptor* setKey = desc.definePushButtonParam( kParamSetKey );
	setKey->setLabel( "SetKey" );

	OFX::DoubleParamDescriptor* transition = desc.defineDoubleParam( kParamTransition );
	transition->setLabel( "Transition" );
	transition->setHint( "Coefficient de transition" );
	transition->setDefault( 1.0 );
	transition->setRange( 0.0, 1.0 );
	transition->setDisplayRange( 0.0, 1.0 );

	//Settings
	OFX::GroupParamDescriptor* groupSettings = desc.defineGroupParam( kParamGroupSettings );
	groupSettings->setLabel( "Settings" );

	OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam( kParamMethod );
	method->setLabel( "Method" );
	method->appendOption( kParamMethodCreation );
	method->appendOption( kParamMethodDelete );
	method->appendOption( kParamMethodMove );
	method->setDefault( 0 );
	method->setHint( "Points method" );
	method->setEvaluateOnChange( false );
	method->setParent( groupSettings );

        OFX::ChoiceParamDescriptor* curve = desc.defineChoiceParam( kParamCurve );
        curve->setLabel( "Curve" );
        curve->appendOption( kParamCurve1 );
        curve->appendOption( kParamCurve2 );
        curve->setDefault( 0 );
        curve->setHint( "Number of curve" );
        curve->setEvaluateOnChange( false );
        curve->setParent( groupSettings );

	OFX::IntParamDescriptor* nbPoints = desc.defineIntParam( kParamNbPoints );
	nbPoints->setDefault( 0 );
	nbPoints->setRange( 0, kMaxNbPoints - 1 );
	nbPoints->setIsSecret( true );
	nbPoints->setParent( groupSettings );

        /*OFX::IntParamDescriptor* nbPointsCurve2 = desc.defineIntParam( kParamNbPointsCurve2 );
        nbPointsCurve2->setDefault( 0 );
        nbPointsCurve2->setRange( 0, (kMaxNbPoints/2) - 1 );
        nbPointsCurve2->setIsSecret( true );
        nbPointsCurve2->setParent( groupSettings );*/

	OFX::BooleanParamDescriptor* inverse = desc.defineBooleanParam( kParamInverse );
	inverse->setLabel( "Inverse" );
	inverse->setDefault( false );
	inverse->setParent( groupSettings );

	OFX::DoubleParamDescriptor* rigidity = desc.defineDoubleParam( kParamRigiditeTPS );
	rigidity->setLabel( "Rigidity" );
	rigidity->setHint( "TPS Rigidity coefficient" );
	rigidity->setDefault( 0.0 );
	//	rigidity->setRange( 0.0, std::numeric_limits<double>::max( ) );
	rigidity->setDisplayRange( 0.0, 10.0 );
	rigidity->setParent( groupSettings );

	OFX::IntParamDescriptor* nbPointsBezier = desc.defineIntParam( kParamNbPointsBezier );
	nbPointsBezier->setLabel( "Bezier" );
	nbPointsBezier->setHint( "Nombre de points dessinant la courbe de bezier" );
        nbPointsBezier->setDefault( 5 );
	nbPointsBezier->setRange( 0, std::numeric_limits<int>::max( ) );
        nbPointsBezier->setDisplayRange( 1, 20 );
	nbPointsBezier->setParent( groupSettings );

	//Overlay Points et tangentes
	OFX::GroupParamDescriptor* groupOverlay = desc.defineGroupParam( kParamGroupOverlay );
	groupOverlay->setLabel( "Overlay points et tangentes" );

	OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam( kParamOverlay );
	overlay->setLabel( "Overlay" );
	overlay->setHint( "Affiche la scene entière ou non" );
	overlay->setDefault( true );
	overlay->setEvaluateOnChange( false );
	overlay->setParent( groupOverlay );

	OFX::BooleanParamDescriptor* overlayIn = desc.defineBooleanParam( kParamOverlayIn );
	overlayIn->setLabel( "Points In" );
	overlayIn->setHint( "Affiche les points d'entrée sur la scène" );
	overlayIn->setDefault( true );
	overlayIn->setEvaluateOnChange( false );
	overlayIn->setParent( groupOverlay );

	OFX::BooleanParamDescriptor* overlayTgtIn = desc.defineBooleanParam( kParamOverlayTgtIn );
	overlayTgtIn->setLabel( "Bezier et tangentes In" );
	overlayTgtIn->setHint( "Affiche la courbe de Bezier et ses tangentes en fonction des points In" );
	overlayTgtIn->setDefault( true );
	overlayTgtIn->setEvaluateOnChange( false );
	overlayTgtIn->setParent( groupOverlay );

	OFX::BooleanParamDescriptor* overlayOut = desc.defineBooleanParam( kParamOverlayOut );
	overlayOut->setLabel( "Points Out" );
	overlayOut->setHint( "Affiche les points de sortie sur la scène" );
	overlayOut->setDefault( false );
	overlayOut->setEvaluateOnChange( false );
	overlayOut->setParent( groupOverlay );

	OFX::BooleanParamDescriptor* overlayTgtOut = desc.defineBooleanParam( kParamOverlayTgtOut );
	overlayTgtOut->setLabel( "Bezier et tangentes Out" );
	overlayTgtOut->setHint( "Affiche la courbe de Bezier et ses tangentes en fonction des points Out" );
	overlayTgtOut->setDefault( false );
	overlayTgtOut->setEvaluateOnChange( false );
	overlayTgtOut->setParent( groupOverlay );

	//////////////////// IN Points ////////////////////
	OFX::GroupParamDescriptor* groupIn = desc.defineGroupParam( kParamGroupIn );
	groupIn->setLabel( "Input points" );
	groupIn->setOpen( false );

	OFX::RGBParamDescriptor* ouverlayInColor = desc.defineRGBParam( kParamOverlayInColor );
	ouverlayInColor->setLabel( "Color" );
	ouverlayInColor->setHint( "Input point overlay color" );
	ouverlayInColor->setDefault( 1.0, 0.0, 0.0 );
	ouverlayInColor->setEvaluateOnChange( false );
	ouverlayInColor->setParent( groupIn );

	OFX::Double2DParamDescriptor * pIn[kMaxNbPoints];
	for( std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn )
	{
		std::string resultIn = boost::lexical_cast<std::string > ( cptIn );
		pIn[cptIn] = desc.defineDouble2DParam( kParamPointIn + resultIn );
		pIn[cptIn]->setLabel( "In " + resultIn );
		pIn[cptIn]->setHint( "Input point " + resultIn );
		pIn[cptIn]->setDefault( positionOrigine, positionOrigine );
		pIn[cptIn]->setParent( groupIn );
	}

	//////////////////// TGT Points IN////////////////////
	OFX::GroupParamDescriptor* groupTgtIn = desc.defineGroupParam( kParamGroupTgtIn );
	groupTgtIn->setLabel( "Tangente points In" );
	groupTgtIn->setOpen( false );

	OFX::RGBParamDescriptor* ouverlayTgtInColor = desc.defineRGBParam( kParamOverlayTgtInColor );
	ouverlayTgtInColor->setLabel( "Color In" );
	ouverlayTgtInColor->setHint( "Tangente point overlay In color" );
	ouverlayTgtInColor->setDefault( 0.95, 0.4, 0.4 );
	ouverlayTgtInColor->setEvaluateOnChange( false );
	ouverlayTgtInColor->setParent( groupTgtIn );

	OFX::Double2DParamDescriptor * pTgtIn[kMaxNbPoints * 2];
	for( std::size_t cptTgtIn = 0; cptTgtIn < kMaxNbPoints * 2; ++cptTgtIn )
	{
		std::string resultTgtIn = boost::lexical_cast<std::string > ( cptTgtIn );
		pTgtIn[cptTgtIn] = desc.defineDouble2DParam( kParamPointTgtIn + resultTgtIn );
		pTgtIn[cptTgtIn]->setLabel( "Tgt In " + resultTgtIn );
		pTgtIn[cptTgtIn]->setHint( "Tgt point In " + resultTgtIn );
		pTgtIn[cptTgtIn]->setDefault( positionOrigine, positionOrigine );
		pTgtIn[cptTgtIn]->setParent( groupTgtIn );
	}

        //////////////////// IN Points Curve2////////////////////
        /*OFX::GroupParamDescriptor* groupInCurve2 = desc.defineGroupParam( kParamGroupInCurve2 );
        groupInCurve2->setLabel( "Input points Curve2" );
        groupInCurve2->setOpen( false );

        OFX::RGBParamDescriptor* ouverlayInColorCurve2 = desc.defineRGBParam( kParamOverlayInColorCurve2 );
        ouverlayInColorCurve2->setLabel( "Color Curve2" );
        ouverlayInColorCurve2->setHint( "Input point overlay color Curve2" );
        ouverlayInColorCurve2->setDefault( 1.0, 0.0, 0.0 );
        ouverlayInColorCurve2->setEvaluateOnChange( false );
        ouverlayInColorCurve2->setParent( groupInCurve2 );

        OFX::Double2DParamDescriptor * pInCurve2[kMaxNbPoints];
        for( std::size_t cptInCurve2 = 0; cptInCurve2 < kMaxNbPoints/2; ++cptInCurve2 )
        {
                std::string resultInCurve2 = boost::lexical_cast<std::string > ( cptInCurve2 );
                pInCurve2[cptInCurve2] = desc.defineDouble2DParam( kParamPointIn + resultInCurve2 );
                pInCurve2[cptInCurve2]->setLabel( "In Curve2 " + resultInCurve2 );
                pInCurve2[cptInCurve2]->setHint( "Input point Curve2 " + resultInCurve2 );
                pInCurve2[cptInCurve2]->setDefault( positionOrigine, positionOrigine );
                pInCurve2[cptInCurve2]->setParent( groupInCurve2 );
        }

        //////////////////// TGT Points IN Curve2////////////////////
        OFX::GroupParamDescriptor* groupTgtInCurve2 = desc.defineGroupParam( kParamGroupTgtInCurve2 );
        groupTgtInCurve2->setLabel( "Tangente points In Curve2" );
        groupTgtInCurve2->setOpen( false );

        OFX::RGBParamDescriptor* ouverlayTgtInColorCurve2 = desc.defineRGBParam( kParamOverlayTgtInColorCurve2 );
        ouverlayTgtInColorCurve2->setLabel( "Color In Curve2" );
        ouverlayTgtInColorCurve2->setHint( "Tangente point overlay In color Curve2" );
        ouverlayTgtInColorCurve2->setDefault( 0.95, 0.4, 0.4 );
        ouverlayTgtInColorCurve2->setEvaluateOnChange( false );
        ouverlayTgtInColorCurve2->setParent( groupTgtInCurve2);

        OFX::Double2DParamDescriptor * pTgtInCurve2[kMaxNbPoints];
        for( std::size_t cptTgtInCurve2 = 0; cptTgtInCurve2 < kMaxNbPoints; ++cptTgtInCurve2 )
        {
                std::string resultTgtInCurve2 = boost::lexical_cast<std::string > ( cptTgtInCurve2 );
                pTgtInCurve2[cptTgtInCurve2] = desc.defineDouble2DParam( kParamPointTgtInCurve2 + resultTgtInCurve2 );
                pTgtInCurve2[cptTgtInCurve2]->setLabel( "Tgt In Curve2 " + resultTgtInCurve2 );
                pTgtInCurve2[cptTgtInCurve2]->setHint( "Tgt point In Curve2 " + resultTgtInCurve2 );
                pTgtInCurve2[cptTgtInCurve2]->setDefault( positionOrigine, positionOrigine );
                pTgtInCurve2[cptTgtInCurve2]->setParent( groupTgtInCurve2 );
        }*/

	//////////////////// OUT Points ////////////////////
	OFX::GroupParamDescriptor* groupOut = desc.defineGroupParam( kParamGroupOut );
	groupOut->setLabel( "Output points" );
	groupOut->setOpen( false );

	OFX::RGBParamDescriptor* ouverlayOutColor = desc.defineRGBParam( kParamOverlayOutColor );
	ouverlayOutColor->setLabel( "Color" );
	ouverlayOutColor->setHint( "Output point overlay color" );
	ouverlayOutColor->setDefault( 0.0, 0.0, 1.0 );
	ouverlayOutColor->setEvaluateOnChange( false );
	ouverlayOutColor->setParent( groupOut );

	OFX::Double2DParamDescriptor * pOut[kMaxNbPoints];
	for( std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut )
	{
		std::string resultOut = boost::lexical_cast<std::string > ( cptOut );
		pOut[cptOut] = desc.defineDouble2DParam( kParamPointOut + resultOut );
		pOut[cptOut]->setLabel( "In " + resultOut );
		pOut[cptOut]->setHint( "Input point " + resultOut );
		pOut[cptOut]->setDefault( positionOrigine, positionOrigine );
		pOut[cptOut]->setParent( groupOut );
	}

	//////////////////// TGT Points Out////////////////////
	OFX::GroupParamDescriptor* groupTgtOut = desc.defineGroupParam( kParamGroupTgtOut );
	groupTgtOut->setLabel( "Tangente points Out" );
	groupTgtOut->setOpen( false );

	OFX::RGBParamDescriptor* ouverlayTgtOutColor = desc.defineRGBParam( kParamOverlayTgtOutColor );
	ouverlayTgtOutColor->setLabel( "Color Out" );
	ouverlayTgtOutColor->setHint( "Tangente point overlay Out color" );
	ouverlayTgtOutColor->setDefault( 0.2, 0.45, 0.95 );
	ouverlayTgtOutColor->setEvaluateOnChange( false );
	ouverlayTgtOutColor->setParent( groupTgtOut );

	OFX::Double2DParamDescriptor * pTgtOut[kMaxNbPoints * 2];
	for( std::size_t cptTgtOut = 0; cptTgtOut < kMaxNbPoints * 2; ++cptTgtOut )
	{
		std::string resultTgtOut = boost::lexical_cast<std::string > ( cptTgtOut );
		pTgtOut[cptTgtOut] = desc.defineDouble2DParam( kParamPointTgtOut + resultTgtOut );
		pTgtOut[cptTgtOut]->setLabel( "Tgt Out " + resultTgtOut );
		pTgtOut[cptTgtOut]->setHint( "Tgt point Out " + resultTgtOut );
		pTgtOut[cptTgtOut]->setDefault( positionOrigine, positionOrigine );
		pTgtOut[cptTgtOut]->setParent( groupTgtOut );
	}

        //////////////////// OUT Points Curve2////////////////////
        /*OFX::GroupParamDescriptor* groupOutCurve2 = desc.defineGroupParam( kParamGroupOutCurve2 );
        groupOutCurve2->setLabel( "Output points Curve2" );
        groupOutCurve2->setOpen( false );

        OFX::RGBParamDescriptor* ouverlayOutColorCurve2 = desc.defineRGBParam( kParamOverlayOutColorCurve2 );
        ouverlayOutColorCurve2->setLabel( "Color Curve2" );
        ouverlayOutColorCurve2->setHint( "Output point overlay color Curve2" );
        ouverlayOutColorCurve2->setDefault( 0.0, 0.0, 1.0 );
        ouverlayOutColorCurve2->setEvaluateOnChange( false );
        ouverlayOutColorCurve2->setParent( groupOutCurve2 );

        OFX::Double2DParamDescriptor * pOutCurve2[kMaxNbPoints];
        for( std::size_t cptOutCurve2 = 0; cptOutCurve2 < kMaxNbPoints/2; ++cptOutCurve2 )
        {
                std::string resultOutCurve2 = boost::lexical_cast<std::string > ( cptOutCurve2 );
                pOutCurve2[cptOutCurve2] = desc.defineDouble2DParam( kParamPointOutCurve2 + resultOutCurve2 );
                pOutCurve2[cptOutCurve2]->setLabel( "In Curve2 " + resultOutCurve2 );
                pOutCurve2[cptOutCurve2]->setHint( "Input point Curve2 " + resultOutCurve2 );
                pOutCurve2[cptOutCurve2]->setDefault( positionOrigine, positionOrigine );
                pOutCurve2[cptOutCurve2]->setParent( groupOutCurve2 );
        }

        //////////////////// TGT Points Out Curve2 ////////////////////
        OFX::GroupParamDescriptor* groupTgtOutCurve2 = desc.defineGroupParam( kParamGroupTgtOutCurve2 );
        groupTgtOutCurve2->setLabel( "Tangente points Out Curve2" );
        groupTgtOutCurve2->setOpen( false );

        OFX::RGBParamDescriptor* ouverlayTgtOutColorCurve2 = desc.defineRGBParam( kParamOverlayTgtOutColorCurve2 );
        ouverlayTgtOutColorCurve2->setLabel( "Color Out Curve2" );
        ouverlayTgtOutColorCurve2->setHint( "Tangente point overlay Out color Curve2" );
        ouverlayTgtOutColorCurve2->setDefault( 0.2, 0.45, 0.95 );
        ouverlayTgtOutColorCurve2->setEvaluateOnChange( false );
        ouverlayTgtOutColorCurve2->setParent( groupTgtOutCurve2 );

        OFX::Double2DParamDescriptor * pTgtOutCurve2[kMaxNbPoints];
        for( std::size_t cptTgtOutCurve2 = 0; cptTgtOutCurve2 < kMaxNbPoints; ++cptTgtOutCurve2 )
        {
                std::string resultTgtOutCurve2 = boost::lexical_cast<std::string > ( cptTgtOutCurve2 );
                pTgtOutCurve2[cptTgtOutCurve2] = desc.defineDouble2DParam( kParamPointTgtOutCurve2 + resultTgtOutCurve2 );
                pTgtOutCurve2[cptTgtOutCurve2]->setLabel( "Tgt Out Curve2 " + resultTgtOutCurve2 );
                pTgtOutCurve2[cptTgtOutCurve2]->setHint( "Tgt point Out Curve2 " + resultTgtOutCurve2 );
                pTgtOutCurve2[cptTgtOutCurve2]->setDefault( positionOrigine, positionOrigine );
                pTgtOutCurve2[cptTgtOutCurve2]->setParent( groupTgtOutCurve2 );
        }*/
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* WarpPluginFactory::createInstance( OfxImageEffectHandle handle,
													 OFX::EContext context )
{
	return new WarpPlugin( handle );
}

}
}
}

