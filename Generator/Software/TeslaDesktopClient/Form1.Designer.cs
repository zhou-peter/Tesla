namespace TeslaDesktopClient
{
    partial class Form1
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.textBoxCom = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.labelLed = new System.Windows.Forms.Label();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.button2 = new System.Windows.Forms.Button();
            this.freqChanger1 = new TeslaDesktopClient.FreqChanger();
            this.SuspendLayout();
            // 
            // textBoxCom
            // 
            this.textBoxCom.Location = new System.Drawing.Point(12, 53);
            this.textBoxCom.Name = "textBoxCom";
            this.textBoxCom.Size = new System.Drawing.Size(51, 20);
            this.textBoxCom.TabIndex = 0;
            this.textBoxCom.Text = "Com3";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(102, 51);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(217, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "Подключиться к железу";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // labelLed
            // 
            this.labelLed.AutoSize = true;
            this.labelLed.Location = new System.Drawing.Point(69, 56);
            this.labelLed.Name = "labelLed";
            this.labelLed.Size = new System.Drawing.Size(22, 13);
            this.labelLed.TabIndex = 2;
            this.labelLed.Text = "[   ]";
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.checkBox1.Location = new System.Drawing.Point(325, 14);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(117, 24);
            this.checkBox1.TabIndex = 3;
            this.checkBox1.Text = "#1 Несущая";
            this.checkBox1.UseVisualStyleBackColor = true;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(102, 15);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(217, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "Подключиться к TeslaCommunication";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // freqChanger1
            // 
            this.freqChanger1.Location = new System.Drawing.Point(12, 116);
            this.freqChanger1.Name = "freqChanger1";
            this.freqChanger1.Size = new System.Drawing.Size(663, 260);
            this.freqChanger1.TabIndex = 13;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(694, 427);
            this.Controls.Add(this.freqChanger1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.labelLed);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.textBoxCom);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxCom;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label labelLed;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button button2;
        private FreqChanger freqChanger1;
    }
}

