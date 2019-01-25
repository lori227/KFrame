using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIServerList : UIDialog
{
	public Dropdown _dropdown_server_list;
	public Button _button_connect;

	// Use this for initialization
	void Start () {
		// 初始化服务器列表
		InitServerList();

		_button_connect.onClick.AddListener( OnConnectClick );
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	private void InitServerList()
	{
		_dropdown_server_list.options.Clear();

        foreach (ServerInfo info in KFServerList.Instance()._server_list)
		{
            var item = new Dropdown.OptionData();
			item.text = info._name;
			_dropdown_server_list.options.Add( item );
		}

        _dropdown_server_list.captionText.text = "请选择服务器";
	}

	private void OnConnectClick()
	{
		var servername = _dropdown_server_list.options[_dropdown_server_list.value].text.ToString();
		var info = KFServerList.Instance().FindServer( servername );
		if ( info == null)
		{
			return;
		}

		//Debug.Log( info._id.ToString() );
	}
}
