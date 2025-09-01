// Turns Transformation Tool - Main Entry Point
// This file provides the main function and platform-specific entry points
// for the Turns Transformation tool.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #include <commdlg.h>
    #include <commctrl.h>
    #pragma comment(lib, "comctl32.lib")
#elif defined(__linux__) || defined(__unix__)
    #define PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysym.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#else
    #error "Unsupported platform"
#endif

// Forward declarations of functions from TurnsTransformation.cpp
struct AppState {
    std::string inputFile;
    std::string outputFile;
    std::string midiOutputFile;
    double transformationPercentage = 50.0;
    std::vector<std::string> selectedVariants;
    bool processingComplete = false;
    std::string statusMessage;
    std::string resultSummary;
    int totalEligibleNotes = 0;
    int transformedNotes = 0;
    std::map<std::string, int> variantUsageCount;
};

// Forward declarations of functions from TurnsTransformation.cpp
void processFile(const std::string& inputFile, const std::string& outputFile, AppState& state);
void convertToMidi(const std::string& inputFile, const std::string& outputFile, AppState& state);

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Turns Transformation Tool";

// Define colors
#ifdef PLATFORM_WINDOWS
// Purple Scarlet background (a rich purple-red color)
#define BG_COLOR RGB(142, 45, 87)
// Light Purple for buttons
#define BTN_COLOR RGB(180, 160, 200)
#endif

#ifdef PLATFORM_WINDOWS
// Windows GUI implementation
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // Create a purple scarlet background brush
    wc.hbrBackground = CreateSolidBrush(BG_COLOR);
    wc.lpszClassName = "TurnsTransformationClass";
    RegisterClassEx(&wc);

    // Create application state
    AppState* state = new AppState();

    // Create window
    HWND hwnd = CreateWindowEx(
        0, "TurnsTransformationClass", WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, state
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    delete state;
    return (int)msg.wParam;
}

