using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class TerminalHandler : MonoBehaviour
{
   
    public GameObject activatedTerminalScreen, hackedTerminalScreen;

    public GameObject activatedTerminalScreenPreselectedButton, hackedTerminalScreenPreselectedButton;

    public void Start()
    {
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
    }
    public void activateTerminal()
    {
        Time.timeScale = 0;
        hackedTerminalScreen.SetActive(false);
        activatedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(activatedTerminalScreenPreselectedButton);
    }

    public void hackTerminal()
    {
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(hackedTerminalScreenPreselectedButton);
    }

    public void closeTerminal()
    {
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        Time.timeScale = 1;
    }

}
