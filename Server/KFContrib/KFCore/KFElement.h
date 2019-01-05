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
        bool SetValue( std::string value );

        // 计算数值
        uint64 CalcValue( float multiple = 1.0f );

        // 获得使用的数值
        uint64 GetValue();

    public:
        uint64 _min_value{ 0 };
        uint64 _max_value{ 0 };
        uint64 _use_value{ 0 };
    };
    //////////////////////////////////////////////////////////////////
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

        //// 是否是符合条件的数据
        //virtual bool IsEligibilityData( KFData* kfdata, float multiple, const std::string& exclude = _invalid_str ) = 0;

        //virtual uint64 CalcValue( float multiple = 1.0f ) = 0;


        //virtual uint64 GetValue( const std::string& value ) = 0;
        //uint64 GetUseValue( const std::string& value );

    protected:
        // 类型
        uint32 _type{ 0 };
    public:
        // 属性名
        std::string _data_name;

        // 父属性
        KFElements* _parent{ nullptr };

        // 操作 +-=* 等.....
        uint32 _operate{ KFOperateEnum::Add };
    };

    /////////////////////////////////////////////////////////////////////////////////
    // 数值元素
    class KFElementVale : public KFElement
    {
    public:
        KFElementVale()
        {
            _type = KFElementEnum::Value;
        }

        // 计算数值
        virtual uint64 CalcValue( float multiple = 1.0f );

        // 是否满足条件
        virtual bool IsEligibilityData( KFData* kfdata, float multiple, const std::string& exclude = _invalid_str );

    public:
        // 属性值
        KFValue _value;
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

        // 是否满足条件
        virtual bool IsEligibilityData( KFData* kfdata, float multiple, const std::string& exclude = _invalid_str );

    public:

        // 配置id( 如果有的话 )
        uint32 _config_id{ 0 };

        // 属性数据
        KFMap< std::string, const std::string&, KFValue > _values;
    };
    /////////////////////////////////////////////////////////////////////////////////
    class KFElements
    {
    public:
        KFElements();
        ~KFElements();

        // non-copy
    private:
        KFElements( const KFElements& other );
        KFElements& operator=( const KFElements& other );

        // 重置
        void Cleanup();

        // 添加
        void AddElement( KFElement* kfelement );

    public:
        // 解析字符串奖励
        bool Parse( const std::string& strdata, const char* function, uint32 line );

        // 格式化字串
        const std::string& Serialize() const;

        // 设置操作
        void SetOperate( uint32 operate );

    public:
        // 原始配置数据
        std::string _data;

        // 是否有随机奖励
        bool _is_rand_value;

        // 是否有改变属性
        bool _is_change_value;

        // 元素列表
        std::vector< KFElement* > _element_list;

        // 额外添加的元素
    };

}

#endif
