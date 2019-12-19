#ifndef __KF_MYSQL_INTERFACE_H__
#define __KF_MYSQL_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMySQLDriver
    {
    public:
        // 插入记录
        virtual bool Insert( const std::string& table, const StringMap& invalue ) = 0;

        // 删除几率
        virtual bool Delete( const std::string& table ) = 0;
        virtual bool Delete( const std::string& table, const std::string& key ) = 0;
        virtual bool Delete( const std::string& table, const StringMap& keyvalues ) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////
        // 更新字段
        template< class T >
        bool Update( const std::string& table, const std::string& field, T invalue )
        {
            StringMap updatevalue;
            updatevalue[ field ] = KFUtility::ToString< T >( invalue );
            return Update( table, updatevalue );
        }

        // 默认字段
        template< class T >
        bool Update( const std::string& table, const std::string& key, const std::string& field, T invalue )
        {
            StringMap updatevalue;
            updatevalue[ field ] = KFUtility::ToString< T >( invalue );

            StringMap keyvalue;
            keyvalue[ __STRING__( id ) ] = key;

            return Update( table, keyvalue, updatevalue );
        }

        // 更新多个字段
        virtual bool Update( const std::string& table, const StringMap& invalue ) = 0;
        virtual bool Update( const std::string& table, const std::string& key, const StringMap& invalue ) = 0;
        virtual bool Update( const std::string& table, const StringMap& keyvalue, const StringMap& invalue ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////
        template< typename... P >
        KFResult< voidptr >::UniqueType Execute( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return VoidExecute( strsql );
        }

        template< typename... P >
        KFResult< uint32 >::UniqueType QueryUInt32( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt32Execute( strsql );
        }

        template< typename... P >
        KFResult< uint64 >::UniqueType QueryUint64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt64Execute( strsql );
        }

        template< typename... P >
        KFResult< std::string >::UniqueType QueryString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return StringExecute( strsql );
        }

        template< typename... P >
        KFResult< StringList >::UniqueType QueryList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListExecute( strsql );
        }

        template< typename... P >
        KFResult< StringMap >::UniqueType QueryMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return MapExecute( strsql );
        }

        template< typename... P >
        KFResult< StringListMap >::UniqueType QueryListMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListMapExecute( strsql );
        }

        // 查询所有
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table ) = 0;
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table, const StringList& fields ) = 0;
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table, const std::string& key ) = 0;
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table, const std::string& key, const StringList& fields ) = 0;
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table, const StringMap& key ) = 0;
        virtual KFResult< StringListMap >::UniqueType Select( const std::string& table, const StringMap& key, const StringList& fields ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 事务( 目前只批量执行, 没有加上mysql事务锁 )
        virtual void Pipeline( StringList& commands ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual KFResult< voidptr >::UniqueType VoidExecute( std::string& strsql ) = 0;
        virtual KFResult< uint32 >::UniqueType UInt32Execute( std::string& strsql ) = 0;
        virtual KFResult< uint64 >::UniqueType UInt64Execute( std::string& strsql ) = 0;
        virtual KFResult< std::string >::UniqueType StringExecute( std::string& strsql ) = 0;
        virtual KFResult< StringMap >::UniqueType MapExecute( std::string& strsql ) = 0;
        virtual KFResult< StringList >::UniqueType ListExecute( std::string& strsql ) = 0;
        virtual KFResult< StringListMap >::UniqueType ListMapExecute( std::string& strsql ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    class KFMySQLInterface : public KFModule
    {
    public:
        // 创建Execute
        virtual KFMySQLDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mysql, KFMySQLInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////
}

#endif