#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "RandomNameGenerator.hpp"


constexpr double STAN0 = 60000.0;
constexpr double SUKCES = 10000000.0;
constexpr int KROTKI_KREDYT = 5;
constexpr double OPROCENTOWANIE_KK = 0.05;
constexpr int ROCZNY_KREDYT = 12;
constexpr double OPROCENTOWANIE_RK = 0.2;
constexpr int X = 24; //Limit dlugosci kredytu
constexpr double OPROCENTOWANIE_DK = 0.35;
constexpr int M = 3;  //Przelicznik limitu wysokosci kredytu
int N = 5; //Ile ostatnich zapisów wektora 'przychody' wlicza się do wartości firmy
constexpr double CI = 100.;
constexpr int CMag = 1000;
constexpr int CMkt = 300;
constexpr int CRob = 100;
constexpr double wI = 5000.;
constexpr double wMag = 3500.;
constexpr double wMkt = 4500.;
constexpr double wRob = 3500.;
int kolejka = 0;

class Pracownik {
public:
    Pracownik(): imie{"Jan Kowalski"}, wynagrodzenie{0.}{}
    Pracownik(const std::string& imie, double wynagrodzenie): imie{imie}, wynagrodzenie{wynagrodzenie} {}
    virtual ~Pracownik() = default;
    Pracownik(const Pracownik& other): imie{other.imie}, wynagrodzenie{other.wynagrodzenie} {}
    Pracownik& operator=(const Pracownik& other) {
        if (this != &other) {
            imie = other.imie;
            wynagrodzenie = other.wynagrodzenie;
        }
        return *this;
    }

    virtual void print() const = 0;

    std::string imie;
    double wynagrodzenie = 0;
};

//Klasy dziedziczące po klasie Pracownik:
class Inzynier : public Pracownik {
public:
    Inzynier() {
        imie = getRandomName();
        wynagrodzenie = wI;
        skonczonyWydzial = "MEiL";
    }
    Inzynier(const std::string& name, const std::string& other);
    ~Inzynier() override = default;
    Inzynier(const Inzynier& other): Pracownik{other}, skonczonyWydzial{other.skonczonyWydzial} {}
    Inzynier& operator=(const Inzynier& other) {
        if (this != &other) {
            Pracownik::operator=(other);
            skonczonyWydzial = other.skonczonyWydzial;
        }
        return *this;
    }

    void print() const override {
        std::cout << "Imie: " << imie << ", Zawod: inzynier, Wydzial: "
                  << skonczonyWydzial << ", Wynagrodzenie: " << wynagrodzenie << '\n';
    }

    std::string skonczonyWydzial;
};

class Magazynier : public Pracownik {
public:
    Magazynier() {
        imie = getRandomName();
        wynagrodzenie = wMag;
        obslugaWozka = true;
    }
    ~Magazynier() override = default;
    Magazynier(const Magazynier& other): Pracownik {other}, obslugaWozka{other.obslugaWozka} {}
    Magazynier& operator=(const Magazynier& other) {
        if (this != &other) {
            Pracownik::operator=(other);
            obslugaWozka = other.obslugaWozka;
        }
        return *this;
    }

    void print() const override {
        std::cout << "Imie: " << imie << ", Zawod: magazynier, Wozek widlowy: "
                  << (obslugaWozka ? "tak" : "nie") << ", Wynagrodzenie: " << wynagrodzenie << '\n';
    }

    bool obslugaWozka;
};

class Marketer : public Pracownik {
public:
    Marketer() {
        imie = getRandomName();
        wynagrodzenie = wMkt;
        nObserwujacych = 1000;
    }
    ~Marketer() override = default;
    Marketer(const Marketer& other): Pracownik{other}, nObserwujacych{other.nObserwujacych} {}
    Marketer& operator=(const Marketer& other) {
        if (this != &other) {
            Pracownik::operator=(other);
            nObserwujacych = other.nObserwujacych;
        }
        return *this;
    }

    void print() const override {
        std::cout << "Imie: " << imie << ", Zawod: marketer, Obserwujacych: "
                  << nObserwujacych << ", Wynagrodzenie: " << wynagrodzenie << '\n';
    }

    unsigned int nObserwujacych;
};

