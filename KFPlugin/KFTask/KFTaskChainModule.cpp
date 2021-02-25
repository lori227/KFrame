#include "KFTaskChainModule.hpp"

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
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
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
        if ( !element_result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element_result->_element->_data_name );
            return false;
        }

        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element_result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element_result->_element->_data_name );
            return false;
        }

        auto chain_index = element_object->CalcValue( element_result->_element->_data_setting, __STRING__( index ) );
        OpenTaskChain( player, element_object->_config_id, chain_index, 0u );
        return true;
    }

    bool KFTaskChainModule::OpenTaskChain( EntityPtr player, uint32 chain_id, uint32 chain_index, uint32 valid_time )
    {
        auto task_chain_setting = KFTaskChainConfig::Instance()->FindSetting( chain_id );
        if ( task_chain_setting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskChainSettingNotExist, chain_id );
            __LOG_ERROR__( "task chain=[{}] can't find setting", chain_id );
            return false;
        }

        if ( chain_index >= task_chain_setting->_task_chain_data.size() )
        {
            __LOG_ERROR__( "task chain=[{}] order=[{}] is error", chain_id, chain_index );
            return false;
        }

        auto task_chain_data = &task_chain_setting->_task_chain_data[chain_index];
        return OpenTaskChainLogicData( player, task_chain_data, chain_id, chain_index, valid_time );
    }

    bool KFTaskChainModule::OpenTaskChainLogicData( EntityPtr player, const TaskChainData* task_chain_data, uint32 chain_id, uint32 chain_index, uint32 valid_time )
    {
        auto task_id = task_chain_data->_task;
        if ( task_chain_data->_type == KFTaskChainEnum::ChainTypePool )
        {
            auto weight_setting = KFWeightConfig::Instance()->FindSetting( task_chain_data->_task );
            if ( weight_setting == nullptr )
            {
                __LOG_ERROR__( "task chain=[{}] order=[{}] can't rand pool=[{}]", chain_id, chain_index, task_chain_data->_task );
                return false;
            }

            auto rand_data = weight_setting->_weight.Rand();
            if ( rand_data == nullptr )
            {
                __LOG_ERROR__( "task chain=[{}] order=[{}] weight pool=[{}] empty", chain_id, chain_index, task_chain_data->_task );
                return false;
            }

            task_id = rand_data->_value;
        }

        // 开启任务
        auto task_data = _kf_task->OpenTask( player, task_id, task_chain_data->_task_status, valid_time, chain_id, chain_index );
        return task_data != nullptr;
    }

    void KFTaskChainModule::CleanTaskChain( EntityPtr player, uint32 chain_id )
    {
        auto task_record = player->Find( __STRING__( task ) );

        std::list<DataPtr> task_list;
        task_record->Find( __STRING__( chain ), chain_id, task_list, true );

        for ( auto task_data : task_list )
        {
            auto task_id = task_data->Get<uint32>( __STRING__( id ) );
            player->RemoveRecord( task_record, task_id );
        }
    }

    void KFTaskChainModule::OpenExtendChain( EntityPtr player, uint32 chain_id, const TaskChainData* task_chain_data )
    {
        if ( task_chain_data->_extend_chain == 0u )
        {
            return;
        }

        auto extend_chain_id = task_chain_data->_extend_chain;
        if ( task_chain_data->_extend_type == KFTaskChainEnum::ChainTypePool )
        {
            auto weight_setting = KFWeightConfig::Instance()->FindSetting( task_chain_data->_extend_chain );
            if ( weight_setting == nullptr )
            {
                return __LOG_ERROR__( "task chain=[{}] order=[{}] can't rand pool=[{}]", chain_id, task_chain_data->_index, task_chain_data->_extend_chain );
            }

            auto rand_data = weight_setting->_weight.Rand();
            if ( rand_data == nullptr )
            {
                return __LOG_ERROR__( "task chain=[{}] order=[{}] weight pool=[{}] empty", chain_id, task_chain_data->_index, task_chain_data->_extend_chain );
            }

            extend_chain_id = rand_data->_value;
        }

        OpenTaskChain( player, extend_chain_id, 0u, 0u );
    }

    bool KFTaskChainModule::IsTaskChain( EntityPtr player, DataPtr task_data )
    {
        auto task_chain_id = task_data->Get<uint32>( __STRING__( chain ) );
        return task_chain_id != 0u;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskChainModule::OnRemoveTaskTaskChainModule )
    {
        // 没有完成, 删除整个任务链
        auto status = data->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            FinishTaskChain( player, data );
        }
    }

    void KFTaskChainModule::FinishTaskChain( EntityPtr player, DataPtr kftask )
    {
        auto task_chain_id = kftask->Get<uint32>( __STRING__( chain ) );
        auto chain_index = kftask->Get<uint32>( __STRING__( index ) );
        auto valid_time = kftask->Get( __STRING__( time ) );
        if ( task_chain_id == 0u )
        {
            return;
        }

        auto task_chain_setting = KFTaskChainConfig::Instance()->FindSetting( task_chain_id );
        if ( task_chain_setting == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] task chain=[{}] can't find setting", player->GetKeyID(), task_chain_id );
        }

        if ( chain_index >= task_chain_setting->_task_chain_data.size() )
        {
            return __LOG_ERROR__( "player=[{}]  task chain=[{}] index=[{}] error", player->GetKeyID(), task_chain_id, chain_index );
        }

        // 开启额外的任务链
        OpenExtendChain( player, task_chain_id, &task_chain_setting->_task_chain_data[chain_index] );

        ++chain_index;
        if ( chain_index >= task_chain_setting->_task_chain_data.size() )
        {
            return;
        }

        OpenTaskChainLogicData( player, &task_chain_setting->_task_chain_data[chain_index], task_chain_id, chain_index, valid_time );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}