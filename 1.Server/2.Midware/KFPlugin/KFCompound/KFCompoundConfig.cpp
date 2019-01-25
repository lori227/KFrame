#include "KFCompoundConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    const KFCompoundSetting* KFCompoundConfig::FindCompoundSetting( const std::string& dataname, uint32 id )
    {
        auto key = CompoundKey( dataname, id );
        return _compound_setting.Find( key );
    }

    bool KFCompoundConfig::LoadConfig()
    {
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto setting = config.FindNode( "Setting" );
        while ( setting.IsValid() )
        {
            auto dataname = setting.GetString( "DataName" );
            auto key = setting.GetUInt32( "Key" );
            auto ksetting = _compound_setting.Create( CompoundKey( dataname, key ) );

            ksetting->_data_name = dataname;
            ksetting->_key = key;

            auto strcompounddata = setting.GetString( "CompoundData" );
            ksetting->_compound_data.Parse( strcompounddata, __FUNC_LINE__ );

            auto strcostdata = setting.GetString( "CostData" );
            ksetting->_cost_data.Parse( strcostdata, __FUNC_LINE__ );

            setting.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}