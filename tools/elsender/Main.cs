/*
   This file is part of VHMsg written by Edward Fast at 
   University of Southern California's Institute for Creative Technologies.
   http://www.ict.usc.edu
   Copyright 2008 Edward Fast, University of Southern California

   VHMsg is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VHMsg is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with VHMsg.  If not, see <http://www.gnu.org/licenses/>.
*/

using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;


// TODO: check box to disable vhmsg message receiving


namespace elsender
{
   public class MainClass
   {
      private static bool UsingWindows;

      private delegate void AddTextLineDelegate(RichTextBox textBox, string s);

      private AddTextLineDelegate m_addTextLineDelegate;
      private Form1 m_form;
      public VHMsg.Client m_vhmsg;

      private string m_addLineText = "";
      private Timer m_addLineTimer;



      public MainClass()
      {
          UsingWindows = Environment.OSVersion.Platform != PlatformID.MacOSX
              && Environment.OSVersion.Platform != PlatformID.Unix;
      }


      public void Run()
      {
         bool   runSingleMessage = false;
         string singleMessage = "";

         string[] args = Environment.GetCommandLineArgs();

         for (int i = 1; i < args.Length; i++)
         {
            if (args[i] == "-m" || args[i] == "-M" ||
                args[i] == "/m" || args[i] == "/M")
            {
               if (i + 1 < args.Length)
               {
                  runSingleMessage = true;
                  singleMessage = args[i + 1];
               }
            }
         }

         if (runSingleMessage)
         {
            using (m_vhmsg = new VHMsg.Client())
            {
               m_vhmsg.OpenConnection();

               m_vhmsg.SendMessage(singleMessage);
            }

            return;
         }



         m_addTextLineDelegate = new AddTextLineDelegate(AddLineInvoke);
         m_addLineTimer = new Timer();
         m_addLineTimer.Tick += new EventHandler(AddLineTimerEvent);
         m_addLineTimer.Interval = 1000;
         m_addLineTimer.Start();

         m_form = new Form1(this);


         if (File.Exists("history.txt"))
         {
            StreamReader historyReader = new StreamReader("history.txt");

            string input;
            while ((input = historyReader.ReadLine()) != null)
            {
               m_form.comboBox1.Items.Add(input);
            }

            historyReader.Close();
            historyReader = null;
         }


         if (m_form.comboBox1.Items.Count > 0)
         {
            m_form.comboBox1.Text = (string)m_form.comboBox1.Items[0];
         }
         else
         {
            m_form.comboBox1.Text = "";
         }


         using (m_vhmsg = new VHMsg.Client())
         {
            m_vhmsg.OpenConnection();

            Console.WriteLine("VHMSG_SERVER: {0}", m_vhmsg.Server);
            Console.WriteLine("VHMSG_SCOPE: {0}", m_vhmsg.Scope);

            m_form.label8.Text = string.Format("VHMSG_SERVER: {0}", m_vhmsg.Server);
            m_form.label9.Text = string.Format("VHMSG_SCOPE: {0}", m_vhmsg.Scope);

            m_vhmsg.MessageEvent += new VHMsg.Client.MessageEventHandler(MessageAction);
            m_vhmsg.SubscribeMessage("*");

            m_vhmsg.SendMessage("vrComponent elsender all");


            Application.Run(m_form);


            m_vhmsg.SendMessage("vrProcEnd elsender");

            try
            {
               using (StreamWriter history = new StreamWriter("history.txt"))
               {
                  for (int i = 0; i < m_form.comboBox1.Items.Count; i++)
                  {
                     history.WriteLine((string)m_form.comboBox1.Items[i]);
                  }
               }
            }
            catch (Exception e)
            {
               Console.WriteLine("Error writing to history file.  " + e);
            }
         }
      }


      private void MessageAction(object sender, VHMsg.Message args)
      {
         //Console.WriteLine( "Received Message '" + args.s + "'" );

         string[] splitargs = args.s.Split(" ".ToCharArray());

         if (splitargs.Length > 0)
         {
            if (splitargs[0] == "adc")
            {
               if (!m_form.checkBox2.Checked)
               {
                  AddLineDelay(m_form.richTextBox1, args.s);
               }
            }
            else if (splitargs[0] == "wsp")
            {
               if (!m_form.checkBox3.Checked)
               {
                  AddLineDelay(m_form.richTextBox1, args.s);
               }
            }
            else if (splitargs[0] == "sbmdebugger")
            {
               if (!m_form.checkBox5.Checked)
               {
                  AddLineDelay(m_form.richTextBox1, args.s);
               }
            }
            else
            {
               if (splitargs[0] == "vrAllCall")
               {
                  m_vhmsg.SendMessage("vrComponent elsender all");
               }
               else if (splitargs[0] == "vrKillComponent")
               {
                  if (splitargs.Length > 1)
                  {
                     if (splitargs[1] == "elsender" || splitargs[1] == "all")
                     {
                        Application.Exit();
                     }
                  }
               }

               AddLineDelay(m_form.richTextBox1, args.s);
            }
         }
      }


      private void AddLineDelay(RichTextBox textBox, string s)
      {
         lock (this)
         {
            m_addLineText += s;
            m_addLineText += Environment.NewLine;
            m_addLineText += Environment.NewLine;
         }
      }


      private void AddLine(RichTextBox textBox, string s)
      {
         if (m_form.InvokeRequired)
         {
            IAsyncResult r = m_form.BeginInvoke(m_addTextLineDelegate, new object[] { textBox, s });
         }
         else
         {
            AddLineInvoke(textBox, s);
         }
      }


      private static void AddLineInvoke(RichTextBox textBox, string s)
      {
         bool restore = false;
         int  start   = 0;
         int  length  = 0;
         if (textBox.SelectionStart != textBox.TextLength)
         {
            restore = true;
            start   = textBox.SelectionStart;
            length  = textBox.SelectionLength;
         }


         textBox.AppendText(s);
         //textBox.AppendText( Environment.NewLine );


         if (restore)
         {
            textBox.Select(start, length);
            textBox.ScrollToCaret();
         }
         else if (UsingWindows)
         {
            Win32Interop.SendMessage(new HandleRef(textBox, textBox.Handle), Win32Interop.WM_VSCROLL, new IntPtr(Win32Interop.SB_BOTTOM), new IntPtr(0));
         }

         //Console.WriteLine( s );
      }


      private void AddLineTimerEvent(object myObject, EventArgs myEventArgs)
      {
         lock (this)
         {
            if (!string.IsNullOrEmpty(m_addLineText))
            {
               AddLine(m_form.richTextBox1, m_addLineText);
               m_addLineText = "";
            }
         }
      }


      /// <summary>
      /// The main entry point for the application.
      /// </summary>
      [STAThread]
      public static void Main() 
      {
         MainClass main = new MainClass();
         main.Run();
      }
   }


   public class Win32Interop
   {
      public const int SB_BOTTOM   = 0x07;
      public const int WM_VSCROLL  = 0x0115;


      [DllImport("user32.dll", CharSet = CharSet.Auto)]
      public static extern IntPtr SendMessage(HandleRef hWnd, uint Msg, IntPtr wParam, IntPtr lParam);
   }
}
