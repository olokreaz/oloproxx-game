#pragma once
#include <boost/serialization/access.hpp>

#include <steam/isteamclient.h>
#include <steam/steamclientpublic.h>
#include <steam/steamtypes.h>

#include <string>

namespace types
{
	class CUser {
		friend class boost::serialization::access;

		CSteamID    m_steamID;
		std::string m_name;

	public:
		explicit CUser( );
		explicit CUser( const CSteamID &steam_id );
		explicit CUser( const std::string &name );

		explicit CUser( const CSteamID &steam_id, const std::string &name );

		const CSteamID&    steam_id( ) const;
		const std::string& name( ) const;

		CUser( const CUser &other );

		CUser( CUser &&other ) noexcept;

		CUser& operator=( const CUser &other );

		CUser& operator=( CUser &&other ) noexcept;

		friend bool operator==( const CUser &lhs, const CUser &rhs );
		friend bool operator!=( const CUser &lhs, const CUser &rhs );
		friend bool operator==( const CUser &lhs, const CSteamID &rhs );
		friend bool operator!=( const CUser &lhs, const CSteamID &rhs );

	private:
		template< class Archive > void serialize( Archive &ar, const unsigned version );

		friend void swap( CUser &lhs, CUser &rhs ) noexcept;
	};

	template< class T = CUser > class CUserManager {
		std::vector< T > m_users;

	public:
		std::vector< T >::iterator begin( ) { return m_users.begin( ); }
		std::vector< T >::iterator end( ) { return m_users.end( ); }

		std::vector< T >::iterator find( CSteamID steam_id )
		{
			return std::find_if( m_users.begin( )
						, m_users.end( )
						, [ &steam_id ] ( const T &user )
							{
								return user.steam_id( ) == steam_id;
							}
					);
		}

		std::vector< T >::iterator find( const std::string &name )
		{
			return std::find_if( m_users.begin( )
						, m_users.end( )
						, [ &name ] ( const T &user ) { return user.name( ) == name; }
					);
		}
	};

	//  ------------------------------------------------ Implementation ------------------------------------------------

	inline CUser::CUser( ) {}

	inline CUser::CUser( const CSteamID &steam_id ): m_steamID { steam_id } {}

	inline CUser::CUser( const std::string &name ): m_name { name } {}

	inline CUser::CUser( const CSteamID &steam_id, const std::string &name ): m_steamID { steam_id },
		m_name { name } {}

	inline const CSteamID& CUser::steam_id( ) const { return m_steamID; }

	inline const std::string& CUser::name( ) const { return m_name; }

	inline CUser::CUser( const CUser &other ): m_steamID( other.m_steamID ),
						m_name( other.m_name ) {}

	inline CUser::CUser( CUser &&other ) noexcept: m_steamID( std::move( other.m_steamID ) ),
							m_name( std::move( other.m_name ) ) {}

	inline CUser& CUser::operator=( const CUser &other )
	{
		if ( this == &other ) return *this;
		m_steamID = other.m_steamID;
		m_name    = other.m_name;
		return *this;
	}

	inline CUser& CUser::operator=( CUser &&other ) noexcept
	{
		if ( this == &other ) return *this;
		m_steamID = std::move( other.m_steamID );
		m_name    = std::move( other.m_name );
		return *this;
	}

	template< class Archive > void CUser::serialize( Archive &ar, const unsigned version )
	{
		uint64 id64;
		id64 = m_steamID.ConvertToUint64( );

		ar & id64;
		ar & m_name;

		m_steamID = CSteamID( id64 );

	}

	inline void swap( CUser &lhs, CUser &rhs ) noexcept
	{
		using std::swap;
		swap( lhs.m_steamID, rhs.m_steamID );
		swap( lhs.m_name, rhs.m_name );
	}

	inline bool operator==( const CUser &lhs, const CUser &rhs )
	{
		return lhs.m_steamID == rhs.m_steamID
			&& lhs.m_name == rhs.m_name;
	}

	inline bool operator!=( const CUser &lhs, const CUser &rhs ) { return !( lhs == rhs ); }
	inline bool operator==( const CUser &lhs, const CSteamID &rhs ) { return lhs.m_steamID == rhs; }
	inline bool operator!=( const CUser &lhs, const CSteamID &rhs ) { return !( lhs == rhs ); }
}
