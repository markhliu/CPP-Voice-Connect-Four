#include "stdafx.h"
#include <iostream>
#include<sstream>
#include <speechapi_cxx.h>
#include <sapi.h>
#include <string>
#include <stdio.h>
#include "CTurtle.hpp"


using namespace std;
using namespace Microsoft::CognitiveServices::Speech;


namespace ct = cturtle;



int xs[7] = { -300, -200, -100, 0, 100, 200, 300 };
int ys[6] = { -250, -150, -50, 50, 150, 250 };
int height[7] = { 0,0,0,0,0,0,0 };
string turn[2] = { "red","yellow" };
string cell[7][6];


ct::TurtleScreen scr(700, 600);
ct::Turtle turtle(scr);
int board(ct::Turtle& turtle) {

    for (int i = -250; i <= 250; i += 100) {
        turtle.penup();
        turtle.goTo(i, -350);
        turtle.pendown();
        turtle.goTo(i, 350);
    };
    turtle.width(1);
    for (int i = -200; i <= 200; i += 100) {
        turtle.penup();
        turtle.goTo(-350, i);
        turtle.pendown();
        turtle.goTo(350, i);
    };

    for (int i = 1; i < 8; i++) {
        turtle.penup();
        turtle.goTo(xs[i - 1], 300);
        turtle.write(to_string(i));
    };

    return 0;
}

bool check_win(int x, int y) {
    bool flag = false;
    int vertical = 1;
    int horizontal = 1;
    int forward = 1;
    int backward = 1;
    //see if the player connects 4 vertically
    for (int i = y - 1; i >= 0 && cell[x][i] == turn[0]; i--, vertical++);
    if (vertical >= 4) flag = true;
    //see if the player connects 4 horizontally
    for (int i = x - 1; i >= 0 && cell[i][y] == turn[0]; i--, horizontal++);
    for (int i = x + 1; i < 7 && cell[i][y] == turn[0]; i++, horizontal++);
    if (horizontal >= 4) flag = true;
    //see if the player connects 4 diagonally in a forward slash fashion
    for (int a = x - 1, b = y - 1; a >= 0 && b >= 0 && cell[a][b] == turn[0]; a--, b--, forward++);
    for (int a = x + 1, b = y + 1; a < 7 && b < 6 && cell[a][b] == turn[0]; a++, b++, forward++);
    if (forward >= 4) flag = true;
    //see if the player connects 4 diagonally in a backward slash fashion
    for (int a = x - 1, b = y + 1; a >= 0 && b < 6 && cell[a][b] == turn[0]; a--, b++, backward++);
    for (int a = x + 1, b = y - 1; a < 7 && b >= 0 && cell[a][b] == turn[0]; a++, b--, backward++);
    if (backward >= 4) flag = true;
    return flag;
}

bool check_tie() {
    bool flag = false;
    if (height[0] == 6 && height[1] == 6 && height[2] == 6 && height[3] == 6 && height[4] == 6 && height[5] == 6 && height[6] == 6) flag = true;
    return flag;
}


int mark_cell(int col) {


    try
    {

        scr.update();
        if (turn[0] == "red") {
            turtle.penup();
            turtle.goTo(xs[col - 1], ys[height[col - 1]]);
            turtle.circle(40, 360, { turn[0] });
            turn[0] = "yellow";
            turn[1] = "red";
        }
        else {
            turtle.penup();
            turtle.goTo(xs[col - 1], ys[height[col - 1]]);
            turtle.circle(40, 360, { turn[0] });
            turn[0] = "red";
            turn[1] = "yellow";
        }


    }
    catch (exception e)
    {
        cout << e.what();
    }




    return 0;
}



int tosay(wstring input)
{
    ISpVoice* pVoice = NULL;

    if (FAILED(::CoInitialize(NULL)))
        return FALSE;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    if (SUCCEEDED(hr))
    {
        hr = pVoice->Speak((L"<voice required='Gender = Female;'>" + input).c_str(), 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }

    ::CoUninitialize();
    return 0;
}
string recognizeSpeech()
{
    auto config = SpeechConfig::FromSubscription("your Azure subscrption key here", "your region code here");
    auto recognizer = SpeechRecognizer::FromConfig(config);

    cout << "Say something...\n";
    try {
        auto result = recognizer->RecognizeOnceAsync().get();
        return result->Text;
    }
    catch (exception e) {
        return " ";
    }
}




int main() {

    for (int x = 0; x < 7; x++) {
        for (int y = 0; y < 6; y++) {
            cell[x][y] = "0";
        }
    };

    wstring input, turns;


    scr.bgcolor({ "azure" });
    turtle.width(3);
    turtle.hideturtle();
    turtle.speed(ct::TS_FASTEST);


    board(turtle);
    string inp;
    do {
        cout << "What's your move " << "player " << turn[0] << "?\n";
        wstring turns(turn[0].begin(), turn[0].end());

        tosay(L"Wht's your move, player " + turns);
        inp = recognizeSpeech();
        inp.erase(inp.size() - 1);
        cout << "you just said " << inp << endl;
        wstring input(inp.begin(), inp.end());
        tosay(L"you just said " + input);

        if (inp == "1" | inp == "2" | inp == "3" | inp == "4" | inp == "5" | inp == "6" | inp == "7") {
            stringstream geek(inp);
            int idx;
            geek >> idx;
            if (height[idx - 1] < 6) {
                mark_cell(idx);
                cell[idx - 1][height[idx - 1]] = turn[0];
                height[idx - 1]++;

                if (check_win(idx - 1, height[idx - 1] - 1) == true) {
                    cout << "Congrats, you have won this game, player " << turn[1] << endl;
                    tosay(L"Congrats, you have won this game, player " + turns); break;
                }
                else if (check_tie() == true) {
                    cout << "game over, it's a tie" << endl;
                    tosay(L"Game over, it's a tie");
                    break;
                }



            }
            else {
                cout << "invalid move" << endl;
                tosay(L"invalid move ");
            };

        }
        else
        {
            cout << "please use use a number between 1 and 7 " << endl;
            tosay(L"Please use a number between 1 and 7");
        }
    } while (inp != "Stop listening");




    scr.update();



    scr.exitonclick();
    return 0;
}
