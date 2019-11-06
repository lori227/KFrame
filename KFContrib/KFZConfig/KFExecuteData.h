#ifndef __KF_EXECUTE_DATA_H__
#define __KF_EXECUTE_DATA_H__

#include "KFParam.h"
#include "KFZConfig/KFSetting.h"

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
        ///////////////////////////////////////////
        // 动态数值
        uint32 _calc_value = 0u;
    };


}

#endif