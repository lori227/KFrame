#include "KFExecuteModule.hpp"

namespace KFrame
{
    void KFExecuteModule::BeforeRun()
    {
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( executeopen ), &KFExecuteModule::OnDeployExecuteOpen );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( executeclose ), &KFExecuteModule::OnDeployExecuteClose );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_EXECUTE__( __STRING__( data ), &KFExecuteModule::OnExecuteAddData );
        __REGISTER_EXECUTE__( __STRING__( drop ), &KFExecuteModule::OnExecuteDropLogic );
        __REGISTER_EXECUTE__( __STRING__( execute ), &KFExecuteModule::OnExecuteQueueLogic );
    }

    void KFExecuteModule::BeforeShut()
    {
        __UN_DEPLOY_FUNCTION__( __STRING__( executeopen ) );
        __UN_DEPLOY_FUNCTION__( __STRING__( executeclose ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_EXECUTE__( __STRING__( data ) );
        __UN_EXECUTE__( __STRING__( drop ) );
        __UN_EXECUTE__( __STRING__( execute ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExecuteModule::BindExecuteFunction( KFModule* module, const std::string& name, KFExecuteFunction& function )
    {
        auto kfhandle = _execute_function.Create( name );
        kfhandle->SetFunction( module, function );
    }

    void KFExecuteModule::UnBindExecuteFunction( const std::string& name )
    {
        _execute_function.Remove( name );
    }

    void KFExecuteModule::SetExecuteOpen( const std::string& name, bool isopen )
    {
        auto kfhandle = _execute_function.Find( name );
        if ( kfhandle != nullptr )
        {
            kfhandle->SetOpen( isopen );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFExecuteModule::OnDeployExecuteOpen )
    {
        SetExecuteOpen( param, true );
    }

    __KF_DEPLOY_FUNCTION__( KFExecuteModule::OnDeployExecuteClose )
    {
        SetExecuteOpen( param, false );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFExecuteModule::Execute( KFEntity* player, uint32 executeid, const char* function, uint32 line )
    {
        return Execute( player, executeid, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( KFEntity* player, uint32 executeid, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        auto kfsetting = KFExecuteConfig::Instance()->FindSetting( executeid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", executeid );
            return false;
        }

        for ( auto& executedata : kfsetting->_execute_data_list )
        {
            Execute( player, executedata._execute, modulename, moduleid, function, line );
        }

        return true;
    }

    bool KFExecuteModule::Execute( KFEntity* player, const UInt32Vector& executelist, const char* function, uint32 line )
    {
        return Execute( player, executelist, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( KFEntity* player, const UInt32Vector& executelist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        for ( auto executeid : executelist )
        {
            auto kfsetting = KFExecuteConfig::Instance()->FindSetting( executeid );
            if ( kfsetting == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", executeid );
                continue;
            }

            for ( auto& executedata : kfsetting->_execute_data_list )
            {
                Execute( player, executedata._execute, modulename, moduleid, function, line );
            }
        }

        return true;
    }

    bool KFExecuteModule::Execute( KFEntity* player, const ExecuteDataPtr& executedata, const char* function, uint32 line )
    {
        return Execute( player, executedata, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( KFEntity* player, const ExecuteDataPtr& executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        // 注册的执行逻辑
        auto kffunction = _execute_function.Find( executedata->_name );
        if ( kffunction != nullptr )
        {
            return kffunction->CallEx<bool>( player, executedata, modulename, moduleid, function, line );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute name=[{}] no function!", executedata->_name );
        }

        return false;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteAddData )
    {
        if ( executedata->_elements.IsEmpty() )
        {
            return false;
        }

        player->AddElement( &executedata->_elements, _default_multiple, modulename, moduleid, __FUNC_LINE__ );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteDropLogic )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute drop param size<1" );
            return false;
        }

        auto& droplist = executedata->_param_list._params[ 0 ]->_vector_value;
        if ( !droplist.empty() )
        {
            _kf_drop->Drop( player, droplist, modulename, moduleid, function, line );
        }

        if ( executedata->_param_list._params.size() >= 3u )
        {
            if ( executedata->_calc_value != 0u )
            {
                auto exetenddropid = KFUtility::GetMaxMapValue( executedata->_param_list._params[ 2 ]->_map_value, executedata->_calc_value );
                if ( exetenddropid != 0u )
                {
                    _kf_drop->Drop( player, exetenddropid, modulename, moduleid, function, line );
                }
            }
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteQueueLogic )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute queue param size<1" );
            return false;
        }

        auto& executelist = executedata->_param_list._params[ 0 ]->_vector_value;
        return Execute( player, executelist, modulename, moduleid, function, line );
    }
}