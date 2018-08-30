#ifndef __KF_REDIS_INTERFACE_H__
#define __KF_REDIS_INTERFACE_H__

#include "KFrame.h"
#include "KFRedisFormat.h"

namespace KFrame
{

    // kfresult 是成员函数, 如果使用相同的query需要把先前的结果保存起来
    // 如: 使用querymap以后, 把结果保存到自己定义的map中, 然后才能在次使用querymap,
    // 否则使用第一次的查询结果会发生错误, 循环是导致宕机

    class KFRedisDriver
    {
    public:
        // 执行数据库
        template< typename... P >
        KFResult< voidptr >* Execute( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UpdateExecute( strsql );
        }

        // 更新数据库
        template< typename T, typename... P >
        KFResult< voidptr >* Update( const T& values, const char* myfmt, P&& ... args )
        {
            auto strvalue = KFRedisFormat::ToString< T >( values );
            auto strformat = __FORMAT__( myfmt, std::forward<P>( args )... );

            auto strsql = strformat + " " + strvalue;
            return UpdateExecute( strsql );
        }

        // uint32
        // redis 其实不会返回uint32 这个函数只是查询string, 然后转成uint32
        // 注意需要查询的数值一定是可以转成uint32的, 否则返回会不正确
        template< typename... P >
        KFResult< uint32 >* QueryUInt32( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt32Execute( strsql );
        }

        // uint64
        // redis 直接返回的integer结果
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

        template< typename... P >
        KFResult< GreaterMapString >* QueryGreaterMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return GreaterMapExecute( strsql );
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
        virtual KFResult< voidptr >* UpdateExecute( const std::string& strsql ) = 0;

        // 查询数据库
        virtual KFResult< uint32 >* UInt32Execute( const std::string& strsql ) = 0;
        virtual KFResult< uint64 >* UInt64Execute( const std::string& strsql ) = 0;
        virtual KFResult< std::string >* StringExecute( const std::string& strsql ) = 0;
        virtual KFResult< MapString >* MapExecute( const std::string& strsql ) = 0;
        virtual KFResult< GreaterMapString >* GreaterMapExecute( const std::string& strsql ) = 0;
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
        // 创建RedisExecute
        virtual KFRedisDriver* CreateExecute( uint32 id ) = 0;
        virtual KFRedisDriver* CreateExecute( const std::string& field, uint32 logicid = 0 ) = 0;
        virtual KFRedisDriver* CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& passwrod ) = 0;
    };


    ///////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_redis, KFRedisInterface );
    ///////////////////////////////////////////////////////////////////////////////

}

#endif