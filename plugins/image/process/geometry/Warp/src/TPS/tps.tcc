#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"
#include "tps.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/math/special_functions/pow.hpp>

#include <vector>
#include <ostream>

namespace tuttle {
namespace plugin {
namespace warp {

template<class Mat>
inline std::ostream& coutMat( std::ostream& os, const Mat& m )
{
	os << "[" << m.size1() << "," << m.size2() << "]\n";
	for( std::size_t y = 0; y < m.size1(); ++y )
	{
		os << "(";
		for( std::size_t x = 0; x < m.size2(); ++x )
		{
			os << boost::format("%4.3d") % m(y,x);
			if( x < m.size2() - 1 )
			{
				os << ",\t";
			}
		}
		os << ")";
		if( y < m.size1() - 1 )
		{
			os << ",";
		}
		os << "\n";
	}
	return os;
}

inline double base_func( const double r2 )
{
	if( r2 == 0 )
		return 0.0;
	return r2 * log( r2 ) * 0.217147241;
}

template<typename SCALAR>
TPS_Morpher<SCALAR>::TPS_Morpher()
{}


/**
 *
 * @param pIn
 * @param pOut
 * @param regularization Amount of "relaxation", 0.0 = exact interpolation
 * @param applyWarp
 * @param nbPoints
 */
template<typename SCALAR>
void TPS_Morpher<SCALAR>::setup( const std::vector< Point2 > pIn, const std::vector< Point2 > pOut, const double regularization, const bool applyWarp, const std::size_t nbPoints )
{
	using boost::math::pow;
	
	BOOST_ASSERT( _pIn.size() == _pOut.size() );
	BOOST_ASSERT( _pIn.size() == nbPoints );

	_pIn = pIn;
	_pOut = pOut;
	_mat_L.resize( nbPoints + 3, nbPoints + 3 );
	_mat_V.resize( nbPoints + 3, 2 );
	_mat_K.resize( nbPoints, nbPoints );
	_nbPoints = nbPoints;
	_activateWarp = applyWarp;

	TUTTLE_TCOUT_VAR( _pIn.size() );
	TUTTLE_TCOUT_VAR( _pOut.size() );
	TUTTLE_TCOUT_VAR( _nbPoints );

	// Fill K and directly copy values into L
	// K = [ 0      U(r01) ...   U(r0n) ]
	//   = [ U(r10) 0      ...   U(r1n) ]
	//   = [ ...    ...    ...   ...    ]
	//   = [ U(rn0) U(rn1) ...   0      ]
	//
	// K.size = n x n
	for( std::size_t y = 0; y < nbPoints; ++y )
	{
		const Point2& point_i = _pIn[y];
		for( std::size_t x = 0; x < nbPoints; ++x )
		{
			if( y == x )
			{
				_mat_L( y, x ) = _mat_K( y, x ) = 0;
				// diagonal: reqularization parameters (lambda * a^2)
//				_mat_L( i, i ) = _mat_K( i, i ) = regularization /** (a*a)*/;
			}
			else
			{
				const Point2& point_j = _pIn[x];
				const Scalar sum = pow<2>( point_i.x - point_j.x ) + pow<2>( point_i.y - point_j.y );
				_mat_L( y, x ) = _mat_K( y, x ) = base_func( sum );
			}
		}
	}

	// fill L with P and trans(P)
	//
	// P = [ 1   x1   y1  ]
	//     [ 1   x2   y2  ]
	//     [ ... ...  ... ]
	//     [ 1   xn   yn  ]
	//
	// P.size = n x 3
	for( std::size_t i = 0; i < nbPoints; ++i )
	{
		const Point2& pt = _pIn[i];
		_mat_L( i, nbPoints + 0 ) = 1.0;
		_mat_L( i, nbPoints + 1 ) = pt.x;
		_mat_L( i, nbPoints + 2 ) = pt.y;

		_mat_L( nbPoints + 0, i ) = 1.0;
		_mat_L( nbPoints + 1, i ) = pt.x;
		_mat_L( nbPoints + 2, i ) = pt.y;
	}

	// L = [ K        |  P  ]
	//     [          | 000 ]
	//     [ trans(P) | 000 ]
	//     [          | 000 ]
	//
	// L.size = (n+3) x (n+3)
	for( std::size_t i = 0; i < 3; ++i )
	{
		for( std::size_t j = 0; j < 3; ++j )
		{
			_mat_L( nbPoints+i, nbPoints+j ) = 0.0;
		}
	}

	// Fill V
	// V = [ x1' x2' ... xn' 000 ]
	//     [ y1' y2' ... yn' 000 ]
	//
	// V.size = 2 x (n+3)
	// here we manipulate trans(V)
	for( std::size_t i = 0; i < nbPoints; ++i )
	{
		const Point2& pIn = _pIn[i];
		const Point2& pOut = _pOut[i];
		_mat_V( i, 0 ) = pOut.x - pIn.x;
		_mat_V( i, 1 ) = pOut.y - pIn.y;
		TUTTLE_TCOUT_VAR2( pOut.x, pIn.x );
		TUTTLE_TCOUT_VAR2( pOut.y, pIn.y );
	}

	_mat_V( nbPoints + 0, 0 ) = _mat_V( nbPoints + 1, 0 ) = _mat_V( nbPoints + 2, 0 ) = 0.0;
	_mat_V( nbPoints + 0, 1 ) = _mat_V( nbPoints + 1, 1 ) = _mat_V( nbPoints + 2, 1 ) = 0.0;

	TUTTLE_TCOUT("");
	TUTTLE_TCOUT( "mtx_v" );
	coutMat( std::cout, _mat_V );
	TUTTLE_TCOUT("");
	TUTTLE_TCOUT( "mtx_orig_k" );
	coutMat( std::cout, _mat_K );
	TUTTLE_TCOUT("");
	TUTTLE_TCOUT( "mtx_l" );
	coutMat( std::cout, _mat_L );
	TUTTLE_TCOUT("");

	// Solve the linear system "inplace"
	permutation_matrix<Scalar> P( nbPoints + 3 );
//	matrix<Scalar> x( nbPoints + 3, 2 );

	lu_factorize( _mat_L, P );
	lu_substitute( _mat_L, P, _mat_V );

	TUTTLE_TCOUT_X( 10, "-");
	TUTTLE_TCOUT( "mtx_v" );
	coutMat( std::cout, _mat_V );
	TUTTLE_TCOUT("");
	TUTTLE_TCOUT( "mtx_l" );
	coutMat( std::cout, _mat_L );
	TUTTLE_TCOUT("");
	TUTTLE_TCOUT_X( 80, "_");
}

template<typename SCALAR>
template<typename S2>
typename TPS_Morpher<SCALAR>::Point2 TPS_Morpher<SCALAR>::operator( )(const point2<S2>& pt ) const
{
	using boost::math::pow;
	if( !_activateWarp || _nbPoints <= 1 )
	{
		return Point2( pt.x, pt.y );
	}
	
	// Nombre de colonnes de la matrice K
	const std::size_t m = _mat_K.size2( );

	double dx = _mat_V( m + 0, 0 ) + _mat_V( m + 1, 0 ) * pt.x + _mat_V( m + 2, 0 ) * pt.y;
	double dy = _mat_V( m + 0, 1 ) + _mat_V( m + 1, 1 ) * pt.x + _mat_V( m + 2, 1 ) * pt.y;

	//std::cout<<"Point "<<pt.x<<"  "<<pt.y<<std::endl;
	//std::cout<<"DX debut "<<dx<<"  "<<dy<<std::endl;

	Const_Matrix_Col mat_Vx( _mat_V, 0 );
	Const_Matrix_Col mat_Vy( _mat_V, 1 );

	std::vector< point2<double> >::const_iterator it_out = _pOut.begin();
	typename Const_Matrix_Col::const_iterator it_Vx( mat_Vx.begin() );
	typename Const_Matrix_Col::const_iterator it_Vy( mat_Vy.begin() );

	//std::cout<<"DX fin "<<dx<<"  "<<dy<<std::endl;

	for( std::size_t i = 0; i < m; ++i, ++it_out, ++it_Vx, ++it_Vy )
	{
		const double d = base_func( pow<2>( it_out->x - pt.x ) + pow<2>( it_out->y - pt.y ) );
		dx += ( *it_Vx ) * d;
		dy += ( *it_Vy ) * d;
	}

	//std::cout<<"DX fin "<<dx<<"  "<<dy<<std::endl;

	return Point2( pt.x+dx, pt.y+dy );
}

}
}
}


