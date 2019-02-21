
#include "KFAppId.h"
#include "KFMacros.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    union AppIdData
    {
        uint64 _id;
        struct AppData
        {
            uint16 _channel_id;
            uint16 _server_type;
            uint16 _zone_id;
            uint16 _worker_id;
        } _app_data;
    };

    KFAppId::KFAppId()
    {
        _data = __NEW_OBJECT__( AppIdData );
        _data->_id = 0;
        _str_app_id = "0.0.0.0";
    }

    KFAppId::KFAppId( uint64 appid )
    {
        _data = __NEW_OBJECT__( AppIdData );
        FromUInt64( appid );
    }

    KFAppId::KFAppId( const std::string& strappid )
    {
        _data = __NEW_OBJECT__( AppIdData );
        FromString( strappid );
    }

    KFAppId::KFAppId( const KFAppId& appid )
    {
        _data = __NEW_OBJECT__( AppIdData );

        _data->_id = appid._data->_id;
        _str_app_id = appid._str_app_id;
    }

    KFAppId& KFAppId::operator=( const KFAppId& appid )
    {
        if ( _data == nullptr )
        {
            _data = __NEW_OBJECT__( AppIdData );
        }

        _data->_id = appid._data->_id;
        _str_app_id = appid._str_app_id;

        return *this;
    }

    KFAppId::~KFAppId()
    {
        __DELETE_OBJECT__( _data );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAppId::FromUInt64( uint64 appid )
    {
        _data->_id = appid;
        _str_app_id = __FORMAT__( "{}.{}.{}.{}", _data->_app_data._channel_id, _data->_app_data._server_type, _data->_app_data._zone_id, _data->_app_data._worker_id );
    }

    void KFAppId::FromString( std::string strappid )
    {
        _str_app_id = strappid;
        _data->_app_data._channel_id = KFUtility::SplitValue< uint16 >( strappid, "." );
        _data->_app_data._server_type = KFUtility::SplitValue< uint16 >( strappid, "." );
        _data->_app_data._zone_id = KFUtility::SplitValue< uint16 >( strappid, "." );
        _data->_app_data._worker_id = KFUtility::SplitValue< uint16 >( strappid, "." );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 渠道
    uint32 KFAppId::GetChannelId() const
    {
        return _data->_app_data._channel_id;
    }

    void KFAppId::SetChannelId( uint32 value )
    {
        _data->_app_data._channel_id = static_cast< uint16 >( value );
    }

    // 类型
    uint32 KFAppId::GetType() const
    {
        return _data->_app_data._server_type;
    }

    void KFAppId::SetType( uint32 value )
    {
        _data->_app_data._server_type = static_cast< uint16 >( value );
    }

    // 小区id
    uint32 KFAppId::GetZoneId() const
    {
        return _data->_app_data._zone_id;
    }

    void KFAppId::SetZoneId( uint32 value )
    {
        _data->_app_data._zone_id = static_cast< uint16 >( value );
    }

    // 进程id
    uint32 KFAppId::GetWorkId() const
    {
        return _data->_app_data._worker_id;
    }

    void KFAppId::SetWorkId( uint32 value )
    {
        _data->_app_data._worker_id = value;
    }

    // id
    uint64 KFAppId::GetId() const
    {
        return _data->_id;
    }

    const std::string& KFAppId::ToString() const
    {
        return _str_app_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // 转换
    const std::string& KFAppId::ToString( uint64 appid )
    {
        static KFAppId _app_id;
        _app_id.FromUInt64( appid );

        return _app_id.ToString();
    }

    uint64 KFAppId::ToUInt64( const std::string& strappid )
    {
        static KFAppId _app_id;
        _app_id.FromString( strappid );
        return _app_id.GetId();
    }
}
