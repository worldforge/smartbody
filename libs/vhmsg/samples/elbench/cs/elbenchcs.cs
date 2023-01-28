

using System;
using System.Runtime.InteropServices;


namespace elbenchcs
{
    public class Win32Interop
    {
        [DllImport("msvcrt.dll")]
        public static extern int _kbhit();

        [DllImport("winmm.dll", EntryPoint = "timeGetTime")]
        public static extern uint timeGetTime();
    }


    /// <summary>
    /// Summary description for Class1.
    /// </summary>
    public class elbenchcs
    {
        public int numMessagesReceived = 0;
        public int m_testSpecialCases = 0;


        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        public static void Main(string[] args)
        {
            int receiveMode = 0;
            int testSpecialCases = 0;

            if (args.Length > 0)
            {
                receiveMode = Convert.ToInt32(args[0]);
            }

            if (args.Length > 1)
            {
                testSpecialCases = Convert.ToInt32(args[1]);
            }

            elbenchcs e = new elbenchcs();
            e.Run(receiveMode, testSpecialCases);
        }


        public void Run(int receiveMode, int testSpecialCases)
        {
            VHMsg.Client vhmsg;
            using (vhmsg = new VHMsg.Client())
            {
                vhmsg.OpenConnection();

                Console.WriteLine("VHMSG_SERVER: {0}", vhmsg.Server);
                Console.WriteLine("VHMSG_SCOPE: {0}", vhmsg.Scope);

                int NUM_MESSAGES = 20000;

                m_testSpecialCases = testSpecialCases;

                if (receiveMode == 1)
                {
                    vhmsg.MessageEvent += new VHMsg.Client.MessageEventHandler(MessageAction);
                    vhmsg.SubscribeMessage("elbench");

                    Console.WriteLine("Receive Mode");

                    if (testSpecialCases == 1)
                    {
                        Console.WriteLine("Testing special case messages");

                        while (Win32Interop._kbhit() == 0)
                        {
                        }
                    }
                    else
                    {
                        uint timeBefore = 0;
                        uint timeAfter;

                        while (Win32Interop._kbhit() == 0)
                        {
                            // we've received our first message
                            if (numMessagesReceived > 0 && timeBefore == 0)
                            {
                                timeBefore = Win32Interop.timeGetTime();
                            }

                            if (numMessagesReceived >= NUM_MESSAGES)
                            {
                                timeAfter = Win32Interop.timeGetTime();

                                Console.WriteLine("Time to receive {0} messages: {1}", NUM_MESSAGES, timeAfter - timeBefore);

                                numMessagesReceived = 0;
                                timeBefore = 0;
                            }
                        }
                    }
                }
                else
                {
                    Console.WriteLine("Send Mode");

                    if (testSpecialCases == 1)
                    {
                        Console.WriteLine("Testing special case messages");

                        string s;

                        s = "Test Message: 'Single-Quote'";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: \"Double-Quote\"";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: [Bracket]";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: {Brace}";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: \\Backslash\\";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: /Slash/";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: (Parenthesis)";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: <Angled Parens?>";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: `Dunno what these are called`";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);

                        s = "Test Message: \"~!@#$%^&*()_+\" - top row special characters";
                        Console.WriteLine("Sending " + s);
                        vhmsg.SendMessage("elbench " + s);
                    }
                    else
                    {
                        long timeBefore = Win32Interop.timeGetTime();

                        for (int i = 0; i < NUM_MESSAGES; i++)
                        {
                            vhmsg.SendMessage("elbench " + i + " Test Message");

                            if (i % 2000 == 0)
                            {
                                Console.WriteLine(i + " messages sent");
                            }
                        }

                        long timeAfter = Win32Interop.timeGetTime();

                        Console.WriteLine("Time to send {0} messages: {1}", NUM_MESSAGES, timeAfter - timeBefore);
                    }
                }
            }
        }


        private void MessageAction(object sender, VHMsg.Message args)
        {
            //Console.WriteLine( "Received Message '" + e.toString() + "'" );

            //Ict.ElvinUtility eu = (Ict.ElvinUtility)sender;

            if (m_testSpecialCases == 1)
            {
                Console.WriteLine("received - '" + args.s + "'");
            }
            else
            {
                numMessagesReceived++;

                if (numMessagesReceived % 2000 == 0)
                {
                    Console.WriteLine(numMessagesReceived + " messages received - '" + args.s + "'");
                }
            }
        }
    }
}
