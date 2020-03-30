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
#include "KFDropGroupConfig.hpp"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFCondition/KFConditionInterface.h"

namespace KFrame
{
    class KFDropModule : public KFDropInterface
    {
    public:
        KFDropModule() = default;
        ~KFDropModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        virtual void UnRegisterDropLogicFunction( const std::string& dataname );
        ////////////////////////////////////////////////////////////////////////////////
        // 掉落
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );
        virtual DropDataList& Drop( KFEntity* player, const UInt32Vector& droplist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );
        virtual DropDataList& Drop( KFEntity* player, uint32 dropid, uint32 count, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

    protected:
        // 绑定掉落逻辑函数
        virtual void BindDropLogicFunction( const std::string& dataname, KFDropLogicFunction& function );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 掉落
        __KF_ADD_ELEMENT_FUNCTION__( AddDropElement );

        // 掉落属性逻辑
        __KF_DROP_LOGIC_FUNCTION__( OnDropDataElement );

        // 掉落逻辑
        DropDataList& DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line );

        // 掉落
        void DropLogic( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist );
        void DropLogic( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line );

        // 互斥条件掉落
        void DropMutexCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist );
        void DropOverlayCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist );

        // 掉落
        void RandDropDataList( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist );
        void RandDropDataByWeight( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist );
        void RandDropDataByProbability( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist );

        // 添加掉落返回数据
        void RandDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropGroupWeight* kfdropweight, UInt32Set& excludedatalist, const char* function, uint32 line );

        // 发送掉落数据到客户端
#ifdef __KF_DEBUG__
        void SendDropDataToClient( KFEntity* player, uint32 dropid, uint32 count, DropDataList& droplist );
#endif
    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 掉落逻辑函数
        KFBind < std::string, const std::string&, KFDropLogicFunction > _drop_logic_function;
    };
}

#endif