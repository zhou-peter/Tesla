namespace VimeoDownloader
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
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
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxInit = new System.Windows.Forms.TextBox();
            this.buttonInit = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.checkBoxDeleteTemp = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.checkBoxClose = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonSelectBaseFolder = new System.Windows.Forms.Button();
            this.textBoxBaseFolder = new System.Windows.Forms.TextBox();
            this.textBoxNewFolder = new System.Windows.Forms.TextBox();
            this.checkBoxMove = new System.Windows.Forms.CheckBox();
            this.buttonProtocol = new System.Windows.Forms.Button();
            this.labelProtocol = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(681, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "https://skyfire.vimeocdn.com/152311234-0xfba9a58a011a778baaf1/2000123/sep/video/7" +
    "63123,763124,763125/master.json?base64_init=1";
            // 
            // textBoxInit
            // 
            this.textBoxInit.Location = new System.Drawing.Point(15, 33);
            this.textBoxInit.Name = "textBoxInit";
            this.textBoxInit.Size = new System.Drawing.Size(981, 20);
            this.textBoxInit.TabIndex = 1;
            // 
            // buttonInit
            // 
            this.buttonInit.Location = new System.Drawing.Point(877, 4);
            this.buttonInit.Name = "buttonInit";
            this.buttonInit.Size = new System.Drawing.Size(119, 23);
            this.buttonInit.TabIndex = 2;
            this.buttonInit.Text = "Download";
            this.buttonInit.UseVisualStyleBackColor = true;
            this.buttonInit.Click += new System.EventHandler(this.buttonInit_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(15, 61);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(981, 20);
            this.progressBar1.TabIndex = 3;
            // 
            // checkBoxDeleteTemp
            // 
            this.checkBoxDeleteTemp.AutoSize = true;
            this.checkBoxDeleteTemp.Checked = true;
            this.checkBoxDeleteTemp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxDeleteTemp.Location = new System.Drawing.Point(15, 87);
            this.checkBoxDeleteTemp.Name = "checkBoxDeleteTemp";
            this.checkBoxDeleteTemp.Size = new System.Drawing.Size(196, 17);
            this.checkBoxDeleteTemp.TabIndex = 5;
            this.checkBoxDeleteTemp.Text = "Delete temprory files when complete";
            this.checkBoxDeleteTemp.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.checkBoxClose);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.buttonSelectBaseFolder);
            this.groupBox1.Controls.Add(this.textBoxBaseFolder);
            this.groupBox1.Controls.Add(this.textBoxNewFolder);
            this.groupBox1.Controls.Add(this.checkBoxMove);
            this.groupBox1.Location = new System.Drawing.Point(15, 110);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(981, 101);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "When download complete";
            // 
            // checkBoxClose
            // 
            this.checkBoxClose.AutoSize = true;
            this.checkBoxClose.Location = new System.Drawing.Point(6, 68);
            this.checkBoxClose.Name = "checkBoxClose";
            this.checkBoxClose.Size = new System.Drawing.Size(144, 17);
            this.checkBoxClose.TabIndex = 11;
            this.checkBoxClose.Text = "Close Vimeo Downloader";
            this.checkBoxClose.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "In base folder";
            // 
            // buttonSelectBaseFolder
            // 
            this.buttonSelectBaseFolder.Location = new System.Drawing.Point(856, 40);
            this.buttonSelectBaseFolder.Name = "buttonSelectBaseFolder";
            this.buttonSelectBaseFolder.Size = new System.Drawing.Size(119, 23);
            this.buttonSelectBaseFolder.TabIndex = 9;
            this.buttonSelectBaseFolder.Text = "Select base folder";
            this.buttonSelectBaseFolder.UseVisualStyleBackColor = true;
            this.buttonSelectBaseFolder.Click += new System.EventHandler(this.buttonSelectBaseFolder_Click);
            // 
            // textBoxBaseFolder
            // 
            this.textBoxBaseFolder.Location = new System.Drawing.Point(83, 42);
            this.textBoxBaseFolder.Name = "textBoxBaseFolder";
            this.textBoxBaseFolder.Size = new System.Drawing.Size(766, 20);
            this.textBoxBaseFolder.TabIndex = 8;
            // 
            // textBoxNewFolder
            // 
            this.textBoxNewFolder.Location = new System.Drawing.Point(178, 17);
            this.textBoxNewFolder.Name = "textBoxNewFolder";
            this.textBoxNewFolder.Size = new System.Drawing.Size(90, 20);
            this.textBoxNewFolder.TabIndex = 7;
            // 
            // checkBoxMove
            // 
            this.checkBoxMove.AutoSize = true;
            this.checkBoxMove.Location = new System.Drawing.Point(6, 19);
            this.checkBoxMove.Name = "checkBoxMove";
            this.checkBoxMove.Size = new System.Drawing.Size(166, 17);
            this.checkBoxMove.TabIndex = 6;
            this.checkBoxMove.Text = "Move output file to new folder";
            this.checkBoxMove.UseVisualStyleBackColor = true;
            // 
            // buttonProtocol
            // 
            this.buttonProtocol.Location = new System.Drawing.Point(15, 234);
            this.buttonProtocol.Name = "buttonProtocol";
            this.buttonProtocol.Size = new System.Drawing.Size(243, 23);
            this.buttonProtocol.TabIndex = 12;
            this.buttonProtocol.Text = "Register vimeo-downloader:// protocol";
            this.buttonProtocol.UseVisualStyleBackColor = true;
            this.buttonProtocol.Click += new System.EventHandler(this.buttonProtocol_Click);
            // 
            // labelProtocol
            // 
            this.labelProtocol.AutoSize = true;
            this.labelProtocol.Location = new System.Drawing.Point(282, 239);
            this.labelProtocol.Name = "labelProtocol";
            this.labelProtocol.Size = new System.Drawing.Size(314, 13);
            this.labelProtocol.TabIndex = 13;
            this.labelProtocol.Text = "<- Run as Administrator. It is required to press one time after unzip";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1010, 268);
            this.Controls.Add(this.labelProtocol);
            this.Controls.Add(this.buttonProtocol);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.checkBoxDeleteTemp);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.buttonInit);
            this.Controls.Add(this.textBoxInit);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Vimeo Video Downloader (sinushkin_alexey@mail.ru) v.1.1";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxInit;
        private System.Windows.Forms.Button buttonInit;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.CheckBox checkBoxDeleteTemp;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox checkBoxClose;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonSelectBaseFolder;
        private System.Windows.Forms.TextBox textBoxBaseFolder;
        private System.Windows.Forms.TextBox textBoxNewFolder;
        private System.Windows.Forms.CheckBox checkBoxMove;
        private System.Windows.Forms.Button buttonProtocol;
        private System.Windows.Forms.Label labelProtocol;
    }
}

