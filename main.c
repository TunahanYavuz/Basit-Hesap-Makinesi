#include <windows.h>
#include <stdio.h>
#include <math.h>

void SonucuYazdir();
double Hesapla(const char* icerik);     //fonksiyonlar.
HWND pencere;   //global değişken(mesaj kutusunu farklı yerlerde kullanacağız).

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main ()
{
    const char HesapMak[]  = "Basit Hesap Mak"; //Pencerenin class adı.

    WNDCLASS wc = {};

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(0);
    wc.lpszClassName = HesapMak;

    RegisterClass(&wc);


    HWND hwnd = CreateWindowEx( //ana pencerenin oluşturulması
            WS_EX_TRANSPARENT, wc.lpszClassName, "Hesap Makinesi", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 235, 305,
            NULL, NULL, wc.hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);



    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE: //Kapatma mesajı.
        if (MessageBox(hwnd, "Kapatmak Istiyor Musun?", "UYARI", MB_YESNO) == IDYES)
            {
                DestroyWindow(hwnd);
            }
            return 0;

        case WM_DESTROY:        //Kapatma.
            PostQuitMessage(0);
            return 0;
        case WM_CREATE: //Butonları ve Mesaj kutusunu oluşturma.
            pencere=CreateWindowEx(0,"edit","",
                           WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL|WS_BORDER|WS_DISABLED,
                           10,5,200,30,hwnd,(HMENU)0, //HMENU ye verilen numara Butonun ID numarasını verir.
                           GetModuleHandle(0),0);
            int buttonNumber=0;
            for (int i = 0; i <4 ; ++i) {   //0 dan 9 a kadar olan sayıları oluşturma.
                for (int j = 0; j <3 ; ++j) {
                    if(i==3&&j==1)          //9 sayısına ulaşılınca döngüyü kırma.
                        break;
                    wchar_t buttonText[2];
                    buttonText[0]= '0' + buttonNumber;
                    buttonText[1]='\0';
                    CreateWindowEx(0, "Button", (LPCSTR)buttonText,
                                   WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                                   10+(j*50),40+(i*40), 50, 40, hwnd, (HMENU)(1 + buttonNumber),  //Her döngü için x ve y konularını ayarlıyoruz.
                                   GetModuleHandle(0), 0);
                    buttonNumber++;
                }

            }
            wchar_t operasyonlar[]={'+', '-', '*', '/'};
            wchar_t operasyon[1];
            for (int i = 0; i < 4; ++i) {//Operasyon butonlarını oluşturuyoruz.


                operasyon[0]=operasyonlar[i];
                CreateWindowEx(0, "Button", (LPCSTR) operasyon, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                               160,40+(i*40), 50, 40, hwnd, (HMENU)(12+i), GetModuleHandle(0), 0);

            }
        CreateWindowEx(0,"Button","=",  //Diğer butonları oluşturuyoruz.
                       WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                       10,200,200,50,hwnd,(HMENU)18,
                       GetModuleHandle(0),0);
        CreateWindowEx(0,"Button","Clear",
                       WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                       60,160,50,40,hwnd,(HMENU)19,
                       GetModuleHandle(0),0);
        CreateWindowEx(0,"Button","C",
                       WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
                       110,160,50,40,hwnd,(HMENU)20,
                       GetModuleHandle(0),0);



        case WM_COMMAND:    //Buton işlemleri.
            if(HIWORD(wParam)==BN_CLICKED) {
                wchar_t butonMetni[2];
                int butonId= LOWORD(wParam);
                if (butonId == 18){     //= ise hesaplıyoruz.
                    SonucuYazdir();
                }
                else if (butonId==19){      //Clear ise tamamen siliyoruz.
                    SetWindowText(pencere,"");
                }
                else if(butonId==20){       //C ise son karakteri siliyoruz.
                    unsigned int length = GetWindowTextLength(pencere);
                    if (length>0){
                        SendMessage(pencere,EM_SETSEL,length-1,(LPARAM)length);     //son karakteri Set ediyoruz.
                        SendMessage(pencere,EM_REPLACESEL,TRUE,(LPARAM)"");        //son karakterin yerine boşluk koyuyoruz.
                    }
                }
                else{   //Sayıları veya Operasyonları yazdırıyoruz.
                GetWindowText(GetDlgItem(hwnd, butonId), (LPSTR)butonMetni, 2);//ButonId sine göre buton metninin içeriğini değiştiriyoruz.
                SendMessage(pencere, EM_REPLACESEL, TRUE,(LPARAM)butonMetni);}  //Buton metnini metın kutusuna(pencereye) yolluyoruz.
            }

        case WM_PAINT:  //Arka planın boyanması.
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+8));
            EndPaint(hwnd, &ps);
        }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void SonucuYazdir(){
    int length = GetWindowTextLength(pencere);
    char* icerik = malloc(length + 1);   //pointer icerik oluşturup ona bir büyüklük ve bellekteki yerini veriyoruz.
    GetWindowText(pencere, icerik, length +1); //icerik pt a penceredeki metni aktarıyoruz.
    double sonuc = Hesapla(icerik);     //sonucu alıyoruz.
    SetWindowText(pencere,"");  //önce pencereyi temizliyoruz
    char sonucmetni[40];    //Sonucu ekrana string olarak göndermek için bir char dizisi oluşturuyoruz.
    sprintf(sonucmetni, "%.2lf",sonuc); //Sonucu char dizisine yazıyoruz.
    SendMessage(pencere,EM_REPLACESEL,TRUE,(LPARAM)sonucmetni);//Sonucu pencereye yazıyoruz.
    free(icerik);   //Bellekte açtığımız yeri boşaltıyoruz.
}
double Hesapla(const char *icerik){ //sadece okunacak olduğundan const char.
    char* temp = malloc(strlen(icerik) + 1);    //geçici char dizisi daha.
    strcpy(temp, icerik);   //diziye iceriği kopyalama.
    char* token= strtok(temp,"+-*/");   //strkok fonksiyonuyla +-*/ görülene kadar metni okuma.
    double sayi1,sonuc= atof(token);    //atof fonksiyonuyla string değeri doble,float değere dönüştürme. Eğer bir işlem yoksa direkt olarak bu değer ekrana yazdırılacaktır.
    char operasyon;
    while (token !=NULL) {      //içerik bitene kadar dögüye sokuyoruz.
        token = strtok(NULL, "+-*/"); //2. sayıyı alacağımız için bir kez daha strtok kullanıyoruz ama NULL ifadesi giriyoruz. Bu şekilde önceden aldığı değerden okumaya devam edecek.
        if (token != NULL){
            sayi1 = atof(token);
            operasyon = icerik[token - temp - 1]; //token - temp yani alınan değerden ilk değere kadar kaç karakter var -1 ise bize bir önceki karakteri vereceğinden operasyonu buluyoruz.
            switch (operasyon) {
                case '*':
                    sonuc *= sayi1;
                    break;
                case '/':
                    if (sayi1 != 0) { sonuc /= sayi1; }
                    else {
                        MessageBox(NULL, "0' a bolunulemez", "HATA", MB_ICONERROR); //0 a bölmeye kalkarsa hata mesajı.
                        return 0.0;
                    }
                    break;
                case '+':
                    sonuc+=sayi1;
                    break;
                case '-':
                    sonuc-=sayi1;
                    break;

            }
        }
    }

    free(temp); //belleği boşaltma.
    return sonuc;   //sonucu döndürme
}