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

    void KFZoneData::SaveTo( std::map<std::string, std::string>& values, bool database )
    {
        values[ KFField::_id ] = KFUtility::ToString<uint32>( _id );
        values[ KFField::_type ] = KFUtility::ToString<uint32>( _type );
        values[ KFField::_name ] = _name;
        values[ KFField::_status ] = KFUtility::ToString<uint32>( _status );
        values[ KFField::_describe ] = _describe;

        if ( database )
        {
            // http
            values[ KFField::_url ] = _zone_http;

            std::string straddress = DEFAULT_SPLIT_STRING;
            for ( auto& kfaddress : _address_list )
            {
                if ( !kfaddress._is_online )
                {
                    continue;
                }

                straddress += __KF_STRING__( kfaddress._appid );

                straddress += FUNCTION_LINK_STRING;
                straddress += kfaddress._address;

                straddress += FUNCTION_LINK_STRING;
                straddress += __KF_STRING__( kfaddress._port );

                straddress += FUNCTION_LINK_STRING;
                straddress += __KF_STRING__( kfaddress._is_online ? 1 : 0 );

                straddress += FUNCTION_LINK_STRING;
                straddress += __KF_STRING__( kfaddress._platform_id );

                straddress += DEFAULT_SPLIT_STRING;
            }
            values[ KFField::_address ] = straddress;
        }
    }

    void KFZoneData::CopyFrom( std::map<std::string, std::string>& values )
    {
        _id = KFUtility::ToValue<uint32>( values[ KFField::_id ] );
        _type = KFUtility::ToValue<uint32>( values[ KFField::_type ] );
        _name = values[ KFField::_name ];
        _status = KFUtility::ToValue<uint32>( values[ KFField::_status ] );
        _describe = values[ KFField::_describe ];
        _zone_http = values[ KFField::_url ];

        auto straddress = values[ KFField::_address ];
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
            kfjson.SetValue( KFField::_ip, _invalid_str );
            kfjson.SetValue( KFField::_port, _invalid_int );
        }
        else
        {
            kfjson.SetValue( KFField::_ip, kfaddress->_address );
            kfjson.SetValue( KFField::_port, kfaddress->_port );
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
#define __LOGIN_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_login )

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
        auto appid = kfjson.GetUInt32( KFField::_app_id );
        auto zoneid = kfjson.GetUInt32( KFField::_id );
        auto zonetype = kfjson.GetUInt32( KFField::_type );
        auto zonename = kfjson.GetString( KFField::_name );
        auto address = kfjson.GetString( KFField::_address );
        auto port = kfjson.GetUInt32( KFField::_port );

        KFLocker kflock( _kf_mutex );
        auto zonedata = _kf_zone_data.Create( zoneid );

        bool update = false;
        if ( zonedata->_id != zoneid )
        {
            update = true;
            zonedata->_id = zoneid;
        }

        if ( zonedata->_name != zonename )
        {
            update = true;
            zonedata->_name = zonename;
        }

        if ( zonedata->_type != zonetype )
        {
            update = true;
            zonedata->_type = zonetype;
        }

        if ( zonedata->_status == KFZoneStatusEnum::Shutoff )
        {
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
        redisdriver->AppendCommand( "hset %s:%u %s %s", KFField::_zone.c_str(), zoneid, KFField::_url.c_str(), url.c_str() );
        redisdriver->AppendCommand( "incr %s", KFField::_zone_list_version.c_str() );
        redisdriver->PipelineExecute();
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
        redisdriver->AppendCommand( "sadd %s %u", KFField::_zone_list.c_str(), zonedata->_id );
        redisdriver->AppendCommand( values, "hmset %s:%u", KFField::_zone.c_str(), zonedata->_id );
        redisdriver->AppendCommand( "incr %s", KFField::_zone_list_version.c_str() );
        redisdriver->AppendCommand( "zincrby %s 0 %u", KFField::_zone_balance.c_str(), zonedata->_id );
        redisdriver->PipelineExecute();
    }

    void KFZoneManage::RemoveZoneDataToDatabase( uint32 zoneid )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;
        redisdriver->AppendCommand( "rem %s %u", KFField::_zone_list.c_str(), zoneid );
        redisdriver->AppendCommand( "del %s:%u", KFField::_zone.c_str(), zoneid );
        redisdriver->AppendCommand( "incr %s", KFField::_zone_list_version.c_str() );
        redisdriver->PipelineExecute();
    }

    __KF_TIMER_FUNCTION__( KFZoneManage::OnTimerLoadZoneDataFormDatabase )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;

        // 查询当前数据库的服务器列表版本比对, 如果不同就加载新的服务器列表
        std::string newzonelistversion = "";
        redisdriver->StringExecute( newzonelistversion, "get %s", KFField::_zone_list_version.c_str() );
        if ( newzonelistversion == _zone_list_version )
        {
            return;
        }

        VectorString queryvalue;
        bool redisresult = redisdriver->VectorExecute( queryvalue, "smembers %s", KFField::_zone_list.c_str() );
        if ( !redisresult || queryvalue.empty() )
        {
            return;
        }

        // 设置新版本
        _zone_list_version = newzonelistversion;

        KFLocker kflock( _kf_mutex );
        for ( auto& strzoneid : queryvalue )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );

            MapString zonevalues;
            redisdriver->MapExecute( zonevalues, "hgetall %s:%u", KFField::_zone.c_str(), zoneid );
            if ( zonevalues.empty() )
            {
                continue;
            }

            auto zonedata = _kf_zone_data.Create( zoneid );
            zonedata->CopyFrom( zonevalues );
        }
    }
}
