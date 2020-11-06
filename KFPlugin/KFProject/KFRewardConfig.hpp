#ifndef __KF_REWARD_CONFIG_H__
#define __KF_REWARD_CONFIG_H__

#include "KFConfig.h"
#include "KFXmlReader/KFElementConfig.h"

namespace KFrame
{
    enum KFRewardEnum
    {
        Element = 1,	// 默认的字串
        CodeJson = 2,	// 简化的json字串
    };

    class KFRewardSetting : public KFStrSetting
    {
    public:
        // 类型
        uint32 _type;

        // 编码
        uint32 _code;

        // 格式化字串
        std::string _element_template;
    };

    class KFRewardConfig : public KFConfigT< KFRewardSetting >, public KFInstance< KFRewardConfig >
    {
    public:
        KFRewardConfig()
        {
            _file_name = "reward";
        }

        // 获取奖励字符串
        const std::string& FormatIntString( const std::string& dataname, uint32 datavalue, uint32 dataid );
        const std::string& FormatStrString( const std::string& dataname, const std::string& datavalue, uint32 dataid );
        const std::string& FormatSettingString( const KFElementSetting& kfsetting );
        const std::string& ParseString( const std::string& strparse );

    protected:
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFRewardSetting* kfsetting );

        // 保存type和code的对应
        uint64 CalcTypeCodeValue( uint64 type, uint64 code );

        // 格式化字串
        const std::string& FormatRewardString( uint32 type, uint32 code, uint32 num );

        // 格式化编码字串
        const std::string& ParseCodeString( const std::string& strparse );

    private:
        UInt64String _id_name_list;
    };
}

#endif