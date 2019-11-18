#ifndef __KF_DATA_H__
#define __KF_DATA_H__

#include "KFMath3D/Math3D.h"
#include "KFZConfig/KFDataSetting.h"

namespace KFrame
{
    class KFData
    {
    public:
        KFData() = default;
        virtual ~KFData() = default;

        template< class T > T InvalidValue();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T = uint64 > T Get();
        template< class T = uint64 > T Get( uint64 key );
        template< class T = uint64 > T Get( const std::string& dataname );
        template< class T = uint64 > T Get( const std::string& parentname, const std::string& dataname );
        template< class T = uint64 > T Get( uint64 key, const std::string& dataname );
        template< class T = uint64 > T Get( const std::string& dataname, uint64 key );
        template< class T = uint64 > T Get( const std::string& parentname, uint64 key, const std::string& dataname );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T = uint64 > T Set( T value );
        template< class T = uint64 > T Set( uint64 key, T value );
        template< class T = uint64 > T Set( const std::string& dataname, T value );
        template< class T = uint64 > T Set( const std::string& parentname, const std::string& dataname, T value );
        template< class T = uint64 > T Set( uint64 key, const std::string& dataname, T value );
        template< class T = uint64 > T Set( const std::string& dataname, uint64 key, T value );
        template< class T = uint64 > T Set( const std::string& parentname, uint64 key, const std::string& dataname, T value );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T = uint64 > T Operate( uint32 operate, T value );
        template< class T = uint64 > T Operate( const std::string& dataname, uint32 operate, T value );
        template< class T = uint64 > T Operate( const std::string& parentname, const std::string& dataname, uint32 operate, T value );
        template< class T = uint64 > T Operate( uint64 key, const std::string& dataname, uint32 operate, T value );
        template< class T = uint64 > T Operate( const std::string& dataname, uint64 key, uint32 operate, T value );
        template< class T = uint64 > T Operate( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, T value );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string ToString( const std::string& dataname );
        std::string ToString( const std::string& parentname, const std::string& dataname );
        std::string ToString( uint64 key, const std::string& dataname );
        std::string ToString( const std::string& parentname, uint64 key, const std::string& dataname );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        // keyid
        virtual uint64 GetKeyID();
        virtual void SetKeyID( uint64 id );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化
        virtual void Initialize( const KFClassSetting* classsetting, const KFDataSetting* datasetting );

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
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 父属性
        KFData* GetParent();
        void SetParent( KFData* data );

        // 遍历节点
        virtual KFData* First();
        virtual KFData* Next();

        // 赋值/保存
        virtual void CopyFrom( KFData* kfother ) = 0;
        virtual void SaveTo( KFData* kfother ) = 0;

        // 格式化成字串
        virtual std::string ToString() = 0;
        virtual void FromString( const std::string& value ) = 0;

        // Map的特殊操作
        virtual void ToMap( MapString& values );
        virtual void FromMap( const MapString& values );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // object

        // 如:money
        virtual KFData* Find( const std::string& dataname );

        // 如:hero->1
        virtual KFData* Find( const std::string& dataname, uint64 key );

        // 如: basic->id
        virtual KFData* Find( const std::string& parentname, const std::string& childname );

        // 如: hero->1->level
        virtual KFData* Find( const std::string& parentname, uint64 key, const std::string& childname );

        // 如: sign->day->1
        virtual KFData* Find( const std::string& parentname, const std::string& childname, uint64 key );

        // 如: sign->day->value
        virtual KFData* Find( const std::string& parentname, const std::string& childname, const std::string& dataname );

        virtual bool Add( const std::string& dataname, KFData* data );
        virtual bool Add( const std::string& dataname, uint64 key, KFData* data );
        virtual bool Add( const std::string& parentname, const std::string& childname, KFData* data );

        virtual KFData* Move( const std::string& dataname, bool usekey );
        virtual bool Remove( const std::string& dataname );
        virtual bool Remove( const std::string& dataname, uint64 key );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // record
        virtual KFData* Find( uint64 key );
        virtual KFData* Find( uint64 key, const std::string& dataname );
        virtual KFData* Find( uint64 parentkey, uint64 childkey );
        virtual KFData* Find( uint64 parentkey, uint64 childkey, const std::string& dataname );
        virtual KFData* Find( uint64 parentkey, const std::string& dataname, uint64 childkey );

        virtual bool Add( uint64 key, KFData* data );
        virtual bool Add( uint64 parentkey, uint64 childkey, KFData* data );
        virtual bool Add( uint64 key, const std::string& dataname, KFData* data );

        virtual KFData* Move( uint64 key );
        virtual bool Remove( uint64 key );

        virtual void Find( const std::string& dataname, uint64 value, std::list< KFData* >& findlist, bool findall );
        virtual bool Check( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
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
        const KFDataSetting* _data_setting = nullptr;

        // 类配置
        const KFClassSetting* _class_setting = nullptr;

    protected:
        // 运行时标记
        uint32 _run_mask = 0u;

        // 父属性
        KFData* _parent = nullptr;
    };
}

#include "KFData.inl"

#endif
