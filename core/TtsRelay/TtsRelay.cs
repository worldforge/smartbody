
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#if (!__MonoCS__)
using System.Speech.Synthesis;
#endif
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;


namespace VHuman
{
    interface IComponent
    {
        bool Init();
        void Close();
        bool RegisterVHMsg(VHMsg.Client vhmsg);
        bool Update(double timeSeconds);
        bool OnVHMessage(object sender, VHMsg.Message e);
    }
}


namespace TtsRelay
{
    // Had to define a custom version of KeyValuePair so that it would xml serialize.  The built-in one wouldn't.  http://blogs.msdn.com/b/seshadripv/archive/2005/11/02/488273.aspx
    public struct KeyValuePairS<K,V>
    {
        public K Key {get;set;}
        public V Value {get;set;}

        public KeyValuePairS(K key, V value) : this() { Key = key; Value = value; }
    }

    public struct GenerateAudioReplyViseme
    {
        public string type;
        public double start;
        public double articulation;

        public GenerateAudioReplyViseme(string type, double start, double articulation) { this.type = type; this.start = start; this.articulation = articulation; }
    }

    public class GenerateAudioReply
    {
        public bool used;
        public string soundFile;
        public List<KeyValuePairS<double, double>> WordBreakList;  // start/end
        public List<KeyValuePairS<string, double>> MarkList;  // name/time
        public List<GenerateAudioReplyViseme> VisemeList;  // type/start/articulation
    }

    public interface ITtsRelay
    {
        bool Init(string visemeMapping);
        string [] GetVoices();
        void SetVisemeMapping(string visemeMapping);
        bool GenerateAudio(string message, string outputFileName, string messageOutputFileName, string voice, ref string xmlReplyReturn, ref GenerateAudioReply generateAudioReplyReturn);
    }


    // used for sorting the events into the correct order to mimick the messages we've always sent
    public enum SortEnum
    {
        // if time is equal, the order is:
        OddMark,
        EndWord,
        EvenMark,
        StartWord,
        Viseme,
    }

    public class SortClass
    {
        public SortEnum sortEnum;
        public object obj;

        public SortClass( SortEnum sortEnum, object obj ) { this.sortEnum = sortEnum; this.obj = obj; }
    }


    /// <summary>
    /// Main program
    /// </summary>
    public class TtsRelayComponent : VHuman.IComponent
    {
        private static VHMsg.Client m_vhmsg;

        private string m_cacheFolder = "../../../../../data/cache/audio";

        private Dictionary<string, ITtsRelay> voicemap;
        #if (!__MonoCS__)
        private MsSpeechRelay msspeech;
        #endif
        private FestivalRelay festival;
        private RvoiceRelay rvoice;
        private CerevoiceRelay cerevoice;

        private string m_visemeMapping;

        private Dictionary<string, GenerateAudioReply> m_cacheList;

        public void ClearCache()
        {
            Console.WriteLine("Cache Map Number Items Removed: " + m_cacheList.Count);
            m_cacheList.Clear();
        }
        public void SetVisemeMapping(string visemeMapping)
        {
            m_visemeMapping = visemeMapping;

            #if (!__MonoCS__)
            if (msspeech != null)  msspeech.SetVisemeMapping(m_visemeMapping);
            #endif
            if (festival != null)  festival.SetVisemeMapping(m_visemeMapping);
            if (rvoice != null)    rvoice.SetVisemeMapping(m_visemeMapping);
            if (cerevoice != null) cerevoice.SetVisemeMapping(m_visemeMapping);
        }

        public void SetCacheFolderPath(string path)
        {
            m_cacheFolder = path;
        }

