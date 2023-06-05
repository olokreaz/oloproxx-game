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

		const CSteamID&    steamId( ) const;
		const std::string& name( ) const;

		CUser( const CUser &other );

		CUser( CUser &&other ) noexcept;

		CUser & operator=( const CUser &other );

		CUser & operator=( CUser &&other ) noexcept;

		friend bool operator==( const CUser &lhs, const CUser &rhs );
		friend bool operator!=( const CUser &lhs, const CUser &rhs );
		friend bool operator==( const CUser &lhs, const CSteamID &rhs );
		friend bool operator!=( const CUser &lhs, const CSteamID &rhs );

	private:
		template< class Archive > void serialize( Archive &ar, const unsigned version );

		friend void swap( CUser &lhs, CUser &rhs ) noexcept;
	};

	inline CUser::CUser( ) {}

	inline CUser::CUser( const CSteamID &steam_id ): m_steamID { steam_id } {}

	inline CUser::CUser( const std::string &name ): m_name { name } {}

	inline CUser::CUser( const CSteamID &steam_id, const std::string &name ): m_steamID { steam_id },
		m_name { name } {}

	inline const CSteamID & CUser::steamId( ) const { return m_steamID; }

	inline const std::string & CUser::name( ) const { return m_name; }

	inline CUser::CUser( const CUser &other ): m_steamID( other.m_steamID ),
						m_name( other.m_name ) {}

	inline CUser::CUser( CUser &&other ) noexcept: m_steamID( std::move(other.m_steamID) ),
							m_name( std::move(other.m_name) ) {}

	inline CUser & CUser::operator=( const CUser &other ) {
		if ( this == &other ) return *this;
		m_steamID = other.m_steamID;
		m_name    = other.m_name;
		return *this;
	}

	inline CUser & CUser::operator=( CUser &&other ) noexcept {
		if ( this == &other ) return *this;
		m_steamID = std::move( other.m_steamID );
		m_name    = std::move( other.m_name );
		return *this;
	}

	template< class Archive > void CUser::serialize( Archive &ar, const unsigned version ) {
		uint64 id64;
		id64 = m_steamID.ConvertToUint64( );

		ar & id64;
		ar & m_name;

		m_steamID = CSteamID( id64 );

	}

	inline void swap( CUser &lhs, CUser &rhs ) noexcept {
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
