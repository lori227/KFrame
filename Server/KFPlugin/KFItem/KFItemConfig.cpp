#include "KFItemConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    KFItemConfig::KFItemConfig()
    {

    }

    KFItemConfig::~KFItemConfig()
    {
    }

    const KFItemSetting* KFItemConfig::FindItemSetting( uint32 itemid ) const
    {
        return _item_setting.Find( itemid );
    }

    void KFItemConfig::AddItemSetting( KFItemSetting* kfsetting )
    {
        _item_setting.Insert( kfsetting->_id, kfsetting );
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFItemConfig::LoadConfig( const char* file )
    {
        _item_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFItemSetting );

                kfsetting->_id = xmlnode.GetUInt32( "Id" );
                kfsetting->_type = xmlnode.GetUInt32( "Type" );
                kfsetting->_quality = xmlnode.GetUInt32( "Quality" );
                kfsetting->_use_count = xmlnode.GetUInt32( "UseCount" );
                kfsetting->_overlay_type = xmlnode.GetUInt32( "OverlayType" );
                kfsetting->_overlay_count = xmlnode.GetUInt32( "OverlayValue" );
                kfsetting->_time_type = xmlnode.GetUInt32( "TimeType" );
                kfsetting->_valid_time = xmlnode.GetUInt32( "TimeValue" );
                kfsetting->_mail_id = xmlnode.GetUInt32( "MailId" );
                kfsetting->_reward_type = xmlnode.GetUInt32( "RewardType" );

                auto rewardvalue = xmlnode.GetString( "RewardValue" );
                kfsetting->_rewards.ParseFromString( rewardvalue, __FUNC_LINE__ );

                kfsetting->_lua_file = xmlnode.GetString( "LuaFile" );
                auto addfunction = xmlnode.GetString( "AddFunction" );
                if ( !addfunction.empty() )
                {
                    kfsetting->_function[ KFItemEnum::AddFunction ] = addfunction;
                }

                auto usefunction = xmlnode.GetString( "UseFunction" );
                if ( !usefunction.empty() )
                {
                    kfsetting->_function[ KFItemEnum::UseFunction ] = usefunction;
                }

                auto removefunction = xmlnode.GetString( "RemoveFunction" );
                if ( !removefunction.empty() )
                {
                    kfsetting->_function[ KFItemEnum::RemoveFunction ] = removefunction;
                }

                AddItemSetting( kfsetting );
                xmlnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}