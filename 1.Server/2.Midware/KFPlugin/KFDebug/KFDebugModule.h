#ifndef __KF_DEBUG_MOUDLE_H__
#define __KF_DEBUG_MOUDLE_H__

/************************************************************************
//    @Module			:    调试系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-20
************************************************************************/

#include "KFrame.h"
#include "KFDebugInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{

    class KFDebugModule : public KFDebugInterface
    {
    public:
        KFDebugModule();
        ~KFDebugModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 卸载调试函数
        virtual void UnRegisterDebugFunction( const std::string& command );

    protected:
        // 绑定调试函数
        virtual void BindDebugFunction( const std::string& command, KFDebugFunction& function );

    protected:
        // 处理客户端命令请求
        __KF_MESSAGE_FUNCTION__( HandleDebugCommandReq );

    protected:
        KFBind< std::string, const std::string&, KFDebugFunction > _kf_debug_function;
    };
}



#endif