#ifndef	__KF_RELATION_CONFIG_H__
#define	__KF_RELATION_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFRelationSetting : public KFStrSetting
	{
	public:
		// 最大数量
		uint32 _max_count = 0u;

		// 是否是双向的
		bool _both_way = false;

		// 是否需要更新属性
		bool _need_update = false;

		// 是否需要主动加载
		bool _online_load = false;

		// 邀请列表最大数量
		uint32 _invite_list_count = 0u;

		// 邀请保留时间(秒)
		uint32 _invite_keep_time = 0u;

		// 数据库列表名字(服务器使用,勿改)
		std::string _data_list_name;

		// 邀请列表属性名字(空就是不需要邀请)
		std::string _invite_data_name;

		// 邀请列表属性名字(服务器使用,勿改)
		std::string _invite_list_name;

		// 拒绝申请(服务器使用)
		std::string _refuse_name;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFRelationConfig : public KFConfigT< KFRelationSetting >, public KFInstance< KFRelationConfig >
	{
	public:
		KFRelationConfig()
		{
			_key_name = "id";
			_file_name = "relation";
		}

		~KFRelationConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFRelationSetting* kfsetting )
		{
			kfsetting->_max_count = xmlnode.ReadUInt32( "maxcount", true );
			kfsetting->_both_way = xmlnode.ReadBoolen( "bothway", true );
			kfsetting->_need_update = xmlnode.ReadBoolen( "needupdate", true );
			kfsetting->_online_load = xmlnode.ReadBoolen( "onlineload", true );
			kfsetting->_invite_list_count = xmlnode.ReadUInt32( "invitelistcount", true );
			kfsetting->_invite_keep_time = xmlnode.ReadUInt32( "invitekeeptime", true );
			kfsetting->_data_list_name = xmlnode.ReadString( "datalistname", true );
			kfsetting->_invite_data_name = xmlnode.ReadString( "invitedataname", true );
			kfsetting->_invite_list_name = xmlnode.ReadString( "invitelistname", true );
			kfsetting->_refuse_name = xmlnode.ReadString( "refusename", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
