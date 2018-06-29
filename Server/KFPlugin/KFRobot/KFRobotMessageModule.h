#ifndef __KF__ROBOT_MESSAGE_MODULE_H__
#define __KF__ROBOT_MESSAGE_MODULE_H__

/************************************************************************
//    @Module			:    网络消息
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-8
************************************************************************/

#include "KFrame.h"


namespace KFrame
{
    class KFRobotMessageModule
    {
    public:
        KFRobotMessageModule();
        ~KFRobotMessageModule();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 执行函数
        bool CallFunction( const KFGuid& guid, uint32 msgid, const char* data, uint32 length );
        //////////////////////////////////////////////////////////////////////////////////////////

        // 取消注册
        void UnRegisterFunction( uint32 msgid );

    public:

        // 添加函数
        void AddFunction( uint32 msgid, KFMessageFunction& function );

    private:
        // 处理器函数
        KFBind< uint32, uint32, KFMessageFunction > _kf_message_function;
    };

    template<typename T>
    void RegisterFunction( KFRobotMessageModule* self, uint32 msgid, T* object, void ( T::*handle )( const KFGuid& guid, const char* data, uint32 length ) )
    {
        KFMessageFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
        self->AddFunction( msgid, function );
    }

#define __REGISTER_ROBOT_MESSAGE__( self, msgid, function) \
    RegisterFunction( self, msgid, this, function );
}


#endif