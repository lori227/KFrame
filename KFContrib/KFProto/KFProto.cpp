#include "KFrame.h"
#include "KFProto.h"
#include "google/protobuf/message.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFProto::Parse( google::protobuf::Message* proto, const int8* data, uint32 length )
    {
        if ( data == nullptr || length == 0 )
        {
            return true;
        }

        bool result = proto->ParseFromArray( data, length );
        if ( !result )
        {
            __LOG_ERROR__( "message[{}] parse failed", proto->GetTypeName() );
        }

        return result;
    }

    std::string& KFProto::Serialize( const ::google::protobuf::Message* proto, uint32 compress_type, uint32 compress_level, bool convert )
    {
        static std::string result = "";
        result.clear();

        auto data = proto->SerializeAsString();
        KFCompress::Compress( data, result, compress_type, compress_level, convert );

        return result;
    }

    bool KFProto::Parse( ::google::protobuf::Message* proto, const std::string& data, uint32 compress_type, bool convert )
    {
        std::string result = "";
        auto ok = KFCompress::UnCompress( data, result, compress_type, convert );
        if ( !result.empty() )
        {
            ok = proto->ParseFromString( result );
            if ( !ok )
            {
                __LOG_ERROR__( "message[{}] parse failed", proto->GetTypeName() );
            }
        }
        else
        {
            if ( !ok )
            {
                __LOG_ERROR__( "message[{}] data empty", proto->GetTypeName() );
            }
        }

        return ok;
    }
}