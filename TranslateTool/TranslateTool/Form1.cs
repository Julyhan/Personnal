using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TranslateTool
{
    public partial class Form1 : Form
    {
        private int num = 0;
        private bool fanyi1 = false;
        private bool fanyi2 = false;
        private string first;
        private string second;
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            first = "http://fanyi.baidu.com/translate#ch/" + textBox5.Text + "/";
            second = "http://fanyi.baidu.com/translate#" + textBox5.Text + "/ch/";
            webBrowser1.Navigate(first + richTextBox1.Text);
            button1.Text = "中译英...";
            button1.Enabled = false;
            fanyi1 = true;
        }

        private void webBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            HtmlElementCollection hes = webBrowser1.Document.GetElementsByTagName("p");
            if (fanyi1 == false)
            {
                webBrowser2.Navigate(second + richTextBox2.Text);
                button1.Text = "英译中...";
                fanyi2 = true;
                return;
            }
            num++;
            if (num >= 2)
            {
                string content = "";
                foreach (HtmlElement he in hes[2].All)
                {
                    content += he.InnerHtml;
                }
                //richTextBox2.AppendText(content);
                richTextBox2.Text = content;
                button1.Text = "翻译";
                button1.Enabled = true;
                fanyi1 = false;
                num = 0;
                webBrowser1.Navigate("about:Tabs");
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox3_TextChanged(object sender, EventArgs e)
        {

        }
        private void textBox5_TextChanged(object sender, EventArgs e)
        {

        }

        private void webBrowser2_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            HtmlElementCollection hes = webBrowser2.Document.GetElementsByTagName("p");
            if (fanyi2 == false)
            {
                return;
            }
            num++;
            if (num >= 2)
            {
                string content = "";
                foreach (HtmlElement he in hes[2].All)
                {
                    content += he.InnerHtml;
                }
                //richTextBox2.AppendText(content);
                richTextBox3.Text = content;
                button1.Text = "翻译";
                button1.Enabled = true;
                fanyi2 = false;
                num = 0;
                webBrowser2.Navigate("about:Tabs");
            }
        }



    }
}
