using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Assertions;

public class AsteroidsGame : MonoBehaviour
{
    public float m_ShipControlSpeed = 1.0f;
    public float m_ShipControlRotationSpeed = 30.0f;
    public uint m_MaxAsteroidsCount = 10;

    public GameObject m_Camera;
    public GameObject m_Ship;
    public GameObject[] m_AsteroidTemplates;

    private IntPtr m_GamePtr = IntPtr.Zero;
    private uint m_TotalAsteroidsCount;

    private GameObject[] m_Asteroids;

    enum KeyState : int
    {
        Left = 1 << 0,
        Right = 1 << 1,
        Up = 1 << 2,
        Down = 1 << 3,
        Space = 1 << 4,
    }

    #region Native

#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
    private const string AsteroidNativeDLL = "AsteroidsCPP.dll";
#elif UNITY_EDITOR_OSX || UNITY_STANDALONE_OSX
    private const string AsteroidNativeDLL = "__Internal";
#endif

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr HelloWorld();

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern int AddNumbers(int a, int b);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr AllocateGamePtr(float shipSpeed, float shipRotationSpeed, uint asteroidTemplatesCount, uint maxAsteroidsCount, Vector2 viewportSize);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void DetroyGamePtr(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern uint GetAsteroidsCount(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void GetAsteroidsPositions(IntPtr gamePtr, out IntPtr ptrPositions);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern Vector3 GetShipPosRot(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void Update(IntPtr gamePtr, int keyState, float deltaTime);

    #endregion

    // Start is called before the first frame update
    void Start()
    {
        // Calculate viewport
        Assert.IsNotNull(m_Camera, "Please assign the camera");
        Camera camera = m_Camera.GetComponent<Camera>();
        Assert.IsNotNull(camera, "The camera gameobject doesn't have a camera component");
        Vector2 viewportSize = new Vector2(camera.orthographicSize * camera.aspect, camera.orthographicSize);

        // Allocate the game instance
        m_GamePtr = AllocateGamePtr(m_ShipControlSpeed, m_ShipControlRotationSpeed, (uint)m_AsteroidTemplates.Length, m_MaxAsteroidsCount, viewportSize);

        // Preallocate Asteroids
        m_TotalAsteroidsCount = GetAsteroidsCount(m_GamePtr);
        m_Asteroids = new GameObject[m_TotalAsteroidsCount];

        uint level = 0;
        uint index = 0;
        foreach (GameObject template in m_AsteroidTemplates)
        {
            // We expect the data to be correct and cannot work properly if not
            Assert.IsNotNull(template, "One of the asteroid template is null");

            for (uint i = 0; i < m_MaxAsteroidsCount * Math.Pow(2, level); i++)
            {
                m_Asteroids[index] = Instantiate(template);
                m_Asteroids[index].SetActive(false);
                index++;
            }
            level++;
        }
    }
    void OnDestroy()
    {
        DetroyGamePtr(m_GamePtr);
    }

    void SynchronizeVisuals()
    {
        // Update the ships
        if (m_Ship)
        {
            Vector3 shipPosRot = GetShipPosRot(m_GamePtr);
            m_Ship.transform.position = new Vector2(shipPosRot.x, shipPosRot.y);
            m_Ship.transform.rotation = Quaternion.Euler(0.0f, 0.0f, shipPosRot.z);
        }

        // Update Asteroids
        IntPtr ptrPositions;
        GetAsteroidsPositions(m_GamePtr, out ptrPositions);
        IntPtr p = ptrPositions;
        Vector2 position;
        for (uint i = 0; i < m_TotalAsteroidsCount; i++)
        {
            position = (Vector2)Marshal.PtrToStructure(p, typeof(Vector2));
            p += Marshal.SizeOf(typeof(Vector2));

            bool isNAN = float.IsNaN(position.x);
            m_Asteroids[i].SetActive(!isNAN);
            if (!isNAN)
                m_Asteroids[i].transform.position = position;
        }

        // Update bullets
    }

    // Update is called once per frame
    void Update()
    {
        if(m_GamePtr == null)
            return;

        int keyState = 0;

        keyState |= Input.GetKey(KeyCode.LeftArrow) ? (int)KeyState.Left : 0;
        keyState |= Input.GetKey(KeyCode.RightArrow) ? (int)KeyState.Right : 0;
        keyState |= Input.GetKey(KeyCode.UpArrow) ? (int)KeyState.Up : 0;
        keyState |= Input.GetKey(KeyCode.DownArrow) ? (int)KeyState.Down : 0;
        keyState |= Input.GetKey(KeyCode.Space) ? (int)KeyState.Space : 0;

        Update(m_GamePtr, keyState, Time.deltaTime);

        SynchronizeVisuals();
    }
}
