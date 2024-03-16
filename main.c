#include <windows.h>
#include <stdio.h>
#include <math.h>

void SendResult();
double Calculate(const char* content);     //fonksiyonlar.
int operationControl=0;
HWND window;   //global değişken(mesaj kutusunu farklı yerlerde kullanacağız).

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main ()
{
    SetConsoleOutputCP(CP_UTF8);
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
            window=CreateWindowEx(0, "edit", "",
                           WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL|WS_BORDER|WS_DISABLED,
                                  10, 5, 200, 30, hwnd, (HMENU)0, //HMENU ye verilen numara Butonun ID numarasını verir.
                           GetModuleHandle(0), 0);
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
            wchar_t operations[]={'+', '-', '*', '/'};
            wchar_t operation[1];
            for (int i = 0; i < 4; ++i) {//Operasyon butonlarını oluşturuyoruz.


                operation[0]=operations[i];
                CreateWindowEx(0, "Button", (LPCSTR) operation, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
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
                wchar_t buttonText[2];
                int buttonId= LOWORD(wParam);
                if (buttonId == 18){     //= ise hesaplıyoruz.
                    SendResult();
                }
                else if (buttonId == 19){      //Clear ise tamamen siliyoruz.
                    SetWindowText(window, "");
                }
                else if(buttonId == 20){       //C ise son karakteri siliyoruz.
                    unsigned int length = GetWindowTextLength(window);
                    if (length>0){
                        SendMessage(window, EM_SETSEL, length - 1, (LPARAM)length);     //son karakteri Set ediyoruz.
                        SendMessage(window, EM_REPLACESEL, TRUE, (LPARAM)"");        //son karakterin yerine boşluk koyuyoruz.
                    }
                }
                else if(buttonId > 11 && buttonId < 16) {   //Operasyonları yazdırıyoruz.
                    GetWindowText(GetDlgItem(hwnd, buttonId), (LPSTR) buttonText,
                                  2);//ButonId sine göre buton metninin içeriğini değiştiriyoruz.

                    if (operationControl == 0){
                        SendMessage(window, EM_REPLACESEL, TRUE,
                                    (LPARAM) buttonText);   //Buton metnini metın kutusuna(pencereye) yolluyoruz.

                        operationControl++;
                    }
                    else {
                        unsigned int length = GetWindowTextLength(window);
                        SendMessage(window, EM_SETSEL, length-1, (LPARAM) length);
                        SendMessage(window, EM_REPLACESEL, TRUE, (LPARAM) buttonText);

                    }
                }
                else {   //Sayıları yazdırıyoruz.
                    GetWindowText(GetDlgItem(hwnd, buttonId), (LPSTR) buttonText, 2);//ButonId sine göre buton metninin içeriğini değiştiriyoruz.
                    SendMessage(window, EM_REPLACESEL, TRUE, (LPARAM) buttonText);
                    operationControl=0;
                }
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
void SendResult(){
    int length = GetWindowTextLength(window);
    char* content = malloc(length + 1);   //pointer content oluşturup ona bir büyüklük ve bellekteki yerini veriyoruz.
    GetWindowText(window, content, length + 1); //content pt a penceredeki metni aktarıyoruz.
    double result = Calculate(content);     //sonucu alıyoruz.
    SetWindowText(window, "");  //önce pencereyi temizliyoruz
    char resultText[40];    //Sonucu ekrana string olarak göndermek için bir char dizisi oluşturuyoruz.
    sprintf(resultText, "%.2lf", result); //Sonucu char dizisine yazıyoruz.
    SendMessage(window, EM_REPLACESEL, TRUE, (LPARAM)resultText);//Sonucu pencereye yazıyoruz.
    free(content);   //Bellekte açtığımız yeri boşaltıyoruz.
}


double Calculate(const char *content) {
    char *temp = malloc(sizeof(content) + 1); //geçici bir char pointer oluşturuyoruz.
    strcpy(temp, content);            //geçici pointer' a içeriği yazıyoruz.
    char operations[sizeof(content)/3];             //operasyonlar ve sayılar için dizi oluşturuyoruz.
    double numbers[sizeof(content)*2/3];

    char *token = strtok(temp, "+-*/");   //fonksiyonla operatör görülene kadar temp ifadesini kesiyoruz.
    numbers[0] = atof(token);                   //string ifadeyi double bir ifadeye çevirip 0. indeks' e atıyoruz.
    int u = 1;
    while (token != NULL){                            //token null değilse yani string ifade bitmemişse bitirene kadar döngüye sokuyoruz.
        token = strtok(NULL, "+-*/");       //NULL ifadesini girerek kaldığı yerden devam etmesini sağlıyoruz.
        if (token != NULL) {                           //Bu koşulda da token null değilse sayılar ve operatörler dizinise değerlerini atıyoruz.
            numbers[u] = atof(token);
            operations[u-1] = content[token - temp - 1];
            u++;
        }
    }
    if(content[0] == '-')           //operatör dizisi sadece iki sayı arasında kalan operatörleri aldığı için içeriğin ilk karakteri-
        numbers[0]=-numbers[0];     //negatifse sayıların ilk değerini - ile çarpıyoruz.
    double num1,num2;
    for (int i = 0; i < sizeof(operations)/ sizeof(operations[0]) ; ++i) {  //operasyon sayısı kadar döngüye giriyoruz
                                                        //bu switch case' de sadece çarpma ve bölme işlemleri için işlem yapılacak.
        switch (operations[i]) {
            case '*':
                num1=numbers[i];    //0. operatör için 0. sayı ve 1. sayı gerektiği için bu iki sayıyı bir değişkene aktarıyoruz.
                num2=numbers[i+1];
                num1=num1*num2;     //daha sonra 0. ve 1. sayı için gerekli işlemi yapıyoruz ve num1 e atıyoruz.
                numbers[i]=num1;    //daha sonra 0. indekse bu işlem sonucusu yazıyoruz.
                for (int j = i+1; j < sizeof(numbers)/sizeof(numbers[0]); ++j) {    //işin zor kısmı burada başlıyor çünkü her çarpma ve bölme işleminden sonra
                    //bu dizileri yeniden düzenlemek gerekiyor. Benim burada kullandığım taktik şuydu kullanılan operatörü silip yerine ondan sonra
                    //gelecek olan operatörleri yazmak ve 1. indekste kullandığımız sayıdan başlayark her indekse bir sonraki sayıyı yazacak.
                    //bu işlem sonunda peş peşe gelen * ve / operatörleri olabileceğinden döngüyü 1 kez geri sararak tekrar kontrol sağlayacak.
                    numbers[j]=numbers[j+1];
                }
                for (int j = i; j < sizeof(operations) / sizeof (operations[0]);++j) {
                    operations[j]=operations[j+1];
                }
                --i;
            break;
            case '/':
                num1=numbers[i];
                num2=numbers[i+1];
                if(num2==0) {
                    MessageBoxW(NULL, L"0' a bölünemez", L"HATA", MB_ICONERROR); //0' a bölme hata mesajı. Main de kullanılan SetConsoleOutput ifadesi ile Türkçe karakter yazdırabildik.
                    return 0.0;
                }
                num1=num1/num2;
                numbers[i]=num1;
                for (int j = i+1; j < sizeof(numbers)/sizeof(numbers[0]); ++j) {
                    numbers[j]=numbers[j+1];
                }
                for (int j = i; j < sizeof(operations) / sizeof (operations[0]);++j) {
                    operations[j]=operations[j+1];
                }
                --i;
            break;
        }
    }
    double result=numbers[0];   //eğer bir toplama ya da çıkarma işlemi yoksa 0. indeksimiz bizim sonucumuz olacağından sonuca sayıların 0. indeksini veriyoruz.
    for (int i = 0; i <= sizeof(operations)/ sizeof(operations[0]) ; ++i) {
        switch (operations[i]) { //toplama ya da çıkarma işlemi varsa bu 0. indeks ve 1. indeks arasında olacağından işleme göre 1. indeksle işlem yapıyoruz.
            case '+':
                result+=numbers[i+1];
                break;
            case '-':
                result-=numbers[i+1];
                break;
        }
    }
    free(temp); //bellekte ayırdığımız yeri boşaltıyoruz.
    return result;
}