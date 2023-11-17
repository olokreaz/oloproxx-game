#include <oloprox/Resource.hpp>
IData::IData( ): m_data { } {}

IData::IData( std::string &path, std::vector< uint8_t > &data ): m_path { std::move( path ) },
								m_data { },
								m_bStorage { true } { m_data . Data = std::move( data ); }

IData::IData( std::string &path, std::span< uint8_t > data ): m_path { path },
								m_data { } { m_data . pData = data; }

IData::~IData( ) = default;

const std::span< uint8_t >& IData::data( ) const { return m_bStorage ? m_data . Data : m_data . pData; }
const std::string&          IData::path( ) const { return m_path; }

IData::IData( const IData &other ): m_path { other . m_path },
				m_data { other . m_data },
				m_bStorage { other . m_bStorage } {}

IData::IData( IData &&other ) noexcept: m_path { std::move( other . m_path ) },
					m_data { std::move( other . m_data ) },
					m_bStorage { std::move( other . m_bStorage ) } {}

IData& IData::operator=( const IData &other )
{
	if ( this == &other ) return *this;
	m_path     = other . m_path;
	m_data     = other . m_data;
	m_bStorage = other . m_bStorage;
	return *this;
}

IData& IData::operator=( IData &&other ) noexcept
{
	if ( this == &other ) return *this;
	m_path     = std::move( other . m_path );
	m_data     = std::move( other . m_data );
	m_bStorage = std::move( other . m_bStorage );
	return *this;
}

void CResourceController::registrate( IData &data ) { m_resources[ data . path( ) ] = data; }

CResource::CResource( std::string_view path )
{
	this -> m_path        = path;
	this -> m_data . Data = systems::files::readFile( path );
	this -> m_bStorage    = true;
}

CResource::CResource( std::string_view path, std::span< uint8 > data )
{
	this -> m_path         = path;
	this -> m_data . pData = data;
	this -> m_bStorage     = false;
}
