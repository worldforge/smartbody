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
using System.Windows.Forms;


namespace elsender
{
   /// <summary>
   /// Summary description for Form1.
   /// </summary>
   public class Form1 : System.Windows.Forms.Form
   {
      public MainClass m_main;
      public System.Windows.Forms.Label label8;
      public System.Windows.Forms.Label label9;
      private System.Windows.Forms.Button button1;
      public System.Windows.Forms.ComboBox comboBox1;
      public System.Windows.Forms.RichTextBox richTextBox1;
      private System.Windows.Forms.Button button2;
      private System.Windows.Forms.CheckBox checkBox1;
      private System.Windows.Forms.Button button3;
      public CheckBox checkBox2;
      public CheckBox checkBox3;
      private CheckBox checkBox4;
      private RichTextBox richTextBox2;
      public CheckBox checkBox5;

      /// <summary>
      /// Required designer variable.
      /// </summary>
      private System.ComponentModel.Container components = null;


      public Form1(MainClass main)
      {
         m_main = main;

         //
         // Required for Windows Form Designer support
         //
         InitializeComponent();


         richTextBox1.Text = "";
         checkBox1.Checked = true;
      }

      /// <summary>
      /// Clean up any resources being used.
      /// </summary>
      protected override void Dispose(bool disposing)
      {
         if (disposing)
         {
            if (components != null)
            {
               components.Dispose();
            }
         }

         base.Dispose(disposing);
      }

