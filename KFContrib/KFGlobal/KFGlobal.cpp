#include "KFrame.h"
#include "KFRand.hpp"
#include "KFUuid.hpp"
#include "KFVersion.h"
#include "KFConstantData.hpp"
#include "KFElementFormat.hpp"

namespace KFrame
{
    KFGlobal* KFGlobal::_kf_global = nullptr;

    KFGlobal::KFGlobal()
    {
        _app_run = true;
        _app_id = 0;
        _game_time = 0;
        _real_time = 0;
        _listen_port = 0;
        _net_type = 0;
        _kf_rand = __NEW_OBJECT__( KFRand );
        _app_id = __NEW_OBJECT__( KFAppId );
        _kf_version = __NEW_OBJECT__( KFVersion );
        _kf_uuid = __NEW_OBJECT__( KFUuid );
        _kf_constant_data = __NEW_OBJECT__( KFConstantData );
        _kf_element_format = __NEW_OBJECT__( KFElementFormat );
    }

    KFGlobal::~KFGlobal()
    {
        __DELETE_OBJECT__( _app_id );
        __DELETE_OBJECT__( _kf_rand );
        __DELETE_OBJECT__( _kf_version );
        __DELETE_OBJECT__( _kf_uuid );
        __DELETE_OBJECT__( _kf_constant_data );
        __DELETE_OBJECT__( _kf_element_format );
    }

    void KFGlobal::Initialize( KFGlobal* global )
    {
        if ( global == nullptr )
        {
            global = new KFGlobal;
        }

        KFGlobal::_kf_global = global;
    }

    KFGlobal* KFGlobal::Instance()
    {
        return KFGlobal::_kf_global;
    }

    bool KFGlobal::LoadVersion( const std::string& file )
    {
        std::ifstream version_file( file );
        if ( !version_file )
        {
            return false;
        }

        std::string data;

        version_file >> data;
        KFUtility::SplitString( data, ":" );
        _kf_version->FromString( data );

        data.clear();
        version_file >> data;
        KFUtility::SplitString( data, ":" );
        _kf_version->_repository_version = data;

        return true;
    }

    const std::string& KFGlobal::GetVersion()
    {
        return _kf_version->_str_version;
    }

    bool KFGlobal::CheckVersionCompatibility( const std::string& version )
    {
        return _kf_version->CheckCompatibility( version );
    }

    uint32 KFGlobal::Rand()
    {
        return _kf_rand->Rand32();
    }

    uint32 KFGlobal::RandRatio( uint32 ratio )
    {
        if ( ratio == 0u )
        {
            return 0u;
        }

        auto value = _kf_rand->Rand32();
        return value % ratio;
    }

    bool KFGlobal::RandCheck( uint32 ratio, uint32 value )
    {
        if ( ratio == 0u || value == 0u )
        {
            return false;
        }

        auto rand = RandRatio( ratio );
        return rand < value;
    }

    uint64 KFGlobal::RandRange( uint64 min, uint64 max, uint64 base )
    {
        if ( max <= min )
        {
            return min;
        }

        auto index = _kf_rand->Rand32() % ( max - min + base );
        return min + index;
    }

    double KFGlobal::RandDouble()
    {
        return _kf_rand->RandFloat();
    }

    double KFGlobal::RandDouble( double first, double second )
    {
        auto min = __MIN__( first, second );
        auto max = __MAX__( first, second );
        auto range = max - min;

        auto rand = _kf_rand->RandFloat();
        return min + rand * range;
    }

    bool KFGlobal::IsServerSameZone( uint64 server_id )
    {
        static KFAppId _kf_app_id;
        _kf_app_id.FromUInt64( server_id );

        return _kf_app_id.GetZoneId() == _app_id->GetZoneId();
    }

    void KFGlobal::InitNetType( std::string& net_type )
    {
        _net_type = __TO_UINT32__( net_type );
        if ( _net_type == 0u )
        {
            _net_type = KFServerEnum::Local;
        }
    }

    void KFGlobal::InitChannelService( std::string& channel_service )
    {
        _channel = KFUtility::SplitValue<uint32>( channel_service, "." );
        if ( _channel == 0 )
        {
            _channel = 1;
        }

        _service = KFUtility::SplitValue<uint32>( channel_service, "." );
        if ( _service == 0u )
        {
            _service = KFServerEnum::Debug;
        }
    }

    // 判断渠道和服务类型
    bool KFGlobal::CheckChannelService( uint32 channel, uint32 service )
    {
        if ( channel != 0 && channel != _channel )
        {
            return false;
        }

        if ( service != 0 && service != _service )
        {
            return false;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 初始化配置
    void KFGlobal::AddUuidData( const std::string& name, uint32 time_bits, uint32 zone_bits, uint32 worker_bits, uint32 seq_bits )
    {
        _kf_uuid->AddData( name, _project_time, time_bits, zone_bits, worker_bits, seq_bits );
    }

    uint64 KFGlobal::STMakeUuid()
    {
        return _kf_uuid->STMake( __STRING__( normal ), _app_id->GetZoneId(), _app_id->GetWorkId(), _real_time );
    }

    uint64 KFGlobal::MTMakeUuid()
    {
        return _kf_uuid->MTMake( __STRING__( normal ), _app_id->GetZoneId(), _app_id->GetWorkId(), _real_time );
    }

    uint64 KFGlobal::STMakeUuid( const std::string& name )
    {
        return _kf_uuid->STMake( name, _app_id->GetZoneId(), _app_id->GetWorkId(), _real_time );
    }

    uint64 KFGlobal::MTMakeUuid( const std::string& name )
    {
        return _kf_uuid->MTMake( name, _app_id->GetZoneId(), _app_id->GetWorkId(), _real_time );
    }

    uint64 KFGlobal::STMakeUuid( const std::string& name, uint32 zone_id )
    {
        return _kf_uuid->STMake( name, zone_id, _app_id->GetWorkId(), _real_time );
    }

    uint64 KFGlobal::MTMakeUuid( const std::string& name, uint32 zone_id )
    {
        return _kf_uuid->MTMake( name, zone_id, _app_id->GetWorkId(), _real_time );
    }

    uint32 KFGlobal::STUuidZoneId( const std::string& name, uint64 uuid )
    {
        return _kf_uuid->STZoneId( name, uuid );
    }

    uint32 KFGlobal::MTUuidZoneId( const std::string& name, uint64 uuid )
    {
        return _kf_uuid->MTZoneId( name, uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFGlobal::STUuidParse( const std::string& name, uint64 uuid )
    {
        return _kf_uuid->STParse( name, uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFGlobal::MTUuidParse( const std::string& name, uint64 uuid )
    {
        return _kf_uuid->MTParse( name, uuid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 添加常量
    void KFGlobal::AddConstant( const std::string& name, uint32 key, uint32 value )
    {
        _kf_constant_data->AddConstant( name, key, value );
    }

    void KFGlobal::AddConstant( const std::string& name, uint32 key, double value )
    {
        _kf_constant_data->AddConstant( name, key, value );
    }

    void KFGlobal::AddConstant( const std::string& name, uint32 key, const std::string& value )
    {
        _kf_constant_data->AddConstant( name, key, value );
    }

    // 获得常量
    std::shared_ptr<const KFConstant> KFGlobal::FindConstant( const std::string& name, uint32 key )
    {
        return _kf_constant_data->FindConstant( name, key );
    }

    // uint32配置
    uint32 KFGlobal::GetUInt32( const std::string& name, uint32 key )
    {
        return _kf_constant_data->GetUInt32( name, key );
    }

    // double配置
    double KFGlobal::GetDouble( const std::string& name, uint32 key )
    {
        return _kf_constant_data->GetDouble( name, key );
    }

    // string配置
    const std::string& KFGlobal::GetString( const std::string& name, uint32 key )
    {
        return _kf_constant_data->GetString( name, key );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGlobal::BindParseToElementStringFunction( KFParseToElementStringFunction& function )
    {
        _kf_element_format->BindParseToElementStringFunction( function );
    }

    void KFGlobal::BindIntToElementStringFunction( KFIntToElementStringFunction& function )
    {
        _kf_element_format->BindIntToElementStringFunction( function );
    }

    void KFGlobal::BindStrToElementStringFunction( KFStrToElementStringFunction& function )
    {
        _kf_element_format->BindStrToElementStringFunction( function );
    }

    void KFGlobal::BindDataToElementStringFunction( KFDataToElementStringFunction& function )
    {
        _kf_element_format->BindDataToElementStringFunction( function );
    }

    const std::string& KFGlobal::ParseString( const std::string& str_parse )
    {
        return _kf_element_format->ParseString( str_parse );
    }

    const std::string& KFGlobal::FormatString( const std::string& data_name, uint32 data_value, uint32 data_id )
    {
        return _kf_element_format->FormatString( data_name, data_value, data_id );
    }

    const std::string& KFGlobal::FormatString( const std::string& data_name, const std::string& data_value, uint32 data_id )
    {
        return _kf_element_format->FormatString( data_name, data_value, data_id );
    }

    const std::string& KFGlobal::FormatString( const KFElementData& element_data )
    {
        return _kf_element_format->FormatString( element_data );
    }

    bool KFGlobal::FormatElement( KFElements& elements, const std::string& data_name, uint32 data_value, uint32 data_id  )
    {
        return _kf_element_format->FormatElement( elements, data_name, data_value, data_id );
    }

    bool KFGlobal::FormatElement( KFElements& elements, const std::string& data_name, const std::string& data_value, uint32 data_id )
    {
        return _kf_element_format->FormatElement( elements, data_name, data_value, data_id );
    }

    bool KFGlobal::FormatElement( KFElements& elements, const KFElementData& element_data )
    {
        return _kf_element_format->FormatElement( elements, element_data );
    }

    bool KFGlobal::ParseElement( KFElements& elements, const char* file, uint64 id )
    {
        return _kf_element_format->ParseElement( elements, file, id );
    }
}