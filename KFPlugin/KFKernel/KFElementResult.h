#ifndef __KF_ELEMENT_RESULT_H__
#define __KF_ELEMENT_RESULT_H__

#include "KFInclude.h"

namespace KFrame
{
    namespace KFDataShowEnum
    {
        enum MyEnum
        {
            Show_None = 0,					// 不显示
            Show_Element = 1,				// 使用Element显示
            Show_Currency = 2,				// 货币属性
            Show_Object = 3,				// 对象属性
            Show_Overlay = 4,				// 可堆叠属性
            Show_NotOverlay = 5,			// 不可堆叠属性
        };
    }
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    class KFData;
    class KFElement;
    class KFElementResult
    {
    public:
        // 添加可叠加的数据
        bool AddResult( uint32 value )
        {
            _total_value += value;
            _show_type = KFDataShowEnum::Show_Currency;
            return true;
        }

        // 添加可叠加的数据
        bool AddResult( const std::string& dataname, uint32 value )
        {
            _data_name = dataname;
            _total_value += value;
            _show_type = KFDataShowEnum::Show_Object;
            return true;
        }

        // 添加可叠加的数据
        bool AddResult( uint32 id, const std::string& overlayname, const std::string& dataname, uint32 value )
        {
            _config_id = id;
            _data_name = dataname;
            _total_value += value;
            _overlay_list[ overlayname ] += value;
            _show_type = KFDataShowEnum::Show_Overlay;
            return true;
        }

        // 添加不可叠加的数据
        bool AddResult( uint32 id, KFData* kfdata )
        {
            _config_id = id;
            _total_value += 1u;
            _not_overlay_list.push_back( kfdata );
            _show_type = KFDataShowEnum::Show_NotOverlay;
            return true;
        }

    public:
        // 序列号
        uint64 _sequence = 0u;

        // 操作逻辑
        uint32 _operate = KFEnum::Add;

        // 添加的元素
        KFElement* _element = nullptr;

        // 倍数
        float _multiple = 0.0f;

        // 是否需要显示
        bool _is_need_show = false;

        // 显示类型
        uint32 _show_type = KFDataShowEnum::Show_Element;
        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////
        // 配置id
        uint32 _config_id = 0u;

        // 主属性名
        std::string _data_name;

        // 可叠加的属性
        std::map< std::string, uint32 > _overlay_list;

        // 总数量
        uint32 _total_value = 0u;
        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////
        // 不可叠加的属性
        std::list< KFData* > _not_overlay_list;
    };
}

#endif