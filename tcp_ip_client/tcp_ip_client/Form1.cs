/*
prg adı: arfesan_omur_test
tarih: 21.02.2016
yazan: Remzi Şahinoğlu

kalite departmanındaki soğuk test cihazları için yazılmış olup
arfesan yerel ağa bağlanmıştır. arduino ya yazılmış olan test
programına değişken yükleyip monitörleme yapmaya yarar
*/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;


namespace tcp_ip_client
{
    public partial class Form1 : Form
    {        
        public string test_tipi_no = "0";       // test tipinin ne olduğunu belirtir

        public string[] testdata = { "","","","","","","","","","","","","" };         // test bilgilerinin gönderilmeden önce yazıldığı yer

        public Form1()
        {                    
            InitializeComponent();

        }

        private void Form1_Load(object sender, EventArgs e)
        {           
            // test tiplerini comboboxa ekle
            comboTestTipi.Items.Add("servis aç-kapa");
            comboTestTipi.Items.Add("imdat aç-kapa");
            comboTestTipi.Items.Add("servis aç-kapa + imdat full açık");
            comboTestTipi.Items.Add("imdat aç-kapa + servis full açık");
            comboTestTipi.Items.Add("servis + imdat aç-kapa");
            comboTestTipi.Items.Add("mercedes servis");
            comboTestTipi.Items.Add("mercedes imdat");
            // yeni testler ekleyeceksen burdan devam et
            comboTestTipi.SelectedIndex = 0;

            txt_ikinci_ic_dongu.Enabled = false;        // döngüleri girmeyi pasif et
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            MessageBox.Show("her komutta bağlantı kurulup, data gönderilip sonra bağlantı sonlandırıldığından bu butona bir atama yapılmamıştır.");
        }

