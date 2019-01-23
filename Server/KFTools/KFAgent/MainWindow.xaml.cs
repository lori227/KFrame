using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;
using System.Xml.Linq;
using static DataConfig;


namespace KFAgent
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public string _setting_file = "./setting.xml";
        protected string _null_text = "请选择";
        protected string _null_data = "null";

        protected string _select_text = "";
        protected string _select_name = "";
        protected bool _select_object = false;
        protected List<DataSetting> _agent_array = new List<DataSetting>();

        public MainWindow()
        {
            InitializeComponent();

            LoadSetting();
            DataConfig.Instance().LoadDataConfig(editDataConfigPath.Text);
            InitDataComboBox();
        }

        protected void LoadSetting()
        {
            try
            {
                XDocument document = XDocument.Load(_setting_file);
                XElement root = document.Root;

                {
                    XElement element = root.Element("DataFile");
                    editDataConfigPath.Text = element.Attribute("Path").Value;
                }

                {
                    XElement element = root.Element("ItemFile");
                    editItemConfigPath.Text = element.Attribute("Path").Value;
                }
            }
            catch (Exception ex)
            {

            }
        }

        protected void SaveSetting()
        {
            try
            {
                XDocument document = new XDocument();
                XElement root = new XElement("Setting");

                {
                    XElement element = new XElement("DataFile");
                    element.SetAttributeValue("Path", editDataConfigPath.Text);
                    root.Add(element);
                }

                {
                    XElement element = new XElement("ItemFile");
                    element.SetAttributeValue("Path", editItemConfigPath.Text);
                    root.Add(element);
                }

                root.Save(_setting_file);
            }
            catch (Exception ex)
            {

            }
        }

        private void Button_Click_Data_File_Path(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.InitialDirectory = "c:\\";
            openFileDialog.Filter = "config files (*.config)|*.config";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                editDataConfigPath.Text = openFileDialog.FileName;
                SaveSetting();

                DataConfig.Instance().LoadDataConfig(editDataConfigPath.Text);
                InitDataComboBox();
            }
        }

        private void Button_Click_Item_File_Path(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.InitialDirectory = "c:\\";
            openFileDialog.Filter = "config files (*.config)|*.config";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                editItemConfigPath.Text = openFileDialog.FileName;
                SaveSetting();
            }

        }

        private void InitDataComboBox()
        {
            comboDataSelect.Items.Clear();
            comboDataSelect.Items.Add(_null_text);

            foreach (var data in DataConfig.Instance()._data_list)
            {
                comboDataSelect.Items.Add(data.Key);
            }

            comboDataSelect.SelectedIndex = 0;
        }

        private void BindSelectionDataSetting()
        {
            if (comboDataSelect.SelectedValue == null)
            {
                return;
            }

            var text = comboDataSelect.SelectedValue.ToString();
            if (text == _null_text)
            {
                return;
            }

            DataSetting datasetting = null;
            DataConfig.Instance().FindDataSetting(text, out datasetting);

            var infoList = new List<DataValue>();
            foreach (var datavalue in datasetting._child_list)
            {
                var temp = new DataValue();
                temp._text = datavalue._text;
                temp._name = datavalue._name;
                temp._value = datavalue._value;
                infoList.Add(temp);
            }

            _select_text = datasetting._text;
            _select_name = datasetting._name;
            _select_object = datasetting._isobject;
            gridData.ItemsSource = infoList;
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            BindSelectionDataSetting();
        }

        private void Button_Click_Add_Agent(object sender, RoutedEventArgs e)
        {
            if ( _select_text == "" || _select_name == "")
            {
                return;
            }

            var datalist = (List<DataValue>)gridData.ItemsSource;
            bool isvalid = false;
            foreach( var datavalue in datalist )
            {
                if ( datavalue._value != "0" )
                {
                    isvalid = true;
                    break;
                }
            }

            if ( !isvalid )
            {
                return;
            }

            var addsetting = new DataSetting();
            addsetting._text = _select_text;
            addsetting._name = _select_name;
            addsetting._isobject = _select_object;
            foreach (var datavalue in datalist)
            {
                if (datavalue._value != "0")
                {
                    addsetting.AddDataValue(datavalue._text, datavalue._name, datavalue._value);
                }
            }

            _agent_array.Add(addsetting);

            // 绑定数据
            BindTotalAgentList();

            // 格式化数据
            FormatTotalAgent();

            // 重新绑定
            BindSelectionDataSetting();
        }

        private void BindTotalAgentList()
        {
            var agenglist = new List<string>();
            foreach( var datasetting in _agent_array )
            {
                string strdata = "";

                if ( datasetting._isobject )
                {
                    strdata += datasetting._text;
                    strdata += ":";

                    // 名字
                    strdata += "{";

                    bool first = true;
                    foreach (var datavalue in datasetting._child_list)
                    {
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            strdata += ",";
                        }

                        strdata += datavalue._text + ":" + datavalue._value.ToString();
                    }

                    strdata += "}";
                }
                else
                {
                    bool first = true;
                    foreach (var datavalue in datasetting._child_list)
                    {
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            strdata += ",";
                        }

                        strdata += datavalue._text + ":" + datavalue._value.ToString();
                    }
                }
                
                agenglist.Add(strdata);
            }
            agentList.ItemsSource = agenglist;
        }

        private void Button_Click_Remove_Agent(object sender, RoutedEventArgs e)
        {
            var index = agentList.SelectedIndex;
            if (index == -1)
            {
                return;
            }

            _agent_array.RemoveAt(index);
            BindTotalAgentList();
            FormatTotalAgent();
        }

        private void Button_Click_Clear_Agent(object sender, RoutedEventArgs e)
        {
            _agent_array.Clear();
            BindTotalAgentList();
            FormatTotalAgent();
        }

        private void FormatTotalAgent()
        {
            if (_agent_array.Count == 0)
            {
                editAgentString.Text = "";
                return;
            }

            string strsplit = ",";
            string stragent = "";

            bool issettingfirst = true;

            stragent += "[";
            foreach (var datasetting in _agent_array)
            {
                if ( issettingfirst)
                {
                    issettingfirst = false;
                }
                else
                {
                    stragent += strsplit;
                }

                // 名字
                stragent += "{";
                if ( datasetting._isobject )
                {
                    stragent += "\"" + datasetting._name + "\":{";

                    bool isdatafirst = true;
                    foreach (var datavalue in datasetting._child_list)
                    {
                        if (isdatafirst)
                        {
                            isdatafirst = false;
                        }
                        else
                        {
                            stragent += strsplit;
                        }

                        stragent += "\"" + datavalue._name + "\":\"" + datavalue._value.ToString() + "\"";
                    }

                    stragent += "}";
                }
                else
                {
                    bool isdatafirst = true;
                    foreach (var datavalue in datasetting._child_list)
                    {
                        if (isdatafirst)
                        {
                            isdatafirst = false;
                        }
                        else
                        {
                            stragent += strsplit;
                        }

                        stragent += "\"" + datavalue._name + "\":\"" + datavalue._value.ToString() + "\"";
                    }
                }

                stragent += "}";
            }

            stragent += "]";

            editAgentString.Text = stragent;
        }

        private void Button_Click_Copy(object sender, RoutedEventArgs e)
        {
            Clipboard.SetText(editAgentString.Text);
        }
    }
}
