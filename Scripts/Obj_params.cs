using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Obj_params : MonoBehaviour {

    public float charge = 0;

    void Start()
    {
        if (charge > 0)
        {
            gameObject.GetComponent<Selection>().Select(2);
        } else
        {
            gameObject.GetComponent<Selection>().Select(1);
        }
    }
}
