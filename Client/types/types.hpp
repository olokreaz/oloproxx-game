#pragma once
#include <boost/any.hpp>
#include <steam/steamtypes.h>

namespace types
{
	template< typename T > class CPackage {
		int32 m_TypeHash = typeid( T ).hash_code( );
		T     m_Data;

	public:
		int32 GetTypeHash( ) const { return m_TypeHash; }

		T* operator->( ) { return &m_Data; }
	};
}
