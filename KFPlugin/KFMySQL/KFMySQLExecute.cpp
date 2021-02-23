#include "KFMySQLExecute.hpp"
#include "Poco/Data/RecordSet.h"

namespace KFrame
{
    bool KFMySQLExecute::CheckDisconnected( int32 code )
    {
        switch ( code )
        {
        case 2013:
        case 2003:
        case 2006:
            return true;
        default:
            break;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::ExecuteSql( std::string& sql )
    {
        Statement statement( *_session );
        return ExecuteSql( statement, sql );
    }

    bool KFMySQLExecute::ExecuteSql( Statement& statement, std::string& sql )
    {
        // 消除转义字符
        FormatSlashes( sql );
        statement << sql;

        uint32 repeat_count = 0u;
        do
        {
            try
            {
                statement.execute();
                return true;
            }
            catch ( Poco::Exception& ce )
            {
                if ( !CheckDisconnected( ce.code() ) )
                {
                    __LOG_ERROR__( "mysql failed [{}]", ce.displayText() );
                    return false;
                }

                // 断线重复执行
                ++repeat_count;
            }
        } while ( repeat_count < 3 );

        // 与mysql断开了
        __LOG_ERROR__( "mysql disconnected [{}]", statement.toString() );
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLExecute::FormatSlashes( std::string& sql )
    {
        KFUtility::ReplaceString( sql, "\\", "\\\\" );
    }

    std::string KFMySQLExecute::FormatKeyString( const StringMap& values )
    {
        std::ostringstream string_stream;
        bool is_begin = true;
        for ( auto& iter : values )
        {
            if ( is_begin )
            {
                is_begin = false;
            }
            else
            {
                string_stream << " and ";
            }

            string_stream << "`" << iter.first << "`=" << "'" << iter.second << "'";
        }

        return string_stream.str();
    }

    std::string KFMySQLExecute::FormatFieldString( const StringList& field_list )
    {
        if ( field_list.empty() )
        {
            return "*";
        }

        std::ostringstream field_stream;

        bool is_begin = true;
        for ( auto& iter : field_list )
        {
            if ( is_begin )
            {
                is_begin = false;
            }
            else
            {
                field_stream << ",";
            }

            field_stream << "`" << iter << "`";
        }

        return field_stream.str();
    }

    std::string KFMySQLExecute::FormatUpdateString( const StringMap& values )
    {
        std::ostringstream value_stream;

        auto is_begin = true;
        for ( auto& iter : values )
        {
            if ( is_begin )
            {
                is_begin = false;
            }
            else
            {
                value_stream << ",";
            }

            value_stream << '`' << iter.first << "`='" << iter.second << "'";
        }

        return value_stream.str();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLWriteExecute::Pipeline( StringList& command_list )
    {
        for ( auto& command : command_list )
        {
            ExecuteSql( command );
        }
    }

    bool KFMySQLWriteExecute::Insert( const std::string& table, const StringMap& values )
    {
        std::ostringstream field_stream;
        std::ostringstream value_stream;
        std::ostringstream update_stream;

        bool is_begin = true;
        for ( auto& iter : values )
        {
            if ( is_begin )
            {
                is_begin = false;
            }
            else
            {
                field_stream << ",";
                value_stream << ",";
                update_stream << ",";
            }

            field_stream << "`" << iter.first << "`";
            value_stream << "'" << iter.second << "'";
            update_stream << '`' << iter.first << "`='" << iter.second << "'";
        }

        auto sql = __FORMAT__( "insert into {}({}) values({}) on duplicate key update {};",
                               table, field_stream.str(), value_stream.str(), update_stream.str() );

        return ExecuteSql( sql );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLWriteExecute::Delete( const std::string& table )
    {
        auto sql = __FORMAT__( "truncate table {};", table );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Delete( const std::string& table, const std::string& key )
    {
        StringMap keys;
        keys[ __STRING__( id ) ] = key;
        return Delete( table, keys );
    }

    bool KFMySQLWriteExecute::Delete( const std::string& table, const StringMap& keys )
    {
        auto str_key = FormatKeyString( keys );
        auto sql = __FORMAT__( "delete from {} where {};", table, str_key );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const StringMap& values )
    {
        auto str_update = FormatUpdateString( values );
        auto sql = __FORMAT__( "update {} set {};", table, str_update );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const std::string& key, const StringMap& values )
    {
        StringMap keys;
        keys[ __STRING__( id ) ] = key;
        return Update( table, keys, values );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const StringMap& keys, const StringMap& values )
    {
        auto str_update = FormatUpdateString( values );
        auto str_key = FormatKeyString( keys );
        auto sql = __FORMAT__( "update {} set {} where {};", table, str_update, str_key );
        return ExecuteSql( sql );
    }

    KFResult<voidptr>::UniqueType KFMySQLWriteExecute::VoidExecute( std::string& sql )
    {
        __NEW_RESULT__( voidptr );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( !ok )
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table )
    {
        static StringMap _empty_key;
        static StringList _empty_field;

        return Select( table, _empty_key, _empty_field );
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringList& field_list )
    {
        static StringMap _empty_key;
        return Select( table, _empty_key, field_list );
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table, const std::string& key )
    {
        static StringList _empty_field;

        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table, const std::string& key, const StringList& field_list )
    {
        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Select( table, keyvalue, field_list );
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringMap& keys )
    {
        static StringList _empty_field;
        return Select( table, keys, _empty_field );
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringMap& keys, const StringList& field_list )
    {
        __NEW_RESULT__( StringMapList );

        std::string sql = "";
        auto str_field = FormatFieldString( field_list );

        if ( keys.empty() )
        {
            sql = __FORMAT__( "select {} from {};", str_field, table );
        }
        else
        {
            auto str_key = FormatKeyString( keys );
            sql = __FORMAT__( "select {} from {} where {};", str_field, table, str_key );
        }

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            auto rowcount = record_set.rowCount();
            for ( auto i = 0u; i < rowcount; ++i )
            {
                StringMap values;
                if ( field_list.empty() )
                {
                    const auto& row = record_set.row( i );
                    auto names = row.names();
                    auto& values = row.values();

                    auto size = names->size();
                    for ( auto j = 0u; j < size; ++j )
                    {
                        values[ names->at( j ) ] = values.at( j ).toString();
                    }
                }
                else
                {
                    auto& row = record_set.row( i );
                    for ( auto& field : field_list )
                    {
                        values[ field ] = row[ field ].toString();
                    }
                }
                result->_value.push_back( values );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<uint32>::UniqueType KFMySQLReadExecute::UInt32Execute( std::string& sql )
    {
        __NEW_RESULT__( uint32 );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            if ( record_set.rowCount() > 0u )
            {
                result->_value = record_set.value( 0, 0 );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    KFResult<uint64>::UniqueType KFMySQLReadExecute::UInt64Execute( std::string& sql )
    {
        __NEW_RESULT__( uint64 );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            if ( record_set.rowCount() > 0u )
            {
                result->_value = record_set.value( 0, 0 );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    KFResult<std::string>::UniqueType KFMySQLReadExecute::StringExecute( std::string& sql )
    {
        __NEW_RESULT__( std::string );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            if ( record_set.rowCount() > 0u )
            {
                result->_value = record_set.value( 0, 0 ).toString();
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    KFResult<StringMap>::UniqueType KFMySQLReadExecute::MapExecute( std::string& sql )
    {
        __NEW_RESULT__( StringMap );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            if ( record_set.rowCount() > 0u )
            {
                const auto& row = record_set.row( 0 );
                auto names = row.names();
                auto& values = row.values();

                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    result->_value[ names->at( j ) ] = values.at( j ).toString();
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }


    KFResult<StringList>::UniqueType KFMySQLReadExecute::ListExecute( std::string& sql )
    {
        __NEW_RESULT__( StringList );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            for ( auto i = 0u; i < record_set.rowCount(); ++i )
            {
                const auto& row = record_set.row( i );
                auto& values = row.values();

                result->_value.push_back( values.at( 0 ).toString() );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    KFResult<StringMapList>::UniqueType KFMySQLReadExecute::ListMapExecute( std::string& sql )
    {
        __NEW_RESULT__( StringMapList );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet record_set( statement );
            for ( auto i = 0u; i < record_set.rowCount(); ++i )
            {
                const auto& row = record_set.row( i );
                auto names = row.names();
                auto& values = row.values();

                StringMap key_values;
                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    key_values[ names->at( j ) ] = values.at( j ).toString();
                }

                result->_value.push_back( key_values );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }
}
