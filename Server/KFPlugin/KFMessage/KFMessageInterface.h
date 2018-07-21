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
        void RegisterFunction( uint32 msgid, T* object, void ( T::*handle )( const KFGuid& guid, const char* data, uint32 length ) )
        {
            KFMessageFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddFunction( msgid, function );
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // 取消注册
        virtual void UnRegisterFunction( uint32 msgid ) = 0;

        // 调用函数
        virtual bool CallFunction( const KFGuid& guid, uint32 msgid, const char* data, uint32 length ) = 0;
    protected:
        // 添加消息函数
        virtual void AddFunction( uint32 msgid, KFMessageFunction& function ) = 0;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_message, KFMessageInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////

#define __KF_MESSAGE_FUNCTION__( function ) \
    void function( const KFGuid& kfguid, const char* data, uint32 length )

#define __KF_TRANSMIT_FUNCTION__( function ) \
    bool function( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )

#define __REGISTER_MESSAGE__( msgid, function ) \
    _kf_message->RegisterFunction( msgid, this, function )

#define __UNREGISTER_MESSAGE__( msgid ) \
    _kf_message->UnRegisterFunction( msgid )
}



#endif