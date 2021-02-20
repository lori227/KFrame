#ifndef __KF_WEIGHT_H__
#define __KF_WEIGHT_H__

#include "KFGlobal/KFGlobal.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // 权重的基类
    class KFWeight
    {
    public:
        // 设置数值
        void SetValue( const std::string& value ) {}

    public:
        // 权重
        uint32 _weight = 0u;

        // 数值
        uint32 _value = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    template<class T>
    class KFWeightList
    {
        typedef std::shared_ptr<T> WeightPtr;
        typedef std::shared_ptr<const T> ConstWeightPtr;
    public:
        virtual ~KFWeightList()
        {
            Clear();
        }

        // 清空数据
        inline virtual void Clear()
        {
            _total_weight = 0u;
            _weight_data.clear();
        }

        // 创建随机权重
        // @value : 额外的权重数值
        // @weight : 额外的权重值
        WeightPtr Create( uint32 value, uint32 weight )
        {
            auto data = __MAKE_SHARED__( T );
            data->_value = value;
            data->_weight = weight;

            _total_weight += data->_weight;
            _weight_data.push_back( data );
            return data;
        }

        // 列表大小
        inline uint32 Size() const
        {
            return (uint32)_weight_data.size();
        }

        // 是否含有某数值
        // @value : 判断的数值
        inline bool Have( uint32 value ) const
        {
            for ( auto& data : _weight_data )
            {
                if ( data->_value == value && data->_weight != 0u )
                {
                    return true;
                }
            }

            return false;
        }

        // 随机
        const ConstWeightPtr Rand() const
        {
            if ( _total_weight > 0u )
            {
                auto weight = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( _total_weight );
                for ( auto& data : _weight_data )
                {
                    weight += data->_weight;
                    if ( rand < weight )
                    {
                        return data;
                    }
                }
            }

            return nullptr;
        }

        // 随机( 排除列表中的项目 )
        // @list : 需要排除的列表
        const ConstWeightPtr RandExcludeList( const UInt32Set& list ) const
        {
            // 新的权重列表
            uint32 total_weight = 0u;
            std::list< ConstWeightPtr > rand_list;

            for ( auto& data : _weight_data )
            {
                // 判断不在列表中
                auto iter = list.find( data->_value );
                if ( iter == list.end() )
                {
                    rand_list.push_back( data );
                    total_weight += data->_weight;
                }
            }

            if ( total_weight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( total_weight );
                for ( auto& data : rand_list )
                {
                    value += data->_weight;
                    if ( rand < value )
                    {
                        return data;
                    }
                }
            }

            return nullptr;
        }

        // 随机( 只包含列表中的项目 )
        // @list : 需要随机的列表
        const ConstWeightPtr RandInList( const UInt32Set& list ) const
        {
            // 新的权重列表
            uint32 total_weight = 0u;
            std::list< ConstWeightPtr > rand_list;

            for ( auto& data : _weight_data )
            {
                // 判断不在列表中
                auto iter = list.find( data->_value );
                if ( iter != list.end() )
                {
                    rand_list.push_back( data );
                    total_weight += data->_weight;
                }
            }

            if ( total_weight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( total_weight );
                for ( auto& data : rand_list )
                {
                    value += data->_weight;
                    if ( rand < value )
                    {
                        return data;
                    }
                }
            }

            return nullptr;
        }

        // 随机( 附加额外的权重列表 )
        // @include_list : 额外增加的权重列表
        const ConstWeightPtr RandExtend( UInt32Map& extend_list ) const
        {
            // 新的权重列表
            uint32 total_weight = _total_weight;
            for ( auto& iter : extend_list )
            {
                total_weight += iter.second;
            }

            if ( total_weight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( total_weight );
                for ( auto data : _weight_data )
                {
                    value += ( data->_weight + extend_list[ data->_value ] );
                    if ( rand < value )
                    {
                        return data;
                    }
                }
            }

            return nullptr;
        }

        // 随机( 添加列表中的项目 )
        // @value : 额外的权重数值
        // @weight : 额外的权重值
        const ConstWeightPtr RandExtend( uint32 value, uint32 weight ) const
        {
            UInt32Map extend_list;
            extend_list[ value ] = weight;
            return RandExtend( extend_list );
        }

        // 随机指定个数的选项(可重复)
        // @count : 随机个数
        const std::vector<ConstWeightPtr>& RandRepeat( uint32 count ) const
        {
            static std::vector<ConstWeightPtr> _result_list;
            _result_list.clear();

            for ( auto i = 0u; i < count; ++i )
            {
                auto result = Rand();
                if ( result != nullptr )
                {
                    _result_list.push_back( result );
                }
            }

            return _result_list;
        }

        // 随机指定个数的选项(不可重复)
        // @count : 随机个数
        const std::set<ConstWeightPtr>& RandNoRepeat( uint32 count ) const
        {
            static std::set<ConstWeightPtr> _result_list;
            _result_list.clear();

            auto rand_weight = 0u;
            for ( auto i = 0u; i < count; ++i )
            {
                auto left_weight = _total_weight - rand_weight;
                if ( left_weight == 0u )
                {
                    break;
                }

                auto weight = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( left_weight );
                for ( auto& data : _weight_data )
                {
                    // 已经在结果列表中
                    if ( _result_list.find( data ) != _result_list.end() )
                    {
                        continue;
                    }

                    weight += data->_weight;
                    if ( rand < weight )
                    {
                        _result_list.insert( data );
                        rand_weight += data->_weight;
                        break;
                    }
                }
            }

            return _result_list;
        }

    public:
        // 总权重
        uint32 _total_weight = 0u;

        // 权重数据
        std::vector<WeightPtr> _weight_data;
    };

}

#endif
