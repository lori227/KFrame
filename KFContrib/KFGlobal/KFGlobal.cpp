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

    KFUUID* KFGlobal::CreateUUID( uint32 type )
    {
        auto iter = _kf_uuids.find( type );
        if ( iter == _kf_uuids.end() )
        {
            auto uuid = new KFUUID( 29, 10, 10, 14 );
            iter = _kf_uuids.insert( std::make_pair( type, uuid ) ).first;
        }

        return iter->second;
    }

    uint64 KFGlobal::MakeUUID()
    {
        return MakeUUID( 0 );
    }

    uint64 KFGlobal::MakeUUID( uint32 type )
    {
        auto zoneid = _app_id->GetZoneId();
        auto workerid = _app_id->GetWorkId();

        auto kfuuid = CreateUUID( type );
        return kfuuid->Make( zoneid, workerid, _real_time );
    }

    uint32 KFGlobal::UUIDZoneId( uint32 type, uint64 uuid )
    {
        auto kfuuid = CreateUUID( type );
        return kfuuid->ZoneId( uuid );
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
}