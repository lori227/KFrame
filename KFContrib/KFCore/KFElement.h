#ifndef __KF_ELEMENT_H__
#define __KF_ELEMENT_H__

#include "KFrame.h"

namespace KFrame
{
    class KFData;
    class KFDataSetting;
    class KFClassSetting;
    class KFValue
    {
    public:
        // 设置数值
        virtual void SetValue( std::string value ) = 0;

        // 获得使用的数值
        virtual const std::string& GetValue()
        {
            return _invalid_str;
        }

        // 计算数值
        virtual uint32 CalcUseValue( const KFDataSetting* kfsetting, float multiple )
        {
            return _invalid_int;
        }

        virtual uint32 CalcUseValue( const KFClassSetting* kfsetting, const std::string& dataname, float multiple )
        {
            return _invalid_int;
        }

        // 获得使用数值
        virtual uint32 GetUseValue()
        {
            return _invalid_int;
        }

        // 是否是string
        virtual bool IsString()
        {
            return false;
        }

        // 是否是数值
        virtual bool IsInt()
        {
            return false;
        }

        // 是否需要显示
        bool IsNeedShow();

    public:
        // 配置属性
        const KFDataSetting* _data_setting = nullptr;
    };
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFIntValue : public KFValue
    {
    public:
        // 设置数值
        virtual void SetValue( std::string value );

        // 计算数值
        virtual uint32 CalcUseValue( const KFDataSetting* kfsetting, float multiple );
        virtual uint32 CalcUseValue( const KFClassSetting* kfsetting, const std::string& dataname, float multiple );

        // 获得使用数值
        virtual uint32 GetUseValue();

        // 是否是数值
        virtual bool IsInt();

    private:
        uint32 _min_value = _invalid_int;
        uint32 _max_value = _invalid_int;
        uint32 _use_value = _invalid_int;
    };
    //////////////////////////////////////////////////////////////////
    class KFStrValue : public KFValue
    {
    public:
        // 设置数值
        virtual void SetValue( std::string value );

        // 获得使用的数值
        virtual const std::string& GetValue();

        // 是否是string
        virtual bool IsString();
    private:
        std::string _str_value;
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFElement
    {
    public:
        virtual ~KFElement() = default;

        virtual bool IsValue() const
        {
            return false;
        }

        virtual bool IsObject() const
        {
            return false;
        }

        // 设置操作类型
        void SetOperate( uint32 operate )
        {
            _operate = operate;
        }

        // 格式化
        virtual const std::string& ToString() const
        {
            return _invalid_str;
        }

        // 是否需要显示
        bool IsNeedShow() const;
    public:
        // 属性名
        std::string _data_name;

        // 操作 +-=* 等.....
        uint32 _operate = KFEnum::Add;

        // 配置属性
        const KFDataSetting* _data_setting = nullptr;
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    // 数值元素
    class KFElementValue : public KFElement
    {
    public:
        ~KFElementValue();

        virtual bool IsValue() const;

        // 设置数值
        void SetValue( std::string value );

        // 格式化
        virtual const std::string& ToString() const;

    public:
        // 属性值
        KFValue* _value = nullptr;
    };
    /////////////////////////////////////////////////////////////////////////////////
    // 对象元素
    class KFElementObject : public KFElement
    {
    public:
        virtual bool IsObject() const;

        // 设置数值
        void SetValue( const std::string& dataname, std::string value );

        // 获得数值
        uint32 GetValue( const KFClassSetting* kfsetting, const std::string& dataname, float multiple );

        // 格式化
        virtual const std::string& ToString() const;
    public:
        // 配置id( 如果有的话 )
        uint32 _config_id = _invalid_int;

        // 属性数据
        KFHashMap< std::string, const std::string&, KFValue > _values;
    };
    /////////////////////////////////////////////////////////////////////////////////
    class KFElements
    {
    public:
        KFElements();
        ~KFElements();

        // 是否为空
        bool IsEmpty() const;

        // 设置操作
        void SetOperate( uint32 operate );

        // 解析字符串奖励
        bool Parse( const std::string& strdata, const char* function, uint32 line );

        // non-copy
    private:
        KFElements( const KFElements& other ) = delete;
        KFElements& operator=( const KFElements& other ) = delete;

        // 重置
        void Cleanup();

    public:
        // 原始数据
        std::string _str_element;

        // 元素列表
        std::vector< KFElement* > _element_list;
    };
}

#endif
