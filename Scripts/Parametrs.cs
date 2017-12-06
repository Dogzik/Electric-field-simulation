using UnityEngine;

public class Parametrs : MonoBehaviour {
    public float charge;

    private void OnTriggerEnter(Collider col) {
        if (col.gameObject.tag == "Phy")
        {
            charge += col.gameObject.GetComponent<Obj_params>().charge;
        }
    }

    private void OnTriggerExit(Collider col) {
        if (col.gameObject.tag == "Phy")
            charge -= col.gameObject.GetComponent<Obj_params>().charge;
    }
}