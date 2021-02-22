#ifndef	__KF_ITEMBAG_CONFIG_H__
#define	__KF_ITEMBAG_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFItemBagSetting : public KFStrSetting
	{
	public:
		// �������
		uint32 _max_count = 0u;

		// ������������
		std::string _index_name;

		// ҳǩ�б�
		StringSet _tab_list;

		// �����Ƿ񵯳����ⱳ��
		std::string _extend_bag;

		// ��ͬʱʹ�õı���
		StringList _use_child_bag;

		// ����Դ�������ƶ����������Ҫ����
		StringSet _update_add_from_source;

		// �ƶ���Ŀ�걳���ĵ�����Ҫ����
		StringSet _update_move_to_target;

		// �ƶ���Ҫ�ͻ���Ʈ�ֵı���
		StringSet _show_move_to_target;

		// ��ӵ����Ƿ�֪ͨ�ͻ���Ʈ��
		bool _show_when_add = false;

		// �Ƿ�֧��ֱ���ƶ�
		bool _can_move = false;

		// �Ƿ�֧��һ���ƶ�����
		bool _can_move_all = false;

		// �Ƿ�֧��һ�����
		bool _can_cleanup = false;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFItemBagConfig : public KFConfigT< KFItemBagSetting >, public KFInstance< KFItemBagConfig >
	{
	public:
		KFItemBagConfig()
		{
			_key_name = "id";
			_file_name = "itembag";
		}

		~KFItemBagConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFItemBagSetting* kfsetting )
		{
			kfsetting->_max_count = xmlnode.ReadUInt32( "maxcount", true );
			kfsetting->_index_name = xmlnode.ReadString( "indexname", true );
			kfsetting->_tab_list = xmlnode.ReadStringSet( "tablist", true );
			kfsetting->_extend_bag = xmlnode.ReadString( "extendbag", true );
			kfsetting->_use_child_bag = xmlnode.ReadStringList( "usechildbag", true );
			kfsetting->_update_add_from_source = xmlnode.ReadStringSet( "updateaddfromsource", true );
			kfsetting->_update_move_to_target = xmlnode.ReadStringSet( "updatemovetotarget", true );
			kfsetting->_show_move_to_target = xmlnode.ReadStringSet( "showmovetotarget", true );
			kfsetting->_show_when_add = xmlnode.ReadBool( "showwhenadd", true );
			kfsetting->_can_move = xmlnode.ReadBool( "canmove", true );
			kfsetting->_can_move_all = xmlnode.ReadBool( "canmoveall", true );
			kfsetting->_can_cleanup = xmlnode.ReadBool( "cancleanup", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
