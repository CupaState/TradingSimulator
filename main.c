#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

short int TRADES = 0;

struct Data
{
	char order_cancel;
	short int id;
	char operation;
	short int count;
	double price;
};

struct Cancel
{
	char order;
	short int id;
};

int binary_search(const void* _struct, short int id)
{
	struct Cancel* c = (struct Cancel*)_struct;
	int left = 0;
	int right = 3339;
	int mid = 0;

	while (left <= right)
	{
		mid = ((left + right) >> 1);
		
		if (c[mid].id == id)
		{
			return mid;
		}
		else if (id > c[mid].id)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}
	return -1;
}

int compare_id(const void* _a, const void* _b)
{
	struct Cancel* a = (struct Data*)_a;
	struct Cancel* b = (struct Data*)_b;

	if (a->id >= b->id)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

//������� ���������� ���� � ��������� ��� ����������
int compare(const void* _a, const void* _b)
{
	struct Data* a = (struct Data*)_a;
	struct Data* b = (struct Data*)_b;
	//�� ������� - � ������� ����������� ����
	if (a->operation == 'S')
	{
		if (b->price < a->price)
		{
			return 1;
		}
		else if (b->price > a->price)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else if (a->operation == 'B')	//�� ������� � ������� �������� ����
	{
		if (a->price < b->price)
		{
			return 1;
		}
		else if (a->price > b->price)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
}

int main(short int argc, char** argv)
{
	time_t start = time(NULL);
	FILE* input;
	input = fopen("input.txt", "r");

	if (input == NULL)
	{
		printf("input = NULL\n");
		return 0;
	}

	short int SIZE_ORDER = 0;
	short int CANCEL_ORDER = 0;

	//���������� ������ ������� ��������
	while (feof(input) != 1)
	{
		char c = ' ';
		fscanf(input, "%c%*s\n", &c);

		if (c == 'C')
		{
			CANCEL_ORDER++;
		}
		else
		{
			SIZE_ORDER++;
		}
	}
	//Data storage orders
	struct Data* data = (struct Data*)malloc(sizeof(struct Data) * 20000);
	//Cancel storage cancels
	struct Cancel* cancel = (struct Cancel*)malloc(sizeof(struct Cancel) * 20000);

	//return to the begin file
	fseek(input, 0, SEEK_SET);

	short int i = 0;	//iterator for Data
	short int item_cancel = 0;	//iterator for Cancel

	while (feof(input) != 1)
	{
		char c = ' ';
		fscanf(input, "%c", &c);

		if (c == 'O')
		{
			fscanf(input, "%*c%hd%*c%c%*c%hd%*c%lf\n",
				&data[i].id, &data[i].operation, &data[i].count, &data[i].price);
			data[i].order_cancel = c;
		}
		else
		{
			fscanf(input, "%*c%hd\n", &cancel[item_cancel].id);
			cancel[item_cancel].order = c;
			item_cancel++;
		}
		i++;
	}

	fclose(input);

	qsort(cancel, CANCEL_ORDER, sizeof(struct Cancel), compare_id); //sorting Cancel
	qsort(data, SIZE_ORDER, sizeof(struct Data), compare); //sorting Data

	FILE* output = fopen("out.txt", "w");	//Open file to write

	printf("The program is running\n");	//disclamer

	if (output != NULL)
	{
		for (short int i = 0; i < SIZE_ORDER; i++)
		{
			for (short int j = 1; j < SIZE_ORDER; j++)
			{
				int res = binary_search(cancel, data[i].id);

				//Check for cancels
				if (res != -1)
				{
					if (data[i].id != NULL)
					{
						short int id = data[i].id;
						fprintf(output, "%c,%d\n", 'X', data[i].id);
						data[i].order_cancel = NULL;
						data[i].id = NULL;
						data[id].count = NULL;
						data[id].operation = NULL;
						data[id].price = 0;
					}
					break;
				}

				//Skip canceled orders
				if (data[i].price < 1 || data[j].price < 1 || data[i].id == 0 || data[j].id == 0)
					continue;

				//If we have sale order and buy order
				if (data[i].operation == 'S' && data[j].operation == 'B') 
				{
					if (data[i].price <= data[j].price)
					{
						++TRADES;

						fprintf(output, "%c,%d,%c,%d,%d,%d,%.2lf\n",
							'T', TRADES, data[i].id > data[j].id ? data[i].operation : data[j].operation,
							data[i].id, data[j].id, data[i].count, data[j].price);
					}

						short int count = data[j].count;
						if (data[j].count - data[i].count > 0)
						{
							data[j].count -= data[i].count;
							data[i].count = 0;
						}
						else
						{
							data[j].count = 0;
							data[i].count -= count;
						}

						if (data[j].count <= 0)
						{
							data[j].count = NULL;
							data[j].id = NULL;
							data[j].operation = NULL;
							data[j].order_cancel = NULL;
							data[j].price = 0;
							j = 0;
						}
						if (data[i].count <= 0)
						{
							data[i].count = NULL;
							data[i].id = NULL;
							data[i].operation = NULL;
							data[i].order_cancel = NULL;
							data[i].price = 0;
							i++;
						}

						continue;
				}
				//if we have buy order and sale order
				else if (data[i].operation == 'B' && data[j].operation == 'S')
				{
					if (data[i].price >= data[j].price)
					{
						++TRADES;

						fprintf(output, "%c,%d,%c,%d,%d,%d,%.2lf\n",
							'T', TRADES, data[i].id > data[j].id ? data[i].operation : data[j].operation,
							data[i].id, data[j].id, data[i].count, data[j].price);
					}

					short int count = data[j].count;
					if (data[j].count > data[i].count)
					{
						data[j].count -= data[i].count;
						data[i].count = 0;
					}
					else
					{
						data[j].count = 0;
						data[i].count -= count;
					}

					if (data[j].count <= 0)
					{
						data[j].count = NULL;
						data[j].id = NULL;
						data[j].operation = NULL;
						data[j].order_cancel = NULL;
						data[j].price = 0;
						j = 0;
					}
					if (data[i].count <= 0)
					{
						data[i].count = NULL;
						data[i].id = NULL;
						data[i].operation = NULL;
						data[i].order_cancel = NULL;
						data[i].price = 0;
						i++;
					}
					continue;
				}
			}
		}
	}
	else
	{
		printf("File does not exist\n");
	}

	//Cancel left orders
	for(int i = 0; i < SIZE_ORDER; i++)
	{
		if(data[i].id != NULL)
		{
			fprintf(output, "%c,%hd\n", 'X', data[i].id);
		}
	}

	fclose(output);
	free(data);
	free(cancel);
	time_t end = time(NULL);
	printf("Time %.2f seconds\n", difftime(end, start));
	printf("Program completed.\nPRESS ANY KEY FOR EXIT\n");
	getchar();
	return 0;
}