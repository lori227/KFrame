#include "KFChatConfig.h"


namespace KFrame
{
    KFChatConfig::KFChatConfig()
    {
    }

    KFChatConfig::~KFChatConfig()
    {
    }

    bool KFChatConfig::LoadConfig()
    {
        _kf_chat_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto chattype = xmlnode.GetUInt32( "ChatType" );
            auto kfsetting = _kf_chat_setting.Create( chattype );

            kfsetting->_chat_type = chattype;
            kfsetting->_chat_interval = xmlnode.GetUInt32( "ChatInterval" );
            kfsetting->_chat_byte_length = xmlnode.GetUInt32( "ChatByteLength" );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFChatSetting* KFChatConfig::FindChatSetting( uint32 chattype ) const
    {
        return _kf_chat_setting.Find( chattype );
    }
}