        public bool Init()
        {
            if (!File.Exists(m_cacheFolder))
            {
                Directory.CreateDirectory(m_cacheFolder);
            }


            m_cacheList = new Dictionary<string, GenerateAudioReply>();
            CacheListLoad();

            #if (!__MonoCS__)
            msspeech = new MsSpeechRelay();
            msspeech.Init(m_visemeMapping);
            #endif
            Console.WriteLine("TtsRelay Checking for DLLs");
            if ( File.Exists( "FestivalDLL.dll" ) )
            {
                festival = new FestivalRelay();
                festival.Init(m_visemeMapping);
            }

            if ( File.Exists( "libFestivalDLL.dylib" ) )
            {
                festival = new FestivalRelay();
                festival.Init(m_visemeMapping);
            }

            if ( File.Exists( "RvoiceDLL.dll" ) )
            {
                rvoice = new RvoiceRelay();
                rvoice.Init(m_visemeMapping);
            }

            if ( File.Exists( "CerevoiceDLL.dll" ) )
            {
                cerevoice = new CerevoiceRelay();
                cerevoice.Init(m_visemeMapping);
            }

            Console.WriteLine("TtsRelay Finished Checking for DLLs");
            voicemap = new Dictionary<string, ITtsRelay>();

            string [] voices;
            #if (!__MonoCS__)
            voices = msspeech.GetVoices();
            foreach ( string voice in voices )
            {
                Console.WriteLine( "Adding '{0}' voice '{1}'", "msspeech", voice );
                voicemap.Add( voice, msspeech );
            }
            #endif
            if ( festival != null )
            {
                voices = festival.GetVoices();
                foreach ( string voice in voices )
                {
                    Console.WriteLine( "Adding '{0}' voice '{1}'", "festival", voice );
                    voicemap.Add( voice, festival );
                }
            }

            if ( rvoice != null )
            {
                voices = rvoice.GetVoices();
                foreach ( string voice in voices )
                {
                    Console.WriteLine( "Adding '{0}' voice '{1}'", "rvoice", voice );
                    voicemap.Add( voice, rvoice );
                }
            }

            if ( cerevoice != null )
            {
                voices = cerevoice.GetVoices();
                foreach ( string voice in voices )
                {
                    Console.WriteLine( "Adding '{0}' voice '{1}'", "cerevoice", voice );
                    voicemap.Add( voice, cerevoice );
                }
            }

            return true;
        }

        public void Close()
        {
            CacheListSave();
        }

        public bool RegisterVHMsg(VHMsg.Client vhmsg)
        {
            m_vhmsg = vhmsg;
            return true;
        }

        public bool Update(double timeSeconds) { return true; }

        public bool OnVHMessage(object sender, VHMsg.Message e)
        {
            string[] splitargs = e.s.Split(" ".ToCharArray());

            if (splitargs[0] == "RemoteSpeechCmd")
            {
                Console.WriteLine("Received speech command: " + e.s + "\n");

                ProcessSpeechMessage(e.s);
            }
            else if (splitargs[0] == "TTSRelay")
            {
                // TTSRelay setmapping [facefx|sbm]
                if (splitargs.Length >= 1 && splitargs[1] == "setmapping")
                {
                    if (splitargs.Length >= 2)
                    {
                        string visemeMapping = splitargs[2];
                        SetVisemeMapping(visemeMapping);

                        Console.WriteLine(string.Format(@"Changing the viseme mapping to: {0}", visemeMapping));
                    }
                }

            }

            return true;
        }

