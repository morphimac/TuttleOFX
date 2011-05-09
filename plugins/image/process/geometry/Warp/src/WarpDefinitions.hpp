#ifndef _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace warp {

const static std::size_t kMaxNbPoints = 10;
//const static std::size_t kPasBezier = 0.2;
static const float lineWidth = 0.5;
static const float pointWidth = 3.0;
static const int seuil = 15;

static const float positionOrigine = -200.0;

//static const int nbCoeffBezier = 50;

//const static std::string kClipSourceA = "A";
const static std::string kClipSourceB = "B";

const static std::string kParamMethod = "method";
const static std::string kParamMethodCreation = "creation";
const static std::string kParamMethodDelete = "delete";
const static std::string kParamMethodMove = "move";
enum EParamMethod
{
        eParamMethodCreation = 0,
        eParamMethodDelete,
        eParamMethodMove,

};

const static std::string kParamCurve = "curve";
const static std::string kParamCurve1 = "curve1";
const static std::string kParamCurve2 = "curve2";
enum EParamCurve
{
        eParamCurve1 = 0,
        eParamCurve2,
};

const static std::string kParamOverlay = "overlay";
const static std::string kParamInverse = "inverse";
const static std::string kParamNbPoints = "nbPoints";
//const static std::string kParamNbPointsCurve2 = "nbPointsCurve2";
const static std::string kParamTransition = "transition";
const static std::string kParamReset = "reset";
const static std::string kParamSetKey = "setKey";

const static std::string kParamGroupSettings = "settings";
const static std::string kParamNbPointsBezier = "Points Bezier";
const static std::string kParamRigiditeTPS = "Rigidite TPS";

const static std::string kParamGroupIn = "groupIn";
const static std::string kParamPointIn = "pIn";
const static std::string kParamOverlayIn = "overlayIn";
const static std::string kParamOverlayInColor = "overlayInColor";

const static std::string kParamGroupOut = "groupOut";
const static std::string kParamPointOut = "pOut";
const static std::string kParamOverlayOut = "overlayOut";
const static std::string kParamOverlayOutColor = "overlayOutColor";

const static std::string kParamGroupTgtIn = "groupTgtIn";
const static std::string kParamPointTgtIn = "pTangenteIn";
const static std::string kParamOverlayTgtIn = "overlayTgtIn";
const static std::string kParamOverlayTgtInColor = "overlayTangentInColor";

const static std::string kParamGroupTgtOut = "groupTgtOut";
const static std::string kParamPointTgtOut = "pTangenteOut";
const static std::string kParamOverlayTgtOut = "overlayTgtOut";
const static std::string kParamOverlayTgtOutColor = "overlayTangentOutColor";

/*const static std::string kParamGroupInCurve2 = "groupInCurve2";
const static std::string kParamPointInCurve2 = "pInCurve2";
const static std::string kParamOverlayInCurve2 = "overlayInCurve2";
const static std::string kParamOverlayInColorCurve2 = "overlayInColorCurve2";

const static std::string kParamGroupOutCurve2 = "groupOutCurve2";
const static std::string kParamPointOutCurve2 = "pOutCurve2";
const static std::string kParamOverlayOutCurve2 = "overlayOutCurve2";
const static std::string kParamOverlayOutColorCurve2 = "overlayOutColorCurve2";

const static std::string kParamGroupTgtInCurve2 = "groupTgtInCurve2";
const static std::string kParamPointTgtInCurve2 = "pTangenteInCurve2";
const static std::string kParamOverlayTgtInCurve2 = "overlayTgtInCurve2";
const static std::string kParamOverlayTgtInColorCurve2 = "overlayTangentInColorCurve2";

const static std::string kParamGroupTgtOutCurve2 = "groupTgtOutCurve2";
const static std::string kParamPointTgtOutCurve2 = "pTangenteOutCurve2";
const static std::string kParamOverlayTgtOutCurve2 = "overlayTgtOutCurve2";
const static std::string kParamOverlayTgtOutColorCurve2 = "overlayTangentOutColorCurve2s";*/

const static std::string kParamGroupOverlay = "groupOverlay";

}
}
}

#endif
