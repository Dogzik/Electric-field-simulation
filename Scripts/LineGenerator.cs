using UnityEngine;

public class LineGenerator : MonoBehaviour {
    public GameObject line;

    // Use this for initialization
    private void Start() { }

    // Update is called once per frame
    private void Update() { }

    public void createLine(Vector3[] points) {
        var temp = Instantiate(line, points[0], Quaternion.identity);
        temp.GetComponent<Line>().pointers = points;
        temp.GetComponent<Line>().update();
    }
}