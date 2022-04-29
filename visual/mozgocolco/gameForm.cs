using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace mozgocolco
{
    
    public partial class gameForm : Form
    {
        public long time_clock = 0;
        public bool visiblity = true;

        private MainForm m_owner = null;

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

        }

        private void time1Update(object sender, EventArgs e)
        {
            byte mins, secs, msecs;
            string s_mins, s_secs, s_msecs;


            time_clock++;
            mins = (byte)(time_clock / 6000);
            if (mins >= 10)   s_mins = Convert.ToString(mins);
            else              s_mins = '0' + Convert.ToString(mins);

            secs = (byte)(time_clock / 100 - mins * 60);
            if (secs >= 10)   s_secs = Convert.ToString(secs);
            else              s_secs = '0' + Convert.ToString(secs);

            msecs = (byte)(time_clock - secs * 100 - mins * 6000);
            if (msecs >= 10)  s_msecs = Convert.ToString(msecs);
            else              s_msecs = '0' + Convert.ToString(msecs);

            label1.Text = s_mins + ':' + s_secs + '.' + s_msecs;
        }


        private void button1_Click(object sender, EventArgs e)
        {
            if (timer1.Enabled)
            {
                timer1.Enabled = false;
                button1.Text = "Старт";
            }
            else
            {
                timer1.Enabled = true;
                button1.Text = "Стоп";
            }
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            pictureBox1.Visible = visiblity;
        }
    }
}
