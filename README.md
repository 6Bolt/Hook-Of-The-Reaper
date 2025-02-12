# Hook-Of-The-Reaper
![HookOfTheReaperLogo330](https://github.com/user-attachments/assets/ff4479bc-adf3-4b0f-8dfb-0a8755b2ad6b)

# The Goals for Hook of the Reaper

#### 1) To do everything that MAMEHooker can do with Serial Port Devices. Starting with light guns, then IO Controllers, and finally force feedback. Able to load INI Files
#### 2) To implement the new Default Light Gun format. This will make any default light gun work with the associated game file, like the INI files.
#### 3) To be open source with the GPLv3 license, so it will remain always open source.
#### 4) To be platform independent. Reason why I chose Qt, as it can compile on many platforms.
#### 5) Have the best performance, and be multi-threaded.



I started this project after getting two Retro Shooter RS3 Reaper light guns as a Christmas present from my daughter in 2024. Never had light guns before, so was a learning experience with the light guns, Tekno-Parrot, MAME, Demulshooter, and MAMEHooker. The only problem I had was with MAMEHooker. It would work for one game, and then freeze up. I tried multiple Windows comparability modes, but it never worked right. I tried some things from the internet, but I never got it to work consistently. I am not blaming MAMEHooker and/or its coder. It is a great program, and many people through out the years have use this program. It was written a while ago and a lot of older programs have problems working with newer Windows OSes. Hence why they have  comparability modes. Seeing a project, I started coding and decided to name the new program, Hook Of The Reaper. Which takes Hook from MAMEHooker, and Reaper from the RS3 Reaper light guns, which are awesome. The picture of the cross hair being ‘hooked’ by a reaper scythe, came into my head for the program icon, and truly represents the program.


# Getting the Program to Work

Since the project is in Beta, only Windows is supported right now. I plan on getting Linux and Mac OSes later on. A person could take the code, and compile in the needed OS.

There is one major change that needs to be done to get Hook Of The Reaper working with MAME and DemulShooter. The ‘network’ has to be enabled on MAME and DemulShooter. MAMEHooker used the window messaging to receive data. This is a Windows type thing, as Linux and MacOS (think it is based on Debian) don’t have this window message type. They might have something that is close to it. But to meet goal #4, I needed a solution that would easily work on all platforms. The ‘network’ was the best options. Instead of using window messaging, it uses TCP Socket data on the localhost (IP 127.0.0.1) on port 8000. TCP Socket has major support on all platforms.

### Enable Network on MAME

Open the mame.ini file, in the main MAME directory. Should be right under or above the mame(64).exe. If it is not there, you might have to start MAME for the first time and run a game. Once you have mame.ini open, go down or search for “OSD OUTPUT OPTIONS.” Below that, you will see “ouput” on the left, and a setting on the right, which might be “windows” if you used MAMEHooker. The “output” needs to be set to “network”. So erase any existing option, like “windows” and put in “network”. Then save the file and you are done.


### Enable Network on DemulShooter

This is a lot easier then MAME. Open up the DemulShooter GUI (DemulShooter_GUI.exe). Pull down the “Page selection:” combo box at the top, and then select “Output” at the bottom. Then check the top box, which is labeled “Enable Outputs”. Then check the third box, which is labeled, “Network Outputs”. Then click the button at the bottom, labeled “Save Config”. That is it, Hook Of The Reaper will now be able to connect to MAME and DemulShooter. 



# Game Files

## INI Game Files

Now you just need to have game files to tell Hook Of The Reaper what signals to watch out for. You can use the MAMEHooker INI gamefiles. Hook Of The Reaper can load these files and process the data for light guns only. I have not set up any I/O controllers, LED controllers, or force feedback. The INI game files can be put in the directory below. Alos the characters ‘%s%’ and ‘|’ are supported in Hook Of the Reaper

### Location of INI Game Files

- path/to/HookOfTheReaper/ini/MAME


## Default Light Gun Game Files

I am working on a new game file format for light guns, I call it Default Light Gun games file. It will take away a lot of the complexity of writing a game file. Lets look how MAMEHooker does damage for 3 Light Guns, one RS3 Reaper and 2 made up. Also how Hook Of The Reaper could do it with the new format.

| Light Gun | MAMEHooker INI | HOTR Default LG |
|-----------|----------------|-----------------|
| RS3 Reaper | P1_Damage=cmr 4 ZZ |:P1_Damage *P1 >Damage |
| Default Gun #2 | P1_Damage=cmr 4 M0(Z)%s% | :P1_Damage *P1 >Damage |
| Default Gun #3 | P1_Damage=cmr 4 TTXER#7 | :P1_Damage *P1 >Damage |


 As you can see, the Hook Of The Reaper, didn’t change. The program will load up the command, based on the Default Light Gun being used. Then the same game files can be used for every light gun. So Default Light Gun game files can be slowly built, and used by everyone. Instead of everyone making custom INI game files. 

### Commands

Light guns only have so many commands, as they can recoil, shake, and few other things. Here is the List of commands I have so far:

### Current Default Light Gun Commands

| Command | Notes |
|---------|-------|
| Damage | (only happens on 0->1, no need for '\|') |
| Recoil | (only happens on 0->1, no need for '\|') |
| Reload |     |
| Ammo   |     |
| Ammo_Value|      |
| Shake | (only happens on 0->1, no need for '\|') |
| Auto_LED |     |
| AspectRatio_16:9 |    |
| AspectRatio_4:3 |      |
| Joystick_Mode |      |
| Keyboard_Mouse_Mode |     |
| Null |      |




But, what if you are playing a new game, and don’t have the signals. Hook Of The Reaper works like MAMEHooker does. If you choose Default Light Guns files first in Settings, and there is no game file of that game name for defaultLG or INI, Hook Of The Reaper will make a new game file in the defaultLG directory, with the name of the game dot txt. The top will have a standard top, and then list out all the signals observed during the time the game was open. So there must be no game file for both INI and Default Light Gun. 

If the setting is not set for use Default Light Gun files first, it will make a new game file in the ini/MAME directory, with the name of the game dot ini. The top will be a standard INI file top, and then list out all the signals observed during the time the game was open. Also, if any new signals pop up later, that are not in the file, it will be added to the file, when the game closes, like MAMEHooker, does. 

### Location of Default Light Gun Game Files

- path/to/HookOfTheReaper/defaultLG

I put in three games in the defaultLG directory, so a person can see how it is set up. Unlike above, every thing has its own line. The Damage example would be like this in the file.

| File Line | What it Represents |
|-----------|--------------------|
| :P1_Damage | A Signal with a beginning : |
| *P1 | A Player or ALL, with a beginning * |
| >Damage | A Command, with a beginning >


A signal can have no Player/All and no Command, or a Player/All and Command. Also Multiple Players and Commands can be after a Signal.

- :P1_Damage
- *P1
- \>Damage
- *P3
- \>Shake

Also, multiple commands can be used, and be selected based on the Signal’s current value. It is implemented the same way MAMEHooker does, with the ‘|’. So If you had:

- :P1_Damage
- *P1
- \>Null|Shake|Recoil|Damage	&emsp;	So When P1_Damage=0 → Null, 1 -> Shake, 2 → Recoil, 3 → Damage

The 0 is the left most Command, then increase by one, going to the right. The same way MAMEHooker has it. Also, the INI files can use the %s% variable. The %s% is replaced by the Signal’s value. It is not used in the Default Light gun files, but want to mention that it is supported.  

# Player Ordering in the Default Light Gun Game Format

At the top of the file, it will start with 'Players' and then the next line is how many players needed for the game, which can be 1 to 4 currently. The next lines will tell Hook Of The Reaper the player ordering. If there are 2 playeres, then there will be P1 and P2. If 4 players, then P1, P2, P3, and P4. But you can put in different players. For example, there is 2 players, and you put in P3 and P4. What will happen is P3 will map onto P1, and P4 will map onto P2. I did it this way, so you could you different light guns in Player's Assignment. So for a certain game, you would like to use a different light guns that are assigned to P3 and P4. then you could change the game file to use players P3 and P4, instead of P1 and P2. Then you didn't have to change the Player's Assignment all the time, if you wanted to use a different light guns for certain games. Below is an example of this.

| Default Light Gun Game File | Notes |
|-----------------------------|-------|
| Players |  |
| 2 | Number of Players |
| P3 | P3 -> P1 in this file  |
| P4   | P4 -> P2 in this File  |
| [States] |      |
| :mame_start |  |
| *All | P3 & P4  |
| >Open_COM | Open COM Ports for P3 & P4 Light Guns |
| [Signals] |      |
| :P1_Damaged |      |
| *P1 | Which is mapped to P3  |
| >Damage | Send Damage CMDs to P3's Light Gun |
| :P2_Ammo |        |
| *P2 | Which is mapped to P4   |
| >Ammo_Value | Send Ammo_Value to P4's Light Gun |


# Current Default Light Gun Files Included

| Directoury | File Name | Game Name |
|------------|-----------|-----------|
| defaultLG/ |     |     |
|     | alien3.txt | Alien 3 |
|     | confmiss.txt | Confidential Mission |
|     | hotd4.txt | House of the Gead 4 |
|     | jp.txt | Jurassic Park |
|     | lgi.txt | Let's Go Island |
|     | lgi3d.txt | Let's Go Island 3D |
|     | lgj.txt | Let's Go Jungle |
|     | ptblank2.txt | Point Blank 2 |
|     | rambo.txt | Rambo   |


# How to Use Hook Of The Reaper 

### Add a Light Gun:

Select ‘File’ in the menu at the top, and to the left. Select “Add Light Gun” and a new small child window will pop up. The first combo box has the supported default light guns, which currently is only RS3 Reaper. When you select a default light gun, it will lock in certain combo boxes, which is the correct settings for that light gun. Then a name is needed in the name box. Also the correct COM port needs to be selected. If the light guns are connected and powered on, the COM Port info will show up on the right side. You can toggle through COM port number, until the correct data shows up on the right. Then you can click the “Add” button or the “OK” button. The “Add" button will add and save the light gun. The “OK button will do the same as the “Add” button, but will close the window after saving. If any changes need to be made after the light gun has been saved, then select “Edit” in the top menu, and then select “Edit Light Gun.” This will bring up a similar child window, but called “Edit Light Gun.” You will be able to edit or delete the light gun. There is also windows for adding a COM Device, but nothing can be done with it yet.


### Player’s Assignment:

After the light guns have been added to Hook Of The Reaper, they need to be assigned to a player number. Click the “File” in the top menu, and then select the “Player’s Assignment.” Which will bring up the Player’s Assignment child window. Currently, there is only four players. Next to each Player, is a combo box loaded with all the saved light guns. Then the light guns can be assigned to a player. Once they have been assigned, the “Assign” or “OK” button can be pressed to save the assignment. 



### Settings:

##### Currently there are only three settings. The first is to use the Default Light Gun game files before the INI game files. There is a check box to the left, that can be checked or un-checked. But if it is set, and you load a game that doesn’t have a Default Light Gun game file, but there is an INI game file, it will load the INI game file. If both were missing it will make new Default Light Gun game file with all the signals. If it is not set, and you load a game that doesn’t have a INI game file, but there is a Default Light Gun game file, it will load the Default Light Gun game file. If both were missing it will make new INI game file with all the signals.  

##### The second option is to disable the multi-threading. Hook Of The Reaper uses multi-threading to boost performance. Currently, it uses 3 threads. One main thread, a 2nd thread for the TCP Socket, and a 3rd for the Serial COM Port. If Multi-threading is disabled, then the GUI will work horribly, if it is waiting to connect to to a TCP Socket. It is highly recommended not to disable multi-threading, unless it is needed. This is why I wanted to use multi-threading, so waiting for the TCP Connection, doesn’t affect the GUI performance. A re-start of the program is needed.

##### The final option is the refresh time for the output signals in msecs. So that performance will not suffer from updating the signal values on the main window, I put the updating on a timer. So smaller the time setting, the faster it will refresh the data, and vice versa. So if you don’t see any signal values changing, then the refresh display time needs to be a smaller amount. But smaller the number, will make the performance less and less. So it is a trade off.  



### Hooker Engine

The Hooker Engine, which does all the magic, will start right away. Also, when any child windows is open, the Hooker Engine will stop working. Once the child window is closed, the engine will re-start. Just make sure that you don’t open a child window when playing a game.    



### State of the Program

The program is currently in Beta (0.8). Right now I plan on adding in more default light guns. Also add in more Default Light Gun game files. Once that is done, I will move onto I/O Devices and LED controllers. 

If you find any bugs, please report them here. 



# Compile the Code

Everything is in Qt, so you only need Qt and the MSVC 2022 tools. Below is what I used in Qt to compile the program.

- Qt 6.8 with a CMake File with MSVC 2022
- My CMake Version: 3.29.3
