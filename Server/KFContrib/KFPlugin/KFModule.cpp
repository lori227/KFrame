#include "KFModule.h"

namespace KFrame
{
    KFModule::KFModule()
    {
        _kf_plugin = nullptr;
        _max_cost_time = 0;
    }

    KFModule::~KFModule()
    {
        _kf_plugin = nullptr;
    }

    void KFModule::Initialize( KFPlugin* plugin )
    {
        _kf_plugin = plugin;
    }
}