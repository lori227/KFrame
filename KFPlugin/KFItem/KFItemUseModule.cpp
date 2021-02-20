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
    void KFItemUseModule::BindCheckItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function )
    {
        auto kffunction = _check_item_use_function.Create( itemtype );
        kffunction->_function = function;
    }

    void KFItemUseModule::UnBindCheckItemUseFunction( uint32 itemtype, const std::string& module )
    {
        _check_item_use_function.Remove( itemtype );
    }

    void KFItemUseModule::BindItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function )
    {
        auto kffunction = _item_use_function.Create( itemtype );
        kffunction->_function = function;
    }

    void KFItemUseModule::UnBindItemUseFunction( uint32 itemtype, const std::string& module )
    {
        _item_use_function.Remove( itemtype );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemUseModule::CheckCanUseItem( KFEntity* player, DataPtr kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting )
    {
        auto kffunction = _check_item_use_function.Find( kfitemsetting->_type );
        if ( kffunction == nullptr )
        {
            return true;
        }

        return kffunction->_function( player, kfitem, kfitemsetting, kftypesetting );
    }

    __KF_MESSAGE_FUNCTION__( KFItemUseModule::HandleUseItemReq, KFMsg::MsgUseItemReq )
    {
        // 判断是否有这个道具
        auto kfitem = kfentity->Find( kfmsg->name(), kfmsg->uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name  );
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfitemsetting == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemSettingNotExist, itemid );
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemSettingNotExist, itemid );
        }

        // 不能使用
        if ( kfitemsetting->_use_count == 0u )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemCanNotUse );
        }

        if ( !CheckCanUseItem( kfentity, kfitem, kfitemsetting, kftypesetting ) )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemCanNotUseStatus );
        }

        auto ok = UseItem( kfentity, kfitem, kfitemsetting, kftypesetting );
        if ( !ok )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemUseFailed );
        }

        UseCoseItem( kfentity, kfitem, kfitemsetting );
        _kf_display->SendToClient( kfentity, KFMsg::ItemUseOk, kfitemsetting->_id );
    }

    bool KFItemUseModule::UseItem( KFEntity* player, DataPtr kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting )
    {
        auto kffunction = _item_use_function.Find( kfitemsetting->_type );
        if ( kffunction == nullptr )
        {
            return false;
        }

        return kffunction->_function( player, kfitem, kfitemsetting, kftypesetting );
    }

    void KFItemUseModule::UseCoseItem( KFEntity* player, DataPtr kfitem, const KFItemSetting* kfitemsetting )
    {
        // 扣除数量
        auto usecount = kfitem->Get<uint32>( __STRING__( usecount ) );
        if ( usecount + 1u >= kfitemsetting->_use_count )
        {
            _kf_item->RemoveItemCount( player, kfitem, 1u );
        }
        else
        {
            player->UpdateObject( kfitem, __STRING__( usecount ), KFEnum::Add, 1u );
        }
    }
}