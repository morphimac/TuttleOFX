#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMBOOLEAN_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMBOOLEAN_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamBoolean
	: public OfxhParam
	, public OfxhKeyframeParam
{
protected:
	std::size_t _index;

public:
	typedef bool BaseType;
	OfxhParamBoolean( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance, const std::size_t index = 0 )
		: OfxhParam( descriptor, name, setInstance )
		, _index( index )
	{}

	// Deriving implementatation needs to overide these
	virtual void get( bool& ) const OFX_EXCEPTION_SPEC                                                 = 0;
	virtual void getAtTime( const OfxTime time, bool& ) const OFX_EXCEPTION_SPEC                       = 0;
	virtual void set( const bool&, const EChange change ) OFX_EXCEPTION_SPEC                           = 0;
	virtual void setAtTime( const OfxTime time, const bool&, const EChange change ) OFX_EXCEPTION_SPEC = 0;

	virtual void set( const int& v, const EChange change ) OFX_EXCEPTION_SPEC
	{
		set( bool(v), change );
	}

	virtual void setAtTime( const OfxTime time, const int& v, const EChange change ) OFX_EXCEPTION_SPEC
	{
		setAtTime( time, bool(v), change );
	}

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}
}

#endif

