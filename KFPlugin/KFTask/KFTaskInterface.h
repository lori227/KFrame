#ifndef __KF_TASK_INTERFACE_H__
#define __KF_TASK_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTaskInterface : public KFModule
    {
    public:
        // 开启任务
        virtual KFData* OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time ) = 0;

        // 开启任务链中的任务
        virtual KFData* OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time, uint32 refreshid, uint32 taskchainid, uint32 order, const UInt32List& logicids ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_task, KFTaskInterface );
    //////////////////////////////////////////////////////////////////////////
}

#endif