#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <yaml-cpp/yaml.h>
namespace fs = std::filesystem;

struct Config
{
	fs::path                   source;
	fs::path                   destination;
	fs::path                   binSource;
	std::filesystem::path      binName;
	std::string                binType;
	std::vector< std::string > ignore;
};

std::string calculateSHA256( const fs::path &filePath )
{
	std::ifstream       file( filePath, std::ios::binary );
	std::vector< char > buffer( std::istreambuf_iterator< char >( file ), { } );

	CryptoPP::SHA256     hash;
	std::string          digest;
	CryptoPP::HexEncoder encoder;

	hash . Update( ( const CryptoPP::byte * ) buffer . data( ), buffer . size( ) );
	digest . resize( hash . DigestSize( ) );
	hash . Final( ( CryptoPP::byte * ) &digest[ 0 ] );

	encoder . Attach( new CryptoPP::StringSink( digest ) );
	encoder . Put( ( CryptoPP::byte * ) &digest[ 0 ], hash . DigestSize( ) );
	encoder . MessageEnd( );

	return digest;
}

void syncFolder( const Config &config, std::unordered_map< std::string, fs::path > &destinationMap )
{
	for ( const auto &entry : fs::recursive_directory_iterator( config . source ) )
	{
		const auto &path            = entry . path( );
		auto        relativePathStr = path . string( );
		std::string newPathStr      = relativePathStr . replace( 0, config . source . string( ) . length( ), config . destination . string( ) );
		fs::path    newPath         = fs::path( newPathStr );

		// Проверяем, является ли это файлом для игнорирования
		bool ignoreFile = std::ranges::any_of(
							config . ignore,
							[&path] ( const auto &ignoreItem )
							{
								auto absIgnoreItem = fs::absolute( ignoreItem );
								if ( fs::is_regular_file( ignoreItem ) && path == absIgnoreItem ) return true;
								if ( fs::is_directory( ignoreItem ) && path . string( ) . find( absIgnoreItem . string( ) ) == 0 ) return true;
								if ( path . filename( ) . string( ) == ignoreItem ) return true;
								if ( path . extension( ) == ignoreItem ) return true;
								return false;
							}
							);

		if ( !ignoreFile )
		{
			if ( fs::is_regular_file( path ) )
			{
				std::string hash = calculateSHA256( path );
				// Если файл с таким же хешем уже существует - пропустить
				if ( destinationMap . find( hash ) != destinationMap . end( ) ) continue;
			}
			if ( fs::is_directory( path ) ) fs::create_directories( newPath );
			else
			{ // Например, "Release.exe" или "Debug.exe" в зависимости от настроек вашего конфиг-файла
				if ( path . filename( ) . string( ) == config . binName && path . parent_path( ) . string( ) == config . binSource )
				{
					std::string newFileName = newPath . string( ) . replace( newPathStr . rfind( config . binType ), config . binType . length( ), "" );
					fs::copy_file( path, fs::path( newFileName ), fs::copy_options::overwrite_existing );
				} else fs::copy_file( path, newPath, fs::copy_options::overwrite_existing );
			}
		}
	}
}

bool validate( fs::path &path, std::string &bin ) { return path . filename( ) . string( ) == bin; }

std::unordered_map< std::string, fs::path > buildHashCache( const fs::path &folderPath )
{
	std::unordered_map< std::string, fs::path > hashMap;
	for ( const auto &entry : fs::recursive_directory_iterator( folderPath ) ) if ( fs::is_regular_file( entry ) ) hashMap[ calculateSHA256( entry ) ] = entry;
	return hashMap;
}

int main( int, char ** )
{
	try
	{
		[&] {}( );

		YAML::Node configYaml = YAML::LoadFile( "config.yml" );
		Config     config;
		config . source      = configYaml[ "source" ] . as< std::string >( );
		config . destination = configYaml[ "destination" ] . as< std::string >( );
		config . binSource   = configYaml[ "bin" ][ "source" ] . as< std::string >( );
		config . binType     = configYaml[ "bin" ][ "type" ] . as< std::string >( );
		config . ignore      = configYaml[ "ignore" ] . as< std::vector< std::string > >( );

		auto destinationMap = buildHashCache( config . destination );
		syncFolder( config, destinationMap );

		std::cout << "Sync completed!" << std::endl;
	} catch ( const std::exception &e ) { std::cerr << "Error: " << e . what( ) << std::endl; }

	return 0;
}
