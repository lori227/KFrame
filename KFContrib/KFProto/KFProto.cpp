#include "KFrame.h"
#include "KFProto.h"
#include "google/protobuf/message.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFProto::Parse( std::shared_ptr<google::protobuf::Message> message, const int8* data, uint32 length )
    {
        return Parse( message.get(), data, length );
    }

    bool KFProto::Parse( google::protobuf::Message* message, const int8* data, uint32 length )
    {
        if ( data == nullptr || length == 0 )
        {
            return true;
        }

        bool result = message->ParseFromArray( data, length );
        if ( !result )
        {
            __LOG_ERROR__( "message[{}] parse failed", message->GetTypeName() );
        }

        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFProto::Parse( std::shared_ptr<google::protobuf::Message> message, const std::string& data, uint32 compress_type, bool convert )
    {
        return Parse( message.get(), data, compress_type, convert );
    }

    bool KFProto::Parse( google::protobuf::Message* message, const std::string& data, uint32 compress_type, bool convert )
    {
        std::string result = "";
        auto ok = KFCompress::UnCompress( data, result, compress_type, convert );
        if ( !result.empty() )
        {
            ok = message->ParseFromString( result );
            if ( !ok )
            {
                __LOG_ERROR__( "message[{}] parse failed", message->GetTypeName() );
            }
        }
        else
        {
            if ( !ok )
            {
                __LOG_ERROR__( "message[{}] data empty", message->GetTypeName() );
            }
        }

        return ok;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::string& KFProto::Serialize( std::shared_ptr<const google::protobuf::Message> message, uint32 compress_type, uint32 compress_level, bool convert )
    {
        return Serialize( message.get(), compress_type, compress_level, convert );
    }

    std::string& KFProto::Serialize( const google::protobuf::Message* message, uint32 compress_type, uint32 compress_level, bool convert )
    {
        static std::string result = "";
        result.clear();

        auto data = message->SerializeAsString();
        KFCompress::Compress( data, result, compress_type, compress_level, convert );

        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////

}