        /// <summary>
        /// Process Speech Message and send reply
        /// </summary>
        /// <param name="message">The message</param>
        private void ProcessSpeechMessage(string message)
        {
            Console.WriteLine("Processing message.\n");
            char[] myDelims = { ' ', '\n', '\r', '\t' };
            string[] splitargs = message.Split(myDelims);

            string actor = splitargs[2];
            string id = splitargs[3];
            string voice = splitargs[4];


            if ( !voicemap.ContainsKey( voice ) )
            {
                Console.WriteLine( "Error: No speech engine loaded that matches voice '{0}'", voice );
                return;
            }

            ITtsRelay relay = voicemap[ voice ];
            

            /// Trim path from filename, and add a .wav to it
            string fileName = splitargs[5];
            fileName = Path.GetFileNameWithoutExtension(fileName) + ".wav";
            string outputFileName = Path.Combine(m_cacheFolder, fileName);
            string outputFileNameFull = Path.GetFullPath(outputFileName);

            /// Get XML portion of message
            string xmlMessageString = message.Substring(message.IndexOf('<'));
            #if (!__MonoCS__)
            if ( relay is MsSpeechRelay )
            {
                /// Remove spurious whitespaces from xml message, since it is a multi-line message
                xmlMessageString = xmlMessageString.Replace('\n', ' ');
                xmlMessageString = xmlMessageString.Replace('\t', ' ');
                xmlMessageString = xmlMessageString.Replace('\r', ' ');
            }
            #endif
            XmlDocument xmlMessage = new XmlDocument();
            xmlMessage.InnerXml = xmlMessageString;
            XmlElement root = xmlMessage.DocumentElement;

            #if (!__MonoCS__)
            /// We make the message such that it only contains the speech XML element
            XmlElement speechPart = root;

            string spID = speechPart.GetAttribute("id");

            if ( relay is MsSpeechRelay )
            {
                /// Currently the SSML messages generated by SmartBody do not fully correspond to the SSML spec, so we need to make minor changes to make it comply
                speechPart.RemoveAttribute("id");
                speechPart.RemoveAttribute("type");
                speechPart.RemoveAttribute("ref");
                speechPart.SetAttribute("version", "1.0");

                // let's find out automatically, which culture we need to specifz for the "xml:lang" attribute
                // due to the evaluation of !voicemap.ContainsKey( voice ) beforehand, we are sure that the voice exists here
                string strCulture = msspeech.GetCulture(voice);
                Console.WriteLine("The culture of the selected voice is " + strCulture);
                speechPart.SetAttribute("xml:lang", strCulture);
            }


            if ( relay is MsSpeechRelay )
            {
                /// We want to rename all tags T0, T1, ... as spID:T0...
                XmlNodeList marks = speechPart.GetElementsByTagName("mark");
                foreach (XmlNode n in marks)
                {
                    XmlElement elem = (XmlElement)n;
                    elem.SetAttribute("name", spID + ":" + elem.GetAttribute("name"));
                }
            }
            #endif

            /// Done, now generate audio
            string xmlReply = "";
            bool allOk = false;

            string relayMessage = xmlMessage.OuterXml;
            #if (!__MonoCS__)
            if ( relay is MsSpeechRelay )
            {
                relayMessage = speechPart.OuterXml.Replace("speech", "speak");
            }
            #endif
            Console.WriteLine( "GenerateAudio() - '{0}' '{1}' '{2}' '{3}'", relayMessage, outputFileNameFull, outputFileNameFull, voice );

            string key = CacheCreateKeyName(voice, m_visemeMapping, relayMessage);

            GenerateAudioReply generateAudioReplyReturn;

            generateAudioReplyReturn = CacheHit(key);
            bool cacheHit = (generateAudioReplyReturn != null);

            if (cacheHit)
            {
                Console.WriteLine("GenerateAudio() - cache hit - not sending to TTS engine");

                allOk = true;

                if (generateAudioReplyReturn.used)
                {
                    if (!File.Exists(generateAudioReplyReturn.soundFile))
                    {
                        cacheHit = false;
                        CacheRemoveKey(key);

                        Console.WriteLine("GenerateAudio() - cache error - audio file from cache entry doesn't exist on disk.  Toggling cacheHit, sending to TTS engine");
                    }
                }
            }

            if (!cacheHit)
            {
                generateAudioReplyReturn = new GenerateAudioReply();
                allOk = relay.GenerateAudio( relayMessage, outputFileNameFull, outputFileNameFull, voice, ref xmlReply, ref generateAudioReplyReturn );

                if (allOk)
                {
                    m_cacheList.Add(key, generateAudioReplyReturn);
                }
            }

            if (generateAudioReplyReturn.used)
            {
                StringBuilder output = new StringBuilder();
                XmlWriterSettings settings = new XmlWriterSettings();
                settings.Indent = true;
                settings.Encoding = Encoding.UTF8;
                using (XmlWriter xml = XmlWriter.Create(output, settings))
                {
                    //xml.WriteStartDocument();
                    xml.WriteProcessingInstruction("xml", @"version=""1.0"" encoding=""UTF-8""");
                    xml.WriteStartElement("speak");
                    xml.WriteStartElement("soundFile");
                    xml.WriteAttributeString("name", generateAudioReplyReturn.soundFile);
                    xml.WriteEndElement();  // soundFile


                    List<KeyValuePair<double, SortClass>> combinedList = new List<KeyValuePair<double, SortClass>>();

                    foreach (KeyValuePairS<double, double> kv in generateAudioReplyReturn.WordBreakList)
                    {
                        combinedList.Add(new KeyValuePair<double, SortClass>(kv.Key, new SortClass(SortEnum.StartWord, kv)));
                        combinedList.Add(new KeyValuePair<double, SortClass>(kv.Value, new SortClass(SortEnum.EndWord, kv)));
                    }

                    foreach (KeyValuePairS<string, double> kv in generateAudioReplyReturn.MarkList)
                    {
                        // special case due to how the xml has been generated all these years
                        // determine whether the marker is odd or even, and sort accordingly
                        string markName = kv.Key;
                        int markNumber = 0;
                        if (markName.StartsWith("T"))
                        {
                            markName = markName.Remove(0, 1); // remove the T
                            markNumber = Convert.ToInt32(markName);
                        }
                        else if (relay is CerevoiceRelay && markName.StartsWith("sp1:T"))
                        {
                            markName = markName.Remove(0, 5); // remove the T
                            markNumber = Convert.ToInt32(markName);
                        }
                        SortEnum sortEnum = markNumber % 2 == 0 ? SortEnum.EvenMark : SortEnum.OddMark;

                        combinedList.Add(new KeyValuePair<double, SortClass>(kv.Value, new SortClass(sortEnum, kv)));
                    }

                    foreach (GenerateAudioReplyViseme kv in generateAudioReplyReturn.VisemeList)
                    {
                        combinedList.Add(new KeyValuePair<double, SortClass>(kv.start, new SortClass(SortEnum.Viseme, kv)));
                    }

                    combinedList.Sort(delegate(KeyValuePair<double, SortClass> x, KeyValuePair<double, SortClass> y)
                    {
                        int compare = x.Key.CompareTo(y.Key);
                        if (compare == 0)
                            return x.Value.sortEnum.CompareTo(y.Value.sortEnum);
                        else
                            return compare;
                    });

                    foreach (KeyValuePair<double, SortClass> kv in combinedList)
                    {
                        SortClass value = kv.Value;

                        if (value.sortEnum == SortEnum.EvenMark ||
                            value.sortEnum == SortEnum.OddMark)
                        {
                            KeyValuePairS<string, double> v = (KeyValuePairS<string, double>)value.obj;
                            xml.WriteStartElement("mark");
                            xml.WriteAttributeString("name", v.Key);
                            xml.WriteAttributeString("time", v.Value.ToString());
                            xml.WriteEndElement();  // mark

                            //Console.WriteLine("mark name='{0}' time='{1}'", v.Key, v.Value.ToString());
                        }
                        else if (value.sortEnum == SortEnum.StartWord)
                        {
                            KeyValuePairS<double, double> v = (KeyValuePairS<double, double>)value.obj;
                            xml.WriteStartElement("word");
                            xml.WriteAttributeString("start", v.Key.ToString());
                            xml.WriteAttributeString("end", v.Value.ToString());

                            //Console.WriteLine("word start='{0}' end='{1}'", v.Key.ToString(), v.Value.ToString());
                        }
                        else if (value.sortEnum == SortEnum.Viseme)
                        {
                            GenerateAudioReplyViseme v = (GenerateAudioReplyViseme)value.obj;
                            xml.WriteStartElement("viseme");
                            xml.WriteAttributeString("start", v.start.ToString());
                            xml.WriteAttributeString("articulation", v.articulation.ToString());
                            xml.WriteAttributeString("type", v.type);
                            xml.WriteEndElement();  // viseme

                            //Console.WriteLine("viseme start='{0}' type='{1}'", v.Value.ToString(), v.Key);
                        }
                        else if (value.sortEnum == SortEnum.EndWord)
                        {
                            xml.WriteEndElement();  // word

                            //Console.WriteLine("word end");
                        }
                    }

                    xml.WriteEndElement();  // speak
                    //xml.WriteEndDocument();
                }

                xmlReply = output.ToString();
            }

            /// Now, send back message to whoever wanted the audio
            ///
            string replyMessage = "RemoteSpeechReply " + actor + " " + id;
            if (allOk)
            {
                replyMessage += " OK: " + xmlReply;
            }
            else
            {
                replyMessage += " ERROR: Unable to choose suggested voice: \"" + voice + "\" and strict mode used";
            }

            m_vhmsg.SendMessage(replyMessage);

            Console.WriteLine("Debug: Sending reply: \"" + replyMessage + "\"\n");
        }

