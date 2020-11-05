#ifndef __KF_EXECUTE_CONFIG_H__
#define __KF_EXECUTE_CONFIG_H__

#include "KFConfig.h"
#include "KFXmlReader/KFExecuteData.h"
#include "KFXmlReader/KFElementConfig.h"

namespace KFrame
{
    class KFExecuteSetting : public KFIntSetting
    {
    public:
        // 执行列表
        KFVector<KFExecuteData> _execute_list;
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
}

#endif