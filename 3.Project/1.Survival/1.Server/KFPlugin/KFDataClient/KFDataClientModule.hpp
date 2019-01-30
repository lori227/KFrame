#ifndef __KF_DATA_CLIENT_MODULE_H__
#define __KF_DATA_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    数据功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-2-1
************************************************************************/

#include "KFrame.h"
#include "KFDataClientInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFDataClientModule : public KFDataClientInterface
    {
    public:
        KFDataClientModule() = default;
        ~KFDataClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayerData( const KFMsg::PBLoginData* pblogin );

        // 保存玩家数据
        virtual bool SavePlayerData( uint64 playerid, const KFMsg::PBObject* pbplayerdata );

        // 查询玩家数据
        virtual bool QueryPlayerData( uint64 sendid, uint64 playerid );

        // 设置名字
        virtual bool SetPlayerName( uint64 playerid, const std::string& oldname, const std::string& newname, uint64 itemuuid );
    protected:
        // 设置回调函数
        virtual void SetLoadPlayerFunction( KFLoadPlayerFunction& function );
        virtual void SetQueryPlayerFunction( KFQueryPlayerFunction& function );
        virtual void SetPlayerNameFunction( KFSetPlayerNameFunction& function );

    protected:
        // 处理加载玩家数据
        __KF_MESSAGE_FUNCTION__( HandleLoadPlayerToGameAck );

        // 查询玩家数据
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerToGameAck );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToGameAck );

    private:
        KFLoadPlayerFunction _load_player_function = nullptr;
        KFQueryPlayerFunction _query_player_function = nullptr;
        KFSetPlayerNameFunction _set_player_name_function = nullptr;
    };


}

#endif