#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <random>
#include <thread>
#include <synchapi.h>

//#pragma comment(lib, "ws2_32.lib")
// TODO: ��Ʈ�ι̳� ���� ���: ��� ��ǥ -> ���� ��ǥ(�迭�� ����)
// TODO: ��Ʈ�ι̳� ȸ�� ó��: ���ܱ��� ����
// TODO: until 07.22(Sat)

#define stdHandle GetStdHandle(STD_OUTPUT_HANDLE)
#define endl '\n'
using namespace std;

const int Black = 0;
const int Blue = 1;
const int Green = 2;
const int BlueGreen = 3;
const int Red = 4;
const int Purple = 5;
const int Yellow = 6;
const int White = 7;
const int Gray = 8;
const int LightBlue = 9;
const int LightGreen = 10;
const int LightBlueGreen = 11;
const int LightRed = 12;
const int LightPurple = 13;
const int LightYellow = 14;
const int LightWhite = 15;

const int Tetromino_I = 1;
const int Tetromino_J = 2;
const int Tetromino_L = 3;
const int Tetromino_O = 4;
const int Tetromino_S = 5;
const int Tetromino_T = 6;
const int Tetromino_Z = 7;

const int width = 11;
const int height = 21;
const int s_width = 30;
const int s_height = 7;
int screen_width, screen_height;

unsigned long dw;
random_device rd;
mt19937 mt(rd());


class Location {
public:
	int x;
	int y;
	Location() {
		x = 0;
		y = 0;
	}

    /// @brief Location Ŭ������ ������
    /// @param _x
    /// @param _y
	Location(int _x, int _y) {
		x = _x; y = _y;
	}
    bool isOutOfRange() const {
        return (x < 1 || y < 1 || x >= width || y >= height);
    }
};

/// @brief Ŀ���� (x, y) ��ġ�� �ű�
/// @param x
/// @param y
void gotoxy(SHORT x, SHORT y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(stdHandle, pos);
}

/// @brief Ŀ���� ������ ���� (x, y) ��ġ�� �ű�
/// @param x
/// @param y
void gotoBlockxy(SHORT x, SHORT y) {
    COORD pos = {static_cast<SHORT>((short)x + s_width), static_cast<SHORT>(y + s_height)};
    SetConsoleCursorPosition(stdHandle, pos);
}

/// @brief ������ ���� (x, y) ��ġ�� �� ����
/// @param x
/// @param y
void placeBlock(int x, int y) {
    COORD pos = {static_cast<SHORT>((x * 2) + s_width), static_cast<SHORT>(y + s_height)};
    SetConsoleCursorPosition(stdHandle, pos);
    cout << "��";
}

/// @brief Ȧ�� ���� (x, y) ��ġ�� �� ����
/// @param x
/// @param y
void placeHoldBlock(int x, int y) {
    COORD pos = {static_cast<SHORT>((x * 2) + 5), static_cast<SHORT>(y + 6)};
    SetConsoleCursorPosition(stdHandle, pos);
    cout << "��";
}

/// @brief �������� ���� ����
void clearGame() {
    for (int i = 1; i < height; ++i) {
        for (int j = 1; j < width; ++j) {
            COORD pos = {static_cast<SHORT>((j * 2) + s_width), static_cast<SHORT>(i + s_height)};
            SetConsoleCursorPosition(stdHandle, pos);
            cout << "  ";
        }
    }
}

/// @brief Ȧ��� �������� ������
void renderBorder()
{
    gotoxy(5, 6); printf("����������HOLD����������");
    gotoxy(5, 7); printf("��            ��");
    gotoxy(5, 8); printf("��            ��");
    gotoxy(5, 9); printf("��            ��");
    gotoxy(5, 10); printf("��            ��");
    gotoxy(5, 11); printf("����������������������������");

    gotoBlockxy(0, 0); printf("��");
    gotoBlockxy(width*2, 0); printf("��");
    gotoBlockxy(width*2, height); printf("��");
    gotoBlockxy(0, height); printf("��");
    for (int i = 1; i < width*2; i++) {
        gotoBlockxy(i, 0); printf("��");
        gotoBlockxy(i, height); printf("��");
    }
    for (int i = 1; i < height; i++) {
        gotoBlockxy(0, i); printf("��");
        gotoBlockxy(width*2, i); printf("��");
    }
    for (int i = 1; i < height; ++i) {
        for (int j = 1; j < width; ++j) {
            placeBlock(j, i);
        }
    }
//    placeHoldBlock(2, 2);
//    placeHoldBlock(2, 3);
//    placeHoldBlock(3, 3);
//    placeHoldBlock(4, 3);
    Sleep(1000);
    clearGame();
}

