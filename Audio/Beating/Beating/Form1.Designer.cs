namespace Beating
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
            this.channel1 = new Beating.Channel();
            this.channel2 = new Beating.Channel();
            this.SuspendLayout();
            // 
            // channel1
            // 
            this.channel1.Location = new System.Drawing.Point(-23, -46);
            this.channel1.Name = "channel1";
            this.channel1.Size = new System.Drawing.Size(500, 102);
            this.channel1.TabIndex = 0;
            // 
            // channel2
            // 
            this.channel2.Location = new System.Drawing.Point(12, 12);
            this.channel2.Name = "channel2";
            this.channel2.Size = new System.Drawing.Size(485, 44);
            this.channel2.TabIndex = 1;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.channel2);
            this.Controls.Add(this.channel1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private Channel channel1;
        private Channel channel2;
    }
}

