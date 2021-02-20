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

        uint32 repeatcount = 0u;
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
                ++repeatcount;
            }
        } while ( repeatcount < 3 );

        // 与mysql断开了
        __LOG_ERROR__( "mysql disconnected [{}]", statement.toString() );
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLExecute::FormatSlashes( std::string& sql )
    {
        KFUtility::ReplaceString( sql, "\\", "\\\\" );
    }

    std::string KFMySQLExecute::FormatKeyString( const StringMap& keyvalue )
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

    std::string KFMySQLExecute::FormatFieldString( const StringList& fields )
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

    std::string KFMySQLExecute::FormatUpdateString( const StringMap& updatevalue )
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
    void KFMySQLWriteExecute::Pipeline( StringList& commands )
    {
        for ( auto& command : commands )
        {
            ExecuteSql( command );
        }
    }

    bool KFMySQLWriteExecute::Insert( const std::string& table, const StringMap& invalue )
    {
        std::ostringstream ossfields;
        std::ostringstream ossvalues;
        std::ostringstream ossupdate;

        bool isbegin = true;
        for ( auto& iter : invalue )
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
    bool KFMySQLWriteExecute::Delete( const std::string& table )
    {
        auto sql = __FORMAT__( "truncate table {};", table );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Delete( const std::string& table, const std::string& key )
    {
        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Delete( table, keyvalue );
    }

    bool KFMySQLWriteExecute::Delete( const std::string& table, const StringMap& keyvalues )
    {
        auto strkey = FormatKeyString( keyvalues );
        auto sql = __FORMAT__( "delete from {} where {};", table, strkey );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const StringMap& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto sql = __FORMAT__( "update {} set {};", table, strupdate );
        return ExecuteSql( sql );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const std::string& key, const StringMap& invalue )
    {
        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Update( table, keyvalue, invalue );
    }

    bool KFMySQLWriteExecute::Update( const std::string& table, const StringMap& keyvalue, const StringMap& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto strkey = FormatKeyString( keyvalue );
        auto sql = __FORMAT__( "update {} set {} where {};", table, strupdate, strkey );
        return ExecuteSql( sql );
    }

    KFResult< voidptr >::UniqueType KFMySQLWriteExecute::VoidExecute( std::string& strsql )
    {
        __NEW_RESULT__( voidptr );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            result->SetResult( KFEnum::Ok );
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table )
    {
        static StringMap _empty_key;
        static StringList _empty_field;

        return Select( table, _empty_key, _empty_field );
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringList& fields )
    {
        static StringMap _empty_key;
        return Select( table, _empty_key, fields );
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table, const std::string& key )
    {
        static StringList _empty_field;

        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table, const std::string& key, const StringList& fields )
    {
        StringMap keyvalue;
        keyvalue[ __STRING__( id ) ] = key;
        return Select( table, keyvalue, fields );
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringMap& keyvalue )
    {
        static StringList _empty_field;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::Select( const std::string& table, const StringMap& keyvalue, const StringList& fields )
    {
        __NEW_RESULT__( StringMapList );

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
        auto ok = ExecuteSql( statement, sql );
        if ( ok )
        {
            RecordSet recordset( statement );
            auto rowcount = recordset.rowCount();
            for ( auto i = 0u; i < rowcount; ++i )
            {
                StringMap mapvalues;
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
                result->_value.push_back( mapvalues );
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////


    KFResult< uint32 >::UniqueType KFMySQLReadExecute::UInt32Execute( std::string& strsql )
    {
        __NEW_RESULT__( uint32 );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                result->_value = recordset.value( 0, 0 );
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }

    KFResult< uint64 >::UniqueType KFMySQLReadExecute::UInt64Execute( std::string& strsql )
    {
        __NEW_RESULT__( uint64 );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                result->_value = recordset.value( 0, 0 );
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }

    KFResult< std::string >::UniqueType KFMySQLReadExecute::StringExecute( std::string& strsql )
    {
        __NEW_RESULT__( std::string );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                result->_value = recordset.value( 0, 0 ).toString();
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }

    KFResult< StringMap >::UniqueType KFMySQLReadExecute::MapExecute( std::string& strsql )
    {
        __NEW_RESULT__( StringMap );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                const auto& row = recordset.row( 0 );
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
            result->SetResult( KFEnum::Error );
        }

        return result;
    }


    KFResult< StringList >::UniqueType KFMySQLReadExecute::ListExecute( std::string& strsql )
    {
        __NEW_RESULT__( StringList );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            for ( auto i = 0u; i < recordset.rowCount(); ++i )
            {
                const auto& row = recordset.row( i );
                auto& values = row.values();

                result->_value.push_back( values.at( 0 ).toString() );
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }

    KFResult< StringMapList >::UniqueType KFMySQLReadExecute::ListMapExecute( std::string& strsql )
    {
        __NEW_RESULT__( StringMapList );

        Statement statement( *_session );
        auto ok = ExecuteSql( statement, strsql );
        if ( ok )
        {
            RecordSet recordset( statement );
            for ( auto i = 0u; i < recordset.rowCount(); ++i )
            {
                const auto& row = recordset.row( i );
                auto names = row.names();
                auto& values = row.values();

                StringMap mapvalues;
                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    mapvalues[ names->at( j ) ] = values.at( j ).toString();
                }

                result->_value.push_back( mapvalues );
            }
        }
        else
        {
            result->SetResult( KFEnum::Error );
        }

        return result;
    }
}
