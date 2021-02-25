#ifndef __KF_MESSAGE_INTERFACE_H__
#define __KF_MESSAGE_INTERFACE_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    typedef std::function<EntityPtr( uint64 )> KFFindEntityFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 消息处理抽象
    class KFMessageHandleAbstract
    {
    public:
        KFMessageHandleAbstract( uint32 type, uint32 msg_id )
        {
            _type = type;
            _msg_id = msg_id;
        }

        virtual ~KFMessageHandleAbstract() = default;

        // 设置开关
        virtual void OpenFunction( bool open ) = 0;

        // 调用函数
        virtual void CallFunction( EntityPtr entity, const Route& route, const char* data, uint32 length ) = 0;

    public:
        // 消息处理类型
        uint32 _type = KFMessageEnum::Normal;

        // 消息id
        uint32 _msg_id = 0u;

        // 消息指针
        std::shared_ptr<google::protobuf::Message> _message = nullptr;
    };

    // 消息处理模板
    template<typename T>
    class KFMessageHandleData : public KFMessageHandleAbstract
    {
    public:
        KFMessageHandleData( uint32 type, uint32 msg_id )
            : KFMessageHandleAbstract( type, msg_id )
        {
            _message = __MAKE_SHARED__( T );
        }

        // 设置开关
        virtual void OpenFunction( bool open )
        {
            _function.SetOpen( open );
        }

        // 调用函数
        virtual void CallFunction( EntityPtr entity, const Route& route, const char* data, uint32 length )
        {
            auto ok = KFProto::Parse( _message, data, length );
            if ( ok )
            {
                _function.Call( entity, route, _msg_id, std::static_pointer_cast<T>( _message ) );
            }
        }

    public:
        // 处理调用函数
        typedef std::function<void( EntityPtr, const Route&, uint32, std::shared_ptr<const T> )> HandleFunctionType;
        KFModuleFunction<HandleFunctionType> _function;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMessageInterface : public KFModule
    {
    public:
        // 添加消息函数
        template<typename ModuleType, typename MessageType >
        void RegisterHandle( uint32 type, uint32 msg_id, ModuleType* module, void( ModuleType::* function )( EntityPtr, const Route&, uint32, std::shared_ptr<const MessageType> ) )
        {
            auto message_handle = __MAKE_SHARED__( KFMessageHandleData<MessageType>, type, msg_id );
            typename KFMessageHandleData<MessageType>::HandleFunctionType handle_function =
                std::bind( function, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            message_handle->_function.SetFunction( module, handle_function );
            AddMessageHandle( message_handle );
        }

        // 取消注册
        template<typename ModuleType>
        void UnRegisterHandle( uint32 msg_id, ModuleType* module )
        {
            RemoveMessageHandle( msg_id );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 为了减少game逻辑, 提供一个查找玩家的回调函数
        template<typename T>
        void RegisterFindEntityFunction( uint32 type, T* module, EntityPtr ( T::* function )( uint64 ) )
        {
            KFFindEntityFunction bind_function = std::bind( function, module, std::placeholders::_1 );
            BindFindEntityFunction( type, module, bind_function );
        }

        template<typename T>
        void UnRegisterFindEntityFunction( uint32 type, T* module )
        {
            UnBindFindEntityFunction( type );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 关闭消息处理
        virtual bool OpenFunction( uint32 msg_id, bool open ) = 0;

        // 调用函数
        virtual bool HandleMessage( const Route& route, uint32 msg_id, const char* data, uint32 length ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加消息函数
        virtual void AddMessageHandle( std::shared_ptr<KFMessageHandleAbstract> message_handle ) = 0;

        // 删除消息函数
        virtual bool RemoveMessageHandle( uint32 msg_id ) = 0;

        virtual void BindFindEntityFunction( uint32 type, KFModule* module, KFFindEntityFunction& function ) = 0;
        virtual void UnBindFindEntityFunction( uint32 type ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_message, KFMessageInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 消息函数
#define __KF_MESSAGE_FUNCTION__( function, msg_type ) \
    void function( EntityPtr entity, const Route& route, uint32 msg_id, std::shared_ptr<const msg_type> kfmsg )

#define __REGISTER_MESSAGE__( msg_flag, msg_id, function ) \
    _kf_message->RegisterHandle( msg_flag, msg_id, this, function )

#define __HANDLE_MESSAGE__( route, msg_id, data, length )\
    _kf_message->HandleMessage( route, msg_id, data, length )

#define __UN_MESSAGE__( msg_id ) \
    _kf_message->UnRegisterHandle( msg_id, this )

    ///////////////////////////////////////////////////////////////////////////////////////////
#define __KF_FORWARD_MESSAGE_FUNCTION__( function ) \
    bool function( const Route& route, uint32 msg_id, const char* data, uint32 length )
    ///////////////////////////////////////////////////////////////////////////////////////////
#define __REGISTER_FIND_ENTITY__(type, function)\
    _kf_message->RegisterFindEntityFunction( type, this, function )
#define __UN_FIND_ENTITY__( type )\
    _kf_message->UnRegisterFindEntityFunction( type, this )

}



#endif