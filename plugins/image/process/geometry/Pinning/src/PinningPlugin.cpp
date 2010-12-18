#include "PinningPlugin.hpp"
#include "PinningProcess.hpp"
#include "PinningDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace tuttle {
namespace plugin {
namespace pinning {

PinningPlugin::PinningPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramMethod = fetchChoiceParam( kParamMethod );
	_paramInterpolation = fetchChoiceParam( kParamInterpolation );
	_paramOverlay = fetchBooleanParam( kParamOverlay );

	_paramGroupIn = fetchGroupParam( kParamGroupIn );
	_paramPointIn0 = fetchDouble2DParam( kParamPointIn + "0" );
	_paramPointIn1 = fetchDouble2DParam( kParamPointIn + "1" );
	_paramPointIn2 = fetchDouble2DParam( kParamPointIn + "2" );
	_paramPointIn3 = fetchDouble2DParam( kParamPointIn + "3" );
	_paramOverlayIn = fetchBooleanParam( kParamOverlayIn );
	_paramOverlayInColor = fetchRGBParam( kParamOverlayInColor );

	_paramGroupOut = fetchGroupParam( kParamGroupIn );
	_paramPointOut0 = fetchDouble2DParam( kParamPointOut + "0" );
	_paramPointOut1 = fetchDouble2DParam( kParamPointOut + "1" );
	_paramPointOut2 = fetchDouble2DParam( kParamPointOut + "2" );
	_paramPointOut3 = fetchDouble2DParam( kParamPointOut + "3" );
	_paramOverlayOut = fetchBooleanParam( kParamOverlayOut );
	_paramOverlayOutColor = fetchRGBParam( kParamOverlayOutColor );

	_paramGroupPerspMatrix = fetchGroupParam( kParamGroupPerspMatrix );
	_paramPerspMatrixRow0 = fetchDouble3DParam( kParamPerspMatrixRow + "0" );
	_paramPerspMatrixRow1 = fetchDouble3DParam( kParamPerspMatrixRow + "1" );
	_paramPerspMatrixRow2 = fetchDouble3DParam( kParamPerspMatrixRow + "2" );

	_paramGroupBilMatrix = fetchGroupParam( kParamGroupBilinearMatrix );
	_paramBilMatrixRow0 = fetchDouble2DParam( kParamBilinearMatrixRow + "0" );
	_paramBilMatrixRow1 = fetchDouble2DParam( kParamBilinearMatrixRow + "1" );
	_paramBilMatrixRow2 = fetchDouble2DParam( kParamBilinearMatrixRow + "2" );
	_paramBilMatrixRow3 = fetchDouble2DParam( kParamBilinearMatrixRow + "3" );

	changedParam( OFX::InstanceChangedArgs( ), kParamMethod );
}

PinningProcessParams<PinningPlugin::Scalar> PinningPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	using namespace boost::numeric::ublas;
	PinningProcessParams<Scalar> params;

	// persp matrix
	bounded_matrix<double, 3, 3 > pm;
	_paramPerspMatrixRow0->getValue( pm( 0, 0 ), pm( 0, 1 ), pm( 0, 2 ) );
	_paramPerspMatrixRow1->getValue( pm( 1, 0 ), pm( 1, 1 ), pm( 1, 2 ) );
	_paramPerspMatrixRow2->getValue( pm( 2, 0 ), pm( 2, 1 ), pm( 2, 2 ) );
	params._perspective._matrix = pm;

	// bilinear matrix
	bounded_matrix<double, 2, 4 > bm;
	_paramBilMatrixRow0->getValue( bm( 0, 0 ), bm( 1, 0 ) );
	_paramBilMatrixRow1->getValue( bm( 0, 1 ), bm( 1, 1 ) );
	_paramBilMatrixRow2->getValue( bm( 0, 2 ), bm( 1, 2 ) );
	_paramBilMatrixRow3->getValue( bm( 0, 3 ), bm( 1, 3 ) );
	params._bilinear._matrix = bm;