class Robotnik : public Pracownik {
public:
    Robotnik() {
        imie = getRandomName();
        wynagrodzenie = wRob;
        rozmiarButa = 42.5;
    }
    ~Robotnik() override = default;
    Robotnik(const Robotnik& other): Pracownik{other}, rozmiarButa{other.rozmiarButa} {}
    Robotnik& operator=(const Robotnik& other) {
        if (this != &other) {
            Pracownik::operator=(other);
            rozmiarButa = other.rozmiarButa;
        }
        return *this;
    }
    void print() const override {
        std::cout << "Imie: " << imie << ", Zawod: robotnik, Rozmiar buta: "
                  << rozmiarButa << ", Wynagrodzenie: " << wynagrodzenie << '\n';
    }

    double rozmiarButa;
};

//Klasa Kredyt
class Kredyt {
public:
    Kredyt(double kwota, int czasSplaty) : dlug{kwota}, pozostaleRaty{czasSplaty} {
        if (czasSplaty <= KROTKI_KREDYT) rata = (1.0 + OPROCENTOWANIE_KK) * kwota / czasSplaty;
        //Kredyt od 6 do 12 miesięcy
        else if (czasSplaty <= ROCZNY_KREDYT) rata = (1.0 + OPROCENTOWANIE_RK) * kwota / czasSplaty;
        else rata = (1.0 + OPROCENTOWANIE_DK) * kwota / czasSplaty;
    }
    ~Kredyt() = default;
    Kredyt(const Kredyt& other): dlug{other.dlug}, pozostaleRaty{other.pozostaleRaty} {}
    Kredyt& operator=(const Kredyt& other) {
        if (this != &other) {
            dlug = other.dlug;
            pozostaleRaty = other.pozostaleRaty;
        }
        return *this;
    }

    void splacRate() {
        dlug -= rata;
        pozostaleRaty--;
    }
    double getRata() const { return rata; }
    int getPozostaleRaty() const { return pozostaleRaty; }

    double dlug;
    int pozostaleRaty;
    double rata;
};

    
class Firma {
public:
    Firma(): stanKonta{STAN0},wartoscFirmy{0.01}, nPracownikow{0}, nInz{0},nMag{0},nMkt{0},nRob{0},cenaProduktu{CI},\
pojemnoscMagazynu{0}, nZmagazynowanychProduktow{0},nMozliwosciSprzedarzy{0},oczekiwanaLiczbaSprzedarzy{0},\
wynagrodzenia{0.},nKredytow{0},raty{0.},zadluzenie{0.},przychod{CI*(double)CRob},dochod{CI*(double)CRob-(wI+wMag+wMkt+wRob)} {
    zatrudnij(std::move(std::make_unique<Inzynier>()));
    zatrudnij(std::move(std::make_unique<Magazynier>()));
    zatrudnij(std::move(std::make_unique<Marketer>()));
    zatrudnij(std::move(std::make_unique<Robotnik>()));
    }
    ~Firma() = default;
    Firma(const Firma& other): stanKonta{other.stanKonta}, wartoscFirmy{other.wartoscFirmy}{}
    Firma& operator=(const Firma& other) {
        if (this != &other) {
            stanKonta = other.stanKonta;
            wartoscFirmy = other.wartoscFirmy;
        }
        return *this;
    }

    double getStanKonta() const { return stanKonta; }
    double getWartoscFirmy() const { return wartoscFirmy; }
    
    void zatrudnij(std::unique_ptr<Pracownik> pracownik) {
    if (pracownik) {
        // Sprawdzenie typu zatrudnianego pracownika
        if (dynamic_cast<Inzynier*>(pracownik.get())) {
            ++nInz;
            cenaProduktu += CI;
        }
        else if (dynamic_cast<Magazynier*>(pracownik.get())) {
            ++nMag;
            pojemnoscMagazynu += CMag;
        }
        else if (dynamic_cast<Marketer*>(pracownik.get())) {
            ++nMkt;
            nMozliwosciSprzedarzy += CMkt;
        }
        else if (dynamic_cast<Robotnik*>(pracownik.get())) {
            ++nRob;
        }

        // Dodawanie pracownika do tablicy
        pracownicy.push_back(std::move(pracownik));

        // Aktualizowanie liczby pracowników i wynagrodzenia
        ++nPracownikow;
        wynagrodzenia += pracownicy[nPracownikow - 1]->wynagrodzenie;
        }
    }