int game[20][10];
/// @brief ��Ʈ�ι̳� ó��
class Tetromino {
public:
    int type; int x; int y; int rotation;
    vector<Location> blocks;

    /// @brief ��Ʈ�ι̳� ������
    /// @param _type ��Ʈ�ι̳��� ����(1 ~ 7)
    /// @param _x
    /// @param _y
    /// @param _rotation ��Ʈ�ι̳��� ȸ�� ����(0' ~ 360')(90' ����)

    Tetromino(int _type, int _x, int _y, int _rotation = 0) {
        type = _type; x = _x; y = _y; rotation = _rotation / 90;
        blocks.resize(4);
        blocks[0] = {x, y}; // ���� ���
        switch (type) {
            case Tetromino_I:
                blocks[1] = {x, y + 1};
                blocks[2] = {x, y + 2};
                blocks[3] = {x, y + 3};
                break;
            case Tetromino_Z:
                blocks[1] = {1, 0};
                blocks[2] = {1, 1};
                blocks[3] = {2, 1};
                break;
            case Tetromino_T:
                blocks[1] = {-1, 1};
                blocks[2] = {0, 1};
                blocks[3] = {1, 1};
                break;
            case Tetromino_S:
                blocks[1] = {1, 0};
                blocks[2] = {0, 1};
                blocks[3] = {-1, 1};
                break;
            case Tetromino_O:
                blocks[1] = {1, 0};
                blocks[2] = {0, 1};
                blocks[3] = {1, 1};
                break;
            case Tetromino_J:
                blocks[1] = {0, 1};
                blocks[2] = {0, 2};
                blocks[3] = {-1, 2};
                break;
            case Tetromino_L:
                blocks[1] = {0, 1};
                blocks[2] = {0, 2};
                blocks[3] = {1, 2};
                break;
        }
    }

    /// @brief �ð� �������� 90' ȸ��
    void rotateClockWise() {
        rotation += (rotation == 4 ? -4 : 1);
        switch (type) {
            case Tetromino_I:
                break;
            case Tetromino_Z:
                break;
            case Tetromino_T:
                break;
            case Tetromino_S:
                break;
            case Tetromino_O:
                break;
            case Tetromino_J:
                break;
            case Tetromino_L:
                break;
        }
    }

    /**
     * @brief ���� ��Ʈ�ι̳븦 ���� ��ġ�� ������
     */
    void render() {
        for (Location& block: blocks) {
            Location here(block.x, block.y);
            if (here.isOutOfRange()) return;
        }
        for (Location& block: blocks) {
            Location here(block.x, block.y);
            placeBlock(here.x, here.y);
        }
    }

private:

};

/// @brief �޸��� Ʈ������(MT19937)�� ���� ����
/// @param from ���� �� �ּڰ�
/// @param to ���� �� �ִ�
/// @return from�̻� to������ ������ ����
int getRandomNumber(int from, int to) {
	uniform_int_distribution<int> distribution(from, to);
	return distribution(mt);
}

/// @brief ȭ���� ��� ����
void clearScreen() {
	FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
}

/// @brief ���� ���� ���� ����
/// @param col ��(1 ~ 15)
void setColor(int col)
{
	SetConsoleTextAttribute(stdHandle, col);
}

/// @brief Ű�� ���� ���� ����
/// @param key ������ Ű
/// @return ������ Ű�� ���ȴ°�?�� ����
bool isKeyDown(int key) {
	return GetAsyncKeyState(key) & 0x8000;
}

/// @brief �̱� �÷��̾� ���� ó��
void singlePlayerGame() {
    clearScreen();
    renderBorder();
    Tetromino test(Tetromino_I, 5, 1);
    test.render();
    Sleep(1000);
    test.rotateClockWise();
    test.render();
}