	params._method = static_cast < EParamMethod > ( _paramMethod->getValue( ) );
	params._interpolation = static_cast < EParamInterpolation > ( _paramInterpolation->getValue( ) );

	return params;
}

void PinningPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	using namespace boost::numeric::ublas;
	if( paramName == kParamMethod )
	{
		bool bil = false;
		bool persp = false;
		bool fourPoints = false;

		switch( static_cast<EParamMethod>( _paramMethod->getValue( ) ) )
		{
			case eParamMethodAffine:
			{
				persp = true;
				break;
			}
			case eParamMethodPerspective:
			{
				fourPoints = true;
				persp = true;
				break;
			}
			case eParamMethodBilinear:
			{
				fourPoints = true;
				bil = true;
				break;
			}
		}
		_paramPointIn3->setIsSecretAndDisabled( !fourPoints );
		_paramPointOut3->setIsSecretAndDisabled( !fourPoints );

		_paramGroupPerspMatrix->setIsSecretAndDisabled( !persp );
		_paramPerspMatrixRow0->setIsSecretAndDisabled( !persp );
		_paramPerspMatrixRow1->setIsSecretAndDisabled( !persp );
		_paramPerspMatrixRow2->setIsSecretAndDisabled( !persp );

		_paramGroupBilMatrix->setIsSecretAndDisabled( !bil );
		_paramBilMatrixRow0->setIsSecretAndDisabled( !bil );
		_paramBilMatrixRow1->setIsSecretAndDisabled( !bil );
		_paramBilMatrixRow2->setIsSecretAndDisabled( !bil );
		_paramBilMatrixRow3->setIsSecretAndDisabled( !bil );

		// recompute the matrix
		changedParam( args, kParamPointIn );
	}
	else if( boost::starts_with( paramName, kParamPointIn ) ||
			 boost::starts_with( paramName, kParamPointOut ) )
	{
		switch( static_cast < EParamMethod > ( _paramMethod->getValue( ) ) )
		{
			case eParamMethodAffine:
			{
				bounded_matrix<Scalar, 3, 3 > perspMatrix;

				////////////////////////////////////////
				/// @todo melanie: compute "perspMatrix" from input/output points -> "_paramPoint*"
				perspMatrix = identity_matrix<Scalar > ( 3 );


                                /* Calculates coefficients of affine transformation
                                  * which maps (xi,yi) to (ui,vi), (i=1,2,3):
                                  *
                                  * ui = c00*xi + c01*yi + c02
                                  *
                                  * vi = c10*xi + c11*yi + c12
                                  *
                                  * Coefficients are calculated by solving linear system:
                                  * / x0 y0  1  0  0  0 \ /c00\ /u0\
                                  * | x1 y1  1  0  0  0 | |c01| |u1|
                                  * | x2 y2  1  0  0  0 | |c02| |u2|
                                  * |  0  0  0 x0 y0  1 | |c10| |v0|
                                  * |  0  0  0 x1 y1  1 | |c11| |v1|
                                  * \  0  0  0 x2 y2  1 / |c12| |v2|
                                  *
                                  * where:
                                  *   cij - matrix coefficients
                                  */
                                /*
                                Mat getAffineTransform( const Point2f src[], const Point2f dst[] )
                                {
                                    //Lignes, colonnes, type de données, valeur de remplissage
                                     Mat M(2, 3, CV_64F), X(6, 1, CV_64F, M.data);
                                     double a[6*6], b[6];
                                     Mat A(6, 6, CV_64F, a), B(6, 1, CV_64F, b);

                                     for( int i = 0; i < 3; i++ )
                                     {
                                         int j = i*12;
                                         int k = i*12+6;
                                         a[j] = a[k+3] = src[i].x;
                                         a[j+1] = a[k+4] = src[i].y;
                                         a[j+2] = a[k+5] = 1;
                                         a[j+3] = a[j+4] = a[j+5] = 0;
                                         a[k] = a[k+1] = a[k+2] = 0;
                                         b[i*2] = dst[i].x;
                                         b[i*2+1] = dst[i].y;
                                     }

                                     solve( A, B, X );
                                     return M;
                                }*/
				////////////////////////////////////////

/*
                                // Matrice points d'entrée
                                bounded_matrix<Scalar, 6, 6 > A;

                                // Vecteur solution
                                bounded_matrix<Scalar, 6, 1 > X;

                                // Vecteur points de sortie
                                bounded_matrix<Scalar, 3, 3 > B;

                                for( int i = 0; i < 3; ++i ){
                                    int j = i*12;
                                    int k = i*12+6;
                                    a[j] = a[k+3] = src[i].x;
                                    a[j+1] = a[k+4] = src[i].y;
                                    a[j+2] = a[k+5] = 1;
                                    a[j+3] = a[j+4] = a[j+5] = 0;
                                    a[k] = a[k+1] = a[k+2] = 0;
                                    b[i*2] = dst[i].x;
                                    b[i*2+1] = dst[i].y;
                                }
*/

                                const int n=6;
                                permutation_matrix<double> P(n);
                                matrix<double> A(n,n);
                                vector<double> x(n);
                                vector<double> b(n);

                                bounded_vector<double, 2> pIn0, pIn1, pIn2;
                                _paramPointIn0->getValue( pIn0[0], pIn0[1] );
                                _paramPointIn1->getValue( pIn1[0], pIn1[1] );
                                _paramPointIn2->getValue( pIn2[0], pIn2[1] );


                                bounded_vector<double, 2> pOut0, pOut1, pOut2;
                                _paramPointOut0->getValue( pOut0[0], pOut0[1] );
                                _paramPointOut1->getValue( pOut1[0], pOut1[1] );
                                _paramPointOut2->getValue( pOut2[0], pOut2[1] );

                                /////////////////////
                                // fill A and b... //


                                for( int i = 0; i < 3; ++i ){
                                    if(i == 0){
                                        for( int j = 0; j < 3; ++j ){
                                            if(j == 2){
                                                A(i, j) = A(i + 3, j + 3) = 1;
                                            }
                                            else{
                                                A(i, j) = A(i + 3, j + 3) = pIn0[j];
                                            }
                                            A(i , j + 3) = A(i + 3, j) = 0;
                                        }
                                        b(i) = pOut0[0];
                                        b(i + 3) = pOut0[1];
                                    }

                                    else if(i == 1){
                                        for( int j = 0; j < 3; ++j ){
                                            if(j == 2){
                                                A(i, j) = A(i + 3, j + 3) = 1;
                                            }
                                            else{
                                                A(i, j) = A(i + 3, j + 3) = pIn1[j];
                                            }
                                            A(i, j + 3) = A(i + 3, j) = 0;
                                        }
                                        b(i) = pOut1[0];
                                        b(i + 3) = pOut1[1];
                                    }

                                    else{
                                        for( int j = 0; j < 3; ++j ){
                                            if(j == 2){
                                                A( i, j ) = A( i + 3, j + 3 ) = 1;
                                            }
                                            else{
                                                A(i, j) = A(i + 3, j + 3) = pIn2[j];
                                            }
                                            A(i, j + 3) = A(i + 3, j) = 0;
                                        }
                                        b(i) = pOut2[0];
                                        b(i + 3) = pOut2[1];
                                    }

                                }

                                lu_factorize(A,P);
                                // Now A and P contain the LU factorization of A
                                x = b;
                                lu_substitute(A,P,x);
                                // Now x contains the solution.

                                for( int i = 0; i < 3; ++i ){
                                        for( int j = 0; j < 3; ++j ){
                                            if(i == 0){
                                                perspMatrix(i, j) = x(j);
                                            }

                                            else if(i == 1){
                                                perspMatrix(i, j) = x(j + 3);
                                            }

                                            else{
                                                if(j == 2){
                                                    perspMatrix(i, j) = 1;
                                                }
                                                else{
                                                    perspMatrix(i, j) = 0;
                                                }
                                            }
                                        }
                                 }


				_paramPerspMatrixRow0->setValue( perspMatrix( 0, 0 ), perspMatrix( 1, 0 ), perspMatrix( 2, 0 ) );
				_paramPerspMatrixRow1->setValue( perspMatrix( 0, 1 ), perspMatrix( 1, 1 ), perspMatrix( 2, 1 ) );
				_paramPerspMatrixRow2->setValue( perspMatrix( 0, 2 ), perspMatrix( 1, 2 ), perspMatrix( 2, 2 ) );
				break;
			}
			case eParamMethodPerspective:
			{
				bounded_matrix<double, 3, 3 > perspMatrix;

				////////////////////////////////////////
				/// @todo vincent: compute "perspMatrix" from input/output points -> "_paramPoint*"
				perspMatrix = identity_matrix<Scalar > ( 3 );
				////////////////////////////////////////

				_paramPerspMatrixRow0->setValue( perspMatrix( 0, 0 ), perspMatrix( 1, 0 ), perspMatrix( 2, 0 ) );
				_paramPerspMatrixRow1->setValue( perspMatrix( 0, 1 ), perspMatrix( 1, 1 ), perspMatrix( 2, 1 ) );
				_paramPerspMatrixRow2->setValue( perspMatrix( 0, 2 ), perspMatrix( 1, 2 ), perspMatrix( 2, 2 ) );
				break;
			}
			case eParamMethodBilinear:
			{
				bounded_matrix<Scalar, 2, 4 > bilMatrix;

				////////////////////////////////////////
				/// @todo adrien: compute "bilMatrix" from input/output points -> "_paramPoint*"
				bilMatrix( 0, 0 ) = 1.0;
				bilMatrix( 0, 1 ) = 0.0;
				bilMatrix( 0, 2 ) = 0.0;
				bilMatrix( 0, 3 ) = 0.0;
				bilMatrix( 1, 0 ) = 0.0;
				bilMatrix( 1, 1 ) = 1.0;
				bilMatrix( 1, 2 ) = 0.0;
				bilMatrix( 1, 3 ) = 2.0;
				////////////////////////////////////////

				_paramBilMatrixRow0->setValue( bilMatrix( 0, 0 ), bilMatrix( 1, 0 ) );
				_paramBilMatrixRow1->setValue( bilMatrix( 0, 1 ), bilMatrix( 1, 1 ) );
				_paramBilMatrixRow2->setValue( bilMatrix( 0, 2 ), bilMatrix( 1, 2 ) );
				_paramBilMatrixRow3->setValue( bilMatrix( 0, 3 ), bilMatrix( 1, 3 ) );
				break;
			}
		}
	}
}

