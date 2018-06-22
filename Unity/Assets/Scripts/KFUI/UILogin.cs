using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Newtonsoft.Json.Linq;


public class UILogin : UIDialog
{
    public InputField _input_account = null;
    public InputField _input_password = null;
    public Button _button_login = null;

    // Use this for initialization
    void Start()
    {
        _button_login.onClick.AddListener(OnLoginClick);
    }

    private void OnLoginClick()
    {
        string account = _input_account.text;
        if (account == "")
        {
            return;
        }

        string password = _input_password.text;
        password = KFCrypto.MD5Encrypt32(password);

        JObject json = new JObject();
        json.Add("channel", "1");
        json.Add("account", account);
        KFAuth.Instance().AuthAccount(json);
    }
}
