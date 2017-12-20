using UnityEngine;
using System.Collections;

public class EnemyAreaCheck : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnTriggerEnter(Collider other){
		if(other.gameObject.tag == "Player"){
			GameObject parent = gameObject.transform.parent.gameObject;
			parent.SendMessage("TraceModeOn");
		}
	}
	void OnTriggerExit(Collider other){
		if(other.gameObject.tag == "Player"){
			GameObject parent = gameObject.transform.parent.gameObject;
			parent.SendMessage("TraceModeOff");
		}
	}
}
