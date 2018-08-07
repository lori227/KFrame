#include "KFItemModule.h"
#include "KFItemConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFItemModule::KFItemModule()
    {
        _kf_component = nullptr;
    }

    KFItemModule::~KFItemModule()
    {
    }

    void KFItemModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_item_config, true );
    }

    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddDataFunction( __KF_STRING__( item ), this, &KFItemModule::OnAddItemCallBack );
        _kf_component->RegisterRemoveDataFunction( __KF_STRING__( item ), this, &KFItemModule::OnRemoveItemCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( item ), __KF_STRING__( count ), this, &KFItemModule::OnItemCountUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_component->RegisterAddAgentFunction( __KF_STRING__( item ), this, &KFItemModule::AddItemAgentData );
        _kf_component->RegisterCheckAgentFunction( __KF_STRING__( item ), this, &KFItemModule::CheckItemAgentData );
        _kf_component->RegisterRemoveAgentFunction( __KF_STRING__( item ), this, &KFItemModule::RemoveItemAgentData );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->RegisterEnterFunction( this, &KFItemModule::OnEnterStartItemTimer );
        _kf_player->RegisterLeaveFunction( this, &KFItemModule::OnLeaveStopItemTimer );

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ, &KFItemModule::HandleUseItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();

        _kf_component->UnRegisterAddDataFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveDataFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterUpdateDataFunction( __KF_STRING__( item ), __KF_STRING__( count ) );

        _kf_component->UnRegisterAddAgentFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterCheckAgentFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveAgentFunction( __KF_STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
    }

    void KFItemModule::OnEnterStartItemTimer( KFEntity* player )
    {
        CheckStartItemTimer( player );
    }

    void KFItemModule::OnLeaveStopItemTimer( KFEntity* player )
    {
        __UNREGISTER_OBJECT_TIMER__( player->GetKeyID() );
    }

    void KFItemModule::CheckStartItemTimer( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );

        // 检查所有衣服, 找到时间最少的一个衣服
        uint32 _min_valid_time = 0xFFFFFFFF;

        auto kfitem = kfitemrecord->FirstData();
        while ( kfitem != nullptr )
        {
            auto validtime = kfitem->GetValue( __KF_STRING__( time ) );
            if ( validtime != 0 && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }

            kfitem = kfitemrecord->NextData();
        }

        if ( _min_valid_time == 0xFFFFFFFF )
        {
            return;
        }

        // 如果已经有超时道具
        if ( _min_valid_time < KFGlobal::Instance()->_real_time )
        {
            _min_valid_time = KFGlobal::Instance()->_real_time;
        }

        // 转换成间隔时间(毫秒) 启动定时器
        auto intervaltime = ( _min_valid_time - KFGlobal::Instance()->_real_time + 1 ) * 1000;
        __REGISTER_LIMIT_TIMER__( player->GetKeyID(), intervaltime, 1, &KFItemModule::OnTimerCheckItemValidTime );
    }

    void KFItemModule::RemoveValidTimeoutItem( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );
        if ( kfitemrecord == nullptr )
        {
            return;
        }

        // 查找时间道具
        std::map< KFData*, uint64 > timeitem;
        auto kfitem = kfitemrecord->FirstData();
        while ( kfitem != nullptr )
        {
            auto validtime = kfitem->GetValue( __KF_STRING__( time ) );
            if ( validtime != 0 )
            {
                timeitem.insert( std::make_pair( kfitem, validtime ) );
            }

            kfitem = kfitemrecord->NextData();
        }

        // 删除时间道具
        for ( auto& iter : timeitem )
        {
            auto* kfitem = iter.first;
            if ( KFGlobal::Instance()->_real_time < iter.second )
            {
                continue;
            }

            // 删除道具
            player->RemoveData( kfitemrecord, kfitem->GetKeyID() );
        }
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerCheckItemValidTime )
    {
        auto player = _kf_player->FindPlayer( objectid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        // 删除过期的时装
        RemoveValidTimeoutItem( player );

        // 检查是否需要开启定时器
        CheckStartItemTimer( player );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_AGENT_FUNCTION__( KFItemModule::AddItemAgentData )
    {
        auto configid = kfagent->_config_id;
        if ( configid == _invalid_int )
        {
            return __LOG_ERROR__( KFLogEnum::System, "[{}] item id = 0!", kfagent->_string );
        }

        auto itemsetting = _kf_item_config->FindItemSetting( configid );
        if ( itemsetting == nullptr )
        {
            return __LOG_ERROR__( KFLogEnum::System, "[{}] item id = 0!", kfagent->_string );
        }

        auto kfagentvalue = kfagent->FindAgentValue( __KF_STRING__( count ) );
        if ( kfagentvalue == nullptr )
        {
            return __LOG_ERROR__( KFLogEnum::System, "[{}] item count = null!", kfagent->_string );
        }

        auto itemcount = _kf_kernel->CalcAgentValue( kfagentvalue, multiple );

        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );
        if ( kfitemrecord == nullptr )
        {
            return __LOG_ERROR__( KFLogEnum::System, "[{}] player[{}] item record = null!", kfagent->_string, player->GetKeyID() );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 只有真正的物品道具才创建实体
        if ( itemsetting->IsRealItem() )
        {
            do
            {
                switch ( itemsetting->_overlay_type )
                {
                case KFItemEnum::OverlayTime:
                    itemcount = AddOverlayTimeItemData( player, kfitemrecord, kfagent, itemsetting, itemcount );
                    break;
                default:
                    itemcount = AddOverlayCountItemData( player, kfitemrecord, kfagent, itemsetting, itemcount );
                    break;
                }
            } while ( itemcount > 0 );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallItemLuaFunction( player, itemsetting, KFItemEnum::AddFunction );
    }

    uint32 KFItemModule::AddOverlayTimeItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count )
    {
        auto datasetting = kfparent->GetDataSetting();

        std::list< KFData* > finditem;
        kfparent->FindData( datasetting->_config_key_name, itemsetting->_id, finditem );

        // 没有找到, 创建新的物品
        if ( finditem.empty() )
        {
            return AddNewItemData( player, kfparent, kfagent, itemsetting, count );
        }

        // 存在该物品, 加上有效时间
        uint32 totaltime = itemsetting->_valid_time * count;

        auto kfitem = finditem.front();
        player->UpdateData( kfitem, __KF_STRING__( time ), KFOperateEnum::Add, totaltime );
        return 0;
    }

    uint32 KFItemModule::AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count )
    {
        auto datasetting = kfparent->GetDataSetting();

        // 叠加数量 > 1 , 查找
        if ( itemsetting->_overlay_count > 1 )
        {
            std::list< KFData* > finditem;
            kfparent->FindData( datasetting->_config_key_name, itemsetting->_id, finditem );

            for ( auto kfitem : finditem )
            {
                auto oldcount = kfitem->GetValue< uint32 >( __KF_STRING__( count ) );
                oldcount = __MIN__( oldcount, itemsetting->_overlay_count );
                auto canadd = itemsetting->_overlay_count - oldcount;
                uint32 addcount = __MIN__( canadd, count );
                if ( addcount == 0 )
                {
                    continue;
                }

                player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Add, addcount );

                // 添加guid属性
                kfagent->AddValue( datasetting->_key_name, KFUtility::ToString( kfitem->GetKeyID() ) );
                count -= addcount;
                if ( count == 0 )
                {
                    return 0;
                }
            }
        }

        // 添加新的物品
        return AddNewItemData( player, kfparent, kfagent, itemsetting, count );
    }

    uint32 KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count )
    {
        auto datasetting = kfparent->GetDataSetting();
        uint32 addcount = __MIN__( count, itemsetting->_overlay_count );

        auto kfitem = _kf_kernel->CreateObject( datasetting );
        kfitem->SetValue( datasetting->_config_key_name, itemsetting->_id );
        kfitem->SetValue( __KF_STRING__( count ), addcount );

        // 设置属性
        for ( auto iter : kfagent->_datas._objects )
        {
            auto name = iter.first;
            if ( name == __KF_STRING__( count ) || name == datasetting->_key_name )
            {
                continue;
            }

            auto value = iter.second->GetValue();
            kfitem->SetValue( name, value );
        }

        // guid
        auto itemguid = KFUtility::Make64Guid( KFGlobal::Instance()->_app_id );
        kfitem->SetValue( datasetting->_key_name, itemguid );

        // 把guid添加进属性表中
        kfagent->AddValue( datasetting->_key_name, KFUtility::ToString< uint64 >( itemguid ) );

        // 添加物品
        player->AddData( kfparent, itemguid, kfitem );
        return count - addcount;
    }

    KFData* KFItemModule::MoveItem( KFEntity* player, KFData* kfsource, KFData* kftarget, uint32 itemcount  )
    {
        auto datasetting = kfsource->GetDataSetting();
        if ( kftarget == nullptr )
        {
            kftarget = _kf_kernel->CreateObject( datasetting );
        }

        kftarget->CopyFrom( kfsource );

        auto sourceguid = kfsource->GetKeyID();
        auto sourcecount = kfsource->GetValue( __KF_STRING__( count ) );

        // 如果堆叠物品取出来一部分, 判断数量
        if ( itemcount < sourcecount )
        {
            // 设置数量
            kftarget->SetValue( __KF_STRING__( count ), itemcount );

            auto configid = kfsource->GetValue< uint32 >( datasetting->_config_key_name );
            auto newitemguid = KFUtility::Make64Guid( KFGlobal::Instance()->_app_id );
            kftarget->SetKeyID( newitemguid );

            player->UpdateData( kfsource, __KF_STRING__( count ), KFOperateEnum::Dec, itemcount );
        }
        else
        {
            player->RemoveData( kfsource->GetParent(), sourceguid );
        }

        return kftarget;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        // 调用脚本
        auto itemcount = kfdata->GetValue<uint32>();
        if ( itemcount > 0 )
        {
            return;
        }

        // 数量为0, 删除该物品
        player->RemoveData( __KF_STRING__( item ), key );
    }

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        auto datasetting = kfdata->GetDataSetting();

        auto itemid = kfdata->GetValue< uint32 >( datasetting->_config_key_name );
        auto itemsetting = _kf_item_config->FindItemSetting( itemid );
        if ( itemsetting == nullptr )
        {
            return;
        }

        // 时间道具 开启有效时间
        OnAddItemDataStartValidTime( player, kfdata, key, itemsetting );
    }

    void KFItemModule::OnAddItemDataStartValidTime( KFEntity* player, KFData* kfdata, uint64 key, const KFItemSetting* itemsetting )
    {
        // 已经存在时间, 不设置有效时间
        auto validtime = kfdata->GetValue( __KF_STRING__( time ) );
        if ( validtime != _invalid_int )
        {
            return CheckStartItemTimer( player );
        }

        // 判断时间
        if ( itemsetting->_time_type == KFItemEnum::AddTimeStart )
        {
            validtime = itemsetting->_valid_time + KFGlobal::Instance()->_real_time;
            player->UpdateData( kfdata, __KF_STRING__( time ), KFOperateEnum::Set, validtime );
            CheckStartItemTimer( player );
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemModule::OnRemoveItemCallBack )
    {
        auto datasetting = kfdata->GetDataSetting();

        auto itemid = kfdata->GetValue< uint32 >( datasetting->_config_key_name );
        auto itemsetting = _kf_item_config->FindItemSetting( itemid );
        if ( itemsetting == nullptr )
        {
            return;
        }

        CallItemLuaFunction( player, itemsetting, KFItemEnum::RemoveFunction );
    }

    void KFItemModule::CallItemLuaFunction( KFEntity* player, const KFItemSetting* itemsetting, uint32 functiontype )
    {
        if ( itemsetting->_lua_file.empty() )
        {
            return;
        }

        auto& luafunction = itemsetting->GetFunction( functiontype );
        if ( luafunction.empty() )
        {
            return;
        }

        auto playerid = static_cast<uint32>( player->GetKeyID() );
        _kf_lua->CallFunction( itemsetting->_lua_file, luafunction, playerid, itemsetting->_id );
    }

    __KF_CHECK_AGENT_FUNCTION__( KFItemModule::CheckItemAgentData )
    {
        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );
        if ( kfitemrecord == nullptr )
        {
            return false;
        }

        auto datasetting = kfitemrecord->GetDataSetting();

        // 获取item列表
        std::list< KFData* > finditem;
        kfitemrecord->FindData( datasetting->_config_key_name, kfagent->_config_id, finditem );
        if ( finditem.empty() )
        {
            return false;
        }

        // 判断数量
        auto itemcount = kfagent->GetValue( __KF_STRING__( count ) );

        uint32 totalcount = 0;
        for ( auto kfitem : finditem )
        {
            // 判断其他属性
            auto iseligibility = kfagent->IsEligibilityData( kfitem, __KF_STRING__( count ) );
            if ( iseligibility )
            {
                // 属性满足, 累计数量
                totalcount += kfitem->GetValue< uint32 >( __KF_STRING__( count ) );
                if ( totalcount >= itemcount )
                {
                    return true;
                }
            }
        }

        return false;
    }

    __KF_REMOVE_AGENT_FUNCTION__( KFItemModule::RemoveItemAgentData )
    {
        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );
        if ( kfitemrecord == nullptr )
        {
            return;
        }

        auto datasetting = kfitemrecord->GetDataSetting();

        // 获取item列表
        std::list< KFData* > finditem;
        kfitemrecord->FindData( datasetting->_config_key_name, kfagent->_config_id, finditem );
        if ( finditem.empty() )
        {
            return;
        }

        // 判断数量
        auto itemcount = kfagent->GetValue( __KF_STRING__( count ) );

        uint32 totalcount = 0;
        for ( auto kfitem : finditem )
        {
            // 判断其他属性
            auto iseligibility = kfagent->IsEligibilityData( kfitem, __KF_STRING__( count ) );
            if ( iseligibility )
            {
                auto removecount = __MIN__( itemcount, kfitem->GetValue< uint32 >( __KF_STRING__( count ) ) );

                player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Dec, removecount );
                itemcount -= removecount;
                if ( itemcount == 0 )
                {
                    break;
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleUseItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemReq );

        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );

        // 判断是否有这个道具
        auto kfitem = kfitemrecord->FindData( kfmsg.itemguid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemNotExist );
        }

        auto itemid = kfitem->GetValue< uint32 >( __KF_STRING__( id ) );
        auto kfsetting = _kf_item_config->FindItemSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataError, itemid );
        }

        if ( !kfsetting->CheckCanUse() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        // 删除道具
        player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Dec, 1 );

        // 添加奖励
        if ( kfsetting->_reward_type == KFItemEnum::ConfigReward )
        {
            player->AddAgentData( &kfsetting->_rewards, 1.0f, true, __FUNC_LINE__ );
        }

        // 调用lua脚本
        CallItemLuaFunction( player, kfsetting, KFItemEnum::UseFunction );
    }
}