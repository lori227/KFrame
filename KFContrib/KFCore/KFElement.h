#ifndef __KF_ELEMENT_H__
#define __KF_ELEMENT_H__

#include "KFrame.h"

namespace KFrame
{
    class KFData;
    class KFValue
    {
    public:
        // 设置数值
        virtual bool SetValue( std::string value ) = 0;

        // 计算数值
        virtual const std::string& CalcValue( float multiple = 1.0f ) = 0;
        virtual uint64 CalcUInt64( float multiple = 1.0f ) = 0;

        // 获得使用的数值
        virtual const std::string& GetValue( float multiple = 1.0f ) = 0;

    public:
        std::string _str_value;
        std::string _use_value;
    };
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFIntValue : public KFValue
    {
    public:
        // 设置数值
        virtual bool SetValue( std::string value );

        // 计算数值
        virtual const std::string& CalcValue( float multiple = 1.0f );
        virtual uint64 CalcUInt64( float multiple = 1.0f );

        // 获得使用的数值
        virtual const std::string& GetValue( float multiple = 1.0f );

    public:
        uint64 _min_value = 0u;
        uint64 _max_value = 0u;
    };
    //////////////////////////////////////////////////////////////////
    class KFStrValue : public KFValue
    {
    public:
        // 设置数值
        virtual bool SetValue( std::string value );

        // 计算数值
        virtual const std::string& CalcValue( float multiple = 1.0f );
        virtual uint64 CalcUInt64( float multiple = 1.0f );

        // 获得使用的数值
        virtual const std::string& GetValue( float multiple = 1.0f );
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    enum KFElementEnum
    {
        Value = 1,
        Object = 2,
    };

    class KFElements;
    class KFElement
    {
    public:
        virtual ~KFElement() = default;

        bool IsValue() const
        {
            return _type == KFElementEnum::Value;
        }

        bool IsObject() const
        {
            return _type == KFElementEnum::Object;
        }

        // 设置操作类型
        void SetOperate( uint32 operate )
        {
            _operate = operate;
        }

    protected:
        // 类型
        uint32 _type = 0u;
    public:
        // 属性名
        std::string _data_name;

        // 父属性
        KFElements* _parent{ nullptr };

        // 操作 +-=* 等.....
        uint32 _operate{ KFEnum::Add };
    };

    /////////////////////////////////////////////////////////////////////////////////
    // 数值元素
    class KFElementValue : public KFElement
    {
    public:
        KFElementValue();
        ~KFElementValue();

        // 设置数值
        bool SetValue( std::string value );

        // 计算数值
        const std::string& CalcValue( float multiple = 1.0f );
        uint64 CalcUInt64( float multiple = 1.0f );
        const std::string& GetValue( float multiple = 1.0f );

    public:
        // 属性值
        KFValue* _value;
    };
    /////////////////////////////////////////////////////////////////////////////////
    // 对象元素
    class KFElementObject : public KFElement
    {
    public:
        KFElementObject()
        {
            _type = KFElementEnum::Object;
        }

        bool SetValue( const std::string& dataname, std::string value );

        // 计算数值
        const std::string& CalcValue( const std::string& name, float multiple = 1.0f );
        uint64 CalcUInt64( const std::string& name, float multiple = 1.0f );

    public:

        // 配置id( 如果有的话 )
        uint32 _config_id = 0u ;

        // 属性数据
        KFHashMap< std::string, const std::string&, KFValue > _values;
    };
    /////////////////////////////////////////////////////////////////////////////////
    class KFElements
    {
    public:
        KFElements();
        ~KFElements();

        // 解析字符串奖励
        bool Parse( const std::string& strdata, const char* function, uint32 line );

        // 格式化字串
        const std::string& Serialize( float multiple = 1.0f ) const;

        // 设置操作
        void SetOperate( uint32 operate );

        // non-copy
    private:
        KFElements( const KFElements& other ) = delete;
        KFElements& operator=( const KFElements& other ) = delete;

        // 重置
        void Cleanup();

        // 添加
        void AddElement( KFElement* kfelement );

    public:
        // 原始配置数据
        std::string _data;

        // 是否有随机奖励
        bool _is_rand_value = false;

        // 元素列表
        std::vector< KFElement* > _element_list;
    };
}

#endif
