#ifndef __KF_TASK_CHAIN_INTERFACE_H__
#define __KF_TASK_CHAIN_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< bool( KFEntity*, uint32, uint32, uint32, uint32, uint32, const char*, uint32 ) > KFOpenTaskChainFunction;
    typedef std::function< void( KFEntity*, const UInt32List&, uint32 ) > KFFinishTaskChainFunction;
    class KFTaskChainInterface : public KFModule
    {
    public:
        // 注册开启任务链功能函数
        template< class T >
        void RegisterOpenTaskChainFunction( const std::string& name, T* object, bool( T::*handle )( KFEntity*, uint32, uint32, uint32, uint32, uint32, const char*, uint32 ) )
        {
            KFOpenTaskChainFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                               std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8 );
            BindOpenTaskChainFunction( name, function );
        }

        void UnRegisterOpenTaskChainFunction( const std::string& name )
        {
            UnBindOpenTaskChainFunction( name );
        }

        // 注册完成任务链功能函数
        template< class T >
        void RegisterFinishTaskChainFunction( const std::string& name, T* object, void( T::*handle )( KFEntity*, const UInt32List&, uint32 ) )
        {
            KFFinishTaskChainFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindFinishTaskChainFunction( name, function );
        }

        void UnRegisterFinishTaskChainFunction( const std::string& name )
        {
            UnBindFinishTaskChainFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 完成任务链任务
        virtual void FinishTaskChain( KFEntity* player, KFData* kftask, const char* function, uint32 line ) = 0;

        // 删除任务链任务
        virtual void RemoveTaskChain( KFEntity* player, KFData* kftask ) = 0;
    private:
        virtual void BindOpenTaskChainFunction( const std::string& name, KFOpenTaskChainFunction& function ) = 0;
        virtual void UnBindOpenTaskChainFunction( const std::string& name ) = 0;

        virtual void BindFinishTaskChainFunction( const std::string& name, KFFinishTaskChainFunction& function ) = 0;
        virtual void UnBindFinishTaskChainFunction( const std::string& name ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_task_chain, KFTaskChainInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_OPEN_TASK_CHAIN_FUNCTION__( _function ) \
    bool _function( KFEntity* player, uint32 taskchainid, uint32 order, uint32 taskid, uint32 logicid, uint32 statusid, const char* function, uint32 line )
#define  __REGISTER__OPEN_TASK_CHAIN__( name, function ) \
    _kf_task_chain->RegisterOpenTaskChainFunction( name, this, function )
#define  __UN_OPEN_TASK_CHAIN__( name ) \
    _kf_task_chain->UnRegisterOpenTaskChainFunction( name )

#define __KF_FINISH_TASK_CHAIN_FUNCTION__( _function ) \
    void _function( KFEntity* player, const UInt32List& logicids, uint32 statusid )
#define  __REGISTER_FINISH_TASK_CHAIN__( name, function ) \
    _kf_task_chain->RegisterFinishTaskChainFunction( name, this, function )
#define  __UN_FINISH_TASK_CHAIN__( name ) \
    _kf_task_chain->UnRegisterFinishTaskChainFunction( name )
}

#endif