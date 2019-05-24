#ifndef __KF_STORE_CONFIG_H__
#define __KF_STORE_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 商品配置
    class KFStoreSetting
    {
    public:
        // 商品id
        uint32 _id = 0u;

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

    class KFStoreConfig : public KFConfig, public KFSingleton< KFStoreConfig >
    {
    public:
        KFStoreConfig() = default;
        ~KFStoreConfig() = default;

        // 加载配置
        bool LoadConfig( const std::string& file );

        // 查找商品配置
        const KFStoreSetting* FindStoreSetting( uint32 id ) const;

    public:

        KFXml* _store_xml = nullptr;

        // 商店版本号
        std::string _version;

        // 商品列表
        KFHashMap< uint32, uint32, KFStoreSetting > _store_list;
    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_store_config = KFStoreConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}

#endif//__KF_STORE_CONFIG_H__
