#ifndef __KF_EXECUTE_DATA_H__
#define __KF_EXECUTE_DATA_H__

#include "KFParam.h"
#include "KFUtility/KFElement.h"

namespace KFrame
{
    // 执行数据
    class KFExecuteData
    {
    public:
        void Reset()
        {
            _name.clear();
            _param_list.Reset();
        }

    public:
        // 名字
        std::string _name;

        // 参数列表
        KFParamList _param_list;

        // 如果是奖励, 就初始化( 为了运行时效率 )
        KFElements _elements;
        ///////////////////////////////////////////
        // 动态数值
        uint64 _calc_value = 0u;
    };
    typedef std::shared_ptr< KFExecuteData > ExecuteDataPtr;
}

#endif