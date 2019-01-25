#include "KFModule.h"

namespace KFrame
{
    KFModule::KFModule()
    {
        _kf_plugin = nullptr;
    }

    KFModule::~KFModule()
    {
        _kf_plugin = nullptr;
    }
}