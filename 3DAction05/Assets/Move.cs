using UnityEngine;
using System.Collections;

public class Move : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		//右の矢印キーを押したら、現在の座標に対してｘの値をプラスする
		if(Input.GetKey (KeyCode.D)){
			transform.position += new Vector3( 0.1f, 0.0f, 0.0f );
		}
		if(Input.GetKey (KeyCode.A)){
			transform.position -= new Vector3( 0.1f, 0.0f, 0.0f );
		}
		if(Input.GetKey (KeyCode.W)){
			transform.position += new Vector3( 0.0f, 0.0f, 0.1f );
		}
		if(Input.GetKey (KeyCode.S)){
			transform.position -= new Vector3( 0.0f, 0.0f, 0.1f );
		}
	}
}
