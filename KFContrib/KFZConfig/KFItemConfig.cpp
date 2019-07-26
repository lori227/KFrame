#include "KFItemConfig.hpp"

namespace KFrame
{
    void KFItemConfig::ReadSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        switch ( kfsetting->_type )
        {
        case KFItemEnum::GiftBag:
            ReadGiftSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Medicine:
            ReadMedicineSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Equip:
            ReadEquipSetting( xmlnode, kfsetting );
            break;
        default:
            ReadCommonSetting( xmlnode, kfsetting );
            break;
        }
    }

    void KFItemConfig::ReadCommonSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_quality = xmlnode.GetUInt32( "Quality" );
        kfsetting->_use_count = xmlnode.GetUInt32( "UseCount", true );
        kfsetting->_overlay_type = xmlnode.GetUInt32( "OverlayType" );
        kfsetting->_overlay_count = xmlnode.GetUInt32( "OverlayCount" );

        kfsetting->_use_limit = KFItemEnum::UseInAll;
        auto struselimit = xmlnode.GetString( "UseLimit", true );
        while ( !struselimit.empty() )
        {
            auto usemask = KFUtility::SplitValue< uint32 >( struselimit, DEFAULT_SPLIT_STRING );
            KFUtility::AddBitMask( kfsetting->_use_limit, usemask );
        }

        auto strsell = xmlnode.GetString( "Sell", true );
        kfsetting->_sell_elements.Parse( strsell, __FUNC_LINE__ );

        kfsetting->_lua_file = xmlnode.GetString( "LuaFile", true );
        auto addfunction = xmlnode.GetString( "AddFunction", true );
        if ( !addfunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::AddFunction ] = addfunction;
        }

        auto usefunction = xmlnode.GetString( "UseFunction", true );
        if ( !usefunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::UseFunction ] = usefunction;
        }

        auto removefunction = xmlnode.GetString( "RemoveFunction", true );
        if ( !removefunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::RemoveFunction ] = removefunction;
        }
    }

    void KFItemConfig::ReadGiftSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        auto rewardvalue = xmlnode.GetString( "Reward" );
        kfsetting->_rewards.Parse( rewardvalue, __FUNC_LINE__ );

        kfsetting->_drop_id = xmlnode.GetUInt32( "DropId" );
    }

    void KFItemConfig::ReadMedicineSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_medicine_type = xmlnode.GetUInt32( "MedicineType" );
        kfsetting->_buff_id = xmlnode.GetUInt32( "BuffId", true );
    }

    void KFItemConfig::ReadEquipSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_equip_type = xmlnode.GetUInt32( "EquipType" );
        kfsetting->_level_limit = xmlnode.GetUInt32( "LevelLimit", true );
        kfsetting->_durability = xmlnode.GetUInt32( "Durability", true );

        auto strskill = xmlnode.GetString( "Skill", true );
        kfsetting->_skills = KFUtility::SplitSet< std::set<uint32> >( strskill, DEFAULT_SPLIT_STRING );
    }
}