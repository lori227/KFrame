#ifndef __KF_DATA_CONFIG_H__
#define __KF_DATA_CONFIG_H__

#include "KFConfig.h"
#include "KFDataSetting.h"

namespace KFrame
{
    class KFDataConfig : public KFStrConfigT< KFClassSetting >, public KFInstance< KFDataConfig >
    {
    public:
        KFDataConfig()
        {
            _file_name = "class";
        }

        // 加载完成
        virtual void LoadComplete();

        // 所有加载完成
        virtual void LoadAllComplete();

        // 获得属性配置
        const KFDataSetting* FindDataSetting( const std::string& classname, const std::string& dataname );
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFClassSetting* kfsetting );
    };
}

#endif