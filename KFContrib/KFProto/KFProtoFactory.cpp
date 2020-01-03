#include "KFProtoFactory.h"
#include "google/protobuf/message.h"
#include "google/protobuf/compiler/importer.h"

namespace KFrame
{
    google::protobuf::Message* KFProtoFactory::CreateMessage( const std::string& name )
    {
        // 提取类型的描述信息
        auto descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName( name );
        if ( descriptor == nullptr )
        {
            __LOG_ERROR__( "can't find [{}] descriptor", name );
            return nullptr;
        }

        auto proto = google::protobuf::MessageFactory::generated_factory()->GetPrototype( descriptor );
        if ( proto == nullptr )
        {
            __LOG_ERROR__( "can't find [{}] proto type", name );
            return nullptr;
        }

        return proto->New();
    }

    void KFProtoFactory::DestroyMessage( google::protobuf::Message* message )
    {
        message->Clear();
        delete message;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __SET_VALUE_DEFINE__( intype, settype ) \
    void KFProtoFactory::Set##settype( google::protobuf::Message* message, const std::string& name, intype value )\
    {\
        auto field = message->GetDescriptor()->FindFieldByName( name );\
        if ( field == nullptr )\
        {   \
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );\
            return;\
        }\
        auto reflection = message->GetReflection();\
        reflection->Set##settype( message, field, value );\
    }

#define __GET_VALUE_DEFINE__( outtype, gettype, defalutvalue ) \
    outtype KFProtoFactory::Get##gettype( google::protobuf::Message* message, const std::string& name )\
    {\
        auto field = message->GetDescriptor()->FindFieldByName( name );\
        if ( field == nullptr )\
        {   \
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );\
            return defalutvalue; \
        }\
        auto reflection = message->GetReflection();\
        return reflection->Get##gettype( *message, field );\
    }

    __SET_VALUE_DEFINE__( int32, Int32 );
    __GET_VALUE_DEFINE__( int32, Int32, 0 );

    __SET_VALUE_DEFINE__( uint32, UInt32 );
    __GET_VALUE_DEFINE__( uint32, UInt32, 0u );

    __SET_VALUE_DEFINE__( int64, Int64 );
    __GET_VALUE_DEFINE__( int64, Int64, 0 );

    __SET_VALUE_DEFINE__( uint64, UInt64 );
    __GET_VALUE_DEFINE__( uint64, UInt64, 0u );

    __SET_VALUE_DEFINE__( float, Float );
    __GET_VALUE_DEFINE__( float, Float, 0.0f );

    __SET_VALUE_DEFINE__( double, Double );
    __GET_VALUE_DEFINE__( double, Double, 0.0f );

    __SET_VALUE_DEFINE__( const std::string&, String );
    __GET_VALUE_DEFINE__( std::string, String, _invalid_string );

    const google::protobuf::Message* KFProtoFactory::GetMessage( google::protobuf::Message* message, const std::string& name )
    {
        auto field = message->GetDescriptor()->FindFieldByName( name );
        if ( field == nullptr )
        {
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );
            return nullptr;
        }

        auto reflection = message->GetReflection();
        return &reflection->GetMessage( *message, field );
    }

    void KFProtoFactory::SetMessage( google::protobuf::Message* message, const std::string& name, const google::protobuf::Message* value )
    {
        auto field = message->GetDescriptor()->FindFieldByName( name );
        if ( field == nullptr )
        {
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );
            return;
        }

        auto reflection = message->GetReflection();
        auto copymessage = reflection->MutableMessage( message, field );
        copymessage->CopyFrom( *value );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int32 KFProtoFactory::GetRepeatedCount( google::protobuf::Message* message, const std::string& name )
    {
        auto field = message->GetDescriptor()->FindFieldByName( name );
        if ( field == nullptr || !field->is_repeated() )
        {
            return 0;
        }

        auto reflection = message->GetReflection();
        switch ( field->type() )
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
            return reflection->GetRepeatedField< google::protobuf::int32 >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_UINT32:
            return reflection->GetRepeatedField< google::protobuf::uint32 >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
            return reflection->GetRepeatedField< google::protobuf::int64 >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_UINT64:
            return reflection->GetRepeatedField< google::protobuf::uint64 >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            return reflection->GetRepeatedField< float >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            return reflection->GetRepeatedField< double >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_STRING:
        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            return reflection->GetRepeatedPtrField< std::string >( *message, field ).size();
            break;
        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            return reflection->GetRepeatedPtrField< google::protobuf::Message >( *message, field ).size();
            break;
        default:
            break;
        }

        return 0;
    }


