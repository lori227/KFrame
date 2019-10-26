#ifndef __KF_ROLE_MODULE_H__
#define __KF_ROLE_MODULE_H__

/************************************************************************
//    @Module			:    角色逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-12-27
************************************************************************/

#include "KFRoleInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFRoleModule : public KFRoleInterface
    {
    public:
        KFRoleModule() = default;
        ~KFRoleModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToGameAck );
    protected:

        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name, uint32 maxlength );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}



#endif