#include "KFDropGroupConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropGroupConfig::ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting )
    {
        kfsetting->_is_drop_count = xmlnode.GetBoolen( "DropCount", true );
        if ( kfsetting->_condition_type == 0u )
        {
            kfsetting->_condition_type = xmlnode.GetUInt32( "ConditionType" );
        }
        if ( kfsetting->_rand_type == 0u )
        {
            kfsetting->_rand_type = xmlnode.GetUInt32( "RandType" );
        }

        KFDropGroupWeight* kfdropweight = nullptr;
        auto dropdataid = xmlnode.GetUInt32( "DropDataId", true );
        auto weight = xmlnode.GetUInt32( "Weight", true );

        if ( weight == KFRandEnum::TenThousand && kfsetting->_rand_type == KFRandEnum::Probability )
        {
            kfdropweight = __KF_NEW__( KFDropGroupWeight );
            kfdropweight->_id = dropdataid;
            kfsetting->_necessary_list.Add( kfdropweight );
        }
        else
        {
            kfdropweight = kfsetting->_rand_list.Create( dropdataid, weight );
        }

        kfdropweight->_is_clear_var = xmlnode.GetBoolen( "Reset", true );

        // 条件
        auto strcondition = xmlnode.GetString( "Condition", true );
        kfdropweight->_conditions.Parse( strcondition, kfsetting->_id, __FUNC_LINE__ );
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
        dropgroupweight->_drop_data_setting = KFDropDataConfig::Instance()->FindSetting( dropgroupweight->_id );
        if ( dropgroupweight->_drop_data_setting == nullptr )
        {
            if ( dropgroupweight->_id != 0u )
            {
                __LOG_ERROR__( "dropid=[{}] dropdata=[{}] can't find setting", kfsetting->_id, dropgroupweight->_id );
            }
        }
    }
}