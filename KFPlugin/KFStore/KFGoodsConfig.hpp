#ifndef __KF_GOODS_CONFIG_H__
#define __KF_GOODS_CONFIG_H__

#include "KFXmlReader/KFElementConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 商品配置
    class KFGoodsSetting : public KFIntSetting
    {
    public:
        // 商店id
        uint32 _store_id = 0u;

        // 商品数据
        KFElements _goods_data;

        // 原价
        KFElements _buy_price;

        // 上架时间
        uint64 _start_buy_time = 0u;

        // 下架时间
        uint64 _end_buy_time = 0u;

        // 折扣价
        KFElements _discount_price;

        // 折扣的开始时间
        uint64 _start_discount_time = 0u;

        // 折扣的结束时间
        uint64 _end_discount_time = 0u;

        // 限购刷新的时间id
        uint32 _limit_buy_time_id = 0u;

        // 限购的数量
        uint32 _limit_buy_count = 0u;

        // 商品组id
        uint32 _group_id = 0u;

        // 商品组权重
        uint32 _group_weight = 0u;

        // 刷新商品个数
        uint32 _refresh_count = 0u;
    public:
        // 判断是否在出售时间内
        bool CheckInBuyTime( uint64 nowtime ) const;

        // 判断是否在折扣时间内
        bool CheckInDiscountTime( uint64 nowtime ) const;

        // 是否限购
        bool IsLimitBuy() const;

        // 获得商品价钱
        const KFElements* CalcBuyPrice( uint64 nowtime ) const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFGoodsGroupWeight : public KFWeight
    {
    public:
        // 刷新数量
        uint32 _refresh_count = 0u;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFGoodsConfig : public KFConfigT< KFGoodsSetting >, public KFInstance< KFGoodsConfig >
    {
    public:
        KFGoodsConfig()
        {
            _file_name = "goods";
        }

        virtual void LoadAllComplete();

        // 随机一个商品
        const KFGoodsGroupWeight* RandGroupGoods( uint32 groupid, UInt32Set& excludelist );

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFGoodsSetting* kfsetting );

        // 清空配置
        virtual void ClearSetting();

    public:
        // 商品随权重组
        KFHashMap<uint32, uint32, KFWeightList<KFGoodsGroupWeight>>_goods_group_weight_list;
    };
}

#endif