      #region Windows Form Designer generated code
      /// <summary>
      /// Required method for Designer support - do not modify
      /// the contents of this method with the code editor.
      /// </summary>
      private void InitializeComponent()
      {
          this.label8 = new System.Windows.Forms.Label();
          this.label9 = new System.Windows.Forms.Label();
          this.button1 = new System.Windows.Forms.Button();
          this.comboBox1 = new System.Windows.Forms.ComboBox();
          this.richTextBox1 = new System.Windows.Forms.RichTextBox();
          this.button2 = new System.Windows.Forms.Button();
          this.checkBox1 = new System.Windows.Forms.CheckBox();
          this.button3 = new System.Windows.Forms.Button();
          this.checkBox2 = new System.Windows.Forms.CheckBox();
          this.checkBox3 = new System.Windows.Forms.CheckBox();
          this.checkBox4 = new System.Windows.Forms.CheckBox();
          this.richTextBox2 = new System.Windows.Forms.RichTextBox();
          this.checkBox5 = new System.Windows.Forms.CheckBox();
          this.SuspendLayout();
          // 
          // label8
          // 
          this.label8.Location = new System.Drawing.Point(0, 0);
          this.label8.Name = "label8";
          this.label8.Size = new System.Drawing.Size(304, 23);
          this.label8.TabIndex = 4;
          this.label8.Text = "label8";
          this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
          // 
          // label9
          // 
          this.label9.Location = new System.Drawing.Point(0, 24);
          this.label9.Name = "label9";
          this.label9.Size = new System.Drawing.Size(304, 16);
          this.label9.TabIndex = 4;
          this.label9.Text = "label9";
          this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
          // 
          // button1
          // 
          this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.button1.Location = new System.Drawing.Point(392, 56);
          this.button1.Name = "button1";
          this.button1.Size = new System.Drawing.Size(40, 20);
          this.button1.TabIndex = 2;
          this.button1.Text = "Send";
          this.button1.Click += new System.EventHandler(this.button1_Click);
          // 
          // comboBox1
          // 
          this.comboBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.comboBox1.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
          this.comboBox1.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
          this.comboBox1.Location = new System.Drawing.Point(8, 56);
          this.comboBox1.Name = "comboBox1";
          this.comboBox1.Size = new System.Drawing.Size(376, 21);
          this.comboBox1.TabIndex = 0;
          this.comboBox1.Text = "comboBox1";
          this.comboBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.comboBox1_KeyPress);
          // 
          // richTextBox1
          // 
          this.richTextBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                      | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.richTextBox1.BackColor = System.Drawing.SystemColors.Control;
          this.richTextBox1.ForeColor = System.Drawing.SystemColors.ControlText;
          this.richTextBox1.Location = new System.Drawing.Point(8, 96);
          this.richTextBox1.Name = "richTextBox1";
          this.richTextBox1.ReadOnly = true;
          this.richTextBox1.Size = new System.Drawing.Size(424, 280);
          this.richTextBox1.TabIndex = 4;
          this.richTextBox1.Text = "richTextBox1";
          this.richTextBox1.LinkClicked += new System.Windows.Forms.LinkClickedEventHandler(this.richTextBox1_LinkClicked);
          // 
          // button2
          // 
          this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.button2.Location = new System.Drawing.Point(392, 80);
          this.button2.Name = "button2";
          this.button2.Size = new System.Drawing.Size(40, 16);
          this.button2.TabIndex = 3;
          this.button2.Text = "clear";
          this.button2.Click += new System.EventHandler(this.button2_Click);
          // 
          // checkBox1
          // 
          this.checkBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.checkBox1.Location = new System.Drawing.Point(336, 32);
          this.checkBox1.Name = "checkBox1";
          this.checkBox1.Size = new System.Drawing.Size(96, 16);
          this.checkBox1.TabIndex = 9;
          this.checkBox1.Text = "autocomplete";
          this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
          // 
          // button3
          // 
          this.button3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.button3.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.button3.Location = new System.Drawing.Point(176, 32);
          this.button3.Name = "button3";
          this.button3.Size = new System.Drawing.Size(80, 16);
          this.button3.TabIndex = 10;
          this.button3.Text = "clear history";
          this.button3.Click += new System.EventHandler(this.button3_Click);
          // 
          // checkBox2
          // 
          this.checkBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.checkBox2.AutoSize = true;
          this.checkBox2.Checked = true;
          this.checkBox2.CheckState = System.Windows.Forms.CheckState.Checked;
          this.checkBox2.Location = new System.Drawing.Point(239, 78);
          this.checkBox2.Name = "checkBox2";
          this.checkBox2.Size = new System.Drawing.Size(66, 17);
          this.checkBox2.TabIndex = 11;
          this.checkBox2.Text = "filter adc";
          this.checkBox2.UseVisualStyleBackColor = true;
          // 
          // checkBox3
          // 
          this.checkBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.checkBox3.AutoSize = true;
          this.checkBox3.Checked = true;
          this.checkBox3.CheckState = System.Windows.Forms.CheckState.Checked;
          this.checkBox3.Location = new System.Drawing.Point(311, 78);
          this.checkBox3.Name = "checkBox3";
          this.checkBox3.Size = new System.Drawing.Size(67, 17);
          this.checkBox3.TabIndex = 12;
          this.checkBox3.Text = "filter wsp";
          this.checkBox3.UseVisualStyleBackColor = true;
          // 
          // checkBox4
          // 
          this.checkBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.checkBox4.Location = new System.Drawing.Point(262, 32);
          this.checkBox4.Name = "checkBox4";
          this.checkBox4.Size = new System.Drawing.Size(68, 18);
          this.checkBox4.TabIndex = 15;
          this.checkBox4.Text = "multiline";
          this.checkBox4.CheckedChanged += new System.EventHandler(this.checkBox4_CheckedChanged);
          // 
          // richTextBox2
          // 
          this.richTextBox2.AcceptsTab = true;
          this.richTextBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.richTextBox2.Location = new System.Drawing.Point(8, 57);
          this.richTextBox2.Name = "richTextBox2";
          this.richTextBox2.Size = new System.Drawing.Size(375, 154);
          this.richTextBox2.TabIndex = 1;
          this.richTextBox2.Text = "";
          this.richTextBox2.Visible = false;
          // 
          // checkBox5
          // 
          this.checkBox5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.checkBox5.AutoSize = true;
          this.checkBox5.Checked = true;
          this.checkBox5.CheckState = System.Windows.Forms.CheckState.Checked;
          this.checkBox5.Location = new System.Drawing.Point(121, 78);
          this.checkBox5.Name = "checkBox5";
          this.checkBox5.Size = new System.Drawing.Size(112, 17);
          this.checkBox5.TabIndex = 16;
          this.checkBox5.Text = "filter sbmdebugger";
          this.checkBox5.UseVisualStyleBackColor = true;
          // 
          // Form1
          // 
          this.AcceptButton = this.button1;
          this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
          this.ClientSize = new System.Drawing.Size(440, 382);
          this.Controls.Add(this.checkBox5);
          this.Controls.Add(this.checkBox4);
          this.Controls.Add(this.checkBox3);
          this.Controls.Add(this.checkBox2);
          this.Controls.Add(this.button3);
          this.Controls.Add(this.checkBox1);
          this.Controls.Add(this.button2);
          this.Controls.Add(this.comboBox1);
          this.Controls.Add(this.button1);
          this.Controls.Add(this.label8);
          this.Controls.Add(this.label9);
          this.Controls.Add(this.richTextBox1);
          this.Controls.Add(this.richTextBox2);
          this.Name = "Form1";
          this.Text = "VHMsg Sender Util C#";
          this.ResumeLayout(false);
          this.PerformLayout();

      }
      #endregion

