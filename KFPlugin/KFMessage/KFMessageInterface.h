#ifndef __KF_MESSAGE_INTERFACE_H__
#define __KF_MESSAGE_INTERFACE_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 消息处理抽象
    class KFMessageHandleAbstract
    {
    public:
        KFMessageHandleAbstract( uint32 msgid )
        {
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
        virtual void CallFunction( const Route& route, const char* data, uint32 length ) = 0;

    public:
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
        KFMessageHandleData( uint32 msgid )
            : KFMessageHandleAbstract( msgid )
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
        virtual void CallFunction( const Route& route, const char* data, uint32 length )
        {
            auto ok = KFProto::Parse( _message, data, length );
            if ( ok )
            {
                _function.Call( route, _msgid, reinterpret_cast<T*>( _message ) );
            }
        }

    public:
        // 处理调用函数
        typedef std::function<void( const Route&, uint32, const typename T* )> HandleFunctionType;
        KFFunction< HandleFunctionType > _function;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMessageInterface : public KFModule
    {
    public:
        // 添加消息函数
        template<typename ModuleType, typename MessageType >
        void RegisterHandle( uint32 msgid, ModuleType* module, void( ModuleType::* function )( const Route&, uint32, const typename MessageType* ) )
        {
            auto messagehandle = __KF_NEW__( KFMessageHandleData< MessageType >, msgid );
            KFMessageHandleData< MessageType >::HandleFunctionType handlefunction = std::bind( function, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
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
        virtual bool RemoveMessageHandle( uint32 msgide, KFModule* module ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_message, KFMessageInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 消息函数
#define __KF_MESSAGE_FUNCTION__( function, msgtype ) \
    void function( const Route& route, uint32 msgid, const msgtype* kfmsg )

#define __HANDLE_MESSAGE__( route, msgid, data, length )\
    _kf_message->HandleMessage( route, msgid, data, length )

#define __REGISTER_MESSAGE__( moduletype, msgid, msgtype, function ) \
    _kf_message->RegisterHandle<moduletype, msgtype>( msgid, this, &moduletype::function )

#define __UN_MESSAGE__( msgid ) \
    _kf_message->UnRegisterHandle( msgid, this )

    ///////////////////////////////////////////////////////////////////////////////////////////
#define __KF_TRANSPOND_MESSAGE_FUNCTION__( function ) \
    bool function( const Route& route, uint32 msgid, const char* data, uint32 length )
}



#endif