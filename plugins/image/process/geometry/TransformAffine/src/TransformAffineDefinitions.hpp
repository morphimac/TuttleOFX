#ifndef _TUTTLE_PLUGIN_TRANSFORMAFFINE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TRANSFORMAFFINE_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace transformAffine {


const static std::string kParamGroupSource = "gpSource";
const static std::string kParamPointSource = "pEntree";
const static std::string kParamGroupSortie = "gpSortie";
const static std::string kParamPointSortie = "pSortie";

const std::string kClipOptionalSourceRef( "SourceRef" );
const std::string kParamDisplayOptions( "displayOptions" );
const std::string kParamGridOverlay( "gridOverlay" );
const std::string kParamGridCenter( "gridCenter" );
const std::string kParamGridCenterOverlay( "gridCenterOverlay" );
const std::string kParamGridScale( "gridScale" );
const std::string kParamCenter( "center" );
const std::string kParamCenterOverlay( "lensCenterOverlay" );


}
}
}

#endif
