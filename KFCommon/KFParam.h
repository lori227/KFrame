#ifndef __KF_PARAM_H__
#define __KF_PARAM_H__

#include "KFInclude.h"

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
        KFParamList()
        {
            _params.clear();
        }

        ~KFParamList()
        {
            Reset();
        }

        void Reset()
        {
            for ( auto param : _params )
            {
                delete param;
            }
            _params.clear();
        }

        KFParam* AddParam()
        {
            auto param = new KFParam;
            _params.push_back( param );
            return param;
        }

    public:
        // 属性列表
        std::vector< KFParam* > _params;
    };
    ////////////////////////////////////////////////////////////////////////////////////////
}

#endif

