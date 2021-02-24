#ifndef __KF_ELEMENT_H__
#define __KF_ELEMENT_H__

#include "KFRange.h"

namespace KFrame
{
    class KFData;
    class KFDataSetting;
    class KFClassSetting;
    class KFValue
    {
    public:
        virtual ~KFValue() = default;

        // 设置数值
        virtual void SetValue( std::string value ) = 0;

        // 获得使用的数值
        virtual const std::string& GetValue()
        {
            return _invalid_string;
        }

        // 计算数值
        virtual uint64 CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, double multiple )
        {
            return _invalid_int;
        }

        virtual uint64 CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, const std::string& data_name, double multiple )
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
        inline bool IsType( uint32 type ) const
        {
            return _type == type;
        }

    public:
        // 类型
        uint32 _type = 0;

        // 配置属性
        std::shared_ptr<const KFDataSetting> _data_setting = nullptr;
    };
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFIntValue : public KFValue
    {
    public:
        KFIntValue();
        virtual ~KFIntValue();

        // 设置数值
        virtual void SetValue( std::string value );

        // 计算数值
        virtual uint64 CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, double multiple );
        virtual uint64 CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, const std::string& data_name, double multiple );

        // 获得使用数值
        virtual uint64 GetUseValue() const;

    private:
        KFRange<uint64> _range;
        uint64 _use_value = _invalid_int;
    };
    //////////////////////////////////////////////////////////////////
    class KFStrValue : public KFValue
    {
    public:
        KFStrValue();
        virtual ~KFStrValue();

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
        virtual ~KFObjValue();

        virtual void SetValue( std::string value ) {};
    public:
        // 元素列表
        std::shared_ptr<KFElementObject> _element = nullptr;
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
        std::shared_ptr<const KFDataSetting> _data_setting = nullptr;
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
        std::shared_ptr<KFValue> _value = nullptr;
    };
    /////////////////////////////////////////////////////////////////////////////////
    // 对象元素
    class KFElementObject : public KFElement
    {
    public:
        virtual bool IsObject() const;

        // 设置数值
        void SetValue( const std::string& data_name, std::string value );

        // 获得数值
        uint64 CalcValue( std::shared_ptr<const KFDataSetting>& setting, const std::string& data_name, double multiple = _default_multiple );
        uint64 GetValue( const std::string& data_name ) const;

        // 格式化
        virtual const std::string& ToString() const;

        // 创建对象数值
        std::shared_ptr<KFObjValue> CreateObjectValue( const std::string& data_name );

    public:
        // 配置id( 如果有的话 )
        uint32 _config_id = _invalid_int;

        // 属性数据
        KFHashMap<std::string, KFValue> _values;
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
        bool Parse( const std::string& data, const char* function, uint32 line );

        // 计算返回元素
        const std::string& CalcElement( double multiple ) const;

        // 获得解析的元素数据
        const std::string& GetElement() const;

        // non-copy
    private:
        KFElements( const KFElements& other ) = delete;
        KFElements& operator=( const KFElements& other ) = delete;

    protected:
        // 解析的元素数据
        std::string _str_element;

    public:
        // 配置表原始数据
        std::string _str_parse;

        // 元素列表
        std::vector<std::shared_ptr<KFElement>> _element_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 元属性数据
    class KFElementData
    {
    public:
        void AddData( const std::string& data_name, const std::string& data_value, uint32 data_id = 0u )
        {
            _data_list.emplace_back( std::make_tuple( data_name, data_value, data_id ) );
        }

        void AddData( const std::string& data_name, uint32 data_value, uint32 data_id = 0u )
        {
            _data_list.emplace_back( std::make_tuple( data_name, __TO_STRING__( data_value ), data_id ) );
        }

        bool IsEmpty() const
        {
            return _data_list.empty();
        }
    public:
        std::list<std::tuple<std::string, std::string, uint32>> _data_list;
    };
}

#endif
