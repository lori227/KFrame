#include "KFRedisLogic.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFRedisLogic::KFRedisLogic()
    {
        _read_execute = __KF_CREATE_BATCH__( KFReadExecute, 5 );
        _write_execute = __KF_CREATE_BATCH__( KFWriteExecute, 5 );
    }

    KFRedisLogic::~KFRedisLogic()
    {
        ShutDown();

        __KF_DESTROY__( KFRedisExecute, _read_execute );
        __KF_DESTROY__( KFWriteExecute, _write_execute );
    }

    void KFRedisLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFRedisLogic::Run()
    {
        _read_execute->Run();
        _write_execute->Run();
    }

    void KFRedisLogic::Initialize( KFRedisType* kfredistype )
    {
        {
            auto kfredislist = kfredistype->FindRedisList( KFDatabaseEnum::Read );
            if ( kfredislist != nullptr )
            {
                auto kfsetting = kfredislist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    auto result = _read_execute->Initialize( kfsetting->_ip, kfsetting->_port, kfsetting->_password );
                    if ( result != KFEnum::Ok )
                    {
                        __LOG_ERROR__( "redis connect[ ip={}:{} ] failed!", kfsetting->_ip, kfsetting->_port );
                    }
                }
            }
        }

        {
            auto kfredislist = kfredistype->FindRedisList( KFDatabaseEnum::Write );
            if ( kfredislist != nullptr )
            {
                auto kfsetting = kfredislist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    auto result = _write_execute->Initialize( kfsetting->_ip, kfsetting->_port, kfsetting->_password );
                    if ( result != KFEnum::Ok )
                    {
                        __LOG_ERROR__( "redis connect[ ip={}:{} ] failed!", kfsetting->_ip, kfsetting->_port );
                    }
                }
            }
        }
    }

    void KFRedisLogic::AppendCommand( const std::string& strsql )
    {
        _commands.push_back( strsql );
    }

    KFResult< voidptr >* KFRedisLogic::VoidExecute( const std::string& strsql )
    {
        return _write_execute->VoidExecute( strsql );
    }

    KFResult< uint64 >* KFRedisLogic::UpdateExecute( const std::string& strsql )
    {
        return _write_execute->UpdateExecute( strsql );
    }

    KFResult< voidptr >* KFRedisLogic::Pipeline()
    {
        auto kfresult = _write_execute->Pipeline( _commands );
        _commands.clear();

        return kfresult;
    }

    KFResult< uint32 >* KFRedisLogic::UInt32Execute( const std::string& strsql )
    {
        return _read_execute->UInt32Execute( strsql );
    }

    KFResult< std::string >* KFRedisLogic::StringExecute( const std::string& strsql )
    {
        return _read_execute->StringExecute( strsql );
    }

    KFResult< uint64 >* KFRedisLogic::UInt64Execute( const std::string& strsql )
    {
        return _read_execute->UInt64Execute( strsql );
    }

    KFResult< MapString >* KFRedisLogic::MapExecute( const std::string& strsql )
    {
        return _read_execute->MapExecute( strsql );
    }

    KFResult< VectorString >* KFRedisLogic::VectorExecute( const std::string& strsql )
    {
        return _read_execute->VectorExecute( strsql );
    }

    KFResult< ListString >* KFRedisLogic::ListExecute( const std::string& strsql )
    {
        return _read_execute->ListExecute( strsql );
    }

    KFResult< ListString >* KFRedisLogic::ListPipelineExecute()
    {
        auto kfresult = _read_execute->ListPipelineExecute( _commands );
        _commands.clear();

        return kfresult;
    }

    KFResult< std::list< MapString > >* KFRedisLogic::ListMapPipelineExecute()
    {
        auto kfresult = _read_execute->ListMapPipelineExecute( _commands );
        _commands.clear();

        return kfresult;
    }
}