#include <iostream>
#include <Windows.h>
using namespace std;

const int height_field = 5;			//	константа высоты;
const int width_field = 7;			//	константа ширины;
const int value_delete = 1;			//	константа для заполнения массива (съеденных фишек);

class Game
{
private:
	int** field_mass;								//	переменная массива;
	int player_turn;								//	переменная хранящая значения хода (1 - ходит первый игрок, 2 - ходит второй игрок);
	int getting_first_value, getting_second_value;	//	переменные для хранения первого и второго значения;
	int flag_winner = 0;							//	флаг выигрыша - по стандарту равно 0 (никто не выиграл);
	bool flag = true;								//	флаг для завершения игры (false - завершение);
	int heap_number[5];								//	(ИИ) переменная для хранения свободных фишек в "куче" - индекс соответвует номеру кучи. [1] = первая "куча";
	int heap_number_copy[5];						//	(ИИ) переменная, в которой храняться копированное значение своб.фишек в "куче;
	int heap_one_binary[3], heap_two_binary[3], heap_three_binary[3], heap_four_binary[3];	//	(ИИ) переменные хранящие в двоичном коде число своб.фишек в "кучи";
	int sum_number_position;												//	(ИИ) переменная суммы всех разрядов;
	int Get_Data_From_User(int flag_from_user);								//	функция обработки данных от пользователя;
	int Delete_Chip(int value_height, int value_width, int flag_winner);	//	функция удаления фишки;
	int AI_Choiсe_Chips(int n);									//	ИИ - всп.функция для выбора фишки (n - переменная для работы цикла выбора хода);
	int AI_Binary_Number(int heap_binary[], int heap_number);	//	ИИ - всп.функция для перевода числа в двоичную систему;
	int AI_Number_Position();									//	ИИ - всп.функция для сложения разрядов "куч";
	int AI_Alignment_Heap(int n);								//	ИИ - всп.функция для выравнивание "кучи" (n - передается из функции AI1_Choiсe_Chips);
	int AI_Copy_Variable(int value_source[], int value_copy[]);	//	ИИ - всп.функция для копирования значений; 
public:
	Game();							//	конструктор для создания массива;
	~Game();						//	деструктор для удаления массива;
	int Start_Position();			//	выбор уровня сложности и первого хода;
	int Print_Mass();				//	печать массива;
	int Сheck_Сhips_Winner();		//	функция проверки выигрыша;
	int Human_Move(int flag_human);	//	выбор фишки игрока;
	int AI_Move();					//	ИИ выбора хода - главная функция;
	void Run_Menu();				//	меню;
};

Game::Game()						//	конструктор для создания массива;
{
	field_mass = new int* [height_field];
	for (int i = 0; i < height_field; i++)
	{
		field_mass[i] = new int[width_field];
		for (int j = 0; j < width_field; j++)
		{
			field_mass[i][j] = 0;
		}
	}
}

Game::~Game()
{
	field_mass = NULL;
	delete[] field_mass;
}

int Game::Start_Position()	//	функция выбора первого хода
{
	printf("------------Меню------------\n");
	printf("Выберете, чей будет первый ход\n(введите соответствующие число):\n1 - 1 игрок; \n2 - 2 игрок (ИИ).\n");
	Get_Data_From_User(1);
	player_turn = getting_first_value;	//	присваиваем переменной хода полученное значение;
	system("cls");

	return player_turn;
}

int Game::Get_Data_From_User(int flag_from_user)
{
	//	flag 1 - флаг передачи значения от пользователя первого хода, 
	//	flag 2 - флаг передачи значения от пользователя о переменной хода;

	char str_from_human[10];				//	переменная для хранения строки длинной 10 символа;
	int size_str = 0;						//	переменная для хранения длины строки (необходима для проверки);

	while (true)
	{
		cin.getline(str_from_human, 10);			//	получаем исходную укороченную строку (укороченную до 10 символов);
		size_str = strlen(str_from_human);					//	получение длины введенной строки;
		getting_first_value = str_from_human[0] - '0';		//	получаем первое значение из строки;
		getting_second_value = str_from_human[1] - '0';		//	получаем второе значение из строки;

		if (flag_from_user == 1 && size_str == 1 && getting_first_value >= 1 && getting_first_value <= 2)
		{
			return getting_first_value;
		}
		else if (flag_from_user == 2 && size_str == 2 && getting_first_value >= 0 && getting_first_value <= 4 && getting_second_value >= 0 && getting_second_value <= 6)
		{
			return getting_first_value, getting_second_value;
		}
		else
		{
			printf("Значение не корректно! Введите значение заново:\t");
		}
	}
	return 0;
}

