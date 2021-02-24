#include "KFMailDatabaseRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFMailDatabaseRedis::KFMailDatabaseRedis()
    {
        _redis_driver = _kf_redis->Create( __STRING__( mail ) );
    }

    uint64 KFMailDatabaseRedis::AddMail( uint32 flag, uint64 objectid, StringMap& maildata, uint32 validtime )
    {
        // 创建一个邮件id
        auto uint64result = _redis_driver->HIncrby( __STRING__( idcreater ), __STRING__( mail ), 1 );
        if ( uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto mailid = uint64result->_value;
        maildata[ __STRING__( id ) ] = __TO_STRING__( mailid );

        // 添加邮件
        _redis_driver->WriteMulti();
        _redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( mail ), mailid ), maildata );
        _redis_driver->Expire( __DATABASE_KEY_2__( __STRING__( mail ), mailid ), validtime );

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
            // 添加小区信息
            _redis_driver->SAdd( __STRING__( mailzonelist ), objectid );
            // 添加小区全局邮件(0 表示全区全服 )
            _redis_driver->ZAdd( __DATABASE_KEY_2__( __STRING__( globalmail ), objectid ), mailid, mailid );
            break;
        case KFMsg::PersonMail:	// 个人邮件
            _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), objectid ), mailid, KFMsg::InitStatus );
            break;
        default:
            break;
        }

        // 执行添加
        auto kfresult = _redis_driver->WriteExec();
        if ( !kfresult->IsOk() )
        {
            mailid = _invalid_int;
        }

        return mailid;
    }

    void KFMailDatabaseRedis::RemoveMail( uint64 mailid )
    {
        UInt64List idlist;
        idlist.push_back( mailid );
        return RemoveMail( idlist );
    }

    void KFMailDatabaseRedis::RemoveMail( const UInt64List& mailidlist )
    {
        StringList mailkeylist;
        for ( auto mailid : mailidlist )
        {
            mailkeylist.push_back( __DATABASE_KEY_2__( __STRING__( mail ), mailid ) );
        }
        _redis_driver->Del( mailkeylist );
    }

    void KFMailDatabaseRedis::ClearOverdueGlobalMail()
    {
        auto kflist = _redis_driver->SMembers( __STRING__( mailzonelist ) );
        if ( kflist->_value.empty() )
        {
            return;
        }

        for ( auto zone_id : kflist->_value )
        {
            auto globalmailkey = __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id );
            auto kfmailmap = _redis_driver->ZRangeByScore( globalmailkey, 0, __MAX_UINT64__ );
            if ( kfmailmap->_value.empty() )
            {
                continue;
            }

            StringList overduelist;
            for ( auto& mailpair : kfmailmap->_value )
            {
                auto kfexist = _redis_driver->Exists( __DATABASE_KEY_2__( __STRING__( mail ), mailpair.first ) );
                if ( kfexist->IsOk() && kfexist->_value == _invalid_int )
                {
                    overduelist.push_back( mailpair.first );
                }
            }

            if ( !overduelist.empty() )
            {
                _redis_driver->ZRem( globalmailkey, overduelist );
            }
        }
    }

    void KFMailDatabaseRedis::LoadGlobalMailToPerson( uint64 player_id, uint32 zone_id )
    {
        auto mailinfokey = __DATABASE_KEY_3__( __STRING__( mailinfo ), player_id, zone_id );

        // 获取玩家已经加载的最近一封GM邮件id
        auto kfmailid = _redis_driver->HGetUInt64( mailinfokey, __STRING__( gmmailid ) );
        if ( !kfmailid->IsOk() )
        {
            return;
        }

        // 查询全局邮件列表
        auto listresult = _redis_driver->ZRangeByScore( __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id ), kfmailid->_value, __MAX_UINT64__ );
        if ( listresult->_value.empty() )
        {
            return;
        }

        _redis_driver->WriteMulti();
        for ( auto& mailpair : listresult->_value )
        {
            _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mailpair.first, KFMsg::InitStatus );
        }
        _redis_driver->HSet( mailinfokey, __STRING__( gmmailid ), listresult->_value.back().first );
        _redis_driver->WriteExec();
    }

    KFResult<StringMapList>::UniqueType KFMailDatabaseRedis::QueryMailList( uint64 player_id, uint64 lastmailid )
    {
        // 查询邮件列表
        auto mailkeylist = _redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ) );
        if ( mailkeylist->_value.empty() )
        {
            return nullptr;
        }

        // 过期的邮件
        StringSet overduelist;

        // 开始查询
        _redis_driver->ReadMulti();
        for ( auto& mailiter : mailkeylist->_value )
        {
            // 判断是否已经下发的邮件
            auto mailid = __TO_UINT64__( mailiter.first );
            if ( mailid <= lastmailid )
            {
                continue;
            }

            overduelist.emplace( mailiter.first );
            _redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( mail ), mailiter.first ) );
        }
        auto kfmaillist = _redis_driver->ReadMapListExec();
        if ( kfmaillist->IsOk() )
        {
            for ( auto& maildata : kfmaillist->_value )
            {
                auto strmailid = maildata[ __STRING__( id ) ];
                overduelist.erase( strmailid );

                // 使用邮件列表的status, 因为有全局邮件
                maildata[ __STRING__( status ) ] = mailkeylist->_value[ strmailid ];
            }

            // 删除过期的邮件
            if ( !overduelist.empty() )
            {
                _redis_driver->HDel( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), overduelist );
            }
        }

        return kfmaillist;
    }

    bool KFMailDatabaseRedis::UpdateMailStatus( uint32 flag, uint64 player_id, uint64 mailid, uint32 status )
    {
        // 判断玩家是否有拥有此邮件
        auto kfquery = _redis_driver->HExists( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mailid );
        if ( !kfquery->IsOk() || kfquery->_value == _invalid_int )
        {
            return false;
        }

        auto ok = false;
        switch ( status )
        {
        case KFMsg::Remove:
        case KFMsg::ReceiveRemove:
        {
            _redis_driver->WriteMulti();
            _redis_driver->HDel( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mailid );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _redis_driver->Del( __DATABASE_KEY_2__( __STRING__( mail ), mailid ) );
            }

            auto kfresult = _redis_driver->WriteExec();
            ok = kfresult->IsOk();
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            auto kfresult = _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mailid, status );
            ok = kfresult->IsOk();
        }
        break;
        default:
            break;
        }

        return ok;
    }

    void KFMailDatabaseRedis::InitNewPlayerMail( uint64 player_id, uint32 zone_id )
    {
        // 全区的邮件
        InitNewPlayerGlobalMail( player_id, 0u );

        // 本小区的邮件
        InitNewPlayerGlobalMail( player_id, zone_id );
    }

    void KFMailDatabaseRedis::InitNewPlayerGlobalMail( uint64 player_id, uint32 zone_id )
    {
        auto listresult = _redis_driver->ZRevRange( __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id ), 0, 0 );
        if ( !listresult->_value.empty() )
        {
            auto& strmaxmailid = listresult->_value.front().first;
            _redis_driver->HSet( __DATABASE_KEY_3__( __STRING__( mailinfo ), player_id, zone_id ), __STRING__( gmmailid ), strmaxmailid );
        }
    }
}