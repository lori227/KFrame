#ifndef __KF_PROJECT_MOUDLE_H__
#define __KF_PROJECT_MOUDLE_H__

/************************************************************************
//    @Module			:    项目配置
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-4
************************************************************************/


#include "KFProjectInterface.h"
#include "KFProjectConfig.hpp"
#include "KFUuidConfig.hpp"
#include "KFConfig/KFTemplateConfig.hpp"
#include "KFConfig/KFConstantConfig.hpp"

namespace KFrame
{
    class KFProjectModule : public KFProjectInterface
    {
    public:
        KFProjectModule() = default;
        ~KFProjectModule() = default;

        // 初始化
        virtual void InitModule();
        virtual void BeforeRun();

        // 加载完成
        virtual void AfterLoad();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 获取奖励字符串
        const std::string& FormatIntString( const std::string& dataname, uint32 datavalue, uint32 dataid );
        const std::string& FormatStrString( const std::string& dataname, const std::string& datavalue, uint32 dataid );
        const std::string& FormatDataString( const KFElementData& elementdata );
        const std::string& ParseString( const std::string& strparse );

        // 保存type和code的对应
        uint64 CalcTypeCodeValue( uint64 type, uint64 code );

        // 格式化字串
        const std::string& FormatRewardString( uint32 type, uint32 code, uint32 num );

        // 格式化编码字串
        const std::string& ParseCodeString( const std::string& strparse );

    private:
        // 保存id和属性名的对应列表
        UInt64String _id_name_list;
    };
}



#endif