#include "KFLogMemory.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	void LogData::AddData( uint64 size, uint64 totalsize )
	{
		++_count;
		_use_size += size;
		_total_size = totalsize;
	}

	void LogData::DecData( uint64 size )
	{
		_count -= __MIN__( _count, 1 );
		_use_size -= __MIN__( _use_size, size );
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	void LogBlock::AddBlock( const std::string& type, uint64 size, uint64 totalsize )
	{
		LogData* pdata = FindData( type );
		pdata->AddData( size, totalsize );
	}

	void LogBlock::DecBlock( const std::string& type, uint64 size )
	{
		LogData* pdata = FindData( type );
		pdata->DecData( size );
	}

	LogData* LogBlock::FindData( const std::string& type )
	{
		auto iter = _log_data.find( type );
		if ( iter == _log_data.end() )
		{
			auto logdata = new LogData();
			iter = _log_data.insert( std::make_pair( type, logdata ) ).first;
		}

		return iter->second;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
}
