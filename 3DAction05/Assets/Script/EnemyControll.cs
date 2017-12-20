using UnityEngine;
using System.Collections;

[RequireComponent (typeof (CharacterController))]
[RequireComponent (typeof (Animator))]
public class EnemyControll : MonoBehaviour {
	private GUIStyle style;
	public GUIStyleState styleState;



	CharacterController _controller;
	Animator _animator;
	Transform _transform;
	float speed = 1.5f;
	float gravity = 200f;
	Vector3 moveDirection;
	Vector3 _target;
	float maxRotSpeed = 200.0f;
	float minTime = 0.1f;
	float _velocity;
	
	bool change;
	float stopRange;
	float cancelRange;

	Vector3 startPosition;
	bool traceMode = false;

	public int hp = 10;
	public GameObject exp;
	private float time = 0;

	// Use this for initialization
	void Start () {
		style = new GUIStyle();
		style.fontSize = 12;
		styleState.textColor = Color.white;
		style.normal = styleState;

		_controller = GetComponent<CharacterController>();
		_animator = GetComponent<Animator>();
		_transform = GetComponent<Transform>();
		stopRange = 3f;
		cancelRange = 6f;

		startPosition = transform.position;

		_target = GetTarget();
		//Debug.Log ("target="+_target);

		InvokeRepeating ("NewTarget",0.01f,5.0f);
	}
	
	// Update is called once per frame
	void Update () {
		if(traceMode){
			//_animator.SetBool("isTrace", true);
			//Debug.Log ("isTrace is true");
			GameObject player = GameObject.FindWithTag("Player");
			_target = player.transform.position;
			if(Vector3.Distance(_transform.position,_target) < cancelRange){
				Move();
			}else{
				traceMode = false;
				_target = startPosition;
				_animator.SetBool("isTrace", false);
			}
			_animator.SetBool("isTrace", traceMode);
		}else{
			if(change){
				_target = GetTarget ();
				//Debug.Log ("target="+_target);
				change = false;
			}
			if(Vector3.Distance(_transform.position,_target) > stopRange){
				Move();
				_animator.SetBool("isMove", true);
			}else{
				//animation.CrossFade ("idle");
				_animator.SetBool("isMove", false);
			}
		}
	}

	Vector3 GetTarget(){
		return transform.position + new Vector3(Random.Range (-10,10),0,0);
	}

	void NewTarget(){
		if(traceMode) return;
		int choice = Random.Range (0,3);
		//Debug.Log ("time="+Time.time +" choice="+choice);
		switch(choice){
		case 0:
			change = true;
			break;
		case 1:
			change = true;
			break;
		case 2:
			_target = transform.position;
			break;
		}
	}

	void Move(){
		//移動のためのコード  
		moveDirection = _transform.forward;
		moveDirection *= speed;
		moveDirection.y -= gravity * Time.deltaTime;
		_controller.Move(moveDirection * Time.deltaTime);
		//回転のためのコード
		var newRotation = Quaternion.LookRotation(_target - _transform.position).eulerAngles;
		var angles = _transform.rotation.eulerAngles;
		_transform.rotation = Quaternion.Euler(angles.x,
		                                       Mathf.SmoothDampAngle(angles.y, newRotation.y, ref _velocity, minTime, maxRotSpeed),
		                                       angles.z);
	}

	void TraceModeOn(){
		Debug.Log ("TraceModeOn");
		traceMode = true;
	}
	void TraceModeOff(){
		//Debug.Log ("TraceModeOff");
		//traceMode = false;
	}
	void OnCollisionEnter(Collision other){
		if(other.gameObject.tag == "Player"){
			GameObject.Find("GameManager").SendMessage("Miss");
		}
	}

	void damege(){
		hp -= 1;
		if (hp <= 0) {
			Destroy (gameObject);
			Transform ins = (Transform)Instantiate (exp, transform.position, transform.rotation);
			Destroy(ins.gameObject, 3.0f);
		}
	}

	//void OnGUI(){
	//	GUI.Label (new Rect (5,  16, 100, 16), "ENEMY_traceMode:" + traceMode.ToString(), style);
	//	GUI.Label (new Rect (5,  32, 100, 16), "ENEMY_target x:" + _target.x.ToString("0.0"), style);
	//	GUI.Label (new Rect (5,  48, 100, 16), "ENEMY_target y:" + _target.y.ToString("0.0"), style);
	//	GUI.Label (new Rect (5,  64, 100, 16), "ENEMY_target z:" + _target.z.ToString("0.0"), style);
	//}
}