    void drukujPracownikow() {
        for (int i = 0; i < nPracownikow; ++i) {
        pracownicy[i]->print();
        }
    }

    void wezKredyt(double kwota, int czasSplaty) {
        if (kwota <= 0) {
            printf("Blad: podana ujemna wartosc kwoty kredytu.\n");
            return;
        }
        if (zadluzenie + kwota > M * wartoscFirmy) {
            printf("Zadluzenie nie moze przekroczyc %d - krotnej wielokrotnosci wartosci firmy, czyli maksymalna wysokosc kredytu to %.2lf.\n", M, (N * wartoscFirmy - zadluzenie));
            return;
        }
        if (czasSplaty <= 0 || czasSplaty > X) {
            printf("Czas splaty musi byc miedzy 1 a %d miesiacami.\n", X);
            return;
        }

        // Dodawanie kredytu
        auto nowyKredyt = std::make_unique<Kredyt>(kwota, czasSplaty);
        kredyty.push_back(std::move(nowyKredyt));

        // Aktualizowanie zadłużenia i liczby kredytów
        zadluzenie += kwota;
        ++nKredytow;
    }

    void produkcja(){
        nZmagazynowanychProduktow+=nRob*CRob;
        if (nZmagazynowanychProduktow>pojemnoscMagazynu) nZmagazynowanychProduktow=pojemnoscMagazynu;
    }

    void oczekiwania() {
        oczekiwanaLiczbaSprzedarzy = std::min(nZmagazynowanychProduktow, nMozliwosciSprzedarzy);
    }

    void obliczPrzychod() {
        przychod = cenaProduktu * oczekiwanaLiczbaSprzedarzy;
        stanKonta += przychod;
        nZmagazynowanychProduktow -= oczekiwanaLiczbaSprzedarzy;

        // Dodaj przychód do historiiPrzychodów
        historiaPrzychodow.push_back(przychod);
    }
   

    void zaplacWynagrodzenie() {
        stanKonta -= wynagrodzenia;
    }

    void splacRaty() {
        for (int i = 0; i < nKredytow; ++i) kredyty[i]->splacRate();
        for (int i = 0; i < nKredytow; ++i) {
            if (kredyty[i]->pozostaleRaty == 0) {
                // Usuń kredyt, przesuwając wskaźniki
                for (int j = i; j < nKredytow - 1; ++j) {
                    kredyty[j] = std::move(kredyty[j + 1]);
                }
                --nKredytow;
                --i;
            }
        }
    }

    void obliczZadluzenie() {
        zadluzenie = 0;
        for (int i = 0; i < nKredytow; ++i) {
            zadluzenie += kredyty[i]->dlug;
        }
    }
    void obliczWartoscFirmy() {
        if (historiaPrzychodow.size() < N) return;
        double sumaPrzychodow = 0.0;
        for (int i = (historiaPrzychodow.size()-N); i < historiaPrzychodow.size(); ++i) {
            printf("historiaPrzychodow[%d] = %lf",i, historiaPrzychodow[i]);
            sumaPrzychodow += historiaPrzychodow[i];
        }
        printf("Po wyjsciu z petli\n");
        wartoscFirmy = sumaPrzychodow / (double)N;
    }

    void obliczDochod() {
        dochod = przychod - wynagrodzenia - raty;
    }

    void wyswietlStatus(int kolejka) {
        printf(
            "\n\nPodsumowanie kolejki %d:\n Stan konta: %.2lf\n \
            Liczba pracownikow: %d\n Inzynierowie: %d\n Magazynierzy: %d\n Marketerzy: %d\n Robotnicy: %d\n \
            Wysokosc miesiecznego wynagrodzenia pracownikow: %.2lf\n Cena produktu: %.2lf\n \
            Magazyn: %d / %d\n Klienci: %d\n Kredyty: %d\n Zadluzenie: %.2lf\n Miesieczna rata: %.2lf\n Przychod: %.2lf\n \
            Dochod: %.2lf\n Wartosc firmy: %.2lf\n", kolejka, stanKonta, nPracownikow, nInz, nMag, nMkt, nRob, \
            wynagrodzenia, cenaProduktu, nZmagazynowanychProduktow, pojemnoscMagazynu,
            nMozliwosciSprzedarzy, nKredytow, zadluzenie, raty, przychod, dochod, wartoscFirmy);
    }

    void zakonczMiesiac(int &kolejka) {
        produkcja();
        oczekiwania();
        obliczPrzychod();
        zaplacWynagrodzenie();
        splacRaty();
        obliczZadluzenie();
        obliczWartoscFirmy();
        obliczDochod();
        wyswietlStatus(kolejka);
    }

private:
    double stanKonta;
    std::vector<std::unique_ptr<Pracownik>> pracownicy;
    int nPracownikow;
    int nInz;
    int nMag;
    int nMkt;
    int nRob;
    double cenaProduktu;
    double wartoscFirmy;
    int pojemnoscMagazynu;
    int nZmagazynowanychProduktow;
    int nMozliwosciSprzedarzy;
    int oczekiwanaLiczbaSprzedarzy;
    double wynagrodzenia;
    std::vector<std::unique_ptr<Kredyt>> kredyty;
    int nKredytow;
    double raty;
    double zadluzenie;
    std::vector<double> historiaPrzychodow;
    double przychod;
    double dochod;
};

// Dla wygody Firma to zmienna globalna
Firma FIRMA;


void akcjaGracza(int &kolejka){
    printf("\n\nTwoja akcja: ");
        std::string akcja;
        std::cin >> akcja;

        if (akcja == "lp") {
            FIRMA.drukujPracownikow();
        } else if (akcja == "zinz") {
            FIRMA.zatrudnij(std::move(std::make_unique<Inzynier>()));
        } else if (akcja == "zmag") {
            FIRMA.zatrudnij(std::move(std::make_unique<Magazynier>()));
        } else if (akcja == "zmkt") {
            FIRMA.zatrudnij(std::move(std::make_unique<Marketer>()));
        } else if (akcja == "zrob") {
            FIRMA.zatrudnij(std::move(std::make_unique<Robotnik>()));
        } else if (akcja == "kred") {
            double wysokoscKredytu;
            int czasSplatyKredytu;
            printf("\nPodaj wysokosc i czas splaty kredytu:\n");
            std::cin >> wysokoscKredytu >> czasSplatyKredytu;
            FIRMA.wezKredyt(wysokoscKredytu, czasSplatyKredytu);
        } else if (akcja == "kt") {
            FIRMA.zakonczMiesiac(kolejka);
            ++kolejka;
        } else if (akcja == "help") {
             printf("\nMozliwe akcje: \nlp (lista pracownikow), \nzinz (zatrudnij inzyniera), \nzmag (zatrudnij magazyniera), \
    \nzmkt (zatrudnij marketera), \nzrob (zatrudnij robotnika), \nkred (wez kredyt), \nkt (zakoncz ture), \nhelp (lista akcji).\n");
        } else {
            printf("\nBlad: nieznana akcja. Uzyj 'help' aby wyswietlic liste mozliwych akcji.\n");
        }
};

void getStatus(){
    // Warunek wygranej
        if (FIRMA.getWartoscFirmy() >= SUKCES) {
            printf("\nGratulacje! Osiagnales wartosc firmy %.2lf i wygrales gre!\n", FIRMA.getWartoscFirmy());
            exit(0);
        }

        // Warunek przegranej
        if (FIRMA.getStanKonta() <= 0) {
            printf("\nBankructwo! Stan twojego konta wynosi %.2lf , wiec przegrales gre!\n", FIRMA.getStanKonta());
            exit(0);
        }
};



int main() {
    printf("\n\nWitaj w grze ekonomicznej! Twoim celem jest osiagniecie wartosci firmy %.2lf.\n\n\n", SUKCES);
    FIRMA.wyswietlStatus(0);
    printf("\nMozliwe akcje: \nlp (lista pracownikow), \nzinz (zatrudnij inzyniera), \nzmag (zatrudnij magazyniera), \
    \nzmkt (zatrudnij marketera), \nzrob (zatrudnij robotnika), \nkred (wez kredyt), \nkt (zakoncz ture), \nhelp (lista akcji).\n");

    kolejka = 1;
    

    while (true) {
        akcjaGracza(kolejka);
        getStatus();  
    }
    return 0;
}