// DomacaZadaca03.cpp - OBA ZADATKA BEZ SFML-a (radi u VS 2022, C++14/17)
#define NOMINMAX  // Spriječava Windows.h da definira makroe min/max
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <set>
#include <thread>
#include <chrono>
#include <windows.h>  // Za konzolne boje i pozicioniranje

using namespace std;

const int REDAKA = 20;
const int STUPACA = 40;

// Pomoćne funkcije za konzolu
void postaviKursor(int x, int y) {
    COORD koord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), koord);
}

void boja(int boja) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), boja);
}

void ocistiEkran() {
    system("cls");
}

// Bresenhamov algoritam (Zadatak 1)
vector<pair<int, int>> bresenham(int r1, int s1, int r2, int s2) {
    vector<pair<int, int>> tocke;
    int dx = abs(s2 - s1);
    int dy = abs(r2 - r1);
    int sx = (s1 < s2) ? 1 : -1;
    int sy = (r1 < r2) ? 1 : -1;
    int err = dx - dy;
    int x = s1, y = r1;

    while (true) {
        tocke.push_back({ y, x });
        if (x == s2 && y == r2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
    return tocke;
}

// Dijkstrin algoritam (Zadatak 2) – vraća put i popunjava matricu posjećenih ćelija
vector<pair<int, int>> dijkstra(int startR, int startC, int endR, int endC,
    const set<pair<int, int>>& zidovi,
    vector<vector<bool>>& posjeceno) {
    const int INF = (numeric_limits<int>::max)();  // Zagrade štite od makroa max
    vector<vector<int>> dist(REDAKA, vector<int>(STUPACA, INF));
    vector<vector<pair<int, int>>> prev(REDAKA, vector<pair<int, int>>(STUPACA, { -1, -1 }));

    // Prioritetni red: par (udaljenost, (redak, stupac))
    priority_queue<pair<int, pair<int, int>>,
        vector<pair<int, pair<int, int>>>,
        greater<pair<int, pair<int, int>>>> pq;

    dist[startR][startC] = 0;
    pq.push({ 0, {startR, startC} });
    posjeceno.assign(REDAKA, vector<bool>(STUPACA, false));

    const int dx[] = { 1, -1, 0, 0 };
    const int dy[] = { 0, 0, 1, -1 };

    while (!pq.empty()) {
        // Uzmi element s vrha (bez strukturirane veze)
        int d = pq.top().first;
        int r = pq.top().second.first;
        int c = pq.top().second.second;
        pq.pop();

        if (posjeceno[r][c]) continue;
        posjeceno[r][c] = true;
        if (r == endR && c == endC) break;

        for (int k = 0; k < 4; ++k) {
            int nr = r + dy[k];
            int nc = c + dx[k];
            if (nr < 0 || nr >= REDAKA || nc < 0 || nc >= STUPACA) continue;
            if (zidovi.count({ nr, nc })) continue;
            int novaDist = d + 1;
            if (novaDist < dist[nr][nc]) {
                dist[nr][nc] = novaDist;
                prev[nr][nc] = { r, c };
                pq.push({ novaDist, {nr, nc} });
            }
        }
    }

    vector<pair<int, int>> put;
    if (dist[endR][endC] == INF) return put; // nema puta

    int r = endR, c = endC;
    while (!(r == startR && c == startC)) {
        put.push_back({ r, c });
        auto p = prev[r][c];
        r = p.first;
        c = p.second;
    }
    put.push_back({ startR, startC });
    reverse(put.begin(), put.end());
    return put;
}

// Iscrtava grid s putem, markerom, zidovima, startom i krajem
void iscrtajGrid(const vector<pair<int, int>>& put,
    size_t trenutniIndeks,
    pair<int, int> start, pair<int, int> kraj,
    const set<pair<int, int>>& zidovi = {},
    const vector<vector<bool>>& posjeceno = {}) {
    vector<vector<char>> grid(REDAKA, vector<char>(STUPACA, '.'));

    // Zidovi
    for (auto& z : zidovi) {
        grid[z.first][z.second] = '#';
    }

    // Start i kraj
    grid[start.first][start.second] = 'A';
    grid[kraj.first][kraj.second] = 'B';

    // Posjećena polja (Dijkstra)
    if (!posjeceno.empty()) {
        for (int r = 0; r < REDAKA; ++r) {
            for (int c = 0; c < STUPACA; ++c) {
                if (posjeceno[r][c] && grid[r][c] == '.')
                    grid[r][c] = ':';
            }
        }
    }

    // Prijeđeni put
    for (size_t i = 0; i <= trenutniIndeks && i < put.size(); ++i) {
        int r = put[i].first, c = put[i].second;
        if (grid[r][c] == '.' || grid[r][c] == ':') grid[r][c] = 'o';
    }

    // Trenutna pozicija
    if (trenutniIndeks < put.size()) {
        int r = put[trenutniIndeks].first, c = put[trenutniIndeks].second;
        grid[r][c] = 'X';
    }

    postaviKursor(0, 0);
    for (int r = 0; r < REDAKA; ++r) {
        for (int c = 0; c < STUPACA; ++c) {
            char znak = grid[r][c];
            if (znak == '#') boja(8);          // tamno siva za zidove
            else if (znak == 'X') boja(14);    // žuta za trenutni marker
            else if (znak == 'o') boja(11);    // svijetlo plava za prijeđeni put
            else if (znak == 'A') boja(10);    // zelena za A
            else if (znak == 'B') boja(12);    // crvena za B
            else if (znak == ':') boja(7);     // svijetlo siva za posjećeno
            else boja(7);                      // siva za prazno
            cout << znak;
        }
        cout << '\n';
    }
    boja(7);
    cout << "A=zelena, B=crvena, X=trenutno, o=put, .=prazno, #=zid" << endl;
}

void animirajPut(const vector<pair<int, int>>& put,
    pair<int, int> start, pair<int, int> kraj,
    const set<pair<int, int>>& zidovi = {},
    const vector<vector<bool>>& posjeceno = {}) {
    ocistiEkran();
    for (size_t i = 0; i < put.size(); ++i) {
        iscrtajGrid(put, i, start, kraj, zidovi, posjeceno);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    // Zadnji frame
    iscrtajGrid(put, put.size() - 1, start, kraj, zidovi, posjeceno);
    cout << "\nGotovo! Pritisni Enter za nastavak..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    int izbor;
    cout << "Odaberi zadatak:\n"
        << "1 - Put bez prepreka (Bresenham)\n"
        << "2 - Put s preprekama (Dijkstra)\n> ";
    cin >> izbor;

    int rA, sA, rB, sB;
    cout << "Unesi redak i stupac tocke A (1-" << REDAKA << ", 1-" << STUPACA << "): ";
    cin >> rA >> sA;
    cout << "Unesi redak i stupac tocke B (1-" << REDAKA << ", 1-" << STUPACA << "): ";
    cin >> rB >> sB;
    rA--; sA--; rB--; sB--;

    // Provjera
    if (rA < 0 || rA >= REDAKA || sA < 0 || sA >= STUPACA ||
        rB < 0 || rB >= REDAKA || sB < 0 || sB >= STUPACA) {
        cerr << "Neispravne koordinate!" << endl;
        return 1;
    }

    if (izbor == 1) {
        auto put = bresenham(rA, sA, rB, sB);
        animirajPut(put, { rA, sA }, { rB, sB });
    }
    else if (izbor == 2) {
        // Zidovi – primjer kao u uputama
        set<pair<int, int>> zidovi;
        for (int c = 0; c < 35; ++c) zidovi.insert({ 10, c });   // horizontalni zid
        for (int r = 5; r < 15; ++r) zidovi.insert({ r, 25 });   // vertikalni zid
        for (int c = 10; c < 20; ++c) zidovi.insert({ 3, c });   // dodatni blok
        zidovi.insert({ 17, 30 });
        zidovi.insert({ 17, 31 });
        // Ukloni zidove s pozicija A i B
        zidovi.erase({ rA, sA });
        zidovi.erase({ rB, sB });

        vector<vector<bool>> posjeceno;
        auto put = dijkstra(rA, sA, rB, sB, zidovi, posjeceno);
        if (put.empty()) {
            cerr << "Ne postoji put do cilja!" << endl;
            return 1;
        }
        animirajPut(put, { rA, sA }, { rB, sB }, zidovi, posjeceno);
    }
    else {
        cout << "Nepoznat izbor." << endl;
    }
    return 0;
}