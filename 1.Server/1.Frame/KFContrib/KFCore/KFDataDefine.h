#ifndef __KF_DATA_DEFINE_H__
#define __KF_DATA_DEFINE_H__

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    namespace KFDataDefine
    {
        enum MyEnum
        {
            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////
            // 数据类型
            Type_Unknow = 0,
            Type_Int32 = 1,
            Type_UInt32 = 2,
            Type_Int64 = 3,
            Type_UInt64 = 4,
            Type_Double = 5,
            Type_String = 6,
            Type_Vector3D = 7,
            Type_Object = 8,
            Type_Record = 9,
            Type_Array = 10,
            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

            Mask_Null_Value = 0,			// 无效值
            Mask_Sync_Client = 1 << 1,		// 更新同步玩家
            Mask_Sync_View = 1 << 2,		// 同步给附近玩家
            Mask_Save_Database = 1 << 3,	// 同步给附近玩家
            Mask_System_Log = 1 << 4,		// 打印日志
            Mask_Public_Data = 1 << 5,		// 公共属性
            Mask_Relation_Data = 1 << 6,	// 关系属性
            Mask_Group_Data = 1 << 7,		// 队伍属性
            Mask_Guild_Data = 1 << 8,		// 帮派属性
            Mask_Rank_Data = 1 << 9,		// 排行榜属性

            Mask_Save_Delete = 1 << 30,		// 删除同步到服务器
            Mask_Sync_Delete = 1 << 31,		// 删除同步到客户端
            //Flag_Show			= 1 << 9,		// 通知客户端显示
            ////////////////////////////////////////////////////////////////////////////
            // 数据的保存和同步类型
            Data_Operate_None = 0,		// 不执行
            Data_Operate_AtOnce = 1,	// 马上
            Data_Operate_Delay = 2,		// 延迟
            ////////////////////////////////////////////////////////////////////////////

        };

        static uint32 ConvertDataType( const std::string& stringtype )
        {
            if ( stringtype == "int32" )
            {
                return Type_Int32;
            }

            if ( stringtype == "uint32" )
            {
                return Type_UInt32;
            }

            if ( stringtype == "int64" )
            {
                return Type_Int64;
            }

            if ( stringtype == "uint64" )
            {
                return Type_UInt64;
            }

            if ( stringtype == "double" )
            {
                return Type_Double;
            }

            if ( stringtype == "string" )
            {
                return Type_String;
            }

            if ( stringtype == "vector3d" )
            {
                return Type_Vector3D;
            }

            if ( stringtype == "object" )
            {
                return Type_Object;
            }

            if ( stringtype == "record" )
            {
                return Type_Record;
            }

            if ( stringtype == "array" )
            {
                return Type_Array;
            }

            return Type_Unknow;
        }
    }
}

#endif