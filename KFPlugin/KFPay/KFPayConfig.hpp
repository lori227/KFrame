#ifndef __KF_PAY_CONFIG_H__
#define __KF_PAY_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

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

    class KFPayConfig : public KFStrConfigT< KFPaySetting >, public KFSingleton< KFPayConfig >
    {
    public:
        KFPayConfig( const std::string& file, bool isclear )
            : KFStrConfigT<KFPaySetting >( file, isclear )
        {
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFPaySetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_pay_config = KFPayConfig::Instance( "pay.xml", true );
    //////////////////////////////////////////////////////////////////////////
}

#endif//__KF_STORE_CONFIG_H__
