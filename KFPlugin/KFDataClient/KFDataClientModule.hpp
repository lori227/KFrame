#ifndef __KF_DATA_CLIENT_MODULE_H__
#define __KF_DATA_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    数据功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-2-1
************************************************************************/

#include "KFDataClientInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFDataKeeper
    {
    public:
        // 小区id
        uint32 _zone_id = 0u;

        // 玩家id
        uint64 _player_id = 0u;

        // 玩家数据
        KFMsg::PBObject _pb_object;

        // 保存标记
        uint32 _save_flag = 0u;

        // 保存时间
        uint64 _save_time = 0u;
    };

    class KFLoadKeeper
    {
    public:
        // 玩家数据
        KFMsg::PBLoginData _pb_login;

        // 保存时间
        uint64 _load_time = 0u;
    };

    class KFDataClientModule : public KFDataClientInterface
    {
    public:
        KFDataClientModule() = default;
        ~KFDataClientModule() = default;

        // 刷新
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayerData( const KFMsg::PBLoginData* login_data );
        virtual void RemoveLoadData( uint64 player_id );

        // 保存玩家数据
        virtual bool SavePlayerData( uint64 player_id, const KFMsg::PBObject* player_data, uint32 save_flag );

        // 查询玩家数据
        virtual bool QueryPlayerData( uint64 send_id, uint64 player_id );
    protected:
        // 设置回调函数
        virtual void SetLoadPlayerFunction( KFModule* module, KFLoadPlayerFunction& function );
        virtual void SetQueryPlayerFunction( KFModule* module, KFQueryPlayerFunction& function );

        // 保存数据
        void SaveKeeperData( uint32 zone_id, uint64 player_id, const KFMsg::PBObject* player_data, uint32 save_flag );

        // 加载数据
        void LoadKeeperData( const KFMsg::PBLoginData* login_data );

        // 计算小区id
        uint32 CalcZoneId( uint64 player_id );
    protected:
        // 保存玩家
        __KF_MESSAGE_FUNCTION__( HandleSavePlayerToGameAck, KFMsg::S2SSavePlayerToGameAck );

        // 处理加载玩家数据
        __KF_MESSAGE_FUNCTION__( HandleLoadPlayerToGameAck, KFMsg::S2SLoadPlayerToGameAck );

        // 查询玩家数据
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerToGameAck, KFMsg::S2SQueryPlayerToGameAck );


    private:
        KFModuleFunction<KFLoadPlayerFunction> _load_player_function;
        KFModuleFunction<KFQueryPlayerFunction> _query_player_function;

        // 数据保存
        KFHashMap<uint64, KFDataKeeper> _data_keeper;

        // 数据加载
        KFHashMap<uint64, KFLoadKeeper> _load_keeper;
    };


}

#endif