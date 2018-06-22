using System;
using System.Linq;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class UIManage : KFSingleton<UIManage>
{
    private UIDialog _ui_current = null;
    Dictionary<string, GameObject> _ui_list = new Dictionary<string, GameObject>();
    Queue<UIDialog> _ui_queue = new Queue<UIDialog>();

    public void ShowUI<T>() where T : UIDialog
    {
        HideUI();

        string name = typeof(T).ToString();
        GameObject uiobject;
        if (!_ui_list.TryGetValue(name, out uiobject))
        {
            GameObject perfb = Resources.Load<GameObject>("UI/" + typeof(T).ToString());
            uiobject = GameObject.Instantiate(perfb);
            uiobject.name = name;
            uiobject.transform.SetParent(KFServices.Instance().transform);

            _ui_list.Add(name, uiobject);
        }

        uiobject.SetActive(true);

        T panel = uiobject.GetComponent<T>();
        _ui_current = panel;
        _ui_queue.Enqueue(panel);
    }

    public void CloseUI()
    {
        HideUI();
        _ui_queue.Clear();
    }
    
    private void HideUI()
    {
        if (_ui_current == null)
        {
            return;
        }

        _ui_current.gameObject.SetActive(false);
        _ui_current = null;
    }

}