bool PinningPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	using namespace boost::numeric::ublas;
	PinningProcessParams<Scalar> params = getProcessParams( args.renderScale );
	bool identity = false;

	// is the transformation matrix is an identity matrix the node is identity,
	// we perform no modification on the input image.
	switch( params._method )
	{
		case eParamMethodAffine:
		case eParamMethodPerspective:
		{
			if( norm_inf( params._perspective._matrix - identity_matrix<Scalar > ( 3 ) ) == 0 )
			{
				identity = true;
			}
			break;
		}
		case eParamMethodBilinear:
		{
			if( params._bilinear._matrix( 0, 0 ) == 1.0 &&
			 params._bilinear._matrix( 0, 1 ) == 0.0 &&
			 params._bilinear._matrix( 0, 2 ) == 0.0 &&
			 params._bilinear._matrix( 0, 3 ) == 0.0 &&
			 params._bilinear._matrix( 1, 0 ) == 0.0 &&
			 params._bilinear._matrix( 1, 1 ) == 1.0 &&
			 params._bilinear._matrix( 1, 2 ) == 0.0 &&
			 params._bilinear._matrix( 1, 3 ) == 0.0 )
			{
				identity = true;
			}
			break;
		}
	}
	identityClip = _clipSrc;
	identityTime = args.time;
	return identity;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PinningPlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				PinningProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PinningProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PinningProcess<rgba32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				PinningProcess<gray8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PinningProcess<gray16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PinningProcess<gray32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString( dstComponents ) << ") not supported by the plugin." );
	}
}


}
}
}
