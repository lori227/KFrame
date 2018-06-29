#ifndef __KF_DATA_CLIENT_MODULE_H__
#define __KF_DATA_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    数据功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-2-1
************************************************************************/

#include "KFrame.h"
#include "KFDataClientInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFDataClientModule : public KFDataClientInterface
    {
    public:
        KFDataClientModule();
        ~KFDataClientModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 发送消息到数据库
        virtual bool SendMessageToData( uint32 zoneid, uint32 msgid, ::google::protobuf::Message* message );

    };
}

#endif