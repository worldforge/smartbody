
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#if (!__MonoCS__)
using System.Speech.Synthesis;
#endif
using System.Xml;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using System.Runtime.InteropServices;

#if (!__MonoCS__)
namespace TtsRelay
{
    public class MsSpeechRelay : ITtsRelay
    {
        /// <summary>
        /// SAPI TTS server
        /// </summary>
        private SpeechSynthesizer ttsServer;

        /// <summary>
        /// Maps SAPI visemes to SmartBody phonemes in order to generate viseme timings
        /// </summary>
        private List<string> visemeIDMap;

        /// <summary>
        /// Variable to enable/disable debug prints
        /// </summary>
        private bool doDebugChecks = true;

        /// <summary>
        /// Keeps track of the total duration of the phonemes and visemes so far encountered in a single request.
        /// Used instead of provided AudioPosition starting points, since they are way off.
        /// </summary>
        private double totalVisemeDuration = 0;
        private double totalPhonemeDuration = 0;

        private GenerateAudioReply generateAudioReply = null;
        private KeyValuePair<double, double> wordMarker;
        private bool markerExists = false;


        XmlDocument facefxMappingDoc = null;

        string visemeMappingType = "facefx";


        const string facefxMapping = @"<mapping>
      <entry viseme='0' target='open' amount='0.000000' />

      <entry viseme='1' target='open' amount='0.500000' />
      <entry viseme='1' target='wide' amount='0.600000' />
      <entry viseme='1' target='tBack' amount='0.400000' />

      <entry viseme='2' target='open' amount='0.550000' />

      <entry viseme='3' target='open' amount='0.400000' />
      <entry viseme='3' target='W' amount='0.550000' />

      <entry viseme='4' target='open' amount='0.500000' />
      <entry viseme='4' target='wide' amount='0.600000' />
      <entry viseme='4' target='tBack' amount='0.400000' />

      <entry viseme='5' target='open' amount='0.400000' />
      <entry viseme='5' target='ShCh' amount='0.500000' />
      <entry viseme='5' target='tRoof' amount='0.500000' />

      <entry viseme='6' target='W' amount='0.500000' />
      <entry viseme='6' target='ShCh' amount='0.300000' />
      <entry viseme='6' target='tRoof' amount='0.400000' />

      <entry viseme='7' target='open' amount='0.400000' />
      <entry viseme='7' target='W' amount='0.8500000' />

      <entry viseme='8' target='open' amount='0.400000' />
      <entry viseme='8' target='W' amount='0.550000' />

      <entry viseme='9' target='open' amount='0.500000' />
      <entry viseme='9' target='wide' amount='0.600000' />
      <entry viseme='9' target='tBack' amount='0.400000' />

      <entry viseme='10' target='open' amount='0.400000' />
      <entry viseme='10' target='W' amount='0.550000' />

      <entry viseme='11' target='open' amount='0.500000' />
      <entry viseme='11' target='wide' amount='0.600000' />
      <entry viseme='11' target='tBack' amount='0.400000' />

      <entry viseme='12' target='open' amount='0.200000' />

      <entry viseme='13' target='open' amount='0.100000' />
      <entry viseme='13' target='W' amount='0.70000' />

      <entry viseme='14' target='open' amount='0.400000' />
      <entry viseme='14' target='tRoof' amount='0.80000' />

      <entry viseme='15' target='open' amount='0.1500000' />
      <entry viseme='15' target='wide' amount='0.500000' />
      <entry viseme='15' target='tRoof' amount='0.400000' />

      <entry viseme='16' target='ShCh' amount='0.850000' />
      <entry viseme='16' target='tRoof' amount='0.400000' />

      <entry viseme='17' target='open' amount='0.450000' />
      <entry viseme='17' target='tTeeth' amount='0.900000' />

      <entry viseme='18' target='FV' amount='0.750000' />

      <entry viseme='19' target='open' amount='0.400000' />
      <entry viseme='19' target='tRoof' amount='0.800000' />

      <entry viseme='20' target='open' amount='0.2500000' />
      <entry viseme='20' target='tBack' amount='0.800000' />
      <entry viseme='19' target='tRoof' amount='0.800000' />

      <entry viseme='21' target='PBM' amount='0.900000' />
   </mapping>";






        public bool Init(string visemeMapping)
        {
            /// Initialize phoneme to viseme mappings
            SetVisemeMapping(visemeMapping);

            ttsServer = new SpeechSynthesizer();

            Console.WriteLine("\nAvailable SAPI compatible voices on this machine are:\n");
            foreach (InstalledVoice v in ttsServer.GetInstalledVoices())
            {
                Console.WriteLine("\"" + v.VoiceInfo.Name + "\"");
            }
            Console.WriteLine("\n");

            /// Add message callbacks to receive TTS events
            ttsServer.BookmarkReached += new EventHandler<BookmarkReachedEventArgs>(ttsServer_BookmarkReached);
            ttsServer.PhonemeReached += new EventHandler<PhonemeReachedEventArgs>(ttsServer_PhonemeReached);
            ttsServer.VisemeReached += new EventHandler<VisemeReachedEventArgs>(ttsServer_VisemeReached);

            return true;
        }


