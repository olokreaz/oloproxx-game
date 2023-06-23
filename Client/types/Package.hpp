#pragma once
#include <boost/serialization/access.hpp>


#include <steam/steamtypes.h>

#include "User.hpp"

namespace types
{
	template< class T > class CPackage {
	public:
		uint32      m_hashTpye = typeid( T ) . hash_code( );
		uint32      m_sizeData = sizeof( T );
		std::string m_typeName = typeid( T ) . name( );
		T           m_data;

		CPackage( ) = default;
		CPackage( const T &data ) { m_data = data; }

		CPackage( const CPackage &other )
		{
			m_hashTpye = other . m_hashTpye;
			m_sizeData = other . m_sizeData;
			m_data     = other . m_data;
		}

		CPackage& operator=( const CPackage &other )
		{
			m_hashTpye = other . m_hashTpye;
			m_sizeData = other . m_sizeData;
			m_data     = other . m_data;
			return *this;
		}

		CPackage( CPackage &&other ) noexcept
		{
			m_hashTpye = other . m_hashTpye;
			m_sizeData = other . m_sizeData;
			m_data     = std::move( other . m_data );
		}

		CPackage& operator=( CPackage &&other ) noexcept
		{
			m_hashTpye = other . m_hashTpye;
			m_sizeData = other . m_sizeData;
			m_data     = std::move( other . m_data );
			return *this;
		}

		T* operator->( ) { return &m_data; }

		~CPackage( ) = default;

	private:
		friend class boost::serialization::access;

		template< class Archive > void serialize( Archive &ar, const uint32 version )
		{
			ar & m_hashTpye;
			ar & m_sizeData;
			ar & m_data;
		}
	};

	template< class T > struct CMessage {
		CPackage< T > m_package;
		CUser         m_sender;
	};

	template< class T > using package_t = CPackage< T >;
	template< class T > using message_t = CMessage< T >;
}
