#ifndef _TUTTLE_PLUGIN_WARP_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_WARP_ALGORITHM_HPP_

#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <boost/gil/extension/channel.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

namespace tuttle {
namespace plugin {
namespace warp {

template<typename Pixel>
struct pixel_merge_t
{
   const double _ratioA;
   const double _ratioB;

   pixel_merge_t( const double ratio )
   : _ratioA(ratio),
    _ratioB(1.0-ratio)
   {
   }

   GIL_FORCEINLINE
   Pixel operator()( const Pixel& srcA, const Pixel& srcB ) const
   {
       using namespace boost::gil;
       Pixel res;
       // res = (srcA * _ratioA) + (srcB * _ratioB)
       pixel_assigns_t<Pixel,Pixel>()(
           pixel_plus_t<Pixel,Pixel,Pixel>()(
               pixel_multiplies_scalar_t<Pixel,double,Pixel>()(
                   srcA,
                   _ratioA
                   ),
               pixel_multiplies_scalar_t<Pixel,double,Pixel>()(
                   srcB,
                   _ratioB
                   )
               ),
           res );
       return res;
   }
};

}
}
}

#endif
