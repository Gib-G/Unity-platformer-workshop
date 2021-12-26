using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using System.IO.Ports;
using UnityEngine.UI;

public class TerminalHandler : MonoBehaviour
{
    // Since we can't write into the RFID tag, we store the employee
    // ID here, instead of in the badge/card.
    private string IDInCard;
    private int activatedTerminals = 0;

    // The terminal the player is facing in the game, if any.
    // Null otherwise.
    private Terminal? currentTerminal = null;

    public GameObject firstTerminal;

    // Menu related stuff.
    private GameObject initScreen, activatedTerminalScreen, hackedTerminalScreen;
    private GameObject initScreenPreselectedButton, activatedTerminalScreenPreselectedButton, hackedTerminalScreenPreselectedButton;
    private InputField IDInput;

    // The text game objects used to prompt terminal messages
    // (Terminal.textWhenActivated, Terminal.text1WhenHacked, etc...).
    private Text textWhenActivated, text1WhenHacked, text2WhenHacked;

    // Serial port stuff used to communicate with the Arduino part.
    public string serialPort = "COM3";
    public int baudrate = 115200;
    private SerialPort _serialPort;

    // Last message sent to the Arduino part on the serial bus.
    private string lastMessage = "";

    // A flag set to false when the init phase is done.
    private bool initializing = true;

    // A flag indicating wether the program should ignore "card" 
    // messages from the serial bus. This is useful to avoid checking
    // the card's ID right after the player has activated a terminal.
    private bool ignoreCard = false;

    public void Start()
    {
        Time.timeScale = 0;

        // The ID stored in the user's badge/card gets initialized with
        // the ID needed to activate the first terminal.
        IDInCard = firstTerminal.GetComponent<Terminal>().IDToActivate.ToString();

        // Gathering some child game objects.
        initScreen = transform.Find("Init").gameObject;
        activatedTerminalScreen = transform.Find("ActivatedTerminal").gameObject;
        hackedTerminalScreen = transform.Find("HackedTerminal").gameObject;
        initScreenPreselectedButton = initScreen.transform.Find("CancelButton").gameObject;
        activatedTerminalScreenPreselectedButton = activatedTerminalScreen.transform.Find("QuitButton").gameObject;
        hackedTerminalScreenPreselectedButton = hackedTerminalScreen.transform.Find("IDInput").gameObject;
        textWhenActivated = activatedTerminalScreen.transform.Find("TerminalPrompt").gameObject.GetComponent<Text>();
        text1WhenHacked = hackedTerminalScreen.transform.Find("TerminalPrompt1").gameObject.GetComponent<Text>();
        text2WhenHacked = hackedTerminalScreen.transform.Find("TerminalPrompt2").gameObject.GetComponent<Text>();
        IDInput = hackedTerminalScreen.transform.Find("IDInput").gameObject.GetComponent<InputField>();

        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        initScreen.SetActive(true);

        _serialPort = new SerialPort(serialPort, baudrate);
        _serialPort.Open();
        // Tells the Arduino to initiate the init phase.
        _serialPort.WriteLine("init");
    }

    public void Update()
    {
        if (_serialPort.BytesToRead <= 0) return;

        // The Arduino has successfully finished its initialization phase.
        string message = _serialPort.ReadLine();
        if (message == "init:ok")
        {
            initializing = false;
            initScreen.SetActive(false);
            Time.timeScale = 1;
        }
        // The Arduino part has successfully written the new employee ID
        // in the user's badge/card.
        else if(message == "write:ok")
        {
            IDInCard = IDInput.text;
        }
        // User scans their badge...
        else if(message == "card")
        {
            // ...in front of a terminal.
            if(currentTerminal != null)
            {
                if(ignoreCard) { return; }
                // Meant to prevent checking the ID in the card right after the player
                // has activated the terminal.
                if (currentTerminal.IDToActivate.ToString() == IDInCard)
                {
                    // The ID on the badge and the one needed to activate the 
                    // terminal match. The terminal gets activated.
                    // We notify the Arduino part.
                    _serialPort.WriteLine("card:ok");
                    // The terminal prompt opens. We can now ignore
                    // any further auth attempt.
                    ignoreCard = true;
                    activateTerminal(currentTerminal);
                }
                else
                {
                    // The ID on the badge and the one needed to activate the 
                    // terminal don't match.
                    // We notify the Arduino part.
                    _serialPort.WriteLine("card:ko");
                }
            }
            // ...not in front of a terminal.
            // In this case, we just want to send the ID stored on the card to the 
            // Arduino part, so the player can check it anytime!
            else
            {
                _serialPort.WriteLine("id:" + IDInCard);
            }
        }
    }

    public void sendTerminalNumber(Terminal terminal)
    {
        currentTerminal = terminal;
        if(currentTerminal != null)
        {
            string message = "terminal:" + terminal.number;
            // Comparing the message to send to the previously sent message.
            // If they're the same, no need to send again (don't spam the serial bus).
            if(message == lastMessage) { return; }
            lastMessage = message;
            _serialPort.WriteLine(message);
        }
        else
        {
            // Same as above.
            // We also prevent sending a first undesired "no_terminal" during the 
            // init phase.
            if(initializing || lastMessage == "terminal:none") { return; }
            lastMessage = "terminal:none";
            _serialPort.WriteLine("terminal:none");
        }
    }
    public void activateTerminal(Terminal terminal)
    {
        Time.timeScale = 0;
        activatedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(activatedTerminalScreenPreselectedButton);

        textWhenActivated.text = terminal.textWhenActivated;

        if(!terminal.IsActivated)
        {
            terminal.IsActivated = true;
            activatedTerminals++;
        }
        // All terminals are activated. The player can finish the level.
        if(activatedTerminals > 3)
        {
            Game.CompleteLevel();
        }
    }

    // Called when the "Hack terminal" button is pressed in
    // the activated terminal menu.
    public void hackTerminal()
    {
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(true);
        EventSystem.current.SetSelectedGameObject(null);
        EventSystem.current.SetSelectedGameObject(hackedTerminalScreenPreselectedButton);

        text1WhenHacked.text = currentTerminal.text1WhenHacked;
        text2WhenHacked.text = currentTerminal.text2WhenHacked;

        // Notifying the Arduino part.
        _serialPort.WriteLine("hacked");
    }

    // Called when the "Save ID" button is pressed in the 
    // hacked terminal menu.
    public void reconfigureBadge()
    {
        // Notify the Arduino side we want to write on the badge.
        _serialPort.WriteLine("write");
    }

    public void closeTerminal()
    {
        initScreen.SetActive(false);
        activatedTerminalScreen.SetActive(false);
        hackedTerminalScreen.SetActive(false);
        // Authenticating to a terminal is possible again.
        ignoreCard = false;
        // Tells the Arduino side to cancel all pending write
        // operations.
        _serialPort.WriteLine("write:cancel");
        Time.timeScale = 1;
    }

    public void loadMainMenu()
    {
        activatedTerminalScreen.SetActive(true);
        hackedTerminalScreen.SetActive(true);
        initScreen.SetActive(true);
        // Authenticating to a terminal is possible again.
        ignoreCard = false;
        // Tells the Arduino side that the init phase has been
        // canceled.
        _serialPort.WriteLine("init:cancel");
        Game.DisplayStartMenu();
    }
}
