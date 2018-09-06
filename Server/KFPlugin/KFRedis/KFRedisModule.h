#ifndef __KF_REDIS_MODULE_H__
#define __KF_REDIS_MODULE_H__

/************************************************************************
//    @Module			:    redis数据库
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-11
************************************************************************/

#include "KFrame.h"
#include "KFRedisInterface.h"
#include "KFRedisExecute.h"

namespace KFrame
{
    class KFRedisModule : public KFRedisInterface
    {
    public:
        KFRedisModule();
        ~KFRedisModule();

        // 加载配置
        virtual void InitModule();
        virtual void AfterRun();

        // 关闭
        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 创建RedisExecute
        virtual KFRedisDriver* CreateExecute( uint32 id );
        virtual KFRedisDriver* CreateExecute( const std::string& field, uint32 logicid = 0 );
        virtual KFRedisDriver* CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password );

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
}



#endif