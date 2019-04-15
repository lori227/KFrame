#ifndef __KF_COMPONENT_H__
#define __KF_COMPONENT_H__

#include "KFrame.h"
#include "KFEntity.h"
#include "KFCore/KFData.h"

namespace KFrame
{
#define __NEED_TO_SAVE__	  1		// 需要保存
#define	__DELETE_AND_REMOVE__ 2		// 删除数据库
#define __DELETE_AND_SAVE__   4		// 删除保存数据库


    typedef std::function< void( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) > KFAddElementFunction;
    typedef std::function< bool( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) > KFCheckElementFunction;
    typedef std::function< void( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) > KFRemoveElementFunction;

    typedef std::function<void( KFEntity*, KFData* kfparent, uint64 key, KFData* kfdata )> KFAddDataFunction;
    typedef std::function<void( KFEntity*, KFData* kfparent, uint64 key, KFData* kfdata )> KFRemoveDataFunction;
    typedef std::function<void( KFEntity*, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )> KFUpdateDataFunction;
    typedef std::function<void( KFEntity*, KFData* kfdata, const std::string& value )> KFUpdateStringFunction;

    typedef std::function< void( KFEntity* ) > KFEntityFunction;
    typedef std::function< void( KFEntity*, const KFMsg::PBObject&  ) > KFSyncFunction;
    typedef std::function< void( KFEntity*, const std::string& ) > KFShowElementFunction;


#define __KF_ADD_ELEMENT_FUNCTION__( addfunction ) \
    void addfunction( const char* function, uint32 line, KFEntity* player, KFData* kfparent, KFElement* kfelement, float multiple  )

#define  __KF_CHECK_ELEMENT_FUNCTION__( checkfunction ) \
    bool checkfunction( const char* function, uint32 line, KFEntity* player, KFData* kfparent, KFElement* kfelement, float multiple )

#define  __KF_REMOVE_ELEMENT_FUNCTION__( removefunction ) \
    void removefunction( const char* function, uint32 line, KFEntity* player, KFData* kfparent, KFElement* kfelement, float multiple )

#define __KF_ADD_DATA_FUNCTION__( addfunction ) \
    void addfunction( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )

#define __KF_REMOVE_DATA_FUNCTION__( removefunction ) \
    void removefunction( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )

#define __KF_UPDATE_DATA_FUNCTION__( updatefunction ) \
    void updatefunction( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )

#define  __KF_UPDATE_STRING_FUNCTION__( updatefunction ) \
    void updatefunction( KFEntity* player, KFData* kfdata, const std::string& value )
    ////////////////////////////////////////////////////////////////////////////////////////////

    // 游戏中的组件, 负责属性回调时间
    class KFComponent
    {
    public:

        virtual void InitEntity( KFEntity* kfentity ) = 0;
        virtual void UnInitEntity( KFEntity* kfentity ) = 0;

        virtual void Run() = 0;
        virtual void AfterRun() = 0;

        virtual void SetName( const std::string& name ) = 0;
        virtual const std::string& GetName() = 0;
        /////////////////////////////////////////////////////////////////
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

        // 设置保存标记
        virtual void SetEntityDataMask( uint32 mask, uint32 savedelaytime ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册添加属性函数
        template< class T >
        void RegisterAddElementFunction( const std::string& dataname, T* object, void ( T::*handle )( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) )
        {
            KFAddElementFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                            std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindAddElementFunction( dataname, function );
        }
        virtual void UnRegisterAddElementFunction( const std::string& dataname ) = 0;

        // 注册判断属性函数
        template< class T >
        void RegisterCheckElementFunction( const std::string& dataname, T* object, bool ( T::*handle )( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) )
        {
            KFCheckElementFunction function = std::bind( handle, object,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                              std::placeholders::_4, std::placeholders::_5, std::placeholders::_6 );
            BindCheckElementFunction( dataname, function );
        }
        virtual void UnRegisterCheckElementFunction( const std::string& dataname ) = 0;

        // 注册删除属性函数
        template< class T >
        void RegisterRemoveElementFunction( const std::string& dataname, T* object, void ( T::*handle )( const char*, uint32, KFEntity*, KFData*, KFElement*, float ) )
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
        void RegisterAddDataFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFAddDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindAddDataFunction( typeid( T ).name(), dataname, function );
        }

        template< class T >
        void UnRegisterAddDataFunction( T* object, const std::string& dataname )
        {
            UnBindAddDataFunction( typeid( T ).name(), dataname );
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
        void RegisterRemoveDataFunction( const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata ) )
        {
            KFRemoveDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindRemoveDataFunction( typeid( T ).name(), dataname, function );
        }

        template< class T >
        void UnRegisterRemoveDataFunction( T* object, const std::string& dataname )
        {
            UnBindRemoveDataFunction( typeid( T ).name(), dataname );
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
        void RegisterUpdateDataFunction( const std::string& parentname, const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue ) )
        {
            KFUpdateDataFunction function = std::bind( handle, object,
                                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                            std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
            BindUpdateDataFunction( typeid( T ).name(), parentname, dataname, function );
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
        void RegisterUpdateStringFunction( const std::string& parentname, const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
        {
            KFUpdateStringFunction function = std::bind( handle, object,
                                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindUpdateStringFunction( typeid( T ).name(), parentname, dataname, function );
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
        void RegisterEntitySaveFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
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
        void RegisterSyncUpdateFunction( T* object, void ( T::*handle )( KFEntity* kfentity, const KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncUpdateFunction( function );
        }
        virtual void UnRegisterSyncUpdateFunction() = 0;

        template< class T >
        void RegisterSyncAddFunction( T* object, void ( T::*handle )( KFEntity* kfentity, const KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncAddFunction( function );
        }
        virtual void UnRegisterSyncAddFunction() = 0;

        template< class T >
        void RegisterSyncRemoveFunction( T* object, void ( T::*handle )( KFEntity* kfentity, const KFMsg::PBObject& pbobject ) )
        {
            KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindSyncRemoveFunction( function );
        }
        virtual void UnRegisterSyncRemoveFunction() = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterShowElementFunction( T* object, void ( T::*handle )( KFEntity*, const std::string& ) )
        {
            KFShowElementFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindShowElementFunction( function );
        }
        virtual void UnRegisterShowElementFunction() = 0;

    protected:
        virtual void BindCheckElementFunction( const std::string& dataname, KFCheckElementFunction& function ) = 0;
        virtual void BindRemoveElementFunction( const std::string& dataname, KFRemoveElementFunction& function ) = 0;
        virtual void BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function ) = 0;

        virtual void BindAddDataModule( const std::string& module, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataModule( const std::string& module ) = 0;
        virtual void BindAddDataFunction( const std::string& module, const std::string& dataname, KFAddDataFunction& function ) = 0;
        virtual void UnBindAddDataFunction( const std::string& module, const std::string& dataname ) = 0;

        virtual void BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataModule( const std::string& module ) = 0;
        virtual void BindRemoveDataFunction( const std::string& module, const std::string& dataname, KFRemoveDataFunction& function ) = 0;
        virtual void UnBindRemoveDataFunction( const std::string& module, const std::string& dataname ) = 0;

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
        virtual void BindEntitySaveFunction( KFEntityFunction& function ) = 0;
    };
}

#endif