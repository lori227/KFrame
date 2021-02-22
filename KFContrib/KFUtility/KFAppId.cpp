
#include "KFAppId.h"
#include "KFMacros.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    union AppIdUnion
    {
        uint64 _id;
        struct AppData
        {
            uint16 _server_type;	// 服务器类型
            uint16 _zone_id;		// 小区id
            uint32 _worker_id;		// 进程id
        } _app_data;
    };

    KFAppId::KFAppId()
    {
        _union = __NEW_OBJECT__( AppIdUnion );
        _union->_id = 0;
        _app_id = "0.0.0";
    }

    KFAppId::KFAppId( uint64 app_id )
    {
        _union = __NEW_OBJECT__( AppIdUnion );
        FromUInt64( app_id );
    }

    KFAppId::KFAppId( const std::string& app_id )
    {
        _union = __NEW_OBJECT__( AppIdUnion );
        FromString( app_id );
    }

    KFAppId::KFAppId( const KFAppId& app_id )
    {
        _union = __NEW_OBJECT__( AppIdUnion );

        _app_id = app_id._app_id;
        _union->_id = app_id._union->_id;
    }

    KFAppId& KFAppId::operator = ( const KFAppId& app_id )
    {
        if ( _union == nullptr )
        {
            _union = __NEW_OBJECT__( AppIdUnion );
        }

        _app_id = app_id._app_id;
        _union->_id = app_id._union->_id;
        return *this;
    }

    KFAppId::~KFAppId()
    {
        __DELETE_OBJECT__( _union );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAppId::FromUInt64( uint64 app_id )
    {
        _union->_id = app_id;
        _app_id = __FORMAT__( "{}.{}.{}", _union->_app_data._server_type, _union->_app_data._zone_id, _union->_app_data._worker_id );
    }

    void KFAppId::FromString( std::string app_id )
    {
        _app_id = app_id;

        int32 type, zone_id, work_id = 0;
        sscanf( app_id.c_str(), "%d.%d.%d", &type, &zone_id, &work_id );
        _union->_app_data._server_type = type;
        _union->_app_data._zone_id = zone_id;
        _union->_app_data._worker_id = work_id;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 类型
    uint32 KFAppId::GetType() const
    {
        return _union->_app_data._server_type;
    }

    // 小区id
    uint32 KFAppId::GetZoneId() const
    {
        return _union->_app_data._zone_id;
    }

    // 进程id
    uint32 KFAppId::GetWorkId() const
    {
        return _union->_app_data._worker_id;
    }

    // id
    uint64 KFAppId::GetId() const
    {
        return _union->_id;
    }

    const std::string& KFAppId::ToString() const
    {
        return _app_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // 转换
    std::string KFAppId::ToString( uint64 app_id )
    {
        static KFAppId _static_app_id;
        _static_app_id.FromUInt64( app_id );
        return _static_app_id.ToString();
    }

    uint64 KFAppId::ToUInt64( const std::string& app_id )
    {
        static KFAppId _static_app_id;
        _static_app_id.FromString( app_id );
        return _static_app_id.GetId();
    }
}
