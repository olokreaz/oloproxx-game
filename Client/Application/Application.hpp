#pragma once

class CApplication {
	bool bQuit = { false };

public:
	operator bool( ) { return bQuit; }
};
