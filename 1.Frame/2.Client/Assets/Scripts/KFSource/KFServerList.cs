using System.Collections;
using System.Collections.Generic;

public class ServerInfo
{
    // 服务器id
    public uint _id;

    // 服务器名字
    public string _name;

    // 服务器地址
    public string _ip;

    // 服务器端口
    public uint _port;

    // 服务器状态
    public uint _status;

    // 服务器类型
    public uint _type;

    // 服务器描述
    public string _describe;
}

public class KFServerList : KFSingleton<KFServerList>
{
    // 服务器列表
    public ArrayList _server_list = new ArrayList();

    public void ClearServer()
    {
        _server_list.Clear();
    }

    public void AddServer(ServerInfo info)
    {
        _server_list.Add(info);
    }

    public void AddServer(uint id, string name, string ip, uint port, uint status, uint type, string describe)
    {
        ServerInfo info = new ServerInfo();
        info._id = id;
        info._name = name;
        info._ip = ip;
        info._port = port;
        info._status = status;
        info._type = type;
        info._describe = describe;
        _server_list.Add(info);
    }

    public ServerInfo FindServer(uint id)
    {
        foreach (ServerInfo info in _server_list)
        {
            if (info._id == id)
            {
                return info;
            }
        }

        return null;
    }

    public ServerInfo FindServer(string name)
    {
        foreach (ServerInfo info in _server_list)
        {
            if (info._name == name)
            {
                return info;
            }
        }

        return null;
    }
}