        public string [] GetVoices()
        {
            //"Microsoft Anna"
            //"Microsoft Mary"
            //"Microsoft Mike"

            List<string> list = new List<string>();

            foreach (InstalledVoice v in ttsServer.GetInstalledVoices())
            {
                list.Add(v.VoiceInfo.Name.Replace(" ", "|"));
            }

            return list.ToArray();
        }

        /// <summary>
        /// Get culture of selected voice
        /// </summary>
        /// <param name="strVoiceName">name of the voice chosen</param>
        /// <returns>culture name for xml:lang statement or empty string if voice not found</returns>
        public string GetCulture(string strVoiceName)
        {
            foreach (InstalledVoice v in ttsServer.GetInstalledVoices())
            {
                // return the name of the culture used for the 'xml:lang' attribut
                if (v.VoiceInfo.Name.Replace(" ", "|") == strVoiceName)
                    return v.VoiceInfo.Culture.Name;
            }
            //
            return "";
        }


        public void SetVisemeMapping(string visemeMapping)
        {
            pvMapInit(visemeMapping);
        }


        /// <summary>
        /// Generate TTS audio, takes SSML input
        /// </summary>
        /// <param name="message">SSML message</param>
        /// <param name="outputFileName">File name to save output in</param>
        /// <param name="voice">Voice to use</param>
        /// <returns></returns>
        public bool GenerateAudio(string message, string outputFileName, string messageOutputFileName, string voice, ref string xmlReplyReturn, ref GenerateAudioReply generateAudioReplyReturn)
        {
            xmlReplyReturn = "";

            generateAudioReply = generateAudioReplyReturn;
            generateAudioReply.used = true;
            generateAudioReply.soundFile = messageOutputFileName;
            generateAudioReply.WordBreakList = new List<KeyValuePairS<double,double>>();
            generateAudioReply.MarkList = new List<KeyValuePairS<string,double>>();
            generateAudioReply.VisemeList = new List<GenerateAudioReplyViseme>();


            totalPhonemeDuration = 0;
            totalVisemeDuration = 0;

            if (doDebugChecks)
            {
                Console.WriteLine("Generating audio for message with voice: " + voice);
            }

            bool allOk = true;

            // Adding this line to make the application compatible with the NeoSpeech Voice engine
            message = message.Replace("</speak>", ".</speak>");

            ttsServer.SetOutputToWaveFile(outputFileName);

            // In GetVoices(), we replace the space in the name with a pipe '|' because we can't select a voice with a space in the name (because it's a vhmsg sbm command).
            // Here, we put the space back in, so that we can correctly select the voice via the SAPI function SelectVoice()
            string voiceModified = voice.Replace("|", " ");

            Console.WriteLine("Selecting SAPI voice: " + voiceModified);
            try
            {
                ttsServer.SelectVoice(voiceModified);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception while choosing voice: " + e.ToString());
            }


            /// We have a pre-tweaked message, no need to tamper with it
            if (doDebugChecks)
            {
                Console.WriteLine("Debug: Generating speech for SSML string: \"" + message + "\"...\n");
            }

            try
            {
                if (allOk)
                {
                    ttsServer.SpeakSsml(message);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            

            // set the output to Null to release the referrence on the audio file
            // see http://msdn.microsoft.com/en-us/library/system.speech.synthesis.speechsynthesizer.setoutputtowavestream
            ttsServer.SetOutputToNull();

            return allOk;
        }


        /// <summary>
        /// Initialize all phoneme to viseme mappings
        /// </summary>
        private void pvMapInit(string visemeMapping)
        {
            visemeMappingType = visemeMapping;

            visemeIDMap = new List<string>();

            if (visemeMapping == "sbmold")
            {
                visemeIDMap.Insert(0, "_");
                visemeIDMap.Insert(1, "Ih");   /// Viseme for aa, ae, ah
                visemeIDMap.Insert(2, "Ao");   /// Viseme for aa
                visemeIDMap.Insert(3, "Ao");   /// ao
                visemeIDMap.Insert(4, "Ih");   /// ey, eh, uh
                visemeIDMap.Insert(5, "Er");   /// er
                visemeIDMap.Insert(6, "Ih");   /// y, iy, ih, ix
                visemeIDMap.Insert(7, "Oh");   /// w, uw
                visemeIDMap.Insert(8, "Oh");   /// ow
                visemeIDMap.Insert(9, "Ih");   /// aw
                visemeIDMap.Insert(10, "Oh");  /// oy
                visemeIDMap.Insert(11, "Ih");  /// ay
                visemeIDMap.Insert(12, "Oh");  /// h
                visemeIDMap.Insert(13, "R");   /// r
                visemeIDMap.Insert(14, "D");   /// l
                visemeIDMap.Insert(15, "Z");   /// s, z
                visemeIDMap.Insert(16, "j");   /// sh, ch, jh, zh
                visemeIDMap.Insert(17, "Th");  /// th, dh
                visemeIDMap.Insert(18, "F");   /// f, v
                visemeIDMap.Insert(19, "D");   /// d, t, n   - also try NG: 2 to 1 against
                visemeIDMap.Insert(20, "KG");  /// k, g, ,ng   - also try NG: 2 to 1 against
                visemeIDMap.Insert(21, "BMP"); /// p, b, m
            }
            else if (visemeMapping == "sbm")
            {
                /// Map constructed from viseme reference:
                /// http://msdn.microsoft.com/en-us/library/ms720881(VS.85).aspx
                ///
                visemeIDMap.Insert(0, "_");    /// silence
                visemeIDMap.Insert(1, "Ah");   /// Viseme for aa, ae, ah
                visemeIDMap.Insert(2, "Aa");   /// Viseme for aa
                visemeIDMap.Insert(3, "Ao");   /// ao
                visemeIDMap.Insert(4, "Eh");   /// ey, eh, uh
                visemeIDMap.Insert(5, "Er");   /// er
                visemeIDMap.Insert(6, "Ih");   /// y, iy, ih, ix
                visemeIDMap.Insert(7, "W");   /// w, uw
                visemeIDMap.Insert(8, "Ow");   /// ow
                visemeIDMap.Insert(9, "Aw");   /// aw
                visemeIDMap.Insert(10, "Oy");  /// oy
                visemeIDMap.Insert(11, "Ay");  /// ay
                visemeIDMap.Insert(12, "H");  /// h
                visemeIDMap.Insert(13, "R");   /// r
                visemeIDMap.Insert(14, "L");   /// l
                visemeIDMap.Insert(15, "Z");   /// s, z
                visemeIDMap.Insert(16, "Sh");   /// sh, ch, jh, zh
                visemeIDMap.Insert(17, "Th");  /// th, dh
                visemeIDMap.Insert(18, "F");   /// f, v
                visemeIDMap.Insert(19, "D");   /// d, t, n   - also try NG: 2 to 1 against
                visemeIDMap.Insert(20, "KG");  /// k, g, ,ng   - also try NG: 2 to 1 against
                visemeIDMap.Insert(21, "BMP"); /// p, b, m
            }
            else if (visemeMapping == "facefx")
            {
                //visemeIDMap.Insert(0, "_");
                try
                {
                    facefxMappingDoc = new XmlDocument();
                    facefxMappingDoc.LoadXml(facefxMapping);
                }
                catch (Exception e)
                {
                    Console.WriteLine("error loading the xml string for facefx visemes:" + e.ToString() + "\n");
                }
            }
        }

        /// <summary>
        /// Viseme callback - maps SAPI visemes to SmartBody visemes/phonemes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ttsServer_VisemeReached(object sender, VisemeReachedEventArgs e)
        {
            int sapiVisemeCount;

            if (visemeMappingType.Equals("facefx"))
                sapiVisemeCount = 21;
            else
                sapiVisemeCount = visemeIDMap.Count;


            if (doDebugChecks)
            {
                int vindex = e.Viseme;

                /// This is a safety capture, if either of these is being triggered, some changes have been made to the SAPI API, change the map accordingly
                if (vindex < 0)
                {
                    Console.WriteLine("Viseme index bound by 0: " + vindex.ToString() + "\n");
                    vindex = 0;
                }
                else if (vindex >= sapiVisemeCount)
                {
                    Console.WriteLine("Viseme index bound by " + visemeIDMap.Count + ": " + vindex.ToString() + "\n");
                    vindex = visemeIDMap.Count - 1;
                }

                if (vindex != e.Viseme)
                    Console.WriteLine("Viseme index truncated from " + e.Viseme.ToString() + " to " + vindex + "\n");

                //Console.WriteLine("Reached viseme: " + e.Viseme.ToString() + " aka: " + visemeIDMap[vindex] + " at time: " + e.AudioPosition.TotalSeconds.ToString() + " for duration: " + e.Duration.ToString() + "\n");
                Console.WriteLine("Total viseme duration: " + this.totalVisemeDuration);
            }

            // We should be able to just take the AudioPosition time, which denotes the point in the request the viseme starts,
            // but this is way off for some reason; different visemes start at the exact same time, and the schedule extends beyond
            // the lenght of the audiofile
            //generateAudioReply.VisemeList.Add(new GenerateAudioReplyViseme(visemeIDMap[e.Viseme], e.AudioPosition.TotalSeconds, 1.0));

            // Instead, we manually keep track of the total duration so far, and use that as the starting point for each viseme.

            if (visemeMappingType.Equals("facefx"))
            {
                XmlNodeList entries = facefxMappingDoc.GetElementsByTagName("entry");

                for (int i = 0; i < entries.Count; ++i)
                {
                    string target = entries[i].Attributes["target"].InnerText;
                    string articulation = entries[i].Attributes["amount"].InnerText;
                    string viseme = entries[i].Attributes["viseme"].InnerText;

                    if (e.Viseme.Equals(Convert.ToInt32(viseme)))
                    {
                        //xmlReply += "<viseme start=\"" + totalVisemeDuration.ToString() + "\" type=\"" + target + "articulation=\"" + articulation + "\" \"/>";
                        try
                        {
                            double articulationValue = Convert.ToDouble(articulation);
                            generateAudioReply.VisemeList.Add(new GenerateAudioReplyViseme(target, totalVisemeDuration, articulationValue ));
                        }
                        catch(Exception e1)
                        {
                            Console.WriteLine(e1.ToString());
                        }

                    }
                }
            }
            else
            {
                //xmlReply += "<viseme start=\"" + totalVisemeDuration.ToString() + "\" type=\"" + visemeIDMap[e.Viseme] + "\"/>";
                generateAudioReply.VisemeList.Add(new GenerateAudioReplyViseme(visemeIDMap[e.Viseme], totalVisemeDuration, 1.0));
            }

            this.totalVisemeDuration += e.Duration.TotalSeconds;
        }

        /// <summary>
        /// Phoneme reached callback - doesn't do anything
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ttsServer_PhonemeReached(object sender, PhonemeReachedEventArgs e)
        {
            totalPhonemeDuration += e.Duration.TotalSeconds;

            if (doDebugChecks)
            {
                Console.WriteLine("Reached phoneme: " + e.Phoneme + " at time: " + e.AudioPosition.TotalSeconds.ToString() + " for duration: " + e.Duration.ToString() + "\n");
                Console.WriteLine("Total phoneme duration: " + this.totalPhonemeDuration);
                //byte[] b = System.Text.Encoding.Unicode.GetBytes(e.Phoneme.ToCharArray());
                //Console.WriteLine("Chars for bytes: ");
                //Console.WriteLine(System.Text.Encoding.ASCII.GetChars(b));
                //string asciiPhoneme = System.Text.Encoding.ASCII.GetString(System.Text.Encoding.Convert(Encoding.Unicode,Encoding.ASCII,b));
                //Console.WriteLine("Test ascii phoneme: " + asciiPhoneme + "\n");
                //Console.WriteLine("Viseme attached= " + phonemeToViseme[phoneme] + "\n");
            }
        }

        /// <summary>
        /// Bookmark reached callback - used to mark word beginnings and ends
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ttsServer_BookmarkReached(object sender, BookmarkReachedEventArgs e)
        {
            String bookmark;
            bookmark = e.Bookmark.Substring(e.Bookmark.IndexOf(':') + 1);

            if (doDebugChecks)
            {
                Console.WriteLine("Reached bookmark: " + bookmark + " at time: " + e.AudioPosition.TotalSeconds.ToString() + "\n");
            }

            // The provided AudioPosition is erroneous, so we're mannually keeping track of where we are using aggregate viseme duration.
            //generateAudioReply.MarkList.Add(new KeyValuePair<string, double>(bookmark, e.AudioPosition.TotalSeconds));
            generateAudioReply.MarkList.Add(new KeyValuePairS<string, double>(bookmark, totalVisemeDuration));

            /// Since we don't have a word beginning/ending callback, we resort
            /// to relying on the fact that each word in the SSML message is
            /// enclosed in a (book)mark, so the number of marks is
            /// always even. But the logic used should not rely on the tag name, only
            /// whether it's the odd or even (in order of occurence)
            if (markerExists)
            {
                /// word tag exists, write end time
                //generateAudioReply.WordBreakList.Add(new KeyValuePair<double,double>(wordMarker.Key, e.AudioPosition.TotalSeconds));
                generateAudioReply.WordBreakList.Add(new KeyValuePairS<double,double>(wordMarker.Key, totalVisemeDuration));
                markerExists = false;
            }
            else
            {
                /// no word tag, add new word tag
                //wordMarker = new KeyValuePair<double,double>(e.AudioPosition.TotalSeconds, 0);
                wordMarker = new KeyValuePair<double,double>(totalVisemeDuration, 0);
                markerExists = true;
            }
        }
    }
}
#endif
