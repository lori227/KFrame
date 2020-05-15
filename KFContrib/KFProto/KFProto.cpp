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

    std::string& KFProto::Serialize( const ::google::protobuf::Message* proto, uint32 compresstype, uint32 compresslevel, bool convert )
    {
        static std::string result = "";
        result.clear();

        auto data = proto->SerializeAsString();
        KFCompress::Compress( data, result, compresstype, compresslevel, convert );

        return result;
    }

    bool KFProto::Parse( ::google::protobuf::Message* proto, const std::string& data, uint32 compresstype, bool convert )
    {
        std::string result = "";
        auto ok = KFCompress::UnCompress( data, result, compresstype, convert );
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