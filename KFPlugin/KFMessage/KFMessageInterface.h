#ifndef __KF_MESSAGE_INTERFACE_H__
#define __KF_MESSAGE_INTERFACE_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    typedef std::function<KFEntity*( uint64 )> KFFindEntityFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 消息处理抽象
    class KFMessageHandleAbstract
    {
    public:
        KFMessageHandleAbstract( uint32 type, uint32 msgid )
        {
            _type = type;
            _msgid = msgid;
        }

        virtual ~KFMessageHandleAbstract()
        {
            __KF_DELETE__( google::protobuf::Message, _message );
        }

        // 获取模块id
        virtual KFModule* GetModule() = 0;

        // 设置开关
        virtual void OpenFunction( bool open ) = 0;

        // 调用函数
        virtual void CallFunction( KFEntity* kfentity, const Route& route, const char* data, uint32 length ) = 0;

    public:
        // 消息处理类型
        uint32 _type = KFMessageEnum::Normal;

        // 消息id
        uint32 _msgid = 0u;

        // 消息指针
        google::protobuf::Message* _message = nullptr;
    };

    // 消息处理模板
    template< typename T >
    class KFMessageHandleData : public KFMessageHandleAbstract
    {
    public:
        KFMessageHandleData( uint32 type, uint32 msgid )
            : KFMessageHandleAbstract( type, msgid )
        {
            _message = __KF_NEW__( T );
        }

        // 获取模块id
        virtual KFModule* GetModule()
        {
            return _function._module;
        }

        // 设置开关
        virtual void OpenFunction( bool open )
        {
            _function.SetOpen( open );
        }

        // 调用函数
        virtual void CallFunction( KFEntity* kfentity, const Route& route, const char* data, uint32 length )
        {
            auto ok = KFProto::Parse( _message, data, length );
            if ( ok )
            {
                _function.Call( kfentity, route, _msgid, reinterpret_cast<T*>( _message ) );
            }
        }

    public:
        // 处理调用函数
        typedef std::function<void( KFEntity*, const Route&, uint32, const T* )> HandleFunctionType;
        KFFunction< HandleFunctionType > _function;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMessageInterface : public KFModule
    {
    public:
        // 添加消息函数
        template<typename ModuleType, typename MessageType >
        void RegisterHandle( uint32 type, uint32 msgid, ModuleType* module, void( ModuleType::* function )( KFEntity*, const Route&, uint32, const MessageType* ) )
        {
            auto messagehandle = __KF_NEW__( KFMessageHandleData< MessageType >, type, msgid );
            typename KFMessageHandleData< MessageType >::HandleFunctionType handlefunction = std::bind( function, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            messagehandle->_function.SetFunction( module, handlefunction );
            AddMessageHandle( messagehandle );
        }

        // 取消注册
        template<typename ModuleType>
        void UnRegisterHandle( uint32 msgid, ModuleType* module )
        {
            RemoveMessageHandle( msgid, module );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 为了减少game逻辑, 提供一个查找玩家的回调函数
        template< typename T >
        void RegisterFindEntityFunction( uint32 type, T* module, KFEntity * ( T::* function )( uint64 ) )
        {
            KFFindEntityFunction bindfunction = std::bind( function, module, std::placeholders::_1 );
            BindFindEntityFunction( type, module, bindfunction );
        }

        template< typename T >
        void RegisterFindEntityFunction( uint32 type, T* module )
        {
            UnBindFindEntityFunction( type );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 关闭消息处理
        virtual bool OpenFunction( uint32 msgid, bool open ) = 0;

        // 调用函数
        virtual bool HandleMessage( const Route& route, uint32 msgid, const char* data, uint32 length ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加消息函数
        virtual void AddMessageHandle( KFMessageHandleAbstract* messagehandle ) = 0;

        // 删除消息函数
        virtual bool RemoveMessageHandle( uint32 msgid, KFModule* module ) = 0;

        virtual void BindFindEntityFunction( uint32 type, KFModule* module, KFFindEntityFunction& function ) = 0;
        virtual void UnBindFindEntityFunction( uint32 type ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_message, KFMessageInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 消息函数
#define __KF_MESSAGE_FUNCTION__( function, msgtype ) \
    void function( KFEntity* kfentity, const Route& route, uint32 msgid, const msgtype* kfmsg )

#define __REGISTER_MESSAGE__( msgflag, msgid, function ) \
    _kf_message->RegisterHandle( msgflag, msgid, this, function )

#define __HANDLE_MESSAGE__( route, msgid, data, length )\
    _kf_message->HandleMessage( route, msgid, data, length )

#define __UN_MESSAGE__( msgid ) \
    _kf_message->UnRegisterHandle( msgid, this )

    ///////////////////////////////////////////////////////////////////////////////////////////
#define __KF_TRANSPOND_MESSAGE_FUNCTION__( function ) \
    bool function( const Route& route, uint32 msgid, const char* data, uint32 length )
    ///////////////////////////////////////////////////////////////////////////////////////////
#define __REGISTER_FIND_ENTITY__(type, function)\
    _kf_message->RegisterFindEntityFunction( type, this, function )
#define __UN_FIND_ENGITY__( type )\
    _kf_message->RegisterFindEntityFunction( type, this )

}



#endif