module;

#include <array>
#include <chrono>
#include <span>
#include <string>
#include <unordered_map>

export module systems.utils;
import types;

using namespace std::literals;

namespace systems::utils
{
	export std::string create_guid( );

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

		static EncodingType checkEncoding( const std::span< unsigned char > &data )
		{
			if ( data . size( ) >= 3 && data[ 0 ] == 0xEF && data[ 1 ] == 0xBB && data[ 2 ] == 0xBF ) return EncodingType::UTF8_BOM;

			if ( data . size( ) >= 2 ) if ( data[ 0 ] == 0xFE && data[ 1 ] == 0xFF ) return EncodingType::UTF16_BOM; // UTF-16 Big-Endian

			if ( data . size( ) >= 4 && data[ 0 ] == 0x00 && data[ 1 ] == 0x00 && data[ 2 ] == 0xFE && data[ 3 ] == 0xFF ) return EncodingType::UTF32_BOM;

			return EncodingType::UNKNOWN;
		}

		// Для однобайтовых типов мы можем использовать std::array
		static constexpr std::array< uint8_t, 3 > UTF_8_BOM = { 0xEF, 0xBB, 0xBF };
		static constexpr std::array< uint8_t, 2 > UTF16_BOM = { 0xFE, 0xFF };
		static constexpr std::array< uint8_t, 4 > UTF32_BOM = { 0x00, 0x00, 0xFE, 0xFF };

		static constexpr std::size_t getBOMSize( EncodingType encType )
		{
			switch ( encType )
			{
				case EncodingType::UTF8_BOM: return UTF_8_BOM . size( );
				case EncodingType::UTF16_BOM: return UTF16_BOM . size( );
				case EncodingType::UTF32_BOM: return UTF32_BOM . size( );
				case EncodingType::UNKNOWN: return 0;
				default: return 0;
			}
		}

		static constexpr EncodingType getUTF( const EncodingType encType )
		{
			auto NoBOMMask = ~static_cast< uint8 >( EncodingType::BOM );
			return static_cast< EncodingType >( static_cast< unsigned >( encType ) & NoBOMMask );
		}

		template< EncodingType encType > static constexpr auto getBOM( )
		{
			if constexpr ( encType == EncodingType::UTF8_BOM ) return UTF_8_BOM;
			else if constexpr ( encType == EncodingType::UTF16_BOM ) return UTF16_BOM;
			else if constexpr ( encType == EncodingType::UTF32_BOM ) return UTF32_BOM;
			else return std::array< uint8_t, 0 > { };
		}
	};

	template< typename Key, typename Value > class Cache
	{
	public:
		using Clock = std::chrono::steady_clock;
		struct Item
		{
			Value             value;
			Clock::time_point expiry;
		};

		void insert( const Key &key, const Value &value, const std::chrono::milliseconds &duration = -1ms )
		{
			auto now    = Clock::now( );
			auto expiry = now + duration;

			auto &item    = _items[ key ];
			item . value  = value;
			item . expiry = expiry;
		}

		bool remove( const Key &key )
		{
			auto it = _items . find( key );
			if ( it != _items . end( ) )
			{
				_items . erase( it );
				return true;
			}
			return false;
		}

		bool update( const Key &key )
		{
			auto now = Clock::now( );
			auto it  = _items . find( key );
			if ( it != _items . end( ) )
			{
				if ( it -> second . expiry > now || it -> second . expiry == Clock::time_point { } )
				{
					it -> second . expiry = now;
					return true;
				}
				// If entry expired, remove it.
				_items . erase( it );
			}
			return false;
		}

	private:
		std::unordered_map< Key, Item > _items;
	};
}
