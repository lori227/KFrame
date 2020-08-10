#ifndef __KF_MESSAGE_MODULE_H__
#define __KF_MESSAGE_MODULE_H__

/************************************************************************
//    @Module			:    网络消息
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-8
************************************************************************/
#include "KFMessageInterface.h"

namespace KFrame
{
    class KFMessageModule : public KFMessageInterface
    {
    public:
        KFMessageModule() = default;
        ~KFMessageModule() = default;

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 执行函数
        virtual bool CallFunction( const Route& route, uint32 msgid, const char* data, uint32 length );
        //////////////////////////////////////////////////////////////////////////////////////////

        // 取消注册
        virtual void UnRegisterFunction( uint32 msgid );

        // 关闭消息处理
        virtual bool OpenFunction( uint32 msgid, bool open );
    protected:

        // 添加函数
        virtual void AddFunction( KFModule* module, uint32 msgid, KFMessageFunction& function );

    private:
        // 处理器函数
        KFBind< uint32, uint32, KFMessageFunction > _kf_message_function;
    };
}



#endif