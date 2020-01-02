#ifndef __KF_ELEMENT_RESULT_H__
#define __KF_ELEMENT_RESULT_H__

#include "KFInclude.h"

namespace KFrame
{
    namespace KFDataShowEnum
    {
        enum MyEnum
        {
            Show_None = 1,					// 不显示
            Show_Element = 2,				// 使用Element显示
            Show_Overlay = 3,				// 使用可叠加显示
            Show_Data = 4,					// 使用Data显示
        };
    }
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class KFData;
    class KFElement;
    class KFElementResult
    {
    public:
        // 清空结果
        void Clear()
        {
            _sequence = 0u;
            _element = nullptr;
            _show_type = KFDataShowEnum::Show_None;
            _config_id = 0u;
            _data_name.clear();
            _not_overlay_list.clear();
            _overlay_list.clear();
        }

        // 添加可叠加的数据
        void AddResult( uint32 id, const std::string& extendname, const std::string& dataname, uint32 value )
        {
            _config_id = id;
            _data_name = dataname;
            _show_type = KFDataShowEnum::Show_Overlay;
            _overlay_list[ extendname ] += value;
        }

        // 添加不可叠加的数据
        void AddResult( KFData* kfdata )
        {
            _not_overlay_list.push_back( kfdata );
            _show_type = KFDataShowEnum::Show_Data;
        }

    public:
        // 序列号
        uint64 _sequence = 0u;

        // 操作逻辑
        uint32 _operate = KFEnum::Add;

        // 添加的元素
        KFElement* _element = nullptr;

        // 显示类型
        uint32 _show_type = KFDataShowEnum::Show_None;

        // 配置id
        uint32 _config_id = 0u;

        // 主属性名
        std::string _data_name;

        // 可叠加的属性
        std::map< std::string, uint32 > _overlay_list;

        // 不可叠加的属性
        std::list< KFData* > _not_overlay_list;
    };
}

#endif