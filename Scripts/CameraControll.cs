using UnityEngine;
using System.Collections;

public class CameraControll : MonoBehaviour {
	public GameObject camera;
	// Use this for initialization
	void Start () {
		camera = GameObject.FindGameObjectWithTag("MainCamera");
	}
	
	public enum RotationAxes { MouseXAndY = 0, MouseX = 1, MouseY = 2 }
	public RotationAxes axes = RotationAxes.MouseXAndY;
	public float sensitivityX = 15F;
	public float sensitivityY = 15F;

	public float minimumX = -360F;
	public float maximumX = 360F;

	public float minimumY = -60F;
	public float maximumY = 60F;

	float rotationY = 0F;
	private float speed = 0.3f;
    private float speedWheel = 5f;

    // Update is called once per frame
    void Update () {
		if(Input.GetMouseButton(2) || Input.GetMouseButton(1)){
			if (axes == RotationAxes.MouseXAndY){
				float rotationX = transform.localEulerAngles.y + Input.GetAxis("Mouse X") * sensitivityX;
				
				rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
				rotationY = Mathf.Clamp (rotationY, minimumY, maximumY);
				
				transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
			}
			else if (axes == RotationAxes.MouseX){
				transform.Rotate(0, Input.GetAxis("Mouse X") * sensitivityX, 0);
			}
			else{
				rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
				rotationY = Mathf.Clamp (rotationY, minimumY, maximumY);
				
				transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
			}
		}
		if(Input.GetKey(KeyCode.LeftShift)){
			speed = 1;
		    speedWheel = 9;
		}
		else {
		    speed = 0.3f;
		    speedWheel = 3;
		}
		
		gameObject.transform.position += gameObject.transform.forward * Input.GetAxis("Vertical") * speed;
		gameObject.transform.position += gameObject.transform.right * Input.GetAxis("Horizontal") * speed;
		
		camera.transform.localPosition += new Vector3(0, 0, -(Input.GetAxis("Mouse ScrollWheel")) * speedWheel);
	}
}
