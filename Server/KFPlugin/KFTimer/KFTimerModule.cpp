#include "KFTimerModule.h"

namespace KFrame
{
    KFTimerModule::KFTimerModule()
    {
    }

    KFTimerModule::~KFTimerModule()
    {

    }

    void KFTimerModule::BeforeRun()
    {
        _kf_timer_manage->Initialize();
        __REGISTER_RUN_FUNCTION__( &KFTimerModule::Run );
    }

    void KFTimerModule::ShutDown()
    {
        _kf_timer_manage->ShutDown();
        __UNREGISTER_RUN_FUNCTION__();
    }

    void KFTimerModule::Run()
    {
        _kf_timer_manage->RunUpdate();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTimerModule::AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function )
    {
        _kf_timer_manage->RegisterLoopTimer( module, objectid, intervaltime, function );
    }

    void KFTimerModule::AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function )
    {
        _kf_timer_manage->RegisterLimitTimer( module, objectid, intervaltime, count, function );
    }

    void KFTimerModule::AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function )
    {
        _kf_timer_manage->RegisterDelayTimer( module, objectid, intervaltime, function );
    }

    bool KFTimerModule::RemoveTimer( const std::string& module )
    {
        return _kf_timer_manage->UnRegisterTimer( module );
    }

    bool KFTimerModule::RemoveTimer( const std::string& module, uint64 objectid )
    {
        return _kf_timer_manage->UnRegisterTimer( module, objectid );
    }

    uint32 KFTimerModule::FindLeftTime( const std::string& module, uint64 objectid )
    {
        return _kf_timer_manage->FindLeftTime( module, objectid );
    }
}