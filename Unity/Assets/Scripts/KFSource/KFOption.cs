using System.Collections;
using System.Collections.Generic;
using System.Xml;
using UnityEngine;

public class KFOption
{
    public class Option
    {
        public Dictionary<int, string> _values = new Dictionary<int, string>();

        public string FindValue(int key)
        {
            string value = "";
            _values.TryGetValue(key, out value);
            return value;
        }
    }

    public static string _config = "Config/option.config";
    public static Dictionary<string, Option> _options = new Dictionary<string, Option>();

    public static string GetString(string name, int key = 0)
    {
        Option option = null;
        if (_options.TryGetValue(name, out option))
        {
            return option.FindValue(key);
        }

        return "";
    }

    public static int GetInt(string name, int key = 0)
    {
        return int.Parse(GetString(name, key));
    }

    public static bool LoadConfig()
    {
        try
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(Application.streamingAssetsPath + "/option.config");

            XmlNode xmlroot = xmlDoc.SelectSingleNode("Config");
            XmlNodeList xmlnodelist = xmlroot.ChildNodes;
            foreach (XmlNode xmlnode in xmlnodelist)
            {
                XmlElement xmlelement = (XmlElement)xmlnode;

                var name = xmlelement.GetAttribute("Name");
                var key = xmlelement.GetAttribute("Key");
                var value = xmlelement.GetAttribute("Value");

                Option option = null;
                if (!_options.TryGetValue(name, out option))
                {
                    option = new Option();
                    _options.Add(name, option);
                }

                option._values.Add((key == "" ? 0 : int.Parse(key)), value);
            }
        }
        catch
        {
            return false;
        }

        return true;
    }

}
