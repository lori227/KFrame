#include "KFRedisLogic.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFRedisLogic::KFRedisLogic()
    {
        _read_execute = __KF_NEW__( KFRedisReadExecute );
        _write_execute = __KF_NEW__( KFRedisWriteExecute );
    }

    KFRedisLogic::~KFRedisLogic()
    {
        ShutDown();

        __KF_DELETE__( KFRedisReadExecute, _read_execute );
        __KF_DELETE__( KFRedisWriteExecute, _write_execute );
    }

    void KFRedisLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
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
                    auto result = _read_execute->Initialize( kfsetting->_module, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
                    if ( result != KFEnum::Ok )
                    {
                        __LOG_ERROR__( "redis connect[module={} ip={}:{}] failed", kfsetting->_module, kfsetting->_ip, kfsetting->_port );
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
                    auto result = _write_execute->Initialize( kfsetting->_module, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
                    if ( result != KFEnum::Ok )
                    {
                        __LOG_ERROR__( "redis connect[module={} ip={}:{}] failed", kfsetting->_module, kfsetting->_ip, kfsetting->_port );
                    }
                }
            }
        }
    }

    void KFRedisLogic::AppendCommand( const std::string& strsql )
    {
        _commands.push_back( strsql );
    }

    KFResult< voidptr >::UniqueType KFRedisLogic::VoidExecute( const std::string& strsql )
    {
        return _write_execute->VoidExecute( strsql );
    }

    KFResult< uint64 >::UniqueType KFRedisLogic::UpdateExecute( const std::string& strsql )
    {
        return _write_execute->UpdateExecute( strsql );
    }

    KFResult< voidptr >::UniqueType KFRedisLogic::Pipeline()
    {
        auto kfresult = _write_execute->Pipeline( _commands );
        _commands.clear();

        return kfresult;
    }

    KFResult< std::string >::UniqueType KFRedisLogic::StringExecute( const std::string& strsql )
    {
        return _read_execute->StringExecute( strsql );
    }

    KFResult< uint64 >::UniqueType KFRedisLogic::UInt64Execute( const std::string& strsql )
    {
        return _read_execute->UInt64Execute( strsql );
    }

    KFResult< StringMap >::UniqueType KFRedisLogic::MapExecute( const std::string& strsql )
    {
        return _read_execute->MapExecute( strsql );
    }

    KFResult< StringVector >::UniqueType KFRedisLogic::VectorExecute( const std::string& strsql )
    {
        return _read_execute->VectorExecute( strsql );
    }

    KFResult< StringList >::UniqueType KFRedisLogic::ListExecute( const std::string& strsql )
    {
        return _read_execute->ListExecute( strsql );
    }

    KFResult< StringList >::UniqueType KFRedisLogic::ListPipelineExecute()
    {
        auto kfresult = _read_execute->ListPipelineExecute( _commands );
        _commands.clear();

        return kfresult;
    }

    KFResult< StringMapList >::UniqueType KFRedisLogic::ListMapPipelineExecute()
    {
        auto kfresult = _read_execute->ListMapPipelineExecute( _commands );
        _commands.clear();

        return kfresult;
    }
}