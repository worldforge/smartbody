REM Word Breaker
REM Read lines in tab delimited text file, send all lines to TTS module, 
REM and write results as BML in individual files.
REM
REM Parameters:
REM -input <file>, tab delimited text file with utterance id and utterance text
REM -output <dir>, directory where files will be saved 

word_breaker.exe -input ..\..\data\Sounds\lines.txt -output ..\..\data\Sounds
