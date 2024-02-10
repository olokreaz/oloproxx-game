module;

#include <array>
#include <chrono>
#include <filesystem>
#include <span>
#include <string>


#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <string>

export module utils;
import config;
import types;

#define __cpp_lib_flat_map 202207L

using namespace std::literals;

namespace utils
{
	export struct EncodingInfo
	{
		enum class EncodingType : uint8
		{
			UNKNOWN = 0 ,

			UTF8  = 1 << 0 ,
			UTF16 = 1 << 1 ,
			UTF32 = 1 << 2 ,

			// BOM indicator
			BOM = 1 << 5 ,

			UTF8_BOM  = UTF8 | BOM ,
			UTF16_BOM = UTF16 | BOM ,
			UTF32_BOM = UTF32 | BOM ,
		};


		static EncodingType checkEncoding( const std::span < unsigned char > &data )
		{
			if ( data . size( ) >= 3 && data[ 0 ] == 0xEF && data[ 1 ] == 0xBB && data[ 2 ] == 0xBF ) return EncodingType::UTF8_BOM;

			if ( data . size( ) >= 2 ) if ( data[ 0 ] == 0xFE && data[ 1 ] == 0xFF ) return EncodingType::UTF16_BOM;// UTF-16 Big-Endian

			if ( data . size( ) >= 4 && data[ 0 ] == 0x00 && data[ 1 ] == 0x00 && data[ 2 ] == 0xFE && data[ 3 ] == 0xFF ) return EncodingType::UTF32_BOM;

			return EncodingType::UNKNOWN;
		}

		// Для однобайтовых типов мы можем использовать std::array
		static constexpr std::array < uint8_t, 3 > UTF_8_BOM = { 0xEF, 0xBB, 0xBF };
		static constexpr std::array < uint8_t, 2 > UTF16_BOM = { 0xFE, 0xFF };
		static constexpr std::array < uint8_t, 4 > UTF32_BOM = { 0x00, 0x00, 0xFE, 0xFF };

		static constexpr std::size_t getBOMSize( EncodingType encType )
		{
			switch ( encType )
			{
				case EncodingType::UTF8_BOM : return UTF_8_BOM . size( );
				case EncodingType::UTF16_BOM : return UTF16_BOM . size( );
				case EncodingType::UTF32_BOM : return UTF32_BOM . size( );
				case EncodingType::UNKNOWN : return 0;
				default : return 0;
			}
		}

		static constexpr EncodingType getUTF( const EncodingType encType )
		{
			constexpr auto NoBOMMask = ~static_cast < uint8 >( EncodingType::BOM );
			return static_cast < EncodingType >( static_cast < unsigned >( encType ) & NoBOMMask );
		}

		template< EncodingType encType > static constexpr auto getBOM( )
		{
			if constexpr ( encType == EncodingType::UTF8_BOM ) return UTF_8_BOM;
			else if constexpr ( encType == EncodingType::UTF16_BOM ) return UTF16_BOM;
			else if constexpr ( encType == EncodingType::UTF32_BOM ) return UTF32_BOM;
			else return std::array < uint8_t, 0 > { };
		}
	};


	export _NODISCARD constexpr float Q_rsqrt( float number )
	{
		float           y;
		constexpr float threehalfs = 1.5F;

		const float x2 = number * 0.5F;
		y              = number;

		long i = *( long * ) &y;
		i      = 0x5f3759df - ( i >> 1 );

		y = *( float * ) &i;
		y = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	}

	export _NODISCARD void NukeProcess( int code = -1, const int offset = 0xff ) { exit( offset | code ); }

	/*export std::shared_ptr < spdlog::logger > create_logger( const std::string_view logger_name, std::string file_name = "application" )
	{
		const auto siDialy = std::make_shared < spdlog::sinks::daily_file_sink_mt >(
				( std::filesystem::current_path( ) / config::kConfig_Logger_Dir / fmt::format(
						"{}.log",
						file_name
						) ) .
				string( ), 0, 0
				);
		const auto siStdout = std::make_shared < spdlog::sinks::stdout_color_sink_mt >( );


		siStdout -> set_color(
				spdlog::level::trace,
				0b111111
				);


		std::shared_ptr < spdlog::logger > logger;

		logger = std::make_shared < spdlog::logger >( logger_name . data( ) );
		logger -> sinks( ) . clear( );

		logger -> sinks( ) . push_back( siStdout );
		logger -> sinks( ) . push_back( siDialy );

		logger -> flush_on( spdlog::level::warn );
		logger -> set_pattern( config::kLogger_pattern );
		logger -> set_level( spdlog::get_level( ) );

		logger -> trace( "logger created" );

		return logger;
	}*/

	template< class _Ty > std::string_view type_name( ) { return std::string_view( typeid( _Ty ) . name( ) ) . substr( 6 ); }

	export inline const std::filesystem::path &exe_path( )
	{
		static const auto p = std::filesystem::path( *__argv ) . parent_path( );
		return p;
	}
}
