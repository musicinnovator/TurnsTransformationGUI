# TurnsTransformationGUI

TurnsTransformationGUI is a tool for transforming musical note data using "turn" ornamentation variants, similar to slides, snaps, and trills, but with more advanced control. The application provides a GUI-independent C++ engine to process note data, apply selected turn variants, and output both transformed text and MIDI files. It features percentage-based control for transformation, multiple choice variant selection, and a random variant pool for user-driven ornamentation.

## Features

- **Turn Variant Processing:** Supports a wide range of turn ornamentation variants, including basic, front/back, trilled, P32, snapped, and between-notes types.
- **Multiple Choice Variant Selection:** Users can select specific variants or opt for random selection from a pool.
- **Percentage-Based Transformation:** Specify the percentage of eligible notes to be transformed.
- **MIDI Output Generation:** Converts processed results into a MIDI file with correct timing and sync.
- **No External GUI Dependencies:** Can be integrated into other applications or run as a command-line tool.

## How It Works

1. **Input File Processing:** Reads a note data file, typically with columns for track, note, duration, and label.
2. **Eligibility Check:** Only notes with certain labels are eligible for transformation.
3. **Variant Selection:** For each eligible note, applies a selected or random turn variant, according to the transformation percentage.
4. **Output Generation:** Writes the transformed results to an output text file and (optionally) a MIDI file.

## Example Workflow

1. Prepare your input file in the format:
    ```
    Track Note Duration Label
    1     C4   480      I8
    1     D4   240      U2R
    ```

2. Run the transformation process (C++ API or GUI).
3. Review the output file for transformed notes.

## Example Output

Text output (`output.txt`):

```
Track      Note       Duration            Label               Turn_Variant             
---------------------------------------------------------------------------------
1          D4         120                 I8                  Turn                     
1          F4         120                 I8                  Turn                     
1          C#4        120                 I8                  Turn                     
1          D4         120                 I8                  Turn                     
1          D4         480                 U2R                 ORIGINAL                 
```

MIDI output (binary file, not shown here) will include the transformed notes in correct sequence and timing.

## Supported Turn Variants (selection)

- `Turn`: Whole step above, principal note, half step below
- `FTurnF`: Half step above, principal note, whole step below
- `FBTurnS`: Half step above, principal, whole step below
- `TT`: Trilled regular turn
- `P32T`: P32 regular turn
- `ST`: Snapped regular turn
- ...and many more (see source code for full list)

## Building and Running

1. Clone the repository:
    ```
    git clone https://github.com/musicinnovator/TurnsTransformationGUI.git
    ```
2. Build using your preferred C++ compiler:
    ```
    g++ -std=c++17 -o TurnsTransformation TurnsTransformation.cpp
    ```
3. Run the application:
    ```
    ./TurnsTransformation input.txt output.txt
    ```

## License

Currently unlicensed. Please contact the author for usage permissions.

## Author

- [musicinnovator](https://github.com/musicinnovator)

## Repository

[TurnsTransformationGUI on GitHub](https://github.com/musicinnovator/TurnsTransformationGUI)