/// @brief �κ� ������
void lobby()
{
	clearScreen();
    puts("Initalizing...");
    Sleep(1000);
    clearScreen();
    /*
    ___________     __         .__        
	\__    ___/____/  |________|__| ______
	  |    |_/ __ \   __\_  __ \  |/  ___/
	  |    |\  ___/|  |  |  | \/  |\___ \ 
	  |____| \___  >__|  |__|  |__/____  >
	             \/                    \/ 
	*/

    // LOGO PRINT 
    {
    	int padding = 20;
    	// 1st Line 
    	setColor(Red);
    	gotoxy(screen_width / 2 - padding, 1);
    	cout << "___________";
    	setColor(Yellow);
    	cout << "     ";
    	setColor(Green);
    	cout << "__";
    	setColor(LightBlue);
    	cout << "         ";
    	setColor(Blue);
    	cout << ".__        ";
    	setColor(Purple);
    	
    	// 2nd Line 
    	setColor(Red);
    	gotoxy(screen_width / 2 - padding, 2);
    	cout << "\\__    ___/";
    	setColor(Yellow);
    	cout << "____";
    	setColor(Green);
    	cout << "/  |";
    	setColor(LightBlue);
    	cout << "________";
    	setColor(Blue);
    	cout << "|__|";
    	setColor(Purple);
    	cout << " ______";
    	
    	// 3rd Line 
    	setColor(Red);
    	gotoxy(screen_width / 2 - padding, 3);
    	cout << "  |    |";
    	setColor(Yellow);
    	cout << "_/ __ \\";
    	setColor(Green);
    	cout << "   __";
    	setColor(LightBlue);
    	cout << "\\_  __ \\";
    	setColor(Blue);
    	cout << "  |";
    	setColor(Purple);
    	cout << "/  ___/";
    	
    	// 4th Line 
    	setColor(Red);
    	gotoxy(screen_width / 2 - padding, 4);
    	cout << "  |    |";
    	setColor(Yellow);
    	cout << "\\  ___/";
    	setColor(Green);
    	cout << "|  |  ";
    	setColor(LightBlue);
    	cout << "|  | \\/";
    	setColor(Blue);
    	cout << "  |";
    	setColor(Purple);
    	cout << "\\___ \\ ";
    	
    	// 5th Line 
    	setColor(Red);
    	gotoxy(screen_width / 2 - padding, 5);
    	cout << "  |____| ";
    	setColor(Yellow);
    	cout << "\\___  ";
    	setColor(Green);
    	cout << ">__|  ";
    	setColor(LightBlue);
    	cout << "|__|  ";
    	setColor(Blue);
    	cout << "|__/";
    	setColor(Purple);
    	cout << "/____ >";
    	
    	// Last Line 
    	gotoxy(screen_width / 2 - padding, 6);
    	setColor(Yellow);
    	cout << "             \\/";
    	setColor(Purple);
    	cout << "                    \\/";
	}
	
	setColor(White);
    const int padding = 9;
    gotoxy(screen_width / 2 - padding, screen_height / 2);
    cout << "> SINGLE PLAYER";
    gotoxy(screen_width / 2 - padding, screen_height / 2 + 1);
    cout << "MULTI PLAYER(TCP/IP)";

    bool isSinglePlayer = true;
    while (!isKeyDown(VK_RETURN))
    {
        if (isKeyDown(VK_UP) || isKeyDown(VK_DOWN))
        {
            isSinglePlayer = !isSinglePlayer;
            gotoxy(screen_width / 2 - padding, screen_height / 2);
            cout << "                         ";
            gotoxy(screen_width / 2 - padding, screen_height / 2 + 1);
            cout << "                         ";
            if (isSinglePlayer)
            {
                gotoxy(screen_width / 2 - padding, screen_height / 2);
                cout << "> SINGLE PLAYER";
                gotoxy(screen_width / 2 - padding, screen_height / 2 + 1);
                cout << "MULTI PLAYER(TCP/IP)";
            }
            else
            {
                gotoxy(screen_width / 2 - padding, screen_height / 2);
                cout << "SINGLE PLAYER";
                gotoxy(screen_width / 2 - padding, screen_height / 2 + 1);
                cout << "> MULTI PLAYER(TCP/IP)";
            }
            Sleep(300);
        }
    }
    clearScreen();
    if (isSinglePlayer)
    {
    	// SinglePlayer Game Logic
        singlePlayerGame();
    }
    int x; cin >> x;
}

int main() {
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(stdHandle, &cursorInfo);
    screen_width = 70; screen_height = 45;
    
    system("mode con cols=70 lines=45");
	
	lobby();
}
