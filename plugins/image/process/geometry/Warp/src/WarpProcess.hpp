#ifndef _TUTTLE_PLUGIN_WARP_PROCESS_HPP_
#define _TUTTLE_PLUGIN_WARP_PROCESS_HPP_

#include "TPS/tps.hpp"

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

/**
 * @brief Warp process
 *
 */
template<class View>
class WarpProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;

	typedef WarpPlugin::Scalar Scalar;

protected :
    WarpPlugin&    _plugin;            ///< Rendering plugin
	WarpProcessParams<Scalar> _params; ///< parameters
	TPS_Morpher<Scalar> _tps;

public:
    WarpProcess( WarpPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "WarpProcess.tcc"

#endif