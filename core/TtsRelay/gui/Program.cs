
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml;


namespace TtsRelayGui
{
    /// <summary>
    /// Main program
    /// </summary>
    class Program
    {
        /// <summary>
        ///Timer variable
        /// </summary>
        private int timer_millisec;

        /// <summary>
        /// // FPS Limiting value.
        /// </summary>
        private const int fps_limit = 100;

        /// <summary>
        /// Message handling server
        /// </summary>
        private static VHMsg.Client vhmsg;

        /// <summary>
        /// Identifies this program on the message system and to other components in the VH toolkit
        /// </summary>
        private string programName = "msspeechrelay";

        private bool isRunning = true;

        private TtsRelay.TtsRelayComponent speechRelayComponent;

        private string m_visemeMapping = "sbm";
        private string m_cacheFolder = "../../../../../data/cache/audio";

        /// <summary>
        /// No constructor level initialization needed
        /// </summary>
        public Program()
        {
        }

        /// <summary>
        /// Message Handling Code:
        /// Callback for every message, will direct program flow
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">Variable containg the message params and the actual message</param>
        private void MessageHandler(object sender, VHMsg.Message e)
        {
            string[] splitargs = e.s.Split(" ".ToCharArray());

            if (splitargs[0] == "vrAllCall")
            {
                vhmsg.SendMessage("vrComponent tts " + programName);
            }
            else if (splitargs[0] == "vrKillComponent")
            {
                if (splitargs.Length > 1)
                {
                    if (splitargs[1] == "tts" || splitargs[1] == "all")
                    {
                        vhmsg.SendMessage("vrProcEnd tts " + programName);
                        isRunning = false;
                    }
                }
            }
            else if (splitargs[0] == "TTSRelayCacheClear")
            {
                Console.WriteLine("----- Cache Clear Begins -----");
                if (speechRelayComponent != null)
                {
                    speechRelayComponent.ClearCache();
                }

                System.IO.DirectoryInfo di = new DirectoryInfo(m_cacheFolder);
                int NumFilesDeleted  = 0;
                int NumFolderDeleted = 0;

                foreach (FileInfo file in di.GetFiles())
                {
                    file.Delete();
                    NumFilesDeleted++;
                }
                foreach (DirectoryInfo dir in di.GetDirectories())
                {
                    dir.Delete(true);
                    NumFolderDeleted++;
                }
                Console.WriteLine("Cache Number Files   Removed: " + NumFilesDeleted);
                Console.WriteLine("Cache Number Folders Removed: " + NumFolderDeleted);
                Console.WriteLine("----- Cache Clear Ends -----");
            }
            speechRelayComponent.OnVHMessage(sender, e);
        }

        private void ParseArguments(List<string> args)
        {
            for (int i = 0; i < args.Count; i++)
            {
                string s = args[i];
                if (s == "-h" || s == "--help")
                {
                    Console.WriteLine("Usage: TtsRelayGui <options>");
                    Console.WriteLine("\t-h, --help:\tDisplay this help menu");
                    Console.WriteLine("\t-mapping <mapping>:\tViseme mapping mode. Use 'sbmold' (simple mapping of visemes, low quality), 'sbm' (diphone-based mapping, medium quality) or 'facefx' (commercial algorithm, medium quality), default is 'sbm'.");

                    isRunning = false;
                }
                else if (s == "-mapping")
                {
                    if ((i + 1) < args.Count)
                    {
                        m_visemeMapping = args[i + 1];
                        i++;
                    }
                }
                else if (s == "-cachePath")
                {
                    m_cacheFolder = args[i + 1];
                    i++;
                }
                else
                {
                    Console.WriteLine("Error: Unknown argument: \"" + s + "\" provided. See -h/--help for command options");
                }
            }
        }

        /// <summary>
        /// Initialize the program
        /// </summary>
        /// <param name="args">Add any arguments to the function in this variable, remember the position of the arguments though</param>
        public void InitializeAndRun(List<string> args)
        {
            ParseArguments(args);

            using (vhmsg = new VHMsg.Client())
            {
                vhmsg.OpenConnection();
                Console.WriteLine( "VHMSG_SERVER: {0}", vhmsg.Server );
                Console.WriteLine( "VHMSG_SCOPE: {0}", vhmsg.Scope );

                /// We only need Remote Speech commands
                vhmsg.SubscribeMessage("RemoteSpeechCmd");
                vhmsg.SubscribeMessage("TTSRelay");
                vhmsg.SubscribeMessage("vrKillComponent");
                vhmsg.SubscribeMessage("vrAllCall");
                vhmsg.SubscribeMessage("TTSRelayCacheClear");
                vhmsg.MessageEvent += new VHMsg.Client.MessageEventHandler(MessageHandler);

                vhmsg.SendMessage("vrComponent tts " + programName);


                speechRelayComponent = new TtsRelay.TtsRelayComponent();

                Console.WriteLine("Setting viseme mapping mode to: {0}", m_visemeMapping);

                speechRelayComponent.SetVisemeMapping(m_visemeMapping);
                speechRelayComponent.SetCacheFolderPath(m_cacheFolder);

                bool ret = speechRelayComponent.Init();
                if (!ret)
                {
                    Console.WriteLine("TtsSpeechRelay Init() failed");
                }


                ret = speechRelayComponent.RegisterVHMsg(vhmsg);
                if (!ret)
                {
                    Console.WriteLine("TtsSpeechRelay RegisterVHMsg() failed");
                }


                // initializing timer value
                timer_millisec = System.DateTime.Now.Millisecond;

                while (isRunning)
                {
                    System.Windows.Forms.Application.DoEvents();

                    // code to limit the CPU usage as otherwise it takes up the complete CPU
                    // Currently limiting to 60 frames per second. Might not be accurately 60 due to granularity issues.
                    int timesincelastframe = System.DateTime.Now.Millisecond - timer_millisec;

                    int ttW;
                    ttW = (1000 / fps_limit) - timesincelastframe;
                    if (ttW > 0)
                        Thread.Sleep(ttW);

                    timer_millisec = System.DateTime.Now.Millisecond;
                }

                speechRelayComponent.Close();
            }
        }

        /// <summary>
        /// Our main function
        /// </summary>
        /// <param name="args"></param>
        [STAThread]
        static void Main(string[] args)
        {
            List<string> commandLineArguments = new List<string>(args);

            Program main = new Program();
            main.InitializeAndRun(commandLineArguments);
        }
    }
}
