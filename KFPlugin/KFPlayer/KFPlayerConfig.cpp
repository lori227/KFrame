#include "KFPlayerConfig.hpp"

namespace KFrame
{
    void KFPlayerConfig::ClearElements()
    {
        for ( auto kflements : _new_player_elements )
        {
            __KF_DELETE__( KFElements, kflements );
        }

        for ( auto kflements : _new_role_elements )
        {
            __KF_DELETE__( KFElements, kflements );
        }

        _new_player_elements.clear();
        _new_role_elements.clear();
    }

    void KFPlayerConfig::ClearSetting()
    {
        ClearElements();
        KFConfigT< KFIntSetting >::ClearSetting();
    }

    KFIntSetting* KFPlayerConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true );
        auto channel = xmlnode.GetUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return _settings.Create( 0 );
    }

    void KFPlayerConfig::ReadSetting( KFNode& xmlnode, KFIntSetting* kfsetting )
    {
        auto kfelements = __KF_NEW__( KFElements );
        auto strelements = xmlnode.GetString( "Elements" );
        kfelements->Parse( strelements, __FUNC_LINE__ );

        auto type = xmlnode.GetUInt32( "Type" );
        if ( type == _invalid_int )
        {
            _new_role_elements.push_back( kfelements );
        }
        else
        {
            _new_player_elements.push_back( kfelements );
        }
    }
}