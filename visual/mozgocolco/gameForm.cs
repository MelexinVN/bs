using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;

namespace mozgocolco
{
    
    public partial class gameForm : Form
    {
        public long time_clock = 0;
        public long time_ms = 0;
        public bool visiblity = true;
        public string input_string = " ";
        private MainForm m_owner = null;

        SoundPlayer sp = new SoundPlayer();

        private byte count = 0;

        public MainForm MainForm
        {
            set { m_owner = value; }
        }

        public gameForm()
        {
            InitializeComponent();
        }

        private void gameForm_Load(object sender, EventArgs e)
        {
            byte i = 0;

            sp.SoundLocation = "sounds/answer.wav";
            sp.Load();

            label01.Text = " ";
            label02.Text = " ";
            label03.Text = " ";
            label04.Text = " ";
            label05.Text = " ";
            label06.Text = " ";
            label07.Text = " ";
            label08.Text = " ";
            label09.Text = " ";
            label10.Text = " ";
            label11.Text = " ";
            label12.Text = " ";
            label13.Text = " ";
            label14.Text = " ";
            label15.Text = " ";
            label16.Text = " ";
            label17.Text = " ";
            label18.Text = " ";
            label19.Text = " ";
            label20.Text = " ";

            while (i < 10)
            {
                dataGridView1.Rows.Add();
                i++;
            }

            i = 1;
            while (i <= 10)
            {
                dataGridView1.Rows[i - 1].Cells[0].Value = i;
                dataGridView1.Rows[i - 1].Cells[1].Value = i + 10;
                i++;
            }
            timer2.Enabled = true;
        }

        private void time1Update(object sender, EventArgs e)
        {
            time_clock++;
            byte mins, secs;
            string s_mins, s_secs;
            
            mins = (byte)(time_clock / 60);
            if (mins >= 10)   s_mins = Convert.ToString(mins);
            else              s_mins = '0' + Convert.ToString(mins);

            secs = (byte)(time_clock - mins * 60);
            if (secs >= 10)   s_secs = Convert.ToString(secs);
            else              s_secs = '0' + Convert.ToString(secs);

            label1.Text = s_mins + ':' + s_secs;
        }


        private void button2_Click(object sender, EventArgs e)
        {
            time_ms = 0;            
            if (timer1.Enabled)
            {
                timer1.Enabled = false;
                button2.Text = "Пуск";
            }
            else
            {
                timer1.Enabled = true;
                button2.Text = "Стоп";
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            time_ms++;
            if (input_string[0] != ' ')
            {
                sp.Play();
                for (int i = 0; i < input_string.Length; i++)
                {
                    if (input_string[i] == '0')
                    {
                        label01.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[0].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[0].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    
                    if (input_string[i] == '1')
                    {
                        label02.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[1].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[1].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n"; 
                        count++;
                    }
                    if (input_string[i] == '2')
                    {
                        label03.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[2].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[2].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '3')
                    {
                        label04.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[3].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[3].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '4')
                    {
                        label05.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[4].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[4].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '5')
                    {
                        label06.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[5].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[5].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '6')
                    {
                        label07.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[6].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[6].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '7')
                    {
                        label08.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[7].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[7].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '8')
                    {
                        label09.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[8].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[8].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '9')
                    {
                        label10.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[9].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[9].Cells[0].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'a')
                    {
                        label11.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[0].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[0].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }

                    if (input_string[i] == 'b')
                    {
                        label12.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[1].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[1].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'c')
                    {
                        label13.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[2].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[2].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'd')
                    {
                        label14.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[3].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[3].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'e')
                    {
                        label15.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[4].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[4].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'f')
                    {
                        label16.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[5].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[5].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'g')
                    {
                        label17.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[6].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[6].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'h')
                    {
                        label18.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[7].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[7].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'i')
                    {
                        label19.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[8].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[8].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'j')
                    {
                        label20.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += Convert.ToString(dataGridView1.Rows[9].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        else textBox1.Text += "ФС\t" + Convert.ToString(dataGridView1.Rows[9].Cells[1].Value) + "\t" + Convert.ToString(time_ms) + " мс" + "\r\n";
                        count++;
                    }
                }
                input_string = " ";
            }
        }

        private void Scroll_text(object sender, EventArgs e)
        {
            textBox1.SelectionStart = textBox1.Text.Length;
            textBox1.ScrollToCaret();
        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void dataGridView2_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click_1(object sender, EventArgs e)
        {

        }

        private void gameForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.S)       
            {
                s_make();
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            
        }

        private void s_make()
        {
            label20.Text = "S";
        }
    }
}
