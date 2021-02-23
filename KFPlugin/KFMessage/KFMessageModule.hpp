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
        virtual bool OpenFunction( uint32 msg_id, bool open );

        // 执行函数
        virtual bool HandleMessage( const Route& route, uint32 msg_id, const char* data, uint32 length );
        //////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加消息函数
        virtual void AddMessageHandle( std::shared_ptr<KFMessageHandleAbstract>& message_handle );

        // 删除消息函数
        virtual bool RemoveMessageHandle( uint32 msg_id, KFModule* module );

        virtual void BindFindEntityFunction( uint32 type, KFModule* module, KFFindEntityFunction& function );
        virtual void UnBindFindEntityFunction( uint32 type );
    private:
        // 处理器函数
        KFHashMap<uint32, KFMessageHandleAbstract> _handle_list;

        // 查找函数
        KFMapModuleFunction<uint32, KFFindEntityFunction> _find_entity_function;
    };
}



#endif