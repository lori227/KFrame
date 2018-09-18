#include "KFEntityEx.h"
#include "KFComponentEx.h"
#include "KFCore/KFAgent.h"
#include "KFKernelConfig.h"
#include "KFKernelModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFCore/KFDataFactory.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFEntityEx::KFEntityEx()
    {
        _is_inited = false;
        _kf_object = nullptr;
        _kf_component = nullptr;
        _is_need_to_save = false;
        _have_add_pb_object = false;
        _have_remove_pb_object = false;
        _have_update_pb_object = false;
    }

    KFEntityEx::~KFEntityEx()
    {
        KFDataFactory::Release( _kf_object );
    }

    KFData* KFEntityEx::GetData()
    {
        return _kf_object;
    }

    void KFEntityEx::SetKeyID( uint64 id )
    {
        _kf_object->SetKeyID( id );
    }

    uint64 KFEntityEx::GetKeyID()
    {
        return _kf_object->GetKeyID();
    }

    void KFEntityEx::SetName( const std::string& name )
    {
        _name = name;
    }

    const char* KFEntityEx::GetName()
    {
        return _name.c_str();
    }

    // 是否初始化完成
    bool KFEntityEx::IsInited()
    {
        return _is_inited;
    }

    void KFEntityEx::SetInited()
    {
        _is_inited = true;
    }

    bool KFEntityEx::IsNeedToSave()
    {
        return _is_need_to_save;
    }

    void KFEntityEx::SetNeetToSave( bool needtosave )
    {
        _is_need_to_save = needtosave;
    }

    void KFEntityEx::InitData( KFComponentEx* kfcomponent, const std::string& dataname )
    {
        static auto _global_class_name = _kf_option->GetValue< std::string >( __KF_STRING__( globalclassname ) );

        _kf_component = kfcomponent;
        _kf_object = KFDataFactory::CreateData( _global_class_name, dataname );
    }

    KFData* KFEntityEx::CreateData( const std::string& dataname )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return KFDataFactory::CreateData( kfdata->GetDataSetting() );
    }

    KFData* KFEntityEx::CreateData( const std::string& dataname, uint64 key )
    {
        auto kfdata = CreateData( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        kfdata->SetKeyID( key );
        return kfdata;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::UpdateData( KFData* kfdata, const std::string& value )
    {
        kfdata->SetValue<std::string>( value );

        // 属性更新回调
        _kf_component->UpdateDataCallBack( this, kfdata, value );
    }

    void KFEntityEx::UpdateData( KFData* kfparent, const std::string& dataname, const std::string& value )
    {
        auto kfdata = kfparent->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    void KFEntityEx::UpdateData( const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }


    void KFEntityEx::UpdateData( const std::string& parentname, const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    void KFEntityEx::UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( parentname, key );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, _invalid_int, operate, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& parentname, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = _kf_object->FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, _invalid_int, operate, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return UpdateData( kfparent, key, dataname, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = kfparent->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        auto key = kfparent->GetKeyID();
        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfparent, uint64 key, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfobject = kfparent->FindData( key );
        if ( kfobject == nullptr )
        {
            kfobject = AutoCreateData( kfparent, key );
            if ( kfobject == nullptr )
            {
                return _invalid_int;
            }
        }

        auto kfdata = kfobject->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfdata, uint32 operate, uint64 value )
    {
        auto key = kfdata->GetKeyID();
        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfdata, uint64 key, uint32 operate, uint64 value )
    {
        auto oldvalue = kfdata->GetValue< uint64 >();
        auto newvalue = KFUtility::Operate< uint64 >( operate, oldvalue, value );
        if ( oldvalue != newvalue )
        {
            kfdata->SetValue< uint64 >( newvalue );

            // 属性更新回调
            _kf_component->UpdateDataCallBack( this, kfdata, key, operate, value, oldvalue, newvalue );
        }

        return newvalue;
    }

    KFData* KFEntityEx::AutoCreateData( KFData* kfparent, uint64 key )
    {
        // 不存在 判断是否创建新的对象
        auto datasetting = kfparent->GetDataSetting();
        if ( !datasetting->HaveFlagMask( KFDataDefine::Mask_Auto_Create ) )
        {
            return nullptr;
        }

        auto kfdata = KFDataFactory::CreateData( datasetting );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        AddData( kfparent, key, kfdata );
        return kfdata;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::AddData( const std::string& parentname, uint64 key, KFData* kfdata )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return AddData( kfparent, key, kfdata );
    }


    bool KFEntityEx::AddData( KFData* kfparent, uint64 key, KFData* kfdata )
    {
        bool result = kfparent->AddData( key, kfdata );
        if ( result )
        {
            // 添加属性回调
            _kf_component->AddDataCallBack( this, kfparent, key, kfdata );
        }

        return result;
    }

    bool KFEntityEx::AddData( KFData* kfparent, KFData* kfdata )
    {
        auto key = kfdata->GetKeyID();
        return AddData( kfparent, key, kfdata );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::RemoveData( const std::string& parentname, uint64 key )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return RemoveData( kfparent, key );
    }

    bool KFEntityEx::RemoveData( KFData* kfparent, uint64 key )
    {
        auto kfdata = kfparent->FindData( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        // 属性删除回调
        _kf_component->RemoveDataCallBack( this, kfparent, key, kfdata );

        return kfparent->RemoveData( key );
    }

    bool KFEntityEx::RemoveData( const std::string& parentname )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        std::list<uint64> keyvector;
        auto kfdata = kfparent->FirstData();
        while ( kfdata != nullptr )
        {
            keyvector.push_back( kfdata->GetKeyID() );
            kfdata = kfparent->NextData();
        }

        for ( auto key : keyvector )
        {
            RemoveData( kfparent, key );
        }

        return true;
    }

    bool KFEntityEx::RemoveData( const std::string& parentname, const std::string& dataname )
    {
        auto kfparent = _kf_object->FindData( parentname, dataname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        std::list<uint64> keyvector;
        auto kfdata = kfparent->FirstData();
        while ( kfdata != nullptr )
        {
            keyvector.push_back( kfdata->GetKeyID() );
            kfdata = kfparent->NextData();
        }

        for ( auto key : keyvector )
        {
            RemoveData( kfparent, key );
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddAgentData( const KFAgents* kfagents, float multiple, bool showclient, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "player[{}] add agent[{:0.2f}][{}]", GetKeyID(), multiple, kfagents->_string );

        for ( auto& kfagent : kfagents->_agents )
        {
            AddAgentData( kfagent, multiple, function, line );
        }

        // 显示给客户端
        if ( _kf_component->_show_reward_function != nullptr )
        {
            auto& strreward = kfagents->FormatRewardAgent( multiple );
            _kf_component->_show_reward_function( this, strreward, showclient, function, line );
        }
    }

    // 添加元数据
    void KFEntityEx::AddAgentData( const KFAgent* kfagent, float multiple, const char* function, uint32 line )
    {
        auto datasetting = _kf_object->_class_setting->GetDataSetting( kfagent->_data_name );
        if ( datasetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find setting [{}]! ", kfagent->_string );
        }

        // 如果有注册的特殊处理函数
        auto kffunction = _kf_component->_add_agent_function.Find( kfagent->_data_name );
        if ( kffunction != nullptr )
        {
            kffunction->_function( this, const_cast< KFAgent* >( kfagent ), multiple, function, line );
        }
        else
        {
            switch ( datasetting->_type )
            {
            case KFDataDefine::Type_String:
            case KFDataDefine::Type_Array:
            case KFDataDefine::Type_Vector3D:
                __LOG_ERROR_FUNCTION__( function, line, "error data type[{}]! ", kfagent->_string );
                break;
            case KFDataDefine::Type_Object:
                AddObjectAgentData( const_cast< KFAgent* >( kfagent ), multiple, function, line );
                break;
            case KFDataDefine::Type_Record:
                AddRecordAgentData( const_cast< KFAgent* >( kfagent ), multiple, function, line );
                break;
            default:
                AddNormalAgentData( const_cast< KFAgent* >( kfagent ), multiple, function, line );
                break;
            }
        }
    }


    void KFEntityEx::AddNormalAgentData( KFAgent* kfagent, float multiple, const char* function, uint32 line )
    {
        // 操作类型
        uint32 operate = kfagent->_operate;

        for ( auto& iter : kfagent->_datas._objects )
        {
            auto value = _kf_component->_kf_kernel_module->CalcAgentValue( iter.second, multiple );
            UpdateData( iter.first, operate, value );
        }
    }

    void KFEntityEx::AddObjectAgentData( KFAgent* kfagent, float multiple, const char* function, uint32 line )
    {
        auto kfparent = _kf_object->FindData( kfagent->_data_name );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find data [{}]!", kfagent->_string );
        }

        for ( auto& iter : kfagent->_datas._objects )
        {
            auto value = _kf_component->_kf_kernel_module->CalcAgentValue( iter.second, multiple );
            UpdateData( kfparent, iter.first, kfagent->_operate, value );
        }
    }

    void KFEntityEx::AddRecordAgentData( KFAgent* kfagent, float multiple, const char* function, uint32 line )
    {
        auto configid = kfagent->_config_id;
        if ( configid == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find config id [{}]!", kfagent->_string );
        }

        // kfrecord
        auto kfparent = _kf_object->FindData( kfagent->_data_name );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find data [{}]!", kfagent->_string );
        }

        auto kfdata = kfparent->FindData( configid );
        if ( kfdata == nullptr )
        {
            UpdateData( kfparent, configid, __KF_STRING__( id ), KFOperateEnum::Set, configid );
        }

        for ( auto& iter : kfagent->_datas._objects )
        {
            auto value = _kf_component->_kf_kernel_module->CalcAgentValue( iter.second, multiple );
            UpdateData( kfparent, configid, iter.first, kfagent->_operate, value );
        }
    }

    // 判断元数据是否满足条件
    bool KFEntityEx::CheckAgentData( const KFAgents* kfagents, const char* function, uint32 line )
    {
        for ( auto& kfagent : kfagents->_agents )
        {
            if ( !CheckAgentData( kfagent, function, line ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckAgentData( const KFAgent* kfagent, const char* function, uint32 line )
    {
        auto datasetting = _kf_object->_class_setting->GetDataSetting( kfagent->_data_name );
        if ( datasetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "can't find setting [{}]!", kfagent->_string );
            return false;
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_check_agent_function.Find( kfagent->_data_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( this, const_cast< KFAgent* >( kfagent ), function, line );
        }

        // 找不到处理函数, 用基础函数来处理
        switch ( datasetting->_type )
        {
        case KFDataDefine::Type_Array:
        case KFDataDefine::Type_String:
        case KFDataDefine::Type_Vector3D:
            __LOG_ERROR_FUNCTION__( function, line, "data type error [{}]!", kfagent->_string );
            break;
        case KFDataDefine::Type_Record:
            return CheckRecordAgentData( const_cast< KFAgent* >( kfagent ) );
            break;
        case KFDataDefine::Type_Object:
            return CheckObjectAgentData( const_cast< KFAgent* >( kfagent ) );
            break;
        default:
            return CheckNormalAgentData( _kf_object, const_cast< KFAgent* >( kfagent ) );
            break;
        }

        return false;
    }


    bool KFEntityEx::CheckNormalAgentData( KFData* kfdata, KFAgent* kfagent )
    {
        for ( auto& iter : kfagent->_datas._objects )
        {
            auto agentvalue = iter.second->GetValue();
            auto datavalue = kfdata->GetValue< uint64 >( iter.first );
            if ( datavalue < agentvalue )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckObjectAgentData( KFAgent* kfagent )
    {
        auto kfdata = _kf_object->FindData( kfagent->_data_name );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return CheckNormalAgentData( kfdata, kfagent );
    }

    bool KFEntityEx::CheckRecordAgentData( KFAgent* kfagent )
    {
        auto kfdata = _kf_object->FindData( kfagent->_data_name, kfagent->_config_id );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return CheckNormalAgentData( kfdata, kfagent );
    }

    void KFEntityEx::RemoveAgentData( const KFAgents* kfagents, const char* function, uint32 line )
    {
        for ( auto& kfagent : kfagents->_agents )
        {
            RemoveAgentData( kfagent, function, line );
        }
    }

    // 删除元数据
    void KFEntityEx::RemoveAgentData( const KFAgent* kfagent, const char* function, uint32 line )
    {
        auto datasetting = _kf_object->_class_setting->GetDataSetting( kfagent->_data_name );
        if ( datasetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find setting [{}]!", kfagent->_string );
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_remove_agent_function.Find( kfagent->_data_name );
        if ( kffunction != nullptr )
        {
            // 删除数据
            kffunction->_function( this, const_cast< KFAgent* >( kfagent ), function, line );
        }
        else
        {
            // 找不到处理函数, 用基础函数来处理
            switch ( datasetting->_type )
            {
            case KFDataDefine::Type_String:
            case KFDataDefine::Type_Array:
            case KFDataDefine::Type_Vector3D:
                __LOG_ERROR_FUNCTION__( function, line, "data type error [{}]!", kfagent->_string );
                break;
            case KFDataDefine::Type_Record:
                RemoveRecordAgentData( const_cast< KFAgent* >( kfagent ), function, line );
                break;
            case KFDataDefine::Type_Object:
                RemoveObjectAgentData( const_cast< KFAgent* >( kfagent ), function, line );
                break;
            default:
                RemoveNormalAgentData( const_cast< KFAgent* >( kfagent ), function, line );
                break;
            }
        }

        // 记录log
        if ( datasetting->HaveFlagMask( KFDataDefine::Mask_System_Log ) )
        {
            __LOG_INFO_FUNCTION__( function, line, "[{}] remove data [{}]", GetKeyID(), kfagent->_string );
        }
    }

    void KFEntityEx::RemoveRecordAgentData( KFAgent* kfagent, const char* function, uint32 line )
    {
        auto kfparent = _kf_object->FindData( kfagent->_data_name, kfagent->_config_id );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line,  "can't find config id [{}]!", kfagent->_string );
        }

        for ( auto& iter : kfagent->_datas._objects )
        {
            auto& dataname = iter.first;
            auto value = iter.second->GetValue();

            UpdateData( kfparent, dataname, KFOperateEnum::Dec, value );
        }
    }

    void KFEntityEx::RemoveObjectAgentData( KFAgent* kfagent, const char* function, uint32 line )
    {
        auto kfparent = _kf_object->FindData( kfagent->_data_name );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find config id [{}]!", kfagent->_string );
        }

        for ( auto& iter : kfagent->_datas._objects )
        {
            auto& dataname = iter.first;
            auto value = iter.second->GetValue();

            UpdateData( kfparent, dataname, KFOperateEnum::Dec, value );
        }
    }

    void KFEntityEx::RemoveNormalAgentData( KFAgent* kfagent, const char* function, uint32 line )
    {
        for ( auto& iter : kfagent->_datas._objects )
        {
            auto& dataname = iter.first;
            auto value = iter.second->GetValue();

            UpdateData( dataname, KFOperateEnum::Dec, value );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddSyncUpdateDataToPBObject( KFData* kfdata, KFMsg::PBObject* pbobject )
    {
        auto datasetting = kfdata->GetDataSetting();
        switch ( datasetting->_type )
        {
        case KFDataDefine::Type_Int32:
            _kf_component->_kf_kernel_module->SaveToInt32( kfdata, pbobject->add_pbint32() );
            break;
        case KFDataDefine::Type_UInt32:
            _kf_component->_kf_kernel_module->SaveToUInt32( kfdata, pbobject->add_pbuint32() );
            break;
        case KFDataDefine::Type_Int64:
            _kf_component->_kf_kernel_module->SaveToInt64( kfdata, pbobject->add_pbint64() );
            break;
        case KFDataDefine::Type_UInt64:
            _kf_component->_kf_kernel_module->SaveToUInt64( kfdata, pbobject->add_pbuint64() );
            break;
        case KFDataDefine::Type_Double:
            _kf_component->_kf_kernel_module->SaveToDouble( kfdata, pbobject->add_pbdouble() );
            break;
        case KFDataDefine::Type_Vector3D:
            _kf_component->_kf_kernel_module->SaveToVector3D( kfdata, pbobject->add_pbvector3d() );
            break;
        case KFDataDefine::Type_String:
            _kf_component->_kf_kernel_module->SaveToString( kfdata, pbobject->add_pbstring() );
            break;
        case KFDataDefine::Type_Array:
            _kf_component->_kf_kernel_module->SaveToArray( kfdata, pbobject->add_pbarray() );
            break;
        }
    }

    KFMsg::PBObject* KFEntityEx::FindObjectFromPBObject( KFData* kfdata, KFMsg::PBObject* pbobject )
    {
        auto datasetting = kfdata->GetDataSetting();
        for ( auto i = 0; i < pbobject->pbobject_size(); ++i )
        {
            auto temp = pbobject->mutable_pbobject( i );
            if ( temp->name() == datasetting->_name )
            {
                return temp;
            }
        }

        // 没有找到, 添加
        auto retobject = pbobject->add_pbobject();
        retobject->set_key( kfdata->GetKeyID() );
        retobject->set_name( datasetting->_name );

        return retobject;
    }

    KFMsg::PBRecord* KFEntityEx::FindRecordFromPBObject( KFData* kfdata, KFMsg::PBObject* pbobject )
    {
        auto datasetting = kfdata->GetDataSetting();
        for ( auto i = 0; i < pbobject->pbrecord_size(); ++i )
        {
            auto temp = pbobject->mutable_pbrecord( i );
            if ( temp->name() == datasetting->_name )
            {
                return temp;
            }
        }

        // 没有找到, 添加
        auto retrecord = pbobject->add_pbrecord();
        retrecord->set_name( datasetting->_name );
        return retrecord;
    }

    KFMsg::PBObject* KFEntityEx::FindObjectFromPBRecord( KFData* kfdata, KFMsg::PBRecord* pbrecord )
    {
        auto key = kfdata->GetKeyID();
        auto datasetting = kfdata->GetDataSetting();
        for ( auto i = 0; i < pbrecord->pbobject_size(); ++i )
        {
            auto pbchild = pbrecord->mutable_pbobject( i );
            auto findkey = pbchild->key();
            if ( findkey == key )
            {
                return pbchild;
            }
        }

        // 没有找到, 添加
        auto retobject = pbrecord->add_pbobject();
        retobject->set_key( key );
        retobject->set_name( datasetting->_name );

        return retobject;
    }

    ////////////////////////////////////////////////////////////////////
    void KFEntityEx::SyncAddData( KFData* kfdata, uint64 key )
    {
        // 没有注册函数
        if ( _kf_component->_entity_sync_add_function == nullptr )
        {
            return;
        }

        // 不需要同步
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();

        auto pbobject = &_add_pb_object;
        do
        {
            auto savedata = datahierarchy.front();
            datahierarchy.pop_front();

            if ( savedata->GetType() == KFDataDefine::Type_Object )
            {
                pbobject = FindObjectFromPBObject( savedata, pbobject );
            }
            else if ( savedata->GetType() == KFDataDefine::Type_Record )
            {
                auto pbrecord = FindRecordFromPBObject( savedata, pbobject );

                savedata = datahierarchy.front();
                datahierarchy.pop_front();

                pbobject = FindObjectFromPBRecord( savedata, pbrecord );
            }

            if ( savedata == kfdata )
            {
                _kf_component->_kf_kernel_module->SaveToObject( savedata, pbobject, KFDataDefine::Mask_Sync_Client );
            }
        } while ( !datahierarchy.empty() );

        // 设置有添加属性
        _have_add_pb_object = true;
    }

    void KFEntityEx::SyncRemoveData( KFData* kfdata, uint64 key )
    {
        if ( _kf_component->_entity_sync_remove_function == nullptr )
        {
            return;
        }

        // 不需要同步
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();

        auto pbobject = &_remove_pb_object;
        do
        {
            auto savedata = datahierarchy.front();
            datahierarchy.pop_front();

            if ( savedata->GetType() == KFDataDefine::Type_Object )
            {
                pbobject = FindObjectFromPBObject( savedata, pbobject );
            }
            else if ( savedata->GetType() == KFDataDefine::Type_Record )
            {
                auto pbrecord = FindRecordFromPBObject( savedata, pbobject );

                savedata = datahierarchy.front();
                datahierarchy.pop_front();

                pbobject = FindObjectFromPBRecord( savedata, pbrecord );
            }
        } while ( !datahierarchy.empty() );

        _have_remove_pb_object = true;
    }

    void KFEntityEx::SyncUpdateData( KFData* kfdata, uint64 key )
    {
        if ( _kf_component->_entity_sync_update_function == nullptr )
        {
            return;
        }

        // 不需要同步
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();
        if ( datahierarchy.empty() )
        {
            return;
        }

        auto pbobject = &_update_pb_object;
        do
        {
            auto savedata = datahierarchy.front();
            datahierarchy.pop_front();

            if ( savedata->GetType() == KFDataDefine::Type_Object )
            {
                pbobject = FindObjectFromPBObject( savedata, pbobject );
            }
            else if ( savedata->GetType() == KFDataDefine::Type_Record )
            {
                auto pbrecord = FindRecordFromPBObject( savedata, pbobject );

                savedata = datahierarchy.front();
                datahierarchy.pop_front();

                pbobject = FindObjectFromPBRecord( savedata, pbrecord );
            }

            if ( savedata == kfdata )
            {
                AddSyncUpdateDataToPBObject( savedata, pbobject );
            }
        } while ( !datahierarchy.empty() );

        // 设置有添加属性
        _have_update_pb_object = true;
    }

    void KFEntityEx::SyncEntityToClient()
    {
        // 同步更新
        SyncUpdateDataToClient();

        // 同步添加
        SyncAddDataToClient();

        // 同步删除
        SyncRemoveDataToClient();
    }

    void KFEntityEx::SyncAddDataToClient()
    {
        if ( !_have_add_pb_object )
        {
            return;
        }

        _add_pb_object.set_key( GetKeyID() );
        _add_pb_object.set_name( _kf_component->GetName() );
        _kf_component->_entity_sync_add_function( this, _add_pb_object );

        _have_add_pb_object = false;
        _add_pb_object.Clear();
    }

    void KFEntityEx::SyncUpdateDataToClient()
    {
        if ( !_have_update_pb_object )
        {
            return;
        }

        _update_pb_object.set_key( GetKeyID() );
        _update_pb_object.set_name( _kf_component->GetName() );
        _kf_component->_entity_sync_update_function( this, _update_pb_object );

        _have_update_pb_object = false;
        _update_pb_object.Clear();
    }

    void KFEntityEx::SyncRemoveDataToClient()
    {
        if ( !_have_remove_pb_object )
        {
            return;
        }

        _remove_pb_object.set_key( GetKeyID() );
        _remove_pb_object.set_name( _kf_component->GetName() );
        _kf_component->_entity_sync_remove_function( this, _remove_pb_object );

        _have_remove_pb_object = false;
        _remove_pb_object.Clear();
    }
}