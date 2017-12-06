using System;
using System.Collections;
using UnityEngine;
using Random = System.Random;

public class Main : MonoBehaviour {
    private const float eps = 1e-3f;
    private const float dt = 5;
    private const int numberOfSmallIteration = 5;
    public const int T = 50;

    public static int N = T;
    public static int M = T;
    public static int K = T;

    private static readonly Random rng = new Random();

    private readonly float jump = 1f;
    private readonly int numberOfStart = 200;
    private readonly float scale = 3f;
    public GameObject bounding_box, box;
    private int numberOfIteration = 1000;
    private float range;

    public GameObject Small_sphere;
    public GameObject[,,] spheres = new GameObject[N + 1, M + 1, K + 1];
    private Vector3 startVector3;

    private void Start() {
        startVector3 = get_vector(N / 2f, M / 2f, K / 2f);
        range = jump * numberOfIteration / 3f;

        for (var x = 0; x <= N; x++)
            for (var y = 0; y <= M; y++)
                for (var z = 0; z <= K; z++)
                    spheres[x, y, z] = Instantiate(Small_sphere, new Vector3(x, y, z) * scale, Quaternion.identity);
        bounding_box.transform.localScale = new Vector3(N, M, K) * scale;
        box = Instantiate(bounding_box, new Vector3(N / 2f, M / 2f, K / 2f) * scale,
                          Quaternion.identity);
    }

    public bool Is_zero(float x) {
        return Math.Abs(x) < eps;
    }

    private bool check(int x, int y, int z) {
        return 0 <= x && x <= N && 0 <= y && y <= M && 0 <= z && z <= K;
    }

    private bool inside(Vector3 p) {
        return Vector3.Distance(startVector3, p) < range + eps;
    }
    private bool insideStrictly(Vector3 p) {
        return Vector3.Distance(startVector3, p) < range;
    }

    private bool isGoodPoint(Vector3 p) {
        var x = (int) Math.Round(p.x / scale);
        var y = (int) Math.Round(p.y / scale);
        var z = (int) Math.Round(p.z / scale);
        return inside(p)
               && (!check(x, y, z)
                   || spheres[x, y, z].GetComponent<Parametrs>().charge > -eps);
    }

    private float Get_value(float t, float a, float b, float c) {
        return (float) (t * Math.Pow(a * a + b * b + c * c, -1.5));
    }

    private Vector3 getGrad(Vector3 cord, float q) {
        if (Is_zero(cord.magnitude))
            return Vector3.zero;
        var x = cord.x;
        var y = cord.y;
        var z = cord.z;
        return new Vector3(Get_value(x, x, y, z), Get_value(y, x, y, z), Get_value(z, x, y, z)) * q;
    }

    private Vector3 get_vector(float x, float y, float z) {
        return new Vector3(x, y, z) * scale;
    }

    public static void Shuffle(ArrayList list) {
        for (var i = 0; i < list.Count; i++) {
            var k = rng.Next(i + 1);
            var value = list[i];
            list[i] = list[k];
            list[k] = value;
        }
    }

    private bool isStart(int x, int y, int z) {
        if (spheres[x, y, z].GetComponent<Parametrs>().charge < eps)
            return false;
        const int zone = 1;
        for (var dx = -zone; dx <= zone; dx++)
            for (var dy = -zone; dy <= zone; dy++)
                for (var dz = -zone; dz <= zone; dz++) {
                    if (dx == 0 && dy == 0 && dz == 0
                        || !check(x + dx, y + dy, z + dz))
                        continue;
                    var charge = spheres[x + dx, y + dy, z + dz].GetComponent<Parametrs>().charge;
                    if (Is_zero(charge))
                        return true;
                }
        return false;
    }

    private Vector3 getPointOnSphere(Vector3 centr, float radius) {
        var a = rng.NextDouble() * Math.PI;
        var b = rng.NextDouble() * Math.PI;
        var x = (float) (Math.Sin(a) * Math.Cos(b));
        var y = (float) (Math.Sin(a) * Math.Sin(b));
        var z = (float) Math.Cos(a);
        var shift = new Vector3(x, y, z) * (radius + eps);
        var result = centr + shift;
        return result;
    }

