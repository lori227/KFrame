#ifndef __KF_TITLE_MODULE_H__
#define __KF_TITLE_MODULE_H__

/************************************************************************
//    @Moudle			:    称号模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-2-7
************************************************************************/

#include "KFrame.h"
#include "KFTitleInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFTitleModule : public KFTitleInterface
    {
    public:
        KFTitleModule();
        ~KFTitleModule();

        // 加载配置
        virtual void InitMoudle();

        // 初始化
        virtual void BeforeRun();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 玩家构件
        KFComponent* _kf_player_logic;
    };
}



#endif