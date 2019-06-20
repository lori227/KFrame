#ifndef __KF_COMPOUND_CONFIG_H__
#define __KF_COMPOUND_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFCompoundSetting : public KFIntSetting
    {
    public:
        // 属性名字
        std::string _data_name;

        // 属性id
        uint32 _key = 0u;

        // 合成的属性
        KFElements _compound_data;

        // 花费材料
        KFElements _cost_data;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFCompoundConfig : public KFIntConfigT< KFCompoundSetting >, public KFSingleton< KFCompoundConfig >
    {
    public:
        KFCompoundConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFCompoundSetting >( file, isclear )
        {
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFCompoundSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_compound_config = KFCompoundConfig::Instance( "compound.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif