//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "prozor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	zavrsen_poligon = false;
	Slika->Canvas->Brush->Color=clWhite;
	Slika->Canvas->FillRect(TRect(0,0,Slika->Width,Slika->Height));
	Slika->Canvas->Brush->Color=clBlack;
	Slika->Canvas->FrameRect(TRect(0,0,Slika->Width,Slika->Height));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SlikaMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	Tacka nova = Tacka(X, Y);
	bool izbrisi = false;
	bool prava = false;

	if (RadioDodajDuz->Checked) {
		prava = false;
		if (drugi_klik) {
			Duz nova_duz(tacke[tacke.size()-1], nova);
			nova_duz.Crtaj(Slika);
            duzi.push_back(nova_duz);
		}
		drugi_klik = !drugi_klik;
	}
	else if (RadioTackaUnutarKonveksnog->Checked) {
		prava = false;
		if(konveksni_omotac.size() != 0) {
			if (tackaUnutarKonveksnog(nova, konveksni_omotac)) {
				ShowMessage("Tacka je unutra!");
			}
			else {
				ShowMessage("Tacka je van!");
			}
		}
	}
	else if (RadioTangente->Checked) {
		prava = false;
		if(konveksni_omotac.size() != 0) {
			if (!tackaUnutarKonveksnog(nova, konveksni_omotac)) {
				pair<int, int> tangente = nadjiTangente(nova, konveksni_omotac);
				int donja = tangente.first;
				int gornja = tangente.second;

				Duz(nova, konveksni_omotac[gornja]).Crtaj(Slika);
				Duz(nova, konveksni_omotac[donja]).Crtaj(Slika);
			}
		}
	}
	else if (RadioCrtajPoligon->Checked) {
		prava = false;
		if(poligon.size() > 2 && (nova - poligon[0]) < 10) {
			Duz d = Duz(poligon[poligon.size() - 1], poligon[0]);
			d.Crtaj(Slika);
			zavrsen_poligon = true;
		}
		else {
			if (zavrsen_poligon) {
				poligon.clear();
                dijagonale.clear();
                zavrsen_poligon = false;
			}
			poligon.push_back(nova);
			int n = poligon.size();
			if(n > 1) {
				Duz d = Duz(poligon[n-1], poligon[n-2]);
				d.Crtaj(Slika);
			}
			nova.Crtaj(Slika, clBlue);
		}
		return;
	}
	else if (RadioDodajPravu->Checked) {
		prava = true;
		if (drugi_klik) {
			prava = true;
			double x1, x2, y1, y2;
			Tacka prethodna = tacke[tacke.size()-1];
			x1 = prethodna.getX();
			y1 = prethodna.getY();
			x2 = nova.getX();
			y2 = nova.getY();

			bool validna_prava = true;
			double k = (double)(y2 - y1) / (double)(x2 - x1);
			double l = y1 - k * x1;
			int brojac = 0;
			for(int i = 0; i < prave.size(); i++) {
				Prava trenutna = prave[i];

			   // paralelne prave
				if (std::abs(k - trenutna.k) < 1e-6) {
					validna_prava = false;
					break;
				}

				double x = (trenutna.l - l) / (k - trenutna.k);
				double y = k * x + l;

				// tacka presjeka van canvasa
				if (x < 0 || x > Slika->Width || y < 0 || y > Slika->Height) {
					validna_prava = false;
					break;
				}

				tacke_presjeka.push_back(Tacka(x,y));
				brojac++;
			}

			if(validna_prava) {
				Prava nova_prava(k,l);
				nova_prava.Crtaj(Slika, clBlue);
				prave.push_back(nova_prava);

				for(int i = 0; i < tacke_presjeka.size(); i++) {
					Tacka trenutna = tacke_presjeka[i];
					trenutna.Crtaj(Slika, clBlue);
				}
			} else {
				izbrisi = true;
				for(int i = 0; i < brojac; i++)
					tacke_presjeka.pop_back();
			  }
		}
		drugi_klik = !drugi_klik;
	}

	tacke.push_back(nova);
	if(!prava)
		nova.Crtaj(Slika, clBlue);


	if(izbrisi) {
		izbrisi = false;
		double x1,x2,y1,y2;
		Tacka prva = tacke[tacke.size()-2];
		x1 = prva.getX();
		y1 = prva.getY();
		Tacka druga = tacke[tacke.size()-1];
		x2 = druga.getX();
		y2 = druga.getY();

		tacke.pop_back();
		tacke.pop_back();

		Slika->Canvas->Brush->Color=clWhite;
		Slika->Canvas->Pen->Color=clWhite;
		Slika->Canvas->Ellipse(x1-3, y1-3,x1+3,y1+3);
		Slika->Canvas->Ellipse(x2-3, y2-3,x2+3,y2+3);
		Slika->Canvas->Brush->Color=clBlack;

		ShowMessage("Prava nije validna! Tacka presjeka van canvasa/Paralelna sa drugom pravom.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonProstiMnogougaoClick(TObject *Sender)
{
	for (int i = 1; i < tacke.size(); i++) {
		if (tacke[i] < tacke[0]) {
			swap(tacke[0], tacke[i]);
		}
	}
	Tacka lijeva(tacke[0]);
	sort(tacke.begin() + 1, tacke.end(), [lijeva](Tacka A, Tacka B) { return Orijentacija(lijeva, A, B) < 0; });
	iscrtajPoligon(tacke, Slika);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonOcistiClick(TObject *Sender)
{
	Slika->Canvas->Brush->Color=clWhite;
	Slika->Canvas->FillRect(TRect(0,0,Slika->Width,Slika->Height));
	Slika->Canvas->Brush->Color=clBlack;
	Slika->Canvas->FrameRect(TRect(0,0,Slika->Width,Slika->Height));
	tacke.clear();
	konveksni_omotac.clear();
	poligon.clear();
	zavrsen_poligon = false;
	dijagonale.clear();
	duzi.clear();
	prave.clear();
	tacke_presjeka.clear();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonGenerisiTackeClick(TObject *Sender)
{
	int broj_tacaka = EditBrojTacaka->Text.ToInt();
	for (int i = 0; i < broj_tacaka; i++) {
		int x = rand() % Slika->Width;
		int y = rand() % Slika->Height;
		Tacka nova(x, y);
		nova.Crtaj(Slika, clBlue);
		tacke.push_back(nova);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonPresjekDuziClick(TObject *Sender)
{
	int n(duzi.size());

	if (n < 2) {
		return;
	}

	Duz duz1(duzi[n-1]), duz2(duzi[n-2]);
	if (daLiSeDuziSijeku(duz1, duz2)) {
		ShowMessage("Duzi se sijeku!");
	}
	else {
        ShowMessage("Duzi se ne sijeku!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonUvijanjePoklonaClick(TObject *Sender)
{
	if (tacke.size() < 3) {
		return;
	}
	for (int i = 1; i < tacke.size(); i++) {
		if (tacke[i] < tacke[0]) {
			swap(tacke[0], tacke[i]);
		}
	}
	Tacka lijeva(tacke[0]);
	konveksni_omotac.push_back(lijeva);

	while(true) {
		int iduca(0);
        Tacka zadnja_dodana(konveksni_omotac[konveksni_omotac.size()-1]);

		if (tacke[iduca] == zadnja_dodana) {
			iduca = 1;
		}
		for (int i = 0; i < tacke.size(); i++) {
			if (zadnja_dodana == tacke[i]) {
				continue;
			}
			if(Orijentacija(zadnja_dodana, tacke[iduca], tacke[i]) > 0) {
				iduca = i;
			}
		}
		if (iduca == 0) {
			break;
		}
		konveksni_omotac.push_back(tacke[iduca]);
	}
	string poruka = "Broj tacaka na konveksnom: " + to_string(konveksni_omotac.size());
	ShowMessage(poruka.c_str());
	iscrtajPoligon(konveksni_omotac, Slika);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ButtomGrahamScanClick(TObject *Sender)
{
	if (tacke.size() < 3) {
		return;
	}

	for (int i = 1; i < tacke.size(); i++) {
		if (tacke[i] < tacke[0]) {
			swap(tacke[0], tacke[i]);
		}
	}

	Tacka lijeva = tacke[0];
	konveksni_omotac.push_back(lijeva);

    sort(tacke.begin() + 1, tacke.end(), [lijeva](Tacka A, Tacka B) { return Orijentacija(lijeva, A, B) < 0; });
	konveksni_omotac.push_back(tacke[1]);

	for(int i = 2; i < tacke.size(); i++) {
		int k = konveksni_omotac.size();
		Tacka prethodna(konveksni_omotac[k-2]), trenutna(konveksni_omotac[k-1]);
		Tacka sljedeca(tacke[i]);

		while(Orijentacija(prethodna, trenutna, sljedeca) > 0) {
			konveksni_omotac.pop_back();
			int k = konveksni_omotac.size();
			prethodna = konveksni_omotac[k-2];
			trenutna = konveksni_omotac[k-1];
		}

		konveksni_omotac.push_back(sljedeca);
	}

	string poruka = "Broj tacaka na konveksnom: " + to_string(konveksni_omotac.size());
	ShowMessage(poruka.c_str());
	iscrtajPoligon(konveksni_omotac, Slika);
}


//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKonveksniInduktivnoClick(TObject *Sender)
{
	if (tacke.size() < 3) {
		return;
	}
	konveksni_omotac = {tacke[0], tacke[1], tacke[2]};
	if (Orijentacija(tacke[0], tacke[1], tacke[2]) > 0) {
		swap(konveksni_omotac[1], konveksni_omotac[2]);
	}

	for(int i=3; i<tacke.size(); i++) {
		Tacka trenutna(tacke[i]);

		if (tackaUnutarKonveksnog(trenutna, konveksni_omotac)) {
			continue;
		}

		pair<int, int> tangente = nadjiTangente(trenutna, konveksni_omotac);
		int donja = tangente.first;
		int gornja = tangente.second;

		if (donja > gornja) {
			konveksni_omotac.erase(konveksni_omotac.begin() + donja + 1, konveksni_omotac.end());
			if (gornja > 0) {
				konveksni_omotac.erase(konveksni_omotac.begin(), konveksni_omotac.begin() + gornja);
			}
			konveksni_omotac.insert(konveksni_omotac.begin(), trenutna);
		}
		else {
			konveksni_omotac.erase(konveksni_omotac.begin() + donja + 1, konveksni_omotac.begin() + gornja);
			konveksni_omotac.insert(konveksni_omotac.begin() + donja + 1, trenutna);
		}
	}
	string poruka = "Broj tacaka na konveksnom: " + to_string(konveksni_omotac.size());
	ShowMessage(poruka.c_str());
	iscrtajPoligon(konveksni_omotac, Slika);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonTriangulacijaClick(TObject *Sender)
{
	if (poligon.size() < 3) {
		return;
	}
	int velicina = poligon.size();
	list<int> lista;

	for(int i=0; i<velicina; i++)
		lista.push_back(i);

	int broj_trouglova = 0;

	auto p = lista.begin();
	auto t = ++lista.begin();
	auto n = ++(++lista.begin());

	while(lista.size() > 3) {
		if (Orijentacija(poligon[*p], poligon[*t], poligon[*n]) < 0) {
			// dobra orijentacija, provjeri da li je ijedna tacka u trouglu
			bool uho = true;
			auto i = n;
			pomjeriIteratorNaprijed(i, lista);
			while(i != p) {
				if(tackaUTrouglu(poligon[*i], Trokut(poligon[*p], poligon[*t], poligon[*n]))) {
					uho = false;
					break;
				}
                pomjeriIteratorNaprijed(i, lista);
			}

			if(uho) {
				dijagonale.push_back({*p, *n});
				lista.erase(t);
				t = p;
				pomjeriIteratorNazad(p, lista);
				continue;
			}
		}
		pomjeriIteratorNaprijed(p, lista);
		pomjeriIteratorNaprijed(t, lista);
		pomjeriIteratorNaprijed(n, lista);
	}
	for(auto par: dijagonale) {
		Tacka t1 = poligon[par.first];
		Tacka t2 = poligon[par.second];
		Duz(t1, t2).Crtaj(Slika);
	}
    string poruka = "Broj trouglova: " + to_string(dijagonale.size() + 1);
	ShowMessage(poruka.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SlikaMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
	string koordinate = "X: " + to_string(X) + " Y: " + to_string(Y);
    TextKoordinate->Text = koordinate.c_str();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonGenerisiHorVerDuziClick(TObject *Sender)
{
	int broj_duzi = EditBrojTacaka->Text.ToInt();
	for (int i = 0; i < broj_duzi; i++) {
		int x1,x2,y1,y2;
		if (rand() % 2 == 0) { // horizontalna
			x1 = rand() % Slika->Width;
			x2 = rand() % Slika->Width;
			y1 = rand() % Slika->Height;
			y2 = y1;
		}
		else { // vertikalna
			y1 = rand() % Slika->Height;
			y2 = rand() % Slika->Height;
			x1 = rand() % Slika->Width;
			x2 = x1;
		}
		Duz nova(Tacka(x1, y1), Tacka(x2, y2));
		nova.Crtaj(Slika, clBlue);
		nova.A.Crtaj(Slika, clRed);
		nova.B.Crtaj(Slika, clRed);
		duzi.push_back(nova);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ButtonPresjekHorVerDuziClick(TObject *Sender)
{
	priority_queue<pair<Duz*, Tacka>, vector<pair<Duz*, Tacka>>, HorVerDuziPoX> eventi;

	for(int i=0; i<duzi.size(); i++) {
		if (duzi[i].horizontalna()) {
			eventi.push({&duzi[i], duzi[i].A});
			eventi.push({&duzi[i], duzi[i].B});
		}
		else {
			eventi.push({&duzi[i], duzi[i].A});
		}
	}

	set<Duz*, AktivneHorVerPoY> aktivne;
	vector<Tacka> presjeci;

	while(!eventi.empty()) {
		pair<Duz*, Tacka> trenutni = eventi.top();
		eventi.pop();

		Duz* trenutna_duz = trenutni.first;
		Tacka trenutna_tacka = trenutni.second;

		if (trenutna_duz->horizontalna()) {
			if (trenutna_tacka == trenutna_duz->A) { // pocetak H
				aktivne.insert(trenutna_duz);
			}
			else { // kraj H
				aktivne.erase(trenutna_duz);
            }
		}
		else {  // vertikalna
			double gornji_y = trenutna_duz->A.y;
			double donji_y = trenutna_duz->B.y;
			Duz temp_d1 = Duz(Tacka(0, gornji_y), Tacka(0, gornji_y));
			Duz temp_d2 = Duz(Tacka(0, donji_y), Tacka(0, donji_y));

			auto it_gornji = aktivne.lower_bound(&temp_d1);
			auto it_donji = aktivne.upper_bound(&temp_d2);

			for(auto it = it_gornji; it != it_donji; it++) {
				Tacka presjek(trenutna_duz->A.x, (*it)->A.y);
				presjeci.push_back(presjek);
				presjek.Crtaj(Slika, clYellow, 5);
            }
		}

	}

	if (presjeci.size() == 0) {
        ShowMessage("Nema presjeka!");
	}
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonNadjiPresjekeDuziClick(TObject *Sender)
{
	priority_queue<pair<Tacka, pair<Duz*, Duz*>>, vector<pair<Tacka, pair<Duz*, Duz*>>>, DuziPoX> eventi;

	for(int i=0; i<duzi.size(); i++) {
		eventi.push({duzi[i].A, {&duzi[i], nullptr}});
		eventi.push({duzi[i].B, {&duzi[i], nullptr}});
	}

	set<Duz*, DuziPoY> aktivne;
	vector<Tacka> presjeci;

	while(!eventi.empty()) {
		auto e = eventi.top();
		eventi.pop();

		Tacka trenutna_tacka = e.first;
		int x_sweep_line = trenutna_tacka.x;
		Duz* trenutna_duz = e.second.first;
		Duz* druga_duz = e.second.second;

		if(trenutna_tacka == trenutna_duz->A && druga_duz == nullptr) {
			// pocetak duzi
			auto it = aktivne.insert(trenutna_duz).first;
			auto prethodna = it;
			auto naredna = it;

			if (aktivne.size() > 1) {
                if (it == aktivne.begin()) {
					naredna++;
					obradi_presjek(x_sweep_line, *it, *naredna, presjeci, eventi);
				}
				else if(it == --aktivne.end()) {
					prethodna--;
					obradi_presjek(x_sweep_line, *prethodna, *it, presjeci, eventi);
				}
				else {
					naredna++;
					obradi_presjek(x_sweep_line, *it, *naredna, presjeci, eventi);
					prethodna--;
					obradi_presjek(x_sweep_line, *prethodna, *it, presjeci, eventi);
				}
			}
		}
		else if(trenutna_tacka == trenutna_duz->B && druga_duz == nullptr) {
			// kraj duzi
			auto it = aktivne.find(trenutna_duz);
			if (it == aktivne.end()) {
				continue;
			}

			if(it != aktivne.begin() && it != --aktivne.end()) {
				auto prethodna = it;
				prethodna--;
				auto naredna = it;
				naredna++;

				obradi_presjek(x_sweep_line, *prethodna, *naredna, presjeci, eventi);
			}

			aktivne.erase(it);
		}
		else {
			// presjek duzi
			auto it1 = aktivne.find(trenutna_duz);
			auto it2 = aktivne.find(druga_duz);

			if (it1 == aktivne.end() || it2 == aktivne.end()) {
				continue;
			}

			aktivne.erase(it1);
			aktivne.erase(it2);

			Duz *nova_duz1 = new Duz(trenutna_tacka, trenutna_duz->B);
			Duz *nova_duz2 = new Duz(trenutna_tacka, druga_duz->B);

			auto it_ispod = aktivne.insert(nova_duz1).first;
			auto it_iznad = aktivne.insert(nova_duz2).first;

			auto it_prije = it_iznad;
			if (it_iznad != aktivne.begin()) {
				it_prije--;
				obradi_presjek(x_sweep_line, *it_prije, *it_iznad, presjeci, eventi);
			}

			auto it_poslije = it_ispod;
			if (it_poslije != --aktivne.end()) {
				it_poslije++;
				obradi_presjek(x_sweep_line, *it_ispod, *it_poslije, presjeci, eventi);
			}

			eventi.push({nova_duz1->B, {nova_duz1, nullptr}});
			eventi.push({nova_duz2->B, {nova_duz2, nullptr}});
		}
	}
	for (auto &presjek: presjeci) {
		presjek.Crtaj(Slika, clYellow, 5);
	}
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonGenerisiDuziClick(TObject *Sender)
{
	int broj_duzi = EditBrojTacaka->Text.ToInt();
	for (int i = 0; i < broj_duzi; i++) {
		int x1,x2,y1,y2;
		x1 = rand() % Slika->Width;
		x2 = rand() % Slika->Width;
		y1 = rand() % Slika->Height;
		y2 = rand() % Slika->Height;

		Duz nova(Tacka(x1, y1), Tacka(x2, y2));
		nova.Crtaj(Slika, clBlue);
		nova.A.Crtaj(Slika, clRed);
		nova.B.Crtaj(Slika, clRed);
		duzi.push_back(nova);
	}
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonKDStabloClick(TObject *Sender)
{
	 int xmin = p_xmin->Text.ToInt();
	 int xmax = p_xmax->Text.ToInt();
	 int ymin = p_ymin->Text.ToInt();
	 int ymax = p_ymax->Text.ToInt();
	 Pravougaonik query_p(xmin, xmax, ymin, ymax);
	 query_p.Crtaj(Slika, clRed);

	 KDStablo stablo(tacke, Slika->Width, Slika->Height);
	 stablo.iscrtaj(Slika);

	 vector<Tacka> query_tacke;
	 stablo.query(query_p, query_tacke);

	for(auto tacka: query_tacke) {
		tacka.Crtaj(Slika, clYellow, 4);
	}
	 // napravi stablo
	 // uradi query
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonGenerisiPraveClick(TObject *Sender)
{
	int broj_duzi = EditBrojTacaka->Text.ToInt();
	for (int i = 0; i < broj_duzi; i++) {
		int x1,x2,y1,y2;
		x1 = (rand() % Slika->Width/7) + 3*Slika->Width/7;
		x2 = (rand() % Slika->Width/7) + 3*Slika->Width/7;
		y1 = (rand() % Slika->Height/7) + 3*Slika->Height/7;
		y2 = (rand() % Slika->Height/7) + 3*Slika->Height/7;

		double k = static_cast<double>(y2 - y1) / (x2 - x1);
		double l = (y1 - k * x1);
		Prava nova_prava(k,l);
		nova_prava.Crtaj(Slika, clBlue);
		prave.push_back(nova_prava);

		// tacke presjeka nove prave sa svim pravima do sada
		for(int i = 0; i < prave.size()-1; i++) {
			Prava trenutna = prave[i];

			double x = (trenutna.l - l) / (k - trenutna.k);
			double y = k * x + l;

			tacke_presjeka.push_back(Tacka(x,y));
		}
	}

	for(int i = 0; i < tacke_presjeka.size(); i++) {
		Tacka trenutna = tacke_presjeka[i];
		trenutna.Crtaj(Slika, clBlue);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKonveksniOmotacPravihClick(TObject *Sender)
{
	tacke.clear();
	if (prave.size()<3) {
		ShowMessage("Nedovoljan broj pravih!");
	} else {
		sort(prave.begin(), prave.end());

		vector<Tacka> tacke_presjeka_susjednih;
		for (int i = 0; i < prave.size() - 1; ++i) {
			Prava& trenutna = prave[i];
			Prava& susjedna = prave[i + 1];

			double x = (susjedna.l - trenutna.l) / (trenutna.k - susjedna.k);
			double y = trenutna.k * x + trenutna.l;

			tacke.push_back(Tacka(x,y));
			tacke_presjeka_susjednih.push_back(Tacka(x,y));
		}

		Prava prva = prave[0];
		Prava zadnja = prave[prave.size()-1];

		double x = (zadnja.l - prva.l) / (prva.k - zadnja.k);
		double y = prva.k * x + prva.l;

		tacke.push_back(Tacka(x,y));
		tacke_presjeka_susjednih.push_back(Tacka(x,y));

		for(int i = 0; i < tacke_presjeka_susjednih.size(); i++) {
			Tacka trenutna = tacke_presjeka_susjednih[i];
			trenutna.Crtaj(Slika, clRed);
		}

		ButtomGrahamScanClick(Sender);
	}
}
//---------------------------------------------------------------------------

