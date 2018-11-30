#include "KFSteamModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFSteamModule::BeforeRun()
    {
        AddSteamFreeWeekendSchedule();
    }

    void KFSteamModule::AddSteamFreeWeekendSchedule()
    {
        auto steamfreeweekendstopday = _kf_option->GetUInt32( __KF_STRING__( steamfreeweekendstopday ) );
        auto steamfreeweekendstophour = _kf_option->GetUInt32( __KF_STRING__( steamfreeweekendstophour ) );

        auto kfschedulesetting = _kf_schedule->CreateScheduleSetting();
        kfschedulesetting->SetDayOfWeek( KFScheduleEnum::Loop, 0, steamfreeweekendstophour, steamfreeweekendstopday );

        __REGISTER_SCHEDULE_FUNCTION__( kfschedulesetting, &KFSteamModule::OnScheduleKickFreeWeekendPlayer );
    }

    __KF_SCHEDULE_FUNCTION__( KFSteamModule::OnScheduleKickFreeWeekendPlayer )
    {
        KickFreeWeekendPlayer();
    }

    void KFSteamModule::KickFreeWeekendPlayer()
    {
        auto kfcomponent = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        for ( auto kfentity = kfcomponent->FirstEntity(); kfentity != nullptr; kfentity = kfcomponent->NextEntity() )
        {
            auto entitydata = kfentity->GetData();
            auto steamfreeweekend = entitydata->GetValue<uint32>( __KF_STRING__( steamfreeweekend ) );
            if ( steamfreeweekend == 1 )
            {
                _kf_player->KickPlayer( kfentity->GetKeyID(), KFMsg::PlatformBeKick, __FUNC_LINE__ );
            }
        }
    }
}
