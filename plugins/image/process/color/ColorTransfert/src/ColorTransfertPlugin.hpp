#ifndef _TUTTLE_PLUGIN_COLORTRANSFERT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFERT_PLUGIN_HPP_

#include "ColorTransfertDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

template<typename Scalar>
struct ColorTransfertProcessParams
{
	
};

/**
 * @brief ColorTransfert plugin
 */
class ColorTransfertPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    ColorTransfertPlugin( OfxImageEffectHandle handle );

public:
	ColorTransfertProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
        OFX::Clip* _clipSrc; ///< Source image clip 1
        OFX::Clip* _clipSrcRef; ///< Source image clip 2
        OFX::Clip* _clipDstRef; ///< Source image clip 3

        OFX::DoubleParam* _paramWeight; ///< Paramètre weight

};

}
}
}

#endif
