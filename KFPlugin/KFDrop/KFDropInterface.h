#ifndef __KF_DROP_INTERFACE_H__
#define __KF_DROP_INTERFACE_H__

#include "KFrame.h"
#include "KFZConfig/KFDropSetting.h"

namespace KFrame
{
    typedef std::list< const KFDropData* > DropDataList;
    class KFDropInterface : public KFModule
    {
    public:
        // 掉落( 直接添加属性 )
        virtual DropDataList& DropElement( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line ) = 0;

        // 掉落逻辑
        virtual DropDataList& DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_drop, KFDropInterface );
    //////////////////////////////////////////////////////////////////////////
}

#endif