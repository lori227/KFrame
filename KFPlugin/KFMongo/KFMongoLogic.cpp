#include "KFMongoLogic.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMongoLogic::KFMongoLogic()
    {
        _read_execute = __KF_NEW__( KFReadExecute );
        _write_execute = __KF_NEW__( KFWriteExecute );
    }

    KFMongoLogic::~KFMongoLogic()
    {
        __KF_DELETE__( KFReadExecute, _read_execute );
        __KF_DELETE__( KFWriteExecute, _write_execute );
    }

    void KFMongoLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFMongoLogic::Initialize( KFMongoType* kfmongotype )
    {
        {
            auto kfmongolist = kfmongotype->FindMongoList( KFDatabaseEnum::Read );
            if ( kfmongolist != nullptr )
            {
                auto kfsetting = kfmongolist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _read_execute->InitMongo( kfsetting );
                }
            }
        }

        {
            auto kfmongolist = kfmongotype->FindMongoList( KFDatabaseEnum::Write );
            if ( kfmongolist != nullptr )
            {
                auto kfsetting = kfmongolist->FindSetting();
                if ( kfsetting != nullptr )
                {
                    _write_execute->InitMongo( kfsetting );
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::CreateIndex( const std::string& table, const std::string& indexname, bool unique, uint32 ttl )
    {
        MapString values;
        values[ indexname ] = "1";
        return CreateIndex( table, indexname, values, unique, ttl );
    }

    bool KFMongoLogic::CreateIndex( const std::string& table, const std::string& indexname, const MapString& values, bool unique, uint32 ttl )
    {
        // 如果是过期时间
        if ( indexname == MongoKeyword::_expire && ttl == 0u )
        {
            ttl = 1u;
        }

        auto ok1 = _write_execute->CreateIndex( table, indexname, values, unique, ttl );
        auto ok2 = _read_execute->CreateIndex( table, indexname, values, unique, ttl );
        return ok1 & ok2;
    }

    bool KFMongoLogic::Expire( const std::string& table, uint64 key, uint64 validtime )
    {
        auto strkey = __TO_STRING__( key );
        return Expire( table, strkey, validtime );
    }

    bool KFMongoLogic::Expire( const std::string& table, const std::string& key, uint64 validtime )
    {
        auto nowtime = KFGlobal::Instance()->_real_time + validtime;
        return ExpireAt( table, key, nowtime );
    }

    // 具体时间点
    bool KFMongoLogic::ExpireAt( const std::string& table, uint64 key, uint64 expiretime )
    {
        auto strkey = __TO_STRING__( key );
        return ExpireAt( table, strkey, expiretime );
    }

    bool KFMongoLogic::ExpireAt( const std::string& table, const std::string& key, uint64 expiretime )
    {
        return _write_execute->ExpireAt( table, key, expiretime );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Insert( const std::string& table, const MapString& values )
    {
        return _write_execute->Insert( table, values, InsertRequest::INSERT_DEFAULT );
    }

    bool KFMongoLogic::Insert( const std::string& table, uint64 key, const MapString& values )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = __TO_STRING__( key );
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_UPSERT );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& key, const MapString& values )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = key;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_UPSERT );
    }

    bool KFMongoLogic::Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = __TO_STRING__( key );

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_UPSERT );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = key;

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_UPSERT );
    }
    /////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Update( const std::string& table, const MapString& invalue )
    {
        static MapString _keys;
        return _write_execute->Update( table, _keys, invalue, UpdateRequest::UPDATE_MULTIUPDATE );
    }

    bool KFMongoLogic::Update( const std::string& table, const std::string& field, const std::string& value )
    {
        static MapString _keys;

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, _keys, values, UpdateRequest::UPDATE_MULTIUPDATE );
    }

    bool KFMongoLogic::Update( const std::string& table, const std::string& keyname, const std::string& key, const std::string& field, const std::string& value )
    {
        MapString keys;
        keys[ keyname ] = key;

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_MULTIUPDATE );
    }

    bool KFMongoLogic::Update( const std::string& table, const std::string& keyname, const std::string& key, const MapString& values )
    {
        MapString keys;
        keys[ keyname ] = key;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_MULTIUPDATE );
    }

    bool KFMongoLogic::Update( const std::string& table, uint64 key, const MapString& values )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = __TO_STRING__( key );
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_DEFAULT );
    }

    bool KFMongoLogic::Update( const std::string& table, const std::string& key, const MapString& values )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = key;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_DEFAULT );
    }

    bool KFMongoLogic::Update( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = __TO_STRING__( key );

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_DEFAULT );
    }

    bool KFMongoLogic::Update( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        MapString keys;
        keys[ MongoKeyword::_id ] = key;

        MapString values;
        values[ field ] = value;
        return _write_execute->Update( table, keys, values, UpdateRequest::UPDATE_DEFAULT );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        auto strkey = __TO_STRING__( key );
        return Push( table, strkey, field, value );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        ListString inlist;
        inlist.push_back( value );
        return _write_execute->Push( table, key, field, inlist );
    }

    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist )
    {
        auto strkey = __TO_STRING__( key );
        return Push( table, strkey, field, inlist );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist )
    {
        ListString strlist;
        for ( auto value : inlist )
        {
            strlist.push_back( __TO_STRING__( value ) );
        }

        return _write_execute->Push( table, key, field, strlist );
    }

    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, ListString& inlist )
    {
        auto strkey = __TO_STRING__( key );
        return _write_execute->Push( table, strkey, field, inlist );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        return _write_execute->Push( table, key, field, inlist );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        auto strkey = __TO_STRING__( key );
        return Pull( table, strkey, field, value );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        ListString inlist;
        inlist.push_back( value );
        return _write_execute->Pull( table, key, field, inlist );
    }

    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist )
    {
        auto strkey = __TO_STRING__( key );
        return Pull( table, strkey, field, inlist );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist )
    {
        ListString strlist;
        for ( auto value : inlist )
        {
            strlist.push_back( __TO_STRING__( value ) );
        }

        return _write_execute->Pull( table, key, field, strlist );
    }

    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, ListString& inlist )
    {
        auto strkey = __TO_STRING__( key );
        return _write_execute->Pull( table, strkey, field, inlist );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        return _write_execute->Pull( table, key, field, inlist );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMongoLogic::Delete( const std::string& table )
    {
        return _write_execute->Delete( table, DeleteRequest::DELETE_DEFAULT );
    }

    bool KFMongoLogic::Delete( const std::string& table, uint64 key )
    {
        return _write_execute->Delete( table, MongoKeyword::_id, __TO_STRING__( key ), DeleteRequest::DELETE_DEFAULT );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& key )
    {
        return _write_execute->Delete( table, MongoKeyword::_id, key, DeleteRequest::DELETE_DEFAULT );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& keyname, uint64 key )
    {
        return _write_execute->Delete( table, keyname, __TO_STRING__( key ), DeleteRequest::DELETE_DEFAULT );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& keyname, const std::string& key )
    {
        return _write_execute->Delete( table, keyname, key, DeleteRequest::DELETE_DEFAULT );
    }

    bool KFMongoLogic::Delete( const std::string& table, const KFMongoSelector& kfseletor )
    {
        return _write_execute->Delete( table, kfseletor, DeleteRequest::DELETE_DEFAULT );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< std::string >::UniqueType KFMongoLogic::QueryString( const std::string& table, uint64 key, const std::string& field )
    {
        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryString( table, strkey, field );
    }

    KFResult< std::string >::UniqueType KFMongoLogic::QueryString( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryString( table, key, field );
    }

    KFResult< uint64 >::UniqueType KFMongoLogic::QueryUInt64( const std::string& table, uint64 key, const std::string& field )
    {
        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryUInt64( table, strkey, field );
    }

    KFResult< uint64 >::UniqueType KFMongoLogic::QueryUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryUInt64( table, key, field );
    }

    KFResult< std::list< uint64 > >::UniqueType KFMongoLogic::QueryListUInt64( const std::string& table, uint64 key, const std::string& field )
    {
        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryListUInt64( table, strkey, field );
    }

    KFResult< std::list< uint64 > >::UniqueType KFMongoLogic::QueryListUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryListUInt64( table, key, field );
    }

    KFResult< ListString >::UniqueType KFMongoLogic::QueryListString( const std::string& table, uint64 key, const std::string& field )
    {
        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryListString( table, strkey, field );
    }

    KFResult< ListString >::UniqueType KFMongoLogic::QueryListString( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryListString( table, key, field );
    }

    KFResult< MapString >::UniqueType KFMongoLogic::QueryMap( const std::string& table, uint64 key )
    {
        static ListString _fields;

        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryMap( table, strkey, _fields );
    }

    KFResult< MapString >::UniqueType KFMongoLogic::QueryMap( const std::string& table, const std::string& key )
    {
        static ListString _fields;
        return _read_execute->QueryMap( table, key, _fields );
    }

    KFResult< MapString >::UniqueType KFMongoLogic::QueryMap( const std::string& table, uint64 key, const ListString& fields )
    {
        auto strkey = __TO_STRING__( key );
        return _read_execute->QueryMap( table, strkey, fields );
    }

    KFResult< MapString >::UniqueType KFMongoLogic::QueryMap( const std::string& table, const std::string& key, const ListString& fields )
    {
        return _read_execute->QueryMap( table, key, fields );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< ListMapString >::UniqueType KFMongoLogic::QueryListMapString( const std::string& table, const KFMongoSelector& kfseletor )
    {
        return _read_execute->QueryListMapString( table, kfseletor );
    }
}