        private string CacheCreateKeyName(string voice, string visemeMapping, string relayMessage)
        {
            return string.Format("{0} - {1} - {2}", voice, visemeMapping, relayMessage);
        }

        private GenerateAudioReply CacheHit(string key)
        {
            GenerateAudioReply ret;
            if (m_cacheList.TryGetValue(key, out ret))
            {
                return ret;
            }
            else
            {
                return null;
            }
        }

        private bool CacheRemoveKey(string key)
        {
            return m_cacheList.Remove(key);
        }

        private bool CacheListSave()
        {
            string cacheFileName = Path.Combine(m_cacheFolder, "cache.xml");

            Console.WriteLine("CacheListSave() - saving cache file - '{0}'", cacheFileName);

            // convert the Dictionary to a List because Dictionary's are not xml serializable
            List<KeyValuePairS<string, GenerateAudioReply>> list = new List<KeyValuePairS<string, GenerateAudioReply>>();
            foreach (KeyValuePair<string, GenerateAudioReply> entry in m_cacheList)
            {
                list.Add(new KeyValuePairS<string, GenerateAudioReply>(entry.Key, entry.Value));
            }

            try
            {
                using (TextWriter textWriter = new StreamWriter(cacheFileName))
                {
                    XmlSerializer serializer = new XmlSerializer(list.GetType());
                    serializer.Serialize(textWriter, list);
                }
            }
            catch (Exception)
            {
                Console.WriteLine("CacheListSave() - cache file save failed!");

                return false;
            }

            return true;
        }

        private bool CacheListLoad()
        {
            string cacheFileName = Path.Combine(m_cacheFolder, "cache.xml");

            Console.WriteLine("CacheListLoad() - reading cache file - '{0}'", cacheFileName);

            List<KeyValuePairS<string, GenerateAudioReply>> list = new List<KeyValuePairS<string, GenerateAudioReply>>();

            try
            {
                using (TextReader textReader = new StreamReader(cacheFileName))
                {
                    XmlSerializer deserializer = new XmlSerializer(list.GetType());
                    list = (List<KeyValuePairS<string, GenerateAudioReply>>)deserializer.Deserialize(textReader);
                }

                m_cacheList = new Dictionary<string, GenerateAudioReply>();

                // convert the List to a Dictionary because Dictionary's are not xml serializable
                foreach (KeyValuePairS<string, GenerateAudioReply> entry in list)
                {
                    m_cacheList.Add(entry.Key, entry.Value);
                }
            }
            catch (Exception)
            {
                Console.WriteLine("CacheListLoad() - cache file load failed, starting with no cache");

                return false;
            }

            return true;
        }
    }
}
