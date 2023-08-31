#pragma once
#include <efsw/efsw.hpp>

using Hash_t = std::string;

#ifdef fs_boost
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

enum EBinType
{
	eDebug   = 0 ,
	eRelease = 1 ,
};

struct Config
{
	fs::path                   source;
	fs::path                   destination;
	EBinType                   bin;
	std::vector< std::string > ignore;
	YAML::Node                 node;
};

extern Config                                    g_config;
extern std::unordered_map< std::string, Hash_t > g_fileHashes;

class IObserverHandler
{
protected:
	virtual void add_( const fs::path &path, efsw::Action action, fs::path oldpath ) = 0;
	virtual void modified_( const fs::path &path, efsw::Action action, fs::path oldpath ) = 0;
	virtual void moved_( const fs::path &path, efsw::Action action, fs::path oldpath ) = 0;
	virtual void delete_( const fs::path &path, efsw::Action action, fs::path oldpath ) = 0;
	virtual void default_( const fs::path &path, efsw::Action action, fs::path oldpath ) = 0;

	virtual void update_data( const fs::path &path, efsw::Action action, fs::path oldpath ) {}
	virtual void update_data( ) {}
};

template< class Hnadler, bool bUseData = false > requires std::derived_from< Hnadler, IObserverHandler >
class CObserver final : public efsw::FileWatchListener , public Hnadler
{
public:
	void handleFileAction(
			efsw::WatchID watchid, const std::string &dir, const std::string &filename = "", efsw::Action action = efsw::Action::Add,
			std::string   oldFilename                                                  = ""
			) override
	{
		if ( filename . ends_with( "~" ) ) return;
		auto path = fs::path( dir ) / filename;
		if constexpr ( bUseData ) this -> update_data( path, action, oldFilename );
		else this -> update_data( );
		switch ( action )
		{
			case efsw::Action::Add:
			{
				this -> add_( path, action, oldFilename );
				break;
			}
			case efsw::Action::Modified:
			{
				this -> modified_( path, action, oldFilename );
				break;
			}
			case efsw::Action::Moved:
			{
				this -> moved_( path, action, oldFilename );
				break;
			}
			case efsw::Action::Delete:
			{
				this -> delete_( path, action, oldFilename );
				break;
			}
			default: ;
		}
	}
};

std::string compute_sha256( const std::string &file_path )
{
	CryptoPP::SHA256 hash;
	Hash_t           result;

	CryptoPP::FileSource(
				file_path . c_str( ),
				true,
				new CryptoPP::HashFilter(
							hash,
							new CryptoPP::HexEncoder( new CryptoPP::StringSink( result ) )
							)
			);

	return result;
}

bool delete_path( const fs::path target )
{
	if ( fs::exists( target ) )
	{
		// Если цель - это директория, удаляем её и всё внутри
		if ( fs::is_directory( target ) ) return fs::remove_all( target );
		// Если цель - это файл, удаляем его
		if ( fs::is_regular_file( target ) ) return fs::remove( target );
	}
	spdlog::info( "Target does not exist" );
	return false;
}

void copy_to( const fs::path &source, const fs::path &destination )
{
	// Создаем целевой каталог, если он еще не существует
	if ( !fs::exists( destination ) ) fs::create_directories( destination );

	// Итерируем через исходный каталог и рекурсивно копируем все вложенные файлы и каталоги
	if ( fs::exists( source ) )
		if ( fs::is_directory( source ) )
			for ( const auto &dir_entry : fs::recursive_directory_iterator( source ) )
			{
				auto current = dir_entry . path( );
				auto target  = destination / fs::relative( current, source );

				// Проверка на игнорируемые расширения файлов
				if ( std::ranges::find( g_config . ignore, current . extension( ) ) != g_config . ignore . end( ) ) continue;
				// Проверка на игнорируемые директории
				if ( std::ranges::find( g_config . ignore, current . parent_path( ) . stem( ) ) != g_config . ignore . end( ) ) continue;
				// Проверка на игнорируемые файлы
				if ( std::ranges::find( g_config . ignore, current . stem( ) ) != g_config . ignore . end( ) ) continue;

				// Является ли текущий элемент каталогом?
				if ( fs::is_directory( current ) ) fs::create_directories( target ); // Если да, то создаем копию каталога
				else
				{
					auto hash = compute_sha256( current . string( ) ); // в противном случае это файл, проверяем его хеш
					if ( g_fileHashes . count( target . string( ) ) == 0 || g_fileHashes[ target . string( ) ] != hash )
					{
						fs::copy_file(
								current, target,
								fs::copy_options::overwrite_existing
								); // Если файл не существует или хеш отличается, копируем его
						g_fileHashes[ target . string( ) ] = hash;
					}
				}
			}
		else fs::copy_file( source, destination, fs::copy_options::overwrite_existing );
}
