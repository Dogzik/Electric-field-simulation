using UnityEngine;

public class Selection : MonoBehaviour {
    private Color[] colors = new Color[] {
        Color.white,
        Color.blue,
        Color.red,
        Color.yellow
    };

    private bool firstSelect = true;
    // Use this for initialization
    private void Start() { }

    // Update is called once per frame
    private void Update() { }

    public void Select(int x) {
        if (firstSelect) {
            colors[0] = colors[x];
            firstSelect = false;
        }
        gameObject.GetComponent<Renderer>().material.color = colors[x];
    }
}