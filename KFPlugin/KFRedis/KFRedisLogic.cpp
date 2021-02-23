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

    void KFRedisLogic::Initialize( const std::string& name, const KFRedisConnectOption* redis_option )
    {
        {
            auto connect_data = &redis_option->_read_connect_data;
            auto result = _read_execute->Initialize( name, connect_data->_ip, connect_data->_port, redis_option->_password );
            if ( result != KFEnum::Ok )
            {
                __LOG_ERROR__( "read redis connect[module={} ip={}:{}] failed", name, connect_data->_ip, connect_data->_port );
            }
        }

        {
            auto connect_data = &redis_option->_write_connect_data;
            auto result = _write_execute->Initialize( name, connect_data->_ip, connect_data->_port, redis_option->_password );
            if ( result != KFEnum::Ok )
            {
                __LOG_ERROR__( "read redis connect[module={} ip={}:{}] failed", name, connect_data->_ip, connect_data->_port );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 写操作
    KFResult<voidptr>::UniqueType KFRedisLogic::WriteVoid( const std::string& sql )
    {
        return _write_execute->WriteVoid( sql );
    }

    KFResult<uint64>::UniqueType KFRedisLogic::WriteUInt64( const std::string& sql )
    {
        return _write_execute->WriteUInt64( sql );
    }

    KFResult<std::string>::UniqueType KFRedisLogic::WriteString( const std::string& sql )
    {
        return _write_execute->WriteString( sql );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 读操作
    KFResult<uint64>::UniqueType KFRedisLogic::ReadUInt64( const std::string& sql )
    {
        return _read_execute->ReadUInt64( sql );
    }

    KFResult<std::string>::UniqueType KFRedisLogic::ReadString( const std::string& sql )
    {
        return _read_execute->ReadString( sql );
    }

    KFResult<StringMap>::UniqueType KFRedisLogic::ReadMap( const std::string& sql )
    {
        return _read_execute->ReadMap( sql );
    }

    KFResult<StringPair>::UniqueType KFRedisLogic::ReadPair( const std::string& sql )
    {
        return _read_execute->ReadPair( sql );
    }

    KFResult<StringList>::UniqueType KFRedisLogic::ReadList( const std::string& sql )
    {
        return _read_execute->ReadList( sql );
    }

    KFResult<StringVector>::UniqueType KFRedisLogic::ReadVector( const std::string& sql )
    {
        return _read_execute->ReadVector( sql );
    }

    KFResult<StringMapList>::UniqueType KFRedisLogic::ReadMapList( const std::string& sql )
    {
        return _read_execute->ReadMapList( sql );
    }

    KFResult<StringPairList>::UniqueType KFRedisLogic::ReadPairList( const std::string& sql )
    {
        return _read_execute->ReadPairList( sql );
    }
}