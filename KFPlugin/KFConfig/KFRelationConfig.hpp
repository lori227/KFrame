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
			kfsetting->_both_way = xmlnode.ReadBool( "bothway", true );
			kfsetting->_need_update = xmlnode.ReadBool( "needupdate", true );
			kfsetting->_online_load = xmlnode.ReadBool( "onlineload", true );
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
