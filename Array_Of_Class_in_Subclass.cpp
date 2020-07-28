
/*
	24.07.2020, 22:05
	Образовалась хорошая задача: сделать интерьер восприимчивым к
	динамическому изменению ширины полей, пока моя голова от этого
	течёт, как и память.
	Есть идея: векторное поле всё ещё представляется в виде трёх
	скалярных; внутри скалярное поле строится на динамической
	основе. Реализация - выделениие памяти SIZE_X*SIZE_Y*SIZE_Z
	дальше работа с ним как с трёхмерным, используя индексацию:
		arr[i][j][k] == arr[i*SIZE_X + j*SIZE_Y + k] поешь говна
		arr[i][j][k] == arr[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k]


	27.07.2020, 18:58
	Реализовано создание векторного поля с произвольными параметрами.
	Следующая задача - реализация стандартных операций с векторными
	полями, для этого, возможно, потребуется всё же отделить класс
	скалярного поля от векторного и сделать как раньше: векторное - 
	наследник скалярного.

	28.07.2020, 11:16
	Видимо, действительно, происходили утечки памяти по причине того,
	что оператор равенства не был определён. Поэтому указатель просто
	начинал смотреть на другую область памяти и то, на что он перестал
	указывать, порождало утечку. Сейчас я переделал всё на родительский
	класс, работает. Выделяется память для одного обьекта странно: под
	3х-мерное векторное поле почему то выделяется память четыре раза.

*/


#include <iostream>
#include <iomanip>
#include <string>


/*

	Я хочу, чтобы внутри класса можно было доступаться до компонент через [],
	а снаружи можно было использовать (%, %, %);


*/

class ScalarField { 
protected:
	int SIZE_X = 2, SIZE_Y = 2, SIZE_Z = 2;
	float *sField;

public:
	ScalarField() {
		std::cout << "\t    ScalarField()\n";
		sField = new float[SIZE_X * SIZE_Y * SIZE_Z];
		std::cout << "\t\tmem. allocated: " << sField << "\n";
	}

	ScalarField(int X, int Y, int Z) {
		SIZE_X = X;
		SIZE_Y = Y;
		SIZE_Z = Z;
		std::cout << "\t\tScalarField(int X, int Y, int Z)\n";
		sField = new float[SIZE_X * SIZE_Y * SIZE_Z];
		std::cout << "\t\t\tmem. allocated: " << sField << "\n";	
	}

	ScalarField(std::string FieldType) : ScalarField () {
		if (FieldType == "zeroes") {
			for (int i = 0; i < SIZE_X; ++i)
				for(int j = 0; j < SIZE_Y; ++j)
					for (int k = 0; k < SIZE_Z; ++k)
						sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k] = 0;	
	
		} else if (FieldType == "random") {
			for (int i = 0; i < SIZE_X; ++i)
				for(int j = 0; j < SIZE_Y; ++j)
					for (int k = 0; k < SIZE_Z; ++k)
						sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k] = 100 - rand() % 200 + 0.0001 * (rand() % 10000);	
			
		} else if (FieldType == "potential") {
			for (int i = 0; i < SIZE_X; ++i)
				for(int j = 0; j < SIZE_Y; ++j)
					for (int k = 0; k < SIZE_Z; ++k)
						sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k] = 1;
			
		} else if (FieldType == "solenoidal") {}
	}

	ScalarField(const ScalarField &other) {
		SIZE_X = other.SIZE_X;
		SIZE_Y = other.SIZE_Y;
		SIZE_Z = other.SIZE_Z;
		std::cout << "\t\t    ScalarField(const ScalarField &other)\n";
		sField = new float[SIZE_X * SIZE_Y * SIZE_Z];
		std::cout << "\t\t\tmem. allocated: " << sField << "\n";
		for (int i = 0; i < SIZE_X; ++i)
				for(int j = 0; j < SIZE_Y; ++j)
					for (int k = 0; k < SIZE_Z; ++k)
						sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k] = other.sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k];	
	}

	~ScalarField() {
		if (sField != nullptr) {
			std::cout << "\t\tmem. deleted: " << sField << "\n";
			delete [] sField;
			sField = nullptr;
		} 
	}
	
	ScalarField& operator = (const ScalarField &other) {
		std::cout << "\t\t    ScalarField& operator = (const ScalarField &other)\n";		
		if (sField != nullptr) {
			std::cout << "\t\t\tmem. deleted: " << sField << "\n";
			delete [] sField;
			sField = nullptr;
		} 

		SIZE_X = other.SIZE_X;
		SIZE_Y = other.SIZE_Y;
		SIZE_Z = other.SIZE_Z;
		sField = new float[SIZE_X * SIZE_Y * SIZE_Z];
		std::cout << "\t\t\tmem. allocated: " << sField << "\n";
		for (int i = 0; i < SIZE_X; ++i)
				for(int j = 0; j < SIZE_Y; ++j)
					for (int k = 0; k < SIZE_Z; ++k)
						sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k] = other.sField[i*SIZE_Y*SIZE_Z + j*SIZE_Z + k];
		
		return *this;
	}

	float& operator() (int X, int Y, int Z) { return *(sField + X*SIZE_Y*SIZE_Z + Y*SIZE_Z + Z); }
	float operator() (int X, int Y, int Z) const { return sField[X*SIZE_Y*SIZE_Z + Y*SIZE_Z + Z]; }

};


class VectorField : public ScalarField {
private:
	static const int  VectorDim = 3;
	ScalarField Field[VectorDim];

public:
	VectorField() {
		std::cout << "\n\tfrom VectorField()\n";
		srand(time(NULL));

		for (int r = 0; r < VectorDim; ++r) {
			ScalarField r_temp("random");
			Field[r] = r_temp;
		}
		std::cout << "\tend of VectorField()\n";		
	}

	VectorField(int X, int Y, int Z) {
		std::cout << "\tVectorField(int X, int Y, int Z)\n";
		srand(time(NULL));

		SIZE_X = X; SIZE_Y = Y; SIZE_Z = Z;
		for (int r = 0; r < VectorDim; ++r) {
			ScalarField r_temp;
			Field[r] = r_temp;
		}
	}

	~VectorField() {
		std::cout << "\tfrom ~VectorField()\n";
	}

	void PrintField(std::ostream &os) const {
		for (int i = 0; i < SIZE_X; ++i) {
			os << " x:" << i << "\n"; 
			for(int j = 0; j < SIZE_Y; ++j) {
				os << " y:" << j;
				for (int k = 0; k < SIZE_Z; ++k) {
					os << " z:" << k;		
					os << std::setprecision(4) << std::fixed 
					<< " ("; for (int r = 0; r < VectorDim - 1; ++r)
								os <<  std::setw(9) << Field[r](i, j, k) << ", ";
								os <<  std::setw(9) << Field[VectorDim - 1](i, j, k) << ")";
				}
				os << "\n";
			}
			os << "\n";
		}
	}

	ScalarField& operator[](int r) { return Field[r]; }
	ScalarField operator[] (int r) const { return Field[r]; }

};


int main(int argc, char const *argv[])
{
	VectorField E;

	E[0](0,0,0) = 0;
	E.PrintField(std::cout);
	
	return 0;	
}