#ifndef __KF_TASK_INTERFACE_H__
#define __KF_TASK_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTaskInterface : public KFModule
    {
    public:
        // create task data with task chain
        // @task_id			task config id
        // @status			task init status
        // @valid_time		task valid time( must be finish in time )
        // @chain_id			task chain config id
        // @chain_index		index in the task chain( default start from 1 )
        virtual DataPtr OpenTask( EntityPtr player, uint32 task_id, uint32 status, uint64 valid_time = 0u,
                                  uint32 chain_id = 0u, uint32 chain_index = 1u ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_task, KFTaskInterface );
    //////////////////////////////////////////////////////////////////////////
}

#endif