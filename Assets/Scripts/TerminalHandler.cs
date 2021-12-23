using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using System.IO.Ports;

public class TerminalHandler : MonoBehaviour
{
   
    private GameObject initScreen, activatedTerminalScreen, hackedTerminalScreen;

    private GameObject initScreenPreselectedButton, activatedTerminalScreenPreselectedButton, hackedTerminalScreenPreselectedButton;

    // private Text...

    // The name of the serial port to use to communicate with
    // the Arduino.
    public string serialPort;

    public int baudrate;

    private SerialPort _serialPort;

    // True during the initialization phase.
    private bool initializing = true;

    public void Start()
    {
        // Some initialization.
        initScreen = transform.Find("Init").gameObject;
        activatedTerminalScreen = transform.Find("ActivatedTerminal").gameObject;
        hackedTerminalScreen = transform.Find("hackedTerminal").gameObject;

        initScreenPreselectedButton = initScreen.transform.Find("CancelButton").gameObject;
        activatedTerminalScreenPreselectedButton = activatedTerminalScreen.transform.Find("QuitButton").gameObject;
        hackedTerminalScreenPreselectedButton = hackedTerminalScreen.transform.Find("IDInput").gameObject;

        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        initScreen.SetActive(true);

        _serialPort = new SerialPort(serialPort, baudrate);
        _serialPort.Open();

        // Tells the Arduino to initiate the init phase.
        _serialPort.WriteLine("INIT");
    }

    public void Update()
    {
        if (!initializing || _serialPort.BytesToRead <= 0) return;

        // The Arduino has successfully finished its initialization phase.
        if (_serialPort.ReadLine() == "OK")
        {
            initializing = false;
            initScreen.SetActive(false);
        }
    }
    public void activateTerminal(Terminal terminal)
    {
        Time.timeScale = 0;
        initScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        activatedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(activatedTerminalScreenPreselectedButton);
    }

    public void hackTerminal()
    {
        initScreen.SetActive(false);
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(hackedTerminalScreenPreselectedButton);
    }

    public void closeTerminal()
    {
        initScreen.SetActive(false);
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        Time.timeScale = 1;
    }

}
