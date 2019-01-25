#ifndef __KF_CHAT_CONFIG_H__
#define __KF_CHAT_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFChatSetting
    {
    public:
        KFChatSetting()
        {
            _chat_type = 0;
            _chat_interval = 0;
            _chat_byte_length = 0;
        }

    public:
        //聊天室id
        uint32 _chat_type;
        //聊天间隔时间
        uint32 _chat_interval;
        //聊天消息字节数限制
        uint32 _chat_byte_length;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFChatConfig : public KFConfig, public KFSingleton< KFChatConfig >
    {
    public:
        KFChatConfig();
        ~KFChatConfig();

        // 加载配置
        bool LoadConfig();

        // 查找聊天室设置
        const KFChatSetting* FindChatSetting( uint32 chattype ) const;

    public:
        // 聊天室配置列表
        KFMap< uint32, uint32, KFChatSetting > _kf_chat_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_chat_config = KFChatConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif