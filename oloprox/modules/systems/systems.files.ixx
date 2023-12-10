module;

#include <filesystem>
#include <span>
#include <vector>

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <wil/resource.h>
#include <wil/result.h>

#include <utf8cpp/utf8.h>

export module systems.files;
import systems.utils;
import types;

namespace systems::files
{
	export [[nodiscard]] std::vector< uint8 > readFile( const std::filesystem::path &path )
	{
		boost::iostreams::mapped_file_source file;
		file . open( path . string( ) );
		if ( !file . is_open( ) ) throw std::runtime_error( "Error opening file" );

		std::string ctx( file . data( ), file . size( ) );
		file . close( );
		std::vector< uint8 > result( file . data( ), file . data( ) + file . size( ) );
		spdlog::debug( "loeaded file: {}", path . string( ) );
		return result;
	}

	export class CResource
	{
		static std::unordered_map< std::wstring, std::span< uint8 > > cache;
		std::span< uint8 >                                            m_Data;
		utils::EncodingInfo::EncodingType                             m_EncodingType = utils::EncodingInfo::EncodingType::UNKNOWN;

	public:
		CResource( ) = default;

		CResource( const std::wstring wsID, const bool bText = false )
		{
			auto it = cache . find( wsID );
			if ( it != cache . end( ) ) m_Data = it -> second;
			else
			{
				m_Data        = _LoadResource( wsID, bText );
				cache[ wsID ] = m_Data;
			}
		}

		_NODISCARD std::span< uint8 > data( ) const { return m_Data; }
		_NODISCARD size_t             size( ) const { return m_Data . size( ); }

		_NODISCARD std::span< uint8 > text( ) const
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
				case EncodingInfo::EncodingType::UTF8: { return std::u8string( text( ) . begin( ), text( ) . end( ) ); }
				case EncodingInfo::EncodingType::UTF16:
				{
					std::u16string u16_text( reinterpret_cast< const char16_t * >( &text( )[ 0 ] ), text( ) . size( ) / 2 ); // Convert to u16string
					std::string    u8s;
					utf8::utf16to8( u16_text . begin( ), u16_text . end( ), back_inserter( u8s ) ); // Convert to utf8 (std::string)
					return std::u8string( reinterpret_cast< const char8_t * >( u8s . data( ) ), u8s . length( ) ); // Return as std::u8string
				}
				case EncodingInfo::EncodingType::UTF32:
				{
					std::u32string u32_text( reinterpret_cast< const char32_t * >( &text( )[ 0 ] ), text( ) . size( ) / 4 ); // Convert to u32string
					std::string    u8s;
					utf8::utf32to8( u32_text . begin( ), u32_text . end( ), back_inserter( u8s ) ); // Convert to utf8 (std::string)
					return std::u8string( reinterpret_cast< const char8_t * >( u8s . data( ) ), u8s . length( ) ); // Return as std::u8string
				}
				default: return std::u8string( m_Data . begin( ), m_Data . end( ) );
			}
		}

	private:
		_NODISCARD std::span< uint8 > _LoadResource( std::wstring_view wsID, bool bText = false )
		{
			try
			{
				wil::unique_hmodule hModule( GetModuleHandleW( nullptr ) );

				HRSRC hResInfo = FindResourceW( hModule . get( ), wsID . data( ),RT_RCDATA );
				THROW_LAST_ERROR_IF_NULL( hResInfo );

				wil::unique_hglobal hResData( LoadResource( hModule . get( ), hResInfo ) );
				THROW_LAST_ERROR_IF_NULL( hResData );

				void *pData = LockResource( hResData . get( ) );
				THROW_LAST_ERROR_IF_NULL( pData );

				DWORD size = SizeofResource( hModule . get( ), hResInfo );

				m_Data = std::span( static_cast< uint8 * >( pData ), size );

				if ( bText ) m_EncodingType = utils::EncodingInfo::checkEncoding( m_Data );

				return m_Data;
			} catch ( const wil::ResultException & )
			{
				spdlog::warn( "Catch ResultException" );
				return { };
			}
		}
	};

	_NODISCARD float Q_rsqrt( float number )
	{
		switch ( static_cast< int >( number ) )
		{
			case 4: return 0.5f;
			case 16: return 0.25f;
			case 64: return 0.125f;
			case 256: return 0.0625f;
			case 1024: return 0.03125f;
			default: break;
		}

		long            i;
		float           x2, y;
		constexpr float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;

		i = *( long * ) &y;
		i = 0x5f3759df - ( i >> 1 );

		y = *( float * ) &i;
		y = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	}
}
