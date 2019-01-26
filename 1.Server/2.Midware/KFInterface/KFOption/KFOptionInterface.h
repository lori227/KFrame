#ifndef __KF_OPTION_INTERFACE_H__
#define __KF_OPTION_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 优化逻辑, 很多地方使用static, 然而option配置需要可以重新加载, 所以使用KFOption指针,
    // 重新读取配置后KFOption指针不会改变
    class KFOption
    {
    public:
        // 整型数值
        uint32 _uint32_value{ 0 };

        // 浮点数值
        double _double_value{ 0.0f };

        // 字符串数值
        std::string _str_value;
    };

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    class KFOptionInterface : public KFModule
    {
    public:
        // 获得配置属性
        virtual const KFOption* FindOption( const std::string& name ) = 0;
        virtual const KFOption* FindOption( const std::string& name, uint32 key ) = 0;
        virtual const KFOption* FindOption( const std::string& name, const std::string& key ) = 0;

        //////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // uint32配置
        virtual uint32 GetUInt32( const std::string& name ) = 0;
        virtual uint32 GetUInt32( const std::string& name, uint32 key ) = 0;
        virtual uint32 GetUInt32( const std::string& name, const std::string& key ) = 0;

        // double配置
        virtual double GetDouble( const std::string& name ) = 0;
        virtual double GetDouble( const std::string& name, uint32 key ) = 0;
        virtual double GetDouble( const std::string& name, const std::string& key ) = 0;

        // string配置
        virtual const std::string& GetString( const std::string& name ) = 0;
        virtual const std::string& GetString( const std::string& name, uint32 key ) = 0;
        virtual const std::string& GetString( const std::string& name, const std::string& key ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_option, KFOptionInterface );
    ///////////////////////////////////////////////////////////////////////////
}



#endif