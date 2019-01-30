#ifndef __KF_MAIL_SHARD_MODULE_H__
#define __KF_MAIL_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMailShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFMailShardModule : public KFMailShardInterface
    {
    public:
        KFMailShardModule() = default;
        ~KFMailShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut ();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 计划清理过期的全局邮件
        __KF_SCHEDULE_FUNCTION__( OnScheduleClearWholeOverdueMail );

        // 增加邮件
        __KF_HTTP_FUNCTION__( HandleGMAddMailReq );

        // 删除邮件
        __KF_HTTP_FUNCTION__( HandleGMDeleteMailReq );

    protected:
        // 查询邮件
        __KF_MESSAGE_FUNCTION__ ( HandleQueryMailReq );

        // 添加邮件
        __KF_MESSAGE_FUNCTION__( HandleAddMailReq );

        // 设置邮件状态
        __KF_MESSAGE_FUNCTION__( HandleUpdateMailStatusReq );

        // 新玩家登陆邮件处理
        __KF_MESSAGE_FUNCTION__( HandleNewPlayerMailReq );

        ///////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理全局邮件到个人邮件
        void LoadGlobalMailToPerson( uint64 playerid, uint32 zoneid );

        // 设置邮件状态
        bool UpdateMailStatus( uint32 flag, uint64 playerid, uint64 mailid, uint32 status );

        // 添加邮件
        uint64 AddMail( uint32 flag, uint64 objectid, MapString& maildata );

    private:
        // 邮件数据库
        KFRedisDriver* _mail_driver{ nullptr };
    };
}

#endif