int Game::Print_Mass()				//	печать массива;
{
	for (int e = -1; e < width_field; e++)		//	печать первой строки (отдельной от массива)
	{
		e == -1 ? printf("  ") : printf("%d ", e);
	}
	printf("\n---------------\n");				//	печатаем разделяющую строчку (границы массива);

	for (int i = 0; i < height_field; i++)		//	выводим на экран массив
	{
		printf("%d|", i);						//	печать номера столбца 
		for (int j = 0; j < width_field; j++)
		{
			if (field_mass[i][j] > 0)			//	если значение массива заполнено - печатаем "Х";
			{
				printf("X ");
			}
			else								//	в остальных случаях (хранит 0) - печатаем 0;
			{
				printf("0 ");
			}
		}
		printf("\n");
	}
	return **field_mass;
}

int Game::Delete_Chip(int value_height, int value_width, int flag_winner)	//	функция удаления фишек (flag 0 - никто еще не выиграл, 2 - выиграл ИИ, 3 - выиграл И1);
{
	field_mass[value_height][value_width] = value_delete;		//	удаление указанной фишки;
	for (int i = value_height; i >= 0; i--)						//	удаление области матрицы (выше и правее);
	{
		for (int j = value_width; j < width_field; j++)
		{
			field_mass[i][j] = value_delete;
			field_mass[4][0] = flag_winner;
		}
	}
	return **field_mass;
}

int Game::Human_Move(int flag_print)			//	флаг нужен для первичной (1) и вторичного (2) вызова функции;
{
	if (flag_print == 1)
	{
		(getting_first_value > 0 && getting_second_value > 0) ? printf("\nКомпьютер сделал ход: %d%d\n", getting_first_value, getting_second_value) : printf("\n");
		printf("Введите координаты фишки для удаления, указав одним числом координаты высоты и координаты ширины выбранной фишки.\n");
		printf("Для подтверждения нажмите 'enter'(ввод):\t");
	}
	else
	{
		printf("Фишка заната! Выберите координаты другой фишки:\t");
	}
	Get_Data_From_User(2);

	if (field_mass[getting_first_value][getting_second_value] == 1)		//	если указанная фишка занята - вызываем снова функцию, но с другим текстом (флагом);
	{
		Human_Move(2);
	}
	else if (getting_first_value == 4 && getting_second_value == 0)		//	если "съедаем" последнюю фишку - проигрываем;
	{
		flag_winner = 2;
	}
	else
	{
		flag_winner = 0;
	}
	Delete_Chip(getting_first_value, getting_second_value, flag_winner);
	return flag_winner;
}

int Game::AI_Move()					//	ИИ выбора хода;
{
	if (field_mass[3][0] == 1 && field_mass[4][1] == 1)		//	если осталась только отравленная фишка - "съедаем" ее и останавливаем функцию;
	{
		getting_first_value = 4, getting_second_value = 0, flag_winner = 3;
	}
	else if (field_mass[2][0] == 1 && field_mass[3][1] == 1 && field_mass[4][2] == 1 && field_mass[3][0] == 0)	//	"частный случай" - если осталось только две неотравленные фишки;
	{
		getting_first_value = 3, getting_second_value = 0, flag_winner = 0;
	}
	else
	{
		for (int i = 1; i < height_field; i++)	//	циклом считаем кол-во свободных фишек в "куче";
		{
			heap_number[i] = 0;					//	зануляем переменную (для того, чтобы пред.ходы не влияли);
			for (int j = 0; j < width_field; j++)
			{
				if (field_mass[i][j] == 0)		//	если встерчается своб.фишка - прибавляем ее к сумме в "куче";
				{
					heap_number[i]++;
				}
			}
		}
		heap_number[4]--;		//	вычитаем единицу из чертвертой кучи, потому что отравленную фишку ИИ не должен "съедать";

		AI_Copy_Variable(heap_number, heap_number_copy);	//	копируем значение своб.фишек в "куче" в переменную для работы с ней;
		AI_Choiсe_Chips(1);												//	вызываем фукнцию для выбора фишки ИИ;	
	}

	Delete_Chip(getting_first_value, getting_second_value, flag_winner);
	return getting_first_value, getting_second_value, flag_winner;	//	возвращаем значения, для удаления в функцию AI;
}

int Game::AI_Copy_Variable(int value_source[], int value_copy[])
{
	//	value_source - переменная из которой будем копировать, value_copy - переменная в которую будем копировать;
	for (int i = 1; i <= 4; i++)
	{
		value_copy[i] = value_source[i];
	}
	return value_copy[5], value_source[5];
}

