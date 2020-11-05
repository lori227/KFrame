#include "KFExecuteConfig.hpp"
#include "KFXmlReader/KFReadSetting.h"

namespace KFrame
{
    void KFExecuteConfig::ReadSetting( KFNode& xmlnode, KFExecuteSetting* kfsetting )
    {
        auto executedata = __KF_NEW__( KFExecuteData );
        KFReadSetting::ReadExecuteData( xmlnode, executedata );
        kfsetting->_execute_list.Insert( executedata );
    }

    void KFExecuteConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;

            for ( auto executedata : kfsetting->_execute_list._objects )
            {
                if ( executedata->_name != __STRING__( data ) )
                {
                    continue;
                }

                if ( executedata->_param_list._params.size() < 3u )
                {
                    __LOG_ERROR__( "task=[{}] execute=[data] param size<3 error", kfsetting->_id );
                    continue;
                }

                auto& dataname = executedata->_param_list._params[ 0 ]->_str_value;
                auto& datavalue = executedata->_param_list._params[ 1 ]->_str_value;
                auto& datakey = executedata->_param_list._params[ 2 ]->_int_value;
                KFElementConfig::Instance()->FormatElement( executedata->_param_list._elements, dataname, datavalue, datakey );
            }

        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
}