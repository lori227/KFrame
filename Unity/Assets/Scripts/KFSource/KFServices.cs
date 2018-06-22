using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KFServices : MonoBehaviour
{
    private static KFServices _instance = null;
    public static KFServices Instance()
    {
        return _instance;
    }

    // Use this for initialization
    void Start()
    {
        _instance = this;
        KFOption.Instance().LoadConfig();

        // 显示ui
        UIManage.Instance().ShowUI<UILogin>();
    }

    // Update is called once per frame
    void Update()
    {

    }
}
