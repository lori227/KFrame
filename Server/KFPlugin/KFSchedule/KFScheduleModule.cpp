#include "KFScheduleModule.h"
#include "KFScheduleManage.h"

namespace KFrame
{
    KFScheduleModule::KFScheduleModule()
    {
    }

    KFScheduleModule::~KFScheduleModule()
    {

    }

    void KFScheduleModule::Run()
    {
        _kf_schedule_manage->RunUpdate();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFScheduleModule::AddSchedule( const std::string& module, KFScheduleSetting* kfsetting, KFScheduleFunction& function )
    {
        _kf_schedule_manage->RegisterSchedule( module, kfsetting, function );
    }

    void KFScheduleModule::RemoveSchedule( const std::string& module )
    {
        _kf_schedule_manage->UnRegisterSchedule( module );
    }

    KFScheduleSetting* KFScheduleModule::CreateScheduleSetting()
    {
        return _kf_schedule_manage->CreateScheduleSetting();
    }
}