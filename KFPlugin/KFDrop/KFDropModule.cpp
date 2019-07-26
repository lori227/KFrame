#include "KFDropModule.hpp"

namespace KFrame
{
    void KFDropModule::InitModule()
    {
        __KF_ADD_CONFIG__( KFDropConfig );
    }

    DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line )
    {
        static DropDataList _drops;
        _drops.clear();

        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), dropid, count );

        auto kfsetting = KFDropConfig::Instance()->FindSetting( dropid );
        if ( kfsetting != nullptr )
        {
            for ( auto i = 0u; i < count; ++i )
            {
                Drop( player, kfsetting, _drops, function, line );
            }

            for ( auto& kfelements : _drops )
            {
                player->AddElement( kfelements, showclient, function, line );
            }
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] can't find setting!", dropid );
        }

        return _drops;
    }

    void KFDropModule::Drop( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line )
    {
        switch ( kfsetting->_type )
        {
        case KFDropEnum::Rand:
            DropDataByRand( player, kfsetting, outlist, function, line );
            break;
        case KFDropEnum::Weight:
            DropDataByWeight( player, kfsetting, outlist, function, line );
            break;
        default:
            __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] type=[{}] error!", kfsetting->_id, kfsetting->_type );
            break;
        }
    }

    void KFDropModule::DropDataByRand( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line )
    {
        bool ok = CheckDropVarCount( player, kfsetting );
        for ( auto dropdata : kfsetting->_drop_list._weight_data )
        {
            if ( ok && dropdata->_is_drop )
            {
                outlist.push_back( &dropdata->_data );
            }
            else
            {
                auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
                if ( rand < dropdata->_weight )
                {
                    outlist.push_back( &dropdata->_data );
                    if ( dropdata->_is_drop )
                    {
                        ResetDropVarCount( player, kfsetting );
                    }
                }
            }
        }
    }

    void KFDropModule::DropDataByWeight( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const char* function, uint32 line )
    {
        bool ok = CheckDropVarCount( player, kfsetting );
        if ( ok )
        {
            // 找到必掉的数据
            for ( auto dropdata : kfsetting->_drop_list._weight_data )
            {
                if ( dropdata->_is_drop )
                {
                    outlist.push_back( &dropdata->_data );
                }
            }
        }
        else
        {
            auto dropdata = kfsetting->_drop_list.Rand();
            if ( dropdata != nullptr && !dropdata->_data.IsEmpty() )
            {
                outlist.push_back( &dropdata->_data );
                if ( dropdata->_is_drop )
                {
                    ResetDropVarCount( player, kfsetting );
                }
            }
        }
    }

    bool KFDropModule::CheckDropVarCount( KFEntity* player, const KFDropSetting* kfsetting )
    {
        if ( kfsetting->_var_id == 0u )
        {
            return false;
        }

        auto value = player->UpdateData( __KF_STRING__( var ), kfsetting->_var_id, __KF_STRING__( value ), KFEnum::Add, 1u );
        bool ok = value >= kfsetting->_max_count;
        if ( ok )
        {
            player->UpdateData( __KF_STRING__( var ), kfsetting->_var_id, __KF_STRING__( value ), KFEnum::Set, 0u );
        }

        return ok;
    }

    void KFDropModule::ResetDropVarCount( KFEntity* player, const KFDropSetting* kfsetting )
    {
        if ( kfsetting->_var_id == 0u )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( var ), kfsetting->_var_id, __KF_STRING__( value ), KFEnum::Set, 0u );
    }

}