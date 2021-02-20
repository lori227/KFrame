#ifndef __KF_PARAM_H__
#define __KF_PARAM_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFParam
    {
    public:
        // 字符串
        std::string _str_value;
        
        // 整数
        uint32 _int_value = 0u;

        // 浮点
        double _double_value = 0.0f;

        // map容器
        UInt32Map _map_value;

        // 数组容器
        UInt32Vector _vector_value;
    };

    class KFParamList
    {
    public:
        inline void Reset()
        {
            _params.clear();
        }

        inline std::shared_ptr<KFParam> AddParam()
        {
            auto param = __MAKE_SHARED__( KFParam );
            _params.push_back( param );
            return param;
        }

    public:
        // 属性列表
        std::vector<std::shared_ptr<KFParam>> _params;
    };
    ////////////////////////////////////////////////////////////////////////////////////////
}

#endif

