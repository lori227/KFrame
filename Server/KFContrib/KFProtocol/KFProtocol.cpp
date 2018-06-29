#include "KFProtocol.h"
#include "google/protobuf/message.h"
#include "KFLogger/KFLogger.h"


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
            KFLogger::LogSystem( KFLogger::Error, "message[%s] parse failed!", name.c_str() );
        }

        return result;
    }
}