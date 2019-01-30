#ifndef __KF_ATTRIBUTE_MODULE_H__
#define __KF_ATTRIBUTE_MODULE_H__

/************************************************************************
//    @Module			:    属性逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-12-27
************************************************************************/

#include "KFAttributeInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFCommand/KFCommandInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFDataClient/KFDataClientInterface.h"

namespace KFrame
{
    class KFAttributeModule : public KFAttributeInterface
    {
    public:
        KFAttributeModule() = default;
        ~KFAttributeModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加属性
        __KF_COMMAND_FUNCTION__( OnCommandAddData );

        // 减少属性
        __KF_COMMAND_FUNCTION__( OnCommandDecData );

        // 设置属性
        __KF_COMMAND_FUNCTION__( OnCommandSetData );

    protected:
        // 查询玩家所有数据
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq );

        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq );

        // 删除属性请求
        __KF_MESSAGE_FUNCTION__( HandleRemoveDataReq );

    protected:
        // 查询玩家数据
        void OnAfterQueryPlayerData( uint32 result, uint64 playerid, KFMsg::PBObject* playerdata );

        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name );

        // 设置名字回调
        void OnAfterSetPlayerName( uint32 result, uint64 playerid, const std::string& name, uint64 itemuuid );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新属性到客户端
        void SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步添加属性到客户端
        void SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 显示添加奖励消息
        void SendElementToClient( KFEntity* player, const std::string& element );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        KFComponent* _kf_component = nullptr;
    };
}



#endif