      private void button1_Click(object sender, System.EventArgs e)
      {
          if (checkBox4.Checked)
          {
              m_main.m_vhmsg.SendMessage(richTextBox2.Text);
          }
          else
          {
            m_main.m_vhmsg.SendMessage(comboBox1.Text);

            if (comboBox1.Items.Count > 0)
            {
                if ((string)comboBox1.Items[0] != comboBox1.Text)
                {
                    comboBox1.Items.Insert(0, comboBox1.Text);
                }
            }
            else
            {
                comboBox1.Items.Insert(0, comboBox1.Text);
            }
          }
      }

      private void comboBox1_KeyPress(object sender, KeyPressEventArgs e)
      {
      }

      private void richTextBox1_LinkClicked(object sender, LinkClickedEventArgs e)
      {
         System.Diagnostics.Process.Start(e.LinkText);
      }

      private void button2_Click(object sender, System.EventArgs e)
      {
         richTextBox1.Clear();
      }

      private void button3_Click(object sender, System.EventArgs e)
      {
          if (checkBox4.Checked)
          {
            richTextBox2.Clear();
          }
          else
          {
            comboBox1.Items.Clear();
          }
      }

      private void checkBox1_CheckedChanged(object sender, EventArgs e)
      {
         if (checkBox1.Checked)
         {
            comboBox1.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox1.AutoCompleteSource = AutoCompleteSource.ListItems;
         }
         else
         {
            comboBox1.AutoCompleteMode = AutoCompleteMode.None;
            comboBox1.AutoCompleteSource = AutoCompleteSource.None;
         }
      }

      private void checkBox4_CheckedChanged(object sender, EventArgs e)
      {
          if (checkBox4.Checked)
          {
              comboBox1.Visible = false;
              richTextBox2.Visible = true;
              int height = richTextBox2.Size.Height - comboBox1.Size.Height;
              richTextBox1.Location = new Point(richTextBox1.Location.X, richTextBox1.Location.Y + height);
              richTextBox1.Size = new Size(richTextBox1.Size.Width, richTextBox1.Size.Height - height);
              checkBox2.Location = new Point(checkBox2.Location.X, checkBox2.Location.Y + height);
              checkBox3.Location = new Point(checkBox3.Location.X, checkBox3.Location.Y + height);
              checkBox5.Location = new Point(checkBox5.Location.X, checkBox5.Location.Y + height);
          }
          else
          {
              comboBox1.Visible = true;
              richTextBox2.Visible = false;
              int height = richTextBox2.Size.Height - comboBox1.Size.Height;
              richTextBox1.Location = new Point(richTextBox1.Location.X, richTextBox1.Location.Y - height);
              richTextBox1.Size = new Size(richTextBox1.Size.Width, richTextBox1.Size.Height + height);
              checkBox2.Location = new Point(checkBox2.Location.X, checkBox2.Location.Y - height);
              checkBox3.Location = new Point(checkBox3.Location.X, checkBox3.Location.Y - height);
              checkBox5.Location = new Point(checkBox5.Location.X, checkBox5.Location.Y - height);
          }
      }
   }
}
