#ifndef __KF_MAIL_DATABASE_LOGIC_H__
#define __KF_MAIL_DATABASE_LOGIC_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMailDatabaseLogic
    {
    public:
        KFMailDatabaseLogic() = default;
        virtual ~KFMailDatabaseLogic() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加邮件到数据库, 返回邮件id
        virtual uint64 AddMail( uint32 flag, uint64 object_id, StringMap& mail_data, uint32 valid_time ) = 0;

        // 删除邮件
        virtual void RemoveMail( uint64 mail_id ) = 0;
        virtual void RemoveMail( const UInt64List& mail_id_list ) = 0;

        // 删除过期的全局邮件
        virtual void ClearOverdueGlobalMail() = 0;

        // 把全局邮件列表加到玩家列表中
        virtual void LoadGlobalMailToPerson( uint64 player_id, uint32 zone_id ) = 0;

        // 查询邮件列表, 会返回nullptr
        virtual KFResult<StringMapList>::UniqueType QueryMailList( uint64 player_id, uint64 last_mail_id ) = 0;

        // 更新邮件状态
        virtual bool UpdateMailStatus( uint32 flag, uint64 player_id, uint64 mail_id, uint32 status ) = 0;

        // 初始化新玩家邮件数据
        virtual void InitNewPlayerMail( uint64 player_id, uint32 zone_id ) = 0;
    };
}



#endif