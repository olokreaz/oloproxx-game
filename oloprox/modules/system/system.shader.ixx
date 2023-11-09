module;

#include <fstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

export module system.shader;
import system.utils;
import types;

namespace fs = std::filesystem;

namespace system::shader
{
	export std::vector< uint32 > CompileGLSLToSPIRV( const std::string &shaderCode )
	{
		// Создаём временный файл для исходного кода shader
		const fs::path glslPath = fs::temp_directory_path( ) / utils::create_guid( );
		std::ofstream( glslPath . string( ) ) << shaderCode;

		// Генерируем имя для выходного файла SPIR-V
		const fs::path spvPath = fs::temp_directory_path( ) / utils::create_guid( );

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

	export std::vector< uint32 > CompileHLSLToSPIRV( const std::string &shaderCode, const std::string &entryPoint = "main" )
	{
		// Генерируем имена временных файлов
		const fs::path hlslPath = fs::temp_directory_path( ) / utils::create_guid( );
		const fs::path spvPath  = fs::temp_directory_path( ) / utils::create_guid( );

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

	export std::vector< uint32 > CompileShaderToSPIRV(
			const std::string &shaderCodeOrPath, const std::string &shaderTypeOrExtension = "glsl",
			const std::string &entryPoint                                                 = "main", const bool isPath = false
			)
	{
		std::string shaderType = shaderTypeOrExtension;
		std::string shaderCode;

		if ( isPath )
		{
			const boost::filesystem::path p( shaderCodeOrPath );
			shaderType = p . extension( ) . string( ) . substr( 1 ); // remove '.'
			try
			{
				// Маппинг файла в память
				const boost::iostreams::mapped_file_source mmap( p . string( ) );
				// Копирование данных из мемори-маппед файла в std::string
				shaderCode . assign( mmap . data( ), mmap . size( ) );
			} catch ( ... )
			{
				spdlog::error( "Failed to load shader file: {}", shaderCodeOrPath );
				exit( 1 );
			}
		} else shaderCode = shaderCodeOrPath;

		return shaderType == "glsl"
				? CompileGLSLToSPIRV( shaderCode )
				: shaderType == "hlsl"
					? CompileHLSLToSPIRV( shaderCode, entryPoint )
					: [&shaderType]
					{
						spdlog::error( "Unknown shader type: {}", shaderType );
						return std::vector< uint32 >( );
					}( );
	}
}
