#include "KFCompoundConfig.hpp"

namespace KFrame
{
    void KFCompoundConfig::ReadSetting( KFNode& xmlnode, KFCompoundSetting* kfsetting )
    {
        kfsetting->_key = xmlnode.GetUInt32( "DataKey" );
        kfsetting->_data_name = xmlnode.GetString( "DataName" );

        auto strcostdata = xmlnode.GetString( "CostData" );
        kfsetting->_cost_data.Parse( strcostdata, __FUNC_LINE__ );

        auto strcompounddata = xmlnode.GetString( "CompoundData" );
        kfsetting->_compound_data.Parse( strcompounddata, __FUNC_LINE__ );
    }
}