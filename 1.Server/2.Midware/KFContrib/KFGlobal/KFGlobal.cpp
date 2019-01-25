#include "KFrame.h"
#include "KFRand.h"
#include "KFGuid.h"
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
        _kf_rand = new KFRand();
        _kf_version = new KFVersion();
        _kf_guid = new KFGuid( 29, 14, 8, 12 );
    }

    KFGlobal::~KFGlobal()
    {
        __DELETE_OBJECT__( _kf_rand );
        __DELETE_OBJECT__( _kf_guid );
        __DELETE_OBJECT__( _kf_version );
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
        if ( channel != 0 && channel != _app_id._union._app_data._channel_id )
        {
            return false;
        }

        if ( service != 0 && service != _service_type )
        {
            return false;
        }

        return true;
    }
}