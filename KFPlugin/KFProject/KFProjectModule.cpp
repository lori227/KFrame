#include "KFProjectModule.hpp"

namespace KFrame
{
    void KFProjectModule::InitModule()
    {
        KFElementConfig::Instance()->SetParseStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::ParseString );
        KFElementConfig::Instance()->SetIntStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatIntString );
        KFElementConfig::Instance()->SetStrStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatStrString );
        KFElementConfig::Instance()->SetSettingStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatSettingString );
    }

    void KFProjectModule::BeforeRun()
    {
        auto kfglobal = KFGlobal::Instance();

    }
}