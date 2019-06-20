#include "KFMailConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMailConfig::ReadSetting( KFNode& xmlnode, KFMailSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_reply_id = xmlnode.GetUInt32( "ReplyId" );

        kfsetting->_title = xmlnode.GetString( "Title" );
        KFUtility::ReplaceString( kfsetting->_title, " ", "" );

        kfsetting->_content = xmlnode.GetString( "Content" );
        KFUtility::ReplaceString( kfsetting->_content, " ", "" );

        kfsetting->_valid_time = xmlnode.GetUInt32( "ValidTime" ) * KFTimeEnum::OneDaySecond;
        kfsetting->_extend = xmlnode.GetString( "Extend" );
        KFUtility::ReplaceString( kfsetting->_extend, " ", "" );

        auto stragent = xmlnode.GetString( "Reward" );
        kfsetting->_rewards.Parse( stragent, __FUNC_LINE__ );
    }
}