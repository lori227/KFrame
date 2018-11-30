#ifndef __KF_CLUSTER_MASTER_MODULE_H__
#define __KF_CLUSTER_MASTER_MODULE_H__

/************************************************************************
//    @Module			:    集群服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFClusterMasterInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFClusterMasterModule : public KFClusterMasterInterface
    {
    public:
        KFClusterMasterModule() = default;
        ~KFClusterMasterModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 注册
        __KF_MESSAGE_FUNCTION__( HandleClusterRegisterReq );

        // 更新连接数量
        __KF_MESSAGE_FUNCTION__( HandleClusterUpdateReq );

        // 登录认证
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthReq );

        // 请求分配shard
        __KF_MESSAGE_FUNCTION__( HandleAllocObjectToMasterReq );

    protected:
        // 连接丢失
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClusterProxy );

    private:
        // 生成认证token
        std::string MakeAuthToken( const KFId& kfid );

        // 获得 objectid 的数量
        std::set< uint64 > GetShardObject( uint64 shardid );

        // 分配shard
        void BalanceAllocShard( uint64 shardid );

        // 判断是否拥有objectid
        bool HaveObject( uint64 shardid, uint64 objectid );

        // 查找拥有object的shard
        uint64 FindShard( uint64 objectid );

        // 发送分配shard到proxy
        void SendAllocShardToProxy( uint64 proxyid );

        // 发送分配shard到shard
        void SendAllocShardToShard();

    private:
        // 序列号
        uint64 _cluster_serial{0};

        // 记录shard分配数据
        std::map< uint64, uint64 > _object_to_shard;

        // shard存在的objectid
        std::map< uint64, std::set< uint64 > > _shard_objects;

        // 所有的objectid列表
        std::set< uint64 > _total_objects;
    };
}



#endif