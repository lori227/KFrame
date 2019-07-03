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
        // 显示文字
        public string _text { get; set; }

        // 游戏内属性名
        public string _name { get; set; }
        
        // 包含的子类
        public string _contain { get; set; }

        // 数值
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

        public void AddDataValue(string text, string name, string value, string contain)
        {
            var datavalue = new DataValue();
            datavalue._text = text;
            datavalue._name = name;
            datavalue._value = value;
            datavalue._contain = contain;

            _child_list.Add(datavalue);
        }
    }

    public Dictionary<string, DataSetting> _data_list = new Dictionary<string, DataSetting>();

    private string ReadXmlData( XElement item, string name )
    {
        var value = "";
        var xmlobject = item.Attribute(name);
        if (xmlobject != null)
        {
            value = xmlobject.Value;
        }

        return value;
    }

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
                var classname = ReadXmlData(item, "Id");
                var dataname = ReadXmlData(item, "Name");
                var datatext = ReadXmlData(item,"Element");
                var containclass = ReadXmlData(item, "ContainClass");
                if (datatext != "" && classname != "Player")
                {
                    DataSetting datastting = null;
                    FindDataSetting(ref classlist, classname, out datastting);
                    datastting._text = datatext;
                    datastting.AddDataValue(datatext, dataname, "0", containclass);
                }
            }

            // 
            foreach (XElement item in enumerable)
            {
                var classname = ReadXmlData(item, "Id");
                var dataname = ReadXmlData(item, "Name");
                var datatext = ReadXmlData(item, "Element");

                if (datatext != "" && classname == "Player")
                {
                    DataSetting datasetting = new DataSetting();
                    datasetting._name = dataname;
                    datasetting._text = datatext;

                    {
                        DataSetting childstting = null;
                        var containclass = ReadXmlData(item, "ContainClass");
                        if (containclass != "")
                        {
                            datasetting._isobject = true;
                            FindDataSetting(ref classlist, containclass, out childstting);
                            foreach( DataValue datavalue in childstting._child_list)
                            {
                                DataSetting substting = null;
                                FindDataSetting(ref classlist, datavalue._contain, out substting);
                                if ( substting._text == null || substting._text == "" )
                                {
                                    if ( datavalue._contain!= "" )
                                    {
                                        datavalue._value = "";
                                    }
                                    datasetting._child_list.Add(datavalue);
                                }
                                else
                                {
                                    foreach (DataValue subvalue in substting._child_list)
                                    {
                                        var addvalue = new DataValue();
                                        addvalue._text = datavalue._text + "-" + subvalue._text;
                                        addvalue._name = datavalue._name + "-" + subvalue._name;
                                        addvalue._value = "0";
                                        datasetting._child_list.Add(addvalue);
                                    }
                                }
                            }
                        }
                        else
                        {
                            datasetting._isobject = false;
                            datasetting.AddDataValue(datatext, dataname, "0", containclass);
                        }
                    }

                    _data_list.Add(datatext, datasetting);
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