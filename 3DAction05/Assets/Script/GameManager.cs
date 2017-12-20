using UnityEngine;
using System.Collections;

public class GameManager : MonoBehaviour {

	private GUIStyle style;
	public GUIStyleState styleState;

	public enum STATE {TITLE,START,COUNTDOWN,PLAY,MISS,CLEAR,LIMIT,GAMEOVER};
	public static STATE gameState;

	private GameObject message;
	private GameObject messageShadow;
	private GameObject subMessage;
	private GameObject subMessageShadow;

	private GameObject guiTime;
	private GameObject guiTimeShadow;

	public GameObject player;

	private int score;
	private float restTime;
	private float startTime;
	public float timeLimit = 30.0f;

	// Use this for initialization
	void Start () {
		style = new GUIStyle();
		style.fontSize = 12;
		styleState.textColor = Color.white;
		style.normal = styleState;

		messageShadow = GameObject.Find("GUIMessageShadow");
		message = GameObject.Find("GUIMessage");
		messageShadow.GetComponent<GUIText>().text = "";
		message.GetComponent<GUIText>().text = "";
		subMessageShadow = GameObject.Find("GUISubMessageShadow");
		subMessage = GameObject.Find("GUISubMessage");
		subMessageShadow.GetComponent<GUIText>().text = "";
		subMessage.GetComponent<GUIText>().text = "";

		guiTimeShadow = GameObject.Find("GUITimeShadow");
		guiTime = GameObject.Find("GUITime");
		guiTimeShadow.GetComponent<GUIText>().text = "";
		guiTime.GetComponent<GUIText>().text = "";

		initialize();
		CameraControll.newCam = 2;
		gameState = STATE.TITLE;
	}
	
	// Update is called once per frame
	void Update () {
		switch(gameState){
		case STATE.TITLE:
			CameraControll.newCam = 2;
			messageShadow.GetComponent<GUIText>().text = "TITLE";
			message.GetComponent<GUIText>().text = "TITLE";
			subMessageShadow.GetComponent<GUIText>().text = "Push Space Key";
			subMessage.GetComponent<GUIText>().text = "Push Space Key";
			if(Input.GetKey(KeyCode.Space)){
				gameState = STATE.START;
			}
			break;
		case STATE.START:
			CameraControll.newCam = 1;
			restTime = timeLimit;
			startTime = Time.time;
			guiTimeShadow.GetComponent<GUIText>().text = restTime.ToString("0.0");
			guiTime.GetComponent<GUIText>().text = restTime.ToString("0.0");
			gameState = STATE.COUNTDOWN;
			StartCoroutine ("StartCountdown");
			break;
		case STATE.COUNTDOWN:
			break;
		case STATE.PLAY:
		case STATE.MISS:
			restTime = timeLimit - (Time.time - startTime);
			if(restTime < 0.0){
				restTime = 0.0f;
				StartCoroutine("GameOver");
				//gameState = STATE.GAMEOVER;
			}
			guiTimeShadow.GetComponent<GUIText>().text = restTime.ToString("0.0");
			guiTime.GetComponent<GUIText>().text = restTime.ToString("0.0");
			break;
		case STATE.CLEAR:
			break;
		case STATE.GAMEOVER:
			break;
		}

	}

	// 初期化
	private void initialize(){
		GameObject[] ps = GameObject.FindGameObjectsWithTag("Player");
		for(int i=0;i<ps.Length;i++){
			Destroy(ps[i]);
		}
		GameObject start = GameObject.Find("Start");
		GameObject ins = (GameObject)Instantiate(player, start.transform.position+new Vector3(0,0.2f,0), start.transform.rotation);
	}

	// スタート時
	private IEnumerator StartCountdown() {

		subMessageShadow.GetComponent<GUIText>().text = "";
		subMessage.GetComponent<GUIText>().text = "";
		messageShadow.GetComponent<GUIText>().text = "3";
		message.GetComponent<GUIText>().text = "3";
		yield return new WaitForSeconds (1.0f);		// 1秒待つ
		messageShadow.GetComponent<GUIText>().text = "2";
		message.GetComponent<GUIText>().text = "2";
		yield return new WaitForSeconds (1.0f);		// 1秒待つ
		messageShadow.GetComponent<GUIText>().text = "1";
		message.GetComponent<GUIText>().text = "1";
		yield return new WaitForSeconds (1.0f);		// 1秒待つ
		messageShadow.GetComponent<GUIText>().text = "START!";
		message.GetComponent<GUIText>().text = "START!";
		yield return new WaitForSeconds (1.0f);		// 1秒待つ
		messageShadow.GetComponent<GUIText>().text = "";
		message.GetComponent<GUIText>().text = "";
		gameState = STATE.PLAY;
	}

	private IEnumerator Clear(){
		CameraControll.newCam = 2;
		gameState = STATE.CLEAR;
		messageShadow.GetComponent<GUIText>().text = "Clear!";
		message.GetComponent<GUIText>().text = "Clear!";
		yield return new WaitForSeconds (5.0f);		// 5秒待つ
		messageShadow.GetComponent<GUIText>().text = "";
		message.GetComponent<GUIText>().text = "";
		initialize();
		CameraControll.newCam = 2;
		gameState = STATE.TITLE;
	}
	
	private IEnumerator Miss(){
		gameState = STATE.MISS;
		messageShadow.GetComponent<GUIText>().text = "MISS!";
		message.GetComponent<GUIText>().text = "MISS!";
		yield return new WaitForSeconds (1.0f);		// 2秒待つ
		messageShadow.GetComponent<GUIText>().text = "";
		message.GetComponent<GUIText>().text = "";
		initialize();
		CameraControll.newCam = 1;
		gameState = STATE.PLAY;
	}

	private IEnumerator GameOver(){
		CameraControll.newCam = 2;
		gameState = STATE.GAMEOVER;
		messageShadow.GetComponent<GUIText>().text = "GameOver!";
		message.GetComponent<GUIText>().text = "GameOver!";
		yield return new WaitForSeconds (5.0f);		// 5秒待つ
		messageShadow.GetComponent<GUIText>().text = "";
		message.GetComponent<GUIText>().text = "";
		initialize();
		CameraControll.newCam = 2;
		gameState = STATE.TITLE;
	}

	void OnGUI(){
		GUI.Label (new Rect (5,  0, 100, 16), "GAME_STATE:" + gameState.ToString(), style);
	}

}
