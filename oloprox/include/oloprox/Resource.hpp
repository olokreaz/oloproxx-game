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
	IData( );

	IData( std::string &path, std::vector< uint8_t > &data );

	IData( std::string &path, std::span< uint8_t > data );

	virtual ~IData( );

	const std::span< uint8_t >& data( ) const;
	const std::string&          path( ) const;

	IData( const IData &other );

	IData( IData &&other ) noexcept;

	IData& operator=( const IData &other );

	IData& operator=( IData &&other ) noexcept;
};

class final CResourceController : public boost::noncopyable
{
	static std::unordered_map< std::string, IData > m_resources;

public:
	CResourceController( )  = delete;
	~CResourceController( ) = delete;

	static void registrate( IData &data );
};

class CResource final : public IData
{
	CResource( std::string_view path );

	CResource( std::string_view path, std::span< uint8 > data );
};
