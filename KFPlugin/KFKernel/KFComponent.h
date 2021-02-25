#ifndef __KF_COMPONENT_H__
#define __KF_COMPONENT_H__

#include "KFEntity.h"
#include "KFElementResult.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<void( EntityPtr )> KFEntityFunction;
    typedef std::function<void( EntityPtr, uint32 )> KFSaveEntityFunction;
    typedef std::function<void( EntityPtr, KFMsg::PBObject& )> KFSyncFunction;
    typedef std::function<void( EntityPtr, KFMsg::PBShowElements& )> KFShowElementFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<bool( EntityPtr, DataPtr, std::shared_ptr<KFElement>, double, const char*, uint32 )> KFCheckAddElementFunction;
    typedef std::function<bool( EntityPtr, DataPtr, KFElementResult*, const char*, uint32 )> KFAddElementFunction;
    typedef std::function<bool( EntityPtr, DataPtr, std::shared_ptr<KFElement>, double, const char*, uint32 )> KFCheckRemoveElementFunction;
    typedef std::function<bool( EntityPtr, DataPtr, KFElementResult*, const char*, uint32 )> KFRemoveElementFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<void( EntityPtr, DataPtr, uint64, DataPtr )> KFAddDataFunction;
    typedef std::function<void( EntityPtr, DataPtr, uint64, DataPtr )> KFRemoveDataFunction;
    typedef std::function<void( EntityPtr, uint64, DataPtr, uint32, uint64, uint64, uint64 )> KFUpdateDataFunction;
    typedef std::function<void( EntityPtr, DataPtr, const std::string&, const std::string& )> KFUpdateStringFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<uint64( EntityPtr, uint64, uint64 )> KFGetConfigValueFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::function<void( EntityPtr, const KFElementResult* )> KFLogElementFunction;
    typedef std::function<bool( EntityPtr, const KFElementResult* )> KFElementResultFunction;

    // 游戏中的组件, 负责属性回调事件
    class KFComponent : public KFModule
    {
    public:
        KFComponent() = default;
        virtual ~KFComponent() = default;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建实体
        virtual EntityPtr CreateEntity( uint64 key ) = 0;
        virtual EntityPtr CreateEntity( uint64 key, const KFMsg::PBObject* data ) = 0;

        // 添加实体
        virtual void AddEntity( uint64 key, EntityPtr entity ) = 0;

        // 查找实体
        virtual EntityPtr FindEntity( uint64 key ) = 0;
        virtual EntityPtr FindEntity( uint64 key, const char* function, uint32 line ) = 0;

        // 删除实体, 保存数据库
        virtual bool RemoveEntity( uint64 key ) = 0;
        virtual bool RemoveEntity( EntityPtr entity ) = 0;

        // 实体数量
        virtual uint32 GetEntityCount() = 0;

        // 遍历列表
        virtual EntityPtr FirstEntity() = 0;
        virtual EntityPtr NextEntity() = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得属性列表
        virtual StringVector& GetDataList( const std::string& data_name ) = 0;

        // 获得属性配置
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting() = 0;
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting( const std::string& data_name ) = 0;
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting( const std::string& parent_name, const std::string& data_name ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册判断属性函数
        template<class T>
        void RegisterCheckAddElementFunction( const std::string& data_name, T* module, bool ( T::*handle )( EntityPtr, DataPtr, std::shared_ptr<KFElement>, double, const char*, uint32 ) )
        {
            KFCheckAddElementFunction function = std::bind( handle, module,
                                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                 std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindCheckAddElementFunction( module, data_name,function );
        }
        virtual void UnRegisterCheckAddElementFunction( const std::string& data_name ) = 0;

        // 注册添加属性函数
        template<class T>
        void RegisterAddElementFunction( const std::string& data_name, T* module, bool ( T::*handle )( EntityPtr, DataPtr, KFElementResult*, const char*, uint32 ) )
        {
            KFAddElementFunction function = std::bind( handle, module,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                            std::placeholders::_4, std::placeholders::_5 );
            BindAddElementFunction( module, data_name, function );
        }
        virtual void UnRegisterAddElementFunction( const std::string& data_name ) = 0;

        // 注册判断属性函数
        template<class T>
        void RegisterCheckRemoveElementFunction( const std::string& data_name, T* module, bool ( T::*handle )( EntityPtr, DataPtr, std::shared_ptr<KFElement>, double, const char*, uint32 ) )
        {
            KFCheckRemoveElementFunction function = std::bind( handle, module,
                                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                    std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindCheckRemoveElementFunction( module, data_name, function );
        }
        virtual void UnRegisterCheckRemoveElementFunction( const std::string& data_name ) = 0;

        // 注册删除属性函数
        template<class T>
        void RegisterRemoveElementFunction( const std::string& data_name, T* module, bool ( T::*handle )( EntityPtr, DataPtr, KFElementResult*, const char*, uint32 ) )
        {
            KFRemoveElementFunction function = std::bind( handle, module,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                               std::placeholders::_4, std::placeholders::_5 );
            BindRemoveElementFunction( module, data_name, function );
        }
        virtual void UnRegisterRemoveElementFunction( const std::string& data_name ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册添加属性逻辑
        template<class T>
        void RegisterAddDataLogic( const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFAddDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataLogic( module, data_name, function );
        }

        template<class T>
        void UnRegisterAddDataLogic( const std::string& data_name, T* module )
        {
            UnBindAddDataLogic( module, data_name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册添加属性回调函数
        template<class T>
        void RegisterAddDataModule( T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFAddDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataModule( module, function );
        }

        template<class T>
        void UnRegisterAddDataModule( T* module )
        {
            UnBindAddDataModule( module );
        }

        template<class T>
        void RegisterAddDataFunction( const std::string& data_name, uint64 key, T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFAddDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataFunction( module, data_name, key, function );
        }

        template<class T>
        void UnRegisterAddDataFunction( T* module, const std::string& data_name, uint64 key )
        {
            UnBindAddDataFunction( module, data_name, key );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册删除属性逻辑
        template<class T>
        void RegisterRemoveDataLogic( const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFRemoveDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataLogic( module, data_name, function );
        }

        template<class T>
        void UnRegisterRemoveDataLogic( const std::string& data_name, T* module )
        {
            UnBindRemoveDataLogic( module, data_name );
        }

        // 注册删除属性回调函数
        template<class T>
        void RegisterRemoveDataModule( T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFRemoveDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataModule( module, function );
        }

        template<class T>
        void UnRegisterRemoveDataModule( T* module )
        {
            UnBindRemoveDataModule( module );
        }

        template<class T>
        void RegisterRemoveDataFunction( const std::string& data_name, uint64 key, T* module, void ( T::*handle )( EntityPtr, DataPtr, uint64, DataPtr ) )
        {
            KFRemoveDataFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataFunction( module, data_name, key, function );
        }

        template<class T>
        void UnRegisterRemoveDataFunction( T* module, const std::string& data_name, uint64 key )
        {
            UnBindRemoveDataFunction( module, data_name, key );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册更新属性回调函数
        template<class T>
        void RegisterUpdateDataModule( T* module, void ( T::*handle )( EntityPtr, uint64, DataPtr, uint32, uint64, uint64, uint64 ) )
        {
            KFUpdateDataFunction function = std::bind( handle, module,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataModule( module, function );
        }

        template<class T>
        void UnRegisterUpdateDataModule( T* module )
        {
            UnBindUpdateDataModule( module );
        }

        template<class T>
        void RegisterUpdateDataFunction( const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, uint64, DataPtr, uint32, uint64, uint64, uint64 ) )
        {
            KFUpdateDataFunction function = std::bind( handle, module,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataFunction( module, _invalid_string, data_name, function );
        }

        template<class T>
        void RegisterUpdateDataFunction( const std::string& parent_name, const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, uint64, DataPtr, uint32, uint64, uint64, uint64 ) )
        {
            KFUpdateDataFunction function = std::bind( handle, module,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataFunction( module, parent_name, data_name, function );
        }

        template<class T>
        void UnRegisterUpdateDataFunction( T* module, const std::string& data_name )
        {
            UnBindUpdateDataFunction( module, _invalid_string, data_name );
        }

        template<class T>
        void UnRegisterUpdateDataFunction( T* module, const std::string& parent_name, const std::string& data_name )
        {
            UnBindUpdateDataFunction( module, parent_name, data_name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册更新属性回调函数
        template<class T>
        void RegisterUpdateStringModule( T* module, void ( T::*handle )( EntityPtr, DataPtr, const std::string&, const std::string& ) )
        {
            KFUpdateStringFunction function = std::bind( handle, module,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindUpdateStringModule( module, function );
        }

        template<class T>
        void UnRegisterUpdateStringModule( T* module )
        {
            UnBindUpdateStringModule( module );
        }

        template<class T>
        void RegisterUpdateStringFunction( const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, DataPtr, const std::string&, const std::string& ) )
        {
            KFUpdateStringFunction function = std::bind( handle, module,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindUpdateStringFunction( module, _invalid_string, data_name, function );
        }

        template<class T>
        void RegisterUpdateStringFunction( const std::string& parent_name, const std::string& data_name, T* module, void ( T::*handle )( EntityPtr, DataPtr, const std::string&, const std::string& ) )
        {
            KFUpdateStringFunction function = std::bind( handle, module,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindUpdateStringFunction( module, parent_name, data_name, function );
        }

        template<class T>
        void UnRegisterUpdateStringFunction( T* module, const std::string& data_name )
        {
            UnBindUpdateStringFunction( module, _invalid_string, data_name );
        }

        template<class T>
        void UnRegisterUpdateStringFunction( T* module, const std::string& parent_name, const std::string& data_name )
        {
            UnBindUpdateStringFunction( module, parent_name, data_name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterEntityRunFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            BindEntityRunFunction( module, function );
        }
        virtual void UnRegisterEntityRunFunction() = 0;

        template<class T>
        void RegisterEntityAfterRunFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            BindEntityAfterRunFunction( module, function );
        }
        virtual void UnRegisterEntityAfterRunFunction() = 0;

        template<class T>
        void RegisterEntitySaveFunction( T* module, void ( T::*handle )( EntityPtr, uint32 ) )
        {
            KFSaveEntityFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindEntitySaveFunction( module, function );
        }
        virtual void UnRegisterEntitySaveFunction() = 0;

        template<class T>
        void RegisterEntityDeleteFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            BindEntityDeleteFunction( module, function );
        }
        virtual void UnRegisterEntityDeleteFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterSyncUpdateFunction( T* module, void ( T::*handle )( EntityPtr, KFMsg::PBObject& ) )
        {
            KFSyncFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindSyncUpdateFunction( module, function );
        }
        virtual void UnRegisterSyncUpdateFunction() = 0;

        template<class T>
        void RegisterSyncAddFunction( T* module, void ( T::*handle )( EntityPtr, KFMsg::PBObject& ) )
        {
            KFSyncFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindSyncAddFunction( module, function );
        }
        virtual void UnRegisterSyncAddFunction() = 0;

        template<class T>
        void RegisterSyncRemoveFunction( T* module, void ( T::*handle )( EntityPtr, KFMsg::PBObject& ) )
        {
            KFSyncFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindSyncRemoveFunction( module, function );
        }
        virtual void UnRegisterSyncRemoveFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterShowElementFunction( T* module, void ( T::*handle )( EntityPtr, KFMsg::PBShowElements& ) )
        {
            KFShowElementFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindShowElementFunction( module, function );
        }
        virtual void UnRegisterShowElementFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterGetConfigValueFunction( const std::string& name, T* module, uint64 ( T::*handle )( EntityPtr, uint64, uint64 ) )
        {
            KFGetConfigValueFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindGetConfigValueFunction( module, name, function );
        }

        void UnRegisterGetConfigValueFunction( const std::string& name )
        {
            UnBindGetConfigValueFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterLogElementFunction( const std::string& name, T* module, void( T::*handle )( EntityPtr, const KFElementResult* ) )
        {
            KFLogElementFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindLogElementFunction( module, name, function );
        }

        void UnRegisterLogElementFunction( const std::string& name )
        {
            UnBindLogElementFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterElementResultFunction( const std::string& name, T* module, bool( T::*handle )( EntityPtr, const KFElementResult* ) )
        {
            KFElementResultFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2 );
            BindElementResultFunction( module, name, function );
        }

        void UnRegisterElementResultFunction( const std::string& name )
        {
            UnBindElementResultFunction( name );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void BindCheckAddElementFunction( KFModule* module, const std::string& data_name, KFCheckAddElementFunction& function ) = 0;
        virtual void BindAddElementFunction( KFModule* module, const std::string& data_name, KFAddElementFunction& function ) = 0;
        virtual void BindCheckRemoveElementFunction( KFModule* module, const std::string& data_name, KFCheckRemoveElementFunction& function ) = 0;
        virtual void BindRemoveElementFunction( KFModule* module, const std::string& data_name, KFRemoveElementFunction& function ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindAddDataLogic( KFModule* module, const std::string& data_name, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataLogic( KFModule* module, const std::string& data_name ) = 0;

        virtual void BindAddDataModule( KFModule* module, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataModule( KFModule* module ) = 0;
        virtual void BindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual void BindRemoveDataLogic( KFModule* module, const std::string& data_name, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataLogic( KFModule* module, const std::string& data_name ) = 0;

        virtual void BindRemoveDataModule( KFModule* module, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataModule( KFModule* module ) = 0;
        virtual void BindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual void BindUpdateDataModule( KFModule* module, KFUpdateDataFunction& function ) = 0;
        virtual void UnBindUpdateDataModule( KFModule* module ) = 0;
        virtual void BindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateDataFunction& function ) = 0;
        virtual void UnBindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual void BindUpdateStringModule( KFModule* module, KFUpdateStringFunction& function ) = 0;
        virtual void UnBindUpdateStringModule( KFModule* module ) = 0;
        virtual void BindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateStringFunction& function ) = 0;
        virtual void UnBindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindEntityAfterRunFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void BindEntityRunFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void BindEntityDeleteFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void BindSyncUpdateFunction( KFModule* module, KFSyncFunction& function ) = 0;
        virtual void BindSyncAddFunction( KFModule* module, KFSyncFunction& function ) = 0;
        virtual void BindSyncRemoveFunction( KFModule* module, KFSyncFunction& function ) = 0;
        virtual void BindShowElementFunction( KFModule* module, KFShowElementFunction& function ) = 0;
        virtual void BindEntitySaveFunction( KFModule* module, KFSaveEntityFunction& function ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindGetConfigValueFunction( KFModule* module, const std::string& data_name, KFGetConfigValueFunction& function ) = 0;
        virtual void UnBindGetConfigValueFunction( const std::string& data_name ) = 0;

        virtual void BindLogElementFunction( KFModule* module, const std::string& data_name, KFLogElementFunction& function ) = 0;
        virtual void UnBindLogElementFunction( const std::string& data_name ) = 0;

        virtual void BindElementResultFunction( KFModule* module, const std::string& data_name, KFElementResultFunction& function ) = 0;
        virtual void UnBindElementResultFunction( const std::string& data_name ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_CHECK_ADD_ELEMENT_FUNCTION__( check_function ) \
    bool check_function( EntityPtr player, DataPtr parent_data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )

#define  __REGISTER_CHECK_ADD_ELEMENT__( data_name, function )\
    _component->RegisterCheckAddElementFunction( data_name, this, function )
#define  __UN_CHECK_ADD_ELEMENT__( data_name )\
    _component->UnRegisterCheckAddElementFunction( data_name  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ADD_ELEMENT_FUNCTION__( add_function ) \
    bool add_function( EntityPtr player, DataPtr parent_data, KFElementResult* element_result, const char* function, uint32 line )

#define  __REGISTER_ADD_ELEMENT__( data_name, function ) \
    _component->RegisterAddElementFunction( data_name, this, function )
#define  __UN_ADD_ELEMENT__( data_name ) \
    _component->UnRegisterAddElementFunction( data_name  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( check_function ) \
    bool check_function( EntityPtr player, DataPtr parent_data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )

#define  __REGISTER_CHECK_REMOVE_ELEMENT__( data_name, function )\
    _component->RegisterCheckRemoveElementFunction( data_name, this, function )
#define  __UN_CHECK_REMOVE_ELEMENT__( data_name )\
    _component->UnRegisterCheckRemoveElementFunction( data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_REMOVE_ELEMENT_FUNCTION__( remove_function ) \
    bool remove_function( EntityPtr player, DataPtr parent_data, KFElementResult* element_result, const char* function, uint32 line )

#define __REGISTER_REMOVE_ELEMENT__( data_name, function ) \
    _component->RegisterRemoveElementFunction( data_name, this, function )
#define __UN_REMOVE_ELEMENT__( data_name ) \
    _component->UnRegisterRemoveElementFunction( data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ADD_DATA_FUNCTION__( add_function ) \
    void add_function( EntityPtr player, DataPtr parent_data, uint64 key, DataPtr data )

#define __REGISTER_ADD_LOGIC__( data_name, function )\
    _component->RegisterAddDataLogic( data_name, this, function )
#define __UN_ADD_LOGIC__( data_name )\
    _component->UnRegisterAddDataLogic( data_name, this )

#define __REGISTER_ADD_DATA__( function )\
    _component->RegisterAddDataModule( this, function )
#define __UN_ADD_DATA__()\
    _component->UnRegisterAddDataModule( this )

#define __REGISTER_ADD_DATA_1__( data_name, function )\
    _component->RegisterAddDataFunction( data_name, 0u, this, function )
#define __REGISTER_ADD_DATA_2__( data_name, key, function )\
    _component->RegisterAddDataFunction( data_name, key, this, function )

#define __UN_ADD_DATA_1__( data_name )\
    _component->UnRegisterAddDataFunction( this, data_name, 0u )
#define __UN_ADD_DATA_2__( data_name, key )\
    _component->UnRegisterAddDataFunction( this, data_name, key  )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_REMOVE_DATA_FUNCTION__( remove_function ) \
    void remove_function( EntityPtr player, DataPtr parent_data, uint64 key, DataPtr data )

#define __REGISTER_REMOVE_LOGIC__( data_name, function )\
    _component->RegisterRemoveDataLogic( data_name, this, function )
#define __UN_REMOVE_LOGIC__( data_name )\
    _component->UnRegisterRemoveDataLogic( data_name, this )

#define __REGISTER_REMOVE_DATA__( function )\
    _component->RegisterRemoveDataModule( this, function )
#define __UN_REMOVE_DATA__()\
    _component->UnRegisterRemoveDataModule( this  )

#define __REGISTER_REMOVE_DATA_1__( data_name, function )\
    _component->RegisterRemoveDataFunction( data_name, 0u, this, function )
#define __REGISTER_REMOVE_DATA_2__( data_name, key, function )\
    _component->RegisterRemoveDataFunction( data_name, key, this, function )
#define __UN_REMOVE_DATA_1__( data_name )\
    _component->UnRegisterRemoveDataFunction( this, data_name, 0u )
#define __UN_REMOVE_DATA_2__( data_name, key )\
    _component->UnRegisterRemoveDataFunction( this, data_name, key )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_UPDATE_DATA_FUNCTION__( update_function ) \
    void update_function( EntityPtr player, uint64 key, DataPtr data, uint32 operate, uint64 value, uint64 old_value, uint64 new_value )

#define  __REGISTER_UPDATE_DATA__( function )\
    _component->RegisterUpdateDataModule( this, function )
#define  __REGISTER_UPDATE_DATA_1__( data_name, update_function)\
    _component->RegisterUpdateDataFunction( data_name, this, update_function )
#define  __REGISTER_UPDATE_DATA_2__( parent_name, data_name, update_function)\
    _component->RegisterUpdateDataFunction( parent_name, data_name, this, update_function )

#define  __UN_UPDATE_DATA__()\
    _component->UnRegisterUpdateDataModule( this )
#define  __UN_UPDATE_DATA_1__( data_name )\
    _component->UnRegisterUpdateDataFunction( this, data_name )
#define  __UN_UPDATE_DATA_2__( parent_name, data_name )\
    _component->UnRegisterUpdateDataFunction( this, parent_name, data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_UPDATE_STRING_FUNCTION__( update_function ) \
    void update_function( EntityPtr player, DataPtr data, const std::string& old_value, const std::string& new_value )

#define  __REGISTER_UPDATE_STRING__( update_function)\
    _component->RegisterUpdateStringModule( this, update_function )
#define  __REGISTER_UPDATE_STRING_1__( data_name, update_function)\
    _component->RegisterUpdateStringFunction( data_name, this, update_function )
#define  __REGISTER_UPDATE_STRING_2__( parent_name, data_name, update_function)\
    _component->RegisterUpdateStringFunction( parent_name, data_name, this, update_function )

#define  __UN_UPDATE_STRING__()\
    _component->UnRegisterUpdateStringModule( this )
#define  __UN_UPDATE_STRING_1__( data_name )\
    _component->UnRegisterUpdateStringFunction( this, data_name )
#define  __UN_UPDATE_STRING_2__( parent_name, data_name )\
    _component->UnRegisterUpdateStringFunction( this, parent_name, data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_GET_CONFIG_VALUE_FUNCTION__( function ) \
    uint64 function( EntityPtr player, uint64 id, uint64 max_value )

#define __REGISTER_GET_CONFIG_VALUE__( data_name, function ) \
    _component->RegisterGetConfigValueFunction( data_name, this, function )
#define __UN_GET_CONFIG_VALUE__( data_name ) \
    _component->UnRegisterGetConfigValueFunction( data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_LOG_ELEMENT_FUNCTION__( function ) \
    void function( EntityPtr player, const KFElementResult* kfresult )

#define __REGISTER_LOG_ELEMENT__( data_name, function ) \
    _component->RegisterLogElementFunction( data_name, this, function )
#define __UN_LOG_ELEMENT__( data_name ) \
    _component->UnRegisterLogElementFunction( data_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_ELEMENT_RESULT_FUNCTION__( function ) \
    bool function( EntityPtr player, const KFElementResult* kfresult )

#define __REGISTER_ELEMENT_RESULT__( module_name, function ) \
    _component->RegisterElementResultFunction( module_name, this, function )
#define __UN_ELEMENT_RESULT__( module_name ) \
    _component->UnRegisterElementResultFunction( module_name )
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif