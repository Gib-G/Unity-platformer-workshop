using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TileAttributes : MonoBehaviour
{
    public bool isSolid = true;
    public bool isLevelGoal = false;
    public bool isDeathBlock = false;
    public bool isTerminal = false;

    // If this tile is a terminal.
    public GameObject terminal;
}
