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
        KFResult< voidptr >::UniqueType Update( const T& values, const char* myfmt, P&& ... args )
        {
            auto strvalue = KFRedisFormat::ToString< T >( values );
            auto strformat = __FORMAT__( myfmt, std::forward<P>( args )... );

            auto strsql = strformat + " " + strvalue;
            return VoidExecute( strsql );
        }

        // 执行数据库
        template< typename... P >
        KFResult< uint64 >::UniqueType Execute( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UpdateExecute( strsql );
        }

        // uint64
        // 返回的integer结果 或者 数字字符串
        template< typename... P >
        KFResult< uint64 >::UniqueType QueryUInt64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt64Execute( strsql );
        }

        // string
        // redis 直接返回的string结果
        template< typename... P >
        KFResult< std::string >::UniqueType QueryString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return StringExecute( strsql );
        }

        // mapstring
        // redis 返回的键值对, 如hash表
        template< typename... P >
        KFResult< MapString >::UniqueType QueryMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return MapExecute( strsql );
        }

        // liststring
        // 返回的数组
        template< typename... P >
        KFResult< ListString >::UniqueType QueryList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListExecute( strsql );
        }

        // vectorstring
        // 返回的数组
        template< typename... P >
        KFResult< VectorString >::UniqueType QueryVector( const char* myfmt, P&& ... args )
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
        virtual KFResult< voidptr >::UniqueType Pipeline() = 0;

        // 指令集
        // 返回string列表
        template< typename... P >
        KFResult< ListString >::UniqueType PipelineList()
        {
            return ListPipelineExecute();
        }

        // 返回map列表
        template< typename... P >
        KFResult< ListMapString >::UniqueType PipelineListMap()
        {
            return ListMapPipelineExecute();
        }

    protected:
        // 执行更新
        virtual KFResult< voidptr >::UniqueType VoidExecute( const std::string& strsql ) = 0;
        virtual KFResult< uint64 >::UniqueType UpdateExecute( const std::string& strsql ) = 0;

        // 查询数据库
        virtual KFResult< uint64 >::UniqueType UInt64Execute( const std::string& strsql ) = 0;
        virtual KFResult< std::string >::UniqueType StringExecute( const std::string& strsql ) = 0;
        virtual KFResult< MapString >::UniqueType MapExecute( const std::string& strsql ) = 0;
        virtual KFResult< ListString >::UniqueType ListExecute( const std::string& strsql ) = 0;
        virtual KFResult< VectorString >::UniqueType VectorExecute( const std::string& strsql ) = 0;

        // 添加执行命令
        virtual void AppendCommand( const std::string& strsql ) = 0;
        virtual KFResult< ListString >::UniqueType ListPipelineExecute() = 0;
        virtual KFResult< ListMapString >::UniqueType ListMapPipelineExecute() = 0;
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