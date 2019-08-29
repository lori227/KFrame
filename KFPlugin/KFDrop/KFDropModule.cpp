#include "KFDropModule.hpp"

namespace KFrame
{


    DropDataList& KFDropModule::DropElement( KFEntity* player, uint32 dropid, uint32 count, bool showclient, const char* function, uint32 line )
    {
        auto& _drops = DropLogic( player, dropid, count, function, line );
        for ( auto kfdropdata : _drops )
        {
            player->AddElement( &kfdropdata->_elements, showclient, function, line );
        }

        return _drops;
    }

    DropDataList& KFDropModule::DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line )
    {
        static DropDataList _drops;
        _drops.clear();

        // 掉落逻辑
        Drop( player, dropid, count, _drops, function, line );

        return _drops;
    }

    void KFDropModule::Drop( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), dropid, count );
        auto kfsetting = KFDropConfig::Instance()->FindSetting( dropid );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] can't find setting!", dropid );
        }

        for ( auto i = 0u; i < count; ++i )
        {
            Drop( player, kfsetting, outlist );
        }
    }

    void KFDropModule::Drop( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        AddDropVarCount( player, kfsetting );

        // 必掉的列表
        for ( auto kfdropweight : kfsetting->_necessary_list._objects )
        {
            // 判断条件
            auto ok = _kf_kernel->CheckCondition( player, &kfdropweight->_conditions );
            if ( !ok )
            {
                continue;
            }

            AddDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }

        // 随机的列表
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
            AddDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }
    }

    void KFDropModule::AddDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropWeight* kfdropweight, const char* function, uint32 line )
    {
        static const std::string _drop_logic_name = "drop";

        // 如果需要重置
        if ( kfdropweight->_is_clear_var )
        {
            ResetDropVarCount( player, kfsetting );
        }

        auto dropdata = &kfdropweight->_drop_data;
        if ( dropdata->_data_name == _drop_logic_name )
        {
            // 如果是掉落的话, 继续执行掉落逻辑
            Drop( player, dropdata->GetValue(), 1u, outlist, function, line );
        }
        else
        {
            outlist.push_back( &kfdropweight->_drop_data );
        }
    }

    void KFDropModule::AddDropVarCount( KFEntity* player, const KFDropSetting* kfsetting )
    {
        if ( kfsetting->_var_id == 0u )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( var ), kfsetting->_var_id, __KF_STRING__( value ), KFEnum::Add, 1u );
    }

    void KFDropModule::ResetDropVarCount( KFEntity* player, const KFDropSetting* kfsetting )
    {
        if ( kfsetting->_var_id == 0u )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( var ), kfsetting->_var_id, __KF_STRING__( value ), KFEnum::Set, 0u );
    }

}