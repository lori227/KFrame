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

    uint64 KFMailDatabaseMongo::AddMail( uint32 flag, uint64 objectid, StringMap& maildata, uint32 validtime )
    {
        auto mailid = _mongo_driver->Operate( __STRING__( idcreater ), __STRING__( mail ), KFEnum::Add, 1u );
        if ( mailid == 0u )
        {
            return 0u;
        }

        // 先添加邮件
        {
            KFDBValue dbvalue;
            __MAP_TO_DBVALUE__( maildata, dbvalue );
            dbvalue.AddValue( MongoKeyword::_id, mailid );
            dbvalue.AddValue( MongoKeyword::_expire, validtime );
            _mongo_driver->Insert( __STRING__( mail ), mailid, dbvalue );
        }

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
        {
            auto tablename = __DATABASE_KEY_2__( __STRING__( globalmail ), objectid );
            _mongo_driver->CreateIndex( tablename, MongoKeyword::_expire );

            // 添加小区全局邮件(0 表示全区全服 )
            KFDBValue dbvalue;
            dbvalue.AddValue( MongoKeyword::_id, mailid );
            dbvalue.AddValue( MongoKeyword::_expire, validtime );
            dbvalue.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
            _mongo_driver->Insert( tablename, dbvalue );
        }
        break;
        case KFMsg::PersonMail:	// 个人邮件
        {
            auto tablename = __DATABASE_KEY_2__( __STRING__( maillist ), objectid );
            _mongo_driver->CreateIndex( tablename, MongoKeyword::_expire );

            KFDBValue dbvalue;
            dbvalue.AddValue( MongoKeyword::_id, mailid );
            dbvalue.AddValue( MongoKeyword::_expire, validtime );
            dbvalue.AddValue( __STRING__( status ), ( uint32 )KFMsg::InitStatus );
            _mongo_driver->Insert( tablename, dbvalue );
        }
        break;
        default:
            break;
        }

        return 0u;
    }

    void KFMailDatabaseMongo::RemoveMail( uint64 mailid )
    {
        UInt64List idlist;
        idlist.push_back( mailid );
        return RemoveMail( idlist );
    }

    void KFMailDatabaseMongo::RemoveMail( const UInt64List& mailidlist )
    {
        for ( auto mailid : mailidlist )
        {
            _mongo_driver->Delete( __STRING__( mail ), mailid );
        }
    }

    void KFMailDatabaseMongo::ClearOverdueGlobalMail()
    {

    }

    void KFMailDatabaseMongo::LoadGlobalMailToPerson( uint64 playerid, uint32 zoneid )
    {
        // 查询全局邮件列表
        auto tablename = __DATABASE_KEY_2__( __STRING__( globalmail ), zoneid );

        // 获取玩家已经加载的最近一封全局邮件id
        auto kfmailid = _mongo_driver->QueryUInt64( __STRING__( mailinfo ), playerid, tablename );
        if ( !kfmailid->IsOk() )
        {
            return;
        }

        KFMongoSelector selector;
        selector._document.AddExpression( MongoKeyword::_id, MongoKeyword::_gt, kfmailid->_value );
        auto kflistresult = _mongo_driver->QueryListRecord( tablename, selector );
        if ( kflistresult->_value.empty() )
        {
            return;
        }

        auto maxmailid = kfmailid->_value;
        for ( auto& dbvalue : kflistresult->_value )
        {
            auto mailid = dbvalue.FindValue( MongoKeyword::_id );
            if ( mailid <= kfmailid->_value )
            {
                continue;
            }

            auto time = dbvalue.FindValue( __STRING__( time ) );
            auto validtime = dbvalue.FindValue( MongoKeyword::_expire );
            auto tablename = __DATABASE_KEY_2__( __STRING__( maillist ), playerid );

            KFDBValue dbvalue;
            dbvalue.AddValue( MongoKeyword::_id, mailid );
            dbvalue.AddValue( MongoKeyword::_expire, ( time + validtime - KFGlobal::Instance()->_real_time ) );
            dbvalue.AddValue( __STRING__( status ), ( uint32 )KFMsg::InitStatus );
            auto ok = _mongo_driver->Insert( tablename, dbvalue );
            if ( ok && mailid > maxmailid )
            {
                maxmailid = mailid;
            }
        }

        // 更新最大邮件id
        _mongo_driver->Insert( __STRING__( mailinfo ), playerid, tablename, maxmailid );
    }

    KFResult< StringMapList >::UniqueType KFMailDatabaseMongo::QueryMailList( uint64 playerid, uint64 lastmailid )
    {
        auto tablename = __DATABASE_KEY_2__( __STRING__( maillist ), playerid );

        KFMongoSelector selector;
        selector._document.AddExpression( MongoKeyword::_id, MongoKeyword::_gt, lastmailid );
        auto kflist = _mongo_driver->QueryListRecord( tablename, selector );
        if ( kflist->_value.empty() )
        {
            return nullptr;
        }

        __NEW_RESULT__( StringMapList );
        for ( auto& dbvalue : kflist->_value )
        {
            auto mailid = dbvalue.FindValue( MongoKeyword::_id );
            if ( mailid == 0u )
            {
                continue;
            }

            // 邮件具体信息
            auto kfmail = _mongo_driver->QueryRecord( __STRING__( mail ), mailid );
            if ( kfmail->_value.IsEmpty() )
            {
                continue;
            }

            StringMap values;
            __DBVALUE_TO_MAP__( kfmail->_value, values );
            values[ __STRING__( status ) ] = dbvalue.FindValue( __STRING__( status ) );
            kfresult->_value.emplace_back( values );
        }

        return kfresult;
    }

    bool KFMailDatabaseMongo::UpdateMailStatus( uint32 flag, uint64 playerid, uint64 mailid, uint32 status )
    {
        auto tablename = __DATABASE_KEY_2__( __STRING__( maillist ), playerid );
        auto kfquery = _mongo_driver->QueryUInt64( tablename, mailid, MongoKeyword::_id );
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
            ok = _mongo_driver->Delete( tablename, mailid );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _mongo_driver->Delete( __STRING__( mail ), mailid );
            }
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            ok = _mongo_driver->Insert( tablename, mailid, __STRING__( status ), status );
        }
        break;
        default:
            break;
        }

        return ok;
    }

    void KFMailDatabaseMongo::InitNewPlayerMail( uint64 playerid, uint32 zoneid )
    {
        // 全区的邮件
        InitNewPlayerGlobalMail( playerid, 0u );

        // 本小区的邮件
        InitNewPlayerGlobalMail( playerid, zoneid );
    }

    void KFMailDatabaseMongo::InitNewPlayerGlobalMail( uint64 playerid, uint32 zoneid )
    {
        KFMongoSelector selector;
        selector._limit_count = 1u;
        selector.AddReturn( MongoKeyword::_id, MongoKeyword::_desc );

        auto tablename = __DATABASE_KEY_2__( __STRING__( globalmail ), zoneid );
        auto listresult = _mongo_driver->QueryListRecord( tablename, selector );
        if ( !listresult->_value.empty() )
        {
            auto& dbvalue = listresult->_value.front();
            auto mailid = dbvalue.FindValue( MongoKeyword::_id );
            if ( mailid > 0u )
            {
                _mongo_driver->Insert( __STRING__( mailinfo ), playerid, tablename, mailid );
            }
        }
    }
}