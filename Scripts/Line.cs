using System;
using UnityEngine;

public class Line : MonoBehaviour {
    public float coefficientSmooth = 4;
    public int numberOfPoints = 10;
    public Vector3[] pointers;

    private float getCoefficient(float x) {
        x = (float) (Math.Abs(x) * Math.PI / 2);
        return (float) Math.Pow(Math.Abs(Math.Cos(x)), coefficientSmooth);
    }

    private void add_point(int k, int i, Vector3[] newPoint, Vector3 point, float[] K, int left, int right) {
        var maxDis = k * 3 / 2;
        for (var j = -maxDis; j <= maxDis; j++) {
            var x = i * k + j;
            if (x >= left && x < right) {
                var coefficient = getCoefficient((float) j / maxDis);
                K[x] += coefficient;
                newPoint[x] += coefficient * point;
            }
        }
    }

    public void update() {
        var points = new Vector3[pointers.Length];
        for (var i = 0; i < pointers.Length; i++)
            points[i] = pointers[i];
        /*Vector3[] newPoint = new Vector3[numberOfPoints];
        int k = (numberOfPoints + points.Length - 1) / points.Length;
        float[] K = new float[numberOfPoints];
        add_point(k, 0, newPoint, points[0], K, 0, numberOfPoints - k / 2);
        add_point(k, pointers.Length - 1, newPoint, points[pointers.Length - 1], K, k / 2, numberOfPoints);
        for (int i = 1; i < points.Length - 1; i++) {
            add_point(k, i, newPoint, points[i], K, k / 2, numberOfPoints - k / 2);
        }
        for (int i = 0; i < numberOfPoints; i++) {
            newPoint[i] /= K[i];
        }
        GetComponent<LineRenderer>().positionCount = numberOfPoints;
        GetComponent<LineRenderer>().SetPositions(newPoint);
        */
        GetComponent<LineRenderer>().positionCount = points.Length;
        GetComponent<LineRenderer>().SetPositions(points);
    }
}