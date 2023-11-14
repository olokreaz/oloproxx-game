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

#include <magic_enum_all.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
export module systems.shader;
import systems.utils;
import types;

namespace fs = std::filesystem;

namespace systems::shader
{
	export enum class ELanguageShader
	{
		eNone = 0 ,
		eGlsl ,
		eHlsl ,
	};

	std::vector< uint32 > CompileGLSLToSPIRV( const std::string &shaderCode ) noexcept
	{
		// Создаём временный файл для исходного кода shader
		const fs::path glslPath = fs::temp_directory_path( ) / "oloprox" / utils::create_guid( );
		std::ofstream( glslPath . string( ) ) << shaderCode;

		// Генерируем имя для выходного файла SPIR-V
		const fs::path spvPath = fs::temp_directory_path( ) / "oloprox" / utils::create_guid( );

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
			spdlog::critical( "GLSL compilation failed" );
			exit( 1 );
		}

		// Читаем скомпилированный SPIR-V из временного файла
		std::ifstream           spvFile( spvPath . string( ), std::ios::binary );
		std::vector< uint32_t > spirv(
						( std::istreambuf_iterator( spvFile ) ),
						std::istreambuf_iterator< char >( )
						);

		return spirv;
	}

	std::vector< uint32 > CompileHLSLToSPIRV( const std::string &shaderCode, const std::string &entryPoint = "main" )
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
			const std::string &   shaderCodeOrPath,
			const ELanguageShader eShaderType = ELanguageShader::eNone,
			const bool            bIsPath     = false,
			const std::string &   entryPoint  = "main"
			)
	{
		std::string shaderCode;

		auto e = eShaderType;

		if ( bIsPath )
		{
			const boost::filesystem::path p( shaderCodeOrPath );

			e = ( bool ) e ? e : p . extension( ) . string( ) == ".hlsl" ? ELanguageShader::eHlsl : ELanguageShader::eGlsl;

			try
			{
				const boost::iostreams::mapped_file_source mmap( p );
				shaderCode . assign( mmap . begin( ), mmap . end( ) );
			} catch ( ... )
			{
				spdlog::error( "Failed to load shader file: {}", shaderCodeOrPath );
				exit( 1 );
			}
		} else shaderCode = shaderCodeOrPath;

		if ( e == ELanguageShader::eGlsl ) return CompileGLSLToSPIRV( shaderCode );
		if ( e == ELanguageShader::eHlsl ) return CompileHLSLToSPIRV( shaderCode, entryPoint );
		return { };
	}
}
