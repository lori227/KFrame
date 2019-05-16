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
    class KFPaySetting
    {
    public:
        // 充值id
        std::string _id;

        // 价钱( 单位: 分 )
        uint32 _price;

        // 购买得到的道具
        KFElements _buy_elements;

        // 首冲变量id
        uint32 _first_variable_id;

        // 首冲获得道具
        KFElements _first_elements;
    };

    class KFPayConfig : public KFConfig, public KFSingleton< KFPayConfig >
    {
    public:
        KFPayConfig()
        {
            _file = "pay.config";
        }

        // 加载配置
        bool LoadConfig();

        // 查找商品配置
        const KFPaySetting* FindPaySetting( const std::string& id ) const;

    private:
        // 商品列表
        KFHashMap< std::string, const std::string&, KFPaySetting > _pay_list;
    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_pay_config = KFPayConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}

#endif//__KF_STORE_CONFIG_H__
