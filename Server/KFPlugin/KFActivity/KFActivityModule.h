#ifndef __KF_ACHIEVE_MOUDLE_H__
#define __KF_ACHIEVE_MOUDLE_H__

/************************************************************************
//    @Module			:    运营活动
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/

#include "KFrame.h"
#include "KFActivityInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFActivityModule : public KFActivityInterface
    {
    public:
        KFActivityModule() = default;
        ~KFActivityModule() = default;

        // 初始化
        virtual void InitModule();

        // 运行
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 领取奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveActivityRewardReq );

    private:
        // 领取成就奖励
        uint32 ReceiveActivityReward( KFEntity* player, uint32 type, uint32 activityid );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 活动数值更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnActivityValueUpdateCallBack );

        // 属性更新回调
        __KF_ADD_DATA_FUNCTION__( OnAddDataCallBack );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataCallBack );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );

        // 更新成就
        void UpdateDataActivityValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 newvalue );
        void UpdateObjectActivityValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate );

        // 判断每日活动重置
        void CheckResetDailyActivity( KFEntity* player );

    private:
        KFComponent* _kf_componnet{ nullptr };
    };
}



#endif