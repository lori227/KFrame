#ifndef __KF_STATE_MANAGE_H__
#define __KF_STATE_MANAGE_H__

#include "KFState.hpp"

namespace KFrame
{
    class KFStateManage : public KFSingleton< KFStateManage >
    {
    public:
        KFStateManage();
        virtual ~KFStateManage();

        // 初始化
        void Initialize();

        // 查找状态
        KFState* FindState( uint32 state );

    protected:
        // 添加状态
        void AddState( uint32 state, KFState* kfstate );

    protected:

        // 状态列表
        std::map< uint32, KFState* > _states;
    };

    ///////////////////////////////////////////////////
    static auto _kf_state_manage = KFStateManage::Instance();
    ///////////////////////////////////////////////////
}

#endif