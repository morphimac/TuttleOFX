#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"

#include <tuttle/plugin/image/ofxToGil.hpp>
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
#include <boost/lexical_cast.hpp>

#include <cstddef>

namespace tuttle {
namespace plugin {
namespace warp {

WarpPlugin::WarpPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramOverlay       = fetchBooleanParam( kParamOverlay );

	_paramNbPoints       = fetchIntParam( kParamNbPoints );

	_paramGroupIn        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn )
	{
		_paramPointIn[cptIn] = fetchDouble2DParam( kParamPointIn + boost::lexical_cast<std::string>(cptIn) );
	}
	_paramOverlayIn      = fetchBooleanParam( kParamOverlayIn );
	_paramOverlayInColor = fetchRGBParam( kParamOverlayInColor );

	_paramGroupOut        = fetchGroupParam( kParamGroupIn );
	for( std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut )
	{
		_paramPointOut[cptOut] = fetchDouble2DParam( kParamPointOut + boost::lexical_cast<std::string>(cptOut) );
	}
	_paramOverlayOut      = fetchBooleanParam( kParamOverlayOut );
	_paramOverlayOutColor = fetchRGBParam( kParamOverlayOutColor );


	/*------------ Tests TPS -----------*/

	// Nombre de points d'entrée
	unsigned p = nbPoints;

	// Initialisation des matrices
	using namespace boost::numeric::ublas;

 	matrix<double> mtx_l(p+3, p+3);
  	matrix<double> mtx_v(p+3, 2);
  	matrix<double> mtx_orig_k(p, p);
  	matrix<double> mtx_passage(p, p);

	// Nombre de colonnes de la matrice K
	const unsigned m = mtx_orig_k.size2();

	/// @param regularization Amount of "relaxation", 0.0 = exact interpolation
	double regularization = 0.0;

	// Empiric value for avg. distance between points
    	//
   	// This variable is normally calculated to make regularization
    	// scale independent, but since our shapes in this application are always
    	// normalized to maxspect [-.5,.5]x[-.5,.5], this approximation is pretty
    	// safe and saves us p*p square roots
    	const double a = 0.5;

	/*------------ REMPLISSAGE DES MATRICES -----------*/
	
	// Remplit k et une partie de l
	for (unsigned i=0; i<p; ++i)
	{
      		OfxPointD point_i = _paramPointIn[i]->getValue();
      		for (unsigned j=0; j<m; ++j)
      		{
        		OfxPointD point_j = _paramPointIn[j]->getValue(); 
        		double sum = square(point_i.x-point_j.x) + square(point_i.y-point_j.y);
        		mtx_l(i,j) = mtx_orig_k(i,j) = base_func(sum);
      		}
    	}

	// Remplit le reste de l
    	for (unsigned i=0; i<p; ++i)
    	{
      		OfxPointD point_i = _paramPointIn[i]->getValue();
      		mtx_l(i, m+0) = 1.0;
      		mtx_l(i, m+1) = point_i.x;
      		mtx_l(i, m+2) = point_i.y;

      		if (i<m)
      		{
        		// diagonal: reqularization parameters (lambda * a^2)
        		mtx_l(i,i) = mtx_orig_k(i,i) = regularization * (a*a);

        		mtx_l(p+0, i) = 1.0;
       			mtx_l(p+1, i) = point_i.x;
        		mtx_l(p+2, i) = point_i.y;
      		}
    	}
	
	for (unsigned i=p; i<p+3; ++i)
	{
      		for (unsigned j=m; j<m+3; ++j)
		{
        		mtx_l(i,j) = 0.0;
		}
	}

	// Remplit une partie de v
	for (unsigned i=0; i<p; ++i)
	{
		OfxPointD point_i = _paramPointOut[i]->getValue();
		mtx_v(i,0) = point_i.x;
	      	mtx_v(i,1) = point_i.y;
	}

	mtx_v(p+0, 0) = mtx_v(p+1, 0) = mtx_v(p+2, 0) = 0.0;
	mtx_v(p+0, 1) = mtx_v(p+1, 1) = mtx_v(p+2, 1) = 0.0;
	
	//Solve the linear system "inplace"
	//int sret = lu_solve(mtx_l, mtx_v);

	lu_factorize( A, P );
	// Now A and P contain the LU factorization of A
	x = b;
	lu_substitute( A, P, x );

	/*assert( sret != 2 );
	if (sret == 1)
	{
	      puts( "Singular matrix! Aborting." );
		exit(1);
	}*/

	// Boucle qui parcourt la liste de points
/*	for ( std::vector<Point>::iterator ite=pts.begin(), end=pts.end(); ite != end; ++ite ){

		double x = ite->x, y=ite->y;
      		double dx = mtx_v(m+0, 0) + mtx_v(m+1, 0)*x + mtx_v(m+2, 0)*y;
      		double dy = mtx_v(m+0, 1) + mtx_v(m+1, 1)*x + mtx_v(m+2, 1)*y;


	}*/

	_instanceChangedArgs.time          = 0;
	_instanceChangedArgs.renderScale.x = 1;
	_instanceChangedArgs.renderScale.y = 1;
	_instanceChangedArgs.reason        = OFX::eChangePluginEdit;
	changedParam( _instanceChangedArgs, kParamNbPoints ); // init IsSecret property for each pair of points parameter
}

WarpProcessParams<WarpPlugin::Scalar> WarpPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	WarpProcessParams<Scalar> params;
	std::size_t size = _paramNbPoints->getValue();

	for( std::size_t i = 0; i < size; ++i )
	{
		point2<double> pIn = ofxToGil( _paramPointIn[i]->getValue() );
		params._inPoints.push_back( pIn );
		point2<double> pOut = ofxToGil( _paramPointOut[i]->getValue() );
		params._outPoints.push_back( pOut );
	}

	return params;
}

void WarpPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( boost::starts_with( paramName, kParamPointIn ) ||
	    boost::starts_with( paramName, kParamPointOut ) ||
		paramName == kParamNbPoints )
	{
		std::size_t size = _paramNbPoints->getValue();
		std::size_t i = 0;
		for(; i < size; ++i )
		{
			_paramPointIn[i]->setIsSecretAndDisabled( false );
			_paramPointOut[i]->setIsSecretAndDisabled( false );
		}
		for( ; i < kMaxNbPoints; ++i )
		{
			_paramPointIn[i]->setIsSecretAndDisabled( true );
			_paramPointOut[i]->setIsSecretAndDisabled( true );
		}
	}
}

//bool WarpPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void WarpPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool WarpPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	WarpProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void WarpPlugin::render( const OFX::RenderArguments &args )
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
            case OFX::eBitDepthUByte :
            {
                WarpProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                WarpProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                WarpProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );

				break;
			}
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                WarpProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                WarpProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                WarpProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{

				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );

				break;
			}
        }
    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );

	}
}


}
}
}
