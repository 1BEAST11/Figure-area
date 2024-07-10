#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) 
{
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получение ранга текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получение общего количества процессов

    mt19937 generator; // Создание генератора случайных чисел
    uniform_real_distribution<double> distribution(-6, 6); // Создание равномерного распределения случайных чисел в заданном диапазоне

    const int num_samples = 1000000000; // Количество случайных точек для генерации (10^9)

    int points_inside_figure = 0; // Счетчик точек, находящихся внутри фигуры для одного процесса
    int total_points_inside_figure = 0; // Общее количество точек внутри фигуры

    double startTime = 0.0; // начальное время
    double endTime = 0.0; // конечное время

    if (rank == 0)
    {
        startTime = MPI_Wtime(); // измерение начала времени параллельной части
    }

    for (int i = rank; i < num_samples; i += size) 
    {
        double x = distribution(generator); // Генерация случайной координаты x
        double y = distribution(generator); // Генерация случайной координаты y

        double phi = atan2(y, x); // расчет "фи"
        double r = sqrt(x * x + y * y); // расчет "ро"

        // условие попадания
        if (r <= 4 + 2 * sin(4*phi)) 
        {
            points_inside_figure++; // Если точка находится внутри фигуры, увеличиваем счетчик
        }
    }

    // Суммируем результаты для всех процессов с использованием функции MPI_Reduce
    MPI_Reduce(&points_inside_figure, &total_points_inside_figure, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        endTime = MPI_Wtime(); // измерение времени конца параллельной части
        double parallelExecutionTime = endTime - startTime; // расчет общего времени паралельной части
        double S = (static_cast<double>(total_points_inside_figure) / num_samples) * (4 * 36); // расчет площади
        cout << "S = " << setprecision(13) << S << endl;
        cout << "Time of the parallel part: " << parallelExecutionTime << " seconds" << endl;
    }

    MPI_Finalize(); // Завершение работы с MPI

    return 0;
}
