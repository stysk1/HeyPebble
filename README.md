# Hey Pebble
Hack TCNJ created project
Apple has Siri, Google has OK Google, Microsoft has Cortana and now Pebble has Hey Pebble!

Introducing Hey Pebble! Launch the app (preferably from quick launch) and press the middle button for dictation to start. Start talking into the app and receive an answer instantly!

The companion app currently contains the search bar that performs a google search and takes the snippet information from the first appearing Wikipedia article.

For testing on an emulator, using the following with the commandline-tool (https://developer.pebble.com/guides/publishing-tools/pebble-tool):

[LINUX]

	// Install the compiled app .pbw file
	pebble install --emulator basalt <path to HeyPebble.pbw>

	// After the app comes up, run a transcription service that will return a static string when the emulator uses dictation
	pebble transcribe "Enter some text here"

	// After the app starts up, type the following to view the live pebble logs from the emulator.
	pebble logs
