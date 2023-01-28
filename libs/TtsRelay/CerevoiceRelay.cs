
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#if (!__MonoCS__)
using System.Speech.Synthesis;
#endif
using System.Xml;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;


namespace TtsRelay
{
    public class CerevoiceDLL
    {
        // [DllImport("msvcrt.dll", EntryPoint="puts")]
        [DllImport("CerevoiceDLL.dll")]
        public static extern bool CEREVOICE_DLL_Init([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("CerevoiceDLL.dll")]
        public static extern void CEREVOICE_DLL_SetVisemeMapping([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("CerevoiceDLL.dll")]
        public static extern bool CEREVOICE_DLL_GenerateAudio([MarshalAs(UnmanagedType.LPStr)] string message, [MarshalAs(UnmanagedType.LPStr)] string outputFilename, [MarshalAs(UnmanagedType.LPStr)] string messageOutputFileName, [MarshalAs(UnmanagedType.LPStr)] string voice, [Out] StringBuilder xmlReplyReturn, ref GenerateAudioReplyInterop generateAudioReplyInterop);

        /*
        [DllImport("kernel32.dll", SetLastError=true)]
        [PreserveSig]
        public static extern uint GetModuleFileName
        (
            [In]
            IntPtr hModule,

            [Out]
            StringBuilder lpFilename,

            [In]
            [MarshalAs(UnmanagedType.U4)]
            int nSize
        );

        StringBuilder fileName = new StringBuilder(255);
        DllFuntions.GetModuleFileName(IntPtr.Zero, fileName, fileName.Capacity);
        Console.WriteLine(fileName);
         */
    }

    public class CerevoiceRelay : ITtsRelay
    {
        public bool Init(string visemeMapping)
        {
            CerevoiceDLL.CEREVOICE_DLL_Init(visemeMapping);
            return true;
        }


        public string [] GetVoices()
        {
            //return new string[] { "katherine", "star" };


            // We enumerate the voices in the folder based on filename.

            string cerevoiceDir = @"../../../../../data/cereproc/voices";

            List<string> list = new List<string>();

            Console.WriteLine("cerevoiceDir: {0}", cerevoiceDir);
            DirectoryInfo voicedi = new DirectoryInfo(cerevoiceDir);
            if (voicedi.Exists)
            {
                FileInfo[] voiceNames = voicedi.GetFiles("*.lic");

                foreach (FileInfo fi in voiceNames)
                {
                    string voiceName = fi.Name;
                    voiceName = voiceName.Replace(".lic", "");
                    voiceName = "Cerevoice_" + voiceName;
                    list.Add(voiceName);
                }
            }

            return list.ToArray();
        }


        public void SetVisemeMapping(string visemeMapping)
        {
            CerevoiceDLL.CEREVOICE_DLL_SetVisemeMapping(visemeMapping);
        }


        /*
        received -- op: RemoteSpeechCmd
        args:
         * RemoteSpeechCmd speak brad 6 katherine
         * ../../data/cache/audio/utt_20111013_204717_brad_6.aiff
         * <?xml version="1.0" encoding="UTF-8"?>
         * <speech type="text/plain">Hello world.  Testing Text to Speech </speech>

        process_message() -
         * speak brad 6 katherine
         * ../../data/cache/audio/utt_20111013_204717_brad_6.aiff
         * <?xml version="1.0" encoding="UTF-8"?>
         * <speech type="text/plain">Hello world.  Testing Text to Speech </speech>'
         *
         * cerevoice_tts::tts() -
         * <?xml version="1.0" encoding="UTF-8"?>
         * <speech type="text/plain">Hello world.  Testing Text to Speech </speech>
         * '../../data/cache/audio/utt_20111013_204717_brad_6.wav'
         * 'd:\edwork\saso\data\cache\audio\utt_20111013_204717_brad_6.wav'
         * 'katherine'
         *
         * from cerevoicerelay
         * RemoteSpeechReply
         * brad 6 OK:
         * <?xml version="1.0" encoding="UTF-8"?>
         * <speak><soundFile name="d:\edwork\saso\data\cache\audio\utt_20111013_204717_brad_6.wav"/>
         * <viseme start="0.000000" type="_"/><mark name="sp1:T0" time="0.010975"/><mark name="sp1:T1" time="0.010975"/><word end="2.054859" start="0.010975"><viseme start="0.010975" type="Ih"/><viseme start="0.160952" type="j"/><viseme start="0.286513" type="EE"/><viseme start="0.419439" type="Ih"/><viseme start="0.483591" type="D"/><viseme start="0.641502" type="Ih"/><viseme start="0.695640" type="D"/><viseme start="0.825386" type="Oh"/><viseme start="0.893364" type="D"/><viseme start="0.961771" type="Ih"/><viseme start="1.046893" type="BMP"/><viseme start="1.096916" type="Ih"/><viseme start="1.126893" type="D"/><viseme start="1.156916" type="OO"/><viseme start="1.225665" type="Oh"/><viseme start="1.310088" type="Oh"/><viseme start="1.473512" type="Ao"/><viseme start="1.547230" type="R"/><viseme start="1.604858" type="Ih"/><viseme start="1.655760" type="D"/><viseme start="1.831755" type="D"/><viseme start="1.929188" type="EE"/></word><mark name="sp1:T2" time="2.054859"/><mark name="sp1:T3" time="2.054859"/><word end="2.285578" start="2.054859"><viseme start="2.054859" type="D"/><viseme start="2.119494" type="Ao"/><viseme start="2.250815" type="D"/></word><mark name="sp1:T4" time="2.285578"/><mark name="sp1:T5" time="2.285578"/><word end="6.415465" start="2.285578"><viseme start="2.285578" type="D"/><viseme start="2.360195" type="EE"/><viseme start="2.447514" type="EE"/><viseme start="2.619389" type="Ih"/><viseme start="2.714467" type="Z"/><viseme start="2.814467" type="D"/><viseme start="2.873999" type="EE"/><viseme start="2.985955" type="Ih"/><viseme start="3.212323" type="Ih"/><viseme start="3.326141" type="NG"/><viseme start="3.427197" type="j"/><viseme start="3.511715" type="EE"/><viseme start="3.616000" type="D"/><viseme start="3.753588" type="EE"/><viseme start="3.825710" type="EE"/><viseme start="3.998662" type="Ih"/><viseme start="4.154739" type="KG"/><viseme start="4.239773" type="Z"/><viseme start="4.284762" type="D"/><viseme start="4.395038" type="EE"/><viseme start="4.467888" type="D"/><viseme start="4.603050" type="EE"/><viseme start="4.731071" type="Oh"/><viseme start="4.861543" type="Ih"/><viseme start="4.996281" type="Z"/><viseme start="5.106213" type="BMP"/><viseme start="5.186693" type="EE"/><viseme start="5.263379" type="EE"/><viseme start="5.524380" type="EE"/><viseme start="5.690654" type="Z"/><viseme start="5.845442" type="EE"/><viseme start="6.035465" type="Ih"/><viseme start="6.225442" type="j"/></word><mark name="sp1:T6" time="6.415465"/><mark name="sp1:T7" time="6.415465"/><viseme start="6.415465" type="_"/>
         * </speak>
         * vrAgentBML brad sbm_test_bml_14 start
         * PlaySound "d:\edwork\saso\data\cache\audio\utt_20111013_204717_brad_6.wav" brad
         *
         * from ttsspeechrelay
         * RemoteSpeechReply
         * brad 9 OK:
         * unused 1 OK:
         * <?xml version="1.0" encoding="UTF-8"?>
         * <speak><soundFile name="D:\edwork\saso\libs\TtsSpeechRelay\bin\data\cache\audio\utt_20111013_210755_brad_9.wav.wav"/>
         * <viseme start="0.000000" type="_"/><mark name="sp1:T0" time="0.010975"/><mark name="sp1:T1" time="0.010975"/><word end="2.054859" start="0.010975"><viseme start="0.010975" type="Ih"/><viseme start="0.160952" type="j"/><viseme start="0.286513" type="EE"/><viseme start="0.419439" type="Ih"/><viseme start="0.483591" type="D"/><viseme start="0.641502" type="Ih"/><viseme start="0.695640" type="D"/><viseme start="0.825386" type="Oh"/><viseme start="0.893364" type="D"/><viseme start="0.961771" type="Ih"/><viseme start="1.046893" type="BMP"/><viseme start="1.096916" type="Ih"/><viseme start="1.126893" type="D"/><viseme start="1.156916" type="OO"/><viseme start="1.225665" type="Oh"/><viseme start="1.310088" type="Oh"/><viseme start="1.473512" type="Ao"/><viseme start="1.547230" type="R"/><viseme start="1.604858" type="Ih"/><viseme start="1.655760" type="D"/><viseme start="1.831755" type="D"/><viseme start="1.929188" type="EE"/></word><mark name="sp1:T2" time="2.054859"/><mark name="sp1:T3" time="2.054859"/><word end="2.285578" start="2.054859"><viseme start="2.054859" type="D"/><viseme start="2.119494" type="Ao"/><viseme start="2.250815" type="D"/></word><mark name="sp1:T4" time="2.285578"/><mark name="sp1:T5" time="2.285578"/><word end="6.415465" start="2.285578"><viseme start="2.285578" type="D"/><viseme start="2.360195" type="EE"/><viseme start="2.447514" type="EE"/><viseme start="2.619389" type="Ih"/><viseme start="2.714467" type="Z"/><viseme start="2.814467" type="D"/><viseme start="2.873999" type="EE"/><viseme start="2.985955" type="Ih"/><viseme start="3.212323" type="Ih"/><viseme start="3.326141" type="NG"/><viseme start="3.427197" type="j"/><viseme start="3.511715" type="EE"/><viseme start="3.616000" type="D"/><viseme start="3.753588" type="EE"/><viseme start="3.825710" type="EE"/><viseme start="3.998662" type="Ih"/><viseme start="4.154739" type="KG"/><viseme start="4.239773" type="Z"/><viseme start="4.284762" type="D"/><viseme start="4.395038" type="EE"/><viseme start="4.467888" type="D"/><viseme start="4.603050" type="EE"/><viseme start="4.731071" type="Oh"/><viseme start="4.861543" type="Ih"/><viseme start="4.996281" type="Z"/><viseme start="5.106213" type="BMP"/><viseme start="5.186693" type="EE"/><viseme start="5.263379" type="EE"/><viseme start="5.524380" type="EE"/><viseme start="5.690654" type="Z"/><viseme start="5.845442" type="EE"/><viseme start="6.035465" type="Ih"/><viseme start="6.225442" type="j"/></word><mark name="sp1:T6" time="6.415465"/><mark name="sp1:T7" time="6.415465"/><viseme start="6.415465" type="_"/>
         * </speak>
        */

        public bool GenerateAudio(string message, string outputFileName, string messageOutputFileName, string voice, ref string xmlReplyReturn, ref GenerateAudioReply generateAudioReplyReturn)
        {
            StringBuilder reply = new StringBuilder(65536);

            // The voice names have "Festival_" prepended to identify them, but the real festival names don't have this
            string cerevoiceVoice = voice.Remove(0, "Cerevoice_".Length);

            GenerateAudioReplyInterop generateAudioReplyInterop = new GenerateAudioReplyInterop();

            CerevoiceDLL.CEREVOICE_DLL_GenerateAudio(message, outputFileName, messageOutputFileName, cerevoiceVoice, reply, ref generateAudioReplyInterop);
            xmlReplyReturn = reply.ToString();

            generateAudioReplyReturn.used = true;
            generateAudioReplyReturn.soundFile = Marshal.PtrToStringAnsi(generateAudioReplyInterop.soundFile);
            generateAudioReplyReturn.WordBreakList = new List<KeyValuePairS<double,double>>();
            generateAudioReplyReturn.MarkList = new List<KeyValuePairS<string,double>>();
            generateAudioReplyReturn.VisemeList = new List<GenerateAudioReplyViseme>();

            for (int i = 0; i < generateAudioReplyInterop.workBreakListNum; i++)
            {
                double wordBreakListStart = generateAudioReplyInterop.wordBreakListStart.ElementAt<double>(i);
                double wordBreakListEnd = generateAudioReplyInterop.wordBreakListEnd.ElementAt<double>(i);
                generateAudioReplyReturn.WordBreakList.Add(new KeyValuePairS<double, double>(wordBreakListStart, wordBreakListEnd));
            }

            for (int i = 0; i < generateAudioReplyInterop.markListNum; i++)
            {
                string markListName = Marshal.PtrToStringAnsi(generateAudioReplyInterop.markListName.ElementAt<IntPtr>(i));
                double markListTime = generateAudioReplyInterop.markListTime.ElementAt<double>(i);
                generateAudioReplyReturn.MarkList.Add(new KeyValuePairS<string, double>(markListName, markListTime));
            }

            for (int i = 0; i < generateAudioReplyInterop.visemeListNum; i++)
            {
                string visemeListType = Marshal.PtrToStringAnsi(generateAudioReplyInterop.visemeListType.ElementAt<IntPtr>(i));
                double visemeListStart = generateAudioReplyInterop.visemeListStart.ElementAt<double>(i);
                double visemeListArticulation = generateAudioReplyInterop.visemeListArticulation.ElementAt<double>(i);
                generateAudioReplyReturn.VisemeList.Add(new GenerateAudioReplyViseme(visemeListType, visemeListStart, visemeListArticulation));
            }

            return true;
        }
    }
}
