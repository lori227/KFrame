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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::UpdateExecute( const std::string& sql )
    {
        return ExecuteSql( sql );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLExecute::Pipeline( const VectorString& commands )
    {
        for ( auto& command : commands )
        {
            ExecuteSql( command );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Delete( const std::string& table, const std::string& key )
    {
        auto sql = __FORMAT__( "delete from {} where `{}`='{}';", table, __KF_STRING__( id ), key );
        return ExecuteSql( sql );
    }

    bool KFMySQLExecute::Delete( const std::string& table, const ListString& keys )
    {
        for ( auto& key : keys )
        {
            Delete( table, key );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::UpdateExecute( const std::string& table, const std::string& key, const std::string& field, const std::string& invalue )
    {
        std::string sql = "";
        if ( key.empty() )
        {
            sql = __FORMAT__( "update {} set `{}`='{}';", table, field, invalue );
        }
        else
        {
            sql = __FORMAT__( "update {} set `{}`='{}' where `{}`='{}';", table, field, invalue, __KF_STRING__( id ), key );
        }

        return ExecuteSql( sql );
    }

    bool KFMySQLExecute::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        std::ostringstream oss;

        auto isbegin = true;
        for ( auto iter : invalue )
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

        std::string sql = "";
        if ( key.empty() )
        {
            sql = __FORMAT__( "update {} set {};", table, oss.str() );
        }
        else
        {
            sql = __FORMAT__( "update {} set {} where `{}`='{}';", table, oss.str(), __KF_STRING__( id ), key );
        }

        return ExecuteSql( sql );
    }

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

        auto sql = ( "insert into {}({}) values({}) on duplicate key update {};",
                     table, ossfields.str(), ossvalues.str(), ossupdate.str() );

        return ExecuteSql( sql );
    }

    bool KFMySQLExecute::SelectExecute( const std::string& table, const std::string& key, const std::string& field, std::string& outvalue )
    {
        auto sql = __FORMAT__( "select `{}` from {} where `{}`='{}';", field, table, __KF_STRING__( id ), key );

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        if ( recordset.rowCount() > 0u )
        {
            outvalue = recordset.value( 0, 0 ).toString();
        }

        return true;
    }

    bool KFMySQLExecute::SelectExecute( const std::string& table, const std::string& key, const std::string& field, uint32 limitcount, ListString& outvalue )
    {
        std::string sql = "";
        if ( limitcount == 0 )
        {
            sql = __FORMAT__( "select `{}` from {} where `{}`='{}';",
                              field, table, __KF_STRING__( id ), key );
        }
        else
        {
            sql = __FORMAT__( "select `{}` from {} where `{}`='{}' limit {};",
                              field, table, __KF_STRING__( id ), key, limitcount );
        }

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        for ( auto i = 0u; i < recordset.rowCount(); ++i )
        {
            outvalue.push_back( recordset.value( 0, i ).toString() );
        }

        return true;
    }

    bool KFMySQLExecute::Select( const std::string& table, const std::string& key, MapString& outvalue )
    {
        auto ok = false;
        if ( outvalue.empty() )
        {
            ok = SelectAll( table, key, outvalue );
        }
        else
        {
            ok = SelectField( table, key, outvalue );
        }

        return ok;
    }

    bool KFMySQLExecute::SelectAll( const std::string& table, const std::string& key, MapString& outvalue )
    {
        auto sql = __FORMAT__( "select * from {} where `{}`='{}';",
                               table, __KF_STRING__( id ), key );

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        if ( recordset.rowCount() == 0 )
        {
            return true;
        }

        // 只取第一行
        const auto& row = recordset.row( 0 );

        auto names = row.names();
        auto& values = row.values();

        auto size = names->size();
        for ( auto i = 0u; i < size; ++i )
        {
            outvalue[ names->at( i ) ] = values.at( i ).toString();
        }

        return true;
    }

    bool KFMySQLExecute::SelectField( const std::string& table, const std::string& key, MapString& outvalue )
    {
        std::ostringstream ossfields;

        bool isbegin = true;
        for ( auto& iter : outvalue )
        {
            if ( isbegin )
            {
                isbegin = false;
            }
            else
            {
                ossfields << ",";
            }

            ossfields << "`" << iter.first << "`";
        }

        auto sql = __FORMAT__( "select {} from {} where `{}`='{}';",
                               ossfields.str(), table, __KF_STRING__( id ), key );

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        if ( recordset.rowCount() == 0 )
        {
            return true;
        }

        for ( auto& iter : outvalue )
        {
            iter.second = recordset.value( iter.first, 0 ).toString();
        }

        return true;
    }

    bool KFMySQLExecute::Select( const std::string& table, const std::string& key, std::list<MapString>& outvalue )
    {
        std::string sql = "";
        if ( key.empty() )
        {
            sql = __FORMAT__( "select * from {};", table );
        }
        else
        {
            sql = __FORMAT__( "select * from {} where `{}`='{}';", table, __KF_STRING__( id ), key );
        }

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        auto rowcount = recordset.rowCount();
        for ( auto i = 0u; i < rowcount; ++i )
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
            outvalue.push_back( mapvalues );
        }

        return true;
    }

    bool KFMySQLExecute::Select( const std::string& table, const std::string& key, ListString& fields, std::list<MapString>& outvalue )
    {
        std::string sql = "";
        if ( key.empty() )
        {
            sql = __FORMAT__( "select * from {};", table );
        }
        else
        {
            sql = __FORMAT__( "select * from {} where `{}`='{}';", table, __KF_STRING__( id ), key );
        }

        Statement statement( *_session );
        statement << sql;
        auto ok = ExecuteSql( statement );
        if ( !ok )
        {
            return false;
        }

        RecordSet recordset( statement );
        auto rowcount = recordset.rowCount();
        for ( auto i = 0u; i < rowcount; ++i )
        {
            auto& row = recordset.row( i );

            MapString mapvalues;
            for ( auto& field : fields )
            {
                mapvalues[ field ] = row[ field ].toString();
            }
            outvalue.push_back( mapvalues );
        }

        return true;
    }
}
