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
        // 刷新类型
        uint32 _refresh_type = 0u;

        // 刷新时间id
        uint32 _refresh_time_id = 0u;

        // 刷新的商品组数量
        std::list < std::tuple< uint32, uint32, uint32 > > _refresh_group_count;

        // 随机类型( 1 可重复商品, 2 不可重复商品 )
        uint32 _random_type = 0u;

        // 商品列表
        UInt32Set _goods_list;

        // 刷新花费列表
        UInt32Vector _refresh_cost_list;

        // 手动刷新是否重置时间
        bool _is_refresh_reset_time = false;
    public:
        // 是否有该商品
        bool IsHaveGoods( uint32 goodsid ) const;
    };


    class KFStoreConfig : public KFConfigT< KFStoreSetting >, public KFInstance< KFStoreConfig >
    {
    public:
        KFStoreConfig()
        {
            _file_name = "store";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFStoreSetting* kfsetting );
    };
}

#endif