#ifndef __KF_REDIS_MANAGE_H__
#define __KF_REDIS_MANAGE_H__

#include "KFRedisExecute.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
    class KFRedisManage : public KFSingleton< KFRedisManage >
    {
    public:
        KFRedisManage();
        virtual ~KFRedisManage();

        // 创建redis
        KFRedisExecute* CreateExecute( uint32 id );
        KFRedisExecute* CreateExecute( const std::string& field, uint32 logicid );
        KFRedisExecute* CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 关闭
        void ShutDown();

    protected:
        // 查找
        KFRedisExecute* FindRedisExecute( uint32 id );

        // 插入redis
        void InsertRedisExecute( uint32 id, KFRedisExecute* kfredisexecute );

    private:
        // 线程锁
        KFMutex _mt_mutex;

        // 多线程列表
        typedef std::pair< uint32, uint32 > RedisKey;
        KFMap< RedisKey, const RedisKey&, KFRedisExecute > _redis_execute;
    };

    ////////////////////////////////////////////////////////////////////
    static auto _kf_redis_manage = KFRedisManage::Instance();
    ////////////////////////////////////////////////////////////////////
}

#endif