#ifndef __KF_MAIL_DATABASE_MONGO_H__
#define __KF_MAIL_DATABASE_MONGO_H__

#include "KFMailDatabaseLogic.hpp"
#include "KFMongo/KFMongoInterface.h"

namespace KFrame
{
    class KFMailDatabaseMongo : public KFMailDatabaseLogic
    {
    public:
        KFMailDatabaseMongo();
        virtual ~KFMailDatabaseMongo() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 添加邮件到数据库, 返回邮件id
        virtual uint64 AddMail( uint32 flag, uint64 objectid, StringMap& maildata, uint32 validtime );

        // 删除邮件
        virtual void RemoveMail( uint64 mailid );
        virtual void RemoveMail( const UInt64List& mailidlist );

        // 删除过期的全局邮件
        virtual void ClearOverdueGlobalMail();

        // 把全局邮件列表加到玩家列表中
        virtual void LoadGlobalMailToPerson( uint64 playerid, uint32 zoneid );

        // 查询邮件列表
        virtual KFResult< StringMapList >::UniqueType QueryMailList( uint64 playerid, uint64 lastmailid );

        // 更新邮件状态
        virtual bool UpdateMailStatus( uint32 flag, uint64 playerid, uint64 mailid, uint32 status );

        // 初始化新玩家邮件数据
        virtual void InitNewPlayerMail( uint64 playerid, uint32 zoneid );

    protected:
        // 初始化全局邮件id
        void InitNewPlayerGlobalMail( uint64 playerid, uint32 zoneid );

    private:
        KFMongoDriver* _mongo_driver = nullptr;
    };
}



#endif