#include "KFMailDatabaseMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __MAIL_MONGO_DRIVER__

    KFMailDatabaseMongo::KFMailDatabaseMongo()
    {
        _mongo_driver = _kf_mongo->Create( __STRING__( mail ) );
        _mongo_driver->CreateIndex( __STRING__( mail ), MongoKeyword::_expire );
    }

    uint64 KFMailDatabaseMongo::AddMail( uint32 flag, uint64 object_id, StringMap& mail_data, uint32 valid_time )
    {
        auto mail_id = _mongo_driver->Operate( __STRING__( idcreater ), __STRING__( mail ), KFEnum::Add, 1u );
        if ( mail_id == 0u )
        {
            return 0u;
        }

        // 先添加邮件
        {
            KFDBValue db_value;
            __MAP_TO_DBVALUE__( mail_data, db_value );
            db_value.AddValue( MongoKeyword::_id, mail_id );
            db_value.AddValue( MongoKeyword::_expire, valid_time );
            _mongo_driver->Insert( __STRING__( mail ), mail_id, db_value );
        }

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
        {
            auto table_name = __DATABASE_KEY_2__( __STRING__( globalmail ), object_id );
            _mongo_driver->CreateIndex( table_name, MongoKeyword::_expire );

            // 添加小区全局邮件(0 表示全区全服 )
            KFDBValue db_value;
            db_value.AddValue( MongoKeyword::_id, mail_id );
            db_value.AddValue( MongoKeyword::_expire, valid_time );
            db_value.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
            _mongo_driver->Insert( table_name, db_value );
        }
        break;
        case KFMsg::PersonMail:	// 个人邮件
        {
            auto table_name = __DATABASE_KEY_2__( __STRING__( maillist ), object_id );
            _mongo_driver->CreateIndex( table_name, MongoKeyword::_expire );

            KFDBValue db_value;
            db_value.AddValue( MongoKeyword::_id, mail_id );
            db_value.AddValue( MongoKeyword::_expire, valid_time );
            db_value.AddValue( __STRING__( status ), ( uint32 )KFMsg::InitStatus );
            _mongo_driver->Insert( table_name, db_value );
        }
        break;
        default:
            break;
        }

        return 0u;
    }

    void KFMailDatabaseMongo::RemoveMail( uint64 mail_id )
    {
        UInt64List id_list;
        id_list.push_back( mail_id );
        return RemoveMail( id_list );
    }

    void KFMailDatabaseMongo::RemoveMail( const UInt64List& mail_id_list )
    {
        for ( auto mail_id : mail_id_list )
        {
            _mongo_driver->Delete( __STRING__( mail ), mail_id );
        }
    }

    void KFMailDatabaseMongo::ClearOverdueGlobalMail()
    {

    }

    void KFMailDatabaseMongo::LoadGlobalMailToPerson( uint64 player_id, uint32 zone_id )
    {
        // 查询全局邮件列表
        auto table_name = __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id );

        // 获取玩家已经加载的最近一封全局邮件id
        auto mail_id_result = _mongo_driver->QueryUInt64( __STRING__( mailinfo ), player_id, table_name );
        if ( !mail_id_result->IsOk() )
        {
            return;
        }

        KFMongoSelector selector;
        selector._document.AddExpression( MongoKeyword::_id, MongoKeyword::_gt, kfmailid->_value );
        auto list_result = _mongo_driver->QueryListRecord( table_name, selector );
        if ( list_result->_value.empty() )
        {
            return;
        }

        auto max_mail_id = mail_id_result->_value;
        for ( auto& db_value : list_result->_value )
        {
            auto mail_id = db_value.FindValue( MongoKeyword::_id );
            if ( mail_id <= kfmailid->_value )
            {
                continue;
            }

            auto time = db_value.FindValue( __STRING__( time ) );
            auto valid_time = db_value.FindValue( MongoKeyword::_expire );
            auto table_name = __DATABASE_KEY_2__( __STRING__( maillist ), player_id );

            KFDBValue db_value;
            db_value.AddValue( MongoKeyword::_id, mail_id );
            db_value.AddValue( MongoKeyword::_expire, ( time + valid_time - KFGlobal::Instance()->_real_time ) );
            db_value.AddValue( __STRING__( status ), ( uint32 )KFMsg::InitStatus );
            auto ok = _mongo_driver->Insert( table_name, db_value );
            if ( ok && mail_id > max_mail_id )
            {
                max_mail_id = mail_id;
            }
        }

        // 更新最大邮件id
        _mongo_driver->Insert( __STRING__( mailinfo ), player_id, table_name, max_mail_id );
    }

    KFResult<StringMapList>::UniqueType KFMailDatabaseMongo::QueryMailList( uint64 player_id, uint64 last_mail_id )
    {
        auto table_name = __DATABASE_KEY_2__( __STRING__( maillist ), player_id );

        KFMongoSelector selector;
        selector._document.AddExpression( MongoKeyword::_id, MongoKeyword::_gt, last_mail_id );
        auto list_result = _mongo_driver->QueryListRecord( table_name, selector );
        if ( list_result->_value.empty() )
        {
            return nullptr;
        }

        __NEW_RESULT__( StringMapList );
        for ( auto& db_value : list_result->_value )
        {
            auto mail_id = db_value.FindValue( MongoKeyword::_id );
            if ( mail_id == 0u )
            {
                continue;
            }

            // 邮件具体信息
            auto mail_result = _mongo_driver->QueryRecord( __STRING__( mail ), mail_id );
            if ( mail_result->_value.IsEmpty() )
            {
                continue;
            }

            StringMap values;
            __DBVALUE_TO_MAP__( mail_result->_value, values );
            values[ __STRING__( status ) ] = db_value.FindValue( __STRING__( status ) );
            kfresult->_value.emplace_back( values );
        }

        return kfresult;
    }

    bool KFMailDatabaseMongo::UpdateMailStatus( uint32 flag, uint64 player_id, uint64 mail_id, uint32 status )
    {
        auto table_name = __DATABASE_KEY_2__( __STRING__( maillist ), player_id );
        auto mail_result = _mongo_driver->QueryUInt64( table_name, mail_id, MongoKeyword::_id );
        if ( !mail_result->IsOk() || mail_result->_value == _invalid_int )
        {
            return false;
        }

        auto ok = false;
        switch ( status )
        {
        case KFMsg::Remove:
        case KFMsg::ReceiveRemove:
        {
            ok = _mongo_driver->Delete( table_name, mail_id );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _mongo_driver->Delete( __STRING__( mail ), mail_id );
            }
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            ok = _mongo_driver->Insert( table_name, mail_id, __STRING__( status ), status );
        }
        break;
        default:
            break;
        }

        return ok;
    }

    void KFMailDatabaseMongo::InitNewPlayerMail( uint64 player_id, uint32 zone_id )
    {
        // 全区的邮件
        InitNewPlayerGlobalMail( player_id, 0u );

        // 本小区的邮件
        InitNewPlayerGlobalMail( player_id, zone_id );
    }

    void KFMailDatabaseMongo::InitNewPlayerGlobalMail( uint64 player_id, uint32 zone_id )
    {
        KFMongoSelector selector;
        selector._limit_count = 1u;
        selector.AddReturn( MongoKeyword::_id, MongoKeyword::_desc );

        auto table_name = __DATABASE_KEY_2__( __STRING__( globalmail ), zone_id );
        auto list_result = _mongo_driver->QueryListRecord( table_name, selector );
        if ( !list_result->_value.empty() )
        {
            auto& db_value = list_result->_value.front();
            auto mail_id = db_value.FindValue( MongoKeyword::_id );
            if ( mail_id > 0u )
            {
                _mongo_driver->Insert( __STRING__( mailinfo ), player_id, table_name, mail_id );
            }
        }
    }
}