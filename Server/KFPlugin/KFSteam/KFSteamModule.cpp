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
        auto _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        if ( _kf_component == nullptr )
        {
            __LOG_ERROR__( "can't find player component" );
            return;
        }

        for ( auto kfentity = _kf_component->FirstEntity(); kfentity != nullptr; kfentity = _kf_component->NextEntity() )
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
