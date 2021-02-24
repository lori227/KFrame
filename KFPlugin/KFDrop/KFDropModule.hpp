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
#include "KFDisplay/KFDisplayInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFDeployClient/KFDeployClientInterface.h"

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

        // 卸载回调
        virtual void UnRegisterDropLogicFunction( const std::string& logic_name );

        ////////////////////////////////////////////////////////////////////////////////
        // 掉落
        virtual const DropDataList& Drop( EntityPtr player, uint32 drop_id, const std::string& module_name, uint64 module_id, const char* function, uint32 line );
        virtual const DropDataList& Drop( EntityPtr player, const UInt32Vector& drop_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line );
        virtual const DropDataList& Drop( EntityPtr player, uint32 drop_id, uint32 count, const std::string& module_name, uint64 module_id, const char* function, uint32 line );
    protected:
        // 关闭, 开启掉落功能
        __KF_DEPLOY_FUNCTION__( OnDeployDropClose );
        __KF_DEPLOY_FUNCTION__( OnDeployDropOpen );

        // 绑定掉落逻辑函数
        virtual void BindDropLogicFunction( const std::string& logic_name, KFModule* module, KFDropLogicFunction& function );

        // 设置掉落开关
        void SetDropLogicOpen( const std::string& logic_name, bool is_open );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 掉落
        __KF_ADD_ELEMENT_FUNCTION__( AddDropElement );

        // 掉落属性逻辑
        __KF_DROP_LOGIC_FUNCTION__( OnDropDataElement );

        // 随机掉落逻辑
        void RandDropLogic( EntityPtr player, uint32 drop_id, uint32 count, DropDataList& out_list, const char* function, uint32 line );
        void RandDropLogic( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list );

        // 执行掉落逻辑
        void ExecuteDropLogic( EntityPtr player, const DropDataList& out_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line );

        // 互斥条件掉落
        void DropMutexCondition( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list );
        void DropOverlayCondition( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list );

        // 掉落
        void RandDropDataList( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list );
        void RandDropDataByWeight( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list );
        void RandDropDataByProbability( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list );

        // 添加掉落返回数据
        void RandDropData( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list,
                           std::shared_ptr<const KFDropGroupWeight> drop_weight, UInt32Set& exclude_data_list, const char* function, uint32 line );

        // 发送掉落数据到客户端
#ifdef __KF_DEBUG__
        void SendDropDataToClient( EntityPtr player, uint32 drop_id, uint32 count, const DropDataList& drop_list );
#endif
    private:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _kf_component = nullptr;

        // 掉落逻辑函数
        KFMapModuleFunction<std::string, KFDropLogicFunction> _drop_logic_function;
    };
}

#endif