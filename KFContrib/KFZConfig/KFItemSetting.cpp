#include "KFItemSetting.hpp"

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

    bool KFItemSetting::IsCanStore( const std::string& name ) const
    {
        if ( !_store_list.empty() )
        {
            auto iter = _store_list.find( name );
            if ( iter == _store_list.end() )
            {
                return false;
            }
        }

        return true;
    }
}
