#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <openssl/evp.h>

std::string getSHA256( const std::string &path )
{
	std::ifstream file( path );
	if ( !file . is_open( ) ) return "";

	EVP_MD_CTX *mdctx;
	if ( ( mdctx = EVP_MD_CTX_new( ) ) == nullptr ) return "";

	const EVP_MD *md = EVP_sha256( );
	EVP_DigestInit_ex( mdctx, md, nullptr );
	char buf[ 512 ];
	while ( file . good( ) ) {
		file . read( buf, sizeof( buf ) );
		EVP_DigestUpdate( mdctx, buf, file . gcount( ) );
	}

	unsigned char md_value[ EVP_MAX_MD_SIZE ];
	unsigned int  md_len = 0;
	EVP_DigestFinal_ex( mdctx, md_value, &md_len );
	EVP_MD_CTX_free( mdctx );

	std::stringstream ss;
	ss << std::hex << std::setfill( '0' );
	for ( int i = 0; i < md_len; i++ ) ss << std::setw( 2 ) << ( unsigned int ) md_value[ i ];

	return ss . str( );
}

void updateHashFile( const std::map< std::string, std::string > &shadersHash )
{
	std::ofstream out( "shaders_hash.txt" );
	if ( !out ) {
		std::cerr << "Couldn't open hash file for writing." << std::endl;
		return;
	}

	for ( const auto &[ file, hash ]: shadersHash ) out << file << " " << hash << std::endl;
}

int main( )
{
	std::string                          shaderDirectory = ".";  // Путь к директории со шейдерами
	std::map< std::string, std::string > shadersHash;

	std::ifstream hashFile( "shaders_hash.txt" );
	if ( hashFile ) {
		std::string line;
		while ( std::getline( hashFile, line ) ) {
			// Делим строку на имя файла и хеш 
			std::string file    = line . substr( 0, line . find( ' ' ) );
			std::string hash    = line . substr( line . find( ' ' ) + 1 );
			shadersHash[ file ] = hash;
		}
	}

	bool updated = false;

	for ( const auto &entry: std::filesystem::directory_iterator( shaderDirectory ) )
		if ( entry . path( ) . extension( ) == ".glsl" ) {
			std::string newHash = getSHA256( entry . path( ) . generic_string( ) );
			if ( shadersHash[ entry . path( ) . generic_string( ) ] != newHash ) {
				updated                                            = true;
				shadersHash[ entry . path( ) . generic_string( ) ] = newHash;
				std::string output                                 = entry . path( ) . stem( ) . generic_string( ) + ".spv";
				std::string command                                = "glslangValidator -V " + entry . path( ) . generic_string( )
											+ " --target-env vulkan1.3 -o " + output;
				std::system( command . c_str( ) );
			}
		}

	if ( updated ) updateHashFile( shadersHash );

	return 0;
}
