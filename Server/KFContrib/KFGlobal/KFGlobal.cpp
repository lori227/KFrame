#include "KFrame.h"
#include "KFRand.h"
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
        _rand = new KFRand();
        _logger = nullptr;
    }

    KFGlobal::~KFGlobal()
    {
        __DELETE_OBJECT__( _rand );
        __DELETE_OBJECT__( _logger );
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
        if ( _logger != nullptr )
        {
            _logger->RunUpdate();
        }
    }

    uint32 KFGlobal::RandRatio( uint32 ratio )
    {
        auto value = _rand->Rand32();
        return value % ratio;
    }

    uint32 KFGlobal::RandInRange( uint32 min, uint32 max, uint32 base )
    {
        if ( max <= min )
        {
            return base;
        }

        uint32 index = _rand->Rand32() % ( max - min + base );
        return min + index;
    }

    void KFGlobal::InitLogger( std::string& strtype )
    {
        auto logtype = KFUtility::SplitValue< uint32 >( strtype, "." );
        switch ( logtype )
        {
        case KFLogTypeEnum::Local:
            _logger = new KFLocalLogger();
            break;
        case KFLogTypeEnum::Remote:
            _logger = new KFRemoteLogger();
            break;
        default:
            break;
        }

        if ( _logger != nullptr )
        {
            _logger->Initialize( _app_name, _app_type, _app_id );
            _log_level = KFUtility::SplitValue< uint32 >( strtype, "." );
        }
    }

    void KFGlobal::SetLogLevel( uint32 level )
    {
        _log_level = level;
    }

    void KFGlobal::Log( uint32 level, uint32 category, const std::string& content )
    {
        _logger->Log( level, category, content );
    }

    void KFGlobal::SetRemoteLogFunction( KFLogFunction& function )
    {
        if ( _logger == nullptr )
        {
            return;
        }

        _logger->SetRemoteLogFunction( function );
    }

}