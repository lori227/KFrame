#include "KFTaskChainModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    namespace KFTaskChainEnum
    {
        enum MyEnum
        {
            ChainTypeTask = 1,		// 任务
            ChainTypeChain = 2,		// 任务链
            ChainTypePool = 3,		// 任务池
        };
    }

    void KFTaskChainModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( taskchain ), &KFTaskChainModule::AddTaskChainElement );
        __REGISTER_REMOVE_DATA_1__( __STRING__( task ), &KFTaskChainModule::OnRemoveTaskTaskChainModule );
    }

    void KFTaskChainModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __STRING__( taskchain ) );
        __UN_REMOVE_DATA_1__( __STRING__( task ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFTaskChainModule::AddTaskChainElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto chainindex = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( index ) );
        OpenTaskChain( player, kfelementobject->_config_id, chainindex, 0u );
        return true;
    }

    bool KFTaskChainModule::OpenTaskChain( KFEntity* player, uint32 chainid, uint32 chainindex, uint32 validtime )
    {
        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( chainid );
        if ( kftaskchainsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskChainSettingNotExist, chainid );
            __LOG_ERROR__( "taskchain=[{}] can't find setting", chainid );
            return false;
        }

        if ( chainindex >= kftaskchainsetting->_task_chain_data.size() )
        {
            __LOG_ERROR__( "taskchain=[{}] order=[{}] is error", chainid, chainindex );
            return false;
        }

        auto taskchaindata = &kftaskchainsetting->_task_chain_data[chainindex];
        return OpenTaskChainLogicData( player, taskchaindata, chainid, chainindex, validtime );
    }

    bool KFTaskChainModule::OpenTaskChainLogicData( KFEntity* player, const TaskChainData* taskchaindata, uint32 chainid, uint32 chainindex, uint32 validtime )
    {
        auto taskid = taskchaindata->_task;
        if ( taskchaindata->_type == KFTaskChainEnum::ChainTypePool )
        {
            auto kfweightsetting = KFWeightConfig::Instance()->FindSetting( taskchaindata->_task );
            if ( kfweightsetting == nullptr )
            {
                __LOG_ERROR__( "taskchain=[{}] order=[{}] can't rand taskchain pool=[{}]", chainid, chainindex, taskchaindata->_task );
                return false;
            }

            auto randdata = kfweightsetting->_weight.Rand();
            if ( randdata == nullptr )
            {
                __LOG_ERROR__( "taskchain=[{}] order=[{}] weight pool=[{}] empty", chainid, chainindex, taskchaindata->_task );
                return false;
            }

            taskid = randdata->_value;
        }

        // 开启任务
        auto kftask = _kf_task->OpenTask( player, taskid, taskchaindata->_task_status, validtime, chainid, chainindex );
        return kftask != nullptr;
    }

    void KFTaskChainModule::CleanTaskChain( KFEntity* player, uint32 chainid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );

        std::list< KFData* > tasklist;
        kftaskrecord->Find( __STRING__( chain ), chainid, tasklist, true );

        for ( auto kftask : tasklist )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            player->RemoveRecord( kftaskrecord, taskid );
        }
    }

    void KFTaskChainModule::OpenExtendChain( KFEntity* player, uint32 chainid, const TaskChainData* taskchaindata )
    {
        if ( taskchaindata->_extend_chain == 0u )
        {
            return;
        }

        auto chainid = taskchaindata->_extend_chain;
        if ( taskchaindata->_extend_type == KFTaskChainEnum::ChainTypePool )
        {
            auto kfweightsetting = KFWeightConfig::Instance()->FindSetting( taskchaindata->_extend_chain );
            if ( kfweightsetting == nullptr )
            {
                return __LOG_ERROR__( "taskchain=[{}] order=[{}] can't rand taskchain pool=[{}]", chainid, taskchaindata->_index, taskchaindata->_extend_chain );
            }

            auto randdata = kfweightsetting->_weight.Rand();
            if ( randdata == nullptr )
            {
                return __LOG_ERROR__( "taskchain=[{}] order=[{}] weight pool=[{}] empty", chainid, taskchaindata->_index, taskchaindata->_extend_chain );
            }

            chainid = randdata->_value;
        }

        OpenTaskChain( player, chainid, 0u, 0u );
    }

    bool KFTaskChainModule::IsTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        return taskchainid != 0u;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskChainModule::OnRemoveTaskTaskChainModule )
    {
        // 没有完成, 删除整个任务链
        auto status = kfdata->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            FinishTaskChain( player, kfdata );
        }
    }

    void KFTaskChainModule::FinishTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto chainindex = kftask->Get<uint32>( __STRING__( index ) );
        auto validtime = kftask->Get( __STRING__( time ) );
        if ( taskchainid == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] taskchain=[{}] can't find setting", player->GetKeyID(), taskchainid );
        }

        if ( chainindex >= kftaskchainsetting->_task_chain_data.size() )
        {
            return __LOG_ERROR__( "player=[{}]  taskchain=[{}] index=[{}] error", player->GetKeyID(), taskchainid, chainindex );
        }

        // 开启额外的任务链
        OpenExtendChain( player, taskchainid, &kftaskchainsetting->_task_chain_data[chainindex] );

        ++chainindex;
        if ( chainindex >= kftaskchainsetting->_task_chain_data.size() )
        {
            return;
        }

        OpenTaskChainLogicData( player, &kftaskchainsetting->_task_chain_data[chainindex], taskchainid, chainindex, validtime );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}