#ifndef __KF_CLOTHES_SETTING_H__
#define __KF_CLOTHES_SETTING_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    class KFClothesSetting
    {
    public:
        KFClothesSetting();

        bool HasModle( uint32 modelid ) const;

    public:

        // 物品id
        uint32 _id;

        // 删除物品发送的邮件id
        uint32 _mail_id;

        // 品质
        uint32 _quality;

        // 对应模型的id
        std::set< uint32 > _model_list;

        // 时限时装兑换奖励
        KFAgents _exchange;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

}


#endif