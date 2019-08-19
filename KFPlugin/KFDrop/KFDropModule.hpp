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
        // 掉落( 直接添加属性 )
        virtual DropDataList& DropElement( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line );

        // 掉落逻辑
        virtual DropDataList& DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line );
    protected:
        // 掉落
        void Drop( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist );
        void Drop( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line );

        // 更新掉落次数
        void AddDropVarCount( KFEntity* player, const KFDropSetting* kfsetting );

        // 更新掉落次数
        void ResetDropVarCount( KFEntity* player, const KFDropSetting* kfsetting );

        // 添加掉落返回数据
        void AddDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropWeight* kfdropweight, const char* function, uint32 line );
    };
}

#endif