#include "KFMySQLExecute.h"
#include "Poco/Data/RecordSet.h"
#include "KFMemory/KFBuffer.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFMySQLExecute::KFMySQLExecute()
    {
        _length = KFrame::Buff_10M;
        _buffer = __KF_INT8__( _length );
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
    bool KFMySQLExecute::ExecuteSql( const char* sql )
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
                    KFLogger::LogSql( KFLogger::Error, "MySQL Failed [%s]!", ce.displayText().c_str() );
                    return false;
                }

                // 断线重复执行
                ++repeatcount;
            }
        } while ( repeatcount < 3 );

        // 与mysql断开了
        KFLogger::LogSql( KFLogger::Error, "MySQL Disconnected [%s]!", statement.toString().c_str() );
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

#define __MYSQL_BUFFER__\
    memset( _buffer, 0, _length );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Execute( const char* format, ... )
    {
        __MYSQL_BUFFER__;

        va_list args;
        va_start( args, format );
        vsprintf( _buffer, format, args );
        va_end( args );

        return ExecuteSql( _buffer );
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLExecute::Pipeline( const VectorString& commands )
    {
        for ( auto& command : commands )
        {
            ExecuteSql( command.c_str() );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLExecute::Delete( const std::string& table, const std::string& key )
    {
        __MYSQL_BUFFER__;

        sprintf( _buffer, "delete from %s where `%s`='%s';",
                 table.c_str(), KFField::_id.c_str(), key.c_str() );

        return ExecuteSql( _buffer );
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
        __MYSQL_BUFFER__;

        if ( key.empty() )
        {
            sprintf( _buffer, "update %s set `%s`='%s';",
                     table.c_str(), field.c_str(), invalue.c_str() );
        }
        else
        {
            sprintf( _buffer, "update %s set `%s`='%s' where `%s`='%s';",
                     table.c_str(), field.c_str(), invalue.c_str(), KFField::_id.c_str(), key.c_str() );
        }

        return ExecuteSql( _buffer );
    }

    bool KFMySQLExecute::Update( const std::string& table, const std::string& key, const MapString& invalue )
    {
        __MYSQL_BUFFER__;

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

        if ( key.empty() )
        {
            sprintf( _buffer, "update %s set %s;",
                     table.c_str(), oss.str().c_str() );
        }
        else
        {
            sprintf( _buffer, "update %s set %s where `%s`='%s';",
                     table.c_str(), oss.str().c_str(), KFField::_id.c_str(), key.c_str() );
        }

        return ExecuteSql( _buffer );
    }

    bool KFMySQLExecute::Insert( const std::string& table, const MapString& invalue )
    {
        __MYSQL_BUFFER__;

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

        sprintf( _buffer, "insert into %s(%s) values(%s) on duplicate key update %s;",
                 table.c_str(), ossfields.str().c_str(), ossvalues.str().c_str(), ossupdate.str().c_str() );

        return ExecuteSql( _buffer );
    }

    bool KFMySQLExecute::SelectExecute( const std::string& table, const std::string& key, const std::string& field, std::string& outvalue )
    {
        __MYSQL_BUFFER__;

        sprintf( _buffer, "select `%s` from %s where `%s`='%s';",
                 field.c_str(), table.c_str(), KFField::_id.c_str(), key.c_str() );

        Statement statement( *_session );
        statement << _buffer;
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
        __MYSQL_BUFFER__;

        if ( limitcount == 0 )
        {
            sprintf( _buffer, "select `%s` from %s where `%s`='%s';",
                     field.c_str(), table.c_str(), KFField::_id.c_str(), key.c_str() );
        }
        else
        {
            sprintf( _buffer, "select `%s` from %s where `%s`='%s' limit %u;",
                     field.c_str(), table.c_str(), KFField::_id.c_str(), key.c_str(), limitcount );
        }

        Statement statement( *_session );
        statement << _buffer;
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
        __MYSQL_BUFFER__;

        sprintf( _buffer, "select * from %s where `%s`='%s';",
                 table.c_str(), KFField::_id.c_str(), key.c_str() );

        Statement statement( *_session );
        statement << _buffer;
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
        __MYSQL_BUFFER__;

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

        sprintf( _buffer, "select %s from %s where `%s`='%s';",
                 ossfields.str().c_str(), table.c_str(), KFField::_id.c_str(), key.c_str() );

        Statement statement( *_session );
        statement << _buffer;
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
        __MYSQL_BUFFER__;

        if ( key.empty() )
        {
            sprintf( _buffer, "select * from %s;",
                     table.c_str() );
        }
        else
        {
            sprintf( _buffer, "select * from %s where `%s`='%s';",
                     table.c_str(), KFField::_id.c_str(), key.c_str() );
        }

        Statement statement( *_session );
        statement << _buffer;
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
        __MYSQL_BUFFER__;

        if ( key.empty() )
        {
            sprintf( _buffer, "select * from %s;",
                     table.c_str() );
        }
        else
        {
            sprintf( _buffer, "select * from %s where `%s`='%s';",
                     table.c_str(), KFField::_id.c_str(), key.c_str() );
        }

        Statement statement( *_session );
        statement << _buffer;
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
