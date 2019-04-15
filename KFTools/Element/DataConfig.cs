using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;
using System.Xml.Linq;
using System.Data;

/// <summary>
/// DataConfig 的摘要说明
/// </summary>
public class DataConfig
{
    public DataConfig()
    {
        //
        // TODO: 在此处添加构造函数逻辑
        //
    }

    private static DataConfig _instance;

    public static DataConfig Instance()
    {
        if (_instance == null)
        {
            _instance = new DataConfig();
        }

        return _instance;
    }

    public class DataValue
    {
        public string _text { get; set; }
        public string _name { get; set; }
        public string _value { get; set; }
    }

    public class DataSetting
    {
        public DataSetting()
        {
            _child_list = new List<DataValue>();
        }

        // 属性文本
        public string _text;

        // 属性名字
        public string _name;

        // 是否是对象
        public bool _isobject;

        // 子属性列表
        public List<DataValue> _child_list;

        public void AddDataValue(string text, string name, string value)
        {
            var datavalue = new DataValue();
            datavalue._text = text;
            datavalue._name = name;
            datavalue._value = value;

            _child_list.Add(datavalue);
        }
    }

    public Dictionary<string, DataSetting> _data_list = new Dictionary<string, DataSetting>();

    public void LoadDataConfig(string xmlfile)
    {
        _data_list.Clear();

        try
        {
            XDocument document = XDocument.Load(xmlfile);
            XElement root = document.Root;
            Dictionary<string, DataSetting> classlist = new Dictionary<string, DataSetting>();
            IEnumerable<XElement> enumerable = root.Elements();
            foreach (XElement item in enumerable)
            {
                var classname = item.Attribute("Class").Value;
                var dataname = item.Attribute("Name").Value;
                var datatext = item.Attribute("Element").Value;

                if (datatext != "" && classname != "Player")
                {

                    DataSetting datastting = null;
                    FindDataSetting(ref classlist, classname, out datastting);
                    datastting._text = datatext;
                    datastting.AddDataValue(datatext, dataname, "0");
                }
            }

            // 
            foreach (XElement item in enumerable)
            {
                var classname = item.Attribute("Class").Value;
                var dataname = item.Attribute("Name").Value;
                var datatext = item.Attribute("Element").Value;

                if (datatext != "" && classname == "Player")
                {
                    DataSetting datastting = new DataSetting();
                    datastting._name = dataname;
                    datastting._text = datatext;

                    {
                        DataSetting childstting = null;
                        var containclass = item.Attribute("ContainClass").Value;
                        if (containclass != "")
                        {
                            datastting._isobject = true;

                            FindDataSetting(ref classlist, containclass, out childstting);
                            datastting._child_list = childstting._child_list;
                        }
                        else
                        {
                            datastting._isobject = false;
                            datastting.AddDataValue(datatext, dataname, "0");
                        }
                    }

                    _data_list.Add(datatext, datastting);
                }
            }
        }
        catch( Exception ex )
        {

        }
    }

    protected void FindDataSetting(ref Dictionary<string, DataSetting> dic, string name, out DataSetting datasetting)
    {
        var ok = dic.TryGetValue(name, out datasetting);
        if (!ok)
        {
            datasetting = new DataSetting();
            datasetting._name = name;
            dic.Add(name, datasetting);
        }
    }

    public void FindDataSetting(string name, out DataSetting datasetting)
    {
        var ok = _data_list.TryGetValue(name, out datasetting);
        if (!ok)
        {
            datasetting = new DataSetting();
            datasetting._name = name;
            _data_list.Add(name, datasetting);
        }
    }
}