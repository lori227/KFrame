#include "KFProjectModule.hpp"

namespace KFrame
{
    void KFProjectModule::InitModule()
    {
        KFGlobal::Instance()->RegisterParseToElementStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::ParseString );
        KFGlobal::Instance()->RegisterIntStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatIntString );
        KFGlobal::Instance()->RegisterStrStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatStrString );
        KFGlobal::Instance()->RegisterDataToElementStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatDatatring );
    }

    void KFProjectModule::BeforeRun()
    {
        auto kfglobal = KFGlobal::Instance();

    }
}