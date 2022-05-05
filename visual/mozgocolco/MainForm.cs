using System;
using System.IO;
using System.IO.Ports;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;
using System.Net;
using System.Resources;
using System.Reflection;
using System.Media;

public enum BaudRate
{
    [Description("2400")]
    Baud2400 = 0,
    [Description("4800")]
    Baud4800 = 1,
    [Description("9600")]
    Baud9600 = 2,
    [Description("14400")]
    Baud14400 = 3,
    [Description("19200")]
    Baud19200 = 4,
    [Description("38400")]
    Baud38400 = 6,
    [Description("57600")]
    Baud57600 = 7,
    [Description("115200")]
    Baud115200 = 8
}

public enum Parity
{
    [Description("Отсутствует")]
    None = 0,
    [Description("Чет")]
    Even = 1,
    [Description("Нечет")]
    Odd = 2
}

public enum DataBits
{
    [Description("7")]
    Seven = 0,
    [Description("8")]
    Eight = 1
}

public enum StopBits
{
    [Description("1")]
    One = 0,
    [Description("2")]
    Two = 2
}

public enum ConverterType
{
    Auto = 1,
    SemiAuto = 0
}

public enum ConnectionError
{
    Ok = 0,

    InvalidArgument = -1,
    PortNotOpened = -2,
    PortError = -5,
    Io = -100,
    Format = -101,
    Timeout = -102,
    InvalidCrc = -103,
    NErr = -104,
    Exception = -105,
    InvalidPacket = -106,
    SensorFailure = -300
}


namespace mozgocolco
{
    public partial class MainForm : Form
    {
        private gameForm gameForm1 = new gameForm();



        public MainForm()
        {
            InitializeComponent();

            gameForm1.MainForm = this;
            gameForm1.Hide();

        }

        private void Form1_Load_1(object sender, EventArgs e)
        {
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
            comboBox1.SelectedIndex = 0;                                            //Выбор COM                    
            serialPort1.PortName = comboBox1.Text;
            comboBox3.SelectedIndex = 3;                           			        //Выбор Bit=8
            serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);
            comboBox2.SelectedIndex = 12;                                           //Выбор BaudRate
            serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);

            gameForm1.KeyPreview = true;

            //sp.SoundLocation = "sounds/answer.wav";
            //sp.Load();
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            Invoke(new EventHandler(DoUpdate));
        }
        private void DoUpdate(object s, EventArgs e)
        {
            string s_input = serialPort1.ReadLine();
            textBox2.Text += s_input + "\r\n";
        }

        private void Scroll_text(object sender, EventArgs e)
        {
            textBox2.SelectionStart = textBox2.Text.Length;            
            textBox2.ScrollToCaret();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)                //если порт открыт
            {
                try
                {
                    serialPort1.Open();
                    button2.Text = "Закрыть";
                }
                catch
                {
                    MessageBox.Show("       Ошибка подключения порта" + "\r\n" +
                                    "                             " + comboBox1.Text + "\r\n" +
                                    "(Порт занят или прибор не включен)");
                }
            }
            else
            {
                serialPort1.Close();
                button2.Text = "Открыть";
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.PortName = comboBox1.Text;                              //выбор порта для подключения
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);             //выбор скорости обмена
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);             //выбор количества бит
        }

        private void button8_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(textBox1.Text); //отправляем команду в порт
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            textBox2.Clear();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
                gameForm1.Show();
            else
                gameForm1.Hide();
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            gameForm1.visiblity = false;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //sp.Play();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            //sp.Dispose();            
            //sp.SoundLocation = "sounds/" + comboBox5.Text;
            //sp.Load();
        }
    }
}
