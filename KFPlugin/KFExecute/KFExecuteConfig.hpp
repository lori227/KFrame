#ifndef __KF_EXECUTE_CONFIG_H__
#define __KF_EXECUTE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFExecuteData.h"

namespace KFrame
{
    class KFExecuteSetting : public KFIntSetting
    {
    public:
        // 执行列表
        std::vector< KFExecuteData* > _execute_data_list;
        KFMap<uint32, uint32, KFExecuteData > _sort_execute_data_list;
    };

    class KFExecuteConfig : public KFConfigT< KFExecuteSetting >, public KFInstance< KFExecuteConfig >
    {
    public:
        KFExecuteConfig()
        {
            _file_name = "execute";
        }

        virtual void LoadAllComplete();

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExecuteSetting* kfsetting );
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFExecuteSortSetting : public KFStrSetting
    {
    public:
        // 排列顺序
        uint32 _sort = 0u;
    };

    class KFExecuteSortConfig : public KFConfigT< KFExecuteSortSetting >, public KFInstance< KFExecuteSortConfig >
    {
    public:
        KFExecuteSortConfig()
        {
            _file_name = "executesort";
        }

        // 获得排序
        uint32 GetExecuteSort( const std::string& name );

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExecuteSortSetting* kfsetting );
    public:
        uint32 _max_sort = 0u;
    };
}

#endif