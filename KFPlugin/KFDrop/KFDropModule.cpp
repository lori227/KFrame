#include "KFDropModule.hpp"

namespace KFrame
{
    void KFDropModule::BindDropLogicFunction( const std::string& dataname, KFDropLogicFunction& function )
    {
        auto kffucntion = _drop_logic_function.Create( dataname );
        kffucntion->_function = function;
    }

    void KFDropModule::UnRegisterDropLogicFunction( const std::string& dataname )
    {
        _drop_logic_function.Remove( dataname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line )
    {
        auto& _drops = DropLogic( player, dropid, count, function, line );
        for ( auto iter = _drops.begin(); iter != _drops.end(); )
        {
            auto dropdata = *iter;
            auto kffunction = _drop_logic_function.Find( dropdata->_data_name );
            if ( kffunction != nullptr )
            {
                iter = _drops.erase( iter );
                kffunction->_function( player, dropdata );
            }
            else
            {
                ++iter;
                player->AddElement( &dropdata->_elements, showclient, function, line );
            }
        }

        return _drops;
    }

    DropDataList& KFDropModule::DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line )
    {
        static DropDataList _drops;
        _drops.clear();

        // 掉落逻辑
        DropLogic( player, dropid, count, _drops, function, line );
        return _drops;
    }

    void KFDropModule::DropLogic( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line )
    {
        if ( dropid != 0u )
        {
            return;
        }

        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), dropid, count );
        auto kfsetting = KFDropGroupConfig::Instance()->FindSetting( dropid );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] can't find setting!", dropid );
        }

        for ( auto i = 0u; i < count; ++i )
        {
            DropLogic( player, kfsetting, outlist );
        }
    }

    void KFDropModule::DropLogic( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        if ( kfsetting->_is_drop_count )
        {
            player->UpdateData( __KF_STRING__( drop ), kfsetting->_id, __KF_STRING__( value ), KFEnum::Add, 1u );
        }

        // 必掉的列表
        for ( auto kfdropweight : kfsetting->_necessary_list._objects )
        {
            // 判断条件
            auto ok = _kf_kernel->CheckCondition( player, &kfdropweight->_conditions );
            if ( ok )
            {
                RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机的列表
        switch ( kfsetting->_condition_type )
        {
        case KFDropSetting::MutexCondition:
            DropMutexCondition( player, kfsetting, outlist );
            break;
        case KFDropSetting::OverlayCondition:
            DropOverlayCondition( player, kfsetting, outlist );
            break;
        default:
            __LOG_ERROR__( "drop=[{}] conditiontype=[{}] error!", kfsetting->_id, kfsetting->_condition_type );
            break;
        }
    }

    void KFDropModule::DropMutexCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        bool ishaveconditiondrop = false;
        std::set<uint32> excludelist;
        for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
        {
            auto ok = _kf_kernel->CheckCondition( player, &kfdropweight->_conditions );
            if ( ok )
            {
                if ( !kfdropweight->_conditions.IsEmpty() )
                {
                    // 设置已经有了条件掉落
                    ishaveconditiondrop = true;
                }
                else
                {
                    // 如果是无条件掉落, 并且已经有了条件掉落, 排除这个无条件掉落
                    if ( ishaveconditiondrop )
                    {
                        excludelist.insert( kfdropweight->_id );
                    }
                }
            }
            else
            {
                excludelist.insert( kfdropweight->_id );
            }
        }

        auto kfdropweight = kfsetting->_rand_list.Rand( excludelist );
        if ( kfdropweight != nullptr )
        {
            RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }
    }

    void KFDropModule::DropOverlayCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        std::set<uint32> excludelist;
        for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
        {
            auto ok = _kf_kernel->CheckCondition( player, &kfdropweight->_conditions );
            if ( !ok )
            {
                excludelist.insert( kfdropweight->_id );
            }
        }

        auto kfdropweight = kfsetting->_rand_list.Rand( excludelist );
        if ( kfdropweight != nullptr )
        {
            RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }
    }

    void KFDropModule::RandDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropGroupWeight* kfdropweight, const char* function, uint32 line )
    {
        if ( kfdropweight->_drop_data_setting == nullptr )
        {
            return;
        }

        // 如果需要重置
        if ( kfdropweight->_is_clear_var && kfsetting->_is_drop_count )
        {
            player->UpdateData( __KF_STRING__( drop ), kfsetting->_id, __KF_STRING__( value ), KFEnum::Set, 0u );
        }

        auto kfdropdataweight = kfdropweight->_drop_data_setting->_drop_data_list.Rand();
        if ( kfdropdataweight == nullptr )
        {
            return;
        }

        auto dropdata = &kfdropdataweight->_drop_data;
        if ( dropdata->_data_name == __KF_STRING__( drop ) )
        {
            // 如果是掉落的话, 继续执行掉落逻辑
            DropLogic( player, dropdata->GetValue(), 1u, outlist, function, line );
        }
        else
        {
            outlist.push_back( dropdata );
        }
    }
}