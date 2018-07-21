#include "KFRedisExecute.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    KFRedisExecute::KFRedisExecute()
    {
        _index = 0;
    }

    KFRedisExecute::~KFRedisExecute()
    {
    }

    int32 KFRedisExecute::Initialize( const char* ip, int32 port, const char* password )
    {
        _length = KFBufferEnum::Buff_10M;
        _buffer = __KF_INT8__( _length );
        return _redis.Connect( ip, port, password );
    }

    void KFRedisExecute::ShutDown()
    {
        _redis.ShutDown();
    }

    void KFRedisExecute::SelectIndex( uint32 index )
    {
        if ( index == _index )
        {
            return;
        }

        _index = index;
        VoidExecute( "select %u", _index );
    }

#define __FORMAT_BUFFER__\
    memset( _buffer, 0, _length );\
    va_list args;\
    va_start( args, format );\
    vsprintf( _buffer, format, args );\
    va_end( args );\

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFRedisExecute::AppendCommand( const char* format, ... )
    {
        __FORMAT_BUFFER__;
        _commands.push_back( _buffer );
    }

    void KFRedisExecute::AppendCommand( const VectorString& fieldvalue, const char* format, ... )
    {
        __FORMAT_BUFFER__;

        KFRedisFormat kfformat;
        kfformat.Append( _buffer );
        kfformat.Append( fieldvalue );
        _commands.push_back( kfformat.ToString().c_str() );
    }

    void KFRedisExecute::AppendCommand( const MapString& fieldvalue, const char* format, ... )
    {
        __FORMAT_BUFFER__;

        KFRedisFormat kfformat;
        kfformat.Append( _buffer );
        kfformat.Append( fieldvalue );
        _commands.push_back( kfformat.ToString().c_str() );
    }

    bool KFRedisExecute::PipelineExecute()
    {
        bool result = PipelineExecute( _commands );
        _commands.clear();

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRedisExecute::PipelineExecute( const ListString& commands )
    {
        try
        {
            return _redis.PipelineExecute( commands );
        }
        catch ( KFRedisException& )
        {
            // 已经断线, 重新连接
            if ( _redis.IsDisconnected() )
            {
                _redis.ReConnect();

                // 重新执行一遍
                return _redis.PipelineExecute( commands );
            }
        }

        return false;
    }

    bool KFRedisExecute::PipelineExecute( ListString& commands, MapString& value )
    {
        try
        {
            return _redis.PipelineExecute( commands, value );
        }
        catch ( KFRedisException& )
        {
            // 已经断线, 重新连接
            if ( _redis.IsDisconnected() )
            {
                _redis.ReConnect();

                // 重新执行一遍
                return _redis.PipelineExecute( commands, value );
            }
        }

        return false;
    }

    bool KFRedisExecute::PipelineExecute( ListString& commands, VectorString& value )
    {
        try
        {
            return _redis.PipelineExecute( commands, value );
        }
        catch ( KFRedisException& )
        {
            // 已经断线, 重新连接
            if ( _redis.IsDisconnected() )
            {
                _redis.ReConnect();

                // 重新执行一遍
                return _redis.PipelineExecute( commands, value );
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRedisExecute::VoidExecute( const char* format, ... )
    {
        static uint32 _value = 0;

        __FORMAT_BUFFER__;
        return CommandExecute< uint32 >( _value, _buffer );
    }

    bool KFRedisExecute::VoidExecute( const VectorString& fieldvalue, const char* format, ... )
    {
        static uint32 _value = 0;

        __FORMAT_BUFFER__;
        KFRedisFormat kfformat;
        kfformat.Append( _buffer );
        kfformat.Append( fieldvalue );
        return CommandExecute< uint32 >( _value, kfformat.ToString().c_str() );
    }

    bool KFRedisExecute::VoidExecute( const MapString& fieldvalue, const char* format, ... )
    {
        static uint32 _value = 0;

        __FORMAT_BUFFER__;
        KFRedisFormat kfformat;
        kfformat.Append( _buffer );
        kfformat.Append( fieldvalue );
        return CommandExecute< uint32 >( _value, kfformat.ToString().c_str() );
    }

    bool KFRedisExecute::UInt32Execute( uint32& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;

        std::string strvalue = "";
        bool result = CommandExecute< std::string >( strvalue, _buffer );
        if ( result )
        {
            value = KFUtility::ToValue< uint32 >( strvalue );
        }

        return result;
    }

    bool KFRedisExecute::UInt64Execute( uint64& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;
        return CommandExecute< uint64 >( value, _buffer );
    }

    bool KFRedisExecute::StringExecute( std::string& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;
        return CommandExecute< std::string >( value, _buffer );
    }

    bool KFRedisExecute::MapExecute( MapString& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;
        return CommandExecute< MapString >( value, _buffer );
    }

    bool KFRedisExecute::VectorExecute( VectorString& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;
        return CommandExecute< VectorString >( value, _buffer );
    }

    bool KFRedisExecute::MapExecute( GreaterMapString& value, const char* format, ... )
    {
        __FORMAT_BUFFER__;
        return CommandExecute< GreaterMapString >( value, _buffer );
    }

}