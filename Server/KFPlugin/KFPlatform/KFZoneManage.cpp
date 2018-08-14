#include "KFZoneManage.h"
#include "KFJson.h"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    KFZoneData::KFZoneData()
    {
        _id = 0;
        _type = 0;
        _status = 0;
        _address_index = 0;
    }

    bool KFZoneData::AddAddress( uint32 appid, const std::string& address, uint32 port, bool isonline, uint32 platformid )
    {
        for ( auto& kfaddress : _address_list )
        {
            if ( kfaddress._appid == appid )
            {
                bool update = false;
                if ( kfaddress._address != address )
                {
                    update = true;
                    kfaddress._address = address;
                }

                if ( kfaddress._port != port )
                {
                    update = true;
                    kfaddress._port = port;
                }

                if ( kfaddress._is_online != isonline )
                {
                    update = true;
                    kfaddress._is_online = isonline;
                }

                if ( kfaddress._platform_id != platformid )
                {
                    update = true;
                    kfaddress._platform_id = platformid;
                }

                kfaddress._offline_time = KFGlobal::Instance()->_game_time + 60000;
                return update;
            }
        }

        KFAddress kfaddress;
        kfaddress._appid = appid;
        kfaddress._address = address;
        kfaddress._port = port;
        kfaddress._is_online = isonline;
        kfaddress._platform_id = platformid;
        kfaddress._offline_time = KFGlobal::Instance()->_game_time + 60000;
        _address_list.push_back( kfaddress );

        return true;
    }

    void KFZoneData::RemoveAddress( uint32 appid )
    {
        for ( auto iter = _address_list.begin(); iter != _address_list.end(); ++iter )
        {
            auto kfaddress = &( *iter );
            if ( kfaddress->_appid == appid )
            {
                _address_list.erase( iter );
                break;
            }
        }
    }

    void KFZoneData::SaveTo( MapString& values, bool database )
    {
        values[ __KF_STRING__( id ) ] = KFUtility::ToString<uint32>( _id );
        values[ __KF_STRING__( type ) ] = KFUtility::ToString<uint32>( _type );
        values[ __KF_STRING__( name ) ] = _name;
        values[ __KF_STRING__( status ) ] = KFUtility::ToString<uint32>( _status );
        values[ __KF_STRING__( describe ) ] = _describe;

        if ( database )
        {
            // http
            values[ __KF_STRING__( url ) ] = _zone_http;

            std::string straddress = DEFAULT_SPLIT_STRING;
            for ( auto& kfaddress : _address_list )
            {
                if ( !kfaddress._is_online )
                {
                    continue;
                }

                straddress += __TO_STRING__( kfaddress._appid );

                straddress += FUNCTION_LINK_STRING;
                straddress += kfaddress._address;

                straddress += FUNCTION_LINK_STRING;
                straddress += __TO_STRING__( kfaddress._port );

                straddress += FUNCTION_LINK_STRING;
                straddress += __TO_STRING__( kfaddress._is_online ? 1 : 0 );

                straddress += FUNCTION_LINK_STRING;
                straddress += __TO_STRING__( kfaddress._platform_id );

                straddress += DEFAULT_SPLIT_STRING;
            }
            values[ __KF_STRING__( address ) ] = straddress;
        }
    }

    void KFZoneData::CopyFrom( MapString& values )
    {
        _id = KFUtility::ToValue<uint32>( values[ __KF_STRING__( id ) ] );
        _type = KFUtility::ToValue<uint32>( values[ __KF_STRING__( type ) ] );
        _name = values[ __KF_STRING__( name ) ];
        _status = KFUtility::ToValue<uint32>( values[ __KF_STRING__( status ) ] );
        _describe = values[ __KF_STRING__( describe ) ];
        _zone_http = values[ __KF_STRING__( url ) ];

        auto straddress = values[ __KF_STRING__( address ) ];
        while ( !straddress.empty() )
        {
            auto appaddress = KFUtility::SplitString( straddress, DEFAULT_SPLIT_STRING );
            if ( !appaddress.empty() )
            {
                auto appid = KFUtility::SplitValue< uint32 >( appaddress, FUNCTION_LINK_STRING );
                auto address = KFUtility::SplitString( appaddress, FUNCTION_LINK_STRING );
                auto port = KFUtility::SplitValue< uint32 >( appaddress, FUNCTION_LINK_STRING );
                auto isonline = ( KFUtility::SplitValue< uint32 >( appaddress, FUNCTION_LINK_STRING ) == 1 );
                auto platformid = KFUtility::SplitValue< uint32 >( appaddress, FUNCTION_LINK_STRING );

                if ( appid != _invalid_int && !address.empty() && port != _invalid_int )
                {
                    AddAddress( appid, address, port, isonline, platformid );
                }
            }
        }
    }

    void KFZoneData::SaveToJson( KFJson& kfjson )
    {
        std::map< std::string, std::string > values;
        SaveTo( values, false );

        kfjson.Parse( values );

        auto kfaddress = SelectAddress();
        if ( kfaddress == nullptr )
        {
            kfjson.SetValue( __KF_STRING__( ip ), _invalid_str );
            kfjson.SetValue( __KF_STRING__( port ), _invalid_int );
        }
        else
        {
            kfjson.SetValue( __KF_STRING__( ip ), kfaddress->_address );
            kfjson.SetValue( __KF_STRING__( port ), kfaddress->_port );
        }
    }

    void KFZoneData::CheckOnline( KFAddress* kfaddress )
    {
        if ( !kfaddress->_is_online )
        {
            return;
        }

        if ( kfaddress->_platform_id == KFGlobal::Instance()->_app_id )
        {
            if ( KFGlobal::Instance()->_game_time > kfaddress->_offline_time )
            {
                kfaddress->_is_online = false;
                _kf_zone_manage->SaveZoneDataToDatabase( this );
            }
        }
    }

    KFAddress* KFZoneData::SelectAddress()
    {
        // 随机选择一个ip地址
        auto addresscount = _address_list.size();
        if ( addresscount == 0 )
        {
            return nullptr;
        }

        auto loopcount = addresscount;
        do
        {
            --loopcount;
            if ( _address_index >= addresscount )
            {
                _address_index = 0;
            }

            auto kfaddress = &_address_list[ _address_index++ ];
            CheckOnline( kfaddress );
            if ( kfaddress->_is_online )
            {
                return kfaddress;
            }
        } while ( loopcount > 0 );

        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
#define __LOGIN_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( login ) )

    KFZoneManage::KFZoneManage()
    {

    }

    KFZoneManage::~KFZoneManage()
    {
    }

    void KFZoneManage::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFZoneManage::OnTimerLoadZoneDataFormDatabase );
    }

    void KFZoneManage::BeforeShut()
    {
        __UNREGISTER_TIMER__();
    }

    void KFZoneManage::Run()
    {
        // 更新状态
    }

    void KFZoneManage::GetZoneJson( KFJson& kfjson )
    {
        KFLocker kflock( _kf_mutex );

        for ( auto iter : _kf_zone_data._objects )
        {
            auto kfdata = iter.second;

            KFJson json;
            kfdata->SaveToJson( json );
            kfjson.append( json );
        }
    }

    void KFZoneManage::RandZoneJson( uint32 zoneid, std::string& ip, uint32& port )
    {
        KFLocker kflock( _kf_mutex );

        auto kfzonedata = _kf_zone_data.Find( zoneid );
        if ( kfzonedata == nullptr )
        {
            return;
        }

        auto kfaddress = kfzonedata->SelectAddress();
        if ( kfaddress == nullptr )
        {
            return;
        }

        ip = kfaddress->_address;
        port = kfaddress->_port;
    }

    std::string KFZoneManage::GetZoneUrl( uint32 zoneid )
    {
        KFLocker kflock( _kf_mutex );

        auto kfzonedata = _kf_zone_data.Find( zoneid );
        if ( kfzonedata == nullptr )
        {
            return _invalid_str;
        }

        return kfzonedata->_zone_http;
    }

    bool KFZoneManage::AddZoneData( KFJson& kfjson )
    {
        auto appid = kfjson.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = kfjson.GetUInt32( __KF_STRING__( id ) );
        auto zonetype = kfjson.GetUInt32( __KF_STRING__( type ) );
        auto zonename = kfjson.GetString( __KF_STRING__( name ) );
        auto address = kfjson.GetString( __KF_STRING__( address ) );
        auto port = kfjson.GetUInt32( __KF_STRING__( port ) );

        KFLocker kflock( _kf_mutex );
        auto zonedata = _kf_zone_data.Create( zoneid );

        bool update = false;
        if ( zonedata->_id != zoneid )
        {
            update = true;
            zonedata->_id = zoneid;
        }

        if ( zonedata->_type != zonetype )
        {
            update = true;
            zonedata->_type = zonetype;
        }

        if ( zonedata->_name != zonename )
        {
            update = true;
            zonedata->_name = zonename;
        }

        if ( zonedata->_status == KFZoneStatusEnum::Shutoff )
        {
            update = true;
            zonedata->_status = KFZoneStatusEnum::Running;
        }

        update |= zonedata->AddAddress( appid, address, port, true, KFGlobal::Instance()->_app_id );
        if ( update )
        {
            SaveZoneDataToDatabase( zonedata );
        }
        return update;
    }

    void KFZoneManage::RemoveZoneData( uint32 id )
    {
        KFLocker kflock( _kf_mutex );

        _kf_zone_data.Remove( id );
        RemoveZoneDataToDatabase( id );
    }

    void KFZoneManage::RemoveZoneAddress( uint32 zoneid, uint32 appid )
    {
        KFLocker kflock( _kf_mutex );
        auto zonedata = FindZoneData( zoneid );
        if ( zonedata == nullptr )
        {
            return;
        }

        zonedata->RemoveAddress( appid );
        SaveZoneDataToDatabase( zonedata );
    }

    void KFZoneManage::UpdateZoneStatus( uint32 id, uint32 status, const std::string& describe )
    {
        KFLocker kflock( _kf_mutex );
        auto zonedata = FindZoneData( id );
        if ( zonedata == nullptr )
        {
            return;
        }

        zonedata->_status = status;
        zonedata->_describe = describe;
        SaveZoneDataToDatabase( zonedata );
    }

    void KFZoneManage::QueryZoneStatus( uint32 id, uint32& status, std::string& describe )
    {
        KFLocker kflock( _kf_mutex );

        auto zonedata = FindZoneData( id );
        if ( zonedata == nullptr )
        {
            return;
        }

        status = zonedata->_status;
        describe = zonedata->_describe;
    }

    void KFZoneManage::UpdateZoneHttp( uint32 zoneid, const std::string& url )
    {
        {
            KFLocker kflock( _kf_mutex );
            auto zonedata = _kf_zone_data.Create( zoneid );
            zonedata->_id = zoneid;
            zonedata->_zone_http = url;
        }

        auto redisdriver = __LOGIN_REDIS_DRIVER__;
        redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( zone ), zoneid, __KF_STRING__( url ), url );
        redisdriver->Append( "incr {}", __KF_STRING__( zonelistversion ) );
        redisdriver->Pipeline( __FUNC_LINE__ );
    }

    KFZoneData* KFZoneManage::FindZoneData( uint32 id )
    {
        return _kf_zone_data.Find( id );
    }

    void KFZoneManage::SaveZoneDataToDatabase( KFZoneData* zonedata )
    {
        std::map<std::string, std::string > values;
        zonedata->SaveTo( values, true );

        auto redisdriver = __LOGIN_REDIS_DRIVER__;
        redisdriver->Append( values, "hmset {}:{}", __KF_STRING__( zone ), zonedata->_id );
        redisdriver->Append( "sadd {} {}", __KF_STRING__( zonelist ), zonedata->_id );
        redisdriver->Append( "incr {}", __KF_STRING__( zonelistversion ) );
        redisdriver->Append( "zincrby {} 0 {}", __KF_STRING__( zonebalance ), zonedata->_id );
        redisdriver->Pipeline( __FUNC_LINE__ );
    }

    void KFZoneManage::RemoveZoneDataToDatabase( uint32 zoneid )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;
        redisdriver->Append( "rem {} {}", __KF_STRING__( zonelist ), zoneid );
        redisdriver->Append( "del {}:{}", __KF_STRING__( zone ), zoneid );
        redisdriver->Append( "incr {}", __KF_STRING__( zonelistversion ) );
        redisdriver->Pipeline( __FUNC_LINE__ );
    }

    __KF_TIMER_FUNCTION__( KFZoneManage::OnTimerLoadZoneDataFormDatabase )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;

        // 查询当前数据库的服务器列表版本比对, 如果不同就加载新的服务器列表
        auto zoneversion = redisdriver->QueryString( __FUNC_LINE__, "get {}", __KF_STRING__( zonelistversion ) );
        if ( !zoneversion->IsOk() || zoneversion->_value == _zone_list_version )
        {
            return;
        }

        auto queryzonelist = redisdriver->QueryList( __FUNC_LINE__, "smembers {}", __KF_STRING__( zonelist ) );
        if ( !queryzonelist->IsOk() || queryzonelist->_value.empty() )
        {
            return;
        }

        // 设置新版本
        _zone_list_version = zoneversion->_value;

        KFLocker kflock( _kf_mutex );
        for ( auto& strzoneid : queryzonelist->_value )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );

            auto queryzonedata = redisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}", __KF_STRING__( zone ), zoneid );
            if ( queryzonedata->_value.empty() )
            {
                continue;
            }

            auto zonedata = _kf_zone_data.Create( zoneid );
            zonedata->CopyFrom( queryzonedata->_value );
        }
    }
}
