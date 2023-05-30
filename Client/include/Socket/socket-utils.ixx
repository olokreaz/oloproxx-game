module;
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <spdlog/spdlog.h>

export module socket:utils;

namespace socket
{
	static std::mutex                mutexUserInputQueue;
	static std::queue< std::string > queueUserInput;

	void LocalUserInput_Init( std::thread *pLocalUserInputThread, bool *bQuit )
	{
		pLocalUserInputThread = new std::thread( [&]( )
								{
									while ( !( *bQuit ) ) {
										std::string buf;
										if ( !( std::cin >> buf ) ) {
											// Well, you would hope that you could close the handle
											// from the other thread to trigger this.  Nope.
											if ( *bQuit ) return;
											*bQuit = true;
											spdlog::error( "Failed to read on stdin, quitting"
												);
											break;
										}

										mutexUserInputQueue.lock( );
										queueUserInput.
												push( std::move( buf
														)
													);
										mutexUserInputQueue.unlock( );
									}
								}
							);
	}

	bool LocalUserInput_GetNext( std::string &result )
	{
		bool got_input = false;
		mutexUserInputQueue.lock( );
		while ( !queueUserInput.empty( ) && !got_input ) {
			result = queueUserInput.front( );
			queueUserInput.pop( );
			// ltrim(result);
			// rtrim(result);
			got_input = !result.empty( ); // ignore blank lines
		}
		mutexUserInputQueue.unlock( );
		return got_input;
	}
}
