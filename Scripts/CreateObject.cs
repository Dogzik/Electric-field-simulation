using UnityEngine;
using UnityEngine.UI;

public class CreateObject : MonoBehaviour {
    private readonly int CreatePositionX = 0;
    private readonly int CreatePositionY = 0;
    private readonly int TimePositionX = 225;
    private readonly int TimePositionY = 0;
    public GameObject curObject;
    public GameObject[] forms = new GameObject[10];
    private bool isTarget;
    public GameObject mesh;
    public GameObject objForCreating;
    public Toggle plane_editor;
    private Vector3 position;
    public GameObject target;
    public GameObject targetObject;
    private bool toggleMesh;
    public Dropdown type_sector;

    public InputField user_charge;

    // Use this for initialization
    private void Start() {
        isTarget = false;
        user_charge.text = "0";
    }

    public void Load_scene(int num)
    {
        Application.LoadLevel(num);
    }

    public void Edit_plane() {
        toggleMesh = plane_editor.isOn;
    }

    public void Create_object() {
        if (isTarget) {
            objForCreating = forms[type_sector.value];
            objForCreating.GetComponent<Obj_params>().charge = float.Parse(user_charge.text);
            Instantiate(objForCreating, position, Quaternion.identity);
            isTarget = false;
        }
    }

    public void Delete_object() {
        if (curObject != null && curObject.tag == "Phy") {
            curObject.transform.position = new Vector3(1000, 1000, 1000);
            Destroy(curObject, 1);
            curObject = null;
        }
    }

    public void Update_lines() {
        var objs = GameObject.FindGameObjectsWithTag("Line");
        foreach (var o in objs)
            Destroy(o);
        GameObject.Find("Initiator").GetComponent<Main>().Draw_lines();
    }

    // Update is called once per frame
    private void Update() {
        if (isTarget) {
            target.SetActive(true);
            target.transform.position = position;
        }
        else {
            target.SetActive(false);
        }
        mesh.GetComponent<Renderer>().enabled = toggleMesh;
        mesh.GetComponent<Collider>().enabled = toggleMesh;

        if (Input.GetMouseButtonDown(0)) {
            var ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit)) {
                if (hit.collider.gameObject.tag == "Phy") {
                    if (curObject != null && curObject.tag == "Phy")
                        curObject.GetComponent<Selection>().Select(0);
                    curObject = hit.collider.gameObject;
                    curObject.GetComponent<Selection>().Select(3);
                }
                if (hit.collider.gameObject.tag == "Plane") {
                    position = hit.point;
                    isTarget = true;
                }
            }
        }
    }
}