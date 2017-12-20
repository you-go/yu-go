using UnityEngine;
using System.Collections;

public class CameraControll : MonoBehaviour {

	private int currentCam;
	public static int newCam;
	private GameObject backCam;
	private GameObject frontCam;
	private GameObject fixedCam1;
	private GameObject fixedCam2;
	private GameObject mainCam;

	// Use this for initialization
	void Start () {
		backCam = GameObject.Find("BackCam");
		frontCam = GameObject.Find("FrontCam");
		fixedCam1 = GameObject.Find("FixedCam1");
		fixedCam2 = GameObject.Find("FixedCam2");

		backCam.GetComponent<Camera>().enabled = false;
		backCam.GetComponent<AudioListener>().enabled = false;
		frontCam.GetComponent<Camera>().enabled = false;
		frontCam.GetComponent<AudioListener>().enabled = false;
		fixedCam1.GetComponent<Camera>().enabled = false;
		fixedCam1.GetComponent<AudioListener>().enabled = false;
		fixedCam2.GetComponent<Camera>().enabled = false;
		fixedCam2.GetComponent<AudioListener>().enabled = false;
		currentCam = 2;
		newCam = 1;
		ChangeCam(newCam);
	}
	
	// Update is called once per frame
	void Update () {
		//if(GameManager.gameState == GameManager.STATE.TITLE){
			//ChangeCam(currentCam,currentCam);
		//}
		//if(GameManager.gameState == GameManager.STATE.START){
		//	newCam = 1;
		//}
		if(Input.GetKey(KeyCode.Alpha1)) newCam = 1;
		if(Input.GetKey(KeyCode.Alpha2)) newCam = 2;
		if(Input.GetKey(KeyCode.Alpha3)) newCam = 3;
		if(Input.GetKey(KeyCode.Alpha4)) newCam = 4;
		if (currentCam != newCam) {
			ChangeCam (newCam);
		}
	}

	public void ChangeCam(int newCam){
		if (currentCam == newCam) {
			return;
		}

		backCam = GameObject.Find("BackCam");
		frontCam = GameObject.Find("FrontCam");
		fixedCam1 = GameObject.Find("FixedCam1");
		fixedCam2 = GameObject.Find("FixedCam2");
		backCam.GetComponent<Camera>().enabled = false;
		backCam.GetComponent<AudioListener>().enabled = false;
		frontCam.GetComponent<Camera>().enabled = false;
		frontCam.GetComponent<AudioListener>().enabled = false;
		fixedCam1.GetComponent<Camera>().enabled = false;
		fixedCam1.GetComponent<AudioListener>().enabled = false;
		fixedCam2.GetComponent<Camera>().enabled = false;
		fixedCam2.GetComponent<AudioListener>().enabled = false;

		switch(newCam)
		{
		case 1:
			backCam.GetComponent<Camera>().enabled = true;
			backCam.GetComponent<AudioListener>().enabled = true;
			break;
		case 2:
			frontCam.GetComponent<Camera>().enabled = true;
			frontCam.GetComponent<AudioListener>().enabled = true;
			break;
		case 3:
			fixedCam1.GetComponent<Camera>().enabled = true;
			fixedCam1.GetComponent<AudioListener>().enabled = true;
			break;
		case 4:
			fixedCam2.GetComponent<Camera>().enabled = true;
			fixedCam2.GetComponent<AudioListener>().enabled = true;
			break;
		}
		currentCam = newCam;

	}
	void OnGUI(){
		GUI.Label (new Rect (5,  20, 100, 30), "currentcam:" + currentCam);
	}
}
