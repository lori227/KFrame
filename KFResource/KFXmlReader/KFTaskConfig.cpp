#include "KFTaskConfig.hpp"
#include "KFXmlReader/KFReadSetting.h"

namespace KFrame
{
    void KFTaskConfig::ReadSetting( KFXmlNode& xmlnode, KFTaskSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_complete_type = xmlnode.GetUInt32( "CompleteMode" );

        auto strprecondition = xmlnode.GetString( "PreCondition" );
        kfsetting->_pre_condition_type = KFReadSetting::ParseConditionList( strprecondition, kfsetting->_pre_condition );

        auto strcompletecondition = xmlnode.GetString( "CompleteCondition" );
        kfsetting->_complete_condition_type = KFReadSetting::ParseConditionList( strcompletecondition, kfsetting->_complete_condition );

        kfsetting->_execute_list = xmlnode.GetUInt32Vector( "Execute" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}