#ifndef __KF_STORE_CONFIG_H__
#define __KF_STORE_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 商品配置
    class KFStoreSetting : public KFIntSetting
    {
    public:
        // 商品类型
        std::string _data_name;

        // 购买得到的道具
        KFElements _buy_elements;

        // 购买花费的道具
        KFHashMap< std::string, const std::string&, KFElements > _cost_elements;
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // 折扣的开始时间
        uint64 _start_discount_time = 0u;

        // 折扣的结束时间
        uint64 _end_discount_time = 0u;

        // 折扣后的价格
        KFHashMap< std::string, const std::string&, KFElements > _discount_elements;
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // 购买限制类型
        std::string _buy_limit_type;

        // 购买限制数量
        uint32 _buy_limit_count = 0u;
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // 商品上架的商店类型
        uint32 _store_type = 0u;

        // 单次购买数量上限
        uint32 _buy_max_count = 0u;

        // 最大拥有限制数( 比如皮肤之类, 拥有就不能再次购买 )
        uint32 _max_own_count = 0u;
        /////////////////////////////////////////////////////////////////////////////////////////////////

    public:
        // 查找商品价格
        const KFElements* FindCostElements( const std::string& buytype, uint64 nowtime ) const;

        // 判断是否在折扣时间内
        bool CheckInDiscount( uint64 nowtime ) const;
    };

    class KFStoreConfig : public KFConfigT< KFStoreSetting >, public KFInstance< KFStoreConfig >
    {
    public:
        KFStoreConfig()
        {
            _file_name = "store";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFStoreSetting* kfsetting );
    };
}

#endif//__KF_STORE_CONFIG_H__
