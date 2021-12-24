using System.Collections;
using UnityEngine;

public class Terminal : MonoBehaviour
{
    public int number;
    public int IDToActivate;

    public bool IsActivated { get; set; } = false;

    // Texts displayed within the in-game terminal menus.

    // Text prompted by the terminal when activated.
    [TextArea]
    public string textWhenActivated;
    // First chunk of text prompted by the terminal when hacked.
    // This has to hold the data-dumped employee table.
    [TextArea]
    public string text1WhenHacked;
    // Second chunk of text prompted by the terminal when hacked.
    [TextArea]
    public string text2WhenHacked;

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}