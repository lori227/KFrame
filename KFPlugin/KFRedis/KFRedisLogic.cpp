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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 写操作
    KFResult< voidptr >::UniqueType KFRedisLogic::WriteVoid( const std::string& strsql )
    {
        return _write_execute->WriteVoid( strsql );
    }

    KFResult< uint64 >::UniqueType KFRedisLogic::WriteUInt64( const std::string& strsql )
    {
        return _write_execute->WriteUInt64( strsql );
    }

    KFResult< std::string >::UniqueType KFRedisLogic::WriteString( const std::string& strsql )
    {
        return _write_execute->WriteString( strsql );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 读操作
    KFResult< uint64 >::UniqueType KFRedisLogic::ReadUInt64( const std::string& strsql )
    {
        return _read_execute->ReadUInt64( strsql );
    }

    KFResult< std::string >::UniqueType KFRedisLogic::ReadString( const std::string& strsql )
    {
        return _read_execute->ReadString( strsql );
    }

    KFResult< StringMap >::UniqueType KFRedisLogic::ReadMap( const std::string& strsql )
    {
        return _read_execute->ReadMap( strsql );
    }

    KFResult< StringPair >::UniqueType KFRedisLogic::ReadPair( const std::string& strsql )
    {
        return _read_execute->ReadPair( strsql );
    }

    KFResult< StringList >::UniqueType KFRedisLogic::ReadList( const std::string& strsql )
    {
        return _read_execute->ReadList( strsql );
    }

    KFResult< StringVector >::UniqueType KFRedisLogic::ReadVector( const std::string& strsql )
    {
        return _read_execute->ReadVector( strsql );
    }

    KFResult< StringMapList >::UniqueType KFRedisLogic::ReadMapList( const std::string& strsql )
    {
        return _read_execute->ReadMapList( strsql );
    }

    KFResult< StringPairList >::UniqueType KFRedisLogic::ReadPairList( const std::string& strsql )
    {
        return _read_execute->ReadPairList( strsql );
    }
}