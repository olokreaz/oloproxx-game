module;

#define NODISCARD [[nodiscard]]
#define EXPORT export NODISCARD

#include <csignal>
#include <fstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <glslang/Public/ShaderLang.h>

#include <spdlog/sinks/stdout_color_sinks.h>

#include <vulkan/vulkan.hpp>

export module system;

import types;

namespace fs = boost::filesystem;

namespace sys
{
	EXPORT std::vector< uint8 > readFile( const fs::path &path )
	{
		boost::iostreams::mapped_file_source file;
		file . open( path );

		if ( file . is_open( ) )
		{
			std::string ctx( file . data( ), file . size( ) );
			file . close( );

			std::vector< uint8 > result;
			result . resize( file . size( ) );
			result . assign( file . data( ), file . data( ) + file . size( ) );

			spdlog::info( "Read file: {}", path . string( ) );
			return result;
		}

		spdlog::warn( "Error opening file" );
		throw std::underflow_error( "Error opening file" );
	}

	export enum class EWindowStatus
	{
		eHide ,
		eShow
	};

	export class Console
	{
		Console( ) = delete;

	public:
		static void initialize( )
		{
			if ( !m_hConsole )
			{
				m_hConsole = GetConsoleWindow( );
				spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [thread %t] [---%^%l%$---] <%n>: %v" );
				set_default_logger( _log );
			}
		}

	protected:
		inline static auto _log = spdlog::stdout_color_mt( "global" );

	public:
		static void regSignal( std::function< void( int32 ) > func )
		{
			std::signal( SIGINT, func . target< void( int32 ) >( ) );
			std::signal( SIGTERM, func . target< void( int32 ) >( ) );
			std::signal( SIGABRT, func . target< void( int32 ) >( ) );
			std::signal( SIGBREAK, func . target< void( int32 ) >( ) );

			spdlog::info( "Register Signal Handler: {}", func . target_type( ) . name( ) );
		}

		static void setConsoleTitle( const std::string &tittle )
		{
			SetConsoleTitleA( tittle . c_str( ) );
			spdlog::info( "Set Console Tittle: {}", tittle );
		}

		static void setLogLevel( const spdlog::level::level_enum level )
		{
			spdlog::set_level( level );
			spdlog::info( "Set Log Level: {}", std::to_underlying< spdlog::level::level_enum >( level ) );
		}

		auto static setStatus( const EWindowStatus status, const HWND hwnd = nullptr )
		{
			const int  command = status == EWindowStatus::eShow ? SW_SHOW : SW_HIDE;
			const HWND window  = hwnd ? hwnd : m_hConsole;

			ShowWindow( window, command );

			if ( window == m_hConsole )
			{
				m_status = status;
				spdlog::info( "{} Console", status == EWindowStatus::eShow ? "Show" : "Hide" );
			} else spdlog::info( "{} Window: {}", status == EWindowStatus::eShow ? "Show" : "Hide", hwnd ? hwnd -> unused : 0 );
		}

		auto static getState( ) { return m_status; }

	private:
		inline static EWindowStatus m_status;
		inline static HWND          m_hConsole;
	};

	std::vector< uint32_t > CompileGLSLToSPIRV( const std::string &shaderCode )
	{
		// Создаём временный файл для исходного кода shader
		const fs::path glslPath = fs::temp_directory_path( ) / fs::unique_path( );
		std::ofstream( glslPath . string( ) ) << shaderCode;

		// Генерируем имя для выходного файла SPIR-V
		const fs::path spvPath = fs::temp_directory_path( ) / fs::unique_path( );

		// Выполняем компиляцию с помощью glslc из Vulkan SDK
		auto                  command = fmt::format( "glslc -fshader-stage=vert {} -o {}", glslPath . string( ), spvPath . string( ) );
		boost::process::child process(
						command,
						boost::process::std_out > boost::process::null
						);
		process . wait( );

		// Получаем код возврата
		if ( const int result = process . exit_code( ); result != 0 )
		{
			spdlog::error( "GLSL compilation failed" );
			throw std::runtime_error( "GLSL compilation failed" );
		}

		// Читаем скомпилированный SPIR-V из временного файла
		std::ifstream           spvFile( spvPath . string( ), std::ios::binary );
		std::vector< uint32_t > spirv(
						( std::istreambuf_iterator( spvFile ) ),
						std::istreambuf_iterator< char >( )
						);

		return spirv;
	}

	std::vector< uint32_t > CompileHLSLToSPIRV( const std::string &shaderCode, const std::string &entryPoint = "main" )
	{
		// Генерируем имена временных файлов
		const fs::path hlslPath = fs::temp_directory_path( ) / fs::unique_path( );
		const fs::path spvPath  = fs::temp_directory_path( ) / fs::unique_path( );

		// Записываем исходный код во временный файл
		std::ofstream( hlslPath . string( ) ) << shaderCode;

		// Вызываем dxc для компиляции в SPIR-V
		auto                  command = fmt::format( "dxc -T ps_6_0 -E {} -spirv {} -Fo {}", entryPoint, hlslPath . string( ), spvPath . string( ) );
		boost::process::child process(
						command,
						boost::process::std_out > boost::process::null
						);
		process . wait( );

		// Получить код возврата
		if ( const int result = process . exit_code( ); result != 0 )
		{
			spdlog::error( "Failed to compile HLSL shader" );
			throw std::runtime_error( "Failed to compile HLSL shader" );
		}

		// Читаем скомпилированный SPIR-V из временного файла
		std::ifstream           spvFile( spvPath . string( ), std::ios::binary );
		std::vector< uint32_t > spirv(
						( std::istreambuf_iterator( spvFile ) ),
						std::istreambuf_iterator< char >( )
						);

		return spirv;
	}

	std::vector< uint32_t > CompileShaderToSPIRV(
			const std::string &shaderCodeOrPath, const std::string &shaderTypeOrExtension, const std::string &entryPoint = "main",
			const bool         isPath                                                                                    = false
			)
	{
		std::string shaderType = shaderTypeOrExtension;
		std::string shaderCode;
		if ( isPath )
		{
			const boost::filesystem::path p( shaderCodeOrPath );
			shaderType = p . extension( ) . string( ) . substr( 1 ); // remove .

			// Use boost::filesystem::ifstream instead
			boost::filesystem::ifstream inFile( p, std::ios::in | std::ios::binary );
			if ( !inFile )
			{
				spdlog::error( "Failed to load shader file: {}", shaderCodeOrPath );
				throw std::runtime_error( "Failed to load shader file: " + shaderCodeOrPath );
			}

			shaderCode = { std::istreambuf_iterator( inFile ), std::istreambuf_iterator< char >( ) };
			inFile . close( );
		} else shaderCode = shaderCodeOrPath;

		if ( shaderType == "glsl" ) return CompileGLSLToSPIRV( shaderCode );
		if ( shaderType == "hlsl" ) return CompileHLSLToSPIRV( shaderCode, entryPoint );
		spdlog::error( "Unknown shader type: {}", shaderType );
		throw std::runtime_error( "Unknown shader type: " + shaderType );
	}
}
