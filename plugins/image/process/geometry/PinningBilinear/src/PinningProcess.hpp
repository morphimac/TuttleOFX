#ifndef _TUTTLE_PLUGIN_PINNING_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PINNING_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {

namespace bgil = boost::gil;

/**
 * @brief Pinning process
 *
 */
template<class View>
class PinningProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;
protected :
	PinningPlugin&    _plugin;        ///< Rendering plugin

public:
    PinningProcess( PinningPlugin& effect );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "PinningProcess.tcc"

#endif
