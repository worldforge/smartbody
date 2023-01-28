CerevoiceRelay can be started with two possible parameters:

-host <server_name>, for running CerevoiceRelay in client mode,  
 connecting to <server_name>. This mode has currently not been
 implemented
-voice <voice_id>, for loading voice <voice_id>. Multiple 
 '-voice <voice_id>' pairs can be given. 

Example: cerevoicerelay -voice cerevoice_2.0.0_katherine -voice
         cerevoice_2.0.0_heather

The voice ID is the name shared by the voice and license files. 
For instance, with the files 'cerevoice_2.0.0_katherine_00009.lic' 
and 'cerevoice_2.0.0_katherine_00009.voice' the voice ID is
'cerevoice_2.0.0_katherine_00009'. CerevoiceRelay assumes the file 
extensions to be '.lic' and '.voice' respectively. These files need 
to be present in the relative directory "..\..\data\cereproc\voices", 
(which should correspond to "<PROJECT_ROOT>\<VHUMAN>\data\cereproc\voices"). 
These files are not on the SVN and must be copied to the correct location 
by the user.

CerevoiceRelay also requires:

 * "../../lib/cerevoice/veng_db/en/norm/abb.txt"
 * "../../lib/cerevoice/veng_db/en/norm/pbreak.txt"
 * "../../lib/cerevoice/veng_db/en/homographs/rules.dat";
 * "../../lib/cerevoice/veng_db/en/reduction/rules.dat"
 * "../../lib/cerevoice/veng_db/en/gb/norm/rules.py"

These files are located on the SVN.

The generated audio files are saved in the relative path 
"..\..\..\dimr\tmpaudio" (which should correspond to 
"<PROJECT_ROOT>\dimr\tmpaudio"). Be aware that the module that 
plays the audio files might need a different relative path. The path
that CerevoiceRelay sends out, therefore, is "dimr\tmpaudio".

CerevoiceRelay uses the file name given to it by SmartBody, with the 
exception that it replaces the ".aiff" extension with ".wav" since it 
is outputting a .wav audio file.

If cerevoice receives a TTS request during runtime with a voice that has
not been loaded, it will ignore the request. This allows multiple voice
relays to run simulteanously. 
