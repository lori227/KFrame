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
        // 关闭消息处理
        virtual bool OpenFunction( uint32 msgid, bool open );

        // 执行函数
        virtual bool HandleMessage( const Route& route, uint32 msgid, const char* data, uint32 length );
        //////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加消息函数
        virtual void AddMessageHandle( KFMessageHandleAbstract* messagehandle );

        // 删除消息函数
        virtual bool RemoveMessageHandle( uint32 msgide, KFModule* module );

        virtual void BindFindEngityFunction( KFFindEntityFunction& function );
        virtual void UnBindFindEngityFunction();
    private:
        // 处理器函数
        std::unordered_map<uint32, KFMessageHandleAbstract*> _handles;

        KFFindEntityFunction _find_entity_function = nullptr;
    };
}



#endif