#include "KFTaskConfig.hpp"

namespace KFrame
{
    bool KFTaskConfig::LoadConfig()
    {
        _task_types.Clear();
        _task_setting.Clear();

        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto taskid = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _task_setting.Create( taskid );

            kfsetting->_id = taskid;
            kfsetting->_parent_name = xmlnode.GetString( "ParentName" );
            kfsetting->_data_name = xmlnode.GetString( "DataName" );
            kfsetting->_data_key = xmlnode.GetUInt32( "DataKey" );
            kfsetting->_level = xmlnode.GetUInt32( "Level" );
            kfsetting->_need_receive = xmlnode.GetBoolen( "Receive" );

            auto strstarttime = xmlnode.GetString( "StartTime" );
            if ( !strstarttime.empty() )
            {
                kfsetting->_start_hour = KFUtility::SplitValue< uint32 >( strstarttime, ":" );
                kfsetting->_start_minute = KFUtility::SplitValue< uint32 >( strstarttime, ":" );
            }

            auto strendtime = xmlnode.GetString( "EndTime" );
            if ( !strendtime.empty() )
            {
                kfsetting->_start_hour = KFUtility::SplitValue< uint32 >( strendtime, ":" );
                kfsetting->_start_minute = KFUtility::SplitValue< uint32 >( strendtime, ":" );
            }

            kfsetting->_handle_type = xmlnode.GetUInt32( "HandleType" );

            kfsetting->_trigger_type = xmlnode.GetUInt32( "TriggerType" );
            kfsetting->_trigger_value = xmlnode.GetUInt32( "TriggerValue" );
            kfsetting->_operate = xmlnode.GetUInt32( "OperateType" );
            kfsetting->_use_type = xmlnode.GetUInt32( "UseType" );
            kfsetting->_use_value = xmlnode.GetUInt32( "UseValue" );
            kfsetting->_done_type = xmlnode.GetUInt32( "DoneType" );
            kfsetting->_done_value = xmlnode.GetUInt32( "DoneValue" );

            auto strrewards = xmlnode.GetString( "Rewards" );
            kfsetting->_rewards.Parse( strrewards, __FUNC_LINE__ );

            kfsetting->_next_id = xmlnode.GetUInt32( "NextId" );
            kfsetting->_next_value = xmlnode.GetUInt32( "NextValue" );

            AddTaskType( kfsetting );
            xmlnode.NextNode();
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTaskSetting::CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const
    {
        if ( level < _level )
        {
            return false;
        }

        if ( _data_key != 0 && _data_key != key )
        {
            return false;
        }

        if ( _trigger_type != 0 && _trigger_type != operate )
        {
            return false;
        }

        // 判断是否在时间范围内
        if ( _start_hour != 0 )
        {
            if ( !KFDate::CheckPassTime( 0, 0, 0, _start_hour, _start_minute ) )
            {
                return false;
            }
        }

        if ( _end_hour != 0 )
        {
            if ( KFDate::CheckPassTime( 0, 0, 0, _end_hour, _end_minute ) )
            {
                return false;
            }
        }

        return true;
    }

    uint32 KFTaskSetting::CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const
    {
        if ( _use_type == KFEnum::UseFinal )
        {
            operatevalue = nowvalue;
        }

        if ( operatevalue < _trigger_value )
        {
            return 0;
        }

        return operatevalue;
    }

    uint32 KFTaskSetting::CalcUseValue( uint32 operatevalue ) const
    {
        if ( _use_value == 0 )
        {
            return operatevalue;
        }

        return _use_value;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskConfig::AddTaskType( KFTaskSetting* kfsetting )
    {
        if ( kfsetting->_parent_name.empty() || kfsetting->_data_name.empty() )
        {
            return;
        }

        auto key = TaskTypeKey( kfsetting->_parent_name, kfsetting->_data_name );
        auto kftypes = _task_types.Create( key );
        kftypes->AddTaskType( kfsetting );
    }

    const KFTaskSetting* KFTaskConfig::FindTaskSetting( uint32 taskid ) const
    {
        return _task_setting.Find( taskid );
    }

    const KFTaskType* KFTaskConfig::FindTypeTaskList( const std::string& parentname, const std::string& dataname ) const
    {
        auto key = TaskTypeKey( parentname, dataname );
        return _task_types.Find( key );
    }

}

