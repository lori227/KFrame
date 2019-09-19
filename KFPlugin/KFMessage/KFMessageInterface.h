#ifndef __KF_MESSAGE_INTERFACE_H__
#define __KF_MESSAGE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMessageInterface : public KFModule
    {
    public:
        // 添加消息函数
        template<typename T>
        void RegisterFunction( uint32 msgid, T* object, void ( T::*handle )( const Route&, uint32, const char*, uint32 ) )
        {
            KFMessageFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddFunction( msgid, function );
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // 取消注册
        virtual void UnRegisterFunction( uint32 msgid ) = 0;

        // 调用函数
        virtual bool CallFunction( const Route& route, uint32 msgid, const char* data, uint32 length ) = 0;
    protected:
        // 添加消息函数
        virtual void AddFunction( uint32 msgid, KFMessageFunction& function ) = 0;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_message, KFMessageInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
#define __CALL_MESSAGE__( route, msgid, data, length )\
    _kf_message->CallFunction( route, msgid, data, length )

#define __REGISTER_MESSAGE__( msgid, function ) \
    _kf_message->RegisterFunction( msgid, this, function )

#define __UN_MESSAGE__( msgid ) \
    _kf_message->UnRegisterFunction( msgid )
}



#endif