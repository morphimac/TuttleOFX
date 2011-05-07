#ifndef _TUTTLE_PLUGIN_COLORTRANSFERT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFERT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

/**
 * @brief ColorTransfert process
 *
 */
using namespace boost::numeric::ublas;
template<class View>
class ColorTransfertProcess : public ImageGilFilterProcessor<View>
{
public:
        typedef typename View::value_type Pixel;
        typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;

        OFX::Clip* _clipSrcRef;       ///< Source ref image clip
        boost::scoped_ptr<OFX::Image> _srcRef;
        OfxRectI _srcRefPixelRod;
        View _srcRefView;


        OFX::Clip* _clipDstRef;       ///< Dst ref image clip
        boost::scoped_ptr<OFX::Image> _dstRef;
        OfxRectI _dstRefPixelRod;
        View _dstRefView;

protected:
        ColorTransfertPlugin&    _plugin;            ///< Rendering plugin
        ColorTransfertProcessParams<Scalar> _params; ///< parameters

        Pixel _srcRefAverage, _dstRefAverage, _srcRefDeviation, _dstRefDeviation;

public:
         ColorTransfertProcess( ColorTransfertPlugin& effect );

	void setup( const OFX::RenderArguments& args );

        void multiThreadProcessImages( const OfxRectI& procWindowRoW );

private:
        void computeAverage( const View& image, Pixel& average, Pixel& deviation );

};

}
}
}

#include "ColorTransfertProcess.tcc"

#endif
