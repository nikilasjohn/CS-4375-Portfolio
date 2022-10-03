#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
using namespace std;

// function to copy the execution of this R command:
// nrow(df[df$pclass==pc & df$survived==sv,])
int likelyClass(double data[][4], int sv, int pc) {
	int count = 0;
	for (int i = 0; i < 1046; i++) {
		if (data[i][1] == pc && data[i][3] == sv) {
			count++;
		}
	}
	return count;
}

// function to copy the execution of this R command:
// nrow(df[as.integer(df$sex)==sx & df$survived==sv,])
int likelySex(double data[][4], int sv, int sx) {
	int count = 0;
	for (int i = 0; i < 1046; i++) {
		if (data[i][2] == sx && data[i][3] == sv) {
			count++;
		}
	}
	return count;
}

// Finds the mean
double findMean(double data[][4], int sv) {
	double sum = 0;
	int count = 0;
	for (int i = 0; i < 1046; i++) {
		if (data[i][3] == sv) {
			sum += data[i][0];
			count++;
		}
	}
	double mean = sum / count;

	return mean;
}

// Finds the square root of the variance of a vector
double findVar(double data[][4], int sv, double mean) {
    double sumSq = 0;
	int count = 0;
    for (int i = 0; i < 1046; i++) {
		if (data[i][3] == sv) {
			sumSq += (data[i][0] - mean) * (data[i][0] - mean);
			count++;
		}
    }
    double sqrtSumSq = sumSq / count;
    return sqrtSumSq;
}

// Probability density function
double calc_age_lh(double v, double mean_v, double var_v) {
	return 1 / (sqrt(2 * M_PI * var_v) * (exp(-((v - mean_v) * (v - mean_v)))/(2 * var_v)));
}

// Function for scratch model
double* calc_raw_prob(int pclass, int sex, double age, double lh_pclass[][3], double lh_sex[][2], double apriori[2], double age_mean[2], double age_var[2]) {
	double *list;
	double num_s = lh_pclass[1][pclass] * lh_sex[1][sex] * apriori[1] * calc_age_lh(age, age_mean[1], age_var[1]);
	double num_p = lh_pclass[0][pclass] * lh_sex[0][sex] * apriori[0] * calc_age_lh(age, age_mean[0], age_var[0]);
	double denominator = num_s + num_p;
	list[0] = num_s / denominator;
	list[1] = num_p / denominator;
	cout << list[0] << endl;
	cout << list[1] << endl;
	return list;
}

int main(int argc, char** argv)
{
	ifstream inFS;
	string line;
	string numObs_in, pClass_in, survived_in, sex_in, age_in;
	vector<int> numObs;
	vector<int> pClass;
    vector<int> survived;
    vector<int> sex;
    vector<double> age;

	cout << "Opening file titanic_project.csv\n" << endl;
	inFS.open("titanic_project.csv");
	if (!inFS.is_open()) {
		cout << "Could not open file titanic_project.csv" << endl;
		return 1;
	}
    string header;
    getline(inFS, header);
    while (inFS.good()) {
		getline(inFS, numObs_in, ',');
        getline(inFS, pClass_in, ',');
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, ',');
		getline(inFS, age_in, '\n');

		numObs.push_back(stoi(numObs_in));
		pClass.push_back(stoi(pClass_in));
        survived.push_back(stoi(survived_in));
        sex.push_back(stoi(sex_in));
        age.push_back(stof(age_in));
	}

	// Create df variable
	double df[1046][4];
	for (int i = 0; i < 1046; i++) {
		for (int j = 0; j < 4; j++) {
			if (j == 0) {
				df[i][j] = age.at(i);
			}
			else if (j == 1) {
				df[i][j] = pClass.at(i);
			}
			else if (j == 2) {
				df[i][j] = sex.at(i);
			}
			else {
				df[i][j] = survived.at(i);
			}
		}
	}

	// Starts timer 
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	// Split data into train and test
	double data_matrix[800][4];

	for (int i = 0; i < 800; i++) {
		for (int j = 0; j < 4; j++) {
			if (j == 0) {
				data_matrix[i][j] = age.at(i);
			}
			else if (j == 1) {
				data_matrix[i][j] = pClass.at(i);
			}
			else if (j == 2) {
				data_matrix[i][j] = sex.at(i);
			}
			else {
				data_matrix[i][j] = survived.at(i);
			}
		}
	}
	
	double test_matrix[200][3];

	int thousand = 1045;
	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 4; j++) {
			if (j == 0) {
				test_matrix[i][j] = age.at(thousand);
			}
			else if (j == 1) {
				test_matrix[i][j] = pClass.at(thousand);
			}
			else if (j == 2) {
				test_matrix[i][j] = sex.at(thousand);
			}
			else {
				data_matrix[i][j] = survived.at(thousand);
			}
		}
		thousand--;
	}

	// Calculate priors
	double apriori[2] = {};
	int numDead = 0;
	int numAlive = 0;

	for (int i = 0; i < survived.size(); i++) {
		if (survived.at(i) == 0) {
			numDead++;
		}
		else {
			numAlive++;
		}
	}

	apriori[0] = numDead / (double) survived.size();
	apriori[1] = numAlive / (double) survived.size();

	cout << "Aprioris: " << endl;
	cout << apriori[0] << " " << apriori[1] << endl;
	cout << endl;

	// Calculate likelihoods for pclass
	double lh_pclass[2][3] = {};

	cout << "Likelihood values for p(pclass|survived):" << endl;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == 0) {
				lh_pclass[i][j] = likelyClass(df, i, j + 1) / (double) numDead;
			}
			else {
				lh_pclass[i][j] = likelyClass(df, i, j + 1) / (double) numAlive;
			}
			cout << lh_pclass[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	// Calculate likelihoods for sex
	double lh_sex[2][2];

	cout << "Likelihood values for p(sex|survived):" << endl;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (i == 0) {
				lh_sex[i][j] = likelySex(df, i, j) / (double) numDead;
			}
			else {
				lh_sex[i][j] = likelySex(df, i, j) / (double) numAlive;
			}
			cout << lh_sex[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	// Calculate likelihoods for quantitative data
	double age_mean[2] = {};
	double age_var[2] = {};

	cout << "Likelihood values for p(sex|age):" << endl;
	cout << "\tMeans:" << endl << "\t";
	for (int i = 0; i < 2; i++) {
		age_mean[i] = findMean(df, i);
		age_var[i] = findVar(df, i, age_mean[i]);
		cout << age_mean[i] << " ";
	}
	cout << endl;

	cout << "\tVariance:" << endl << "\t";
	for (int i = 0; i < 2; i++) {
		cout << age_var[i] << " ";
	}
	cout << endl << endl;

	double *result;
	for (int i = 0; i < 1; i++) {
		result = calc_raw_prob(test_matrix[i][1], test_matrix[i][2], test_matrix[i][0], lh_pclass, lh_sex, apriori, age_mean, age_var);
	}

	// Ends the timer
	chrono::steady_clock::time_point end = chrono::steady_clock::now(); 

	cout << "Time taken (microseconds): " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;

    return 0;
} 