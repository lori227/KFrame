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
		// �������
		uint32 _max_count = 0u;

		// �Ƿ���˫���
		bool _both_way = false;

		// �Ƿ���Ҫ��������
		bool _need_update = false;

		// �Ƿ���Ҫ��������
		bool _online_load = false;

		// �����б��������
		uint32 _invite_list_count = 0u;

		// ���뱣��ʱ��(��)
		uint32 _invite_keep_time = 0u;

		// ���ݿ��б�����(������ʹ��,���)
		std::string _data_list_name;

		// �����б���������(�վ��ǲ���Ҫ����)
		std::string _invite_data_name;

		// �����б���������(������ʹ��,���)
		std::string _invite_list_name;

		// �ܾ�����(������ʹ��)
		std::string _refuse_name;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFRelationConfig : public KFConfigT<KFRelationSetting>, public KFInstance<KFRelationConfig>
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
			setting->_both_way = xml_node.ReadBool( "is_both_way", true );
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
