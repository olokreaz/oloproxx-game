#pragma once
#include <memory>
#include <span>
#include <string_view>
#include <unordered_map>
#include <boost/noncopyable.hpp>

#include <coroutine/windows.h>

import systems.files;

class IData
{
protected:
	std::string m_path;
	// std::span< uint8_t > m_data;

	union data_t
	{
		std::span< uint8_t >   pData;
		std::vector< uint8_t > Data;
	}                              m_data;
	bool                           m_bStorage { false };

public:
	IData( ) : m_data { } {}

	IData( std::string &path, std::vector< uint8_t > &data ) : m_path { std::move( path ) },
								m_data { },
								m_bStorage { true } { m_data . Data = std::move( data ); }

	IData( std::string &path, std::span< uint8_t > data ) : m_path { path },
								m_data { } { m_data . pData = data; }

	virtual ~IData( ) = default;

	const std::span< uint8_t >& data( ) const { return m_data . pData; }
	const std::string&          path( ) const { return m_path; }

	IData( const IData &other ) : m_path { other . m_path },
					m_data { other . m_data },
					m_bStorage { other . m_bStorage } {}

	IData( IData &&other ) noexcept : m_path { std::move( other . m_path ) },
					m_data { std::move( other . m_data ) },
					m_bStorage { std::move( other . m_bStorage ) } {}

	IData& operator=( const IData &other )
	{
		if ( this == &other ) return *this;
		m_path     = other . m_path;
		m_data     = other . m_data;
		m_bStorage = other . m_bStorage;
		return *this;
	}

	IData& operator=( IData &&other ) noexcept
	{
		if ( this == &other ) return *this;
		m_path     = std::move( other . m_path );
		m_data     = std::move( other . m_data );
		m_bStorage = std::move( other . m_bStorage );
		return *this;
	}
};

class final CResourceController : public boost::noncopyable
{
	static std::unordered_map< std::string, IData > m_resources;

public:
	CResourceController( )  = delete;
	~CResourceController( ) = delete;

	static void registrate( IData &data ) { m_resources[ data . path( ) ] = data; }
};

class CResource final : public IData
{
	CResource( std::string_view path )
	{
		this -> m_path        = path;
		this -> m_data . Data = systems::files::readFile( path );
		this -> m_bStorage    = true;
	}

	CResource( std::string_view path, std::span< uint8 > data )
	{
		this -> m_path         = path;
		this -> m_data . pData = data;
		this -> m_bStorage     = false;
	}
};
