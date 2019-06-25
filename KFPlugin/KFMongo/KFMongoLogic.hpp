#ifndef __KF_MONGO_LOGIC_H__
#define __KF_MONGO_LOGIC_H__

#include "KFMongoInterface.h"
#include "KFMongoExecute.hpp"

namespace KFrame
{
    // 读写分离,
    // 也支持单一节点, 配置表设置同一个数据库
    class KFMongoLogic : public KFMongoDriver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFMongoLogic();
        virtual ~KFMongoLogic();

        void ShutDown();

        void Initialize( KFMongoType* kfmongotype );
        /////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 读执行器
        KFReadExecute* _read_execute = nullptr;

        // 写执行器
        KFWriteExecute* _write_execute = nullptr;
    };
}
#endif