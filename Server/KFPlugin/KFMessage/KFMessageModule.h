#ifndef __KF_MESSAGE_MODULE_H__
#define __KF_MESSAGE_MODULE_H__

/************************************************************************
//    @Module			:    网络消息
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-8
************************************************************************/

#include "KFrame.h"
#include "KFMessageInterface.h"

namespace KFrame
{
    class KFMessageModule : public KFMessageInterface
    {
    public:
        KFMessageModule();
        ~KFMessageModule();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 执行函数
        virtual bool CallFunction( const KFGuid& guid, uint32 msgid, const char* data, uint32 length );
        //////////////////////////////////////////////////////////////////////////////////////////

        // 取消注册
        virtual void UnRegisterFunction( uint32 msgid );

    protected:

        // 添加函数
        virtual void AddFunction( uint32 msgid, KFMessageFunction& function );

    private:
        // 处理器函数
        KFBind< uint32, uint32, KFMessageFunction > _kf_message_function;
    };
}



#endif