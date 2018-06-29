#include "KFPlayerConfig.h"

namespace KFrame
{
    KFPlayerConfig::KFPlayerConfig()
    {
        _daily_refresh_hour = 0;
        _day_of_week = 1;
        _day_of_month = 1;
    }

    KFPlayerConfig::~KFPlayerConfig()
    {

    }

    bool KFPlayerConfig::LoadConfig( const char* file )
    {
        try
        {

        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}