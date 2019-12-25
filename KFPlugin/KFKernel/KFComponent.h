#ifndef __KF_COMPONENT_H__
#define __KF_COMPONENT_H__

#include "KFEntity.h"
#include "KFElementResult.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function< void( KFEntity* ) > KFEntityFunction;
    typedef std::function< void( KFEntity*, uint32 ) > KFSaveEntityFunction;
    typedef std::function< void( KFEntity*, KFMsg::PBObject& ) > KFSyncFunction;
    typedef std::function< void( KFEntity*, KFMsg::PBShowElement& ) > KFShowElementFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function< bool( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) > KFCheckAddElementFunction;
    typedef std::function< void( KFEntity*, KFData*, KFElementResult*, const char*, uint32, float ) > KFAddElementFunction;
    typedef std::function< bool( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) > KFCheckRemoveElementFunction;
    typedef std::function< void( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) > KFRemoveElementFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<void( KFEntity*, KFData*, uint64, KFData* )> KFAddDataFunction;
    typedef std::function<void( KFEntity*, KFData*, uint64, KFData* )> KFRemoveDataFunction;
    typedef std::function<void( KFEntity*, uint64, KFData*, uint32, uint64, uint64, uint64 )> KFUpdateDataFunction;
    typedef std::function<void( KFEntity*, KFData*, const std::string& )> KFUpdateStringFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<uint64( KFEntity*, uint64, uint64 )> KFGetConfigValueFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function< void( KFEntity*, const std::string&, const KFElementResult* ) > KFLogElementFunction;

    // 游戏中的组件, 负责属性回调时间
    class KFComponent
    {
    public:
        KFComponent() = default;
        virtual ~KFComponent() = default;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建实体
        virtual KFEntity* CreateEntity( uint64 key ) = 0;
        virtual KFEntity* CreateEntity( uint64 key, const KFMsg::PBObject* data ) = 0;

        // 添加实体
        virtual void AddEntity( uint64 key, KFEntity* kfentity ) = 0;

        // 查找实体
        virtual KFEntity* FindEntity( uint64 key ) = 0;
        virtual KFEntity* FindEntity( uint64 key, const char* function, uint32 line ) = 0;

        // 删除实体, 保存数据库
        virtual bool RemoveEntity( uint64 key ) = 0;
        virtual bool RemoveEntity( KFEntity* kfentity ) = 0;

        // 实体数量
        virtual uint32 GetEntityCount() = 0;

        // 遍历列表
        virtual KFEntity* FirstEntity() = 0;
        virtual KFEntity* NextEntity() = 0;

        // 获得属性列表
        virtual StringVector& GetDataList( const std::string& dataname ) = 0;

        // 获得类配置
        virtual const KFClassSetting* FindClassSetting() = 0;
        virtual const KFClassSetting* FindClassSetting( const std::string& dataname ) = 0;

        // 获得属性配置
        virtual const KFDataSetting* FindDataSetting() = 0;
        virtual const KFDataSetting* FindDataSetting( const std::string& dataname ) = 0;
        virtual const KFDataSetting* FindDataSetting( const std::string& parentname, const std::string& dataname ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetLogicValue( const std::string& dataname, uint32 value ) = 0;
        virtual void SetLogicValue( const std::string& parentname, const std::string& dataname, uint32 value ) = 0;

        virtual void SetMaxValue( const std::string& dataname, uint32 value ) = 0;
        virtual void SetMaxValue( const std::string& parentname, const std::string& dataname, uint32 value ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册判断属性函数
        template< class T >
        void RegisterCheckAddElementFunction( const std::string& dataname, T* object, bool ( T::*handle )( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) )
        {
            KFCheckAddElementFunction function = std::bind( handle, object,
                                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                 std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindCheckAddElementFunction( dataname, function );
        }
        virtual void UnRegisterCheckAddElementFunction( const std::string& dataname ) = 0;

        // 注册添加属性函数
        template< class T >
        void RegisterAddElementFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity*, KFData*, KFElementResult*, const char*, uint32, float ) )
        {
            KFAddElementFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                            std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindAddElementFunction( dataname, function );
        }
        virtual void UnRegisterAddElementFunction( const std::string& dataname ) = 0;

        // 注册判断属性函数
        template< class T >
        void RegisterCheckRemoveElementFunction( const std::string& dataname, T* object, bool ( T::*handle )( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) )
        {
            KFCheckRemoveElementFunction function = std::bind( handle, object,
                                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                    std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindCheckRemoveElementFunction( dataname, function );
        }
        virtual void UnRegisterCheckRemoveElementFunction( const std::string& dataname ) = 0;

        // 注册删除属性函数
        template< class T >
        void RegisterRemoveElementFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity*, KFData*, KFElement*, const char*, uint32, float ) )
        {
            KFRemoveElementFunction function = std::bind( handle, object,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                               std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindRemoveElementFunction( dataname, function );
        }
        virtual void UnRegisterRemoveElementFunction( const std::string& dataname ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册添加属性回调函数
        template< class T >
        void RegisterAddDataModule( T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFAddDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataModule( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterAddDataModule( T* object )
        {
            UnBindAddDataModule( typeid( T ).name() );
        }

        template< class T >
        void RegisterAddDataFunction( const std::string& dataname, uint64 key, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFAddDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataFunction( typeid( T ).name(), dataname, key, function );
        }

        template< class T >
        void UnRegisterAddDataFunction( T* object, const std::string& dataname, uint64 key )
        {
            UnBindAddDataFunction( typeid( T ).name(), dataname, key );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 注册删除属性回调函数
        template< class T >
        void RegisterRemoveDataModule( T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFRemoveDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataModule( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterRemoveDataModule( T* object )
        {
            UnBindRemoveDataModule( typeid( T ).name() );
        }

        template< class T >
        void RegisterRemoveDataFunction( const std::string& dataname, uint64 key, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFRemoveDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataFunction( typeid( T ).name(), dataname, key, function );
        }

        template< class T >
        void UnRegisterRemoveDataFunction( T* object, const std::string& dataname, uint64 key )
        {
            UnBindRemoveDataFunction( typeid( T ).name(), dataname, key );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册更新属性回调函数
        template< class T >
        void RegisterUpdateDataModule( T* object, void ( T::*handle )( KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue ) )
        {
            KFUpdateDataFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataModule( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterUpdateDataModule( T* object )
        {
            UnBindUpdateDataModule( typeid( T ).name() );
        }

        template< class T >
        void RegisterUpdateDataFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue ) )
        {
            KFUpdateDataFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataFunction( typeid( T ).name(), _invalid_string, dataname, function );
        }

        template< class T >
        void RegisterUpdateDataFunction( const std::string& parentname, const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue ) )
        {
            KFUpdateDataFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataFunction( typeid( T ).name(), parentname, dataname, function );
        }

        template< class T >
        void UnRegisterUpdateDataFunction( T* object, const std::string& dataname )
        {
            UnBindUpdateDataFunction( typeid( T ).name(), _invalid_string, dataname );
        }

        template< class T >
        void UnRegisterUpdateDataFunction( T* object, const std::string& parentname, const std::string& dataname )
        {
            UnBindUpdateDataFunction( typeid( T ).name(), parentname, dataname );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册更新属性回调函数
        template< class T >
        void RegisterUpdateStringModule( T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
        {
            KFUpdateStringFunction function = std::bind( handle, object,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindUpdateStringModule( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterUpdateStringModule( T* object )
        {
            UnBindUpdateStringModule( typeid( T ).name() );
        }

        template< class T >
        void RegisterUpdateStringFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
        {
            KFUpdateStringFunction function = std::bind( handle, object,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindUpdateStringFunction( typeid( T ).name(), _invalid_string, dataname, function );
        }

        template< class T >
        void RegisterUpdateStringFunction( const std::string& parentname, const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
        {
            KFUpdateStringFunction function = std::bind( handle, object,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindUpdateStringFunction( typeid( T ).name(), parentname, dataname, function );
        }

        template< class T >
        void UnRegisterUpdateStringFunction( T* object, const std::string& dataname )
        {
            UnBindUpdateStringFunction( typeid( T ).name(), _invalid_string, dataname );
        }

        template< class T >
        void UnRegisterUpdateStringFunction( T* object, const std::string& parentname, const std::string& dataname )
        {
            UnBindUpdateStringFunction( typeid( T ).name(), parentname, dataname );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterEntityInitializeFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindEntityInitializeFunction( function );
        }
        virtual void UnRegisterEntityInitializeFunction() = 0;

        template< class T >
        void RegisterEntityUninitializeFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindEntityUninitializeFunction( function );
        }
        virtual void UnRegisterEntityUninitializeFunction() = 0;

        template< class T >
        void RegisterEntityRunFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindEntityRunFunction( function );
        }
        virtual void UnRegisterEntityRunFunction() = 0;

        template< class T >
        void RegisterEntityAfterRunFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindEntityAfterRunFunction( function );
        }
        virtual void UnRegisterEntityAfterRunFunction() = 0;

        template< class T >
        void RegisterEntitySaveFunction( T* object, void ( T::*handle )( KFEntity* kfentity, uint32 flag ) )
        {
            KFSaveEntityFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindEntitySaveFunction( function );
        }
        virtual void UnRegisterEntitySaveFunction() = 0;

        template< class T >
        void RegisterEntityDeleteFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            BindEntityDeleteFunction( function );
        }
        virtual void UnRegisterEntityDeleteFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterSyncUpdateFunction( T* object, void ( T::*handle )( KFEntity* kfentity, KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncUpdateFunction( function );
        }
        virtual void UnRegisterSyncUpdateFunction() = 0;

        template< class T >
        void RegisterSyncAddFunction( T* object, void ( T::*handle )( KFEntity* kfentity, KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncAddFunction( function );
        }
        virtual void UnRegisterSyncAddFunction() = 0;

        template< class T >
        void RegisterSyncRemoveFunction( T* object, void ( T::*handle )( KFEntity* kfentity, KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncRemoveFunction( function );
        }
        virtual void UnRegisterSyncRemoveFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterShowElementFunction( T* object, void ( T::*handle )( KFEntity*, KFMsg::PBShowElement& ) )
        {
            KFShowElementFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindShowElementFunction( function );
        }
        virtual void UnRegisterShowElementFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterGetConfigValueFunction( const std::string& name, T* object, uint64 ( T::*handle )( KFEntity*, uint64, uint64 ) )
        {
            KFGetConfigValueFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindGetConfigValueFunction( name, function );
        }

        void UnRegisterGetConfigValueFunction( const std::string& name )
        {
            UnBindGetConfigValueFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterLogElementFunction( const std::string& name, T* object, void( T::*handle )( KFEntity*, const std::string&, const KFElementResult* ) )
        {
            KFLogElementFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindLogElementFunction( name, function );
        }

        void UnRegisterLogElementFunction( const std::string& name )
        {
            UnBindLogElementFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void BindCheckAddElementFunction( const std::string& dataname, KFCheckAddElementFunction& function ) = 0;
        virtual void BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function ) = 0;
        virtual void BindCheckRemoveElementFunction( const std::string& dataname, KFCheckRemoveElementFunction& function ) = 0;
        virtual void BindRemoveElementFunction( const std::string& dataname, KFRemoveElementFunction& function ) = 0;

        virtual void BindAddDataModule( const std::string& module, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataModule( const std::string& module ) = 0;
        virtual void BindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key ) = 0;

        virtual void BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataModule( const std::string& module ) = 0;
        virtual void BindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key ) = 0;

        virtual void BindUpdateDataModule( const std::string& module, KFUpdateDataFunction& function ) = 0;
        virtual void UnBindUpdateDataModule( const std::string& module ) = 0;
        virtual void BindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function ) = 0;
        virtual void UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname ) = 0;

        virtual void BindUpdateStringModule( const std::string& module, KFUpdateStringFunction& function ) = 0;
        virtual void UnBindUpdateStringModule( const std::string& module ) = 0;
        virtual void BindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function ) = 0;
        virtual void UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname ) = 0;

        virtual void BindEntityInitializeFunction( KFEntityFunction& function ) = 0;
        virtual void BindEntityUninitializeFunction( KFEntityFunction& function ) = 0;
        virtual void BindEntityAfterRunFunction( KFEntityFunction& function ) = 0;
        virtual void BindEntityRunFunction( KFEntityFunction& function ) = 0;
        virtual void BindEntityDeleteFunction( KFEntityFunction& function ) = 0;
        virtual void BindSyncUpdateFunction( KFSyncFunction& function ) = 0;
        virtual void BindSyncAddFunction( KFSyncFunction& function ) = 0;
        virtual void BindSyncRemoveFunction( KFSyncFunction& function ) = 0;
        virtual void BindShowElementFunction( KFShowElementFunction& function ) = 0;
        virtual void BindEntitySaveFunction( KFSaveEntityFunction& function ) = 0;

        virtual void BindGetConfigValueFunction( const std::string& name, KFGetConfigValueFunction& function ) = 0;
        virtual void UnBindGetConfigValueFunction( const std::string& name ) = 0;

        virtual void BindLogElementFunction( const std::string& name, KFLogElementFunction& function ) = 0;
        virtual void UnBindLogElementFunction( const std::string& name ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_CHECK_ADD_ELEMENT_FUNCTION__( checkfunction ) \
    bool checkfunction( KFEntity* player, KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple )

#define  __REGISTER_CHECK_ADD_ELEMENT__( dataname, function )\
    _kf_component->RegisterCheckAddElementFunction( dataname, this, function )
#define  __UN_CHECK_ADD_ELEMENT__( dataname )\
    _kf_component->UnRegisterCheckAddElementFunction( dataname  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ADD_ELEMENT_FUNCTION__( addfunction ) \
    void addfunction( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const char* function, uint32 line, float multiple )

#define  __REGISTER_ADD_ELEMENT__( dataname, function ) \
    _kf_component->RegisterAddElementFunction( dataname, this, function )
#define  __UN_ADD_ELEMENT__( dataname ) \
    _kf_component->UnRegisterAddElementFunction( dataname  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( checkfunction ) \
    bool checkfunction( KFEntity* player, KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple )

#define  __REGISTER_CHECK_REMOVE_ELEMENT__( dataname, function )\
    _kf_component->RegisterCheckRemoveElementFunction( dataname, this, function )
#define  __UN_CHECK_REMOVE_ELEMENT__( dataname )\
    _kf_component->UnRegisterCheckRemoveElementFunction( dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_REMOVE_ELEMENT_FUNCTION__( removefunction ) \
    void removefunction( KFEntity* player, KFData* kfparent, KFElement* kfelement, const char* function, uint32 line, float multiple)

#define __REGISTER_REMOVE_ELEMENT__( dataname, function ) \
    _kf_component->RegisterRemoveElementFunction( dataname, this, function )
#define __UN_REMOVE_ELEMENT__( dataname ) \
    _kf_component->UnRegisterRemoveElementFunction( dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ADD_DATA_FUNCTION__( addfunction ) \
    void addfunction( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )

#define __REGISTER_ADD_DATA__( function )\
    _kf_component->RegisterAddDataModule( this, function )
#define __UN_ADD_DATA__()\
    _kf_component->UnRegisterAddDataModule( this )

#define __REGISTER_ADD_DATA_1__( dataname, function )\
    _kf_component->RegisterAddDataFunction( dataname, 0u, this, function )
#define __REGISTER_ADD_DATA_2__( dataname, key, function )\
    _kf_component->RegisterAddDataFunction( dataname, key, this, function )

#define __UN_ADD_DATA_1__( dataname )\
    _kf_component->UnRegisterAddDataFunction( this, dataname, 0u )
#define __UN_ADD_DATA_2__( dataname, key )\
    _kf_component->UnRegisterAddDataFunction( this, dataname, key  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_REMOVE_DATA_FUNCTION__( removefunction ) \
    void removefunction( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )

#define __REGISTER_REMOVE_DATA__( function )\
    _kf_component->RegisterRemoveDataModule( this, function )
#define __UN_REMOVE_DATA__()\
    _kf_component->UnRegisterRemoveDataModule( this  )

#define __REGISTER_REMOVE_DATA_1__( dataname, function )\
    _kf_component->RegisterRemoveDataFunction( dataname, 0u, this, function )
#define __REGISTER_REMOVE_DATA_2__( dataname, key, function )\
    _kf_component->RegisterRemoveDataFunction( dataname, key, this, function )
#define __UN_REMOVE_DATA_1__( dataname )\
    _kf_component->UnRegisterRemoveDataFunction( this, dataname, 0u )
#define __UN_REMOVE_DATA_2__( dataname, key )\
    _kf_component->UnRegisterRemoveDataFunction( this, dataname, key )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_UPDATE_DATA_FUNCTION__( updatefunction ) \
    void updatefunction( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )

#define  __REGISTER_UPDATE_DATA__( function )\
    _kf_component->RegisterUpdateDataModule( this, function )
#define  __REGISTER_UPDATE_DATA_1__( dataname, updatefunction)\
    _kf_component->RegisterUpdateDataFunction( dataname, this, updatefunction )
#define  __REGISTER_UPDATE_DATA_2__( parentname, dataname, updatefunction)\
    _kf_component->RegisterUpdateDataFunction( parentname, dataname, this, updatefunction )

#define  __UN_UPDATE_DATA__()\
    _kf_component->UnRegisterUpdateDataModule( this )
#define  __UN_UPDATE_DATA_1__( dataname )\
    _kf_component->UnRegisterUpdateDataFunction( this, dataname )
#define  __UN_UPDATE_DATA_2__( parentname, dataname )\
    _kf_component->UnRegisterUpdateDataFunction( this, parentname, dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_UPDATE_STRING_FUNCTION__( updatefunction ) \
    void updatefunction( KFEntity* player, KFData* kfdata, const std::string& value )

#define  __REGISTER_UPDATE_STRING__( updatefunction)\
    _kf_component->RegisterUpdateStringModule( this, updatefunction )
#define  __REGISTER_UPDATE_STRING_1__( dataname, updatefunction)\
    _kf_component->RegisterUpdateStringFunction( dataname, this, updatefunction )
#define  __REGISTER_UPDATE_STRING_2__( parentname, dataname, updatefunction)\
    _kf_component->RegisterUpdateStringFunction( parentname, dataname, this, updatefunction )

#define  __UN_UPDATE_STRING__()\
    _kf_component->UnRegisterUpdateStringModule( this )
#define  __UN_UPDATE_STRING_1__( dataname )\
    _kf_component->UnRegisterUpdateStringFunction( this, dataname )
#define  __UN_UPDATE_STRING_2__( parentname, dataname )\
    _kf_component->UnRegisterUpdateStringFunction( this, parentname, dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_GET_CONFIG_VALUE_FUNCTION__( function ) \
    uint64 function( KFEntity* player, uint64 id, uint64 maxvalue )

#define __REGISTER_GET_CONFIG_VALUE__( dataname, function ) \
    _kf_component->RegisterGetConfigValueFunction( dataname, this, function )
#define __UN_GET_CONFIG_VALUE__( dataname ) \
    _kf_component->UnRegisterGetConfigValueFunction( dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_LOG_ELEMENT_FUNCTION__( function ) \
    void function( KFEntity* player, const std::string& modulename, const KFElementResult* kfresult )

#define __REGISTER_LOG_ELEMENT__( dataname, function ) \
    _kf_component->RegisterLogElementFunction( dataname, this, function )
#define __UN_LOG_ELEMENT__( dataname ) \
    _kf_component->UnRegisterLogElementFunction( dataname )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif