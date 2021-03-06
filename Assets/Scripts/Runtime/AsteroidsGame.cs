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
    public float m_ShipMaxSpeed = 3.0f;

    public uint m_MaxAsteroidsCount = 10;
    public float m_MinAsteroidsSpeed = 1.0f;
    public float m_MaxAsteroidsSpeed = 5.0f;

    public float m_FireRate = 10.0f;
    public float m_BulletLifeSpan = 1.0f;
    public float m_BulletSpeed = 5.0f;

    public GameObject m_Camera;
    public GameObject m_Ship;
    public GameObject[] m_AsteroidTemplates;
    public GameObject m_BulletTemplate;

    private IntPtr m_GamePtr = IntPtr.Zero;
    private uint m_TotalAsteroidsCount;
    private float[] m_AsteroidsPositionsArray;
    private uint m_BulletsCount;
    private float[] m_BulletsPositionsArray;

    private GameObject[] m_Asteroids;
    private GameObject[] m_Bullets;

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
    static extern IntPtr AllocateGamePtr(float shipSpeed,
                                         float shipRotationSpeed,
                                         float shipMaxSpeed,
                                         float shipSqrRadius,
                                         uint asteroidTemplatesCount,
                                         uint maxAsteroidsCount,
                                         float minAsteroidsSpeed,
                                         float maxAsteroidsSpeed,
                                         float fireRate,
                                         float bulletLifeSpan,
                                         float bulletSpeed,
                                         float bulletSqrRadius,
                                         Vector2 viewportSize);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void DetroyGamePtr(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern uint SetAsteroidTemplateSqrRadius(IntPtr gamePtr, uint level, float sqrRadius);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern uint GetAsteroidsCount(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern uint GetBulletsCount(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void GetAsteroidsPositions(IntPtr gamePtr, out IntPtr ptrPositions);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void GetBulletsPositions(IntPtr gamePtr, out IntPtr ptrPositions);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern Vector3 GetShipPosRot(IntPtr gamePtr);

    [DllImport(AsteroidNativeDLL, CallingConvention = CallingConvention.Cdecl)]
    static extern void Update(IntPtr gamePtr, int keyState, float deltaTime);

    #endregion

    float GetSqrRadiusFromBound(Bounds bounds)
    {
        return bounds.extents.x * bounds.extents.x;
    }

    void ReInitGame()
    {
        // Cleaning
        if (m_GamePtr != null)
            DetroyGamePtr(m_GamePtr);

        if (m_Asteroids != null)
        {
            foreach (GameObject asteroid in m_Asteroids)
            {
                Destroy(asteroid);
            }
            m_Asteroids = null;
        }

        if (m_Bullets != null)
        {
            foreach (GameObject bullet in m_Bullets)
            {
                Destroy(bullet);
            }
            m_Bullets = null;
        }

        // Calculate viewport
        Assert.IsNotNull(m_Camera, "Please assign the camera");
        Camera camera = m_Camera.GetComponent<Camera>();
        Assert.IsNotNull(camera, "The camera gameobject doesn't have a camera component");
        Vector2 viewportSize = new Vector2(camera.orthographicSize * camera.aspect, camera.orthographicSize);

        // Retreive Ship Radius
        Assert.IsNotNull(m_Ship, "Please assign the ship");
        SpriteRenderer shipRenderer = m_Ship.GetComponent<SpriteRenderer>();
        Assert.IsNotNull(shipRenderer, "The ship gameobject doesn't have a sprite renderer");
        float shipSqrRadius = GetSqrRadiusFromBound(shipRenderer.bounds);

        // Retreive Bullet Radius
        Assert.IsNotNull(m_BulletTemplate, "Please assign the bullet");
        SpriteRenderer bulletRenderer = m_BulletTemplate.GetComponent<SpriteRenderer>();
        Assert.IsNotNull(bulletRenderer, "The bullet gameobject doesn't have a sprite renderer");
        float bulletSqrRadius = GetSqrRadiusFromBound(bulletRenderer.bounds);

        m_GamePtr = AllocateGamePtr(m_ShipControlSpeed,
                                    m_ShipControlRotationSpeed,
                                    m_ShipMaxSpeed,
                                    shipSqrRadius,
                                    (uint)m_AsteroidTemplates.Length,
                                    m_MaxAsteroidsCount,
                                    m_MinAsteroidsSpeed,
                                    m_MaxAsteroidsSpeed,
                                    m_FireRate,
                                    m_BulletLifeSpan,
                                    m_BulletSpeed,
                                    bulletSqrRadius,
                                    viewportSize);

        // Preallocate Asteroids
        m_TotalAsteroidsCount = GetAsteroidsCount(m_GamePtr);
        m_Asteroids = new GameObject[m_TotalAsteroidsCount];
        m_AsteroidsPositionsArray = new float[m_TotalAsteroidsCount * 2];

        uint level = 0;
        uint index = 0;
        foreach (GameObject template in m_AsteroidTemplates)
        {
            // We expect the data to be correct and cannot work properly if not
            Assert.IsNotNull(template, "One of the asteroid template is null");
            SpriteRenderer asteroidRenderer = template.GetComponent<SpriteRenderer>();
            Assert.IsNotNull(asteroidRenderer, "One of the asteroid gameobject doesn't have a sprite renderer");

            SetAsteroidTemplateSqrRadius(m_GamePtr, level, GetSqrRadiusFromBound(asteroidRenderer.bounds));

            for (uint i = 0; i < m_MaxAsteroidsCount * Math.Pow(2, level); i++)
            {
                m_Asteroids[index] = Instantiate(template);
                m_Asteroids[index].SetActive(false);
                index++;
            }
            level++;
        }

        m_BulletsCount = GetBulletsCount(m_GamePtr);
        m_Bullets = new GameObject[m_BulletsCount];
        m_BulletsPositionsArray = new float[m_BulletsCount * 2];
        // Preallocate Bullets
        for (uint i = 0; i < m_BulletsCount; i++)
        {
            m_Bullets[i] = Instantiate(m_BulletTemplate);
            m_Bullets[i].SetActive(false);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        ReInitGame();
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
        // TODO: Optimize by only transfering active asteroids
        IntPtr ptrPositions;
        GetAsteroidsPositions(m_GamePtr, out ptrPositions);
        Marshal.Copy(ptrPositions, m_AsteroidsPositionsArray, 0, (int)m_TotalAsteroidsCount * 2);
        for (uint i = 0; i < m_TotalAsteroidsCount; i++)
        {
            // We use NAN to know if an asteroid is active
            bool active = !float.IsNaN(m_AsteroidsPositionsArray[i * 2]);
            if (m_Asteroids[i].activeSelf != active)
                m_Asteroids[i].SetActive(active);
            if (active)
                m_Asteroids[i].transform.position = new Vector2(m_AsteroidsPositionsArray[i * 2], m_AsteroidsPositionsArray[i * 2 + 1]);
        }

        // Update bullets
        // TODO: Factorise code with above
        GetBulletsPositions(m_GamePtr, out ptrPositions);
        Marshal.Copy(ptrPositions, m_BulletsPositionsArray, 0, (int)m_BulletsCount * 2);
        for (uint i = 0; i < m_BulletsCount; i++)
        {
            // We use NAN to know if an asteroid is active
            bool active = !float.IsNaN(m_BulletsPositionsArray[i * 2]);
            if (m_Bullets[i].activeSelf != active)
                m_Bullets[i].SetActive(active);
            if (active)
                m_Bullets[i].transform.position = new Vector2(m_BulletsPositionsArray[i * 2], m_BulletsPositionsArray[i * 2 + 1]);
        }
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

        if (Input.GetKey(KeyCode.R))
        {
            // Restart
            ReInitGame();
        }

        Update(m_GamePtr, keyState, Time.deltaTime);

        SynchronizeVisuals();
    }
}
