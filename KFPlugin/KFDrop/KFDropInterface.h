#ifndef __KF_DROP_INTERFACE_H__
#define __KF_DROP_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFEntity;
    class KFElements;
    typedef  std::list< const KFElements* > DropDataList;

    class KFDropInterface : public KFModule
    {
    public:
        // 掉落( 直接添加属性 )
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_drop, KFDropInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif