#include "ColorTransfertPlugin.hpp"
#include "ColorTransfertProcess.hpp"
#include "ColorTransfertDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {


ColorTransfertPlugin::ColorTransfertPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
        _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
}

ColorTransfertProcessParams<ColorTransfertPlugin::Scalar> ColorTransfertPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
        ColorTransfertProcessParams<Scalar> params;
        return params;
}

void ColorTransfertPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//    if( paramName == kParamHelpButton )
//    {
//        sendMessage( OFX::Message::eMessageMessage,
//                     "", // No XML resources
//                     kParamHelpString );
//    }

}

//bool ColorTransfertPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	ColorTransfertProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void ColorTransfertPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	ColorTransfertProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool ColorTransfertPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	ColorTransfertProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorTransfertPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ColorTransfertProcess>( *this, args );
}


}
}
}
