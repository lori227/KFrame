#ifndef __KF_MYSQL_DRIVER_H__
#define __KF_MYSQL_DRIVER_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMySQLDriver
    {
    public:
        // 插入记录
        virtual bool Insert( const std::string& table, const StringMap& values ) = 0;

        // 删除纪录
        virtual bool Delete( const std::string& table ) = 0;
        virtual bool Delete( const std::string& table, const std::string& key ) = 0;
        virtual bool Delete( const std::string& table, const StringMap& keys ) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////
        // 更新字段
        template<class T>
        bool Update( const std::string& table, const std::string& field, T value )
        {
            StringMap values;
            values[ field ] = KFUtility::ToString<T>( value );
            return Update( table, values );
        }

        // 默认字段
        template<class T>
        bool Update( const std::string& table, const std::string& key, const std::string& field, T value )
        {
            StringMap values;
            values[ field ] = KFUtility::ToString<T>( value );

            StringMap keys;
            keys[ __STRING__( id ) ] = key;

            return Update( table, keys, values );
        }

        // 更新多个字段
        virtual bool Update( const std::string& table, const StringMap& value ) = 0;
        virtual bool Update( const std::string& table, const std::string& key, const StringMap& values ) = 0;
        virtual bool Update( const std::string& table, const StringMap& keys, const StringMap& values ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////
        template<typename... P>
        KFResult<voidptr>::UniqueType Execute( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return VoidExecute( sql );
        }

        template<typename... P>
        KFResult<uint32>::UniqueType QueryUInt32( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return UInt32Execute( sql );
        }

        template<typename... P>
        KFResult<uint64>::UniqueType QueryUint64( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return UInt64Execute( sql );
        }

        template<typename... P>
        KFResult<std::string>::UniqueType QueryString( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return StringExecute( sql );
        }

        template<typename... P>
        KFResult<StringList>::UniqueType QueryList( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ListExecute( sql );
        }

        template<typename... P>
        KFResult<StringMap>::UniqueType QueryMap( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return MapExecute( sql );
        }

        template<typename... P>
        KFResult<StringMapList>::UniqueType QueryListMap( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ListMapExecute( sql );
        }

        // 查询所有
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table ) = 0;
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringList& field_list ) = 0;
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table, const std::string& key ) = 0;
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table, const std::string& key, const StringList& field_list ) = 0;
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringMap& keys ) = 0;
        virtual KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringMap& keys, const StringList& field_list ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 事务( 目前只批量执行, 没有加上mysql事务锁 )
        virtual void Pipeline( StringList& commands ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual KFResult<voidptr>::UniqueType VoidExecute( std::string& sql ) = 0;
        virtual KFResult<uint32>::UniqueType UInt32Execute( std::string& sql ) = 0;
        virtual KFResult<uint64>::UniqueType UInt64Execute( std::string& sql ) = 0;
        virtual KFResult<std::string>::UniqueType StringExecute( std::string& sql ) = 0;
        virtual KFResult<StringMap>::UniqueType MapExecute( std::string& sql ) = 0;
        virtual KFResult<StringList>::UniqueType ListExecute( std::string& sql ) = 0;
        virtual KFResult<StringMapList>::UniqueType ListMapExecute( std::string& sql ) = 0;
    };
}

#endif