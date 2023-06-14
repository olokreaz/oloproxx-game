#pragma once
#include <fstream>
#include <ranges>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/access.hpp>

#include <steam/steamclientpublic.h>
#include <steam/steamtypes.h>

namespace types
{
	class CUser {
		friend class boost::serialization::access;

		CSteamID    m_steamID;
		std::string m_name;
		std::string m_dName;

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
		friend class boost::serialization::access;
		template< class Archive > void serialize( Archive &ar, const unsigned version ) { ar & m_users; }

	protected:
		using type = T;
		std::vector< type > m_users;

	public:
		std::vector< type >::iterator begin( ) { return m_users . begin( ); }
		std::vector< type >::iterator end( ) { return m_users . end( ); }

		std::vector< type >::iterator find( CSteamID steam_id )
		{
			return std::ranges::find_if( m_users
							, [ &steam_id ] ( const T &user ) { return user . steam_id( ) == steam_id; }
						);
		}

		std::vector< type >::iterator find( const std::string &name )
		{
			return std::ranges::find_if( m_users
							, [ &name ] ( const T &user ) { return user . name( ) == name; }
						);
		}

		void save( )
		{
			if ( boost::filesystem::exists( "users_dump.xml" ) ) {
				spdlog::info( "Removing old users_dump.xml" );
				load( );
				boost::filesystem::remove( "users_dump.xml" );
				spdlog::info( "Removed old users_dump.xml" );
			}
			spdlog::info( "Saving users to users_dump.xml" );
			std::ofstream                fl_users( "users_dump.xml" );
			boost::archive::xml_oarchive oa( fl_users );
			oa << *this;
			fl_users . close( );
			spdlog::info( "Saved users to users_dump.xml" );
		}

		void load( )
		{
			if ( boost::filesystem::exists( "users_dump.xml" ) ) {
				spdlog::warn( "users_dump.xml does not exist" );
				return;
			}
			spdlog::info( "Loading users from users_dump.xml" );
			std::ifstream                fl_users( "users_dump.xml" );
			boost::archive::xml_iarchive ia( fl_users );
			ia >> *this;
			fl_users . close( );
			spdlog::info( "Loaded users from users_dump.xml" );
		}

		void add( const type &user )
		{
			if ( std::ranges::find( m_users, user ) != m_users . end( ) ) {
				spdlog::warn( "User {} already exists", user . name( ) );
				return;
			}
			m_users . push_back( user );
			add_( user );
			spdlog::info( "Added user {}", user . name( ) );
		}

		virtual void remove( const type &user ) = 0;

	protected:
		virtual void add_( const type &user ) = 0;
	};

	//  ------------------------------------------------ Implementation ------------------------------------------------

	inline CUser::CUser( ) {}

	inline CUser::CUser( const CSteamID &steam_id ): m_steamID { steam_id } {}

	inline CUser::CUser( const std::string &name ): m_name { name } {}

	inline CUser::CUser( const CSteamID &steam_id, const std::string &name ): m_steamID { steam_id }
										, m_name { name } {}

	inline const CSteamID& CUser::steam_id( ) const { return m_steamID; }

	inline const std::string& CUser::name( ) const { return m_name; }

	inline CUser::CUser( const CUser &other ): m_steamID( other . m_steamID )
						, m_name( other . m_name ) {}

	inline CUser::CUser( CUser &&other ) noexcept: m_steamID( std::move( other . m_steamID ) )
							, m_name( std::move( other . m_name ) ) {}

	inline CUser& CUser::operator=( const CUser &other )
	{
		if ( this == &other ) return *this;
		m_steamID = other . m_steamID;
		m_name    = other . m_name;
		return *this;
	}

	inline CUser& CUser::operator=( CUser &&other ) noexcept
	{
		if ( this == &other ) return *this;
		m_steamID = std::move( other . m_steamID );
		m_name    = std::move( other . m_name );
		return *this;
	}

	template< class Archive > void CUser::serialize( Archive &ar, const unsigned version )
	{
		uint64 id64;
		id64 = m_steamID . ConvertToUint64( );

		ar & id64;
		ar & m_name;

		m_steamID = CSteamID( id64 );
	}

	inline void swap( CUser &lhs, CUser &rhs ) noexcept
	{
		using std::swap;
		swap( lhs . m_steamID, rhs . m_steamID );
		swap( lhs . m_name, rhs . m_name );
	}

	inline bool operator==( const CUser &lhs, const CUser &rhs )
	{
		return lhs . m_steamID == rhs . m_steamID
			&& lhs . m_name == rhs . m_name;
	}

	inline bool operator!=( const CUser &lhs, const CUser &rhs ) { return !( lhs == rhs ); }
	inline bool operator==( const CUser &lhs, const CSteamID &rhs ) { return lhs . m_steamID == rhs; }
	inline bool operator!=( const CUser &lhs, const CSteamID &rhs ) { return !( lhs == rhs ); }
}
