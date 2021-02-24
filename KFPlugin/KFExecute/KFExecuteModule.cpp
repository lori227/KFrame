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
    void KFExecuteModule::BindExecuteFunction( const std::string& name, KFModule* module, KFExecuteFunction& function )
    {
        auto function_data = _execute_function.Create( name );
        function_data->SetFunction( module, function );
    }

    void KFExecuteModule::UnBindExecuteFunction( const std::string& name )
    {
        _execute_function.Remove( name );
    }

    void KFExecuteModule::SetExecuteOpen( const std::string& name, bool is_open )
    {
        auto function_data = _execute_function.Find( name );
        if ( function_data != nullptr )
        {
            function_data->SetOpen( is_open );
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
    bool KFExecuteModule::Execute( EntityPtr player, uint32 execute_id, const char* function, uint32 line )
    {
        return Execute( player, execute_id, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, uint32 execute_id, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        auto setting = KFExecuteConfig::Instance()->FindSetting( execute_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", execute_id );
            return false;
        }

        for ( auto& execute_data : setting->_execute_data )
        {
            Execute( player, execute_data._execute, module_name, module_id, function, line );
        }

        return true;
    }

    bool KFExecuteModule::Execute( EntityPtr player, const UInt32Vector& execute_list, const char* function, uint32 line )
    {
        return Execute( player, execute_list, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, const UInt32Vector& execute_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        for ( auto execute_id : execute_list )
        {
            auto setting = KFExecuteConfig::Instance()->FindSetting( execute_id );
            if ( setting == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "execute=[{}] can't find setting", execute_id );
                continue;
            }

            for ( auto& execute_data : setting->_execute_data )
            {
                Execute( player, execute_data._execute, module_name, module_id, function, line );
            }
        }

        return true;
    }

    bool KFExecuteModule::Execute( EntityPtr player, const ExecuteDataPtr& execute_data, const char* function, uint32 line )
    {
        return Execute( player, execute_data, _invalid_string, _invalid_int, function, line );
    }

    bool KFExecuteModule::Execute( EntityPtr player, const ExecuteDataPtr& execute_data, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        // 注册的执行逻辑
        auto function_data = _execute_function.Find( execute_data->_name );
        if ( function_data != nullptr )
        {
            return function_data->CallEx<bool>( player, execute_data, module_name, module_id, function, line );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute name=[{}] no function!", execute_data->_name );
        }

        return false;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteAddData )
    {
        if ( execute_data->_elements.IsEmpty() )
        {
            return false;
        }

        player->AddElement( &execute_data->_elements, _default_multiple, module_name, module_id, __FUNC_LINE__ );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteDropLogic )
    {
        if ( execute_data->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute drop param size<1" );
            return false;
        }

        auto& drop_list = execute_data->_param_list._params[ 0 ]->_vector_value;
        if ( !drop_list.empty() )
        {
            _kf_drop->Drop( player, drop_list, module_name, module_id, function, line );
        }

        if ( execute_data->_param_list._params.size() >= 3u )
        {
            if ( execute_data->_calc_value != 0u )
            {
                auto exetend_drop_id = KFUtility::GetMaxMapValue( execute_data->_param_list._params[ 2 ]->_map_value, execute_data->_calc_value );
                if ( exetend_drop_id != 0u )
                {
                    _kf_drop->Drop( player, exetend_drop_id, module_name, module_id, function, line );
                }
            }
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFExecuteModule::OnExecuteQueueLogic )
    {
        if ( execute_data->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute queue param size<1" );
            return false;
        }

        auto& execute_list = execute_data->_param_list._params[ 0 ]->_vector_value;
        return Execute( player, execute_list, module_name, module_id, function, line );
    }
}