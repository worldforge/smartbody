Phoneme Scheduler
-----------------

To make a SmartBody character speak using recorded voice (via a .wav file), it is necessary to create a schedule of phonemes (individual sounds that comprise a spokem word) and the timings of each phoneme that can then be processed by SmartBody, and in turn, lip synched accurately by the character.

SmartBody uses an XML format that contains a list of words, their timings, and their phonemes. To use such a file:

1) Make sure the character uses 'audiofile' for its voice:

mycharacter = scene.getCharacter("Brad")
mycharacter.setStringAttribute("voice", "audiofile")

2) Set the main path to find the sound files (relative paths will prepend the media path):

scene.addAssetPath("audio", "sounds")

3) Set the specific directory for your character. If all the sound files are in the same directory, a '.' can be used:

mycharacter.setStringAttribute("voiceCode", ".")

4) Place the .wav file, .bml file (containing the phoneme and word schedule) into the directory 

5) Play the sound via BML. For example, if the .wav file is called 'introduction.wav':

bml.execBML("Brad", "<speech ref="introduction"/>



To create the phoneme schedule, we are leveraging the phoneme alignment code that is in the CSLU Toolkit (http://www.cslu.ogi.edu/toolkit/) which needs to be downloaded separately due to their licenese.

----------------------------------------------------------------------
Instructions for how to use the phoneme alignment tools are below:
----------------------------------------------------------------------


/fa and /Tcl80 folders are not included by default due to license. Please download cslu toolkit at http://www.cslu.ogi.edu/toolkit/. 
Copy /fa from CSLU_ROOT/Toolkit/2.0/apps/fa and /Tcl80 from CSLU_ROOT/Tcl80.

-------------------------------------------------File Layout--------------------------------------------------------------------

/fa:		script for generating phonemes and doing force alignment. (two scripts are used)
		gen_pronun.tcl: generate list of phonemes in .ntp file, parameters: <.txt> <.ntp> [-dict curstom_dictionary.txt]
		fa_new_multichan.tcl: generate phoneme time schedules in .txt1 file, parameters: <.wave> <.ntp> <.txt1>. Note that wav file has to be mono track.

/files:		put .wav, .txt files here they will be used to generate .txt1 files.
		converter will automatically look for all .txt1 files in this folder and generate .bml files.


/Tcl80:		where tcl80sh.exe sits

mapping.txt:	where you can find the mapping from worldbet symbols to timit symbols to diphone phonemes used for converter. It has to be a four column text file. Note that the current SmartBody scheme converts the 40 English phonemes to a reduced set of 21 (shown in the 3rd column). This can be modified as needed to eliminate or change that mapping.

auto.sh:	automatic script that generate .txt1 files given .wav and .txt files under "files" folder and convert .txt1 to .bml files


Usage example:

auto.sh 



Some useful links:
http://web.mit.edu/course/6/6.863/share/nltk_lite/timit/
http://www.cslu.ogi.edu/toolkit/
http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2682710/