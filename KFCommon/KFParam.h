#ifndef __KF_PARAM_H__
#define __KF_PARAM_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFParam
    {
    public:
        std::string _str_value;
        uint32 _int_value = 0u;
        double _double_value = 0.0f;
        UInt32Map _map_value;
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

