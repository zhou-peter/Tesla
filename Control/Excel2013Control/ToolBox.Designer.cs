namespace Excel2013Control
{
    partial class ToolBox : Microsoft.Office.Tools.Ribbon.RibbonBase
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public ToolBox()
            : base(Globals.Factory.GetRibbonFactory())
        {
            InitializeComponent();
        }

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

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tab1 = this.Factory.CreateRibbonTab();
            this.group1 = this.Factory.CreateRibbonGroup();
            this.buttonService = this.Factory.CreateRibbonButton();
            this.buttonPort = this.Factory.CreateRibbonButton();
            this.buttonConfig = this.Factory.CreateRibbonButton();
            this.separator1 = this.Factory.CreateRibbonSeparator();
            this.checkBox1 = this.Factory.CreateRibbonCheckBox();
            this.checkBox2 = this.Factory.CreateRibbonCheckBox();
            this.checkBox3 = this.Factory.CreateRibbonCheckBox();
            this.separator2 = this.Factory.CreateRibbonSeparator();
            this.checkBox4 = this.Factory.CreateRibbonCheckBox();
            this.checkBox5 = this.Factory.CreateRibbonCheckBox();
            this.checkBox6 = this.Factory.CreateRibbonCheckBox();
            this.labelBlink = this.Factory.CreateRibbonLabel();
            this.tab1.SuspendLayout();
            this.group1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tab1
            // 
            this.tab1.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office;
            this.tab1.Groups.Add(this.group1);
            this.tab1.Label = "TabAddIns";
            this.tab1.Name = "tab1";
            // 
            // group1
            // 
            this.group1.Items.Add(this.buttonService);
            this.group1.Items.Add(this.buttonPort);
            this.group1.Items.Add(this.buttonConfig);
            this.group1.Items.Add(this.separator1);
            this.group1.Items.Add(this.checkBox1);
            this.group1.Items.Add(this.checkBox2);
            this.group1.Items.Add(this.checkBox3);
            this.group1.Items.Add(this.separator2);
            this.group1.Items.Add(this.checkBox4);
            this.group1.Items.Add(this.checkBox5);
            this.group1.Items.Add(this.checkBox6);
            this.group1.Items.Add(this.labelBlink);
            this.group1.Label = "TeslaCommunication";
            this.group1.Name = "group1";
            // 
            // buttonService
            // 
            this.buttonService.Label = "Подключиться к службе";
            this.buttonService.Name = "buttonService";
            this.buttonService.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.buttonService_Click);
            // 
            // buttonPort
            // 
            this.buttonPort.Label = "Открыть порт";
            this.buttonPort.Name = "buttonPort";
            this.buttonPort.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.buttonPort_Click);
            // 
            // buttonConfig
            // 
            this.buttonConfig.Label = "Отправить конфигурацию";
            this.buttonConfig.Name = "buttonConfig";
            this.buttonConfig.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.buttonConfig_Click);
            // 
            // separator1
            // 
            this.separator1.Name = "separator1";
            // 
            // checkBox1
            // 
            this.checkBox1.Label = "1 Несущая";
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox1_Click);
            // 
            // checkBox2
            // 
            this.checkBox2.Label = "2 Пачки";
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox2_Click);
            // 
            // checkBox3
            // 
            this.checkBox3.Label = "3 Проломы";
            this.checkBox3.Name = "checkBox3";
            this.checkBox3.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox3_Click);
            // 
            // separator2
            // 
            this.separator2.Name = "separator2";
            // 
            // checkBox4
            // 
            this.checkBox4.Label = "4 Отступ проломов";
            this.checkBox4.Name = "checkBox4";
            this.checkBox4.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox4_Click);
            // 
            // checkBox5
            // 
            this.checkBox5.Label = "5 Пропуск верхнего плеча";
            this.checkBox5.Name = "checkBox5";
            this.checkBox5.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox5_Click);
            // 
            // checkBox6
            // 
            this.checkBox6.Label = "6 Пропуск нижнего плеча";
            this.checkBox6.Name = "checkBox6";
            this.checkBox6.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler(this.checkBox6_Click);
            // 
            // labelBlink
            // 
            this.labelBlink.Label = "[ ● ]";
            this.labelBlink.Name = "labelBlink";
            // 
            // ToolBox
            // 
            this.Name = "ToolBox";
            this.RibbonType = "Microsoft.Excel.Workbook";
            this.Tabs.Add(this.tab1);
            this.Load += new Microsoft.Office.Tools.Ribbon.RibbonUIEventHandler(this.ToolBox_Load);
            this.tab1.ResumeLayout(false);
            this.tab1.PerformLayout();
            this.group1.ResumeLayout(false);
            this.group1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab tab1;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup group1;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton buttonService;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton buttonPort;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton buttonConfig;
        internal Microsoft.Office.Tools.Ribbon.RibbonSeparator separator1;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox1;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox2;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox3;
        internal Microsoft.Office.Tools.Ribbon.RibbonSeparator separator2;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox4;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox5;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox checkBox6;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel labelBlink;
    }

    partial class ThisRibbonCollection
    {
        internal ToolBox ToolBox
        {
            get { return this.GetRibbon<ToolBox>(); }
        }
    }
}
