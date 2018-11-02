#ifndef __KF_MYSQL_INTERFACE_H__
#define __KF_MYSQL_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMySQLDriver
    {
    public:
        // 插入记录
        virtual bool Insert( const std::string& table, const MapString& invalue ) = 0;

        // 删除几率
        virtual bool Delete( const std::string& table ) = 0;
        virtual bool Delete( const std::string& table, const std::string& key ) = 0;
        virtual bool Delete( const std::string& table, const MapString& keyvalues ) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////
        // 更新字段
        template< class T >
        bool Update( const std::string& table, const std::string& field, T invalue )
        {
            MapString updatevalue;
            updatevalue[ field ] = KFUtility::ToString< T >( invalue );
            return Update( table, updatevalue );
        }

        // 默认字段
        template< class T >
        bool Update( const std::string& table, const std::string& key, const std::string& field, T invalue )
        {
            MapString updatevalue;
            updatevalue[ field ] = KFUtility::ToString< T >( invalue );

            MapString keyvalue;
            keyvalue[ __KF_STRING__( id ) ] = key;

            return Update( table, keyvalue, updatevalue );
        }

        // 更新多个字段
        virtual bool Update( const std::string& table, const MapString& invalue ) = 0;
        virtual bool Update( const std::string& table, const std::string& key, const MapString& invalue ) = 0;
        virtual bool Update( const std::string& table, const MapString& keyvalue, const MapString& invalue ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////
        template< typename... P >
        KFResult< voidptr >* Execute( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return VoidExecute( strsql );
        }

        template< typename... P >
        KFResult< uint32 >* QueryUInt32( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt32Execute( strsql );
        }

        template< typename... P >
        KFResult< uint64 >* QueryUint64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return UInt64Execute( strsql );
        }

        template< typename... P >
        KFResult< std::string >* QueryString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return StringExecute( strsql );
        }

        template< typename... P >
        KFResult< ListString >* QueryList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListExecute( strsql );
        }

        template< typename... P >
        KFResult< MapString >* QueryMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return MapExecute( strsql );
        }

        template< typename... P >
        KFResult< std::list< MapString > >* QueryListMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ListMapExecute( strsql );
        }

        // 查询所有
        virtual KFResult< std::list< MapString > >* Select( const std::string& table ) = 0;
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const ListString& fields ) = 0;
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const std::string& key ) = 0;
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const std::string& key, const ListString& fields ) = 0;
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const MapString& key ) = 0;
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const MapString& key, const ListString& fields ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 事务( 目前只批量执行, 没有加上mysql事务锁 )
        virtual void Pipeline( const ListString& commands ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual KFResult< voidptr >* VoidExecute( const std::string& strsql ) = 0;
        virtual KFResult< uint32 >* UInt32Execute( const std::string& strsql ) = 0;
        virtual KFResult< uint64 >* UInt64Execute( const std::string& strsql ) = 0;
        virtual KFResult< std::string >* StringExecute( const std::string& strsql ) = 0;
        virtual KFResult< MapString >* MapExecute( const std::string& strsql ) = 0;
        virtual KFResult< ListString >* ListExecute( const std::string& strsql ) = 0;
        virtual KFResult< std::list< MapString > >* ListMapExecute( const std::string& strsql ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    class KFMySQLInterface : public KFModule
    {
    public:
        // 创建Execute
        virtual KFMySQLDriver* CreateExecute( const std::string& module, uint32 logicid = 0 ) = 0;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mysql, KFMySQLInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////
}

#endif