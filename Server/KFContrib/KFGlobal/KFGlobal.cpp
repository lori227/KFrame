#include "KFrame.h"
#include "KFRand.h"
#include "KFGuid.h"
#include "KFLogger.h"

namespace KFrame
{
    KFGlobal* KFGlobal::_kf_global = nullptr;

    KFGlobal::KFGlobal()
    {
        _app_run = true;
        _app_id = 0;
        _app_channel = 0;
        _game_time = 0;
        _real_time = 0;
        _listen_port = 0;
        _zone_id = 0;
        _log_level = 0;
        _kf_rand = new KFRand();
        _kf_logger = nullptr;
        _kf_version = new KFVersion();
        _kf_guid = new KFGuid( 30, 12, 8, 13 );
    }

    KFGlobal::~KFGlobal()
    {
        __DELETE_OBJECT__( _kf_rand );
        __DELETE_OBJECT__( _kf_logger );
        __DELETE_OBJECT__( _kf_version );
        __DELETE_OBJECT__( _kf_guid );
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

    void KFGlobal::RunUpdate()
    {
        if ( _kf_logger != nullptr )
        {
            _kf_logger->RunUpdate();
        }
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

    uint32 KFGlobal::RandRatio( uint32 ratio )
    {
        auto value = _kf_rand->Rand32();
        return value % ratio;
    }

    uint32 KFGlobal::RandInRange( uint32 min, uint32 max, uint32 base )
    {
        if ( max <= min )
        {
            return base;
        }

        uint32 index = _kf_rand->Rand32() % ( max - min + base );
        return min + index;
    }

    uint64 KFGlobal::Make64Guid()
    {
        return _kf_guid->Make64Guid();
    }

    void KFGlobal::Print64Guid( uint64 guid )
    {
        _kf_guid->Print64Guid( guid );
    }

    void KFGlobal::InitNetService( std::string& strtype )
    {
        _net_type = KFUtility::SplitValue< uint32 >( strtype, "." );
        if ( _net_type == 0 )
        {
            _net_type = KFServerEnum::Local;
        }

        _service_type = KFUtility::SplitValue< uint32 >( strtype, "." );
        if ( _service_type == 0 )
        {
            _service_type = KFServerEnum::Debug;
        }
    }

    // 判断渠道和服务类型
    bool KFGlobal::CheckChannelService( uint32 channel, uint32 service )
    {
        if ( channel != 0 && channel != _app_channel )
        {
            return false;
        }

        if ( service != 0 && service != _service_type )
        {
            return false;
        }

        return true;
    }

    void KFGlobal::InitLogger( std::string& strtype )
    {
        auto logtype = KFUtility::SplitValue< uint32 >( strtype, "." );
        switch ( logtype )
        {
        case KFLogTypeEnum::Local:
            _kf_logger = new KFLocalLogger();
            break;
        case KFLogTypeEnum::Remote:
            _kf_logger = new KFRemoteLogger();
            break;
        default:
            break;
        }

        if ( _kf_logger != nullptr )
        {
            _kf_logger->Initialize( _app_name, _app_type, _app_id );
            SetLogLevel( KFUtility::SplitValue< uint32 >( strtype, "." ) );
        }
    }

    void KFGlobal::SetLogLevel( uint32 level )
    {
        _log_level = level;
        if ( _log_level == 0 )
        {
            KFMalloc::Instance()->SetLogMemoryOpen( true );
        }
        else
        {
            KFMalloc::Instance()->SetLogMemoryOpen( false );
        }
    }

    void KFGlobal::Log( uint32 level, const std::string& content )
    {
        _kf_logger->Log( level, content );
    }

    void KFGlobal::SetRemoteLogFunction( KFLogFunction& function )
    {
        if ( _kf_logger == nullptr )
        {
            return;
        }

        _kf_logger->SetRemoteLogFunction( function );
    }

}