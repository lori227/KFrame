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
    bool KFExecuteModule::Execute( EntityPtr player, uint32 executeid, const char* function, uint32 line )
    {
        return Execute( player, executeid, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, uint32 executeid, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        auto setting = KFExecuteConfig::Instance()->FindSetting( executeid );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", executeid );
            return false;
        }

        for ( auto& executedata : setting->_execute_data )
        {
            Execute( player, executedata._execute, module_name, module_id, function, line );
        }

        return true;
    }

    bool KFExecuteModule::Execute( EntityPtr player, const UInt32Vector& executelist, const char* function, uint32 line )
    {
        return Execute( player, executelist, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, const UInt32Vector& executelist, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        for ( auto executeid : executelist )
        {
            auto setting = KFExecuteConfig::Instance()->FindSetting( executeid );
            if ( setting == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", executeid );
                continue;
            }

            for ( auto& executedata : setting->_execute_data )
            {
                Execute( player, executedata._execute, module_name, module_id, function, line );
            }
        }

        return true;
    }

    bool KFExecuteModule::Execute( EntityPtr player, const ExecuteDataPtr& executedata, const char* function, uint32 line )
    {
        return Execute( player, executedata, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, const ExecuteDataPtr& executedata, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        // 注册的执行逻辑
        auto kffunction = _execute_function.Find( executedata->_name );
        if ( kffunction != nullptr )
        {
            return kffunction->CallEx<bool>( player, executedata, module_name, module_id, function, line );
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

        player->AddElement( &executedata->_elements, _default_multiple, module_name, module_id, __FUNC_LINE__ );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteDropLogic )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute drop param size<1" );
            return false;
        }

        auto& drop_list = executedata->_param_list._params[ 0 ]->_vector_value;
        if ( !drop_list.empty() )
        {
            _kf_drop->Drop( player, drop_list, module_name, module_id, function, line );
        }

        if ( executedata->_param_list._params.size() >= 3u )
        {
            if ( executedata->_calc_value != 0u )
            {
                auto exetenddropid = KFUtility::GetMaxMapValue( executedata->_param_list._params[ 2 ]->_map_value, executedata->_calc_value );
                if ( exetenddropid != 0u )
                {
                    _kf_drop->Drop( player, exetenddropid, module_name, module_id, function, line );
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
        return Execute( player, executelist, module_name, module_id, function, line );
    }
}