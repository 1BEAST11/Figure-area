#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) 
{
    MPI_Init(&argc, &argv); // ������������� MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // ��������� ����� �������� ��������
    MPI_Comm_size(MPI_COMM_WORLD, &size); // ��������� ������ ���������� ���������

    mt19937 generator; // �������� ���������� ��������� �����
    uniform_real_distribution<double> distribution(-6, 6); // �������� ������������ ������������� ��������� ����� � �������� ���������

    const int num_samples = 1000000000; // ���������� ��������� ����� ��� ��������� (10^9)

    int points_inside_figure = 0; // ������� �����, ����������� ������ ������ ��� ������ ��������
    int total_points_inside_figure = 0; // ����� ���������� ����� ������ ������

    double startTime = 0.0; // ��������� �����
    double endTime = 0.0; // �������� �����

    if (rank == 0)
    {
        startTime = MPI_Wtime(); // ��������� ������ ������� ������������ �����
    }

    for (int i = rank; i < num_samples; i += size) 
    {
        double x = distribution(generator); // ��������� ��������� ���������� x
        double y = distribution(generator); // ��������� ��������� ���������� y

        double phi = atan2(y, x); // ������ "��"
        double r = sqrt(x * x + y * y); // ������ "��"

        // ������� ���������
        if (r <= 4 + 2 * sin(4*phi)) 
        {
            points_inside_figure++; // ���� ����� ��������� ������ ������, ����������� �������
        }
    }

    // ��������� ���������� ��� ���� ��������� � �������������� ������� MPI_Reduce
    MPI_Reduce(&points_inside_figure, &total_points_inside_figure, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        endTime = MPI_Wtime(); // ��������� ������� ����� ������������ �����
        double parallelExecutionTime = endTime - startTime; // ������ ������ ������� ����������� �����
        double S = (static_cast<double>(total_points_inside_figure) / num_samples) * (4 * 36); // ������ �������
        cout << "S = " << setprecision(13) << S << endl;
        cout << "Time of the parallel part: " << parallelExecutionTime << " seconds" << endl;
    }

    MPI_Finalize(); // ���������� ������ � MPI

    return 0;
}
