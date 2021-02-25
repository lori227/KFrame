#include "KFItemUseModule.hpp"

namespace KFrame
{
    namespace KFItemUseEnum
    {
        enum EConstDefine
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            UseInAll = 0,		// 任何都能使用
            UseInNormal = 1,	// 在普通状态中使用
            UseInFight = 2,		// 在战斗中使用
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////
        };
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFItemUseModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_USE_ITEM_REQ, &KFItemUseModule::HandleUseItemReq );
    }

    void KFItemUseModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemUseModule::BindCheckItemUseFunction( uint32 item_type, KFModule* module, KFItemUseFunction& function )
    {
        auto function_data = _check_item_use_function.Create( item_type );
        function_data->SetFunction( module, function );
    }

    void KFItemUseModule::UnBindCheckItemUseFunction( uint32 item_type, KFModule* module )
    {
        _check_item_use_function.Remove( item_type );
    }

    void KFItemUseModule::BindItemUseFunction( uint32 item_type, KFModule* module, KFItemUseFunction& function )
    {
        auto function_data = _item_use_function.Create( item_type );
        function_data->SetFunction( module, function );
    }

    void KFItemUseModule::UnBindItemUseFunction( uint32 item_type, KFModule* module )
    {
        _item_use_function.Remove( item_type );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemUseModule::CheckCanUseItem( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting )
    {
        auto function_data = _check_item_use_function.Find( item_setting->_type );
        if ( function_data == nullptr )
        {
            return true;
        }

        return function_data->CallEx<bool>( player, item_data, item_setting, item_type_setting );
    }

    __KF_MESSAGE_FUNCTION__( KFItemUseModule::HandleUseItemReq, KFMsg::MsgUseItemReq )
    {
        // 判断是否有这个道具
        auto item_data = entity->Find( kfmsg->name(), kfmsg->uuid() );
        if ( item_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemDataNotExist );
        }

        auto item_id = item_data->Get<uint32>( item_data->_data_setting->_config_key_name  );
        auto item_setting = KFItemConfig::Instance()->FindSetting( item_id );
        if ( item_setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSettingNotExist, item_id );
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSettingNotExist, item_id );
        }

        // 不能使用
        if ( item_setting->_use_count == 0u )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemCanNotUse );
        }

        if ( !CheckCanUseItem( entity, item_data, item_setting, item_type_setting ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemCanNotUseStatus );
        }

        auto ok = UseItem( entity, item_data, item_setting, item_type_setting );
        if ( !ok )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemUseFailed );
        }

        UseCostItem( entity, item_data, item_setting );
        _kf_display->SendToClient( entity, KFMsg::ItemUseOk, item_setting->_id );
    }

    bool KFItemUseModule::UseItem( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting )
    {
        auto function_data = _item_use_function.Find( item_setting->_type );
        if ( function_data == nullptr )
        {
            return false;
        }

        return function_data->CallEx<bool>( player, item_data, item_setting, item_type_setting );
    }

    void KFItemUseModule::UseCostItem( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting )
    {
        // 扣除数量
        auto use_count = item_data->Get<uint32>( __STRING__( usecount ) );
        if ( use_count + 1u >= item_setting->_use_count )
        {
            _kf_item->RemoveItemCount( player, item_data, 1u );
        }
        else
        {
            player->UpdateObject( item_data, __STRING__( usecount ), KFEnum::Add, 1u );
        }
    }
}