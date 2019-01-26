#ifndef __KF_REDIS_INTERFACE_H__
#define __KF_REDIS_INTERFACE_H__

#include "KFrame.h"
#include "KFRedisFormat.h"

namespace KFrame
{
    class KFRedisDriver
    {
    public:
        virtual ~KFRedisDriver() = default;

        // 更新数据库
        template< typename T, typename... P >
        KFResult< voidptr >* Update( const T& values, const char* myfmt, P&& ... args )
        {
            auto strvalue = KFRedisFormat::ToString< T >( values );
            auto strformat = __FORMAT__( myfmt, std::forward<P>( args )... );

            auto strsql = strformat + " " + strvalue;
            return VoidExecute( strsql );
        }

        // 执行数据库
        template< typename... P >
        KFResult< uint64 >* Execute( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UpdateExecute( strsql );
        }

        // uint64
        // 返回的integer结果 或者 数字字符串
        template< typename... P >
        KFResult< uint64 >* QueryUInt64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt64Execute( strsql );
        }

        // string
        // redis 直接返回的string结果
        template< typename... P >
        KFResult< std::string >* QueryString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return StringExecute( strsql );
        }

        // mapstring
        // redis 返回的键值对, 如hash表
        template< typename... P >
        KFResult< MapString >* QueryMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return MapExecute( strsql );
        }

        // liststring
        // 返回的数组
        template< typename... P >
        KFResult< ListString >* QueryList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListExecute( strsql );
        }

        // vectorstring
        // 返回的数组
        template< typename... P >
        KFResult< VectorString >* QueryVector( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return VectorExecute( strsql );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加指令
        template< typename... P >
        void Append( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            AppendCommand( strsql );
        }

        template< typename T, typename... P >
        void Append( const T& values, const char* myfmt, P&& ... args )
        {
            auto strvalue = KFRedisFormat::ToString< T >( values );
            auto strformat = __FORMAT__( myfmt, std::forward<P>( args )... );

            auto strsql = strformat + " " + strvalue;
            AppendCommand( strsql );
        }

        // 执行指令集
        virtual KFResult< voidptr >* Pipeline() = 0;

        // 指令集
        // 返回string列表
        template< typename... P >
        KFResult< ListString >* PipelineList()
        {
            return ListPipelineExecute();
        }

        // 返回map列表
        template< typename... P >
        KFResult < std::list< MapString > >* PipelineListMap()
        {
            return ListMapPipelineExecute();
        }

    protected:
        // 执行更新
        virtual KFResult< voidptr >* VoidExecute( const std::string& strsql ) = 0;
        virtual KFResult< uint64 >* UpdateExecute( const std::string& strsql ) = 0;

        // 查询数据库
        virtual KFResult< uint64 >* UInt64Execute( const std::string& strsql ) = 0;
        virtual KFResult< std::string >* StringExecute( const std::string& strsql ) = 0;
        virtual KFResult< MapString >* MapExecute( const std::string& strsql ) = 0;
        virtual KFResult< ListString >* ListExecute( const std::string& strsql ) = 0;
        virtual KFResult< VectorString >* VectorExecute( const std::string& strsql ) = 0;

        // 添加执行命令
        virtual void AppendCommand( const std::string& strsql ) = 0;
        virtual KFResult< ListString >* ListPipelineExecute() = 0;
        virtual KFResult< std::list< MapString > >* ListMapPipelineExecute() = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class KFRedisInterface : public KFModule
    {
    public:
        // 创建redis
        virtual KFRedisDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;
    };


    ///////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_redis, KFRedisInterface );
    ///////////////////////////////////////////////////////////////////////////////
}

#endif