    private Vector3 getGrad(Vector3 direction, ArrayList tmp) {
        var grad = Vector3.zero;
        foreach (int[] position in tmp) {
            var xx = position[0];
            var yy = position[1];
            var zz = position[2];
            var charge = spheres[xx, yy, zz].GetComponent<Parametrs>().charge;
            var temp = get_vector(xx, yy, zz);
            grad += getGrad(direction - temp, charge);
        }
        return grad.normalized;
    }

    public void Draw_lines_2() {
        var tmp = new ArrayList();
        var start = new ArrayList();
        for (var x = 0; x <= N; x++)
            for (var y = 0; y <= M; y++)
                for (var z = 0; z <= K; z++) {
                    if (!Is_zero(spheres[x, y, z].GetComponent<Parametrs>().charge))
                        tmp.Add(new[] {
                            x, y, z
                        });
                    if (isStart(x, y, z))
                        start.Add(get_vector(x, y, z));
                }
        Shuffle(start);
        var all = new ArrayList();
        var count = Math.Min(numberOfStart, tmp.Count);
        for (var i = 0; i < Math.Min(start.Count, count); i++)
            all.Add(start[i]);
        for (var i = 0; i < count; i++)
            all.Add(getPointOnSphere(startVector3, range));
        foreach (Vector3 startPoint in all) {
            var line = new ArrayList();
            var direction = startPoint;
            line.Add(direction);
            var it = 0;
            while (isGoodPoint(direction) && it < numberOfIteration) {
                var grad = getGrad(direction, tmp);
                var grad2 = (getGrad(direction + dt * grad, tmp) - grad) / dt;
                for (var i = 0; i < numberOfSmallIteration
                                && it < numberOfIteration
                                && isGoodPoint(direction); i++, numberOfIteration++) {
                    if (Is_zero(grad.magnitude))
                        break;
                    direction += grad.normalized * jump;
                    grad += grad2 * jump;
                    line.Add(direction);
                }
                if (Is_zero(grad.magnitude))
                    break;
            }
            if (line.Count > 0) {
                var res = (Vector3[]) line.ToArray(typeof(Vector3));
                GameObject.Find("Main").GetComponent<LineGenerator>().createLine(res);
            }
        }
    }

    public void Draw_lines() {
        var tmp = new ArrayList();
        var start = new ArrayList();
        for (var x = 0; x <= N; x++)
            for (var y = 0; y <= M; y++)
                for (var z = 0; z <= K; z++) {
                    if (!Is_zero(spheres[x, y, z].GetComponent<Parametrs>().charge))
                        tmp.Add(new[] {
                            x, y, z
                        });
                    if (isStart(x, y, z))
                        start.Add(get_vector(x, y, z));
                }
        Shuffle(start);
        var all = new ArrayList();
        var count = Math.Min(numberOfStart, tmp.Count);
        for (var i = 0; i < Math.Min(start.Count, count); i++)
            all.Add(start[i]);
        for (var i = 0; i < count * 3; i++)
            all.Add(getPointOnSphere(startVector3, range));
        foreach (Vector3 startPoint in all) {
            var line = new ArrayList();
            var direction = startPoint;
            line.Add(direction);
            var it = 0;
            while (isGoodPoint(direction) && it < numberOfIteration) {
                var grad = getGrad(direction, tmp);
                if (Is_zero(grad.magnitude))
                    break;
                direction += grad * jump;
                if (insideStrictly(direction)) {
                    line.Add(direction);
                    it++;
                }
            }
            if (line.Count > 1) {
                var res = (Vector3[]) line.ToArray(typeof(Vector3));
                GameObject.Find("Main").GetComponent<LineGenerator>().createLine(res);
            }
        }
    }

    private void Pause() { }

    private void Resume() { }
}