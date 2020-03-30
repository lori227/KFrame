#include "KFDirClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

    void KFDirClientModule::BeforeRun()
    {

    }

    void KFDirClientModule::BeforeShut()
    {
    }

    void KFDirClientModule::UpdateGateToDir( uint64 appid, const std::string& ip, uint32 port, uint32 count, uint32 expiretime )
    {
        static auto _url = _kf_ip_address->GetDirUrl() + __STRING__( zoneupdate );
        auto kfsetting = KFZoneConfig::Instance()->ZoneSetting();

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appid ), appid );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfsetting->_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( ip ), ip );
        __JSON_SET_VALUE__( kfjson, __STRING__( port ), port );
        __JSON_SET_VALUE__( kfjson, __STRING__( count ), count );
        __JSON_SET_VALUE__( kfjson, __STRING__( time ), expiretime );
        _kf_http_client->MTGet< KFDirClientModule >( _url, kfjson );
    }

    bool KFDirClientModule::ZoneRegisterToDir()
    {
        // 推荐小区
        {
            static auto _recommend_url = _kf_ip_address->GetDirUrl() + __STRING__( zonerecommend );
            __JSON_OBJECT_DOCUMENT__( kfjson );
            __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), KFZoneConfig::Instance()->RecommendZoneId() );
            _kf_http_client->MTGet<KFDirClientModule>( _recommend_url, kfjson );
        }

        // 注册小区信息
        {
            auto kfsetting = KFZoneConfig::Instance()->ZoneSetting();
            auto _register_url = _kf_ip_address->GetDirUrl() + __STRING__( zoneregister );

            // 注册小区信息
            __JSON_OBJECT_DOCUMENT__( kfjson );
            __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfsetting->_id );
            __JSON_SET_VALUE__( kfjson, __STRING__( name ), kfsetting->_name );

            auto recvdata = _kf_http_client->STGet( _register_url, kfjson );
            __JSON_PARSE_STRING__( kfresult, recvdata );
            auto retcode = _kf_http_client->GetCode( kfresult );
            return ( retcode == KFMsg::Ok );
        }
    }

    bool KFDirClientModule::WorldRegisterToDir( uint64 worldid, const std::string& url )
    {
        static auto _url = _kf_ip_address->GetDirUrl() + __STRING__( worldregister );

        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( world ), worldid );
        __JSON_SET_VALUE__( sendjson, __STRING__( url ), url );
        auto resultdata = _kf_http_client->STGet( _url, sendjson );

        __JSON_PARSE_STRING__( recvjson, resultdata );
        auto retcode = __JSON_GET_UINT32__( recvjson, __STRING__( retcode ) );
        return ( retcode == KFMsg::Ok );
    }

    void KFDirClientModule::ZoneBalanceToDir( uint32 zoneid, uint32 count )
    {
        static auto _update_url = _kf_ip_address->GetDirUrl() + __STRING__( zonebalance );

        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( count ), count );
        __JSON_SET_VALUE__( sendjson, __STRING__( zoneid ), zoneid );
        _kf_http_client->MTGet< KFDirClientModule >( _update_url, sendjson );
    }

}