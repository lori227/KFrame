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
            return _invalid_string;
        }

        // 计算数值
        virtual uint64 CalcUseValue( const KFDataSetting* kfsetting, double multiple )
        {
            return _invalid_int;
        }

        virtual uint64 CalcUseValue( const KFDataSetting* kfsetting, const std::string& dataname, double multiple )
        {
            return _invalid_int;
        }

        // 获得使用数值
        virtual uint64 GetUseValue() const
        {
            return _invalid_int;
        }

        // 是否需要显示
        bool IsNeedShow();

        // 是否类型
        bool IsType( uint32 type ) const
        {
            return _type == type;
        }

    public:
        // 类型
        uint32 _type = 0;

        // 配置属性
        const KFDataSetting* _data_setting = nullptr;
    };
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFIntValue : public KFValue
    {
    public:
        KFIntValue();

        // 设置数值
        virtual void SetValue( std::string value );

        // 计算数值
        virtual uint64 CalcUseValue( const KFDataSetting* kfsetting, double multiple );
        virtual uint64 CalcUseValue( const KFDataSetting* kfsetting, const std::string& dataname, double multiple );

        // 获得使用数值
        virtual uint64 GetUseValue() const;

    private:
        uint64 _min_value = _invalid_int;
        uint64 _max_value = _invalid_int;
        uint64 _use_value = _invalid_int;
    };
    //////////////////////////////////////////////////////////////////
    class KFStrValue : public KFValue
    {
    public:
        KFStrValue();

        // 设置数值
        virtual void SetValue( std::string value );

        // 获得使用的数值
        virtual const std::string& GetValue();

    private:
        std::string _str_value;
    };

    //////////////////////////////////////////////////////////////////
    class KFElementObject;
    class KFObjValue : public KFValue
    {
    public:
        KFObjValue();
        ~KFObjValue();

        virtual void SetValue( std::string value ) {};
    public:
        // 元素列表
        KFElementObject* _element = nullptr;;
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
            return _invalid_string;
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
        uint64 CalcValue( const KFDataSetting* kfsetting, const std::string& dataname, double multiple );
        uint64 GetValue( const std::string& dataname ) const;

        // 格式化
        virtual const std::string& ToString() const;

        // 创建对象数值
        KFObjValue* CreateObjectValue( const std::string& dataname );

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
        KFElements() = default;
        ~KFElements();

        // 重置
        void Clear();

        // 是否为空
        bool IsEmpty() const;

        // 设置操作
        void SetOperate( uint32 operate );

        // 解析字符串奖励
        bool Parse( const std::string& strdata, const char* function, uint32 line );

        // 计算返回元素
        const std::string& CalcElement( double multiple ) const;

        // non-copy
    private:
        KFElements( const KFElements& other ) = delete;
        KFElements& operator=( const KFElements& other ) = delete;

    public:
        // 原始数据
        std::string _str_element;

        // 元素列表
        std::vector< KFElement* > _element_list;
    };
}

#endif
