#include "KFMySQLLogic.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMySQLLogic::KFMySQLLogic()
    {
        _read_execute = __KF_NEW__( KFReadExecute );
        _write_execute = __KF_NEW__( KFWriteExecute );
    }

    KFMySQLLogic::~KFMySQLLogic()
    {
        ShutDown();

        __KF_DELETE__( KFReadExecute, _read_execute );
        __KF_DELETE__( KFWriteExecute, _write_execute );
    }

    void KFMySQLLogic::ShutDown()
    {

    }

    void KFMySQLLogic::Initialize( KFMySQLType* kfmysqltype )
    {
        {
            auto kfredislist = kfmysqltype->FindMySQLList( KFDatabaseEnum::Read );
            if ( kfredislist != nullptr )
            {
                auto kfsetting = kfredislist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _read_execute->InitMySQL( kfsetting->_user, kfsetting->_password, kfsetting->_database, kfsetting->_ip, kfsetting->_port );
                }
            }
        }

        {
            auto kfredislist = kfmysqltype->FindMySQLList( KFDatabaseEnum::Write );
            if ( kfredislist != nullptr )
            {
                auto kfsetting = kfredislist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _write_execute->InitMySQL( kfsetting->_user, kfsetting->_password, kfsetting->_database, kfsetting->_ip, kfsetting->_port );
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLLogic::Insert( const std::string& table, const MapString& invalue )
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

    bool KFMySQLLogic::Delete( const std::string& table, const MapString& keyvalues )
    {
        return _write_execute->Delete( table, keyvalues );
    }

    // 更新多个字段
    bool KFMySQLLogic::Update( const std::string& table, const MapString& invalue )
    {
        return _write_execute->Update( table, invalue );
    }

    bool KFMySQLLogic::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        return _write_execute->Update( table, key, invalue );
    }

    bool KFMySQLLogic::Update( const std::string& table, const MapString& keyvalue, const MapString& invalue )
    {
        return _write_execute->Update( table, keyvalue, invalue );
    }

    void KFMySQLLogic::Pipeline( const ListString& commands )
    {
        _write_execute->Pipeline( commands );
    }

    KFResult< voidptr >::UniqueType KFMySQLLogic::VoidExecute( const std::string& strsql )
    {
        return _write_execute->VoidExecute( strsql );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table )
    {
        return _read_execute->Select( table );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table, const ListString& fields )
    {
        return _read_execute->Select( table, fields );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key, const ListString& fields )
    {
        return _read_execute->Select( table, key, fields );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table, const MapString& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::Select( const std::string& table, const MapString& key, const ListString& fields )
    {
        return _read_execute->Select( table, key, fields );
    }

    KFResult< uint32 >::UniqueType KFMySQLLogic::UInt32Execute( const std::string& strsql )
    {
        return _read_execute->UInt32Execute( strsql );
    }

    KFResult< uint64 >::UniqueType KFMySQLLogic::UInt64Execute( const std::string& strsql )
    {
        return _read_execute->UInt64Execute( strsql );
    }

    KFResult< std::string >::UniqueType KFMySQLLogic::StringExecute( const std::string& strsql )
    {
        return _read_execute->StringExecute( strsql );
    }

    KFResult< MapString >::UniqueType KFMySQLLogic::MapExecute( const std::string& strsql )
    {
        return _read_execute->MapExecute( strsql );
    }

    KFResult< ListString >::UniqueType KFMySQLLogic::ListExecute( const std::string& strsql )
    {
        return _read_execute->ListExecute( strsql );
    }

    KFResult< std::list< MapString > >::UniqueType KFMySQLLogic::ListMapExecute( const std::string& strsql )
    {
        return _read_execute->ListMapExecute( strsql );
    }
}