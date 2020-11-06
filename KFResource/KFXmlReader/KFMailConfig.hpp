#ifndef __KF_MAIL_CONFIG_H__
#define __KF_MAIL_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFMailSetting : public KFIntSetting
    {
    public:
        // 邮件的类型
        uint32 _type = 0u;

        // 标题
        std::string _title;

        // 内容
        std::string _content;

        // 有效时间
        uint32 _valid_time = 0u;

        // 附件奖励配置
        KFElements _rewards;

        // 读取/领取完成后是否立即删除
        uint32 _del_now = 0u;

        // 领取邮件的回复邮件id
        uint32 _reply_id = 0u;

        // 附加信息
        std::string _extend;
    };

    ////////////////////////////////////////////////////////////////////////////////////

    class KFMailConfig : public KFConfigT< KFMailSetting >, public KFInstance< KFMailConfig >
    {
    public:
        KFMailConfig()
        {
            _file_name = "mail";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                KFElementConfig::Instance()->ParseElement( kfsetting->_rewards, __FILE__, __LINE__ );
            }
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFMailSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.ReadUInt32( "Type" );
            kfsetting->_reply_id = xmlnode.ReadUInt32( "ReplyId" );

            kfsetting->_title = xmlnode.ReadString( "Title" );
            KFUtility::ReplaceString( kfsetting->_title, " ", "" );

            kfsetting->_content = xmlnode.ReadString( "Content" );
            KFUtility::ReplaceString( kfsetting->_content, " ", "" );

            kfsetting->_valid_time = xmlnode.ReadUInt32( "ValidTime" ) * KFTimeEnum::OneDaySecond;
            kfsetting->_extend = xmlnode.ReadString( "Extend" );
            KFUtility::ReplaceString( kfsetting->_extend, " ", "" );

            kfsetting->_rewards._str_parse = xmlnode.ReadString( "Reward" );
        }
    };
}

#endif