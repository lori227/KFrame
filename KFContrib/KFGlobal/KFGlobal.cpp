#include "KFrame.h"
#include "KFRand.h"
#include "KFUUID.h"
#include "KFVersion.h"

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
        _kf_rand = new KFRand();
        _app_id = new KFAppId();
        _kf_version = new KFVersion();
        _kf_uuid = new KFUUID();
        _kf_uuid_mutex = new KFMutex();
    }

    KFGlobal::~KFGlobal()
    {
        __DELETE_OBJECT__( _app_id );
        __DELETE_OBJECT__( _kf_rand );
        __DELETE_OBJECT__( _kf_version );

        for ( auto& iter : _kf_uuids )
        {
            delete iter.second;
        }
        _kf_uuids.clear();
    }

    void KFGlobal::Initialize( KFGlobal* kfglobal )
    {
        if ( kfglobal == nullptr )
        {
            kfglobal = new KFGlobal;
        }

        KFGlobal::_kf_global = kfglobal;
    }

    KFGlobal* KFGlobal::Instance()
    {
        return KFGlobal::_kf_global;
    }

    bool KFGlobal::LoadVersion( const std::string& file )
    {
        std::ifstream versionfile( file );
        if ( !versionfile )
        {
            return false;
        }

        std::string strdata;

        versionfile >> strdata;
        KFUtility::SplitString( strdata, ":" );
        _kf_version->FromString( strdata );

        strdata.clear();
        versionfile >> strdata;
        KFUtility::SplitString( strdata, ":" );
        _kf_version->_repository_version = strdata;

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

    uint32 KFGlobal::RandRange( uint32 min, uint32 max, uint32 base )
    {
        if ( max <= min )
        {
            return min;
        }

        uint32 index = _kf_rand->Rand32() % ( max - min + base );
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

    bool KFGlobal::IsServerSameZone( uint64 serverid )
    {
        static KFAppId _kf_app_id;
        _kf_app_id.FromUInt64( serverid );

        return _kf_app_id.GetZoneId() == _app_id->GetZoneId();
    }

    void KFGlobal::InitNetType( std::string& strtype )
    {
        _net_type = KFUtility::ToValue< uint32 >( strtype );
        if ( _net_type == 0u )
        {
            _net_type = KFServerEnum::Local;
        }
    }

    void KFGlobal::InitChannelService( std::string& strtype )
    {
        _channel = KFUtility::SplitValue< uint32 >( strtype, "." );
        if ( _channel == 0 )
        {
            _channel = 1;
        }

        _service = KFUtility::SplitValue< uint32 >( strtype, "." );
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
    void KFGlobal::UUIDStartTime( uint64 starttime )
    {
        _uuid_setting.InitStartTime( starttime );
        _kf_uuid->InitStartTime( starttime );
    }

    void KFGlobal::UUIDSetting( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits )
    {
        _uuid_setting.InitSetting( timebits, zonebits, workerbits, seqbits );
        _kf_uuid->InitSetting( timebits, zonebits, workerbits, seqbits );
    }

    KFUUID* KFGlobal::CreateUUID( const std::string& name )
    {
        auto iter = _kf_uuids.find( name );
        if ( iter == _kf_uuids.end() )
        {
            auto kfuuid = new KFUUID();
            kfuuid->InitStartTime( _uuid_setting._start_time );
            kfuuid->InitSetting( _uuid_setting._time_bits, _uuid_setting._zone_bits, _uuid_setting._worker_bits, _uuid_setting._seq_bits );
            iter = _kf_uuids.emplace( name, kfuuid ).first;
        }

        return iter->second;
    }

    uint64 KFGlobal::STMakeUUID()
    {
        auto zoneid = _app_id->GetZoneId();
        auto workerid = _app_id->GetWorkId();
        return _kf_uuid->Make( zoneid, workerid, _real_time );
    }

    uint64 KFGlobal::MTMakeUUID()
    {
        KFLocker locker( *_kf_uuid_mutex );
        return STMakeUUID();
    }

    uint64 KFGlobal::STMakeUUID( const std::string& name )
    {
        auto zoneid = _app_id->GetZoneId();
        auto workerid = _app_id->GetWorkId();

        auto kfuuid = CreateUUID( name );
        return kfuuid->Make( zoneid, workerid, _real_time );
    }

    uint64 KFGlobal::MTMakeUUID( const std::string& name )
    {
        KFLocker locker( *_kf_uuid_mutex );
        return STMakeUUID( name );
    }

    uint32 KFGlobal::STUUIDZoneId( const std::string& name, uint64 uuid )
    {
        auto kfuuid = CreateUUID( name );
        return kfuuid->ZoneId( uuid );
    }

    uint32 KFGlobal::MTUUIDZoneId( const std::string& name, uint64 uuid )
    {
        KFLocker locker( *_kf_uuid_mutex );
        return STUUIDZoneId( name, uuid );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
}