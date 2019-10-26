#include "KFUnlockConfig.hpp"

namespace KFrame
{
    void KFUnlockConfig::ReadSetting( KFNode& xmlnode, KFUnlockSetting* kfsetting )
    {
        auto strcondition = xmlnode.GetString( "UnLockCondition", true );
        kfsetting->_conditions.Parse( strcondition, kfsetting->_id, __FUNC_LINE__ );

        kfsetting->_data_name = xmlnode.GetString( "DataName" );
        kfsetting->_data_id = xmlnode.GetUInt32( "DataKey" );
        kfsetting->_data_value = xmlnode.GetUInt32( "DataValue" );
    }
}