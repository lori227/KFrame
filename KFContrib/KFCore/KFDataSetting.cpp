#include "KFDataSetting.h"

namespace KFrame
{
    KFDataSetting::KFDataSetting()
    {
        _data_mask = 0;
        _type = 0;
    }

    bool KFDataSetting::HaveFlagMask( uint32 mask ) const
    {
        return KFUtility::HaveBitMask( _data_mask, mask );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
}