#include "KFMongoLogic.hpp"
#include "KFMongoModule.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////
    KFMongoLogic::KFMongoLogic()
    {
        _read_execute = __KF_NEW__( KFMongoReadExecute );
        _write_execute = __KF_NEW__( KFMongoWriteExecute );
    }

    KFMongoLogic::~KFMongoLogic()
    {
        __KF_DELETE__( KFMongoReadExecute, _read_execute );
        __KF_DELETE__( KFMongoWriteExecute, _write_execute );
    }

    void KFMongoLogic::ShutDown()
    {
        _read_execute->ShutDown();
        _write_execute->ShutDown();
    }

    void KFMongoLogic::Initialize( const std::string& name, const KFMongoConnectOption* mongo_option )
    {
        {
            _read_execute->InitMongo( mongo_option, &mongo_option->_read_connect_data );
        }

        {
            _write_execute->InitMongo( mongo_option, &mongo_option->_write_connect_data );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::CreateIndex( const std::string& table, const std::string& index_name, bool unique, uint32 ttl )
    {
        MongoIndexType values;
        values[ index_name ] = MongoKeyword::_asc;
        return CreateIndex( table, index_name, values, unique, ttl );
    }

    bool KFMongoLogic::CreateIndex( const std::string& table, const std::string& index_name, const MongoIndexType& values, bool unique /* = false */, uint32 ttl /* = 0u */ )
    {
        // 如果是过期时间
        if ( index_name == MongoKeyword::_expire && ttl == 0u )
        {
            ttl = 1u;
        }

        auto ok1 = _write_execute->CreateIndex( table, index_name, values, unique, ttl );
        auto ok2 = _read_execute->CreateIndex( table, index_name, values, unique, ttl );
        return ok1 & ok2;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Expire( const std::string& table, uint64 key, uint64 valid_time )
    {
        auto expire_time = KFGlobal::Instance()->_real_time + valid_time;
        return _write_execute->ExpireAt( table, key, expire_time );
    }

    bool KFMongoLogic::Expire( const std::string& table, const std::string& key, uint64 valid_time )
    {
        auto expire_time = KFGlobal::Instance()->_real_time + valid_time;
        return _write_execute->ExpireAt( table, key, expire_time );
    }

    // 具体时间点
    bool KFMongoLogic::ExpireAt( const std::string& table, uint64 key, uint64 expire_time )
    {
        return _write_execute->ExpireAt( table, key, expire_time );
    }

    bool KFMongoLogic::ExpireAt( const std::string& table, const std::string& key, uint64 expire_time )
    {
        return _write_execute->ExpireAt( table, key, expire_time );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<uint64>::UniqueType KFMongoLogic::Count( const std::string& table )
    {
        return _read_execute->Count( table );
    }

    KFResult<uint64>::UniqueType KFMongoLogic::Count( const std::string& table, const std::string& field, uint64 key )
    {
        return _read_execute->Count( table, field, key );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Insert( const std::string& table, const KFDBValue& db_value )
    {
        auto uuid = KFGlobal::Instance()->MTMakeUuid( table );
        const_cast< KFDBValue& >( db_value ).AddValue( MongoKeyword::_id, uuid );
        return _write_execute->Insert( table, db_value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& field, uint64 value )
    {
        KFDBValue db_value;
        db_value.AddValue( field, value );
        return _write_execute->Update( table, db_value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& field, const std::string& value, bool is_binary )
    {
        KFDBValue db_value;
        db_value.AddValue( field, value, is_binary );
        return _write_execute->Update( table, db_value );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Insert( const std::string& table, uint64 key, const KFDBValue& db_value )
    {
        return _write_execute->Update( table, key, db_value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& key, const KFDBValue& db_value )
    {
        return _write_execute->Update( table, key, db_value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const KFDBValue& db_value, const KFMongoSelector& selector_data )
    {
        return _write_execute->Update( table, db_value, selector_data );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& field, uint64 value, const KFMongoSelector& selector_data )
    {
        KFDBValue db_value;
        db_value.AddValue( field, value );
        return _write_execute->Update( table, db_value, selector_data );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& field, const std::string& value, const KFMongoSelector& selector_data )
    {
        KFDBValue db_value;
        db_value.AddValue( field, value );
        return _write_execute->Update( table, db_value, selector_data );
    }

    bool KFMongoLogic::Insert( const std::string& table, uint64 key, const std::string& field, uint64 value )
    {
        return _write_execute->Update( table, MongoKeyword::_id, key, field, value );
    }

    bool KFMongoLogic::Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value, bool is_binary )
    {
        if ( is_binary )
        {
            Poco::MongoDB::Binary::Ptr binary = new Poco::MongoDB::Binary( value );
            return _write_execute->Update( table, MongoKeyword::_id, key, field, binary );
        }

        return _write_execute->Update( table, MongoKeyword::_id, key, field, value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& key, const std::string& field, uint64 value )
    {
        return _write_execute->Update( table, MongoKeyword::_id, key, field, value );
    }

    bool KFMongoLogic::Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value, bool is_binary )
    {
        if ( is_binary )
        {
            Poco::MongoDB::Binary::Ptr binary = new Poco::MongoDB::Binary( value );
            return _write_execute->Update( table, MongoKeyword::_id, key, field, binary );
        }

        return _write_execute->Update( table, MongoKeyword::_id, key, field, value );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFMongoLogic::Operate( const std::string& table, const std::string& field, uint32 operate, uint64 value )
    {
        return _write_execute->Operate( table, _invalid_string, 0u, field, operate, value );
    }

    uint64 KFMongoLogic::Operate( const std::string& table, uint64 key, const std::string& field, uint32 operate, uint64 value )
    {
        return _write_execute->Operate( table, MongoKeyword::_id, key, field, operate, value );
    }

    uint64 KFMongoLogic::Operate( const std::string& table, const std::string& key, const std::string& field, uint32 operate, uint64 value )
    {
        return _write_execute->Operate( table, MongoKeyword::_id, key, field, operate, value );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Delete( const std::string& table )
    {
        return _write_execute->Delete( table );
    }

    bool KFMongoLogic::Delete( const std::string& table, uint64 key )
    {
        return _write_execute->Delete( table, MongoKeyword::_id, key );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& key )
    {
        return _write_execute->Delete( table, MongoKeyword::_id, key );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& key_name, uint64 value )
    {
        return _write_execute->Delete( table, key_name, value );
    }

    bool KFMongoLogic::Delete( const std::string& table, const std::string& key_name, const std::string& value )
    {
        return _write_execute->Delete( table, key_name, value );
    }

    bool KFMongoLogic::Delete( const std::string& table, const KFMongoSelector& selector_data )
    {
        return _write_execute->Delete( table, selector_data );
    }

    bool KFMongoLogic::DeleteField( const std::string& table, uint64 key, const std::string& field )
    {
        return _write_execute->DeleteField( table, key, field );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, uint64 value )
    {
        UInt64List values;
        values.push_back( value );
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        StringList values;
        values.push_back( value );
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, uint64 value )
    {
        UInt64List values;
        values.push_back( value );
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        StringList values;
        values.push_back( value );
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, const UInt64List& values )
    {
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, const UInt64List& values )
    {
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, uint64 key, const std::string& field, const StringList& values )
    {
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Push( const std::string& table, const std::string& key, const std::string& field, const StringList& values )
    {
        return _write_execute->Push( table, MongoKeyword::_id, key, field, values );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, uint64 value )
    {
        UInt64List values;
        values.push_back( value );
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value )
    {
        StringList values;
        values.push_back( value );
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, uint64 value )
    {
        UInt64List values;
        values.push_back( value );
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value )
    {
        StringList values;
        values.push_back( value );
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, UInt64List& values )
    {
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, UInt64List& values )
    {
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, uint64 key, const std::string& field, StringList& values )
    {
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }

    bool KFMongoLogic::Pull( const std::string& table, const std::string& key, const std::string& field, StringList& values )
    {
        return _write_execute->Pull( table, MongoKeyword::_id, key, field, values );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<uint64>::UniqueType KFMongoLogic::QueryUInt64( const std::string& table, uint64 key, const std::string& field )
    {
        return _read_execute->QueryValue< uint64, Poco::UInt64, Poco::UInt64>( table, key, field );
    }

    KFResult<uint64>::UniqueType KFMongoLogic::QueryUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryValue<uint64, std::string, Poco::UInt64>( table, key, field );
    }

    KFResult<std::string>::UniqueType KFMongoLogic::QueryString( const std::string& table, uint64 key, const std::string& field )
    {
        return _read_execute->QueryValue<std::string, Poco::UInt64, std::string>( table, key, field );
    }

    KFResult<std::string>::UniqueType KFMongoLogic::QueryString( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryValue<std::string, std::string, std::string>( table, key, field );
    }

    KFResult<std::string>::UniqueType KFMongoLogic::QueryBinary( const std::string& table, uint64 key, const std::string& field )
    {
        return _read_execute->QueryBinary<Poco::UInt64>( table, key, field );
    }

    KFResult<std::string>::UniqueType KFMongoLogic::QueryBinary( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryBinary<std::string>( table, key, field );
    }

    KFResult<UInt64List>::UniqueType KFMongoLogic::QueryListUInt64( const std::string& table, uint64 key, const std::string& field )
    {
        return _read_execute->QueryList<UInt64List, Poco::UInt64, Poco::UInt64>( table, key, field );
    }

    KFResult<UInt64List>::UniqueType KFMongoLogic::QueryListUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryList<UInt64List, std::string, Poco::UInt64>( table, key, field );
    }

    KFResult<StringList>::UniqueType KFMongoLogic::QueryListString( const std::string& table, uint64 key, const std::string& field )
    {
        return _read_execute->QueryList<StringList, Poco::UInt64, std::string>( table, key, field );
    }

    KFResult<StringList>::UniqueType KFMongoLogic::QueryListString( const std::string& table, const std::string& key, const std::string& field )
    {
        return _read_execute->QueryList<StringList, std::string, std::string>( table, key, field );
    }

    KFResult<KFDBValue>::UniqueType KFMongoLogic::QueryRecord( const std::string& table, uint64 key )
    {
        static StringList _fields;
        return _read_execute->QueryRecord( table, key, _fields );
    }

    KFResult<KFDBValue>::UniqueType KFMongoLogic::QueryRecord( const std::string& table, const std::string& key )
    {
        static StringList _fields;
        return _read_execute->QueryRecord( table, key, _fields );
    }

    KFResult<KFDBValue>::UniqueType KFMongoLogic::QueryRecord( const std::string& table, uint64 key, const StringList& field_list )
    {
        return _read_execute->QueryRecord( table, key, field_list );
    }

    KFResult<KFDBValue>::UniqueType KFMongoLogic::QueryRecord( const std::string& table, const std::string& key, const StringList& field_list )
    {
        return _read_execute->QueryRecord( table, key, field_list );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<std::list<KFDBValue>>::UniqueType KFMongoLogic::QueryListRecord( const std::string& table )
    {
        static KFMongoSelector selector_data;
        return _read_execute->QueryListRecord( table, selector_data );
    }

    KFResult<std::list<KFDBValue>>::UniqueType KFMongoLogic::QueryListRecord( const std::string& table, const KFMongoSelector& selector_data )
    {
        return _read_execute->QueryListRecord( table, selector_data );
    }
}