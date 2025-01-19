// Gallows_Game.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <sys\locking.h>
#include <string.h>
#include <ctype.h>


// Максимальная длина слова
#define MAX_WORD_LENGTH 21

using namespace std;


// кол-во попыток
int Tries = 10;

// кол-во угаданных слов
int CountWords = 0;

// Загрузка слова
bool LoadWord(FILE* file, char* word) {
	int i = 0;
	char s[MAX_WORD_LENGTH] = { 0 };
	// кол-во слов в файле
	static int count = -1;

	if (count == -1) {
		// подсчет кол-ва слов
		while (!feof(file))
		{
			fgets(s, MAX_WORD_LENGTH, file);
			count++;
		}
		// если нет слов
		if (count == 0) return false;
		// возврат файлового указателя в начало файла
		fseek(file, 0, 0);
	}
	// случайное число
	int n = rand() % count;
	// поиск слова
	while (i <= n)
	{
		fgets(s, MAX_WORD_LENGTH, file);
		i++;
	}
	// определяем длину слова
	int wordlen = strlen(s);
	// минимальная длина слова 2 буквы
	if (wordlen <= 1) return false;
	// убираем Enter (в DOS'е 2 байта 13 10)
	if (s[wordlen - 1] == 10) s[wordlen - 2] = 0;
	else if (s[wordlen - 1] == 13) s[wordlen - 1] = 0;
	// копируем слово
	strcpy_s(word, wordlen, s);
	// получаем дескриптор файла
	int hfile = _fileno(file);
	// вычисляем размер файла
	int size = _filelength(hfile);

	// блокировка файла
	fseek(file, 0, 0);
	_locking(hfile, _LK_NBLCK, size);
	return true;
}

//игра
void Game(char* word) {
	
	int len = strlen(word);

	//перевод в большие буквы
	_strupr_s(word, len + 1);
	//строка копия
	char* copy = new char[len + 1];
	memset(copy, '*', len);
	copy[len] = 0;

	// массив алфавит + пробелы
	char letters[52];
	int i, j = 0;
	for (i = 0; i < 26; i++) {
		letters[j++] = i + 'A';
		letters[j++] = ' ';
	}
	//замыкающий ноль
	letters[51] = 0;

	//буква
	char letter;

	char* pos;
	bool replece = false;

	do {
		//очистка экрана
		system("cls");
		cout << copy << endl << endl;
		cout << letters << endl << endl;
		cout << "Count of tries: " << Tries << endl << endl;
		cout << "Input any letter: \t";
		cin >> letter;

		//звуковой сигнал
		Beep(500, 200);

		//проверка на букву
		if (!isalpha(letter)) {
			cout << "It's not a letter" << endl;
			//задержка на 1 секунду
			Sleep(1000);
			continue;
		}
		//перевод буквы в большую
		letter = toupper(letter);
		//поиск буквы в алфавите
		pos = strchr(letters, letter);

		//такая буква уже была введена?
		if (pos == 0) {
			cout << "This letter have been already pressed" << endl;
			Sleep(1000);
			continue;
		}
		else {
			//убираем букву из алфавита
			pos[0] = ' ';
		}
		// поиск буквы в слове
		for (i = 0; i < len; i++) {
			if (word[i] == letter) {
				copy[i] = letter;
				replece = true;
			}
		}
		if (replece == 0) {
			Tries--;
		}
		else replece = 0;

		//условие победы
		if (strcmp(word, copy) == 0) {
			system("cls");
			cout << copy << endl << endl;
			cout << letters << endl << endl;
			cout << "Count of tries: " << Tries << endl << endl;
			cout << "Congratulation !!!" << endl;
			CountWords++;
			break;
		}
	} while (Tries != 0);
	delete[]copy;
}


void main()
{
	//отрываем файл
	FILE* f;
	fopen_s(&f, "words.txt", "rb");
	//если файл не открылся
	if (f == 0) {
		//ошибка
		perror("Open");
		return;
	}
	srand(time(0));
	char Word[20];
	//пытаемся загрузить слово
	if (!LoadWord(f, Word)) {
		//если не удачно
		cout << "Error !!!" << endl;
		fclose(f);
		return;
	}
	char answer;
	//играем пока не надоест
	do {
		Game(Word);
		//если попыток не осталось то выход из игры
		if (Tries == 0) {
			cout << " Count of words: " << CountWords << endl;
			cout << "Bye-bye" << endl;
			break;
		}
		//если попытки остались
		cout << "Continue ???(Y/N)\t";
		cin >> answer;
		//еще играем?
		if (answer == 'Y' || answer == 'y') {
			Tries = 10;
			if (!LoadWord(f, Word)) {
				cout << "Error !!!" << endl;
				fclose(f);
				return;
			}
		}
	} while (answer == 'Y' || answer == 'y');

	//получаем дескриптор
	int hFile = _fileno(f);

	//разблокировка файла
	int size = _filelength(hFile);
	fseek(f, 0, 0);
	_locking(hFile, _LK_UNLCK, size);
	fclose(f);


}