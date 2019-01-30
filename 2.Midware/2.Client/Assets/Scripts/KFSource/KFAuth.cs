using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json.Linq;

public class KFAuth : KFSingleton<KFAuth>
{
    public uint _account_id = 0;
    public uint _acconnt_flag = 0;
    public string _token = "";


    public void AuthAccount(JObject json)
    {
        var authurl = KFOption.Instance().GetString("authurl");
        var result = KFHttpClient.Request(authurl, json);

        // 获得服务器列表
        var jtoken = result.GetValue("retcode");
        if (jtoken == null)
        {
            return;
        }

        var retcode = int.Parse(jtoken.ToString());
        if (retcode != 1)
        {
            return;
        }

        _account_id = uint.Parse(result["accountid"].ToString());
        _acconnt_flag = uint.Parse(result["accountflag"].ToString());
        _token = result["token"].ToString();

        KFServerList.Instance().ClearServer();

        // 保存服务器列表
        JArray jarray = result.GetValue("serverlist") as JArray;
        if (jarray != null)
        {
            for (int i = 0; i < jarray.Count; ++i)
            {
                JObject jobject = JObject.Parse(jarray[i].ToString());

                ServerInfo info = new ServerInfo();
                info._id = uint.Parse( jobject["id"].ToString());
                info._name = jobject["name"].ToString();
                info._ip = jobject["ip"].ToString();
                info._port = uint.Parse(jobject["port"].ToString());
                info._status = uint.Parse(jobject["status"].ToString());
                info._type = uint.Parse(jobject["type"].ToString());
                info._describe = jobject["describe"].ToString();
                KFServerList.Instance().AddServer(info);
            }
        }

        // 显示服务器列表
        UIManage.Instance().ShowUI<UIServerList>();
    }

}
