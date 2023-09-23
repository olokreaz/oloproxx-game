#pragma once
#include <filesystem>
#include <efsw/efsw.h>
using fs = std::filesystem;

class IObserver
{
protected:
	virtual void _onAdd( fs::path path ) {}
	virtual void _onRemove( fs::path path ) {}
	virtual void _onModify( fs::path path ) {}
	virtual void _onMove( fs::path from, fs::path to ) {}

	virtual bool _filter( ) {}
	virtual void _update_context( fs::path path ) {}
};

template< typename T > requires std::derived_from< T, efsw::FileWatchListener >
class Handler final : public efsw::FileWatchListener {};