// Windows message handler
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Get the application state
    AppState* state = NULL;
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        state = (AppState*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
    } else {
        state = (AppState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch (uMsg) {
        case WM_CREATE: {
            // Create controls
            // Input file button
            HWND hButton1 = CreateWindow(
                "BUTTON", "Select Input File", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 20, 150, 30, hwnd, (HMENU)1, NULL, NULL
            );
            
            // Set button color to light purple
            SetWindowLongPtr(hButton1, GWL_STYLE, GetWindowLongPtr(hButton1, GWL_STYLE) | BS_OWNERDRAW);

            // Output file button
            HWND hButton2 = CreateWindow(
                "BUTTON", "Select Output File", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 60, 150, 30, hwnd, (HMENU)2, NULL, NULL
            );
            
            // Set button color to light purple
            SetWindowLongPtr(hButton2, GWL_STYLE, GetWindowLongPtr(hButton2, GWL_STYLE) | BS_OWNERDRAW);

            // MIDI output file button
            HWND hButton3 = CreateWindow(
                "BUTTON", "Select MIDI Output", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 100, 150, 30, hwnd, (HMENU)3, NULL, NULL
            );
            
            // Set button color to light purple
            SetWindowLongPtr(hButton3, GWL_STYLE, GetWindowLongPtr(hButton3, GWL_STYLE) | BS_OWNERDRAW);

            // Transformation percentage label
            CreateWindow(
                "STATIC", "Transformation %:", WS_VISIBLE | WS_CHILD,
                20, 140, 150, 20, hwnd, NULL, NULL, NULL
            );

            // Transformation percentage trackbar
            HWND hTrackbar = CreateWindow(
                TRACKBAR_CLASS, "", WS_VISIBLE | WS_CHILD | TBS_AUTOTICKS,
                20, 160, 200, 30, hwnd, (HMENU)4, NULL, NULL
            );
            SendMessage(hTrackbar, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
            SendMessage(hTrackbar, TBM_SETPOS, TRUE, 50);

            // Variant selection label
            CreateWindow(
                "STATIC", "Variant Selection:", WS_VISIBLE | WS_CHILD,
                20, 200, 150, 20, hwnd, NULL, NULL, NULL
            );

            // Variant selection combobox
            HWND hComboBox = CreateWindow(
                "COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
                20, 220, 200, 200, hwnd, (HMENU)5, NULL, NULL
            );
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"Random");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"Turn - Whole step above, half step below");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"FTurnF - Half step above, whole step below");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"TT - Trilled regular turn");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"BNT - Between notes turn");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"P32T - P32 regular turn");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"ST - Snapped regular turn");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"FBTurnF - Front/back half step");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"TFBTurnSS - Trilled front/back whole steps");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"P32FBTurnFS - P32 front half, back whole");
            SendMessage(hComboBox, CB_SETCURSEL, 0, 0);

            // Process button
            HWND hButton6 = CreateWindow(
                "BUTTON", "Process File", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 260, 150, 30, hwnd, (HMENU)6, NULL, NULL
            );
            
            // Set button color to light purple
            SetWindowLongPtr(hButton6, GWL_STYLE, GetWindowLongPtr(hButton6, GWL_STYLE) | BS_OWNERDRAW);

            // Generate MIDI button
            HWND hButton7 = CreateWindow(
                "BUTTON", "Generate MIDI", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                180, 260, 150, 30, hwnd, (HMENU)7, NULL, NULL
            );
            
            // Set button color to light purple
            SetWindowLongPtr(hButton7, GWL_STYLE, GetWindowLongPtr(hButton7, GWL_STYLE) | BS_OWNERDRAW);

            // Status text
            CreateWindow(
                "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                20, 300, WINDOW_WIDTH - 40, 250, hwnd, (HMENU)8, NULL, NULL
            );

            // Title label
            HWND hTitle = CreateWindow(
                "STATIC", "Turns Transformation Tool", WS_VISIBLE | WS_CHILD | SS_CENTER,
                200, 20, 400, 30, hwnd, NULL, NULL, NULL
            );
            
            // Set text color to white for better visibility on purple background
            SetWindowLongPtr(hTitle, GWL_STYLE, GetWindowLongPtr(hTitle, GWL_STYLE) | SS_CENTERIMAGE);

          HWND hDesc = CreateWindow(
    "EDIT",
    "Transforms eligible notes (I8, U2R, SPD, SLP, RN, LAD, SAN, RTR2, TNTDN, SMP, LP, TNTLN, and RTD2 with Turns.",
    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
    200, 50, 600, 100,  // Adjust width and height
    hwnd,
    NULL,
    NULL,
    NULL
);
            
            // Set text color to white for better visibility on purple background
            SetWindowLongPtr(hDesc, GWL_STYLE, GetWindowLongPtr(hDesc, GWL_STYLE) | SS_CENTERIMAGE);

            return 0;
        }

        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            
            // Draw custom button with light purple color
            if (dis->CtlType == ODT_BUTTON) {
                RECT rect = dis->rcItem;
                
                // Fill button with light purple
                FillRect(dis->hDC, &rect, CreateSolidBrush(BTN_COLOR));
                
                // Draw button border
                FrameRect(dis->hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
                
                // Get button text
                char buttonText[256];
                GetWindowText(dis->hwndItem, buttonText, sizeof(buttonText));
                
                // Set text color to black for contrast
                SetTextColor(dis->hDC, RGB(0, 0, 0));
                SetBkMode(dis->hDC, TRANSPARENT);
                
                // Center text in button
                DrawText(dis->hDC, buttonText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                return TRUE;
            }
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255)); // White text
            SetBkColor(hdcStatic, BG_COLOR); // Purple scarlet background
            return (LRESULT)CreateSolidBrush(BG_COLOR);
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case 1: { // Select Input File
                    OPENFILENAME ofn = {};
                    char szFile[260] = {0};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileName(&ofn)) {
                        state->inputFile = ofn.lpstrFile;
                        // Update status
                        HWND hStatus = GetDlgItem(hwnd, 8);
                        std::string status = "Input file selected: " + state->inputFile + "\r\n";
                        SetWindowText(hStatus, status.c_str());
                    }
                    break;
                }

                case 2: { // Select Output File
                    OPENFILENAME ofn = {};
                    char szFile[260] = {0};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST;

                    if (GetSaveFileName(&ofn)) {
                        state->outputFile = ofn.lpstrFile;
                        // Update status
                        HWND hStatus = GetDlgItem(hwnd, 8);
                        std::string status = "Output file selected: " + state->outputFile + "\r\n";
                        SetWindowText(hStatus, status.c_str());
                    }
                    break;
                }

                case 3: { // Select MIDI Output File
                    OPENFILENAME ofn = {};
                    char szFile[260] = {0};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "MIDI Files\0*.mid\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST;

                    if (GetSaveFileName(&ofn)) {
                        state->midiOutputFile = ofn.lpstrFile;
                        // Update status
                        HWND hStatus = GetDlgItem(hwnd, 8);
                        std::string status = "MIDI output file selected: " + state->midiOutputFile + "\r\n";
                        SetWindowText(hStatus, status.c_str());
                    }
                    break;
                }

                case 6: { // Process File
                    if (state->inputFile.empty() || state->outputFile.empty()) {
                        MessageBox(hwnd, "Please select input and output files.", "Error", MB_ICONERROR | MB_OK);
                        break;
                    }

                    // Get transformation percentage
                    HWND hTrackbar = GetDlgItem(hwnd, 4);
                    state->transformationPercentage = SendMessage(hTrackbar, TBM_GETPOS, 0, 0);

                    // Get variant selection
                    HWND hComboBox = GetDlgItem(hwnd, 5);
                    int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                    char variantText[100];
                    SendMessage(hComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)variantText);
                    
                    // Clear previous variants
                    state->selectedVariants.clear();
                    
                    // Set selected variant
                    if (selectedIndex == 0) {
                        state->selectedVariants.push_back("RANDOM");
                    } else if (selectedIndex == 1) {
                        state->selectedVariants.push_back("Turn");
                    } else if (selectedIndex == 2) {
                        state->selectedVariants.push_back("FTurnF");
                    } else if (selectedIndex == 3) {
                        state->selectedVariants.push_back("TT");
                    } else if (selectedIndex == 4) {
                        state->selectedVariants.push_back("BNT");
                    } else if (selectedIndex == 5) {
                        state->selectedVariants.push_back("P32T");
                    } else if (selectedIndex == 6) {
                        state->selectedVariants.push_back("ST");
                    } else if (selectedIndex == 7) {
                        state->selectedVariants.push_back("FBTurnF");
                    } else if (selectedIndex == 8) {
                        state->selectedVariants.push_back("TFBTurnSS");
                    } else if (selectedIndex == 9) {
                        state->selectedVariants.push_back("P32FBTurnFS");
                    }

                    // Process the file
                    processFile(state->inputFile, state->outputFile, *state);

                    // Update status
                    HWND hStatus = GetDlgItem(hwnd, 8);
                    SetWindowText(hStatus, state->resultSummary.c_str());
                    break;
                }

                case 7: { // Generate MIDI
                    if (state->outputFile.empty() || state->midiOutputFile.empty()) {
                        MessageBox(hwnd, "Please process a file and select MIDI output file.", "Error", MB_ICONERROR | MB_OK);
                        break;
                    }

                    // Convert to MIDI
                    convertToMidi(state->outputFile, state->midiOutputFile, *state);

                    // Update status
                    HWND hStatus = GetDlgItem(hwnd, 8);
                    std::string currentText;
                    int textLength = GetWindowTextLength(hStatus);
                    if (textLength > 0) {
                        char* buffer = new char[textLength + 1];
                        GetWindowText(hStatus, buffer, textLength + 1);
                        currentText = buffer;
                        delete[] buffer;
                    }
                    currentText += "\n" + state->statusMessage;
                    SetWindowText(hStatus, currentText.c_str());
                    break;
                }
            }
            break;
        }

        case WM_HSCROLL: {
            // Handle trackbar changes
            HWND hTrackbar = GetDlgItem(hwnd, 4);
            if ((HWND)lParam == hTrackbar) {
                int pos = SendMessage(hTrackbar, TBM_GETPOS, 0, 0);
                state->transformationPercentage = pos;
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#elif defined(PLATFORM_LINUX)
// Linux GUI implementation using X11

int main(int argc, char* argv[]) {
    // Check if we're running in command-line mode
    if (argc >= 3) {
        // Command-line mode
        AppState state;
        state.inputFile = argv[1];
        state.outputFile = argv[2];
        
        if (argc > 3) {
            state.midiOutputFile = argv[3];
        }
        
        if (argc > 4) {
            state.transformationPercentage = std::stod(argv[4]);
        }
        
        if (argc > 5) {
            state.selectedVariants.push_back(argv[5]);
        } else {
            state.selectedVariants.push_back("RANDOM");
        }
        
        // Process the file
        processFile(state.inputFile, state.outputFile, state);
        std::cout << state.statusMessage << std::endl;
        
        // Generate MIDI if output file is specified
        if (!state.midiOutputFile.empty()) {
            convertToMidi(state.outputFile, state.midiOutputFile, state);
            std::cout << state.statusMessage << std::endl;
        }
        
        return 0;
    }

    // GUI mode
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cerr << "Cannot open display" << std::endl;
        return 1;
    }

    // Get screen
    int screen = DefaultScreen(display);
    
    // Create window with purple scarlet background
    XColor purple_scarlet;
    Colormap colormap = DefaultColormap(display, screen);
    XParseColor(display, colormap, "#8E2D57", &purple_scarlet); // Purple scarlet color
    XAllocColor(display, colormap, &purple_scarlet);
    
    Window window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, 1,
        BlackPixel(display, screen), purple_scarlet.pixel
    );
    
    // Set window title
    XStoreName(display, window, WINDOW_TITLE);
    
    // Select window events
    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
    
    // Create GC for drawing
    GC gc = XCreateGC(display, window, 0, NULL);
    
    // Map window to display
    XMapWindow(display, window);
    
    // Create application state
    AppState state;
    
    // Event loop
    XEvent event;
    bool running = true;
    
    while (running) {
        XNextEvent(display, &event);
        
        switch (event.type) {
            case Expose: {
                // Draw UI elements
                XDrawString(display, window, gc, 20, 30, "Input File:", 11);
                XDrawString(display, window, gc, 20, 70, "Output File:", 12);
                XDrawString(display, window, gc, 20, 110, "MIDI Output:", 12);
                XDrawString(display, window, gc, 20, 150, "Transformation %:", 17);
                XDrawString(display, window, gc, 20, 210, "Variant Selection:", 18);
                XDrawString(display, window, gc, 20, 270, "Status:", 7);
                
                // Draw title
                XDrawString(display, window, gc, 200, 30, "Turns Transformation Tool", 25);
                XDrawString(display, window, gc, 200, 50, "Transforms eligible notes with turn variants", 42);
                
                // Create light purple color for buttons
                XColor light_purple;
                XParseColor(display, colormap, "#B4A0C8", &light_purple); // Light purple color
                XAllocColor(display, colormap, &light_purple);
                
                // Save original foreground color
                unsigned long original_fg = XGetForeground(display, gc);
                
                // Set light purple color for button backgrounds
                XSetForeground(display, gc, light_purple.pixel);
                
                // Draw buttons with light purple background
                XFillRectangle(display, window, gc, 150, 20, 150, 30);
                XFillRectangle(display, window, gc, 150, 60, 150, 30);
                XFillRectangle(display, window, gc, 150, 100, 150, 30);
                
                // Reset to white for text
                XSetForeground(display, gc, WhitePixel(display, screen));
                
                // Draw button text
                XDrawString(display, window, gc, 170, 40, "Select Input File", 16);
                XDrawString(display, window, gc, 170, 80, "Select Output File", 17);
                XDrawString(display, window, gc, 170, 120, "Select MIDI Output", 17);
                
                // Draw button borders
                XDrawRectangle(display, window, gc, 150, 20, 150, 30);
                XDrawRectangle(display, window, gc, 150, 60, 150, 30);
                XDrawRectangle(display, window, gc, 150, 100, 150, 30);
                
                // Draw slider with light purple background
                XSetForeground(display, gc, light_purple.pixel);
                XFillRectangle(display, window, gc, 150, 140, 200, 20);
                
                // Reset to white for border
                XSetForeground(display, gc, WhitePixel(display, screen));
                XDrawRectangle(display, window, gc, 150, 140, 200, 20);
                
                // Draw slider handle with darker purple
                XColor dark_purple;
                XParseColor(display, colormap, "#6A4C93", &dark_purple);
                XAllocColor(display, colormap, &dark_purple);
                XSetForeground(display, gc, dark_purple.pixel);
                
                int sliderPos = 150 + (state.transformationPercentage * 2);
                XFillRectangle(display, window, gc, sliderPos, 140, 10, 20);
                
                // Draw variant selection with light purple background
                XSetForeground(display, gc, light_purple.pixel);
                XFillRectangle(display, window, gc, 150, 200, 200, 30);
                
                // Reset to white for text and border
                XSetForeground(display, gc, WhitePixel(display, screen));
                XDrawRectangle(display, window, gc, 150, 200, 200, 30);
                XDrawString(display, window, gc, 170, 220, "Random", 6);
                
                // Draw process buttons with light purple background
                XSetForeground(display, gc, light_purple.pixel);
                XFillRectangle(display, window, gc, 20, 300, 150, 30);
                XFillRectangle(display, window, gc, 180, 300, 150, 30);
                
                // Reset to white for text
                XSetForeground(display, gc, WhitePixel(display, screen));
                
                // Draw button text
                XDrawString(display, window, gc, 60, 320, "Process File", 12);
                XDrawString(display, window, gc, 210, 320, "Generate MIDI", 13);
                
                // Draw button borders
                XDrawRectangle(display, window, gc, 20, 300, 150, 30);
                XDrawRectangle(display, window, gc, 180, 300, 150, 30);
                
                // Draw status area
                XDrawRectangle(display, window, gc, 20, 340, WINDOW_WIDTH - 40, 240);
                if (!state.statusMessage.empty()) {
                    XDrawString(display, window, gc, 30, 360, state.statusMessage.c_str(), state.statusMessage.length());
                }
                break;
            }
                
            case ButtonPress: {
                // Handle button clicks
                int x = event.xbutton.x;
                int y = event.xbutton.y;
                
                // Input file button
                if (x >= 150 && x <= 300 && y >= 20 && y <= 50) {
                    // Open file dialog (simplified)
                    state.inputFile = "/tmp/input.txt";
                    state.statusMessage = "Input file selected: " + state.inputFile;
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // Output file button
                else if (x >= 150 && x <= 300 && y >= 60 && y <= 90) {
                    // Open file dialog (simplified)
                    state.outputFile = "/tmp/output.txt";
                    state.statusMessage = "Output file selected: " + state.outputFile;
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // MIDI output file button
                else if (x >= 150 && x <= 300 && y >= 100 && y <= 130) {
                    // Open file dialog (simplified)
                    state.midiOutputFile = "/tmp/output.mid";
                    state.statusMessage = "MIDI output file selected: " + state.midiOutputFile;
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // Slider
                else if (x >= 150 && x <= 350 && y >= 140 && y <= 160) {
                    state.transformationPercentage = (x - 150) / 2;
                    if (state.transformationPercentage < 0) state.transformationPercentage = 0;
                    if (state.transformationPercentage > 100) state.transformationPercentage = 100;
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // Variant selection
                else if (x >= 150 && x <= 350 && y >= 200 && y <= 230) {
                    // Toggle through variants (simplified)
                    state.selectedVariants.clear();
                    state.selectedVariants.push_back("RANDOM");
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // Process file button
                else if (x >= 20 && x <= 170 && y >= 300 && y <= 330) {
                    if (state.inputFile.empty() || state.outputFile.empty()) {
                        state.statusMessage = "Error: Please select input and output files.";
                    } else {
                        processFile(state.inputFile, state.outputFile, state);
                    }
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                
                // Generate MIDI button
                else if (x >= 180 && x <= 330 && y >= 300 && y <= 330) {
                    if (state.outputFile.empty() || state.midiOutputFile.empty()) {
                        state.statusMessage = "Error: Please process a file and select MIDI output file.";
                    } else {
                        convertToMidi(state.outputFile, state.midiOutputFile, state);
                    }
                    XClearArea(display, window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, True);
                }
                break;
            }
                
            case KeyPress: {
                // Handle key press (ESC to quit)
                if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
                    running = false;
                }
                break;
            }
        }
    }
    
    // Clean up
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    
    return 0;
}
#endif

// Standard entry point for command-line usage
#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file> [midi_output_file] [transformation_percentage] [variant]" << std::endl;
        std::cout << "Example: " << argv[0] << " input.txt output.txt output.mid 50 RANDOM" << std::endl;
        return 1;
    }

    AppState state;
    state.inputFile = argv[1];
    state.outputFile = argv[2];
    
    if (argc > 3) {
        state.midiOutputFile = argv[3];
    }
    
    if (argc > 4) {
        state.transformationPercentage = std::stod(argv[4]);
    }
    
    if (argc > 5) {
        state.selectedVariants.push_back(argv[5]);
    } else {
        state.selectedVariants.push_back("RANDOM");
    }
    
    // Process the file
    processFile(state.inputFile, state.outputFile, state);
    std::cout << state.statusMessage << std::endl;
    
    // Generate MIDI if output file is specified
    if (!state.midiOutputFile.empty()) {
        convertToMidi(state.outputFile, state.midiOutputFile, state);
        std::cout << state.statusMessage << std::endl;
    }
    
    return 0;
}
#endif