#ifndef __KF_PROTO_FACTORY_H__
#define __KF_PROTO_FACTORY_H__

#include "KFrame.h"

namespace KFrame
{
#define __GET_VALUE_DECLARATION__( outtype, gettype )\
    outtype Get##gettype( google::protobuf::Message* message, const std::string& name );
#define __SET_VALUE_DECLARATION__( intype, settype )\
    void Set##settype( google::protobuf::Message* message, const std::string& name, intype value );
#define __GET_REPEATED_DECLARATION__( outtype, gettype )\
    outtype GetRepeated##gettype( const google::protobuf::Message* message, const std::string& name, int32 index );
#define __ADD_REPEATED_DECLARATION__( intype, settype )\
    void AddRepeated##settype( google::protobuf::Message* message, const std::string& name, intype value );

    class KFProtoFactory : public KFSingleton< KFProtoFactory >
    {
    public:
        // 创建消息
        google::protobuf::Message* CreateMessage( const std::string& name );

        // 销毁消息
        void DestroyMessage( google::protobuf::Message* );

        // 解析消息
        //google::protobuf::Message* ParserMessage( const std::string& name, const char* data, uint32 length );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 设置和获取数值
        __GET_VALUE_DECLARATION__( int32, Int32 );
        __SET_VALUE_DECLARATION__( int32, Int32 );

        __GET_VALUE_DECLARATION__( uint32, UInt32 );
        __SET_VALUE_DECLARATION__( uint32, UInt32 );

        __GET_VALUE_DECLARATION__( int64, Int64 );
        __SET_VALUE_DECLARATION__( int64, Int64 );

        __GET_VALUE_DECLARATION__( uint64, UInt64 );
        __SET_VALUE_DECLARATION__( uint64, UInt64 );

        __GET_VALUE_DECLARATION__( float, Float );
        __SET_VALUE_DECLARATION__( float, Float );

        __GET_VALUE_DECLARATION__( double, Double );
        __SET_VALUE_DECLARATION__( double, Double );

        __GET_VALUE_DECLARATION__( std::string, String );
        __SET_VALUE_DECLARATION__( const std::string&, String );

        __GET_VALUE_DECLARATION__( const google::protobuf::Message*, Message );
        __SET_VALUE_DECLARATION__( const google::protobuf::Message*, Message );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得Repeated字段数量
        int32 GetRepeatedCount( google::protobuf::Message* message, const std::string& name );

        __GET_REPEATED_DECLARATION__( int32, Int32 );
        __ADD_REPEATED_DECLARATION__( int32, Int32 );

        __GET_REPEATED_DECLARATION__( uint32, UInt32 );
        __ADD_REPEATED_DECLARATION__( uint32, UInt32 );

        __GET_REPEATED_DECLARATION__( int64, Int64 );
        __ADD_REPEATED_DECLARATION__( int64, Int64 );

        __GET_REPEATED_DECLARATION__( uint64, UInt64 );
        __ADD_REPEATED_DECLARATION__( uint64, UInt64 );

        __GET_REPEATED_DECLARATION__( float, Float );
        __ADD_REPEATED_DECLARATION__( float, Float );

        __GET_REPEATED_DECLARATION__( double, Double );
        __ADD_REPEATED_DECLARATION__( double, Double );

        __GET_REPEATED_DECLARATION__( std::string, String );
        __ADD_REPEATED_DECLARATION__( const std::string&, String );

        __GET_REPEATED_DECLARATION__( const google::protobuf::Message*, Message );
        __ADD_REPEATED_DECLARATION__( google::protobuf::Message*, Message );
    };
}


#endif