#define __ADD_REPEATED_DEFINE__( intype, settype ) \
    void KFProtoFactory::AddRepeated##settype( google::protobuf::Message* message, const std::string& name, intype value )\
    {   \
        auto field = message->GetDescriptor()->FindFieldByName( name );  \
        if ( field == nullptr )\
        {   \
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );  \
            return; \
        }\
        if ( !field->is_repeated() )\
        {\
            __LOG_ERROR__( "message[{}] descriptor[{}] is not repeated", message->GetTypeName(), name );  \
            return; \
        }\
        auto reflection = message->GetReflection(); \
        reflection->Add##settype( message, field, value );\
    }

#define __GET_REPEATED_DEFINE__( outtype, gettype, defalutvalue ) \
    outtype KFProtoFactory::GetRepeated##gettype( const google::protobuf::Message* message, const std::string& name, int32 index )\
    {\
        auto field = message->GetDescriptor()->FindFieldByName( name );\
        if ( field == nullptr )\
        {   \
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );  \
            return defalutvalue; \
        }\
        if ( !field->is_repeated() )\
        {\
            __LOG_ERROR__( "message[{}] descriptor[{}] is not repeated", message->GetTypeName(), name );  \
            return defalutvalue; \
        }\
        auto reflection = message->GetReflection();\
        return reflection->GetRepeated##gettype( *message, field, index );\
    }

    __ADD_REPEATED_DEFINE__( int32, Int32 );
    __GET_REPEATED_DEFINE__( int32, Int32, 0 );

    __ADD_REPEATED_DEFINE__( uint32, UInt32 );
    __GET_REPEATED_DEFINE__( uint32, UInt32, 0u );

    __ADD_REPEATED_DEFINE__( int64, Int64 );
    __GET_REPEATED_DEFINE__( int64, Int64, 0 );

    __ADD_REPEATED_DEFINE__( uint64, UInt64 );
    __GET_REPEATED_DEFINE__( uint64, UInt64, 0u );

    __ADD_REPEATED_DEFINE__( float, Float );
    __GET_REPEATED_DEFINE__( float, Float, 0.0f );

    __ADD_REPEATED_DEFINE__( double, Double );
    __GET_REPEATED_DEFINE__( double, Double, 0.0f );

    __ADD_REPEATED_DEFINE__( const std::string&, String );
    __GET_REPEATED_DEFINE__( std::string, String, _invalid_string );

    void KFProtoFactory::AddRepeatedMessage( google::protobuf::Message* message, const std::string& name, google::protobuf::Message* value )
    {
        auto field = message->GetDescriptor()->FindFieldByName( name );
        if ( field == nullptr )
        {
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );

            return;

        }
        if ( !field->is_repeated() )
        {
            __LOG_ERROR__( "message[{}] descriptor[{}] is not repeated", message->GetTypeName(), name );

            return;

        }
        auto reflection = message->GetReflection();
        auto submessage = reflection->AddMessage( message, field );
        submessage->CopyFrom( *value );
    }

    const google::protobuf::Message* KFProtoFactory::GetRepeatedMessage( const google::protobuf::Message* message, const std::string& name, int32 index )
    {
        auto field = message->GetDescriptor()->FindFieldByName( name );
        if ( field == nullptr )
        {
            __LOG_ERROR__( "message[{}] can't find [{}] descriptor", message->GetTypeName(), name );
            return nullptr;
        }
        if ( !field->is_repeated() )
        {
            __LOG_ERROR__( "message[{}] descriptor[{}] is not repeated", message->GetTypeName(), name );
            return nullptr;
        }
        auto reflection = message->GetReflection();
        return &reflection->GetRepeatedMessage( *message, field, index );
    }
}