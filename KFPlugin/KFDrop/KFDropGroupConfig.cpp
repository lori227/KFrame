#include "KFDropGroupConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropGroupConfig::ReadSetting( KFXmlNode& xmlnode, KFDropSetting* kfsetting )
    {
        kfsetting->_is_drop_count = xmlnode.ReadBoolen( "dropcount", true );
        kfsetting->_is_exclude = xmlnode.ReadBoolen( "exclude", true );
        kfsetting->_condition_type = xmlnode.ReadUInt32( "conditiontype", true );
        kfsetting->_rand_type = xmlnode.ReadUInt32( "randtype", true );

        KFDropGroupWeight* kfdropweight = nullptr;
        auto weight = xmlnode.ReadUInt32( "weight", true );

        if ( weight == KFRandEnum::TenThousand )
        {
            kfdropweight = __KF_NEW__( KFDropGroupWeight );
            kfsetting->_necessary_list.Add( kfdropweight );
            kfdropweight->_value = kfsetting->_necessary_list._objects.size();
        }
        else
        {
            auto id = kfsetting->_rand_list._weight_data.size();
            kfdropweight = kfsetting->_rand_list.Create( ++id, weight );
        }

        kfdropweight->_is_clear_var = xmlnode.ReadBoolen( "reset", true );
        kfdropweight->_drop_data_id = xmlnode.ReadUInt32( "dropdataid", true );

        // 条件
        kfdropweight->_conditions = xmlnode.ReadStaticConditions( "condition", true );
    }

    void KFDropGroupConfig::LoadAllComplete()
    {
        for ( auto iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto kfdropgroupweight : kfsetting->_necessary_list._objects )
            {
                InitDropDataSetting( kfsetting, kfdropgroupweight );
            }

            for ( auto kfdropgroupweight : kfsetting->_rand_list._weight_data )
            {
                InitDropDataSetting( kfsetting, kfdropgroupweight );
            }
        }
    }

    void KFDropGroupConfig::InitDropDataSetting( KFDropSetting* kfsetting, KFDropGroupWeight* dropgroupweight )
    {
        dropgroupweight->_drop_data_setting = KFDropDataConfig::Instance()->FindSetting( dropgroupweight->_drop_data_id );
        if ( dropgroupweight->_drop_data_setting == nullptr )
        {
            if ( dropgroupweight->_drop_data_id != 0u )
            {
                __LOG_ERROR__( "dropid=[{}] dropdata=[{}] can't find setting", kfsetting->_id, dropgroupweight->_drop_data_id );
            }
        }

        // 判断掉落条件
        for ( auto kfcondition : dropgroupweight->_conditions->_condition_list )
        {
            InitDropConditonSetting( kfsetting, kfcondition->_left );
            InitDropConditonSetting( kfsetting, kfcondition->_right );
        }
    }

    void KFDropGroupConfig::InitDropConditonSetting( KFDropSetting* kfsetting, KFStaticConditionExpression* kfexpression )
    {
        for ( auto kfdata : kfexpression->_data_list )
        {
            if ( !kfdata->IsVariable() )
            {
                continue;
            }

            auto kfvariable = ( KFStaticConditionVariable* )kfdata;
            if ( kfvariable->_data_name == __STRING__( drop ) )
            {
                kfvariable->_parent_name = __STRING__( drop );
                kfvariable->_data_name = __STRING__( value );
                kfvariable->_data_id = kfsetting->_id;
            }
        }
    }
}