#ifndef __KF_DROP_MOUDLE_H__
#define __KF_DROP_MOUDLE_H__

/************************************************************************
//    @Module			:    掉落系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-20
************************************************************************/

#include "KFDropInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFDropConfig.hpp"

namespace KFrame
{
    class KFDropModule : public KFDropInterface
    {
    public:
        KFDropModule() = default;
        ~KFDropModule() = default;

        // 初始化
        virtual void InitModule();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 掉落( 直接添加属性 )
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line );
    protected:
        // 掉落
        void Drop( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line );

        // 随机掉落
        void DropDataByRand( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line );

        // 权重掉落
        void DropDataByWeight( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line );

        // 判断掉落次数
        bool CheckDropVarCount( KFEntity* player, const KFDropSetting* kfsetting );

        // 更新掉落词素
        void ResetDropVarCount( KFEntity* player, const KFDropSetting* kfsetting );
    };
}

#endif