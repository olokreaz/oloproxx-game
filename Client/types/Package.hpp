#pragma once
#include <boost/serialization/access.hpp>

#include <steam/steamtypes.h>

#include "User.hpp"

namespace types
{
	template< class T > class CPackage {
		uint32 m_hashTpye = typeid( T ).hash_code( );
		uint32 m_size     = sizeof( T );
		T      m_data;

	public:
		CPackage( ) = default;
		CPackage( const T &data ) { m_data = data; }

		CPackage( const CPackage &other )
		{
			m_hashTpye = other.m_hashTpye;
			m_size     = other.m_size;
			m_data     = other.m_data;
		}

		CPackage& operator=( const CPackage &other )
		{
			m_hashTpye = other.m_hashTpye;
			m_size     = other.m_size;
			m_data     = other.m_data;
			return *this;
		}

		CPackage( CPackage &&other ) noexcept
		{
			m_hashTpye = other.m_hashTpye;
			m_size     = other.m_size;
			m_data     = std::move( other.m_data );
		}

		CPackage& operator=( CPackage &&other ) noexcept
		{
			m_hashTpye = other.m_hashTpye;
			m_size     = other.m_size;
			m_data     = std::move( other.m_data );
			return *this;
		}

		~CPackage( ) = default;

		const T& GetData( ) const { return m_data; }
		void     SetData( const T &data ) { m_data = data; }
		uint32   GetHashType( ) const { return m_hashTpye; }
		uint32   GetSize( ) const { return m_size; }

	private:
		friend class boost::serialization::access;

		template< class Archive > void serialize( Archive &ar, const uint32 version )
		{
			ar & m_hashTpye;
			ar & m_size;
			ar & m_data;
		}
	};

	template< class T > class CMessage {
		CPackage< T > m_package;
		CUser         m_sender;
		std::tm       m_time;
		CMessage( ) { }
	};

	template< class T > using package_t = CPackage< T >;
}
