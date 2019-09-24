using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Threading;
using VimeoDownloader.JsClasses;
using Microsoft.Win32;

namespace VimeoDownloader
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            exeFilePath = Assembly.GetEntryAssembly().Location;
            exeDirectory = exeFilePath.Replace("VimeoDownloader.exe", "");

            string fileConfig = exeDirectory + "BaseDirectoryPath.txt";
            if (File.Exists(fileConfig))
            {
                textBoxBaseFolder.Text = File.ReadAllText(fileConfig);
            }

            client = new HttpClient();

            string[] args = Environment.GetCommandLineArgs();
            if (args.Length > 1)
            {
                textBoxInit.Text = args[1].Replace("vimeo-downloader://","");
                download();
            }
        }
        HttpClient client;
        string url;
        string exeDirectory;
        string exeFilePath;
        bool shouldStop = false;
 


        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
            shouldStop = true;

            string fileConfig = exeDirectory +"BaseDirectoryPath.txt";
            if (File.Exists(fileConfig))
            {
                string content = File.ReadAllText( fileConfig);
                if (content != textBoxBaseFolder.Text)
                {
                    File.WriteAllText(fileConfig, textBoxBaseFolder.Text);
                }
            }
            else
            {
                File.WriteAllText(fileConfig, textBoxBaseFolder.Text);
            }
            
        }


        private void buttonInit_Click(object sender, EventArgs e)
        {
            download();
        }

        void download()
        {
            url = textBoxInit.Text;
            new Thread(new ThreadStart(run)).Start();
        }
        void run_close()
        {
            this.Invoke((Action)(() =>
            {
                this.Close();
            }));
        }

        void run_processStarted()
        {
            this.Invoke((Action)(() =>
            {
                buttonInit.Enabled = false;
            }));
        }

        void run_resetForm()
        {
            this.Invoke((Action)(() =>
            {
                buttonInit.Enabled = true;
                progressBar1.Value = 0;
            }));
        }
        void run_updateProgress(int value, int maxValue)
        {
            this.Invoke((Action)(() =>
            {
                progressBar1.Value = value;                
                if (progressBar1.Maximum != maxValue)
                {
                    progressBar1.Maximum = maxValue;
                }
                progressBar1.Refresh();
            }));
        }

        void run()
        {
            try
            {


                HttpResponseMessage response = client.GetAsync(url).Result;
                string startTimestamp = DateTime.Now.ToString().Replace(":", "-");

                if (response.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    string data = ASCIIEncoding.ASCII.GetString(response.Content.ReadAsByteArrayAsync().Result);
                    string fileName = exeDirectory+startTimestamp + "-master.json";

                    File.WriteAllText(fileName, data);
                    container c = JsonConvert.DeserializeObject<container>(data);

                    if (c != null)
                    {
                        int segmentsCount = 0;
                        run_processStarted();
                        video videoSegments = null;
                        audio audioSegments = null;
                        //calculate segments count using audio or video stream
                        if (c.audios != null && c.audios.Length > 0)
                        {
                            audioSegments = c.audios[c.audios.Length - 1];
                            segmentsCount = audioSegments.segments.Length;
                        }
                        if (c.videos != null && c.videos.Length > 0)
                        {
                            videoSegments = c.videos[c.videos.Length - 1];
                            segmentsCount = videoSegments.segments.Length;
                        }

                        if (segmentsCount > 0)
                        {
                            run_updateProgress(0, segmentsCount);

                            int sepIndex = url.IndexOf("/sep/");
                            string baseUrl = url.Substring(0, sepIndex + 5);
                            string videoUrl = baseUrl + video.path + videoSegments.base_url;
                            string audioUrl = baseUrl + audioSegments.base_url.Replace("../", "");

                            Directory.CreateDirectory(exeDirectory + startTimestamp);
                            string directory = exeDirectory + startTimestamp;
                            List<string> audioFilesList = new List<string>();
                            List<string> videoFilesList = new List<string>();
                            // zero segment stored in json file
                            if (audioSegments != null)
                            {
                                string audioFileName = directory+ "\\a[0000].m4s";
                                File.WriteAllBytes(audioFileName, audioSegments.segment0);
                                audioFilesList.Add(audioFileName);
                            }
                            if (videoSegments != null)
                            {
                                string videoFileName = directory + "\\v[0000].m4s";
                                File.WriteAllBytes(videoFileName, videoSegments.segment0);
                                videoFilesList.Add(videoFileName);
                            }

                            HttpClient audioClient = new HttpClient();
                            HttpClient videoClient = new HttpClient();

                            for (int i = 0; i < segmentsCount; i++)
                            {
                                if (shouldStop) return;

                                //for index 0 we have name (and real part) 1.m4s
                                string videoSegmentUrl = videoUrl + videoSegments.segments[i].url;
                                string audioSegmentUrl = audioUrl + videoSegments.segments[i].url;

                                string videoFileName = directory + String.Format("\\v[{0:d4}].m4s", i + 1);
                                string audioFileName = directory + String.Format("\\a[{0:d4}].m4s", i + 1);

                                var vTask = videoClient.GetAsync(videoSegmentUrl);
                                var aTask = audioClient.GetAsync(audioSegmentUrl);

                                byte[] vData = vTask.Result.Content.ReadAsByteArrayAsync().Result;
                                byte[] aData = aTask.Result.Content.ReadAsByteArrayAsync().Result;

                                File.WriteAllBytes(videoFileName, vData);
                                File.WriteAllBytes(audioFileName, aData);

                                audioFilesList.Add(audioFileName);
                                videoFilesList.Add(videoFileName);

                                if (shouldStop) return;
                                run_updateProgress(i, segmentsCount);
                            }
                            string concatAudioFileName = directory + "\\audio.m4s";
                            string concatVideoFileName = directory + "\\video.m4s";
                            //concatenate files
                            using (FileStream fs = File.Create(concatAudioFileName))
                            {
                                for (int i = 0; i < audioFilesList.Count; i++)
                                {
                                    byte[] tmpData = File.ReadAllBytes(audioFilesList[i]);
                                    fs.Write(tmpData, 0, tmpData.Length);
                                    run_updateProgress(i, segmentsCount);
                                }
                            }
                            using (FileStream fs = File.Create(concatVideoFileName))
                            {
                                for (int i = 0; i < videoFilesList.Count; i++)
                                {
                                    byte[] tmpData = File.ReadAllBytes(videoFilesList[i]);
                                    fs.Write(tmpData, 0, tmpData.Length);
                                    run_updateProgress(i, segmentsCount);
                                }
                            }

                            //concatenate video+audio
                            ProcessStartInfo cmd = new ProcessStartInfo(exeDirectory + "ffmpeg.exe",
                                                String.Format("-i \"{0}\\video.m4s\" -i \"{0}\\audio.m4s\" -c copy \"{0}\\output.mp4\"", 
                                                directory));
                            cmd.WorkingDirectory = directory;
                            Process pConcatenate = Process.Start(cmd);

                            //delete temprory files
                            if (checkBoxDeleteTemp.Checked)
                            {
                                for (int i = 0; i < videoFilesList.Count; i++)
                                {
                                    File.Delete(videoFilesList[i]);
                                    File.Delete(audioFilesList[i]);
                                    run_updateProgress(i, segmentsCount);
                                }
                                
                                while (!pConcatenate.HasExited)
                                {
                                    if (shouldStop) return;
                                    Thread.Sleep(100);
                                }
                                File.Delete(concatAudioFileName);
                                File.Delete(concatVideoFileName);
                            }

                            if (checkBoxMove.Checked)
                            {
                                string newDirectory = textBoxBaseFolder.Text + "\\" + textBoxNewFolder.Text;
                                string src = Path.Combine(directory, "output.mp4");
                                string dst = Path.Combine(newDirectory, "output.mp4");
                                Directory.CreateDirectory(newDirectory);
                                File.Move(src, dst);
                                directory = newDirectory;
                            }

                            //open output directory
                            ProcessStartInfo openFolder = new ProcessStartInfo
                            {
                                FileName = "explorer.exe",
                                Arguments = directory
                            };
                            //MessageBox.Show(openFolder.Arguments);
                            Process.Start(openFolder);

                            if (checkBoxClose.Checked)
                            {
                                run_close();
                            }

                        }
                    }
                }
                else
                {
                    MessageBox.Show(response.ToString());
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            if (!shouldStop)
            {
                run_resetForm();
            }

        }

        private void buttonSelectBaseFolder_Click(object sender, EventArgs e)
        {
            using (var folderDialog = new FolderBrowserDialog())
            {
                if (folderDialog.ShowDialog() == DialogResult.OK)
                {
                    textBoxBaseFolder.Text = folderDialog.SelectedPath;
                }
            }
        }

        private void buttonProtocol_Click(object sender, EventArgs e)
        { 
            RegistryKey keyClasses = RegistryKey.OpenBaseKey(RegistryHive.ClassesRoot, RegistryView.Default);
            
            try
            {
                RegistryKey root = keyClasses.OpenSubKey("vimeo-downloader");
                if (root == null)
                {
                    //create
                    root = keyClasses.CreateSubKey("vimeo-downloader");
                    RegistryKey shellKey= root.CreateSubKey("shell");
                    RegistryKey openKey = shellKey.CreateSubKey("open");
                    RegistryKey commandKey=openKey.CreateSubKey("command");
                    commandKey.SetValue("", String.Format("\"{0}\" \"%1\"", exeFilePath));
                    root.SetValue("URL Protocol", "");
                    root.SetValue("", "URL:Vimeo Downloader protocol");
                    labelProtocol.Text = "Successful register vimeo-downloader:// protocol to path " + exeFilePath;
                }
                else
                {
                    //update
                    RegistryKey commandKey = root.OpenSubKey("shell").OpenSubKey("open").OpenSubKey("command", true);
                    commandKey.SetValue("", String.Format("\"{0}\" \"%1\"", exeFilePath));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                keyClasses.Close();
            }
        }
    }
}
