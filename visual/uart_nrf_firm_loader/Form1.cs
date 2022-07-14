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

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public int frequency = 140;      //значение частоты по умолчанию
        public UInt16 crcInitValue = 0;
        public bool ready_to_load = false;
        public bool try_to_load = false;
        public long hex_file_size = 0;
        public ushort string_counter = 0;
        public string rec_string;
        public string[] string_buf = new string[ushort.MaxValue];//буфер строк размер 65535. Должно хватить, самый большой hex-файл, который есть на Теплоприборе не более 9000 строк
        public Form1()
        {
            InitializeComponent();
        }
        private void Form1_Load(object sender, EventArgs e)                         //загрузка окна
        {
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
            comboBox1.SelectedIndex = 0;                                            //Выбор COM                    
            serialPort1.PortName = comboBox1.Text;
            comboBox3.SelectedIndex = 3;                           			        //Выбор Bit=8
            serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);
            comboBox2.SelectedIndex = 12;                                           //Выбор BaudRate
            serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);

            if (Directory.Exists("firmwares"))
            {
                string[] firmwares = Directory.GetFiles("firmwares","*.hex");

                foreach (string filename in firmwares)
                {
                    comboBox4.Items.Add(filename);
                }
                if (comboBox4.Items.Count >= 1) comboBox4.SelectedIndex = 0; 
            }
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e) //прием данных в порт
        {
            Invoke(new EventHandler(DoUpdate));                //вызов обработки принятых данных
            //label5.Text = "Принято: ";
        }

        private void DoUpdate(object s, EventArgs e)
        {
            string s_input = "";
            int kol_bytes = serialPort1.BytesToRead;  //количество вновь принятых байтов
            byte[] temp_input = new byte[kol_bytes];
            serialPort1.Read(temp_input, 0, kol_bytes);

            for (int i = 0; i < kol_bytes; i++)
            {
                s_input += Convert.ToChar(temp_input[i]);
            }

            label8.Text = s_input;

            rec_string += s_input;
            if (((rec_string[0] != 0x52) && (rec_string[0] != 0x5A) && (rec_string[0] != 0x41)) || (rec_string.Length >= 12)) rec_string = "";

            if (((rec_string.Length > 1)) && (rec_string[rec_string.Length - 1] == '!'))
            {
                textBox3.Text += rec_string + "\r\n";
                if (try_to_load)            
                {//если получен запрос новой прошивки
                    if ((rec_string.Length >= 3) && ((rec_string[0] == 0x52) && (rec_string[1] == 0x54) && (rec_string[2] == 0x4C))) //если получено "RTL" ready to load
                    {//если программа в режиме попытки загрузить прошивку
                        ready_to_load = true;           //включаем режим готовности загрузки
                        try_to_load = false;
                        label22.Text = "МК готов к загрузке";
                        label22.ForeColor = Color.Green;
                        label21.Text = "0 %";
                    }
                }
                else if (ready_to_load)
                {
                    if ((rec_string.Length >= 8) && ((rec_string[0] == 0x5A) && (rec_string[1] == 0x4E) && (rec_string[2] == 0x53))) //если получено "ZNS" zapros next string
                    {
                        ushort string_number = (ushort)((rec_string[3] - 48) * 10000 + (rec_string[4] - 48) * 1000 + (rec_string[5] - 48) * 100 + (rec_string[6] - 48) * 10 + (rec_string[7] - 48));
                        if (serialPort1.IsOpen)
                        {
                            serialPort1.Write(string_buf[string_number] + ";");           //отправка запрошенной строки
                            label14.Text = string_buf[string_number] + ";";
                            textBox1.Text += string_buf[string_number] + ";" + "\r\n";
                        }
                        if (string_number > string_counter)
                        {
                            try_to_load = false;
                            ready_to_load = false;
                            label22.ForeColor = Color.Black;
                            label22.Text = "   ";
                            label21.Text = "   ";
                            MessageBox.Show("       Загрузка ПО завершена");
                        }
                    }
                    if ((rec_string[0] == 0x41) && (rec_string[1] == 0x4F) && (rec_string[2] == 0x4B)) //если получено "AOK" all OK
                    {
                        try_to_load = false;
                        ready_to_load = false;
                        label22.ForeColor = Color.Black;
                        label22.Text = "   ";
                        label21.Text = "   ";
                        MessageBox.Show("       Загрузка ПО завершена");
                    }
                }
                rec_string = "";
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

        private void button2_Click_1(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)                //если порт открыт
            {
                try
                {
                    serialPort1.Open();
                    label6.Text = "Подключение к порту " + comboBox1.Text;
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
                label6.Text = " ";
                button2.Text = "Открыть";
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
        }

        private void button1_Click(object sender, EventArgs e)
        {//обновить список файлов прошивок
            comboBox4.Items.Clear();

            if (Directory.Exists("firmwares"))
            {
                string[] firmwares = Directory.GetFiles("firmwares", "*.hex");

                foreach (string filename in firmwares)
                {
                    comboBox4.Items.Add(filename);
                }
                if (comboBox4.Items.Count >= 1) comboBox4.SelectedIndex = 0;
            }
        }
        private void button13_Click(object sender, EventArgs e)
        {//при ражатии на кнопку загрузки
            using (FileStream fs = File.Open(comboBox4.SelectedItem.ToString(), FileMode.Open))
            {//открываем файл
                hex_file_size = fs.Length;                  //определяем размер файла
                byte[] f = new byte[hex_file_size];         
                char[] ch = new char[hex_file_size];
                fs.Read(f, 0, f.Length);                    //читаем файл
                string_counter = 0;
                for (int i = 0; i < hex_file_size; i++)
                {
                    ch[i] = Convert.ToChar(f[i]);           //преобразуем каждый байт файла в символы
                    if (ch[i] == ':') string_counter++;     //считаем количество строк
                }
                string[] hex_strings = new string[string_counter];//создаем массив строк
                string temp_string = "";
                string_counter = 0;                
                for (int i = 0; i < hex_file_size; i++)
                {//формируем массив строк
                    temp_string += ch[i];                           //собираем символы во временную строку
                    if ((ch[i] == 0x0A) && (ch[i-1] == 0x0D))       //если конец строки
                    {
                        hex_strings[string_counter] = temp_string;  //записываем очередную строку в массив
                        string_counter++;
                        temp_string = "";
                    }
                }
                for (int i = 0; i < string_counter; i++)
                {
                    string_buf[i] = hex_strings[i];                      //копируем массив строк в буфер
                }
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write(":!!!;");
                    label14.Text = ":!!!;";
                    textBox1.Text += ":!!!;\r\n";
                }
            }
        }

        private void button11_Click(object sender, EventArgs e)
        {
            textBox3.Clear();
            textBox1.Clear();
            label14.Text = "  ";
            label8.Text = "  ";
        }

        private void button14_Click(object sender, EventArgs e)
        {//перезагружаем МК в режим обновления прошивки
            if (!try_to_load)
            {
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write(":!;");
                    label14.Text = ":!;";
                    textBox1.Text += ":!;\r\n";
                }
                try_to_load = true;     //поднимаем флаг попытки загрузить прошивку
                button14.Text = "Отмена загрузки";
            }
            else
            {
                try_to_load = false;
                ready_to_load = false;
                button14.Text = "Запрос загрузки";
            }
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            textBox3.SelectionStart = textBox3.Text.Length;
            textBox3.ScrollToCaret();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            textBox1.SelectionStart = textBox1.Text.Length;
            textBox1.ScrollToCaret();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(":!;");
                label14.Text = ":!;";
                textBox1.Text += ":!;\r\n";
            }
        }
    }
}
