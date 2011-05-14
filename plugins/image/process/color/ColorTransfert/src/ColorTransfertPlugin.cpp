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

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorTransfertPlugin::render( const OFX::RenderArguments &args )
{
//	doGilRender<ColorTransfertProcess>( *this, args );

	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<ColorTransfertProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<ColorTransfertProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}


}
}
}
