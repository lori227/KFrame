#ifndef __KF_TASK_INTERFACE_H__
#define __KF_TASK_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTaskInterface : public KFModule
    {
    public:
        // create task data with taskchain
        // @taskid			task config id
        // @status			task init status
        // @validtime		task valid time( must be finish in time )
        // @chainid			task chain config id
        // @chainindex		index in the task chain( default start from 1 )
        // @refreshid		task chain refresh config id
        virtual KFData* OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 validtime = 0u,
                                  uint32 chainid = 0u, uint32 chainindex = 1u, uint32 refreshid = 0u ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_task, KFTaskInterface );
    //////////////////////////////////////////////////////////////////////////
}

#endif