#include <stdio.h>
#include <math.h>
#include <time.h>

int main()
{
	srand((unsigned int)time(NULL));
	int input[225] = { //5
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	double hidden[41] = { 1 };
	double hin[41] = { -1 };
	double output[10] = { 0 };
	double oin[11] = { -1 };
	double itoh[226][40] = { 1 };
	double itoh_d[226][40] = { 0 };
	double htoo[41][10] = { 1 };
	double htoo_d[41][10] = { 0 };
	double target[10] = { 0,0,0,0,0,1,0,0,0,0 };
	double alpha = 0.5;
	double delta_h[41];
	double delta_o[11];

	int oflag = 0;
	int wflag = 0;
    int count = 0;

	//RNG를 이용해 bias 초기화 (-1~1)
	for (int i = 0; i < 226; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			itoh[i][j] = (rand() % 100) / 100.0;
			if (rand() % 2) itoh[i][j] *= -1;
		}
	}

	for (int i = 0; i < 41; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			htoo[i][j] = (rand() % 100) / 100.0;
			if (rand() % 2) htoo[i][j] *= -1;
		}
	}


	while (1)
	{
		//z_in, z 계산
		for (int i = 1; i < 41; i++)
		{
			hin[i] = itoh[0][i];
			for (int j = 1; j < 226; j++)
			{
				hin[i] += input[j - 1] * itoh[j][i];
			}

			//z[i] 계산 완료
			hidden[i] = (1 / (1 + exp(hin[i])));
		}

		//y_in, y 계산
		for (int i = 0; i < 10; i++)
		{
			oin[i] = htoo[0][i];
			for (int j = 1; j < 41; j++)
			{
				oin[i] += htoo[j][i];
			}

			output[i] = (1 / (1 + exp(oin[i])));
		}

		//delta k(output)
		for (int i = 0; i < 10; i++)
		{ 
			delta_o[i] = (target[i] - output[i]) * ((1 / (1 + exp(oin[i]))) * (1 - (1 / (1 + exp(oin[i])))));
			htoo_d[0][i] = alpha * delta_o[i];

			for (int j = 1; j < 41; j++)
			{
				htoo_d[j][i] = alpha * delta_o[i] * hidden[j];
			}
		}

		//delta j(hidden)
		for (int i = 0; i < 41; i++)
		{
			for (int j = 1; j < 11; j++)
			{
				delta_h[i] += delta_o[j] * htoo[i][j];
			}

			delta_h[i] = delta_h[i] * ((1 / (1 + exp(hin[i]))) * (1 - (1 / (1 + exp(hin[i])))));

			itoh_d[0][i] = alpha * delta_h[i];
			for (int j = 1; j < 226; j++)
			{
				itoh_d[j][i] = alpha * delta_h[i] * input[j];
			}
		}

		//update biases
		for (int i = 0; i < 226; i++)
		{
			for (int j = 0; j < 40; j++)
			{
				itoh[i][j] += itoh_d[i][j];
			}
		}

		for (int i = 0; i < 41; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				htoo[i][j] += htoo_d[i][j];
			}
		}

		oflag = 0;		
		for (int i = 0; i < 10; i++)
		{
			if (output[i] == 1)
				oflag++;
		}

		if (oflag == 1) break;

        count++;
        if(count == 1000)
        {
           	for (int i = 0; i < 10; i++)
		    	printf("%lf, ", output[i]);
	    	printf("\n");
            count = 0;
        }
	}


	return 0;
}
