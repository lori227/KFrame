#include "KFMailDatabaseRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFMailDatabaseRedis::KFMailDatabaseRedis()
    {
        _redis_driver = _kf_redis->Create( __STRING__( mail ) );
    }

    uint64 KFMailDatabaseRedis::AddMail( uint32 flag, uint64 object_id, StringMap& mail_data, uint32 valid_time )
    {
        // 创建一个邮件id
        auto uint64_result = _redis_driver->HIncrby( __STRING__( idcreater ), __STRING__( mail ), 1 );
        if ( uint64_result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto mail_id = uint64_result->_value;
        mail_data[ __STRING__( id ) ] = __TO_STRING__( mail_id );

        // 添加邮件
        _redis_driver->WriteMulti();
        _redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( mail ), mail_id ), mail_data );
        _redis_driver->Expire( __DATABASE_KEY_2__( __STRING__( mail ), mail_id ), valid_time );

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
            // 添加小区信息
            _redis_driver->SAdd( __STRING__( mailzonelist ), object_id );
            // 添加小区全局邮件(0 表示全区全服 )
            _redis_driver->ZAdd( __DATABASE_KEY_2__( __STRING__( globalmail ), object_id ), mail_id, mail_id );
            break;
        case KFMsg::PersonMail:	// 个人邮件
            _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), object_id ), mail_id, KFMsg::InitStatus );
            break;
        default:
            break;
        }

        // 执行添加
        auto result = _redis_driver->WriteExec();
        if ( !result->IsOk() )
        {
            mail_id = _invalid_int;
        }

        return mail_id;
    }

    void KFMailDatabaseRedis::RemoveMail( uint64 mail_id )
    {
        UInt64List id_list;
        id_list.push_back( mail_id );
        return RemoveMail( id_list );
    }

    void KFMailDatabaseRedis::RemoveMail( const UInt64List& mail_id_list )
    {
        StringList mail_key_list;
        for ( auto mail_id : mail_id_list )
        {
            mail_key_list.push_back( __DATABASE_KEY_2__( __STRING__( mail ), mail_id ) );
        }
        _redis_driver->Del( mail_key_list );
    }

    void KFMailDatabaseRedis::ClearOverdueGlobalMail()
    {
        auto list_result = _redis_driver->SMembers( __STRING__( mailzonelist ) );
        if ( list_result->_value.empty() )
        {
            return;
        }

        for ( auto zone_id : list_result->_value )
        {
            auto global_mail_key = __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id );
            auto mail_list_result = _redis_driver->ZRangeByScore( global_mail_key, 0, __MAX_UINT64__ );
            if ( mail_list_result->_value.empty() )
            {
                continue;
            }

            StringList overdue_list;
            for ( auto& mail_pair : mail_list_result->_value )
            {
                auto exist_result = _redis_driver->Exists( __DATABASE_KEY_2__( __STRING__( mail ), mail_pair.first ) );
                if ( exist_result->IsOk() && exist_result->_value == _invalid_int )
                {
                    overdue_list.push_back( mail_pair.first );
                }
            }

            if ( !overdue_list.empty() )
            {
                _redis_driver->ZRem( global_mail_key, overdue_list );
            }
        }
    }

    void KFMailDatabaseRedis::LoadGlobalMailToPerson( uint64 player_id, uint32 zone_id )
    {
        auto mail_info_key = __DATABASE_KEY_3__( __STRING__( mailinfo ), player_id, zone_id );

        // 获取玩家已经加载的最近一封GM邮件id
        auto mail_id_result = _redis_driver->HGetUInt64( mail_info_key, __STRING__( gmmailid ) );
        if ( !mail_id_result->IsOk() )
        {
            return;
        }

        // 查询全局邮件列表
        auto mail_list_result = _redis_driver->ZRangeByScore( __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id ), mail_id_result->_value, __MAX_UINT64__ );
        if ( mail_list_result->_value.empty() )
        {
            return;
        }

        _redis_driver->WriteMulti();
        for ( auto& mail_pair : mail_list_result->_value )
        {
            _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mail_pair.first, KFMsg::InitStatus );
        }
        _redis_driver->HSet( mail_info_key, __STRING__( gmmailid ), mail_list_result->_value.back().first );
        _redis_driver->WriteExec();
    }

    KFResult<StringMapList>::UniqueType KFMailDatabaseRedis::QueryMailList( uint64 player_id, uint64 last_mail_id )
    {
        // 查询邮件列表
        auto mail_key_list = _redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ) );
        if ( mail_key_list->_value.empty() )
        {
            return nullptr;
        }

        // 过期的邮件
        StringSet overdue_list;

        // 开始查询
        _redis_driver->ReadMulti();
        for ( auto& mail_iter : mail_key_list->_value )
        {
            // 判断是否已经下发的邮件
            auto mail_id = __TO_UINT64__( mail_iter.first );
            if ( mail_id <= last_mail_id )
            {
                continue;
            }

            overdue_list.emplace( mail_iter.first );
            _redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( mail ), mail_iter.first ) );
        }
        auto mail_list_result = _redis_driver->ReadMapListExec();
        if ( mail_list_result->IsOk() )
        {
            for ( auto& mail_data : mail_list_result->_value )
            {
                auto str_mail_id = mail_data[ __STRING__( id ) ];
                overdue_list.erase( str_mail_id );

                // 使用邮件列表的status, 因为有全局邮件
                mail_data[ __STRING__( status ) ] = mail_key_list->_value[ str_mail_id ];
            }

            // 删除过期的邮件
            if ( !overdue_list.empty() )
            {
                _redis_driver->HDel( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), overdue_list );
            }
        }

        return mail_list_result;
    }

    bool KFMailDatabaseRedis::UpdateMailStatus( uint32 flag, uint64 player_id, uint64 mail_id, uint32 status )
    {
        // 判断玩家是否有拥有此邮件
        auto exist_result = _redis_driver->HExists( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mail_id );
        if ( !exist_result->IsOk() || exist_result->_value == _invalid_int )
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
            _redis_driver->HDel( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mail_id );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _redis_driver->Del( __DATABASE_KEY_2__( __STRING__( mail ), mail_id ) );
            }

            auto result = _redis_driver->WriteExec();
            ok = result->IsOk();
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            auto result = _redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( maillist ), player_id ), mail_id, status );
            ok = result->IsOk();
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
        auto mail_list_result = _redis_driver->ZRevRange( __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id ), 0, 0 );
        if ( !mail_list_result->_value.empty() )
        {
            auto& max_mail_id = mail_list_result->_value.front().first;
            _redis_driver->HSet( __DATABASE_KEY_3__( __STRING__( mailinfo ), player_id, zone_id ), __STRING__( gmmailid ), max_mail_id );
        }
    }
}