#ifndef __KF_PAY_CONFIG_H__
#define __KF_PAY_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 充值配置
    class KFPaySetting : public KFStrSetting
    {
    public:
        // 价钱( 单位: 分 )
        uint32 _price;

        // 购买得到的道具
        KFElements _buy_elements;

        // 首冲变量id
        uint32 _first_variable_id;

        // 首冲获得道具
        KFElements _first_elements;
    };

    class KFPayConfig : public KFConfigT< KFPaySetting >, public KFInstance< KFPayConfig >
    {
    public:
        KFPayConfig()
        {
            _file_name = "pay";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFPaySetting* kfsetting )
        {
            kfsetting->_price = xmlnode.ReadUInt32( "Price" );

            auto strbuyelement = xmlnode.ReadString( "BuyElement" );
            kfsetting->_buy_elements.Parse( strbuyelement, __FUNC_LINE__ );

            kfsetting->_first_variable_id = xmlnode.ReadUInt32( "FirstVarId" );

            auto strfirstelement = xmlnode.ReadString( "FirstElement" );
            kfsetting->_first_elements.Parse( strfirstelement, __FUNC_LINE__ );
        }
    };
}

#endif
