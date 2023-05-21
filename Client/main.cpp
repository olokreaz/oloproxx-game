#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>

#include <coroutine>
#include "include/App/App.h"

using namespace std;
using namespace fmt::literals;

namespace vs = views;
namespace rng = ranges;

#ifndef _DEBUG
#include <spdlog/sinks/basic_file_sink.h>
static inline shared_ptr< spdlog::logger > g_logger
		= spdlog::basic_logger_mt( "Global"
					   , fmt::format( "logs/{:%Y-%m-%d}.log"
							  , fmt::localtime( std::time( nullptr ) )
							)
					 );
#endif

CApp_t g_app( new CApp( ) );

template< class T > class CPackage {
	T m_content;

public:
	int32 const size = { sizeof T };

	T* operator->( ) { return &m_content; }
};

int main( int, char ** )
{
	#ifndef _DEBUG
	set_default_logger( g_logger );
	#endif
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v" );

	tf::Taskflow tf( "main" );

	struct testData {
		string id;
		string name;

		bool check_pass( string &pass ) { return password_crypted == pass; }
		void setPassword( string npass ) { password_crypted = npass; }

	protected:
		string password_crypted;
	};

	CPackage< testData > Package;

	Package->id   = "087B0DBF-0CB9-4611-A68B-BA6F46DEF4C3";
	Package->name = "Pidorasin";
	Package->setPassword( "123456" );

	vector< std::byte > buffer( sizeof Package );

	memcpy( buffer.data( ), &Package, Package.size );

	fmt::print( "buffer: {}\n", buffer );

	/// next programm

	auto buffer2 = buffer;
	fmt::print( "buffer2: {}\n", buffer2 );

	CPackage< testData > Package2 = *reinterpret_cast< CPackage< testData > * >( buffer2.data( ) );
	fmt::print( "data: \nid: {}\nname: {}", Package2->id, Package2->name );

	return 0;
	// return app->run( );
}
