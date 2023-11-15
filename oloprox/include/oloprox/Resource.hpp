#pragma once
#include <span>
#include <string_view>

class IData
{
protected:
	std::string_view m_path;
	std::span<uint8_t> m_data;
public:
	IData( ) = default;
	virtual uint8_t* data( ) const = 0;
	virtual size_t   size( ) const = 0;
};

template< class T > class CResource : public IData {};
