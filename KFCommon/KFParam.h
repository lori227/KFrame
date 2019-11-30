#ifndef __KF_PARAM_H__
#define __KF_PARAM_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFParam
    {
    public:
        std::string _str_value;
        uint32 _int_value = 0u;
        double _double_value = 0.0f;
        MapUInt32 _map_value;
        VectorUInt32 _vector_value;
    };

    class KFParamList
    {
    public:
        KFParamList()
        {
            _elements.Clear();
            _params.clear();
        }

        ~KFParamList()
        {
            Reset();
        }

        void Reset()
        {
            _elements.Clear();
            for ( auto param : _params )
            {
                __KF_DELETE__( KFParam, param );
            }
            _params.clear();
        }

        KFParam* AddParam()
        {
            auto param = __KF_NEW__( KFParam );
            _params.push_back( param );
            return param;
        }

    public:
        // 属性列表
        std::vector< KFParam* > _params;

        // 如果是属性, 格式化的属性元素
        KFElements _elements;
    };
    ////////////////////////////////////////////////////////////////////////////////////////
}

#endif

