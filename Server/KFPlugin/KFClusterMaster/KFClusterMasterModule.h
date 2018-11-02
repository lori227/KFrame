#ifndef __KF_CLUSTER_MASTER_MODULE_H__
#define __KF_CLUSTER_MASTER_MODULE_H__

/************************************************************************
//    @Module			:    集群服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFrame.h"
#include "KFClusterMasterInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFClusterMasterModule : public KFClusterMasterInterface
    {
    public:
        KFClusterMasterModule();
        ~KFClusterMasterModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

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
        std::string MakeAuthToken( const KFGuid& guid );

        // 获得 objectid 的数量
        std::set< uint32 > GetShardObject( uint32 shardid );

        // 分配shard
        void BalanceAllocShard( uint32 shardid );

        // 判断是否拥有objectid
        bool HaveObject( uint32 shardid, uint32 objectid );

        // 查找拥有object的shard
        uint32 FindShard( uint32 objectid );

        // 发送分配shard到proxy
        void SendAllocShardToProxy( uint32 proxyid );

        // 发送分配shard到shard
        void SendAllocShardToShard();

    private:
        // 集群认证秘钥
        std::string _cluster_key;

        // 序列号
        uint32 _cluster_serial;

        // 记录shard分配数据
        std::map< uint32, uint32 > _object_to_shard;

        // shard存在的objectid
        std::map< uint32, std::set< uint32 > > _shard_objects;

        // 所有的objectid列表
        std::set< uint32 > _total_objects;
    };
}



#endif