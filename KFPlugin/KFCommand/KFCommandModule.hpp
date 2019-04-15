#ifndef __KF_COMMAND_MOUDLE_H__
#define __KF_COMMAND_MOUDLE_H__

/************************************************************************
//    @Module			:    调试系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-20
************************************************************************/

#include "KFCommandInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFCommandModule : public KFCommandInterface
    {
    public:
        KFCommandModule() = default;
        ~KFCommandModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 卸载调试函数
        virtual void UnRegisterFunction( const std::string& command );

    protected:
        // 绑定调试函数
        virtual void BindFunction( const std::string& command, KFCommandFunction& function );

    protected:
        // 处理客户端命令请求
        __KF_MESSAGE_FUNCTION__( HandleCommandReq );

    protected:
        KFBind< std::string, const std::string&, KFCommandFunction > _kf_command_function;
    };
}



#endif