int Game::AI_Choiсe_Chips(int n)
{
	do
	{
		if (heap_number_copy[n] > 0)
		{
			heap_number_copy[n]--;						//	вычитаем одну фишку (для хода);
			AI_Alignment_Heap(n);						//	вызываем фукнцию для выравнивания "кучи";
			AI_Number_Position();						//	вызов функции подсчета всей суммы "куч";

			if (sum_number_position == 0)				//	если сумма равна 0 - останавливаем и возвращаем значения для удаления;
			{
				if (n == 4) { heap_number_copy[n]++; }	//	если рассматриваем "кучу" #4 - прибавляем единицу;

				getting_first_value = n;				//	присваиваем "высоту" равную номеру "кучи";
				getting_second_value = heap_number_copy[n];	//	присваиваем "ширине" число, которые равно кол-во своб.фишек;
				flag_winner = 0;
				break;
			}
			else
			{
				AI_Choiсe_Chips(n);						// рекурсивно вызываем цикл для того, чтобы пересчитать еще раз значения по другому;
				break;
			}
		}
		else
		{
			AI_Copy_Variable(heap_number, heap_number_copy);	//	вызываем функцию, которая возвращает значение своб.фишек в "куче";
			n++;												//	переходим к след."кучи";
			sum_number_position = 1;							//	для срабатывания следующего цикла;
		}
	} while (sum_number_position != 0);

	return getting_first_value, getting_second_value, flag_winner;	//	возвращаем значения, для удаления в функцию AI;
}

int Game::AI_Alignment_Heap(int n)
{
	heap_number_copy[4]++;								//	прибавляем, потому что "отравленная" фишка не подсчитана;
	if (heap_number_copy[n] < heap_number_copy[n - 1])	//	если рассматриваемая переменная меньше, чем пред.переменная;
	{
		heap_number_copy[n - 1] = heap_number_copy[n];	//	пред.переменной присваиваем значение рассматриваемой переменной;
		if (n == 0)										//	как только переменная закончится - возвращаем значение;
		{
			return heap_number_copy[5];
		}
		else
		{
			AI_Alignment_Heap(n - 1);					//	вызываем рекурсивно функцию, но уже рассматриваем пред.переменную;
		}
	}
	heap_number_copy[4]--;								//	вычитаем, потому что "отравленная" фишка не должна быть "съедена";
	return heap_number_copy[5];
}

int Game::AI_Number_Position()		//	(ИИ) всп.функция для сложения разрядов "куч";
{
	AI_Binary_Number(heap_one_binary, heap_number_copy[1]);		//	вызов функции для перевода числа (кол-во своб.фишек в опредленной "куче") в двоичную систему;
	AI_Binary_Number(heap_two_binary, heap_number_copy[2]);
	AI_Binary_Number(heap_three_binary, heap_number_copy[3]);
	AI_Binary_Number(heap_four_binary, heap_number_copy[4]);

	int SNP[3] = { 0, 0, 0 };		//	переменные сумм отдельных куч: [0] - sum_one_number_position, [1] - sum_two_number_position, [2] - sum_three_number_position;
	for (int i = 0; i < 3; i++)
	{
		SNP[i] = (heap_one_binary[i] + heap_two_binary[i]) - (heap_three_binary[i] + heap_four_binary[i]);		//	поразрядный подсчет
		SNP[i] = abs(SNP[i]);						//	число по модулю (потому что может встретиться '-1';
	}

	sum_number_position = SNP[0] + SNP[1] - SNP[2];			//	подсчет всей суммы разрядов;
	sum_number_position = abs(sum_number_position);			//	берем число по модулю;
	sum_number_position == 1 ? sum_number_position = 1 : sum_number_position = 0;		//	else 2 or 0;

	return sum_number_position;
}

int Game::AI_Binary_Number(int heap_binary[], int heap_number)	//	(ИИ) вспом.функция для перевода числа в двоичную систему;
{
	int swap = heap_number;
	for (int i = 2; i >= 0; i--)			//	считаем разряды начиная с конца;
	{
		if (swap != 0)
		{
			heap_binary[i] = swap % 2;
			swap /= 2;
		}
		else
		{
			heap_binary[i] = 0;
		}
	}

	return heap_binary[3];
}

int Game::Сheck_Сhips_Winner()
{
	if (field_mass[4][0] == 2)
	{
		printf("\nВы проиграли! Игра завершена!\n");
		return flag = false;
	}
	else if (field_mass[4][0] == 3)
	{
		printf("\nВы выиграли! Игра завершена!\n");
		return flag = false;
	}
	else
	{
		return flag = true;
	}
}

void Game::Run_Menu()
{
	Start_Position();
	Print_Mass();

	while (flag == true)
	{
		if (player_turn == 1)
		{
			Human_Move(1);
			player_turn++;
		}
		else
		{
			AI_Move();
			player_turn--;
		}

		system("cls");
		Print_Mass();
		Сheck_Сhips_Winner();
	}
}

int main()
{
	system("chcp 1251");
	system("cls");

	Game exGame;
	exGame.Run_Menu();
	exGame.~Game();
}