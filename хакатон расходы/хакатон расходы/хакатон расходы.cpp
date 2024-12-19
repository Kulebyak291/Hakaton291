#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <limits>
#include <cctype>
#include <locale>
#define NOMINMAX
#include <Windows.h>
#include <sstream>
using namespace std;

void clearConsole() {
    int result = system("cls");
    if (result != 0) {
        cout << "Не удалось очистить консоль. Код ошибки: " << result << endl;
    }
}
struct Expense {
    int sum;
    string category;
    string descrip;
    time_t date;
};
void addExp(vector<Expense>& expenses) {
    Expense expense;
    while (true) {
        cout << "Введите сумму расхода (положительное число): ";
        cin >> expense.sum;
        if (cin.fail() || expense.sum < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Неверный ввод. Пожалуйста, введите положительное число." << endl;
            return;
        }
        cout << "Введите категорию расхода: ";
        cin.ignore();
        getline(cin, expense.category);

        cout << "Введите описание расхода: ";
        getline(cin, expense.descrip);

        expense.date = time(nullptr);
        expenses.push_back(expense);
        break;
    }
}
void viewExp(vector<Expense>& expenses) {
    if (expenses.empty()) {
        cout << "Нет расходов для отображения." << endl << endl;
        return;
    }
    int sortChoice;
    cout << "Выберите способ сортировки:\n" << "1. По дате\n" << "2. По сумме\n" << "Выберите опцию: ";
    cin >> sortChoice;
    if (sortChoice == 1) {
        sort(expenses.begin(), expenses.end(), [](Expense& a, Expense& b) {
            return a.date < b.date;
            });
    }
    else if (sortChoice == 2) {
        sort(expenses.begin(), expenses.end(), [](Expense& a, Expense& b) {
            return a.sum < b.sum;
            });
    }
    else {
        cout << "Некорректный выбор критерия сортировки." << endl << endl;
        return;
    }
    char buffer[26];
    for (auto& expense : expenses) {
        ctime_s(buffer, sizeof(buffer), &expense.date);
        cout << "\nДата: " << buffer << "Сумма: " << expense.sum << ", Категория: " << expense.category << ", Описание: " << expense.descrip << endl;
    }
}
int totalSum(vector<Expense>& expenses) {
    int total = 0;
    for (auto& expense : expenses) {
        total += expense.sum;
    }
    return total;
}
string toLow(const string& str) {
    string lowStr;
    locale loc("");
    for (char c : str) {
        lowStr += tolower(c, loc);
    }
    return lowStr;
}
void search(vector<Expense>& expenses, string& category) {
    string lowCateg = toLow(category);
    bool found = false;
    for (const auto& expense : expenses) {
        string tmp = toLow(expense.category);
        if (tmp == lowCateg) {
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &expense.date);
            cout << "Дата: " << buffer << "Сумма: " << expense.sum << ", Описание: " << expense.descrip << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "Расходы по категории " << category << " не найдены." << endl;
    }
}
void save(vector<Expense>& expenses, string& filename) {
    ofstream file(filename);
    for (auto& expense : expenses) {
        file << expense.sum << " " << expense.date << " " << expense.category << ";" << expense.descrip << endl;
    }
}
void load(vector<Expense>& expenses, string& filename) {
    ifstream file(filename);
    if (file) {
        expenses.clear();
        string line;
        while (getline(file, line)) { 
            Expense expense;
            istringstream iss(line); 
            if (!(iss >> expense.sum >> expense.date)) {
                cout << "Ошибка загрузки строки: " << line << endl;
                continue; 
            }
            string categAndDescr;
            getline(iss, categAndDescr); 
            size_t separPos = categAndDescr.find(';');
            if (separPos != string::npos) {
                expense.category = categAndDescr.substr(0, separPos);
                expense.descrip = categAndDescr.substr(separPos + 1);
            }
            else {
                expense.category = categAndDescr; 
                expense.descrip = ""; 
            }

            expenses.push_back(expense);
        }
    }
    else {
        cout << "Не удалось открыть файл: " << filename << endl;
    }
}
void delExp(vector<Expense>& expenses) {
    if (expenses.empty()) {
        cout << "Список расходов пуст. Удаление невозможно." << endl;
        return;
    }
    cout << "Список расходов:" << endl;
    for (size_t i = 0; i < expenses.size(); ++i) {
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &expenses[i].date);
        cout << i + 1 << ". " << expenses[i].category << " (Сумма: " << expenses[i].sum << ", Описание: " << expenses[i].descrip << ", Дата: " << buffer << ")" << endl;
    }
    size_t index;
    cout << "Введите номер расхода, который необходимо удалить: ";
    cin >> index;
    if (cin.fail() || index < 1 || index > expenses.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Неверный ввод. Удаление отменено." << endl;
        return;
    }
    expenses.erase(expenses.begin() + (index - 1));
    cout << "Расход успешно удалён!" << endl;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string filename = "expenses.txt";
    vector<Expense> expenses;
    load(expenses, filename);
    int choice;
    clearConsole();
    do {
        cout << "\nМеню(Ввод на английском!):\n" << "1. Добавить расход\n" << "2. Просмотреть все расходы\n" << "3. Подсчитать общую сумму расходов\n" << "4. Поиск по категории расхода\n" << "5. Удалить расход\n" << "6. Выход\n" << "Выберите опцию: ";
        cin >> choice;
        clearConsole();
        switch (choice) {
        case 1:
            addExp(expenses);
            break;
        case 2:
            viewExp(expenses);
            break;
        case 3: {
            int total = totalSum(expenses);
            cout << "Общая сумма расходов: " << total << endl;
            break;
        }
        case 4: {
            string category;
            cout << "Введите категорию для поиска: ";
            cin.ignore();
            getline(cin, category);
            search(expenses, category);
            break;
        }
        case 5:
            delExp(expenses);
            break;
        case 6:
            save(expenses, filename);
            cout << "Данные сохранены. Выход из программы." << endl;
            break;
        default:
            cout << "Некорректный выбор, введите цифру от 1 до 6. Попробуйте снова." << endl;
            break;
        }
    } while (choice != 6);
}