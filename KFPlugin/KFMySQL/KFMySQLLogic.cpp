#include "KFMySQLLogic.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMySQLLogic::KFMySQLLogic()
    {
        _read_execute = __KF_NEW__( KFMySQLReadExecute );
        _write_execute = __KF_NEW__( KFMySQLWriteExecute );
    }

    KFMySQLLogic::~KFMySQLLogic()
    {
        __KF_DELETE__( KFMySQLReadExecute, _read_execute );
        __KF_DELETE__( KFMySQLWriteExecute, _write_execute );
    }

    void KFMySQLLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFMySQLLogic::Initialize( const std::string& name, const KFMySQLConnectOption* kfmysqloption )
    {
        {
            _read_execute->InitMySQL( kfmysqloption->_database, kfmysqloption->_user, kfmysqloption->_password,
                                      kfmysqloption->_read_connect_data._ip, kfmysqloption->_read_connect_data._port );
        }

        {
            _write_execute->InitMySQL( kfmysqloption->_database, kfmysqloption->_user, kfmysqloption->_password,
                                       kfmysqloption->_write_connect_data._ip, kfmysqloption->_write_connect_data._port );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLLogic::Insert( const std::string& table, const StringMap& invalue )
    {
        return _write_execute->Insert( table, invalue );
    }

    bool KFMySQLLogic::Delete( const std::string& table )
    {
        return _write_execute->Delete( table );
    }

    bool KFMySQLLogic::Delete( const std::string& table, const std::string& key )
    {
        return _write_execute->Delete( table, key );
    }

    bool KFMySQLLogic::Delete( const std::string& table, const StringMap& keyvalues )
    {
        return _write_execute->Delete( table, keyvalues );
    }

    // 更新多个字段
    bool KFMySQLLogic::Update( const std::string& table, const StringMap& invalue )
    {
        return _write_execute->Update( table, invalue );
    }

    bool KFMySQLLogic::Update( const std::string& table, const std::string& key, const StringMap& invalue )
    {
        return _write_execute->Update( table, key, invalue );
    }

    bool KFMySQLLogic::Update( const std::string& table, const StringMap& keyvalue, const StringMap& invalue )
    {
        return _write_execute->Update( table, keyvalue, invalue );
    }

    void KFMySQLLogic::Pipeline( StringList& commands )
    {
        _write_execute->Pipeline( commands );
    }

    KFResult< voidptr >::UniqueType KFMySQLLogic::VoidExecute( std::string& strsql )
    {
        return _write_execute->VoidExecute( strsql );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table )
    {
        return _read_execute->Select( table );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table, const StringList& fields )
    {
        return _read_execute->Select( table, fields );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key, const StringList& fields )
    {
        return _read_execute->Select( table, key, fields );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table, const StringMap& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::Select( const std::string& table, const StringMap& key, const StringList& fields )
    {
        return _read_execute->Select( table, key, fields );
    }

    KFResult< uint32 >::UniqueType KFMySQLLogic::UInt32Execute( std::string& strsql )
    {
        return _read_execute->UInt32Execute( strsql );
    }

    KFResult< uint64 >::UniqueType KFMySQLLogic::UInt64Execute( std::string& strsql )
    {
        return _read_execute->UInt64Execute( strsql );
    }

    KFResult< std::string >::UniqueType KFMySQLLogic::StringExecute( std::string& strsql )
    {
        return _read_execute->StringExecute( strsql );
    }

    KFResult< StringMap >::UniqueType KFMySQLLogic::MapExecute( std::string& strsql )
    {
        return _read_execute->MapExecute( strsql );
    }

    KFResult< StringList >::UniqueType KFMySQLLogic::ListExecute( std::string& strsql )
    {
        return _read_execute->ListExecute( strsql );
    }

    KFResult< StringMapList >::UniqueType KFMySQLLogic::ListMapExecute( std::string& strsql )
    {
        return _read_execute->ListMapExecute( strsql );
    }
}