using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.SceneManagement;

public class GameCompleteMenu : MonoBehaviour
{
    public GameObject preselectedButton;

    // Start is called before the first frame update
    void Start()
    {
        Game.currentLevelIndex = 0;

        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(preselectedButton);
    }

    public void DisplayStartMenu()
    {
        SceneManager.LoadScene(Game.StartMenuSceneName);
    }
}
