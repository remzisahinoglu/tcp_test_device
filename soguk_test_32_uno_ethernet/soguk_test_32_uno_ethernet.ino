/*
prg adı: soguk_test_32_uno_ethernet
board: arduino uno r3
tarih: 01.07.2016
yazan: Remzi ŞAHİNOĞLU

bu program soğuk testi cihazı için networkden kontrol edilmesi için
yazılmıştır.

*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };                      // istenilen bir numara yazılabilir ama diğer cihazlarla çakışmaması gerek
IPAddress ip(192,168,120,202);                                            // laptopa bağlanacaksa onun ki ile çakışmayacak bir numara
IPAddress gateway(192,168,120,1);                                         // varsayılan ağ geçidi, laptop direk bağlanacaksa aynı, modeme bağlanacaksa modeminki ile aynı 
IPAddress subnet(255, 255, 255, 0);                       
IPAddress myDns(111,112,113,94);                                          // bu internete bağlanmak için gerekli

EthernetServer server(23);                                                // telnet defaults to port 23

#include "LedControl.h"
LedControl lc = LedControl(5,2,3,1);                                      // Arduino Pins: (DIN:10, CLK:12, LOAD/CS:11, no.of devices is 1)

//-------------------------- İSTASYON NUMARASI ---------------------------//
String idno = "1";                                                        // cihazın idnosu

//------------------------------ ÇIKIŞLAR --------------------------------//

const int imdat_valf = 6;                                                 // servis kaçak testi için havayı kitleme valfi (normalde kapalı valf kullanılacak)
const int servis_valf = 7;                                                // servis selenoid valfi çıkışı (normalde açık valf kullanılacak)(dijital çıkış)
const int servis_valf_2 = 8;                                              // imdat selenoid valfi çıkışı (normalde açık valf kullanılacak)

//---------------------------- DEĞİŞKENLER -------------------------------//
String durum = "enerji kesildi";                                          // istasyonun durum bilgisini
unsigned int test_tipi_no = 0;                                            // test tipinin ne olduğunu temsil eder
unsigned int test_start = 0;                                              // test bittiğinde 0 olur ve test durur, start komutu pc den gelir
unsigned int test_pause = 0;                                              // test pause yapılmak istendiğinde 1 olur, komut pc den gelir
unsigned int test_reset = 0;                                              // test reset için bu 1 yapılır
unsigned int servise_hava_ver = 0;                                        // servise hava verilme komutu geldiğinde 1 olur
unsigned int imdata_hava_ver = 0;                                         // imdata hava verilme komutu geldiğinde 1 olur
unsigned int servise_hava_ver_2 = 0;                                      // kilit servise hava verilme komutu geldiğinde 1 olur

//---------------------- PERİYOD ZAMAN ATAMALARI -------------------------//
unsigned int servis_doldurma_sn = 0;                                      // servise havanın verili durma süresi
unsigned int servis_bosaltma_sn = 0;                                      // serviseteki havanın boşaltma süresi

unsigned int imdat_doldurma_sn = 0;
unsigned int imdat_bosaltma_sn = 0;

//-------------------------- ÇEVRİM ATAMALARI ----------------------------//
unsigned long toplam_cevrim = 0;                                          // toplam çevrim sayısı
unsigned long baslama_cevrimi = 0;                                        // teste başlama çevrimi, normalde 0

unsigned int ic_dongu_1_kod = 0;                                          // 1 = döngünün aktif olduğunu
unsigned int ic_dongu_2_kod = 0;                                          // 0 = döngünün pasif olduğunu gösterir

unsigned long ic_dongu_1 = 0;                                             // birinci adım iç çevrim adedi sayıcısı
unsigned long ic_hedef_dongu_1 = 0;                                       // yapması istenilen hedef çevrim adedi

unsigned long ic_dongu_2 = 0;                                             // ikinci adım iç çevrim adedi
unsigned long ic_hedef_dongu_2 = 0;                                       // yapması istenilen hedef çevrim adedi

unsigned long dis_dongu_1 = 0;                                            // dış döngü çevrim adedi sayıcısı
unsigned long dis_cevrim_hedef_1 = 0;                                     // yapması istenilen hedef çevrim adedi

//--------------------------- EKRANDA GÖSTER -----------------------------//
void display_set()
{
  char ekran[8] = {};                                                     // max7219 modulunde 8 adet dipslay olduğu için 
  int index = 0;                                                          // max7219u ayarlamak için kullanıldı

  for (int i = 0; i < String(toplam_cevrim).length(); i++)
  {
    char inChar = String(toplam_cevrim)[i];                         // tek bir byte oku
    switch(index)
    {
      case 0:                 // 1.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ' ';       // değer yoksa display boş gözüksün
        ekran[4] = ' ';       // değer yoksa display boş gözüksün
        ekran[3] = ' ';       // değer yoksa display boş gözüksün
        ekran[2] = ' ';       // değer yoksa display boş gözüksün
        ekran[1] = ' ';       // değer yoksa display boş gözüksün
        ekran[0] = inChar;
        break;
        
      case 1:                 // 2.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ' ';       // değer yoksa display boş gözüksün
        ekran[4] = ' ';       // değer yoksa display boş gözüksün
        ekran[3] = ' ';       // değer yoksa display boş gözüksün
        ekran[2] = ' ';       // değer yoksa display boş gözüksün
        ekran[1] = ekran[0];  // 1.okunan katakteri 2.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 2:                 // 3.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ' ';       // değer yoksa display boş gözüksün
        ekran[4] = ' ';       // değer yoksa display boş gözüksün
        ekran[3] = ' ';       // değer yoksa display boş gözüksün
        ekran[2] = ekran[1];  // 1.okunan katakteri 3.displaye kaydır
        ekran[1] = ekran[0];  // 2.okunan katakteri 2.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 3:                 // 4.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ' ';       // değer yoksa display boş gözüksün
        ekran[4] = ' ';       // değer yoksa display boş gözüksün
        ekran[3] = ekran[2];  // 1.okunan katakteri 4.displaye kaydır
        ekran[2] = ekran[1];  // 2.okunan katakteri 3.displaye kaydır
        ekran[1] = ekran[0];  // 3.okunan katakteri 2.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 4:                 // 5.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ' ';       // değer yoksa display boş gözüksün
        ekran[4] = ekran[3];  // 1.okunan katakteri 4.displaye kaydır
        ekran[3] = ekran[2];  // 2.okunan katakteri 3.displaye kaydır
        ekran[2] = ekran[1];  // 3.okunan katakteri 2.displaye kaydır
        ekran[1] = ekran[0];  // 4.okunan katakteri 1.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 5:                 // 6.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ' ';       // değer yoksa display boş gözüksün
        ekran[5] = ekran[4];  // 1.okunan katakteri 5.displaye kaydır
        ekran[4] = ekran[3];  // 2.okunan katakteri 4.displaye kaydır
        ekran[3] = ekran[2];  // 3.okunan katakteri 3.displaye kaydır
        ekran[2] = ekran[1];  // 4.okunan katakteri 2.displaye kaydır
        ekran[1] = ekran[0];  // 5.okunan katakteri 1.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 6:                 // 7.karakter okunduğunda
        ekran[7] = ' ';       // değer yoksa display boş gözüksün
        ekran[6] = ekran[5];  // 1.okunan katakteri 6.displaye kaydır
        ekran[5] = ekran[4];  // 2.okunan katakteri 5.displaye kaydır
        ekran[4] = ekran[3];  // 3.okunan katakteri 4.displaye kaydır
        ekran[3] = ekran[2];  // 4.okunan katakteri 3.displaye kaydır
        ekran[2] = ekran[1];  // 5.okunan katakteri 2.displaye kaydır
        ekran[1] = ekran[0];  // 6.okunan katakteri 1.displaye kaydır
        ekran[0] = inChar;
        break;
        
      case 7:                 // 8.karakter okunduğunda
        ekran[7] = ekran[6];  // 1.okunan katakteri 7.displaye kaydır
        ekran[6] = ekran[5];  // 2.okunan katakteri 6.displaye kaydır
        ekran[5] = ekran[4];  // 3.okunan katakteri 5.displaye kaydır
        ekran[4] = ekran[3];  // 4.okunan katakteri 4.displaye kaydır
        ekran[3] = ekran[2];  // 5.okunan katakteri 3.displaye kaydır
        ekran[2] = ekran[1];  // 6.okunan katakteri 2.displaye kaydır
        ekran[1] = ekran[0];  // 7.okunan katakteri 1.displaye kaydır
        ekran[0] = inChar;
        break;
    }
    index++;
  }

  for (int i = 0; i < String(toplam_cevrim).length(); i++)                // çevrim uzunluğu kadar döngü yap, çünkü o kadar display yansın
  {
    lc.setDigit(0,i,(String(ekran[i])).toInt(),false);                    // (grup no, digit no, data, point) eğer true ise point yanar
  }
}

//-------------------------- BİR ÇEVRİM ALGORİTMASI ----------------------//
void periyod_run()
{
  // algoritmanın anlaşılması için pinout_variables.xlsx dosyasına bak
  switch(test_tipi_no)
  {
    case 1: // servis aç-kapa
      digitalWrite(servis_valf, HIGH);      // servise hava verildi
      digitalWrite(imdat_valf, LOW);        // imdattaki hava boşaltıldı
      bekle(servis_doldurma_sn);
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      bekle(servis_bosaltma_sn);
      break;
      
    case 2: // imdat aç-kapa
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(imdat_doldurma_sn);
      digitalWrite(imdat_valf, LOW);        // imdattaki hava boşaltıldı
      bekle(imdat_bosaltma_sn);
      break;
      
    case 3: // servis aç-kapa + imdat full açık
      digitalWrite(servis_valf, HIGH);      // servise hava verildi
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(servis_doldurma_sn);
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      bekle(servis_bosaltma_sn);
      break;
      
    case 4: // imdat aç-kapa + servis full açık
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(imdat_doldurma_sn);
      digitalWrite(imdat_valf, LOW);        // imdattaki hava boşaltıldı
      bekle(imdat_bosaltma_sn);
      break;
      
    case 5: // servis + imdat aç-kapa
      digitalWrite(servis_valf, HIGH);      // servise hava verildi
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(servis_doldurma_sn);
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, LOW);        // imdattaki hava boşaltıldı
      bekle(servis_bosaltma_sn);
      break;
      
    case 6: // mercedes servis 7. adım
      digitalWrite(servis_valf, LOW);       // servise hava boşaltıldı
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(2000);     // sn bekle
      digitalWrite(servis_valf, HIGH);      // servise hava verildi
      digitalWrite(imdat_valf, HIGH);       // imdattaki hava boşaltıldı
      bekle(2000);     // sn bekle
      digitalWrite(servis_valf, HIGH);      // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, LOW);        // imdata hava verildi
      bekle(2000);     // sn bekle
      break;
      
    case 7: // mercedes imdat 7. adım
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, HIGH);       // imdata hava verildi
      bekle(2000);     // sn bekle
      digitalWrite(servis_valf, LOW);       // servise hava verildi
      digitalWrite(imdat_valf, LOW);        // imdattaki hava boşaltıldı
      bekle(2000);     // sn bekle
      digitalWrite(servis_valf, HIGH);      // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, LOW);        // imdata hava verildi
      bekle(2000);     // sn bekle
      break;

    case 8: // scania testi
      digitalWrite(servis_valf, HIGH);      // servisteki hava boşaltıldı
      digitalWrite(imdat_valf, LOW);        // imdata hava verildi
      bekle(servis_doldurma_sn);
      digitalWrite(servis_valf, LOW);       // servisteki hava boşaltıldı
      bekle(servis_bosaltma_sn);
      break;
  }
}

//----------------------- BİR ÇEVRİM ALGORİTMASI-2 -----------------------//
void periyod_run_2()
{
  digitalWrite(servis_valf_2, HIGH);        // servisteki hava boşaltıldı
  digitalWrite(imdat_valf, LOW);            // imdata hava verildi
  bekle(servis_doldurma_sn);
  digitalWrite(servis_valf_2, LOW);         // servisteki hava boşaltıldı
  bekle(servis_bosaltma_sn);
}

//----------------------- GECİKME ALT PROGRAMI ---------------------------//
void bekle(int time)
{
  for(int t = 1; t <= time; t++)
  {
    delay(1);                                                             // 1 ms bekeleme alt programı
    komut_oku();
    if(test_start == 0){return;}                                          // döngüyü kır, test başlangıcına dön, reset gibi işlem yap
    while(test_pause == 1)                                                // test pause geldiğinde valfleri sıfırla ve burda kal
    {
      valfleri_sifirla(); 
      manual_kontrol();
      komut_oku();
    }
  }
}

//--------------------------------- RESET --------------------------------//
void resetle()
{
  durum = "resetlendi";

  test_start = 0;
  test_pause = 0;
  toplam_cevrim = 0;
  
  lc.clearDisplay(0);                                                     // clear display register
  
  digitalWrite(servis_valf, LOW);                                         // ilk önce havayı boşalt sonra kilit valfler sürekli çalışmasın diye low a çek
  digitalWrite(imdat_valf, LOW);
  digitalWrite(servis_valf_2, LOW);
}

//--------------------------- TEST BİTTİĞİNDE ----------------------------//
void valfleri_sifirla()
{
  durum = "beklemede";
  
  digitalWrite(servis_valf, LOW);                                         // pause komutlarında burası çalışacak
  digitalWrite(imdat_valf, LOW);
  digitalWrite(servis_valf_2, LOW);
}

//--------------------------- MANUAL KONTROL -----------------------------//
void manual_kontrol()
{
  if(servise_hava_ver == 1){ digitalWrite(servis_valf, HIGH); }else{ digitalWrite(servis_valf, LOW); }        // servis valfini manual olarak aç-kapa yap
  if(imdata_hava_ver == 1){ digitalWrite(imdat_valf, HIGH); }else{ digitalWrite(imdat_valf, LOW); }           // imdat valfini manual olarak aç-kapa yap
  if(servise_hava_ver_2 == 1){ digitalWrite(servis_valf_2, HIGH); }else{ digitalWrite(servis_valf_2, LOW); }  // servis kilit valfini manual olarak aç-kapa yap

  if(test_reset == 1)                                                     // test reset komutu geldiğinde
  {
    test_reset = 0;
    resetle();
  }
}

//-------------------------- KOMUT GELDİĞİNDE ----------------------------//
void komut_oku()
{
  EthernetClient client = server.available();                             // wait for a new client:
  
  if (client.available() > 0)                                         // serialdan bir data geldiğinde burası bir olur
  {
    String inputString = "";                                          // sıfırlanması gerek
    String paket[4] = {};                                             // modbus protokolündeki paketlerin tutulduğu yer
    int kirpmaislemi = 0;
    
    while (client.available())                                        // paketi okuma işlemine başla
    {
      delay(1);                                                       // eğer bu gecikmeyi yapmazsak buffer dolmadan okuma yapmadan çıkıyor
      char inChar = client.read();
      inputString += inChar;
      
      if (inChar == ':')                                              // kolon karakteri, komutun başlangıcı
      {
        kirpmaislemi = 1;
        inputString = "";                                             // eğer bilgi gereksiz geldi ise sıfırla
      }
    }
    
    if (kirpmaislemi == 1)                                            // kırpma işlemine başla
    {  
      kirpmaislemi = 0;                                               // tekrardan kırpma işlemi yapılmasın diye
      
      int part = 0;
      String bilgi = "";
      
      for (int i = 0; i < inputString.length(); i++)                  // ayıklama işlemine başla
      {
        char ch = inputString[i];
      
        if (ch == ';')                                                // kolon karakteri, komutun başlangıcı
        {
          if(part < 5)                                                // 4 paket olduğu için başka paketleri görmezden gel
          {
            paket[part] = bilgi;                                      // paketi yaz
            part++;                                                   // paketin bir sonraki partına geç
            bilgi = "";                                               // yeniden üzerine yazılmaması için temizlenmeli
          }
        }
        else
        {
          bilgi += ch;                                                // paketi ayıklarken karakterler birleştir
        }      
      }
      
      part = 0;
      bilgi = "";   
      inputString = "";                                               // sıfırlanması gerek
    }
    
    if (paket[0] == idno)                                             // idno doğru ise gelen komutu işleme alınsın
    {
      if (paket[1] == "3")                                            // OKUMA KOMUTU geldiğinde
      {
        String data_gonder = "";                                      // okuma komutu geldiğinde datayı gonder
        switch (paket[2].toInt())                                     // istenilen adrese gelen paketi oku, gönder
        {
          case 0: data_gonder = idno; break;                          // idno yu gönder
          case 1: data_gonder = test_tipi_no; break;                  // test tipinin ne olduğunu temsil eder
          case 2: data_gonder = servis_doldurma_sn; break;            // doldurma süresi
          case 3: data_gonder = servis_bosaltma_sn; break;            // boşaltma süresi
          case 4: data_gonder = imdat_doldurma_sn; break;
          case 5: data_gonder = imdat_bosaltma_sn; break;
          case 6: data_gonder = baslama_cevrimi; break;               // int türüne çevrildi fakat long değişkenine atandı çünkü int türü 32768 e kadar yüklenebilir fazlası için long olması gerek
          case 7: data_gonder = ic_dongu_1_kod; break;                // ilk adımın iç döngü kod değerini oku int türüne çevir ilgili değişkene ata
          case 8: data_gonder = ic_dongu_2_kod; break;                // ikinci adımın iç döngü kod değeri
          case 9: data_gonder = dis_cevrim_hedef_1; break;            // dış çevrim adedi
          case 10: data_gonder = ic_hedef_dongu_1; break;             // ilk adımın iç çevrim adedi
          case 11: data_gonder = ic_hedef_dongu_2; break;             // ikinci adımın iç çevrim adedi
          case 12: data_gonder = durum; break;
          case 13: data_gonder = test_start; break;
          case 14: data_gonder = test_pause; break;
          case 15: data_gonder = test_reset; break;
          case 16: data_gonder = servise_hava_ver; break;             // servise hava ver
          case 17: data_gonder = imdata_hava_ver; break;              // imdata hava ver
          case 18: data_gonder = servise_hava_ver_2; break;
          case 19: data_gonder = toplam_cevrim; break;
        }
        
        client.print(data_gonder);                                    // datayı gönder
      }
      
      if (paket[1] == "4")                                            // YAZMA KOMUTU geldiğinde
      {
        switch (paket[2].toInt())                                     // istenilen adrese gelen paketi yükle
        {
          case 0: break;
          case 1: test_tipi_no = paket[3].toInt(); break;                 // test tipinin ne olduğunu temsil eder
          case 2: servis_doldurma_sn = paket[3].toInt(); break;           // doldurma süresi
          case 3: servis_bosaltma_sn = paket[3].toInt(); break;           // boşaltma süresi
          case 4: imdat_doldurma_sn = paket[3].toInt(); break;
          case 5: imdat_bosaltma_sn = paket[3].toInt(); break;
          case 6: baslama_cevrimi = paket[3].toInt(); break;              // int türüne çevrildi fakat long değişkenine atandı çünkü int türü 32768 e kadar yüklenebilir fazlası için long olması gerek
          case 7: ic_dongu_1_kod = paket[3].toInt(); break;               // ilk adımın iç döngü kod değerini oku int türüne çevir ilgili değişkene ata
          case 8: ic_dongu_2_kod = paket[3].toInt(); break;               // ikinci adımın iç döngü kod değeri
          case 9: dis_cevrim_hedef_1 = paket[3].toInt(); break;           // dış çevrim adedi
          case 10: ic_hedef_dongu_1 = paket[3].toInt(); break;            // ilk adımın iç çevrim adedi
          case 11: ic_hedef_dongu_2 = paket[3].toInt(); break;            // ikinci adımın iç çevrim adedi
          case 12: durum = paket[3].toInt(); break;
          case 13: test_start = paket[3].toInt(); break;
          case 14: test_pause = paket[3].toInt(); break;
          case 15: test_reset = paket[3].toInt(); break;
          case 16: servise_hava_ver = paket[3].toInt(); break;            // servise hava ver
          case 17: imdata_hava_ver = paket[3].toInt(); break;             // imdata hava ver
          case 18: servise_hava_ver_2 = paket[3].toInt(); break;
          case 19: toplam_cevrim = paket[3].toInt(); break;
        }
      }
    }
    
    paket[0] = "";                                                        // id numarasını sil
    paket[1] = "";                                                        // function kodu sil
    paket[2] = "";                                                        // adres bilgisini sil
    paket[3] = "";                                                        // datayı sil
  }
}

//------------------------- BAŞLANGIÇ AYARLARI ---------------------------//
void setup()
{
  //Serial.begin(9600);
  //delay(100);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);                        // initialize the ethernet device
  server.begin();                                                         // start listening for clients
  
  lc.shutdown(0,false);                                                   // Enable display
  lc.setIntensity(0,3);                                                   // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);                                                     // Clear display register
  
  // analog girişler için atama yapılmasına gerek yok
  // digital giriş ve çıkışlar
  pinMode(servis_valf, OUTPUT);
  pinMode(imdat_valf, OUTPUT);
  pinMode(servis_valf_2, OUTPUT);

  digitalWrite(servis_valf, LOW);                                         // çıkışları sıfırla
  digitalWrite(imdat_valf, LOW);
  digitalWrite(servis_valf_2, LOW);

}

//----------------------------- ANA PROGRAM ------------------------------//
void loop()
{   
  d1:                                                                                    // boşta işlemci çok fazla işlem yapıp ısınmasın diye
  komut_oku();                                                                           // pc den gelen komutları oku
  
  if(test_start == 1)                                                                    // testi başlat/bitir, bittiğinde bu değer 0 olur, otomatik mod
  {
    durum = "calisiyor";
    toplam_cevrim = baslama_cevrimi;                                                     // başlama çevrimini toplam çevrime atayarak burdan devam et
    
    for(dis_dongu_1 = 0; dis_dongu_1 < dis_cevrim_hedef_1; dis_dongu_1++)                // dış döngünün başladığı yer
    {
      if(ic_dongu_1_kod == 1)                                                            // iç döngü 1 adım aktif ise
      {
        for(ic_dongu_1 = 0; ic_dongu_1 < ic_hedef_dongu_1; ic_dongu_1++)                 // iç döngü 1. adım
        {
          periyod_run();                                                                 // bir çevrim algoritmasını işlet; ilk adım          
          toplam_cevrim++;                                                               // toplam çevrim sayısını bir arttır
          display_set();                                                                 // max7219 modulunu ayarla 
          
          komut_oku();                                                                   // pc den komut geldiğinde
          if(test_reset == 1){resetle(); goto d1;}                                       // reset komutunda oransal sıfırlanacak valfler de körükteki havayı boşaltacak
          while(test_pause == 1)                                                         // pause komutunda oransal sıfırlanmayacak sadece valfler körükteki havayı boşaltacak
          {
            valfleri_sifirla();
            manual_kontrol();
            komut_oku();
          }
          durum = "calisiyor";
        }
      }
      
      if(ic_dongu_2_kod == 1)                                                            // iç döngü 2 adım aktif ise
      {
        for(ic_dongu_2 = 0; ic_dongu_2 < ic_hedef_dongu_2; ic_dongu_2++)                 // iç döngü 2. adım    
        {
          periyod_run_2();                                                               // bir çevrim algoritmasını işlet; ikinci adım
          toplam_cevrim++;                                                               // toplam çevrim sayısını bir arttır
          display_set();                                                                 // max7219 modulunu ayarla 
          
          komut_oku();                                                                   // pc den komut geldiğinde
          if(test_start == 0){resetle(); goto d1;}                                       // reset komutunda oransal sıfırlanacak valfler de körükteki havayı boşaltacak
          while(test_pause == 1)                                                         // pause komutunda oransal sıfırlanmayacak sadece valfler körükteki havayı boşaltacak
          {
            valfleri_sifirla();
            manual_kontrol();
            komut_oku();
          }
          durum = "calisiyor";
        }
      }
    }
    
    test_start = 0;                                                                      // testi bitirmek ve döngüye tekrar girmesin diye 0 yap
    durum = "test bitti";
    digitalWrite(servis_valf, LOW);
    digitalWrite(imdat_valf, LOW);
    digitalWrite(servis_valf_2, LOW);
  }
  else                                                                                   // test start = 0 olduğu durum, manual komutlar burada çalışabilir
  {    
    manual_kontrol();                                                                    // manual komutların işletildiği yer
  }
}



















