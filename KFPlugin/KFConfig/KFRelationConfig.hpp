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
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFRelationSetting> setting )
		{
			setting->_max_count = xml_node.ReadUInt32( "maxcount", true );
			setting->_both_way = xml_node.ReadBool( "bothway", true );
			setting->_need_update = xml_node.ReadBool( "needupdate", true );
			setting->_online_load = xml_node.ReadBool( "onlineload", true );
			setting->_invite_list_count = xml_node.ReadUInt32( "invitelistcount", true );
			setting->_invite_keep_time = xml_node.ReadUInt32( "invitekeeptime", true );
			setting->_data_list_name = xml_node.ReadString( "datalistname", true );
			setting->_invite_data_name = xml_node.ReadString( "invitedataname", true );
			setting->_invite_list_name = xml_node.ReadString( "invitelistname", true );
			setting->_refuse_name = xml_node.ReadString( "refusename", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
