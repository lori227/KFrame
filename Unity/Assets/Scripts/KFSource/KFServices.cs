using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KFServices : MonoBehaviour {

	// Use this for initialization
	void Start () {
        KFOption.Instance().LoadConfig();
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
