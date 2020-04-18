#ifndef __KF_COUNT_COST_CONFIG_H__
#define __KF_COUNT_COST_CONFIG_H__

#include "KFConfig.h"
#include "KFCore/KFElement.h"
#include "KFElementConfig.h"

namespace KFrame
{
    class KFCountCostData
    {
    public:
        // 最小次数
        uint32 _min_count = 1u;

        // 最大次数
        uint32 _max_count = 1u;

        // 价格
        KFElements _cost_elements;
    };

    class KFCountCostSetting : public KFIntSetting
    {
    public:
        // 刷新时间id( 为0表示固定价格 )
        uint32 _refresh_time_id = 0;

        // 花费列表
        KFVector< KFCountCostData > _count_cost_list;

    public:
        // 获得花费
        const KFElements* GetCost( uint32 count ) const
        {
            if ( _count_cost_list._objects.empty() )
            {
                return nullptr;
            }

            for ( auto countcostdata : _count_cost_list._objects )
            {
                if ( count >= countcostdata->_min_count && count <= countcostdata->_max_count )
                {
                    return &countcostdata->_cost_elements;
                }
            }

            return &_count_cost_list._objects.back()->_cost_elements;
        }
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFCountCostConfig : public KFConfigT< KFCountCostSetting >, public KFInstance< KFCountCostConfig >
    {
    public:
        KFCountCostConfig()
        {
            _key_name = "CostId";
            _file_name = "countcost";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                for ( auto countcostdata : kfsetting->_count_cost_list._objects )
                {
                    KFElementConfig::Instance()->ParseElement( countcostdata->_cost_elements, countcostdata->_cost_elements._str_element, __FILE__, kfsetting->_id );
                }
            }
        }

        // 获得花费
        std::tuple<const KFElements*, uint32> GetCostByCount( uint32 costid, uint32 count ) const
        {
            auto kfsetting = _settings.Find( costid );
            if ( kfsetting == nullptr )
            {
                return std::make_tuple( nullptr, 0u );
            }

            auto costelements = kfsetting->GetCost( count );
            return std::make_tuple( costelements, kfsetting->_refresh_time_id );
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFCountCostSetting* kfsetting )
        {
            kfsetting->_refresh_time_id = xmlnode.GetUInt32( "RefreshTimeId" );

            auto countcostdata = __KF_NEW__( KFCountCostData );
            countcostdata->_min_count = xmlnode.GetUInt32( "MinCount" );
            countcostdata->_max_count = xmlnode.GetUInt32( "MaxCount" );
            countcostdata->_cost_elements._str_element = xmlnode.GetString( "Cost" );
            kfsetting->_count_cost_list.Add( countcostdata );
        }
    };
}

#endif