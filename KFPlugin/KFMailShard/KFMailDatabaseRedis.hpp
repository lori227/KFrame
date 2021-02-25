#ifndef __KF_MAIL_DATABASE_REDIS_H__
#define __KF_MAIL_DATABASE_REDIS_H__

#include "KFMailDatabaseLogic.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    class KFMailDatabaseRedis : public KFMailDatabaseLogic
    {
    public:
        KFMailDatabaseRedis();
        virtual ~KFMailDatabaseRedis() = default;
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 添加邮件到数据库, 返回邮件id
        virtual uint64 AddMail( uint32 flag, uint64 object_id, StringMap& mail_data, uint32 valid_time );

        // 删除邮件
        virtual void RemoveMail( uint64 mail_id );
        virtual void RemoveMail( const UInt64List& mail_id_list );

        // 删除过期的全局邮件
        virtual void ClearOverdueGlobalMail();

        // 把全局邮件列表加到玩家列表中
        virtual void LoadGlobalMailToPerson( uint64 player_id, uint32 zone_id );

        // 查询邮件列表
        virtual KFResult<StringMapList>::UniqueType QueryMailList( uint64 player_id, uint64 last_mail_id );

        // 更新邮件状态
        virtual bool UpdateMailStatus( uint32 flag, uint64 player_id, uint64 mail_id, uint32 status );

        // 初始化新玩家邮件数据
        virtual void InitNewPlayerMail( uint64 player_id, uint32 zone_id );

    protected:
        // 初始化全局邮件id
        void InitNewPlayerGlobalMail( uint64 player_id, uint32 zone_id );
    private:
        KFRedisDriver* _redis_driver = nullptr;
    };
}



#endif