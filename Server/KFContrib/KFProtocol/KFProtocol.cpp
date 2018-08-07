#include "KFrame.h"
#include "KFProtocol.h"
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
            std::string name = proto->GetTypeName();
            __LOG_ERROR__( KFLogEnum::System, "message[{}] parse failed!", name );
        }

        return result;
    }

    std::string& KFProto::Serialize( const ::google::protobuf::Message* proto, uint32 mask )
    {
        static std::string result = "";
        result.clear();

        auto data = proto->SerializeAsString();
        switch ( mask )
        {
        case KFCompressEnum::Convert:
            result = KFDecode::UByteToString( data );
            break;
        case KFCompressEnum::Zib:
            KFCompress::Zib( data, result );
            break;
        case KFCompressEnum::Compress:
            KFCompress::Compress( data, result );
            break;
        default:
            result = data;
            break;
        }

        return result;
    }

    bool KFProto::Parse( ::google::protobuf::Message* proto, const std::string& data, uint32 mask )
    {
        std::string result = "";
        switch ( mask )
        {
        case KFCompressEnum::Convert:
            result = KFDecode::StringToUByte( data );
            break;
        case KFCompressEnum::Zib:
            KFCompress::UnZib( data, result );
            break;
        case KFCompressEnum::Compress:
            KFCompress::UnCompress( data, result );
            break;
        default:
            result = data;
            break;
        }

        auto ok = proto->ParseFromString( result );
        if ( !ok )
        {
            std::string name = proto->GetTypeName();
            __LOG_ERROR__( KFLogEnum::System, "message[{}] parse failed!", name );
        }

        return ok;
    }
}