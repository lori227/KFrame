#ifndef __KF_MAIL_CONFIG_H__
#define __KF_MAIL_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

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

    class KFMailConfig : public KFIntConfigT< KFMailSetting >, public KFSingleton< KFMailConfig >
    {
    public:
        KFMailConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFMailSetting >( file, isclear )
        {
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFMailSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_mail_config = KFMailConfig::Instance( "mail.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif