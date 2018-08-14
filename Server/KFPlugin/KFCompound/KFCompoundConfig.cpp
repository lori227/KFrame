#include "KFCompoundConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFCompoundConfig::KFCompoundConfig()
    {

    }

    KFCompoundConfig::~KFCompoundConfig()
    {

    }

    void KFCompoundConfig::AddCompoundSetting( KFCompoundSetting* kfsetting )
    {
        auto key = CompoundKey( kfsetting->_data_name, kfsetting->_key );
        _compound_setting.Insert( key, kfsetting );
    }

    const KFCompoundSetting* KFCompoundConfig::FindCompoundSetting( const std::string& dataname, uint32 id )
    {
        auto key = CompoundKey( dataname, id );
        return _compound_setting.Find( key );
    }

    bool KFCompoundConfig::LoadConfig()
    {
        _compound_setting.Clear();

        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto setting = config.FindNode( "Setting" );
            while ( setting.IsValid() )
            {
                auto ksetting = __KF_CREATE__( KFCompoundSetting );

                ksetting->_data_name = setting.GetString( "DataName" );
                ksetting->_key = setting.GetUInt32( "Key" );

                auto strcompounddata = setting.GetString( "CompoundData" );
                ksetting->_compound_data.ParseFromString( strcompounddata, __FUNC_LINE__ );

                auto strcostdata = setting.GetString( "CostData" );
                auto ok = ksetting->_cost_data.ParseFromString( strcostdata, __FUNC_LINE__ );
                if ( ok )
                {
                    AddCompoundSetting( ksetting );
                }

                setting.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}