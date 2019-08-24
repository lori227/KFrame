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
        __KF_DELETE__( KFReadExecute, _read_execute );
        __KF_DELETE__( KFWriteExecute, _write_execute );
    }

    void KFMySQLLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFMySQLLogic::Initialize( KFMySQLType* kfmysqltype )
    {
        {
            auto kfmysqllist = kfmysqltype->FindMySQLList( KFDatabaseEnum::Read );
            if ( kfmysqllist != nullptr )
            {
                auto kfsetting = kfmysqllist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _read_execute->InitMySQL( kfsetting );
                }
            }
        }

        {
            auto kfmysqllist = kfmysqltype->FindMySQLList( KFDatabaseEnum::Write );
            if ( kfmysqllist != nullptr )
            {
                auto kfsetting = kfmysqllist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _write_execute->InitMySQL( kfsetting );
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

    void KFMySQLLogic::Pipeline( ListString& commands )
    {
        _write_execute->Pipeline( commands );
    }

    KFResult< voidptr >::UniqueType KFMySQLLogic::VoidExecute( std::string& strsql )
    {
        return _write_execute->VoidExecute( strsql );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table )
    {
        return _read_execute->Select( table );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table, const ListString& fields )
    {
        return _read_execute->Select( table, fields );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table, const std::string& key, const ListString& fields )
    {
        return _read_execute->Select( table, key, fields );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table, const MapString& key )
    {
        return _read_execute->Select( table, key );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::Select( const std::string& table, const MapString& key, const ListString& fields )
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

    KFResult< MapString >::UniqueType KFMySQLLogic::MapExecute( std::string& strsql )
    {
        return _read_execute->MapExecute( strsql );
    }

    KFResult< ListString >::UniqueType KFMySQLLogic::ListExecute( std::string& strsql )
    {
        return _read_execute->ListExecute( strsql );
    }

    KFResult< ListMapString >::UniqueType KFMySQLLogic::ListMapExecute( std::string& strsql )
    {
        return _read_execute->ListMapExecute( strsql );
    }
}