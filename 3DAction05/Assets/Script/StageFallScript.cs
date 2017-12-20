using UnityEngine;
using System.Collections;

public class StageFallScript : MonoBehaviour {

	//GameManeger
	GameObject gm;

	// Use this for initialization
	void Start () {
		gm = GameObject.Find ("GameManager");
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnTriggerEnter(Collider other){
		if(other.tag == "Player"){
			gm.gameObject.SendMessage("Miss");
		}
	}
}
