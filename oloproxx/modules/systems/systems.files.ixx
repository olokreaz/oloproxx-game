module;

#include <filesystem>
#include <span>
#include <vector>

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <utf8cpp/utf8.h>

export module systems.files;
import utils;
import types;

namespace bi = boost::interprocess;

namespace systems::files
{
	export [[nodiscard]] std::vector < uint8 > readFile( const std::filesystem::path &path )
	{
		boost::iostreams::mapped_file_source file;
		file . open( path . string( ) );
		if ( !file . is_open( ) ) throw std::runtime_error( "Error opening file" );

		std::string ctx( file . data( ), file . size( ) );
		file . close( );
		std::vector < uint8 > result( file . data( ), file . data( ) + file . size( ) );
		spdlog::debug( "loeaded file: {}", path . string( ) );
		return result;
	}


	export class CResource
	{
	public:
		using iterator = std::span < uint8 >::iterator;
		using const_iterator = std::span < uint8 >::const_iterator;

	private:
		static std::unordered_map < std::wstring, std::span < uint8 > > m_cache;
		std::span < uint8 >                                             m_Data;
		utils::EncodingInfo::EncodingType                               m_EncodingType = utils::EncodingInfo::EncodingType::UNKNOWN;

		bi::mapped_region m_region;

	public:
		CResource( ) = default;

		CResource( const std::wstring wsID, const bool bText = false )
		{
			const auto it = m_cache . find( wsID );
			if ( it != m_cache . end( ) ) m_Data = it -> second;
			else
			{
				m_Data          = _LoadResource( wsID );
				m_cache[ wsID ] = m_Data;
			}
		}

		_NODISCARD std::span < uint8 > data( ) const { return m_Data; }
		_NODISCARD size_t              size( ) const { return m_Data . size( ); }

		_NODISCARD std::span < uint8 > text( ) const
		{
			if ( m_EncodingType == utils::EncodingInfo::EncodingType::UNKNOWN ) return m_Data;
			return m_Data . subspan( utils::EncodingInfo::getBOMSize( m_EncodingType ) );
		}

		_NODISCARD std::u8string getBomText( )
		{
			using utils::EncodingInfo;
			// Convert to UTF-8 depending on the encoding type of the data.
			switch ( EncodingInfo::getUTF( m_EncodingType ) )
			{
				case EncodingInfo::EncodingType::UTF8 : { return std::u8string( text( ) . begin( ), text( ) . end( ) ); }
				case EncodingInfo::EncodingType::UTF16 :
				{
					std::u16string u16_text( reinterpret_cast < const char16_t * >( &text( )[ 0 ] ), text( ) . size( ) / 2 );// Convert to u16string
					std::string    u8s;
					utf8::utf16to8( u16_text . begin( ), u16_text . end( ), back_inserter( u8s ) );                // Convert to utf8 (std::string)
					return std::u8string( reinterpret_cast < const char8_t * >( u8s . data( ) ), u8s . length( ) );// Return as std::u8string
				}
				case EncodingInfo::EncodingType::UTF32 :
				{
					std::u32string u32_text( reinterpret_cast < const char32_t * >( &text( )[ 0 ] ), text( ) . size( ) / 4 );// Convert to u32string
					std::string    u8s;
					utf8::utf32to8( u32_text . begin( ), u32_text . end( ), back_inserter( u8s ) );                // Convert to utf8 (std::string)
					return std::u8string( reinterpret_cast < const char8_t * >( u8s . data( ) ), u8s . length( ) );// Return as std::u8string
				}
				default : return std::u8string( m_Data . begin( ), m_Data . end( ) );
			}
		}

		iterator       begin( ) { return m_Data . begin( ); }
		const_iterator cbegin( ) const { return m_Data . cbegin( ); }
		iterator       end( ) { return m_Data . end( ); }
		const_iterator cend( ) const { return m_Data . cend( ); }

		void release( )
		{
			if ( m_region . get_address( ) == nullptr ) return;
			m_region . flush( );
			m_region . ~mapped_region( );
		}

	private:
		void __load_resource_path( std::filesystem::path path )
		{
			const bi::file_mapping file( path . string( ) . c_str( ), bi::read_only );

			const bi::mapped_region region( file, bi::read_only );

			void *            pData = region . get_address( );
			const std::size_t size  = region . get_size( );

			m_Data         = std::span( static_cast < uint8 * >( pData ), size );
			m_EncodingType = utils::EncodingInfo::checkEncoding( m_Data );

		}


		_NODISCARD std::span < uint8 > _LoadResource( std::wstring_view wsID )
		{
			const HMODULE hModule = GetModuleHandleW( nullptr );

			const HRSRC hResInfo = FindResourceW( hModule, wsID . data( ),RT_RCDATA );

			const HGLOBAL hResData( LoadResource( hModule, hResInfo ) );

			void *pData = LockResource( hResData );

			const DWORD size = SizeofResource( hModule, hResInfo );

			m_Data = std::span( static_cast < uint8 * >( pData ), size );

			m_EncodingType = utils::EncodingInfo::checkEncoding( m_Data );

			return m_Data;
		}
	};
}
