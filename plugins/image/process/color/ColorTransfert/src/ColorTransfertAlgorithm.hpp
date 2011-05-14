#ifndef _TUTTLE_PLUGIN_COLORTRANSFERT_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFERT_ALGORITHM_HPP_

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_minmax.hpp>
//#include <boost/gil/extension/color/hsl.hpp>
//#include <boost/gil/extension/color/distribution.hpp>
#include <boost/gil/extension/typedefs.hpp>
#include <boost/gil/extension/channel.hpp>

#include <boost/numeric/ublas/matrix.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

using namespace boost::gil;

template <typename Channel, typename ChannelR>
struct channel_sqrt_t : public std::unary_function<Channel, ChannelR>
{

	GIL_FORCEINLINE
	ChannelR operator( )( typename channel_traits<Channel>::const_reference ch ) const
	{
		return std::sqrt( ChannelR( ch ) );
	}
};

template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
struct pixel_sqrt_t
{

	GIL_FORCEINLINE
	PixelR operator ( ) (const PixelRef & p ) const
	{
		PixelR result;
		static_transform( p, result, channel_sqrt_t<typename channel_type<PixelRef>::type, typename channel_type<PixelR>::type > ( ) );
		return result;
	}
};

template <typename Channel, typename ChannelR>
struct channel_log_t : public std::unary_function<Channel, ChannelR>
{

	GIL_FORCEINLINE
	ChannelR operator( )( typename channel_traits<Channel>::const_reference ch ) const
	{
		return std::log( ChannelR( ch ) );
	}
};

template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
struct pixel_log_t
{

	GIL_FORCEINLINE
	PixelR operator ( ) (const PixelRef & p ) const
	{
		PixelR result;
		static_transform( p, result, channel_log_t<typename channel_type<PixelRef>::type, typename channel_type<PixelR>::type > ( ) );
		return result;
	}
};

template <typename Channel, typename ChannelR>
struct channel_value_pow_t : public std::unary_function<Channel, ChannelR>
{

	Channel _value;

	channel_value_pow_t( const Channel value )
	: _value( value ) { }

	GIL_FORCEINLINE
	ChannelR operator( )( typename channel_traits<Channel>::const_reference ch ) const
	{
		return std::pow( _value, ChannelR( ch ) );
	}
};

template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
struct pixel_value_pow_t
{

	typedef typename channel_type<PixelR>::type ChannelR;
	ChannelR _value;

	pixel_value_pow_t( const ChannelR value )
	: _value( value ) { }

	GIL_FORCEINLINE
	PixelR operator ( ) (const PixelRef & p ) const
	{
		PixelR result;
		static_transform( p, result, channel_value_pow_t<typename channel_type<PixelRef>::type, typename channel_type<PixelR>::type > ( _value ) );
		return result;
	}
};

template <typename PixelRef, typename Matrix, typename PixelR = PixelRef> // models pixel concept
struct pixel_matrix33_multiply_t
{
	typedef typename channel_type<PixelR>::type ChannelR;
	const Matrix _matrix;

	pixel_matrix33_multiply_t( const Matrix& m )
	: _matrix(m)
	{}

	GIL_FORCEINLINE
	PixelR operator ( ) ( const PixelRef & p ) const
	{
		PixelR result = p;
		result[0] = _matrix( 0, 0 ) * p[0] + _matrix( 0, 1 ) * p[1] + _matrix( 0, 2 ) * p[2];
		result[1] = _matrix( 1, 0 ) * p[0] + _matrix( 1, 1 ) * p[1] + _matrix( 1, 2 ) * p[2];
		result[2] = _matrix( 2, 0 ) * p[0] + _matrix( 2, 1 ) * p[1] + _matrix( 2, 2 ) * p[2];
		return result;
	}
};

namespace detail {
template<typename T>
struct matrices_rgb_lab_t
{
	typedef boost::numeric::ublas::bounded_matrix<T, 3, 3 > Matrix33;
	Matrix33 _RGB_2_XYZ;
	Matrix33 _XYZ_2_LAB;
	Matrix33 _LAB_2_XYZ;
	Matrix33 _XYZ_2_RGB;

	matrices_rgb_lab_t()
	{
		_RGB_2_XYZ(0, 0) = 1.0;
		_RGB_2_XYZ(0, 1) = 1.0;
		_RGB_2_XYZ(0, 2) = 1.0;
		_RGB_2_XYZ(1, 0) = 1.0;
		// ...
		/// @todo mel
	}
};
}

template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
struct pixel_rgb_to_lab_t
{
	typedef typename channel_type<PixelR>::type ChannelR;
	typedef typename detail::matrices_rgb_lab_t<ChannelR> MatrixContants;
	typedef typename MatrixContants::Matrix33 Matrix;

	static const MatrixContants _matrices;

	GIL_FORCEINLINE
	PixelR operator ( ) (const PixelRef & p ) const
	{
		PixelR rgb;
		pixel_assigns_t<PixelRef, PixelR> ( )( p, rgb );
		// RGB to XYZ
		PixelR xyz = pixel_matrix33_multiply_t<PixelRef, Matrix>( _matrices._RGB_2_XYZ )( rgb );
		// log(v)
		PixelR xyz_log = pixel_log_t<PixelR, PixelR>()( xyz );
		// XYZ to LAB (lambda alpha beta)
		PixelR lab = pixel_matrix33_multiply_t<PixelRef, Matrix>( _matrices._XYZ_2_LAB )( xyz_log );
		return lab;
	}
};

template <typename PixelRef, typename PixelR>
const typename pixel_rgb_to_lab_t<PixelRef, PixelR>::MatrixContants pixel_rgb_to_lab_t<PixelRef, PixelR>::_matrices; // init static variable.


/// @todo mel
//template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
//struct pixel_lab_to_rgb_t
//{
		// LAB to XYZ
		// pow(10, v)
		// XYZ to RGB
//};

}
}
}

#endif
