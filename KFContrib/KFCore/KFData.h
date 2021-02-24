#ifndef __KF_DATA_H__
#define __KF_DATA_H__

#include "KFDataSetting.h"
#include "KFMath3D/Math3D.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFData : public std::enable_shared_from_this<KFData>
    {
    public:
    public:
        KFData() = default;
        virtual ~KFData();

        // 无效值
        template<class T> T InvalidValue();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // int/uint32等读取数值
        template<class T = uint64> T Get();

        // array读取数值
        // @key : 键值
        template<class T = uint64> T Get( uint64 key );

        // object读取数值
        // @data_name : 属性名
        template<class T = uint64> T Get( const std::string& data_name );

        // object读取数值
        // @parent_name : 父属性名
        // @data_name : 属性名
        template<class T = uint64> T Get( const std::string& parent_name, const std::string& data_name );

        // record读取数值
        // @key : 键值
        // @data_name : 属性名
        template<class T = uint64> T Get( uint64 key, const std::string& data_name );

        // object读取数值
        // @data_name : 属性名
        // @key : 键值
        template<class T = uint64> T Get( const std::string& data_name, uint64 key );

        // object读取数值
        // @parent_name : 父属性名
        // @key : 键值
        // @data_name : 属性名
        template<class T = uint64> T Get( const std::string& parent_name, uint64 key, const std::string& data_name );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // data设置数值
        // @value : 数值
        template<class T = uint64> T Set( T value );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // data设置数值
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( uint32 operate, T value );

        // object设置数值
        // @data_name : 属性名
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( const std::string& data_name, uint32 operate, T value );

        // object设置数值
        // @parent_name : 父属性名
        // @data_name : 属性名
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( const std::string& parent_name, const std::string& data_name, uint32 operate, T value );

        // record设置数值
        // @key : 键值
        // @data_name : 属性名
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( uint64 key, const std::string& data_name, uint32 operate, T value );

        // record设置数值
        // @data_name : 属性名
        // @key : 键值
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( const std::string& data_name, uint64 key, uint32 operate, T value );

        // object设置数值
        // @parent_name : 父属性名
        // @key : 键值
        // @data_name : 属性名
        // @operate : 操作类型
        // @value : 数值
        template<class T = uint64> T Operate( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, T value );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转成成字符串
        // @data_name : 属性名
        std::string ToString( const std::string& data_name );

        // 转成成字符串
        // @parent_name : 父属性名
        // @data_name : 属性名
        std::string ToString( const std::string& parent_name, const std::string& data_name );

        // 转成成字符串
        // @key : 键值
        // @data_name : 属性名
        std::string ToString( uint64 key, const std::string& data_name );

        // 转成成字符串
        // @parent_name : 父属性名
        // @key : 键值
        // @data_name : 属性名
        std::string ToString( const std::string& parent_name, uint64 key, const std::string& data_name );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得指针
        DataPtr GetDataPtr();

        // 重置
        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 数量
        virtual uint32 Size();

        // 最大数量
        virtual uint32 MaxSize();

        // 是否达到了最大值
        virtual bool IsFull();

        // 重置数量
        virtual void Resize( uint32 size );

        // config id
        virtual uint32 GetConfigID();

        // keyid
        virtual uint64 GetKeyID();
        virtual void SetKeyID( uint64 id );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化
        void InitSetting( std::shared_ptr<const KFDataSetting> data_setting );

        // 初始化数值
        virtual void InitData();

        // 属性标识
        bool HaveMask( uint32 mask ) const;

        // 添加标识
        void AddMask( uint32 mask );

        // 删除标识
        void RemoveMask( uint32 mask );

        // 是否需要同步给客户端
        bool IsNeedSyncToClient() const;

        // 判断属性
        bool IsInt() const;
        bool IsArray() const;
        bool IsRecord() const;
        bool IsObject() const;
        bool IsString() const;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 父属性
        KFData* GetParent();
        void SetParent( KFData* data );

        // 遍历节点
        virtual DataPtr First();
        virtual DataPtr Next();

        // 赋值/保存
        virtual void CopyFrom( DataPtr& other ) = 0;
        virtual void SaveTo( DataPtr& other ) = 0;

        // 格式化成字串
        virtual std::string ToString() = 0;
        virtual void FromString( const std::string& value ) = 0;

        // Map的特殊操作
        virtual void ToMap( StringMap& values );
        virtual void FromMap( const StringMap& values );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // object

        // 如:money
        virtual DataPtr Find( const std::string& data_name );

        // 如:hero->1
        virtual DataPtr Find( const std::string& data_name, uint64 key );

        // 如: basic->id
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name );

        // 如: hero->1->level
        virtual DataPtr Find( const std::string& parent_name, uint64 key, const std::string& child_name );

        // 如: sign->day->1
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name, uint64 key );

        // 如: sign->day->value
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name, const std::string& data_name );

        // 添加
        virtual bool Add( const std::string& data_name, DataPtr& data );
        virtual bool Add( const std::string& data_name, uint64 key, DataPtr& data );
        virtual bool Add( const std::string& parent_name, const std::string& child_name, DataPtr& data );

        // 删除
        virtual DataPtr Remove( const std::string& data_name );
        virtual DataPtr Remove( const std::string& data_name, uint64 key );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // record
        virtual DataPtr Find( uint64 key );
        virtual DataPtr Find( uint64 key, const std::string& data_name );
        virtual DataPtr Find( uint64 parent_key, uint64 child_key );
        virtual DataPtr Find( uint64 parent_key, uint64 child_key, const std::string& data_name );
        virtual DataPtr Find( uint64 parent_key, const std::string& data_name, uint64 child_key );

        virtual bool Add( uint64 key, DataPtr& data );
        virtual bool Add( uint64 parent_key, uint64 child_key, DataPtr& data );
        virtual bool Add( uint64 key, const std::string& data_name, DataPtr& data );

        virtual DataPtr Remove( uint64 key );

        virtual void Find( const std::string& data_name, uint64 value, std::list<DataPtr>& find_list, bool find_all );
        virtual bool Check( const std::string& data_name, uint64 value, const std::string& check_name, uint64 check_value );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // array
        virtual DataPtr Insert( uint64 value );
        virtual uint32 GetEmpty();
        virtual uint32 GetIndex( uint64 value );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得配置表属性
        virtual uint64 GetConfigValue( const std::string& name, uint64 id, uint64 maxvalue = __MAX_UINT64__ );
    protected:
        virtual int32 GetInt32();
        virtual int32 SetInt32( int32 );

        virtual uint32 GetUInt32();
        virtual uint32 SetUInt32( uint32 );

        virtual int64 GetInt64();
        virtual int64 SetInt64( int64 );

        virtual uint64 GetUInt64();
        virtual uint64 SetUInt64( uint64 );

        virtual double GetDouble();
        virtual double SetDouble( double );

        virtual const std::string& GetString();
        virtual const std::string& SetString( const std::string& );

        virtual const Math3D::Vector3D& GetVector3D();
        virtual const Math3D::Vector3D& SetVector3D( const Math3D::Vector3D& );

    public:
        // 类型
        uint32 _data_type = 0u;

        // 属性配置
        std::shared_ptr<const KFDataSetting> _data_setting = nullptr;
    protected:
        // 运行时标记
        uint32 _run_mask = 0u;

        // 父属性
        KFData* _parent = nullptr;
    };
}

#include "KFData.inl"

#endif
