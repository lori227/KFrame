#ifndef __KF_WEIGHT_H__
#define __KF_WEIGHT_H__

#include "KFMemory/KFMalloc.h"
#include "KFGlobal/KFGlobal.h"

namespace KFrame
{
    // 权重的基类
    class KFWeight
    {
    public:
        // 设置数值
        void SetValue( const std::string& value ) {}

    public:
        // 权重
        uint32 _weight = 0u;

        // id
        uint32 _id = 0u;
    };

    template< class T >
    class KFWeightList
    {
    public:
        ~KFWeightList()
        {
            Clear();
        }

        // 清空数据
        void Clear()
        {
            _total_weight = 0u;
            for ( auto data : _weight_data )
            {
                __KF_DELETE__( T, data );
            }
            _weight_data.clear();
        }

        T* Create( uint32 id, uint32 weight )
        {
            auto data = __KF_NEW__( T );
            data->_id = id;
            data->_weight = weight;

            _total_weight += data->_weight;
            _weight_data.push_back( data );
            return data;
        }

        // 随机
        const T* Rand() const
        {
            if ( _total_weight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( _total_weight );
                for ( auto data : _weight_data )
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

        // 随机( 排除列表中的项目 )
        const T* Rand( std::set< uint32 >& excludelist ) const
        {
            // 新的权重列表
            uint32 totalweight = 0u;
            std::list< T* > randlist;
            for ( auto data : _weight_data )
            {
                // 判断不在列表中
                if ( excludelist.find( data->_id ) == excludelist.end() )
                {
                    randlist.push_back( data );
                    totalweight += data->_weight;
                }
            }

            if ( totalweight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( totalweight );
                for ( auto data : randlist )
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

        // 随机( 添加列表中的项目 )
        const T* Rand( std::map< uint32, uint32 >& includelist ) const
        {
            // 新的权重列表
            uint32 totalweight = _total_weight;
            for ( auto& iter : includelist )
            {
                totalweight += iter.second;
            }

            if ( totalweight != 0u )
            {
                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( totalweight );
                for ( auto data : _weight_data )
                {
                    value += ( data->_weight + includelist[ data->_id ] );
                    if ( rand < value )
                    {
                        return data;
                    }
                }
            }

            return nullptr;
        }

        // 随机( 添加列表中的项目 )
        const T* Rand( uint32 id, uint32 weight ) const
        {
            std::map< uint32, uint32 > includelist;
            includelist[ id ] = weight;
            return Rand( includelist );
        }

        // 随机列表
        std::set< const T* >& Rand( uint32 count ) const
        {
            static std::set< const T* > _rand_list;
            _rand_list.clear();

            auto randweight = 0u;
            for ( auto i = 0u; i < count; ++i )
            {
                auto leftweight = _total_weight - randweight;
                if ( leftweight == 0u )
                {
                    break;
                }

                auto value = 0u;
                auto rand = KFGlobal::Instance()->RandRatio( leftweight );
                for ( auto data : _weight_data )
                {
                    // 判断是否随机过
                    if ( _rand_list.find( data ) != _rand_list.end() )
                    {
                        continue;
                    }

                    value += data->_weight;
                    if ( rand < value )
                    {
                        _rand_list.insert( data );
                        randweight += data->_weight;
                        break;
                    }
                }
            }

            return _rand_list;
        }

        // 是否含有某项目
        bool Have( uint32 id ) const
        {
            for ( auto data : _weight_data )
            {
                if ( data->_id == id && data->_weight != 0u )
                {
                    return true;
                }
            }

            return false;
        }

    public:
        // 总权重
        uint32 _total_weight = 0u;

        // 权重数据
        std::vector< T* > _weight_data;
    };

}

#endif
