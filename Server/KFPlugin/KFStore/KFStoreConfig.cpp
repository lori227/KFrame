#include "KFStoreConfig.h"

namespace KFrame
{
    KFStoreConfig::KFStoreConfig()
    {

    }

    KFStoreConfig::~KFStoreConfig()
    {

    }

    bool KFStoreConfig::LoadConfig()
    {
        _version = 0;
        _store_setting.Clear();
        _client_show.Clear();

        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFStoreSetting );
                kfsetting->_id = xmlnode.GetUInt32( "Id" );

                kfsetting->_type = xmlnode.GetUInt32( "Type" );
                kfsetting->_item_type = xmlnode.GetUInt32( "ItemType" );
                kfsetting->_buy_max_num = xmlnode.GetUInt32( "BuyMaxNum" );

                auto strbuyitem = xmlnode.GetString( "BuyItem" );
                kfsetting->_buy_item.ParseFromString( strbuyitem, __FUNC_LINE__ );

                StringSplit( kfsetting->_cost_item, xmlnode.GetString( "Money" ), "money" );
                StringSplit( kfsetting->_cost_item, xmlnode.GetString( "Diamond" ), "diamond" );
                StringSplit( kfsetting->_discount_item, xmlnode.GetString( "DiscountMoney" ), "money" );
                StringSplit( kfsetting->_discount_item, xmlnode.GetString( "DiscountDiamond" ), "diamond" );

                kfsetting->_start_discount_time = KFDate::TimeFormate( xmlnode.GetString( "StartDiscountTime" ) );
                kfsetting->_end_discount_time = KFDate::TimeFormate( xmlnode.GetString( "EndDiscountTime" ) );
                kfsetting->_start_buy_time = KFDate::TimeFormate( xmlnode.GetString( "StartBuyTime" ) );
                kfsetting->_end_buy_time = KFDate::TimeFormate( xmlnode.GetString( "EndBuyTime" ) );
                kfsetting->_limit_type = xmlnode.GetUInt32( "LimitType" );
                kfsetting->_limit_count = xmlnode.GetUInt32( "LimitCount" );
                kfsetting->_cost_give = xmlnode.GetString( "CostGive" );
                kfsetting->_max_owm_ = xmlnode.GetUInt32( "MaxOwn" );
                kfsetting->_give_mail_id = xmlnode.GetUInt32( "GiveMailid" );
                kfsetting->_return_mail_id = xmlnode.GetUInt32( "ReturnMailId" );
                kfsetting->_give_friend_liness = xmlnode.GetUInt32( "GiveFriendLiness" );


                _version = xmlnode.GetUInt32( "Version" );
                _store_setting.Insert( kfsetting->_id, kfsetting );

                // 热更新消息包
                MakePBStore( xmlnode );

                xmlnode.NextNode();
            }
            _client_show.set_version( _version );
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    void KFStoreConfig::MakePBStore( KFNode& xmlnode )
    {
        static const char* _keys[] =
        {
            "Id",
            "Money",
            "Diamond",
            "DiscountMoney",
            "DiscountDiamond",
            "StartDiscountTime",
            "EndDiscountTime",
            "StartBuyTime",
            "EndBuyTime",
            "LimitType",
            "LimitCount",
            "MaxOwn",
        };
        static uint32 _count = std::extent<decltype( _keys )>::value;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        auto pbstore = _client_show.mutable_stores()->add_store();
        for ( auto i = 0u; i < _count; ++i )
        {
            auto name = _keys[ i ];
            auto value = xmlnode.GetString( _keys[ i ] );

            auto pbdata = pbstore->add_data();
            pbdata->set_name( name );
            pbdata->set_value( value );
        }
    }

    bool KFStoreConfig::StringSplit( KFMap< std::string, const std::string&, KFAgents >& des, std::string src, std::string key )
    {
        if ( src.empty() )
        {
            return true;
        }

        auto kfagents = des.Create( key );
        if ( !kfagents->ParseFromString( src, __FUNC_LINE__ ) )
        {
            __LOG_ERROR__( KFLogEnum::System, "Parse StoreConfig Error key:{},src:{}", key, src );
            return false;
        }

        return true;
    }

    const KFStoreSetting* KFStoreConfig::FindStoreSetting( uint32 id ) const
    {
        return _store_setting.Find( id );
    }

    const KFMsg::MsgQueryStoreInfoAck* KFStoreConfig::GetStoreClientShowInfo() const
    {
        return &_client_show;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFStoreSetting::CheckDiscount( uint64 nowtime ) const
    {
        if ( _invalid_int == _start_discount_time || _invalid_int == _end_discount_time )
        {
            return false;
        }

        return KFDate::CheckInTime( _start_discount_time, _end_discount_time, nowtime );
    }

    bool KFStoreSetting::CheckGive() const
    {
        return !_cost_give.empty();
    }


    bool KFStoreSetting::CheckParam( const std::string& buytype, KFStoreEnum::StoreOperaType operatype, uint32 num, uint64 nowtime ) const
    {
        if ( num > _buy_max_num )
        {
            return false;
        }

        switch ( operatype )
        {
        case KFrame::KFStoreEnum::Buy: {
            return CheckCostParam( buytype, nowtime );
        }
        break;
        case KFrame::KFStoreEnum::Give:
        {
            if ( !CheckGive() || _cost_give != buytype )
            {
                return false;
            }
            return CheckCostParam( buytype, nowtime );
        }
        break;
        default:
            return false;
            break;
        }
        return false;
    }


    const KFAgents* KFStoreSetting::FindBuyCostAgents( const std::string& buytype, uint64 nowtime, uint32 num ) const
    {
        if ( CheckDiscount( nowtime ) )
        {

            return _discount_item.Find( buytype );
        }

        return _cost_item.Find( buytype );
    }

    const KFAgents* KFStoreSetting::FindGiveCostAgents( uint64 nowtime ) const
    {
        if ( CheckDiscount( nowtime ) )
        {
            return _discount_item.Find( _cost_give );
        }

        return _cost_item.Find( _cost_give );
    }

    bool KFStoreSetting::CheckCostParam( const std::string& buytype, uint64 nowtime ) const
    {
        if ( CheckDiscount( nowtime ) )
        {
            return _discount_item.Find( buytype ) != nullptr;
        }
        else
        {
            return _cost_item.Find( buytype ) != nullptr;
        }
    }

    bool KFStoreSetting::CheckOutOfData( uint64 nowtime ) const
    {
        if ( _invalid_int == _start_buy_time || _invalid_int == _end_buy_time )
        {
            return true;
        }

        return KFDate::CheckInTime( _start_buy_time, _end_buy_time, nowtime );
    }
}