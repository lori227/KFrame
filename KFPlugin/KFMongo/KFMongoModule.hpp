#ifndef __KF_MONGO_MODULE_H__
#define __KF_MONGO_MODULE_H__

/************************************************************************
//    @Module			:    mongo数据库
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-6-24
************************************************************************/

#include "KFMongoInterface.h"
#include "KFMongoLogic.hpp"

namespace KFrame
{
    class KFMongoModule : public KFMongoInterface
    {
    public:
        KFMongoModule() = default;
        ~KFMongoModule() = default;

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 创建Execute
        virtual KFMongoDriver* Create( const std::string& module, uint32 logicid = 0 );

    protected:
        // 查询
        KFMongoLogic* FindMongoLogic( uint32 id );

        // 插入
        void InsertMongoLogic( uint32 id, KFMongoLogic* kflogic );

    private:
        // 互斥量
        KFMutex _kf_mongo_mutex;

        // 数据库列表
        typedef std::pair< uint32, uint32 > MongoKey;
        KFMap< MongoKey, KFMongoLogic > _mongo_logic_map;
    };
}



#endif