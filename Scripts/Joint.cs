using UnityEngine;

public class Joint : MonoBehaviour {
    private Vector3 direction;

    public GameObject target;

    // Use this for initialization
    private void Start() {
        direction = gameObject.transform.position - target.transform.position;
    }

    // Update is called once per frame
    private void Update() {
        var temp = gameObject.transform.position;
        temp.y = direction.y + target.transform.position.y;
        gameObject.transform.position = temp;
    }
}