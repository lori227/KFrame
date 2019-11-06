#ifndef __KF_DROP_DATA_H__
#define __KF_DROP_DATA_H__

#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFDropData
    {
    public:
        // 掉落数据id
        uint32 _drop_data_id = 0u;

        // 掉落索引
        uint32 _data_index = 0u;

        // 掉落属性名
        std::string _data_name;

        // 掉落key
        uint32 _data_key = 0u;

        // 掉落数值
        std::string _data_value;

        // 掉落数值
        uint32 _min_value = 0u;
        uint32 _max_value = 0u;

        // 掉落的属性数据( 方便使用entity接口 )
        KFElements _elements;

    public:
        // 获得数值
        uint32 GetValue() const
        {
            return KFGlobal::Instance()->RandRange( _min_value, _max_value, 1 );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::list< const KFDropData* > DropDataList;
    ///////////////////////////////////////////////////////////////////////////////////////////////
}

#endif