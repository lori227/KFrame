#ifndef __KF_OPTION_INTERFACE_H__
#define __KF_OPTION_INTERFACE_H__

#include "KFrame.h"
#include "KFZConfig/KFOptionSetting.h"

namespace KFrame
{
    // 优化逻辑, 很多地方使用static, 然而option配置需要可以重新加载, 所以使用KFOption指针,
    // 重新读取配置后KFOption指针不会改变
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    class KFOptionInterface : public KFModule
    {
    public:
        // 获得配置属性
        virtual const KFOptionSetting* FindOption( const std::string& name ) = 0;
        virtual const KFOptionSetting* FindOption( const std::string& name, uint32 key ) = 0;
        virtual const KFOptionSetting* FindOption( const std::string& name, const std::string& key ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加配置
        virtual void AddOption( const std::string& name, uint32 value ) = 0;
        virtual void AddOption( const std::string& name, uint32 key, uint32 value ) = 0;
        virtual void AddOption( const std::string& name, const std::string& key, uint32 value ) = 0;

        virtual void AddOption( const std::string& name, double value ) = 0;
        virtual void AddOption( const std::string& name, uint32 key, double value ) = 0;
        virtual void AddOption( const std::string& name, const std::string& key, double value ) = 0;

        virtual void AddOption( const std::string& name, const std::string& value ) = 0;
        virtual void AddOption( const std::string& name, uint32 key, const std::string& value ) = 0;
        virtual void AddOption( const std::string& name, const std::string& key, const std::string& value ) = 0;
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