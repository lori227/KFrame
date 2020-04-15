#include "KFExecuteConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFExecuteConfig::ReadSetting( KFNode& xmlnode, KFExecuteSetting* kfsetting )
    {
        auto executedata = __KF_NEW__( KFExecuteData );
        KFReadSetting::ReadExecuteData( xmlnode, executedata );
        kfsetting->_execute_data_list.push_back( executedata );
    }

    void KFExecuteConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            kfsetting->_sort_execute_data_list.Clear( false );
            for ( auto executedata : kfsetting->_execute_data_list )
            {
                auto sort = KFExecuteSortConfig::Instance()->GetExecuteSort( executedata->_name );
                kfsetting->_sort_execute_data_list.Insert( sort, executedata );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFExecuteSortConfig::ReadSetting( KFNode& xmlnode, KFExecuteSortSetting* kfsetting )
    {
        kfsetting->_sort = xmlnode.GetUInt32( "Sort" );
        if ( kfsetting->_sort > _max_sort )
        {
            _max_sort = kfsetting->_sort;
        }
    }

    uint32 KFExecuteSortConfig::GetExecuteSort( const std::string& name )
    {
        auto kfsetting = _settings.Create( name );
        if ( kfsetting->_sort == 0u )
        {
            kfsetting->_sort = ++_max_sort;
        }

        return kfsetting->_sort;
    }
}