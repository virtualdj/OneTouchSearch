# OneTouchSearch
A Windows tray icon program that emulates the *One Touch Search* feature of the old Logitech mice with SetPoint.
When it is running and the user presses the default hotkey <kbd>CTRL</kbd>+<kbd>ALT</kbd>+<kbd>SHIFT</kbd>+<kbd>K</kbd> the currently selected text (in any application) is used to launch a search with the default browser.

## Disclaimer
It's my first C++ project so I don't practice this language very well (actually, "at all"!). Feel free to submit PRs with code corrections and new features, if you like, because at the moment this is the maximum I can do. :smiley:

## Usage
1. Either compile the source code with VS2010 or run the binary available on the **Release** section: an icon will be placed on the system tray.
2. Important note: due to UIPI (User Interface Privilege Isolation), only if you run the application as Administrator it will be able to grab text from both admin and non-admin apps, so be sure to try both modes. Consider that the original One Touch Search from Logitech cannot grab text from admin apps, too.
3. Select some text on a Windows application, such as Notepad or Word, etc..
4. Press <kbd>CTRL</kbd>+<kbd>ALT</kbd>+<kbd>SHIFT</kbd>+<kbd>K</kbd> on your keyboard, a new browser tab (or instance) should search the highlighted text using Google US.
5. (Optional) Assign the hotkey to one of the mouse buttons using Logitech Options, so you can search exactly as the old Logitech SetPoint feature with the same name.

## Advanced configuration
As you may guess from the sources, I'm not able to create a UI for the configuration like for the Logitech's OneTouchSearch. But you can change the *Search Engine* and *Hotkey* from the registry, if you like.

### How to change the Search Engine used
Open the registry key:

`HKEY_CURRENT_USER\Software\OneTouchSearch`

and change the value `SearchEngineURL` with the URL of your choice. Some examples:
* Google Search in Italian
  `https://www.google.it/search?hl=it&q=`
* Duck Duck Go in English
  `https://duckduckgo.com/?q=`

### How to change the hotkey
This is more difficult, as it requires a DWORD value where the lower word is the key and the higher word is the modifier (CTRL, ALT, WIN, etc.). You'll have open the registry key:

`HKEY_CURRENT_USER\Software\OneTouchSearch`

and change the value `Hotkey` with the number that represents the combination of the two (LOWORD = key, HIWORD = modifier). For example this is how the default hotkey is converted to a number:
1. The **modifier** are <kbd>CTRL</kbd>+<kbd>ALT</kbd>+<kbd>SHIFT</kbd> so consult [this table](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-hotkey#parameters) for the related C++ Win32 constants:
  * <kbd>CTRL</kbd> = 0x0002
  * <kbd>ALT</kbd> = 0x0001
  * <kbd>SHIFT</kbd> = 0x0004
2. Open the Windows Calculator in **programmer** mode and click on **HEX** to input and output **hexadecimal** values.
3. Sum the values of the chosen modifiers, so `2 + 1 + 4 = 7` and multiply this value by `10000` (hex), you'll get `70000`.
4. The chosen **key** is <kbd>K</kbd> so consult [this table](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) for the related C++ Win32 constants:
  * <kbd>K</kbd> = 0x4B
5. Add the key to the previous result (always in HEX mode), so you'll get `70000 + 4B = 7004B`.
6. `7004B` is the value you'll have to put in **hexadecimal mode** into the registry value `Hotkey`.

## Used libraries/code
This is a mash-up of different libraries/code samples found on CodeProject:
* **Clipboard Backup** by sswater shi ([link](https://www.codeproject.com/Articles/11372/Clipboard-backup-Visual-C))
* **URL Encode** by Daniel Cohen Gindi ([link](https://www.codeproject.com/Articles/19629/How-to-Encode-Decode-URLs-to-the-UTF8-Format-with))
* **Tray Icon** by Istvan Pasztor ([link](https://www.codeproject.com/Articles/127791/A-simple-and-easy-to-use-trayicon))
