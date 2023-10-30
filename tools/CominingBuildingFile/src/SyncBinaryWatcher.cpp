#include <app/SyncBinaryWatcher.hpp>

void CSyncBinaryWatcher::onFileEvent( cppfs::FileHandle &fh, cppfs::FileEvent event )
{
	if ( !this -> valide_on_ignore( fh . path( ) ) ) return;


	switch ( event )
	{
		case cppfs::FileCreated: this -> onFileCreated( fh );
			return;
		case cppfs::FileRemoved: this -> onFileRemoved( fh );
			return;
		case cppfs::FileModified: this -> onFileModified( fh );
			return;
		case cppfs::FileAttrChanged: this -> onFileAttrChanged( fh );
			return;;
	}
	std::unreachable( );
}

void CSyncBinaryWatcher::onFileCreated( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileCreated: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileRemoved( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileRemoved: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileModified( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileModified: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileAttrChanged( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileAttrChanged: {}", fh . path( ) ); }
