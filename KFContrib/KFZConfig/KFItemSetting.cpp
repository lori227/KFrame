#include "KFItemSetting.hpp"
#include "KFItemTypeConfig.hpp"

namespace KFrame
{
    bool KFItemSetting::CheckCanUse() const
    {
        return _use_count > 0;
    }

    bool KFItemSetting::IsRealItem() const
    {
        switch ( _type )
        {
        case KFItemEnum::Script:
            return false;
            break;
        default:
            break;
        }

        return true;
    }

    const std::string& KFItemSetting::GetFunction( uint32 type ) const
    {
        if ( type >= KFItemEnum::MaxFunction )
        {
            return _invalid_str;
        }

        return _function[ type ];
    }

    bool KFItemSetting::IsOverlay() const
    {
        return _overlay_count > 1u;
    }
}
