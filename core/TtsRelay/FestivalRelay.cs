
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
    public class FestivalDLL
    {

        #if (!__MonoCS__)
        // [DllImport("msvcrt.dll", EntryPoint="puts")]
        [DllImport("FestivalDLL.dll")]
        public static extern bool FESTIVAL_DLL_Init([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("FestivalDLL.dll")]
        public static extern void FESTIVAL_DLL_SetVisemeMapping([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("FestivalDLL.dll")]
        public static extern bool FESTIVAL_DLL_GenerateAudio([MarshalAs(UnmanagedType.LPStr)] string message, [MarshalAs(UnmanagedType.LPStr)] string outputFilename, [MarshalAs(UnmanagedType.LPStr)] string messageOutputFileName, [MarshalAs(UnmanagedType.LPStr)] string voice, [Out] StringBuilder xmlReplyReturn, ref GenerateAudioReplyInterop generateAudioReplyInterop);

        [DllImport("FestivalDLL.dll")]
        //[return : MarshalAs(UnmanagedType.LPStr)]
        //public static extern string FESTIVAL_DLL_GetFestivalLibDir();

        #else

        [DllImport("libFestivalDLL.dylib")]
        public static extern bool FESTIVAL_DLL_Init([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("libFestivalDLL.dylib")]
        public static extern void FESTIVAL_DLL_SetVisemeMapping([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("libFestivalDLL.dylib")]
        public static extern bool FESTIVAL_DLL_GenerateAudio([MarshalAs(UnmanagedType.LPStr)] string message, [MarshalAs(UnmanagedType.LPStr)] string outputFilename, [MarshalAs(UnmanagedType.LPStr)] string messageOutputFileName, [MarshalAs(UnmanagedType.LPStr)] string voice, [Out] StringBuilder xmlReplyReturn, ref GenerateAudioReplyInterop generateAudioReplyInterop);

        [DllImport("libFestivalDLL.dylib")]
        //[return : MarshalAs(UnmanagedType.LPStr)]
        //public static extern string FESTIVAL_DLL_GetFestivalLibDir();
        //public static extern System.IntPtr FESTIVAL_DLL_GetFestivalLibDir();
        #endif
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
        public static extern System.IntPtr FESTIVAL_DLL_GetFestivalLibDir();
    }

    public class FestivalRelay : ITtsRelay
    {
        public bool Init(string visemeMapping)
        {
            FestivalDLL.FESTIVAL_DLL_Init(visemeMapping);
            return true;
        }


        public string [] GetVoices()
        {
            // We want to query festival to see what voices are installed
            // Apparently this is possible by sending a (voice.list) command to festival
            // But I was unable to get this to work, because (1) the command didn't work for me and
            // (2) if it did work, I'm not sure how I could capture the output to a variable.
            // So we brute force this by enumerating the folder names.
            // I moved this logic to C# because the code is much easier.
            // if voices are outside the /english folder, just duplicate the block below

            //string festivalLibDir = FestivalDLL.FESTIVAL_DLL_GetFestivalLibDir();
            string festivalLibDir = Marshal.PtrToStringAnsi(FestivalDLL.FESTIVAL_DLL_GetFestivalLibDir());
            //string festivalLibDir = "../../lib/festival/festival/lib";

            List<string> list = new List<string>();
            string festivalVoiceDir = festivalLibDir + "/voices/english";
            Console.WriteLine("festivalVoiceDir: {0}", festivalVoiceDir);
            DirectoryInfo voicedi = new DirectoryInfo(festivalVoiceDir);
            if (voicedi.Exists == true)
            {
                Console.WriteLine("festivalVoiceDir is TRUE");
                DirectoryInfo[] voiceNames = voicedi.GetDirectories();

                foreach (DirectoryInfo di in voiceNames)
                {
                    list.Add("Festival_voice_" + di.Name);
                }
            }
            else { Console.WriteLine("festivalVoiceDir is FALSE");}
            return list.ToArray();
        }


        public void SetVisemeMapping(string visemeMapping)
        {
            FestivalDLL.FESTIVAL_DLL_SetVisemeMapping(visemeMapping);
        }


        public bool GenerateAudio(string message, string outputFileName, string messageOutputFileName, string voice, ref string xmlReplyReturn, ref GenerateAudioReply generateAudioReplyReturn)
        {
            Console.WriteLine("Generating Audio...");
            StringBuilder reply = new StringBuilder(65536);

            // The voice names have "Festival_" prepended to identify them, but the real festival names don't have this
            string festivalVoice = voice.Remove(0, "Festival_".Length);

            GenerateAudioReplyInterop generateAudioReplyInterop = new GenerateAudioReplyInterop();

            FestivalDLL.FESTIVAL_DLL_GenerateAudio(message, outputFileName, messageOutputFileName, festivalVoice, reply, ref generateAudioReplyInterop);
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
