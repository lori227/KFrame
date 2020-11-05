#include "KFUnlockConfig.hpp"
#include "KFXmlReader/KFReadSetting.h"

namespace KFrame
{
    void KFUnlockConfig::ReadSetting( KFNode& xmlnode, KFUnlockSetting* kfsetting )
    {
        kfsetting->_data_name = xmlnode.GetString( "DataName" );
        kfsetting->_data_id = xmlnode.GetUInt32( "DataKey" );
        kfsetting->_data_value = xmlnode.GetUInt32( "DataValue" );
        kfsetting->_child_name = xmlnode.GetString( "ChildName" );

        auto strunlockcondition = xmlnode.GetString( "UnLockCondition" );
        kfsetting->_condition_type = KFReadSetting::ParseConditionList( strunlockcondition, kfsetting->_unlock_condition );
    }
}