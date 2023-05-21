#pragma once

class ISocket {
public:
	virtual void run( ) =0;
	virtual void recv( ) = 0;
	virtual void update( ) = 0;
	virtual void stop( ) = 0;
};
