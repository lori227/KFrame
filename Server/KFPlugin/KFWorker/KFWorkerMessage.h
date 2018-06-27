#ifndef __KF_WORKER_MESSAGE_H__
#define __KF_WORKER_MESSAGE_H__

#include "KFrame.h"

namespace KFrame
{
	__MT_CLASS__( KFWorkerMessage )
	{
	public:
		KFWorkerMessage( );
		~KFWorkerMessage();

		void CopyFrom( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length );

	public:
		KFGuid _kfguid;
		uint32 _msgid;
		char* _data;
		uint32 _length;
	};
}


#endif