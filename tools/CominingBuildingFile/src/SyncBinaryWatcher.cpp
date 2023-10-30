#include <app/SyncBinaryWatcher.hpp>

void CSyncBinaryWatcher::onFileEvent( cppfs::FileHandle &fh, cppfs::FileEvent event )
{
	if ( !this -> valide_on_ignore( fh . path( ) ) ) return;

	auto oDest = this -> try_take_specific_path( fh . path( ) );

	if ( oDest ) dest_context = fs::path( *oDest );
	else dest_context         = m_pConfig -> destination / fs::relative( fh . path( ), m_pConfig -> source );

	switch ( event )
	{
		case cppfs::FileCreated: this -> onFileCreated( fh );
			return;
		case cppfs::FileRemoved: this -> onFileRemoved( fh );
			return;
		case cppfs::FileModified: this -> onFileModified( fh );
			return;
		case cppfs::FileAttrChanged: this -> onFileAttrChanged( fh );
	}
}

void CSyncBinaryWatcher::onFileCreated( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileCreated: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileRemoved( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileRemoved: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileModified( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileModified: {}", fh . path( ) ); }

void CSyncBinaryWatcher::onFileAttrChanged( cppfs::FileHandle &fh ) { m_logger -> debug( "onFileAttrChanged: {}", fh . path( ) ); }
