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
    bool KFMySQLExecute::ExecuteSql( const std::string& sql )
    {
        Statement statement( *_session );
        statement << sql;
        return ExecuteSql( statement );
    }

    bool KFMySQLExecute::ExecuteSql( Statement& statement )
    {
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
                    __LOG_ERROR__( "mysql failed [{}]!", ce.displayText() );
                    return false;
                }

                // 断线重复执行
                ++repeatcount;
            }
        } while ( repeatcount < 3 );

        // 与mysql断开了
        __LOG_ERROR__( "mysql disconnected [{}]!", statement.toString() );
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
    void KFWriteExecute::Pipeline( const ListString& commands )
    {
        for ( auto& command : commands )
        {
            ExecuteSql( command );
        }
    }

    bool KFWriteExecute::Insert( const std::string& table, const MapString& invalue )
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
    bool KFWriteExecute::Delete( const std::string& table )
    {
        auto sql = __FORMAT__( "truncate table {};", table );
        return ExecuteSql( sql );
    }

    bool KFWriteExecute::Delete( const std::string& table, const std::string& key )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Delete( table, keyvalue );
    }

    bool KFWriteExecute::Delete( const std::string& table, const MapString& keyvalues )
    {
        auto strkey = FormatKeyString( keyvalues );
        auto sql = __FORMAT__( "delete from {} where {};", table, strkey );
        return ExecuteSql( sql );
    }

    bool KFWriteExecute::Update( const std::string& table, const MapString& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto sql = __FORMAT__( "update {} set {};", table, strupdate );
        return ExecuteSql( sql );
    }

    bool KFWriteExecute::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Update( table, keyvalue, invalue );
    }

    bool KFWriteExecute::Update( const std::string& table, const MapString& keyvalue, const MapString& invalue )
    {
        auto strupdate = FormatUpdateString( invalue );
        auto strkey = FormatKeyString( keyvalue );
        auto sql = __FORMAT__( "update {} set {} where {};", table, strupdate, strkey );
        return ExecuteSql( sql );
    }

    KFResult< voidptr >::UniqueType KFWriteExecute::VoidExecute( const std::string& strsql )
    {
        __NEW_RESULT__( voidptr );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            kfresult->SetResult( KFEnum::Ok );
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table )
    {
        static MapString _empty_key;
        static ListString _empty_field;

        return Select( table, _empty_key, _empty_field );
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table, const ListString& fields )
    {
        static MapString _empty_key;
        return Select( table, _empty_key, fields );
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table, const std::string& key )
    {
        static ListString _empty_field;

        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table, const std::string& key, const ListString& fields )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( id ) ] = key;
        return Select( table, keyvalue, fields );
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table, const MapString& keyvalue )
    {
        static ListString _empty_field;
        return Select( table, keyvalue, _empty_field );
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::Select( const std::string& table, const MapString& keyvalue, const ListString& fields )
    {
        __NEW_RESULT__( std::list< MapString > );

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
                kfresult->_value.push_back( mapvalues );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////


    KFResult< uint32 >::UniqueType KFReadExecute::UInt32Execute( const std::string& strsql )
    {
        __NEW_RESULT__( uint32 );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                kfresult->_value = recordset.value( 0, 0 );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< uint64 >::UniqueType KFReadExecute::UInt64Execute( const std::string& strsql )
    {
        __NEW_RESULT__( uint64 );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                kfresult->_value = recordset.value( 0, 0 );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< std::string >::UniqueType KFReadExecute::StringExecute( const std::string& strsql )
    {
        __NEW_RESULT__( std::string );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            RecordSet recordset( statement );
            if ( recordset.rowCount() > 0u )
            {
                kfresult->_value = recordset.value( 0, 0 ).toString();
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< MapString >::UniqueType KFReadExecute::MapExecute( const std::string& strsql )
    {
        __NEW_RESULT__( MapString );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
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
                    kfresult->_value[ names->at( j ) ] = values.at( j ).toString();
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }


    KFResult< ListString >::UniqueType KFReadExecute::ListExecute( const std::string& strsql )
    {
        __NEW_RESULT__( ListString );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            RecordSet recordset( statement );
            for ( auto i = 0u; i < recordset.rowCount(); ++i )
            {
                const auto& row = recordset.row( i );
                auto& values = row.values();

                kfresult->_value.push_back( values.at( 0 ).toString() );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< std::list< MapString > >::UniqueType KFReadExecute::ListMapExecute( const std::string& strsql )
    {
        __NEW_RESULT__( std::list< MapString > );

        Statement statement( *_session );
        statement << strsql;
        auto ok = ExecuteSql( statement );
        if ( ok )
        {
            RecordSet recordset( statement );
            for ( auto i = 0u; i < recordset.rowCount(); ++i )
            {
                const auto& row = recordset.row( i );
                auto names = row.names();
                auto& values = row.values();

                MapString mapvalues;
                auto size = names->size();
                for ( auto j = 0u; j < size; ++j )
                {
                    mapvalues[ names->at( j ) ] = values.at( j ).toString();
                }

                kfresult->_value.push_back( mapvalues );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }
}