        private async void btnSend_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));                 // buradaki ip farklı olabilir elle girilmelidir
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();
                byte[] data = System.Text.Encoding.ASCII.GetBytes(txtSendData.Text);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                for (int t = 0; t < 3; t++) { await Delay(); }  // datanın alınmasını 0,3 sn bekle 100ms çarpan

                if (tcpclnt.Available > 0)
                {
                    // gelen datayı oku
                    data = new Byte[256];                                                       // Buffer to store the response bytes
                    String responseData = String.Empty;                                         // String to store the response ASCII representation
                    Int32 bytes = stm.Read(data, 0, data.Length);                               // Read the first batch of the TcpServer response bytes
                    responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                    stm.Close();                                                                // Close everything
                    tcpclnt.Close();

                    txtRecievedData.Text += responseData;
                    txtRecievedData.Text += "\r\n";
                }
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            MessageBox.Show("her komutta bağlantı kurulup, data gönderilip sonra bağlantı sonlandırıldığından bu butona bir atama yapılmamıştır.");
        }

        private void chk_ilk_ic_dongu_kodu_CheckedChanged(object sender, EventArgs e)
        {
            if (chk_ilk_ic_dongu_kodu.Checked == true)   // iç döngü kodu check boxı aktif ise mimi diyagramı aç
            {               
                txt_ilk_ic_dongu.Enabled = true;         // döngüleri girmeyi aktif et
            }
            else
            {               
                txt_ilk_ic_dongu.Enabled = false;        // döngüleri girmeyi pasif et
            }
        }

        private void chk_ikinci_ic_dongu_kodu_CheckedChanged_1(object sender, EventArgs e)
        {
            if (chk_ikinci_ic_dongu_kodu.Checked == true)   // iç döngü kodu check boxı aktif ise
            {
                txt_ikinci_ic_dongu.Enabled = true;         // döngüleri girmeyi aktif et
            }
            else
            {
                txt_ikinci_ic_dongu.Enabled = false;        // döngüleri girmeyi pasif et
            }
        }

        private void btnPeriyodHesapla_Click(object sender, EventArgs e)
        {
            try
            {
                labelimdatPeriyodSuresi.Text = Convert.ToString(Convert.ToInt32(txtimdatBaslamaSaniyesi.Text) + Convert.ToInt32(txtimdatBosaltmaSuresi.Text) + Convert.ToInt32(txtimdatDoldurmaSuresi.Text));
                labelServisPeriyodSuresi.Text = Convert.ToString(Convert.ToInt32(txtServisBaslamaSaniyesi.Text) + Convert.ToInt32(txtServisBosaltmaSuresi.Text) + Convert.ToInt32(txtServisDoldurmaSuresi.Text));

                labelPeriyod.Text = Convert.ToString(60 / Convert.ToInt32(labelServisPeriyodSuresi.Text));
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + ". Tam sayı kullan, harf yazma!");
            }
        }

        private void btnToplamCevrimHesapla_Click(object sender, EventArgs e)
        {
            // çevrim atamalarında toplam çevrimler txt box lara girilip yazıldığında ekranın sağ alt köşesinde toplam çevrim adedini göster
            int a = 0;
            int b = 0;
            int c = 0;

            try
            {
                a = Convert.ToInt32(txt_DisDongu.Text);

                if (chk_ilk_ic_dongu_kodu.Checked == true)
                {
                    b = Convert.ToInt32(txt_ilk_ic_dongu.Text);
                }
                else
                {
                    b = 0;
                }
                if (chk_ikinci_ic_dongu_kodu.Checked == true)
                {
                    c = Convert.ToInt32(txt_ikinci_ic_dongu.Text);
                }
                else
                {
                    c = 0;
                }

                txtToplamCevrimHesapla.Text = Convert.ToString(a * (b + c));
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + ". Boşluk bırakma, harf yazma!");
            }
        }

        private void comboTestTipi_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "servis aç-kapa")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = true;
                txtServisBosaltmaSuresi.Enabled = true;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = false;
                txtimdatBosaltmaSuresi.Enabled = false;

                test_tipi_no = "1";
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "imdat aç-kapa")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = false;
                txtServisBosaltmaSuresi.Enabled = false;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = true;
                txtimdatBosaltmaSuresi.Enabled = true;

                test_tipi_no = "2";
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "servis aç-kapa + imdat full açık")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = true;
                txtServisBosaltmaSuresi.Enabled = true;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = false;
                txtimdatBosaltmaSuresi.Enabled = false;

                test_tipi_no = "3";
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "imdat aç-kapa + servis full açık")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = false;
                txtServisBosaltmaSuresi.Enabled = false;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = true;
                txtimdatBosaltmaSuresi.Enabled = true;

                test_tipi_no = "4";
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "servis + imdat aç-kapa")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = true;
                txtServisBosaltmaSuresi.Enabled = true;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = false;
                txtimdatBosaltmaSuresi.Enabled = false;

                test_tipi_no = "5";
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "mercedes servis")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = false;
                txtServisBosaltmaSuresi.Enabled = false;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = false;
                txtimdatBosaltmaSuresi.Enabled = false;

                txtServisBaslamaSaniyesi.Text = "0";
                txtServisDoldurmaSuresi.Text = "4";
                txtServisBosaltmaSuresi.Text = "2";

                txtimdatBaslamaSaniyesi.Text = "0";
                txtimdatDoldurmaSuresi.Text = "2";
                txtimdatBosaltmaSuresi.Text = "2";

                test_tipi_no = "6";

                MessageBox.Show("bu test özel olduğu için algoritma test süreleri istasyon devrelerine gömülmüştür.");
            }

            if (comboTestTipi.GetItemText(comboTestTipi.SelectedItem) == "mercedes imdat")
            {
                txtServisBaslamaSaniyesi.Enabled = false;
                txtServisDoldurmaSuresi.Enabled = false;
                txtServisBosaltmaSuresi.Enabled = false;

                txtimdatBaslamaSaniyesi.Enabled = false;
                txtimdatDoldurmaSuresi.Enabled = false;
                txtimdatBosaltmaSuresi.Enabled = false;

                txtServisBaslamaSaniyesi.Text = "2";
                txtServisDoldurmaSuresi.Text = "2";
                txtServisBosaltmaSuresi.Text = "2";

                txtimdatBaslamaSaniyesi.Text = "4";
                txtimdatDoldurmaSuresi.Text = "2";
                txtimdatBosaltmaSuresi.Text = "0";

                test_tipi_no = "7";

                MessageBox.Show("bu test özel olduğu için algoritma test süreleri istasyon devrelerine gömülmüştür.");
            }
        }

        private async void btnTestYukle_Click(object sender, EventArgs e)
        {
            try
            {
                txtDurum.Text = "gönderiliyor";

                //testdata[0] = idno;           // bu arduinonun içine yazılıdır
                testdata[1] = test_tipi_no;
                testdata[2] = Convert.ToString(Convert.ToInt32(txtServisDoldurmaSuresi.Text) * 1000);
                testdata[3] = Convert.ToString(Convert.ToInt32(txtServisBosaltmaSuresi.Text) * 1000);
                testdata[4] = Convert.ToString(Convert.ToInt32(txtimdatDoldurmaSuresi.Text) * 1000);
                testdata[5] = Convert.ToString(Convert.ToInt32(txtimdatBosaltmaSuresi.Text) * 1000);
                testdata[6] = txt_Baslama_Cevrimi.Text;
                if (chk_ilk_ic_dongu_kodu.Checked == true) { testdata[7] = "1"; }else { testdata[7] = "0"; }
                if (chk_ikinci_ic_dongu_kodu.Checked == true) { testdata[8] = "1"; } else { testdata[8] = "0"; }
                testdata[9] = txt_DisDongu.Text;
                testdata[10] = txt_ilk_ic_dongu.Text;
                testdata[11] = txt_ikinci_ic_dongu.Text;
                testdata[12] = "recete alindi";

                // bağlantıyı kur
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                for ( int i = 1; i < 12; i++ )          // 0. register idno yu gösteriyor
                {
                    string modbus_paket = ":1;4;" + i + ";" + testdata[i] + ";";
                    byte[] data = asen.GetBytes(modbus_paket);          // modbus protokolü şeklinde gönderilecek
                    stm.Write(data, 0, data.Length);
                    stm.Flush();

                    for (int t = 0; t < 2; t++) { await Delay(); }      // datanın alınmasını 0,2 sn bekle 100ms çarpan
                }
               
                stm.Close();
                tcpclnt.Close();
                txtDurum.Text = "gönderildi";
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnTestSil_Click(object sender, EventArgs e)
        {
            try
            {
                DialogResult testsil = MessageBox.Show("İstasyondaki veriler silinecek! Emin misin?", "", MessageBoxButtons.YesNo);
                if (testsil == DialogResult.Yes)
                {
                    TcpClient tcpclnt = new TcpClient();
                    tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                    Stream stm = tcpclnt.GetStream();
                    ASCIIEncoding asen = new ASCIIEncoding();

                    string modbus_paket = ":1;4;15;1;";                 // teset reset = 1, startı 0 e çekersen reset gelir
                    byte[] data = asen.GetBytes(modbus_paket);          // modbus protokolü şeklinde gönderilecek
                    stm.Write(data, 0, data.Length);
                    stm.Flush();

                    stm.Close();
                    tcpclnt.Close();
                }
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private async void btnHepsiniGuncelle_Click_1(object sender, EventArgs e)
        {
            try
            {         
                // bağlantıyı kur
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                // toplam çevrimi oku ve listviewe yaz
                string modbus_paket = ":1;3;19;";                     // toplam cevrimi okumak için modbus komutunu gönder
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);                                                    // gönder
                stm.Flush();                                                                        // gönderene kadar bekle

                for (int t = 0; t < 2; t++) { await Delay(); }                                      // datanın alınmasını 0,2 sn bekle 100ms çarpan

                // gelen datayı oku
                data = new Byte[256];                                                       // Buffer to store the response bytes
                String responseData = String.Empty;                                         // String to store the response ASCII representation
                Int32 bytes = stm.Read(data, 0, data.Length);                               // Read the first batch of the TcpServer response bytes
                responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);

                txtYapilanCevrim.Text = responseData;                                           // çevrim bilgisini ekrana yaz

                // durum bilgisini oku ve listview e yaz
                modbus_paket = ":1;3;12;";                            // durum bilgisini okumak için modbus komutunu gönder
                data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);                                                    // gönder
                stm.Flush();

                for (int t = 0; t < 2; t++) { await Delay(); }                                      // datanın alınmasını 0,2 sn bekle 100ms çarpan

                // gelen datayı oku
                data = new Byte[256];                                                       // Buffer to store the response bytes
                responseData = String.Empty;                                                // String to store the response ASCII representation
                bytes = stm.Read(data, 0, data.Length);                                     // Read the first batch of the TcpServer response bytes
                responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);

                txtDurum.Text = responseData;                          // durum bilgisini ekrana yaz

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnAcilStop_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;14;1;";   // pause registerini 1 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnDevamEt_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;14;0;";   // pause registerini 0 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();

            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnHepsiniBaslat_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;13;1;";   // start registerini 1 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        async Task Delay()
        {
            await Task.Delay(100);          // 100 ms saniye gecikme yap, bu komutun kullanılacağı fonklarından async void olması gerekir
        }

        private void btnServiseHavaVer_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;16;1;";             // servise hava ver registerını 1 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnServiseHavaBosalt_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;16;0;";             // servise hava ver registerını 0 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnServise2HavaVer_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;18;1;";             // servise2 hava ver registerını 1 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnServise2HavaBosalt_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;18;0;";             // servise2 hava ver registerını 0 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnImdatHavaVer_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;17;1;";             // imdat hava ver registerını 1 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

        private void btnImdatHavaBosalt_Click(object sender, EventArgs e)
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                tcpclnt.Connect(txtIPno.Text, Convert.ToInt32(txtPortNo.Text));
                Stream stm = tcpclnt.GetStream();
                ASCIIEncoding asen = new ASCIIEncoding();

                string modbus_paket = ":1;4;17;0;";             // imdat hava ver registerını 0 yap
                byte[] data = asen.GetBytes(modbus_paket);
                stm.Write(data, 0, data.Length);
                stm.Flush();

                stm.Close();
                tcpclnt.Close();
            }
            catch (Exception hata)
            {
                MessageBox.Show(hata.Message + "\r\n" + hata.StackTrace);
            }
        }

 
    }
}
