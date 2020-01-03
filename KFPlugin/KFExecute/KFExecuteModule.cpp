#include "KFExecuteModule.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExecuteModule::BindExecuteFunction( const std::string& name, KFExecuteFunction& function )
    {
        auto kffucntion = _execute_function.Create( name );
        kffucntion->_function = function;
    }

    void KFExecuteModule::UnBindExecuteFunction( const std::string& name )
    {
        _execute_function.Remove( name );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFExecuteModule::Execute( KFEntity* player, const KFExecuteData* executedata, const char* function, uint32 line )
    {
        return Execute( player, executedata, __STRING__( execute ), 0u, function, line );
    }

    bool KFExecuteModule::Execute( KFEntity* player, const KFExecuteData* executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        // 如果是属性, 直接添加属性
        if ( executedata->_name == __STRING__( data ) )
        {
            if ( executedata->_param_list._elements.IsEmpty() )
            {
                return false;
            }

            player->AddElement( &executedata->_param_list._elements, modulename, __FUNC_LINE__ );
            return true;
        }

        // 如果是掉落, 判断是否有掉落分支
        if ( executedata->_name == __STRING__( drop ) )
        {
            if ( executedata->_param_list._params.size() < 1u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "execute drop param size<1" );
                return false;
            }

            auto& droplist = executedata->_param_list._params[ 0 ]->_vector_value;
            if ( !droplist.empty() )
            {
                _kf_drop->Drop( player, droplist, modulename, function, line );
            }

            if ( executedata->_param_list._params.size() >= 3u )
            {
                if ( executedata->_calc_value != 0u )
                {
                    auto exetenddropid = KFUtility::GetMaxMapValue( executedata->_param_list._params[ 2 ]->_map_value, executedata->_calc_value );
                    if ( exetenddropid != 0u )
                    {
                        _kf_drop->Drop( player, exetenddropid, modulename, function, line );
                    }
                }
            }

            return true;
        }

        // 注册的执行逻辑
        auto kffunction = _execute_function.Find( executedata->_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( player, executedata, modulename, moduleid, function, line );
        }

        return false;
    }

}