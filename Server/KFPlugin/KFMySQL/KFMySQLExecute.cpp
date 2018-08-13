#include "KFMySQLExecute.h"
#include "Poco/Data/RecordSet.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMySQLExecute::KFMySQLExecute()
    {
    }

    KFMySQLExecute::~KFMySQLExecute()
    {

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    bool KFMySQLExecute::ExecuteSql( const std::string& sql )
    {
        Statement statement( *_session );
        statement << sql;
        return ExecuteSql( statement );
    }

    bool KFMySQLExecute::ExecuteSql( Statement& statement )
    {
        uint32 repeatcount = 0;

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
                    __LOG_ERROR__( KFLogEnum::Sql, "mysql failed [{}]!", ce.displayText() );
                    return false;
                }

                // 断线重复执行
                ++repeatcount;
            }
        } while ( repeatcount < 3 );

        // 与mysql断开了
        __LOG_ERROR__( KFLogEnum::Sql, "mysql disconnected [{}]!", statement.toString() );
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    char* KFMySQLExecute::FormatSlashes( char* buffer, uint32 length )
    {
        std::string temp = buffer;
        memset( buffer, 0, length );

        int32 index = 0;
        for ( auto iter = temp.begin(); iter != temp.end(); ++iter )
        {
            auto ch = *iter;
            buffer[ index++ ] = ch;

            if ( ch == '\\' )
            {
                buffer[ index++ ] = ch;
            }
        }

        return buffer;
    }

    std::string KFMySQLExecute::FormatKeyString( const MapString& keyvalue )
    {
        std::ostringstream osskeys;
        bool isbegin = true;
        for ( auto& iter : keyvalue )
        {
            if ( isbegin )
            {
                isbegin = false;
            }
            else
            {
                osskeys << " and ";
            }

            osskeys << "`" << iter.first << "`=" << "'" << iter.second << "'";
        }

        return osskeys.str();
    }

    std::string KFMySQLExecute::FormatFieldString( const ListString& fields )
    {
        if ( fields.empty() )
        {
            return "*";
        }

        std::ostringstream ossfields;

        bool isbegin = true;
        for ( auto& iter : fields )
        {
            if ( isbegin )
            {
                isbegin = false;
            }
            else
            {
                ossfields << ",";
            }

            ossfields << "`" << iter << "`";
        }

        return ossfields.str();
    }

    std::string KFMySQLExecute::FormatUpdateString( const MapString& updatevalue )
    {
        std::ostringstream oss;

        auto isbegin = true;
        for ( auto& iter : updatevalue )
        {
            if ( isbegin )
            {
                isbegin = false;
            }
            else
            {
                oss << ",";
            }

            oss << '`' << iter.first << "`='" << iter.second << "'";
        }

        return oss.str();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLExecute::Pipeline( const ListString& commands )
    {
        for ( auto& command : commands )
        {
            ExecuteSql( command );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Insert( const std::string& table, const MapString& invalue )
    {
        std::ostringstream ossfields;
        std::ostringstream ossvalues;
        std::ostringstream ossupdate;

        bool isbegin = true;
        for ( auto iter : invalue )
        {
            if ( isbegin )
            {
                isbegin = false;
            }
            else
            {
                ossfields << ",";
                ossvalues << ",";
                ossupdate << ",";
            }

            ossfields << "`" << iter.first << "`";
            ossvalues << "'" << iter.second << "'";
            ossupdate << '`' << iter.first << "`='" << iter.second << "'";
        }

        auto sql = __FORMAT__( "insert into {}({}) values({}) on duplicate key update {};",
                               table, ossfields.str(), ossvalues.str(), ossupdate.str() );

        return ExecuteSql( sql );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Delete( const std::string& table )
    {
        auto sql = __FORMAT__( "truncate table {};", table );
        return ExecuteSql( sql );
    }

    bool KFMySQLExecute::Delete( const std::string& table, const std::string& key )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Delete( table, keyvalue );
    }

    bool KFMySQLExecute::Delete( const std::string& table, const MapString& keyvalues )
    {
        auto strkey = FormatKeyString( keyvalues );
        auto sql = __FORMAT__( "delete from {} where {};", table, strkey );
        return ExecuteSql( sql );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Update( const std::string& table, const MapString& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto sql = __FORMAT__( "update {} set {};", table, strupdate );
        return ExecuteSql( sql );
    }

    bool KFMySQLExecute::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Update( table, keyvalue, invalue );
    }

    bool KFMySQLExecute::Update( const std::string& table, const MapString& keyvalue, const MapString& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto strkey = FormatKeyString( keyvalue );
        auto sql = __FORMAT__( "update {} set {} where {};", table, strupdate, strkey );
        return ExecuteSql( sql );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table )
    {
        static MapString _empty_key;
        static ListString _empty_field;

        return Select( table, _empty_key, _empty_field );
    }

    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table, const ListString& fields )
    {
        static MapString _empty_key;
        return Select( table, _empty_key, fields );
    }

    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table, const std::string& key )
    {
        static ListString _empty_field;

        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table, const std::string& key, const ListString& fields )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Select( table, keyvalue, fields );
    }

    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table, const MapString& keyvalue )
    {
        static ListString _empty_field;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< std::list< MapString > >* KFMySQLExecute::Select( const std::string& table, const MapString& keyvalue, const ListString& fields )
    {
        _list_map_result._value.clear();

        std::string sql = "";
        auto strfield = FormatFieldString( fields );

        if ( keyvalue.empty() )
        {
            sql = __FORMAT__( "select {} from {};", strfield, table );
        }
        else
        {
            auto strkey = FormatKeyString( keyvalue );
            sql = __FORMAT__( "select {} from {} where {};", strfield, table, strkey );
        }

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _list_map_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            auto rowcount = recordset.rowCount();
            for ( auto i = 0u; i < rowcount; ++i )
            {
                MapString mapvalues;
                if ( fields.empty() )
                {
                    const auto& row = recordset.row( i );
                    auto names = row.names();
                    auto& values = row.values();

                    auto size = names->size();
                    for ( auto j = 0u; j < size; ++j )
                    {
                        mapvalues[ names->at( j ) ] = values.at( j ).toString();
                    }
                }
                else
                {
                    auto& row = recordset.row( i );
                    for ( auto& field : fields )
                    {
                        mapvalues[ field ] = row[ field ].toString();
                    }
                }
                _list_map_result._value.push_back( mapvalues );
            }
        }
        else
        {
            _list_map_result.SetResult( KFEnum::Error );
        }

        return &_list_map_result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< voidptr >* KFMySQLExecute::VoidExecute( const std::string& strsql )
    {
        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _void_result.SetResult( KFEnum::Ok );
        }
        else
        {
            _void_result.SetResult( KFEnum::Error );
        }

        return &_void_result;
    }

    KFResult< uint32 >*  KFMySQLExecute::UInt32Execute( const std::string& strsql )
    {
        _uint32_result._value = _invalid_int;

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _uint32_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                _uint32_result._value = recordset.value( 0, 0 );
            }
        }
        else
        {
            _uint32_result.SetResult( KFEnum::Error );
        }

        return &_uint32_result;
    }

    KFResult< uint64 >*  KFMySQLExecute::UInt64Execute( const std::string& strsql )
    {
        _uint64_result._value = _invalid_int;

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _uint64_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                _uint64_result._value = recordset.value( 0, 0 );
            }
        }
        else
        {
            _uint64_result.SetResult( KFEnum::Error );
        }

        return &_uint64_result;
    }

    KFResult< std::string >*  KFMySQLExecute::StringExecute( const std::string& strsql )
    {
        _string_result._value = _invalid_str;

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _string_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                _string_result._value = recordset.value( 0, 0 ).toString();
            }
        }
        else
        {
            _string_result.SetResult( KFEnum::Error );
        }

        return &_string_result;
    }

    KFResult< MapString >*  KFMySQLExecute::MapExecute( const std::string& strsql )
    {
        _map_result._value.clear();

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _map_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                const auto& row = recordset.row( 0 );
                auto names = row.names();
                auto& values = row.values();

                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    _map_result._value[ names->at( j ) ] = values.at( j ).toString();
                }
            }
        }
        else
        {
            _map_result.SetResult( KFEnum::Error );
        }

        return &_map_result;
    }

    KFResult< std::list< MapString > >*  KFMySQLExecute::ListMapExecute( const std::string& strsql )
    {
        _list_map_result._value.clear();

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            _list_map_result.SetResult( KFEnum::Ok );

            RecordSet recordset( statement );
            for ( auto i = 0u; i < recordset.rowCount(); ++i )
            {
                const auto& row = recordset.row( i );
                auto names = row.names();
                auto& values = row.values();

                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    _map_result._value[ names->at( j ) ] = values.at( j ).toString();
                }
            }
        }
        else
        {
            _list_map_result.SetResult( KFEnum::Error );
        }

        return &_list_map_result;
    }
}
