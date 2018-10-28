using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Control
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        MainWindowState state;
        SineDrawer drawer;
        public MainWindow()
        {
            InitializeComponent();
            state=MainWindowState.Load();
            this.DataContext = state;
            drawer = new SineDrawer(SineCanvas, state);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            state.Save();
        }

        private void ButtonDraw_Click(object sender, RoutedEventArgs e)
        {
            drawer.Draw();
        }
    }
}
