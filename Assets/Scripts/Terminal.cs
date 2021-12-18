using System.Collections;
using UnityEngine;

public class Terminal : MonoBehaviour
{

    public int number;
    public int employeeId;

    // What the terminal prompts the player when it is activated.
    [TextArea]
    public string textPrompt;

    // The info the terminal prompts the player after it has been hacked.
    [TextArea]
    public string dataDump;

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}